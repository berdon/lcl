#include <zigbee/adapter/ZStack/ZnpCommands.hpp>

#include "util/VectorUtil.hpp"
#include "zigbee/adapter/ZStack/ZNPTypes.hpp"

namespace lcl::zigbee::adapter::zstack {
  ZnpCommand sysResetReq(const bool soft_reset) {
    auto buffer = std::vector<uint8_t>(1);
    buffer.at(0) = soft_reset ? 1 : 0;
    return ZnpCommand { SYS_RESET_REQ, SUBSYSTEM_SYS, AREQ, buffer, SYS_RESET_IND };
  }

  ZnpCommand sysPing() {
    return ZnpCommand { SYS_PING, SUBSYSTEM_SYS, SREQ, {} };
  }

  ZnpCommand sysVersion() {
    return ZnpCommand { SYS_VERSION, SUBSYSTEM_SYS, SREQ, {} };
  }

  ZnpCommand appCnfBdbSetChannel(const bool is_primary, const std::set<ZnpChannelMask> &channels) {
    auto buffer = std::vector<uint8_t>(5);
    buffer.at(0) = is_primary ? 0x01 : 0x00;

    uint32_t bitmask = 0;
    for (const auto channel : channels) {
      bitmask |= channel;
    }
    SET_VECTOR_AT_LITTLE_ENDIAN_U32(buffer, 1, bitmask);

    return ZnpCommand { MT_APP_CNF_BDB_SET_CHANNEL, SUBSYSTEM_APP_CNF, SREQ, buffer };
  }

  ZnpCommand appCnfBdbStartCommissioning(ZnpCommissioningMode mode) {
    auto buffer = std::vector(std::vector{ static_cast<uint8_t>(mode) });
    return ZnpCommand { MT_APP_CNF_BDB_START_COMMISSIONING, SUBSYSTEM_APP_CNF, SREQ, buffer };
  }

  ZnpCommand sapiWriteConfiguration(
    const uint8_t configuration_id,
    const std::vector<uint8_t> &data
  ) {
    auto buffer = std::vector<uint8_t>(2 + data.size());
    buffer.at(0) = configuration_id;
    buffer.at(1) = data.size();
    FILL_VECTOR(buffer, 2, data);

    return ZnpCommand { SAPI_WRITE_CONFIGURATION, SUBSYSTEM_SAPI, SREQ, buffer };
  }

  ZnpCommand sapiWriteConfiguration(
    const uint8_t configuration_id,
    const std::vector<uint8_t>::const_iterator &begin,
    const std::vector<uint8_t>::const_iterator &end
  ) {
    auto buffer = std::vector<uint8_t>(2 + (end - begin));
    buffer.at(0) = configuration_id;
    buffer.at(1) = end - begin;
    auto index = 2;
    for (auto it = begin; it != end; ++it) {
      buffer.at(index++) = *it;
    }

    return ZnpCommand { SAPI_WRITE_CONFIGURATION, SUBSYSTEM_SAPI, SREQ, buffer };
  }

  ZnpCommand sysOsalNvWrite(const DeviceConfiguration id, const uint8_t offset, const std::vector<uint8_t> &data) {
    auto buffer = std::vector<uint8_t>(4 + data.size());
    SET_VECTOR_AT_LITTLE_ENDIAN_U16(buffer, 0, id);
    buffer.at(2) = offset;
    buffer.at(3) = data.size();
    FILL_VECTOR(buffer, 4, data);

    return ZnpCommand { SYS_OSAL_NV_WRITE, SUBSYSTEM_SYS, SREQ, buffer };
  }

  ZnpCommand sysOsalNvWrite(
    const DeviceConfiguration id,
    const uint8_t offset,
    const std::vector<uint8_t>::const_iterator &begin,
    const std::vector<uint8_t>::const_iterator &end
  ) {
    auto buffer = std::vector<uint8_t>(4 + (end - begin));
    SET_VECTOR_AT_LITTLE_ENDIAN_U16(buffer, 0, id);
    buffer.at(2) = offset;
    buffer.at(3) = end - begin;
    auto index = 4;
    for (auto it = begin; it != end; ++it) {
      buffer.at(index++) = *it;
    }

    return ZnpCommand { SYS_OSAL_NV_WRITE, SUBSYSTEM_SYS, SREQ, buffer };
  }

  ZnpCommand sysOsalNvWriteExt(const DeviceConfiguration id, const uint16_t offset, const std::vector<uint8_t> &data) {
    auto buffer = std::vector<uint8_t>(6 + data.size());
    SET_VECTOR_AT_LITTLE_ENDIAN_U16(buffer, 0, id);
    SET_VECTOR_AT_LITTLE_ENDIAN_U16(buffer, 2, offset);
    SET_VECTOR_AT_LITTLE_ENDIAN_U16(buffer, 4, data.size());
    FILL_VECTOR(buffer, 6, data);

    return ZnpCommand { SYS_OSAL_NV_WRITE_EXT, SUBSYSTEM_SYS, SREQ, buffer };
  }

  ZnpCommand sysOsalNvWriteExt(
    const DeviceConfiguration id,
    const uint16_t offset,
    const std::vector<uint8_t>::const_iterator &begin,
    const std::vector<uint8_t>::const_iterator &end
  ) {
    auto buffer = std::vector<uint8_t>(6 + (end - begin));
    SET_VECTOR_AT_LITTLE_ENDIAN_U16(buffer, 0, id);
    SET_VECTOR_AT_LITTLE_ENDIAN_U16(buffer, 2, offset);
    SET_VECTOR_AT_LITTLE_ENDIAN_U16(buffer, 4, end - begin);
    auto index = 6;
    for (auto it = begin; it != end; ++it) {
      buffer.at(index++) = *it;
    }

    return ZnpCommand { SYS_OSAL_NV_WRITE_EXT, SUBSYSTEM_SYS, SREQ, buffer };
  }

  ZnpCommand sysOsalNvLength(const DeviceConfiguration id) {
    auto buffer = std::vector(std::vector({
      TO_VECTOR_ARG_LITTLE_ENDIAN_U16(id)
    }));
    return ZnpCommand { SYS_OSAL_NV_LENGTH, SUBSYSTEM_SYS, SREQ, buffer };
  }

  ZnpCommand sysOsalNvRead(const DeviceConfiguration id, const uint8_t offset) {
    auto buffer = std::vector(std::vector({
      TO_VECTOR_ARG_LITTLE_ENDIAN_U16(id),
      offset,
    }));
    return ZnpCommand { SYS_OSAL_NV_READ, SUBSYSTEM_SYS, SREQ, buffer };
  }

  ZnpCommand sysOsalNvReadExt(const DeviceConfiguration id, const uint16_t offset) {
    auto buffer = std::vector(std::vector({
      TO_VECTOR_ARG_LITTLE_ENDIAN_U16(id),
      TO_VECTOR_ARG_LITTLE_ENDIAN_U16(offset),
    }));
    return ZnpCommand { SYS_OSAL_NV_READ_EXT, SUBSYSTEM_SYS, SREQ, buffer };
  }

  ZnpCommand sysOsalNvDelete(const DeviceConfiguration id, const uint16_t length) {
    auto buffer = std::vector(std::vector({
      TO_VECTOR_ARG_LITTLE_ENDIAN_U16(id),
      TO_VECTOR_ARG_LITTLE_ENDIAN_U16(length),
    }));
    return ZnpCommand { SYS_OSAL_NV_DELETE, SUBSYSTEM_SYS, SREQ, buffer };
  }

  ZnpCommand sysOsalNvItemInit(
    const DeviceConfiguration id,
    const uint16_t total_length,
    const std::vector<uint8_t>::const_iterator &begin,
    const std::vector<uint8_t>::const_iterator &end
  ) {
    auto buffer = std::vector<uint8_t>(5 + (end - begin));
    SET_VECTOR_AT_LITTLE_ENDIAN_U16(buffer, 0, id);
    SET_VECTOR_AT_LITTLE_ENDIAN_U16(buffer, 2, total_length);
    buffer.at(4) = end - begin;
    auto index = 5;
    for (auto it = begin; it != end; ++it) {
      buffer.at(index++) = *it;
    }

    return ZnpCommand { SYS_OSAL_NV_ITEM_INIT, SUBSYSTEM_SYS, SREQ, buffer };
  }

  ZnpCommand utilSetPanId(const uint16_t pan_id) {
    auto buffer = std::vector(std::vector({
      TO_VECTOR_ARG_LITTLE_ENDIAN_U16(pan_id)
    }));
    return ZnpCommand { UTIL_SET_PAN_ID, SUBSYSTEM_UTIL, SREQ, buffer };
  }

  ZnpCommand utilSetChannels(const uint32_t channels) {
    auto buffer = std::vector(std::vector({
      TO_VECTOR_ARG_LITTLE_ENDIAN_U32(channels)
    }));
    return ZnpCommand { UTIL_SET_PAN_ID, SUBSYSTEM_UTIL, SREQ, buffer };
  }

  ZnpCommand utilGetDeviceInfo() {
    return ZnpCommand { UTIL_GET_DEVICE_INFO, SUBSYSTEM_UTIL, SREQ };
  }

  ZnpCommand afRegister(const AfEndpointDescription& description) {
    auto buffer = std::vector<uint8_t>(73);
    buffer.at(0) = description.endpoint;
    SET_VECTOR_AT_LITTLE_ENDIAN_U16(buffer, 1, description.app_profile_id);
    SET_VECTOR_AT_LITTLE_ENDIAN_U16(buffer, 3, description.app_device_id);
    buffer.at(5) = description.app_device_version;
    buffer.at(6) = description.latency_request;
    buffer.at(7) = description.app_number_in_clusters;
    FILL_VECTOR(buffer, 8, description.app_in_cluster_list);
    buffer.at(9 + 32) = description.app_number_out_clusters;
    FILL_VECTOR(buffer, 10 + 32, description.app_out_cluster_list);

    return ZnpCommand { AF_REGISTER, SUBSYSTEM_AF, SREQ, buffer };
  }

  ZnpCommand zdoActiveEndpointRequest(
    const uint8_t zdp_transaction_id,
    const uint16_t destination_address,
    const uint16_t network_address_of_interest
  ) {
    const auto buffer = std::vector(std::vector({
      // TODO: Maybe required?
      static_cast<uint8_t>(zdp_transaction_id),
      TO_VECTOR_ARG_LITTLE_ENDIAN_U16(destination_address),
      TO_VECTOR_ARG_LITTLE_ENDIAN_U16(network_address_of_interest)
    }));

    return ZnpCommand { ZDO_ACTIVE_EP_REQ, SUBSYSTEM_ZDO, SREQ, buffer };
  }
}
