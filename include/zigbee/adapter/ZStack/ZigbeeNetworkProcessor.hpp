#pragma once
#include "ZnpTypes.hpp"
#include "asio/serial_port.hpp"
#include "asio/ip/tcp.hpp"
#include "async/task.hpp"
#include "result/Either.hpp"
#include "zigbee/adapter/IAdapter.hpp"

namespace lcl::zigbee::adapter::zstack {
  constexpr uint8_t SOF = 0xFE;

  inline std::string toHexString(const std::string& input) {
    std::string hex_str;
    for (const unsigned char c : input) {
      hex_str += std::format("{:02X} ", static_cast<unsigned int>(c));
    }
    if (!hex_str.empty()) hex_str.pop_back(); // Remove trailing space
    return hex_str;
  }

  inline std::string toHexString(const std::array<uint8_t, 256> &buffer, uint8_t size) {
    std::string hex_str;
    for (const auto *byte = buffer.begin(); byte != buffer.begin() + size; byte++) {
      hex_str += std::format("{:02X} ", static_cast<unsigned int>(*byte));
    }
    if (!hex_str.empty()) hex_str.pop_back(); // Remove trailing space
    return hex_str;
  }

  inline std::string toHexString(const std::vector<uint8_t> &buffer) {
    std::string output;
    for (const auto byte : buffer) {
      output += std::format("{:02X} ", static_cast<unsigned int>(byte));
    }
    if (!output.empty()) output.pop_back(); // Remove trailing space
    return output;
  }

  inline std::string toDecimalString(const std::vector<uint8_t> &buffer) {
    std::string output;
    for (const auto byte : buffer) {
      output += std::format("{} ", static_cast<unsigned int>(byte));
    }
    if (!output.empty()) output.pop_back(); // Remove trailing space
    return output;
  }



  inline std::string toDecimalString(const std::array<uint8_t, 256> &buffer, uint8_t size) {
    std::string output;
    for (const auto *byte = buffer.begin(); byte != buffer.begin() + size; byte++) {
      output += std::format("{} ", static_cast<unsigned int>(*byte));
    }
    if (!output.empty()) output.pop_back(); // Remove trailing space
    return output;
  }

  class ZigbeeNetworkProcessor {
    ConnectionUri connection_uri;
    ConnectionOptions connection_options;

    std::optional<asio::ip::tcp::socket> socket;
    std::optional<asio::serial_port> serial_port;
    asio::io_context io_context;
    std::thread read_thread;
    std::vector<RawZnpResponse> responses;
    mutable std::mutex response_mutex;
    const uint8_t zdp_transaction_id = 0x00;
    std::optional<SysVersionResponse> version = std::nullopt;

  public:
    [[nodiscard]] ZigbeeNetworkProcessor(const std::string &connection_string, ConnectionOptions connection_options);
    ~ZigbeeNetworkProcessor();

    Errean<AdapterError> connect();
    void startAsyncRead();
    void asyncRead();
    void handleRead(const std::array<uint8_t, 256>& buffer, uint8_t size, std::vector<uint8_t> &pending_buffer);

    bool isInterPan() const {
      return is_inter_pan;
    }

    CMD_ERROR(SysPingResponse) ping();
    CMD_ERROR(SysResetCallback) reset(bool soft_reset = true);
    CMD_ERROR(SysVersionResponse) getVersion(bool force_reload = false);
    CMD_ERROR(StatusableResponse) setStartupOptions(const ZnpStartupOptions& options);
    CMD_ERROR(SysOsalNvReadResponse<ZnpStartupOptions>) getStartupOptions();
    CMD_ERROR(StatusableResponse) setLogicalType(LogicalType);
    CMD_ERROR(StatusableResponse) setPanID(uint16_t pan_id);
    CMD_ERROR(StatusableResponse) setExtendedPanID(uint64_t extended_pan_id);
    CMD_ERROR(StatusableResponse) setApsUseExtendedPanID(uint64_t extended_pan_id);
    CMD_ERROR(StatusableResponse) setPreconfiguredKeysEnabled(bool enabled);
    CMD_ERROR(StatusableResponse) setPreconfiguredKeys(const std::array<uint8_t, 16>& key);
    CMD_ERROR(StatusableResponse) registerApplicationEndpoint(const AfEndpointDescription& description);
    CMD_ERROR(GetDeviceInfoResponse) getDeviceInfo();
    CMD_ERROR(StatusableResponse) getActiveEndpoints(uint16_t destination_address, uint16_t network_address_of_interest);
    CMD_ERROR(SysOsalNvReadResponse<ZnpMemoryAlignment>) getMemoryAlignment();
    CMD_ERROR(StatusableResponse) setZdoDirectCallback(bool enabled);
    CMD_ERROR(StatusableResponse) setChannelList(const std::set<ZnpChannelMask> &channel_list);
    CMD_ERROR(StatusableResponse) deleteNetworkInformationBlock();
    CMD_ERROR(StatusableResponse) setBdbCommissioningChannel(bool is_primary, const std::set<ZnpChannelMask> &channels);
    CMD_ERROR(StatusableResponse) startBdbCommissioning(ZnpCommissioningMode mode);

  private:
    bool is_inter_pan = false;

    [[nodiscard]] RawZnpResponse request(const ZnpCommand& command);

    [[nodiscard]] CMD_ERROR(StatusableResponse) initializeItem(DeviceConfiguration configuration,
                                                               uint16_t total_length,
                                                               const std::vector<uint8_t>::const_iterator &begin,
                                                               const std::vector<uint8_t>::const_iterator &end);
    [[nodiscard]] CMD_ERROR(StatusableResponse) writeItem(DeviceConfiguration configuration,
                                                          const std::vector<uint8_t> &,
                                                          bool auto_initialize = true);
    [[nodiscard]] CMD_ERROR(SysOsalNvReadResponse<std::vector<uint8_t>>) readItem(DeviceConfiguration configuration);
    [[nodiscard]] CMD_ERROR(SysOsalNvDeleteResponse) deleteItem(DeviceConfiguration configuration);
    [[nodiscard]] CMD_ERROR(SysOsalNvLengthResponse) getItemLength(DeviceConfiguration configuration);

    [[nodiscard]] CMD_ERROR(StatusableResponse) writeConfiguration(ConfigurationProperty property,
                                                                   const std::vector<uint8_t> &data);
    [[nodiscard]] CMD_ERROR(StatusableResponse) writeConfiguration(ConfigurationProperty property,
                                                                   const std::vector<uint8_t>::const_iterator &begin,
                                                                   const std::vector<uint8_t>::const_iterator &end);
  };
}
