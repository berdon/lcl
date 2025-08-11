/**
 * Tags: provider, zigbee
 */
#pragma once
#include <provider/IProvider.hpp>
#include <asio.hpp>
#include <string_view>
#include <memory>

namespace lcl::provider {
  class IDelegate;
  enum class ConnectionType { TCP, USB };

  class ZigbeeProvider : public IProvider {
  public:
    explicit ZigbeeProvider(std::string_view connection_string);
    ~ZigbeeProvider() override;
    [[nodiscard]] std::vector<Device> GetDevices() const override;
    [[nodiscard]] std::optional<Device> GetDevice(std::string_view id) const override;
    void SetDelegate(std::shared_ptr<IDelegate> delegate) override;

  private:
    void parseAndConnect(std::string_view connection_string);
    void connectTcp(std::string_view host, uint16_t port);
    void connectUsb(std::string_view device_path);
    void startAsyncRead();
    void asyncRead();
    void handleRead(const char* data, std::size_t length);

    asio::io_context io_context_;
    asio::ip::tcp::socket socket_;
    asio::serial_port serial_port_;
    std::shared_ptr<IDelegate> delegate_;
    ConnectionType connection_type_;
    std::vector<Device> devices_;
    mutable std::mutex devices_mutex_;
    std::thread read_thread_;
  };
} // namespace lcl::provider