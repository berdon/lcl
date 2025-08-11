#include "zigbee/adapter/ZStack/ZnpTypes.hpp"

#include <iomanip>
#include <string>

#include "zigbee/adapter/ZStack/ZigbeeNetworkProcessor.hpp"

namespace lcl::zigbee::adapter::zstack {
  std::string IEEEAddress::toString() const {
    return string_representation.value();
  }

  std::string ShortAddress::toString() const {
    return string_representation.value();
  }

  CMD_ERROR(StatusableResponse) StatusableResponse::parse(const RawZnpResponse &response) {
    auto typed_response = StatusableResponse { };
    typed_response.status = response.payload[0] == 0;
    return CMD_ERROR(StatusableResponse)::value(typed_response);
  }

  Either<StatusableResponse, ZnpCommandError> StatusableResponse::success() {
    return StatusableResponse {true};
  }

  Either<StatusableResponse, ZnpCommandError> StatusableResponse::failure() {
    return StatusableResponse {false};
  }

  Either<SysOsalNvDeleteResponse, ZnpCommandError> SysOsalNvDeleteResponse::parse(const RawZnpResponse &response) {
    auto typed_response = SysOsalNvDeleteResponse { };
    typed_response.status = static_cast<SysOsalNvDeleteStatus>(response.payload[0]);
    return CMD_ERROR(SysOsalNvDeleteResponse)::value(typed_response);
  }

  CMD_ERROR(SysPingResponse) SysPingResponse::parse(const RawZnpResponse& response) {
    auto typed_response = SysPingResponse { };
    const auto capabilities = response.payload[0] << 8 | response.payload[1];
    if ((MT_CAP_SYS & capabilities) == MT_CAP_SYS) {
      typed_response.capabilities.emplace(MT_CAP_SYS);
    }
    if ((MT_CAP_MAC & capabilities) == MT_CAP_MAC) {
      typed_response.capabilities.emplace(MT_CAP_MAC);
    }
    if ((MT_CAP_NWK & capabilities) == MT_CAP_NWK) {
      typed_response.capabilities.emplace(MT_CAP_NWK);
    }
    if ((MT_CAP_AF & capabilities) == MT_CAP_AF) {
      typed_response.capabilities.emplace(MT_CAP_AF);
    }
    if ((MT_CAP_ZDO & capabilities) == MT_CAP_ZDO) {
      typed_response.capabilities.emplace(MT_CAP_ZDO);
    }
    if ((MT_CAP_SAPI & capabilities) == MT_CAP_SAPI) {
      typed_response.capabilities.emplace(MT_CAP_SAPI);
    }
    if ((MT_CAP_UTIL & capabilities) == MT_CAP_UTIL) {
      typed_response.capabilities.emplace(MT_CAP_UTIL);
    }
    if ((MT_CAP_DEBUG & capabilities) == MT_CAP_DEBUG) {
      typed_response.capabilities.emplace(MT_CAP_DEBUG);
    }
    if ((MT_CAP_APP & capabilities) == MT_CAP_APP) {
      typed_response.capabilities.emplace(MT_CAP_APP);
    }
    if ((MT_CAP_ZOAD & capabilities) == MT_CAP_ZOAD) {
      typed_response.capabilities.emplace(MT_CAP_ZOAD);
    }

    return Either<SysPingResponse, ZnpCommandError>::value(typed_response);
  }

  CMD_ERROR(SysVersionResponse) SysVersionResponse::parse(const RawZnpResponse &response) {
    auto typed_response = SysVersionResponse { };
    typed_response.transport_protocol_revision = response.payload[0];
    typed_response.product_id = response.payload[1];
    typed_response.major_release = response.payload[2];
    typed_response.minor_release = response.payload[3];
    typed_response.maintenance_release = response.payload[4];

    return CMD_ERROR(SysVersionResponse)::value(typed_response);
  }

  template<>
  Either<SysOsalNvReadResponse<std::vector<uint8_t>>, ZnpCommandError> SysOsalNvReadResponse<std::vector<uint8_t>>::parse(const RawZnpResponse &result) {
    CMD_ERROR(SysOsalNvReadResponse) response = SysOsalNvReadResponse {};
    response->status = result.payload[0] == 0;
    const auto length = result.payload[1];
    response->data = std::vector<uint8_t>(length);
    std::ranges::copy(result.payload.begin() + 2, result.payload.end(), response->data.begin());
    return response;
  }

  CMD_ERROR(SysResetCallback) SysResetCallback::parse(const RawZnpResponse& response) {
    auto typed_response = SysResetCallback { };
    typed_response.reason = static_cast<SysResetReason>(response.payload[0]);
    typed_response.transport_protocol_version = response.payload[1];
    typed_response.major_release = response.payload[2];
    typed_response.minor_release = response.payload[3];
    typed_response.hardware_revision = response.payload[4];

    return CMD_ERROR(SysResetCallback)::value(typed_response);
  }

  CMD_ERROR(SysOsalNvLengthResponse) SysOsalNvLengthResponse::parse(const RawZnpResponse& response) {
    auto typed_response = SysOsalNvLengthResponse { };
    typed_response.length = response.payload[1] << 8 | response.payload[0];
    return CMD_ERROR(SysOsalNvLengthResponse)::value(typed_response);
  }

  CMD_ERROR(ZnpStartupOptions) ZnpStartupOptions::parse(const std::vector<uint8_t>& buffer, const uint8_t offset, uint8_t length) {
    auto typed_response = ZnpStartupOptions { };
    typed_response.clearNetworkFrameCounter = ZCD_STARTOPT_CLEAR_NWK_FRAME_COUNTER == (buffer[offset + 0] & ZCD_STARTOPT_CLEAR_NWK_FRAME_COUNTER);
    typed_response.clearNetworkState = ZCD_STARTOPT_CLEAR_STATE == (buffer[offset + 0] & ZCD_STARTOPT_CLEAR_STATE);
    typed_response.clearDeviceConfiguration = ZCD_STARTOPT_CLEAR_CONFIG == (buffer[offset + 0] & ZCD_STARTOPT_CLEAR_CONFIG);
    return CMD_ERROR(ZnpStartupOptions)::value(typed_response);
  }

  CMD_ERROR(GetDeviceInfoResponse) GetDeviceInfoResponse::parse(const RawZnpResponse &response) {
    auto typed_response = GetDeviceInfoResponse { };
    auto index = 0;
    typed_response.status = response.payload.at(index++) == 0;

    std::array<uint8_t, 8> ieee_address_buffer = {};
    std::copy_n(response.payload.begin() + (index++), 8, ieee_address_buffer.begin());
    typed_response.ieee_address = IEEEAddress(ieee_address_buffer);
    index += 7;

    std::array<uint8_t, 2> short_address_buffer = {};
    std::copy_n(response.payload.begin() + (index++), 2, short_address_buffer.begin());
    typed_response.short_address = ShortAddress(short_address_buffer);
    index += 1;

    typed_response.device_type = response.payload.at(index++);
    typed_response.device_state = static_cast<DeviceState>(response.payload.at(index++));
    typed_response.number_of_associated_devices = response.payload.at(index++);

    for (std::vector<uint8_t>::size_type i = index; i < response.payload.size(); i += 2) {
      const auto device_address = (response.payload.at(i + 1) << 8) | (response.payload.at(i) & 0xFF);
      typed_response.associated_devices.push_back(device_address);
    }

    return CMD_ERROR(GetDeviceInfoResponse)::value(typed_response);
  }
}
