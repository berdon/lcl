#pragma once
#include <cstdint>
#include <iomanip>
#include <set>

#include "ZnpConstants.hpp"
#include "asio/serial_port.hpp"
#include "asio/ip/tcp.hpp"
#include "result/Either.hpp"

namespace lcl::zigbee::adapter::zstack {
#define CMD_ERROR(RETURN) Either<RETURN, ZnpCommandError>
  enum ZnpMemoryAlignment : uint8_t {
    ZNP_MEMORY_ALIGNMENT_UNALIGNED = 21,
    ZNP_MEMORY_ALIGNMENT_ALIGNED = 24
  };

  enum ConnectionType { CONNECTION_TYPE_TCP, CONNECTION_TYPE_USB };

  enum ZStackErrorCode: uint8_t {
    EC_ZSTACK_RESET_FAILED,
    EC_ZSTACK_FAILED_TO_GET_MEMORY_ALIGNMENT,
    EC_ZSTACK_FAILED_TO_SET_LOGICAL_TYPE,
    EC_ZSTACK_COMMAND_ERROR,
    EC_ZSTACK_INVALID_PAN_ID,
    EC_ZSTACK_FAILURE_STATUS,
    EC_ZSTACK_COMMISSIONING_FAILED
  };

  struct ZStackError: Error<ZStackErrorCode, void> {
    using ErrorCodeType = ZStackErrorCode;
    using ValueType = void;
  };

  struct ZnpCommandError: Error<Command, void> {
    using ErrorCodeType = Command;
    using ValueType = void;
  };

  struct ZnpCommand {
    Command commandId;
    Subsystem subsystem;
    Type type;
    std::vector<uint8_t> data;
    Command responseId = {};
  };

  struct ConnectionOptions { };

  struct SerialConnectionOptions: ConnectionOptions {
    uint32_t baud_rate = 115200;
    uint8_t character_size = 8;
    asio::serial_port_base::parity parity;
    asio::serial_port_base::stop_bits stop_bits;
    asio::serial_port_base::flow_control flow_control;
  };

  struct ConnectionUri {
    ConnectionType connection_type;
    std::optional<std::string> host;
    std::optional<uint16_t> port;
    std::optional<std::string> device_path;

    static ConnectionUri parse(const std::string &);
  };

  struct Payload {
    uint8_t length;
  };

  struct RawZnpResponse {
    Command command;
    Subsystem subsystem;
    Type type;
    std::vector<uint8_t> payload;
  };

  struct StatusableResponse {
    bool status;

    [[nodiscard]] static CMD_ERROR(StatusableResponse) parse(const RawZnpResponse& response);

    // TODO: Make status variables
    [[nodiscard]] static CMD_ERROR(StatusableResponse) success();
    [[nodiscard]] static CMD_ERROR(StatusableResponse) failure();
  };

  struct SysOsalNvDeleteResponse {
    SysOsalNvDeleteStatus status;

    [[nodiscard]] static CMD_ERROR(SysOsalNvDeleteResponse) parse(const RawZnpResponse& response);
  };

  struct SysPingResponse {
    std::set<Capability> capabilities;
    [[nodiscard]] static CMD_ERROR(SysPingResponse) parse(const RawZnpResponse& response);
  };

  struct SysVersionResponse {
    uint8_t transport_protocol_revision;
    uint8_t product_id;
    uint8_t major_release;
    uint8_t minor_release;
    uint8_t maintenance_release;
    [[nodiscard]] static CMD_ERROR(SysVersionResponse) parse(const RawZnpResponse& response);
  };

  struct SysResetCallback {
    SysResetReason reason;
    uint8_t transport_protocol_version;
    uint8_t major_release;
    uint8_t minor_release;
    uint8_t hardware_revision;
    [[nodiscard]] static CMD_ERROR(SysResetCallback) parse(const RawZnpResponse& response);
  };

  struct SysOsalNvLengthResponse {
    uint16_t length;
    [[nodiscard]] static CMD_ERROR(SysOsalNvLengthResponse) parse(const RawZnpResponse& response);
  };

  template<typename T>
  struct SysOsalNvReadResponse : StatusableResponse {
    T data;

    [[nodiscard]] static CMD_ERROR(SysOsalNvReadResponse<T>) parse(const RawZnpResponse& response);
  };

  struct ZnpStartupOptions {
    bool clearNetworkFrameCounter;
    bool clearDeviceConfiguration;
    bool clearNetworkState;

    static ZnpStartupOptions clearAll() {
      return {true, true, true};
    };

    static ZnpStartupOptions clearDeviceAndNetwork() {
      return {false, true, true};
    }

    static ZnpStartupOptions normal() {
      return {false, false, false};
    }

    [[nodiscard]] static CMD_ERROR(ZnpStartupOptions) parse(const std::vector<uint8_t>& buffer, uint8_t offset, uint8_t length);
  };

  struct ZnpNetworkOptions {
    uint16_t pan_id;
    uint64_t extended_pan_id;
    std::set<ZnpChannel> channel_list;
    std::array<uint8_t, 16> network_key;
    bool network_key_distribute;
  };

  struct AfEndpointDescription {
    /**
     * Specifies the endpoint of the device.
     */
    uint8_t endpoint;

    /**
     * Specifies the profile ID of the application.
     */
    uint16_t app_profile_id;

    /**
     * Specifies the device description ID for this endpoint.
     */
    uint16_t app_device_id;

    /**
     * Specifies the device version number.
     */
    uint8_t app_device_version;

    /**
     * Specifies latency.
     * - 0x00-No latency
     * - 0x01-fast beacons
     * - 0x02-slow beacons
     */
    uint8_t latency_request;

    /**
     * The number of Input cluster IDs following in the AppInClusterList.
     */
    uint8_t app_number_in_clusters;

    /**
     * Specifies the list of Input Cluster IDs.
     */
    std::array<uint8_t, 32> app_in_cluster_list;

    /**
     * Specifies the number of Output cluster IDs following in the AppOutClusterList.
     */
    uint8_t app_number_out_clusters;

    /**
     * Specifies the list of Output Cluster IDs.
     */
    std::array<uint8_t, 32> app_out_cluster_list;
  };

  struct IEEEAddress {
    [[nodiscard]] std::string toString() const;

    explicit IEEEAddress() = default;
    explicit IEEEAddress(const std::array<uint8_t, 8>& data) {
      this->data = data;

      std::ostringstream oss;
      oss << std::hex << std::setfill('0');
      for (const auto& byte : data) {
        oss << std::setw(2) << static_cast<unsigned>(byte);
      }
      string_representation = oss.str();
    }
  private:
    std::optional<std::string> string_representation = std::nullopt;
    std::array<uint8_t, 8> data = {};
  };

  struct ShortAddress {
    [[nodiscard]] std::string toString() const;

    explicit ShortAddress() = default;
    explicit ShortAddress(const std::array<uint8_t, 2>& data) {
      this->data = data;

      std::ostringstream oss;
      oss << std::hex << std::setfill('0');
      for (const auto& byte : data) {
        oss << std::setw(2) << static_cast<unsigned>(byte);
      }
      string_representation = oss.str();
    }
  private:
    std::optional<std::string> string_representation = std::nullopt;
    std::array<uint8_t, 2> data = {};
  };

  struct GetDeviceInfoResponse : StatusableResponse {
    /**
     * IEEE address of the device.
     */
    IEEEAddress ieee_address = IEEEAddress();

    /**
     * Short address of the device.
     */
    ShortAddress short_address = ShortAddress();

    /**
     * TODO: Convert to typed?
     * Indicates device type, where bits 0 to 2 indicate the capability for the device to operate as a coordinator,
     * router, or end device, respectively.
     */
    uint8_t device_type;

    /**
     * Indicates the state of the device with different possible state.
     */
    DeviceState device_state;

    /**
     * Specifies the number of devices being associated to the target device.
     */
    uint8_t number_of_associated_devices;

    /**
     * Array of 16-bits specifies the network address associated with the device.
     */
    std::vector<uint16_t> associated_devices;

    [[nodiscard]] static CMD_ERROR(GetDeviceInfoResponse) parse(const RawZnpResponse& response);
  };
}
