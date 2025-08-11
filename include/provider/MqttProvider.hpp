/**
* Tags: provider, mqtt
 */
#pragma once
#include <provider/IProvider.hpp>
#include <mqtt/async_client.h>
#include <string_view>
#include <memory>
#include <mutex>
#include <vector>

namespace lcl::provider {
  class IDelegate;

  class MqttProvider : public IProvider {
  public:
    explicit MqttProvider(std::string_view broker_address);
    ~MqttProvider() override;

    [[nodiscard]] std::vector<Device> GetDevices() const override;
    [[nodiscard]] std::optional<Device> GetDevice(std::string_view id) const override;
    void SetDelegate(std::shared_ptr<IDelegate> delegate) override;

  private:
    class MqttCallback : public mqtt::callback {
    public:
      explicit MqttCallback(MqttProvider& parent) : parent_(parent) {}
      void connected([[maybe_unused]] const std::string& cause) override;
      void connection_lost([[maybe_unused]] const std::string& cause) override;
      void message_arrived(mqtt::const_message_ptr msg) override;
      void delivery_complete([[maybe_unused]] mqtt::delivery_token_ptr token) override;

    private:
      MqttProvider& parent_;
    };

    void connect(std::string_view broker_address);
    void handleMessage(const std::string& topic, const std::string& payload);

    std::unique_ptr<mqtt::async_client> client_;
    std::shared_ptr<IDelegate> delegate_;
    std::vector<Device> devices_;
    mutable std::mutex devices_mutex_;
    std::string client_id_;
    MqttCallback callback_; // Store callback as a member
  };
} // namespace lcl::provider
