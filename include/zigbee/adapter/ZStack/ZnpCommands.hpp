//
// Created by Austin Hanson on 8/3/25.
//

#pragma once
#include <vector>
#include <memory>

#include "ZnpTypes.hpp"

namespace lcl::zigbee::adapter::zstack {
  ZnpCommand sysResetReq(bool soft_reset = true);
  ZnpCommand sysPing();
  ZnpCommand sysVersion();

  ZnpCommand appCnfBdbSetChannel(bool is_primary, const std::set<ZnpChannelMask> &channels);
  ZnpCommand appCnfBdbStartCommissioning(ZnpCommissioningMode mode);

  ZnpCommand sapiWriteConfiguration(uint8_t configuration_id, const std::vector<uint8_t> &data);
  ZnpCommand sapiWriteConfiguration(uint8_t configuration_id, const std::vector<uint8_t>::const_iterator &begin, const std::vector<uint8_t>::const_iterator &end);

  ZnpCommand sysOsalNvWrite(uint16_t id, uint8_t offset, const std::vector<uint8_t> &data);

  ZnpCommand sysOsalNvWrite(DeviceConfiguration id, uint8_t offset, const std::vector<uint8_t> &data);
  ZnpCommand sysOsalNvWrite(DeviceConfiguration id, uint8_t offset, const std::vector<uint8_t>::const_iterator &begin, const std::vector<uint8_t>::const_iterator &end);
  ZnpCommand sysOsalNvWriteExt(DeviceConfiguration id, uint16_t offset, const std::vector<uint8_t> &data);
  ZnpCommand sysOsalNvWriteExt(DeviceConfiguration id, uint16_t offset, const std::vector<uint8_t>::const_iterator &begin, const std::vector<uint8_t>::const_iterator &end);
  ZnpCommand sysOsalNvLength(DeviceConfiguration id);
  ZnpCommand sysOsalNvRead(DeviceConfiguration id, uint8_t offset);
  ZnpCommand sysOsalNvReadExt(DeviceConfiguration id, uint16_t offset);
  ZnpCommand sysOsalNvDelete(DeviceConfiguration id, uint16_t length);

  ZnpCommand sysOsalNvItemInit(DeviceConfiguration id, uint16_t total_length, const std::vector<uint8_t>::const_iterator &begin, const std::vector<uint8_t>::const_iterator &end);

  ZnpCommand utilSetPanId(uint16_t pan_id);
  ZnpCommand utilSetChannels(uint32_t channels);
  ZnpCommand utilGetDeviceInfo();

  ZnpCommand afRegister(const AfEndpointDescription& description);

  ZnpCommand zdoActiveEndpointRequest(uint8_t zdp_transaction_id, uint16_t destination_address, uint16_t network_address_of_interest);
}
