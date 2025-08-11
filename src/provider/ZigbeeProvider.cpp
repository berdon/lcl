/**
 * Tags: provider, zigbee, tcp, usb
 */
#include <provider/ZigbeeProvider.hpp>
#include <provider/IDelegate.hpp>
#include <asio.hpp>
#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <regex>
#include <array>
#include <thread>

namespace lcl::provider {

  ZigbeeProvider::ZigbeeProvider(std::string_view connection_string)
      : io_context_(), socket_(io_context_), serial_port_(io_context_) {
    parseAndConnect(connection_string);
    startAsyncRead();
  }

  ZigbeeProvider::~ZigbeeProvider() {
    io_context_.stop();
    if (socket_.is_open()) {
      socket_.close();
    }
    if (serial_port_.is_open()) {
      serial_port_.close();
    }
    if (read_thread_.joinable()) {
      read_thread_.join();
    }
  }

  std::vector<Device> ZigbeeProvider::GetDevices() const {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    return devices_;
  }

  std::optional<Device> ZigbeeProvider::GetDevice(std::string_view id) const {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    auto it = std::find_if(devices_.begin(), devices_.end(),
        [id](const Device& d) { return d.id == id; });
    if (it != devices_.end()) {
      return *it;
    }
    return std::nullopt;
  }

  void ZigbeeProvider::SetDelegate(std::shared_ptr<IDelegate> delegate) {
    delegate_ = std::move(delegate);
    if (delegate_) {
      std::lock_guard<std::mutex> lock(devices_mutex_);
      for (const auto& device : devices_) {
        delegate_->OnNewDevice(device);
      }
    }
  }

  void ZigbeeProvider::parseAndConnect(std::string_view connection_string) {
    std::regex tcp_regex(R"(tcp://([^:]+):(\d+))");
    std::regex usb_regex(R"(usb://(/dev/.*))");
    std::cmatch match;

    std::string conn_str(connection_string);

    if (std::regex_match(conn_str.c_str(), match, tcp_regex)) {
      connection_type_ = ConnectionType::TCP;
      std::string host = match[1].str();
      uint16_t port = std::stoi(match[2].str());
      connectTcp(host, port);
    } else if (std::regex_match(conn_str.c_str(), match, usb_regex)) {
      connection_type_ = ConnectionType::USB;
      std::string device_path = match[1].str();
      connectUsb(device_path);
    } else {
      throw std::invalid_argument("Invalid connection string format");
    }
  }

  void ZigbeeProvider::connectTcp(std::string_view host, uint16_t port) {
    try {
      asio::ip::tcp::resolver resolver(io_context_);
      auto endpoints = resolver.resolve(host, std::to_string(port));
      asio::connect(socket_, endpoints);
      socket_.set_option(asio::ip::tcp::no_delay(true));
    } catch (const std::exception& e) {
      throw std::runtime_error("TCP connection failed: " + std::string(e.what()));
    }
  }

  void ZigbeeProvider::connectUsb(std::string_view device_path) {
    try {
      serial_port_.open(std::string(device_path));
      serial_port_.set_option(asio::serial_port_base::baud_rate(115200));
      serial_port_.set_option(asio::serial_port_base::character_size(8));
      serial_port_.set_option(asio::serial_port_base::parity(
          asio::serial_port_base::parity::none));
      serial_port_.set_option(asio::serial_port_base::stop_bits(
          asio::serial_port_base::stop_bits::one));
      serial_port_.set_option(asio::serial_port_base::flow_control(
          asio::serial_port_base::flow_control::none));
    } catch (const std::exception& e) {
      throw std::runtime_error("USB connection failed: " + std::string(e.what()));
    }
  }

  void ZigbeeProvider::startAsyncRead() {
    read_thread_ = std::thread([this]() {
      try {
        while (!io_context_.stopped()) {
          io_context_.restart();
          asyncRead();
          io_context_.run();
        }
      } catch (const std::exception& e) {
        if (delegate_) {
          delegate_->OnDeviceChanged(Device{"", "Error", false});
        }
      }
    });
  }

  void ZigbeeProvider::asyncRead() {
    auto buffer = std::make_shared<std::array<char, 256>>();
    if (connection_type_ == ConnectionType::TCP) {
      socket_.async_read_some(
          asio::buffer(*buffer),
          [this, buffer](const asio::error_code& ec, std::size_t bytes_transferred) {
            if (!ec) {
              handleRead(buffer->data(), bytes_transferred);
              asyncRead();
            } else if (ec != asio::error::operation_aborted) {
              if (delegate_) {
                delegate_->OnDeviceChanged(Device{"", "Disconnected", false});
              }
            }
          });
    } else {
      serial_port_.async_read_some(
          asio::buffer(*buffer),
          [this, buffer](const asio::error_code& ec, std::size_t bytes_transferred) {
            if (!ec) {
              handleRead(buffer->data(), bytes_transferred);
              asyncRead();
            } else if (ec != asio::error::operation_aborted) {
              if (delegate_) {
                delegate_->OnDeviceChanged(Device{"", "Disconnected", false});
              }
            }
          });
    }
  }

  void ZigbeeProvider::handleRead(const char* data, std::size_t length) {
    // Simple Zigbee frame parser (simplified for example)
    std::string received(data, length);
    if (received.find("ZIGBEE_DEVICE") != std::string::npos) {
      std::string device_id = "zigbee_" + std::to_string(devices_.size() + 1);
      Device new_device{device_id, "Zigbee Device " + device_id, true};

      std::lock_guard<std::mutex> lock(devices_mutex_);
      auto it = std::find_if(devices_.begin(), devices_.end(),
          [&device_id](const Device& d) { return d.id == device_id; });

      if (it == devices_.end()) {
        devices_.push_back(new_device);
        if (delegate_) {
          delegate_->OnNewDevice(new_device);
        }
      } else if (it->isOnline != new_device.isOnline) {
        *it = new_device;
        if (delegate_) {
          delegate_->OnDeviceChanged(new_device);
        }
      }
    }
  }

} // namespace lcl::provider