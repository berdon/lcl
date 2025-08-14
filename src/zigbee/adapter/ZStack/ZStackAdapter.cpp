/**
 * Tags: provider, zigbee, tcp, usb
 */
#include <iostream>
#include <zigbee/adapter/ZStack/ZStackAdapter.hpp>

#include "logger/logger.hpp"
#include "zigbee/ZigbeeError.hpp"
#include "zigbee/ZSpec.hpp"

#define TAG() "ZStackAdapter"
#define VERIFY_COMMISSIONING_STEP(result, msg) \
  if (!result || !result->status) { \
    co_return ZStackError { EC_ZSTACK_COMMISSIONING_FAILED, msg, result.error() }; \
  }

namespace lcl::zigbee::adapter {
  using namespace zstack;
  using namespace logger;

  ZStackAdapter::ZStackAdapter(const std::string &connection, const ConnectionOptions options = {}): network_processor(connection, options) {
  }

  TaskErrean<ZStackError> ZStackAdapter::initializeProcessor() {
    // TODO: Do this a few times
    // Ping and capabilities
    auto ping_response = co_await network_processor.ping();
    if (!ping_response) {
      co_return ZStackError {
        EC_ZSTACK_COMMAND_ERROR,
        ping_response.errorOrThrow().message,
        ping_response.errorOrThrow()
      };
    }

    for (auto capability: ping_response.value()->capabilities) {
      capabilities.insert(capability);
    }

    // Grab the firmware version
    auto version_result = co_await network_processor.getVersion();
    if (!version_result) {
      Logger::debug(TAG(), "Failed to get z-stack firmware version; assuming 1.2");
      version = SysVersionResponse {
      {true},
        2,
        0,
        2,
        0
      };
    } else {
      version = *version_result;
    }

    // Grab the memory alignment of the adapter
    auto nwkkey_response = co_await network_processor.getMemoryAlignment();
    if (!nwkkey_response || !nwkkey_response->status) {
      co_return ZStackError {
        EC_ZSTACK_FAILED_TO_GET_MEMORY_ALIGNMENT,
        "Failed to get the configured memory alignment.",
        nwkkey_response.error()
      };
    }
    memory_alignment = nwkkey_response->data;

    const auto commissioning_result = beginCommissioning({
      // 0x1A62,
      // 0xDDDDDDDDDDDDDDDD,
      0x1A64,
      0xDDDDDDDDDDDDDDDD,
      // {ZNP_CHANNEL_11},
      {ZNP_CHANNEL_12},
      {0X01, 0X03, 0X05, 0X07, 0X09, 0X0B, 0X0D, 0X0F, 0X00, 0X02, 0X04, 0X06, 0X08, 0X0A, 0X0C, 0X0D},
      false
    }, true, true);
    std::this_thread::sleep_for(std::chrono::seconds(10));

    const auto result = network_processor.getActiveEndpoints(0xFFFF, 0xFFFF);

    co_return std::nullopt;
  }

  std::set<ZnpChannelMask> toChannelMask(const std::set<ZnpChannel> &channels) {
    auto channel_masks = std::set<ZnpChannelMask>();
    for (const auto channel : channels) {
      if (channel == ZNP_CHANNEL_11) channel_masks.emplace(ZNP_CHANNEL_MASK_11);
      else if (channel == ZNP_CHANNEL_12) channel_masks.emplace(ZNP_CHANNEL_MASK_12);
      else if (channel == ZNP_CHANNEL_13) channel_masks.emplace(ZNP_CHANNEL_MASK_13);
      else if (channel == ZNP_CHANNEL_14) channel_masks.emplace(ZNP_CHANNEL_MASK_14);
      else if (channel == ZNP_CHANNEL_15) channel_masks.emplace(ZNP_CHANNEL_MASK_15);
      else if (channel == ZNP_CHANNEL_16) channel_masks.emplace(ZNP_CHANNEL_MASK_16);
      else if (channel == ZNP_CHANNEL_17) channel_masks.emplace(ZNP_CHANNEL_MASK_17);
      else if (channel == ZNP_CHANNEL_18) channel_masks.emplace(ZNP_CHANNEL_MASK_18);
      else if (channel == ZNP_CHANNEL_19) channel_masks.emplace(ZNP_CHANNEL_MASK_19);
      else if (channel == ZNP_CHANNEL_20) channel_masks.emplace(ZNP_CHANNEL_MASK_20);
      else if (channel == ZNP_CHANNEL_21) channel_masks.emplace(ZNP_CHANNEL_MASK_21);
      else if (channel == ZNP_CHANNEL_22) channel_masks.emplace(ZNP_CHANNEL_MASK_22);
      else if (channel == ZNP_CHANNEL_23) channel_masks.emplace(ZNP_CHANNEL_MASK_23);
      else if (channel == ZNP_CHANNEL_24) channel_masks.emplace(ZNP_CHANNEL_MASK_24);
      else if (channel == ZNP_CHANNEL_25) channel_masks.emplace(ZNP_CHANNEL_MASK_25);
      else if (channel == ZNP_CHANNEL_26) channel_masks.emplace(ZNP_CHANNEL_MASK_26);
    }
    return channel_masks;
  }

  TaskErrean<ZStackError> ZStackAdapter::beginCommissioning(
    const ZnpNetworkOptions &network_options,
    const bool fail_on_collision,
    const bool write_configuration_flag
  ) {
    if (network_options.pan_id == 0xFFFF) {
      co_return ZStackError { EC_ZSTACK_INVALID_PAN_ID, "PAN ID cannot be 0xFFFF" };
    }

    Logger::info("USER", "Deleting network block information");
    const auto nib_result = co_await network_processor.deleteNetworkInformationBlock();
    if (!nib_result || !nib_result->status) {
      co_return ZStackError { EC_ZSTACK_COMMISSIONING_FAILED, "Failed to clear the NIB", nib_result.error() };
    }

    const auto clear_result = co_await clearAdapter();
    if (!clear_result) {
      co_return ZStackError { EC_ZSTACK_COMMISSIONING_FAILED, "Failed to clear the adapter", clear_result };
    }

    const auto update_result = co_await updateCommissioningItems(network_options);
    if (!update_result) co_return {update_result};

    if (std::set<uint8_t>({ZNP_VERSION_ZSTACK_3x0, ZNP_VERSION_ZSTACK_30x}).contains(version->product_id)) {
      Logger::info("USER", "Setting primary BDB Commissioning channels");
      auto set_channel_result = co_await network_processor.setBdbCommissioningChannel(true, toChannelMask(network_options.channel_list));
      if (!set_channel_result || !set_channel_result->status) {
        co_return ZStackError { EC_ZSTACK_COMMISSIONING_FAILED, "Failed to set the primary BDB Commissioning channel set", set_channel_result };
      }

      Logger::info("USER", "Setting secondary BDB Commissioning channels");
      set_channel_result = co_await network_processor.setBdbCommissioningChannel(false, std::set<ZnpChannelMask> {});
      if (!set_channel_result || !set_channel_result->status) {
        co_return ZStackError { EC_ZSTACK_COMMISSIONING_FAILED, "Failed to set the secondary BDB Commissioning channel set", set_channel_result };
      }

      Logger::info("USER", "Starting BDB Commissioning commissioning");
      const auto start_result = co_await network_processor.startBdbCommissioning(ZNP_COMMISSIONING_MODE_NETWORK_FORMATION);
      VERIFY_COMMISSIONING_STEP(set_channel_result, "Failed to start BDB Commissioning");
    } else {

    }

    co_return true;
  }

  TaskErrean<ZStackError> ZStackAdapter::updateCommissioningItems(const ZnpNetworkOptions &network_options) {
    Logger::info("USER", "Clearing startup options");
    const auto startup_result = co_await network_processor.setStartupOptions(ZnpStartupOptions::normal());
    if (!startup_result || !startup_result->status) {
      co_return ZStackError { EC_ZSTACK_COMMISSIONING_FAILED, "Failed to set startup options.", startup_result };
    }

    Logger::info("USER", "Setting the logical type to coordinator");
    const auto logical_type_result = co_await network_processor.setLogicalType(LOGICAL_TYPE_COORDINATOR);
    if (!logical_type_result || !logical_type_result->status) {
      co_return ZStackError { EC_ZSTACK_COMMISSIONING_FAILED, "Failed to set logical type.", logical_type_result };
    }

    Logger::info("USER", "Enabling ZDO direct callback");
    const auto zdo_direct_db_result = co_await network_processor.setZdoDirectCallback(true);
    if (!zdo_direct_db_result || !zdo_direct_db_result->status) {
      co_return ZStackError { EC_ZSTACK_COMMISSIONING_FAILED, "Failed to enable the ZDO direct callback.", zdo_direct_db_result };
    }

    Logger::info("USER", "Setting the channel lists");
    const auto channel_list_result = co_await network_processor.setChannelList(toChannelMask(network_options.channel_list));
    if (!channel_list_result || !channel_list_result->status) {
      co_return ZStackError { EC_ZSTACK_COMMISSIONING_FAILED, "Failed to set the channel list.", channel_list_result };
    }

    Logger::info("USER", "Setting the pan ID");
    const auto pan_id_result = co_await network_processor.setPanID(network_options.pan_id);
    if (!pan_id_result || !pan_id_result->status) {
      co_return ZStackError { EC_ZSTACK_COMMISSIONING_FAILED, "Failed to set the pan ID.", pan_id_result };
    }

    Logger::info("USER", "Setting the extended pan ID");
    const auto extended_pan_id_result = co_await network_processor.setExtendedPanID(network_options.extended_pan_id);
    if (!extended_pan_id_result || !extended_pan_id_result->status) {
      co_return ZStackError { EC_ZSTACK_COMMISSIONING_FAILED, "Failed to set the extended pan ID.", extended_pan_id_result };
    }

    Logger::info("USER", "Setting the APS use extended pan ID");
    const auto aps_use_extended_pan_id_result = co_await network_processor.setApsUseExtendedPanID(network_options.extended_pan_id);
    if (!aps_use_extended_pan_id_result || !aps_use_extended_pan_id_result->status) {
      co_return ZStackError { EC_ZSTACK_COMMISSIONING_FAILED, "Failed to set the APS use extended pan ID.", aps_use_extended_pan_id_result };
    }

    Logger::info("USER", "Enabling preconfigured keys");
    const auto enabled_prec_keys_result = co_await network_processor.setPreconfiguredKeysEnabled(network_options.network_key_distribute);
    if (!enabled_prec_keys_result || !enabled_prec_keys_result->status) {
      co_return ZStackError { EC_ZSTACK_COMMISSIONING_FAILED, "Failed to enable preconfigured keys.", enabled_prec_keys_result };
    }

    Logger::info("USER", "Setting preconfigured keys");
    const auto preconfigured_keys_result = co_await network_processor.setPreconfiguredKeys(network_options.network_key);
    if (!preconfigured_keys_result || !preconfigured_keys_result->status) {
      co_return ZStackError { EC_ZSTACK_COMMISSIONING_FAILED, "Failed to set preconfigured keys", preconfigured_keys_result.error() };
    }

    co_return true;
  }

  TaskErrean<ZStackError> ZStackAdapter::clearAdapter() {
    auto startup_write_result = co_await network_processor.setStartupOptions(ZnpStartupOptions::clearDeviceAndNetwork());
    if (!startup_write_result || !startup_write_result->status)
      co_return ZStackError { EC_ZSTACK_FAILURE_STATUS, "Failed to set startup options", startup_write_result.error() };

    const auto reset_result = co_await network_processor.reset();
    if (!startup_write_result || !startup_write_result->status)
      co_return ZStackError { EC_ZSTACK_FAILURE_STATUS, "Failed to reset the adapter", startup_write_result.error() };

    startup_write_result = co_await network_processor.setStartupOptions(ZnpStartupOptions::normal());
    if (!startup_write_result || !startup_write_result->status)
      co_return ZStackError { EC_ZSTACK_FAILURE_STATUS, "Failed to set startup options", startup_write_result.error() };

    co_return true;
  }

  TaskErrean<ZStackError> ZStackAdapter::clearZigbeeNetworkProcessor() {
    auto write_response = co_await network_processor.setStartupOptions(ZnpStartupOptions::clearDeviceAndNetwork());
    if (!write_response || !write_response->status) {
      co_return ZStackError {
        EC_ZSTACK_FAILURE_STATUS,
        "Received a failure status while writing startup options.",
        write_response.error()
      };
    }

    // Restart to force the configurations to be reloaded
    co_await network_processor.reset();

    write_response = co_await network_processor.setStartupOptions(ZnpStartupOptions::normal());
    if (!write_response || !write_response->status) {
      co_return ZStackError {
        EC_ZSTACK_FAILURE_STATUS,
        "Received a failure status while writing startup options.",
        write_response.error()
      };
    }

    co_return true;
  }

  TaskErrean<AdapterError> ZStackAdapter::start() {
    const auto connect_result = co_await network_processor.connect();
    if (!connect_result) {
      co_return connect_result.value();
    }

    const auto initialize_result = co_await initializeProcessor();
    if (!initialize_result) {
      co_return AdapterError {
        EC_ADAPTER_FAILED_TO_START,
        "Failed to start the z-stack adapter.",
        initialize_result};
    }

    co_return true;
  }
} // namespace lcl::zigbee::adapter
