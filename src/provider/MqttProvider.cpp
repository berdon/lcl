/**
 * Tags: provider, mqtt
 */
#include <provider/MqttProvider.hpp>
#include <provider/IDelegate.hpp>
#include <regex>
#include <random>
#include <chrono>

namespace lcl::provider {
  MqttProvider::MqttProvider(std::string_view broker_address)
      : client_id_("lcl_mqtt_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count())),
        callback_(*this) {
    connect(broker_address);
  }

  MqttProvider::~MqttProvider() {
    if (client_) {
      try {
        client_->disconnect()->wait();
      } catch (const mqtt::exception& e) {
        // Log error if needed, but don't throw in destructor
      }
    }
  }

  std::vector<Device> MqttProvider::GetDevices() const {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    return devices_;
  }

  std::optional<Device> MqttProvider::GetDevice(std::string_view id) const {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    auto it = std::find_if(devices_.begin(), devices_.end(),
        [id](const Device& d) { return d.id == id; });
    if (it != devices_.end()) {
      return *it;
    }
    return std::nullopt;
  }

  void MqttProvider::SetDelegate(std::shared_ptr<IDelegate> delegate) {
    delegate_ = std::move(delegate);
    if (delegate_) {
      std::lock_guard<std::mutex> lock(devices_mutex_);
      for (const auto& device : devices_) {
        delegate_->OnNewDevice(device);
      }
    }
  }

  void MqttProvider::connect(std::string_view broker_address) {
    std::regex mqtt_regex(R"(mqtt://([^:]+):(\d+))");
    std::cmatch match;
    std::string conn_str(broker_address);

    if (!std::regex_match(conn_str.c_str(), match, mqtt_regex)) {
      throw std::invalid_argument("Invalid MQTT broker address format");
    }

    std::string host = match[1].str();
    std::string port = match[2].str();
    std::string address = "tcp://" + host + ":" + port;

    try {
      client_ = std::make_unique<mqtt::async_client>(address, client_id_);
      client_->set_callback(callback_); // Use stored callback
      mqtt::connect_options conn_opts;
      conn_opts.set_keep_alive_interval(20);
      conn_opts.set_clean_session(true);
      conn_opts.set_user_name("openhab");
      conn_opts.set_password("bB6cYQJW6a.!pEq3gZ");

      client_->connect(conn_opts)->wait();
      client_->subscribe("home/devices/#", 1)->wait();
      client_->subscribe("#", 1)->wait();
    } catch (const mqtt::exception& e) {
      throw std::runtime_error("MQTT connection failed: " + std::string(e.what()));
    }
  }

  void MqttProvider::MqttCallback::connected([[maybe_unused]] const std::string& cause) {
    parent_.client_->subscribe("home/devices/#", 1);
    parent_.client_->subscribe("#", 1);
  }

  void MqttProvider::MqttCallback::connection_lost([[maybe_unused]] const std::string& cause) {
    if (parent_.delegate_) {
      parent_.delegate_->OnDeviceChanged(Device{"", "Disconnected", false});
    }
  }

  void MqttProvider::MqttCallback::message_arrived(mqtt::const_message_ptr msg) {
    parent_.handleMessage(msg->get_topic(), msg->get_payload_str());
  }

  void MqttProvider::MqttCallback::delivery_complete([[maybe_unused]] mqtt::delivery_token_ptr token) {
    // No implementation needed
  }

  void MqttProvider::handleMessage(const std::string& topic, const std::string& payload) {
    if (topic.find("home/devices/") != 0) {
      return;
    }

    std::string device_id = topic.substr(13); // Remove "home/devices/" prefix
    Device new_device{device_id, "MQTT Device " + device_id, !payload.empty()};

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
} // namespace lcl::provider