/**
 * Tags: zigbee, zstack
 */

#include "zigbee/adapter/ZStack/ZigbeeNetworkProcessor.hpp"

#include <iomanip>
#include <iostream>
#include <mutex>
#include <regex>

#include "asio/connect.hpp"
#include "logger/logger.hpp"
#include "util/VectorUtil.hpp"
#include "zigbee/adapter/ZStack/ZnpCommands.hpp"

#define TAG() "ZigbeeNetworkProcessor"

namespace lcl::zigbee::adapter::zstack {
  using logger::Logger;

  ConnectionUri ConnectionUri::parse(const std::string &connection_string) {
    const std::regex tcp_regex(R"(tcp://([^:]+):(\d+))");
    const std::regex usb_regex(R"(usb://(/dev/.*))");

    if (std::cmatch match; std::regex_match(connection_string.c_str(), match, tcp_regex)) {
      return ConnectionUri { ConnectionType::CONNECTION_TYPE_TCP, match[1].str(), std::stoi(match[2].str()), std::nullopt };
    }

    if (std::cmatch match; std::regex_match(connection_string.c_str(), match, usb_regex)) {
      return ConnectionUri { ConnectionType::CONNECTION_TYPE_USB, std::nullopt, std::nullopt, match[1].str() };
    }

    throw std::invalid_argument("Invalid connection string format");
  }

  ZigbeeNetworkProcessor::ZigbeeNetworkProcessor(
    const std::string &connection_string,
    const ConnectionOptions connection_options
  ): connection_uri(ConnectionUri::parse(connection_string)), connection_options(connection_options) { }

  ZigbeeNetworkProcessor::~ZigbeeNetworkProcessor() {
    io_context.stop();
    if (socket.has_value() && socket->is_open()) {
      socket->close();
    }
    if (serial_port.has_value() && serial_port->is_open()) {
      serial_port->close();
    }
    if (read_thread.joinable()) {
      read_thread.join();
    }
  }

  Errean<AdapterError> ZigbeeNetworkProcessor::connect() {
    switch (connection_uri.connection_type) {
      case ConnectionType::CONNECTION_TYPE_TCP:
        try {
          socket = asio::ip::tcp::socket(io_context);
          asio::ip::tcp::resolver resolver(io_context);
          const auto endpoints = resolver.resolve(
            *connection_uri.host,
            std::to_string(*connection_uri.port));
          asio::connect(*socket, endpoints);
          socket->set_option(asio::ip::tcp::no_delay(true));
        } catch (const std::exception& e) {
          throw std::runtime_error("TCP connection failed: " + std::string(e.what()));
        }
        startAsyncRead();
        break;
      case ConnectionType::CONNECTION_TYPE_USB:
        try {
          auto serial_options = static_cast<SerialConnectionOptions>(connection_options);
          serial_port = asio::serial_port(io_context);
          serial_port->open(std::string(*connection_uri.device_path));
          serial_port->set_option(asio::serial_port_base::baud_rate(serial_options.baud_rate));
          serial_port->set_option(asio::serial_port_base::character_size(serial_options.character_size));
          serial_port->set_option(asio::serial_port_base::parity(serial_options.parity));
          serial_port->set_option(asio::serial_port_base::stop_bits(serial_options.stop_bits));
          serial_port->set_option(asio::serial_port_base::flow_control(serial_options.flow_control));
        } catch (const std::exception& e) {
          throw std::runtime_error("USB connection failed: " + std::string(e.what()));
        }
        startAsyncRead();
        break;
      default:
        throw std::invalid_argument("Invalid connection uri");
    }

    return true;
  }

  void ZigbeeNetworkProcessor::startAsyncRead() {
    read_thread = std::thread([this]() {
      try {
        while (!io_context.stopped()) {
          io_context.restart();
          asyncRead();
          io_context.run();
        }
      } catch (const std::exception&) {
        throw;
        // if (delegate) {
        //   delegate->OnDeviceChanged(Device{"", "Error", false});
        // }
      }
    });
  }

  std::optional<RawZnpResponse> parseResponse(std::vector<uint8_t> &buffer) {
    if (buffer.size() < 1) {
      return std::nullopt;
    }

    if (buffer[0] != SOF) {
      // TODO
      throw std::invalid_argument("Invalid response");
    }

    // Not enough bytes
    if (buffer.size() < 4) {
      return std::nullopt;
    }

    const auto length = buffer[1];
    if (buffer.size() < 4 + length) {
      return std::nullopt;
    }

    const auto type = static_cast<Type>(buffer[2] >> 5);
    const auto subsystem = static_cast<Subsystem>(0x1F & buffer[2]);
    const auto command_id = static_cast<Command>(buffer[3]);
    const auto data = std::vector(buffer.begin() + 4, buffer.begin() + 4 + length);

    buffer.erase(buffer.begin(), buffer.begin() + 4 + length + 1);

    // TODO: Verify checksum
    // const auto checksum = (*buffer)[4 + length];

    Logger::trace(TAG(), "<<< FRAME: %02X, %02X, %02X, %s", command_id, subsystem, type, toHexString(data), toDecimalString(data));

    return RawZnpResponse { command_id, subsystem, type, data };
  }

  void ZigbeeNetworkProcessor::handleRead(const std::array<uint8_t, 256> &buffer, const uint8_t size, std::vector<uint8_t> &pending_buffer) {
    std::lock_guard lock(response_mutex);
    if (Logger::currentLogLevel() == logger::LogLevel::TRACE) {
      Logger::trace(TAG(), "Reading [%d]", size);
      Logger::trace(TAG(), "%s | %s", toHexString(buffer, size), toDecimalString(buffer, size));
    }
    if (size <= 1) return;
    for (auto it = buffer.begin(); it < buffer.begin() + size; ++it) {
      pending_buffer.push_back(*it);
    }

    auto response = parseResponse(pending_buffer);
    while (response.has_value()) {
      responses.push_back(*response);
      response = parseResponse(pending_buffer);
    }
  }

  auto pending_buffer = std::vector<uint8_t>(0);
  void ZigbeeNetworkProcessor::asyncRead() {
    auto buffer = std::make_shared<std::array<uint8_t, 256>>();
    if (connection_uri.connection_type == CONNECTION_TYPE_TCP) {
      socket->async_read_some(
          asio::buffer(*buffer),
          [this, buffer](const asio::error_code& ec, const std::size_t bytes_transferred) {
            if (!ec) {
              handleRead(*buffer, bytes_transferred, pending_buffer);
              asyncRead();
            } else if (ec != asio::error::operation_aborted) {
              // if (delegate_) {
              //   delegate_->OnDeviceChanged(Device{"", "Disconnected", false});
              // }
            }
          });
    } else {
      serial_port->async_read_some(
          asio::buffer(*buffer),
          [this, buffer](const asio::error_code& ec, const std::size_t bytes_transferred) {
            if (!ec) {
              handleRead(*buffer, bytes_transferred, pending_buffer);
              asyncRead();
            } else if (ec != asio::error::operation_aborted) {
              throw;
              // if (delegate_) {
              //   delegate_->OnDeviceChanged(Device{"", "Disconnected", false});
              // }
            }
          });
    }
  }

  RawZnpResponse ZigbeeNetworkProcessor::request(const ZnpCommand& command) {
    // Create the buffer of data
    // TODO: Length
    std::vector<uint8_t> frame {
      SOF,
      static_cast<uint8_t>(command.data.empty() ? 0 : command.data.size()),
      static_cast<uint8_t>((command.type << 5 & 0xE0) | (command.subsystem & 0x1F)),
      command.commandId
    };

    if (!command.data.empty()) {
      frame.insert(frame.end(), command.data.begin(), command.data.end());
    }

    uint8_t checksum = 0;
    for (std::vector<uint8_t>::size_type i = 1; i < frame.size(); i++) {
      checksum ^= frame[i];
    }
    frame.push_back(checksum);

    if (command.commandId == SYS_RESET_REQ) {
      std::lock_guard lock(response_mutex);
      responses.clear();
    }

    try {
      switch (connection_uri.connection_type) {
        case CONNECTION_TYPE_TCP:
          socket->write_some(asio::buffer(frame));
          break;
        case CONNECTION_TYPE_USB:
          if (Logger::currentLogLevel() == logger::LogLevel::TRACE) {
            Logger::trace(TAG(), "Writing: ");
            Logger::trace(TAG(), "%s | %s", toHexString(frame), toDecimalString(frame));
          }
          serial_port->write_some(asio::buffer(frame));
          break;
      }

      while (true) {
        {
          std::lock_guard lock(response_mutex);
          if (!responses.empty()) {
            auto index = 0;
            for (const auto& r: responses) {
              if ((r.command == command.commandId && command.type == SREQ && r.type == SRSP)
                || (r.command == command.responseId && command.type == AREQ && r.type == AREQ)) {
                RawZnpResponse response = r;
                responses.erase(responses.begin() + index);
                return response;
              }
              index += 1;
            }
          }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    } catch (const std::exception&) {
      throw;
    }
  }

  CMD_ERROR(StatusableResponse) ZigbeeNetworkProcessor::initializeItem(
    const DeviceConfiguration device_configuration,
    const uint16_t total_length,
    const std::vector<uint8_t>::const_iterator &begin,
    const std::vector<uint8_t>::const_iterator &end
  ) {
    Logger::debug(TAG(), "Initializing NV Item [" + std::to_string(device_configuration) + "]");
    Logger::trace(TAG(), "[Send]: SYS_OSAL_NV_ITEM_INIT >>>");
    const auto result = request(sysOsalNvItemInit(
      device_configuration,
      total_length,
      begin,
      end));
    const auto parsed_result = StatusableResponse::parse(result);
    if (!parsed_result || !parsed_result->status) {
      return CMD_ERROR(SysOsalNvReadResponse<std::vector<uint8_t>>)::error({
        SYS_OSAL_NV_ITEM_INIT,
        "Failed to init data for configuration " + std::to_string(device_configuration),
        parsed_result
      });
    }
    Logger::trace(TAG(), "[Send]: SYS_OSAL_NV_ITEM_INIT <<<");

    return CMD_ERROR(StatusableResponse)::value({true});
  }

  CMD_ERROR(StatusableResponse) ZigbeeNetworkProcessor::writeItem(
    const DeviceConfiguration configuration,
    const std::vector<uint8_t> &buffer,
    const bool auto_initialize
  ) {
    Logger::debug(TAG(), "Writing NV Item [" + std::to_string(configuration) + "]");
    auto length_result = getItemLength(configuration);
    if (!length_result) {
      return CMD_ERROR(SysOsalNvReadResponse<std::vector<uint8_t>>)::error({
        SYS_OSAL_NV_LENGTH,
        "Failed to get length of configuration " + std::to_string(configuration),
        length_result
      });
    }

    Logger::trace(TAG(), "[Send]: SYS_OSAL_NV_WRITE_EXT >>>");
    auto bytes_remaining = buffer.size();
    if (const auto exists = length_result->length > 0; !exists) {
      if (!auto_initialize) {
        throw "Cannot write NV memory item which does not exist";
      }
      Logger::debug(TAG(), "Item doesn't exist; initializing");
      const uint8_t initial_length = buffer.size() > 240 ? 240 : buffer.size();
      const auto init_result = initializeItem(configuration, buffer.size(), buffer.begin(), buffer.begin() + initial_length);
      if (!init_result || !init_result->status) {
        return init_result;
      }
      bytes_remaining -= initial_length;
    }

    while (bytes_remaining > 0) {
      const uint16_t write_length = bytes_remaining > 240 ? 240 : bytes_remaining;
      const uint16_t offset = buffer.size() - bytes_remaining;
      const auto write_result = request(sysOsalNvWriteExt(configuration, offset, buffer.begin() + offset, buffer.begin() + write_length));
      Logger::trace(TAG(), "Writing " + std::to_string(write_length) + " bytes");
      const auto parsed_result = StatusableResponse::parse(write_result);
      if (!parsed_result || !parsed_result->status) {
        return parsed_result;
      }

      bytes_remaining -= write_length;
    }
    Logger::trace(TAG(), "[Send]: SYS_OSAL_NV_WRITE_EXT <<<");

    return CMD_ERROR(StatusableResponse)::value({true});
  }

  CMD_ERROR(SysOsalNvReadResponse<std::vector<uint8_t>>) ZigbeeNetworkProcessor::readItem(
    DeviceConfiguration configuration
  ) {
    Logger::debug(TAG(), "Reading NV Item [" + std::to_string(configuration) + "]");
    const auto length_result = getItemLength(configuration);
    if (!length_result) {
      return CMD_ERROR(SysOsalNvReadResponse<std::vector<uint8_t>>)::error({
        SYS_OSAL_NV_LENGTH,
        "Failed to get length of configuration " + std::to_string(configuration),
        length_result
      });
    }

    if (length_result->length == 0) {
      return CMD_ERROR(SysOsalNvReadResponse<std::vector<uint8_t>>)::value({});
    }

    Logger::trace(TAG(), "[Send]: SYS_OSAL_NV_READ_EXT >>>");
    const auto length = length_result->length;
    auto buffer = std::vector<uint8_t>(length);
    auto offset = 0;
    while (offset < length) {
      const auto result = request(sysOsalNvRead(
        configuration,
        offset));
      const auto parsed_result = SysOsalNvReadResponse<std::vector<uint8_t>>::parse(result);
      if (!parsed_result || !parsed_result->status) {
        return CMD_ERROR(SysOsalNvReadResponse<std::vector<uint8_t>>)::error({
          SYS_OSAL_NV_LENGTH,
          ("Failed to read chunk of data for configuration " + std::to_string(configuration)),
          length_result
        });
      }
      std::ranges::copy(parsed_result->data, buffer.begin() + offset);
      offset += parsed_result->data.size();
    }
    Logger::trace(TAG(), "[Send]: SYS_OSAL_NV_READ_EXT <<<");

    return CMD_ERROR(SysOsalNvReadResponse<std::vector<uint8_t>>)::value({
      {true},
      buffer
    });
  }

  [[nodiscard]] CMD_ERROR(SysOsalNvDeleteResponse) ZigbeeNetworkProcessor::deleteItem(const DeviceConfiguration device_configuration) {
    Logger::debug(TAG(), "Deleting NV Item [" + std::to_string(device_configuration) + "]");
    const auto length_result = getItemLength(device_configuration);
    if (!length_result) {
      return CMD_ERROR(SysOsalNvReadResponse<std::vector<uint8_t>>)::error({
        SYS_OSAL_NV_LENGTH,
        "Failed to get length of configuration " + std::to_string(device_configuration),
        length_result
      });
    }

    Logger::trace(TAG(), "[Send]: SYS_OSAL_NV_DELETE >>>");
    const auto result = request(sysOsalNvDelete(device_configuration, length_result->length));
    Logger::trace(TAG(), "[Recv]: SYS_OSAL_NV_DELETE <<<");
    return SysOsalNvDeleteResponse::parse(result);
  }

  CMD_ERROR(SysOsalNvLengthResponse) ZigbeeNetworkProcessor::getItemLength(const DeviceConfiguration device_configuration) {
    Logger::trace(TAG(), "[Send]: SYS_GET_LENGTH_REQ >>>");
    const auto result = request(sysOsalNvLength(device_configuration));
    Logger::trace(TAG(), "[Recv]: SYS_GET_LENGTH_REQ <<<");
    return SysOsalNvLengthResponse::parse(result);
  }

  Either<SysPingResponse, ZnpCommandError> ZigbeeNetworkProcessor::ping() {
    Logger::info(TAG(), "Pinging adapter");
    Logger::trace(TAG(), "[Send]: SYS_PING >>>");
    const auto result = request(sysPing());
    Logger::trace(TAG(), "[Recv]: SYS_PING <<<");
    return SysPingResponse::parse(result);
  }

  CMD_ERROR(SysResetCallback) ZigbeeNetworkProcessor::reset(const bool soft_reset) {
    Logger::trace(TAG(), "[Send]: SYS_RESET_REQ (async) >>>");
    const auto result = request(sysResetReq(soft_reset));
    Logger::trace(TAG(), "[Recv]: SYS_RESET_IND (async) <<<");
    return SysResetCallback::parse(result);
  }

  CMD_ERROR(SysVersionResponse) ZigbeeNetworkProcessor::getVersion(const bool force_reload) {
    if (!force_reload && version.has_value()) {
      return CMD_ERROR(SysVersionResponse)::value(version.value());
    }

    Logger::trace(TAG(), "[Send]: SYS_VERSION >>>");
    const auto result = request(sysVersion());
    Logger::trace(TAG(), "[Recv]: SYS_VERSION <<<");
    const auto parsed_result = SysVersionResponse::parse(result);
    version = *parsed_result;
    return parsed_result;
  }

  CMD_ERROR(StatusableResponse) ZigbeeNetworkProcessor::setStartupOptions(const ZnpStartupOptions& options) {
    Logger::info(TAG(), "Setting adapter startup options");
    uint8_t configuration = 0x00;
    if (options.clearNetworkFrameCounter) {
      configuration |= ZCD_STARTOPT_CLEAR_NWK_FRAME_COUNTER;
    }
    if (options.clearNetworkState) {
      configuration |= ZCD_STARTOPT_CLEAR_STATE;
    }
    if (options.clearDeviceConfiguration) {
      configuration |= ZCD_STARTOPT_CLEAR_CONFIG;
    }

    const auto result = writeItem(
      ZCD_NV_STARTUP_OPTION,
      std::vector({configuration}));

    return result;
  }

  CMD_ERROR(SysOsalNvReadResponse<ZnpStartupOptions>) ZigbeeNetworkProcessor::getStartupOptions() {
    Logger::info(TAG(), "Getting adapter startup options");
    const auto result = readItem(ZCD_NV_STARTUP_OPTION);
    RETURN_ON_FALSY(result);

    const auto parsed_result = ZnpStartupOptions::parse(result->data, 0, result->data.size());
    RETURN_ON_FALSY(parsed_result);

    return CMD_ERROR(SysOsalNvReadResponse<ZnpStartupOptions>)::value({
      {true},
      *parsed_result
    });
  }

  CMD_ERROR(StatusableResponse) ZigbeeNetworkProcessor::setLogicalType(const LogicalType type) {
    Logger::info(TAG(), "Setting adapter logical type");
    const auto result = writeItem(
      ZCD_NV_LOGICAL_TYPE,
      std::vector<uint8_t>({type}));
    return result;
  }

  CMD_ERROR(StatusableResponse) ZigbeeNetworkProcessor::setPanID(const uint16_t pan_id) {
    Logger::info(TAG(), "Setting adapter PAN ID");
    const auto result = writeItem(
      ZCD_NV_PAN_ID,
      std::vector({
        TO_VECTOR_ARG_LITTLE_ENDIAN_U16(pan_id)
      }));
    return result;
  }

  CMD_ERROR(StatusableResponse) ZigbeeNetworkProcessor::setExtendedPanID(const uint64_t extended_pan_id) {
    Logger::info(TAG(), "Setting adapter extended PAN ID");
    const auto result = writeItem(
      ZCD_NV_EXTENDED_PAN_ID,
      std::vector({
        TO_VECTOR_ARG_LITTLE_ENDIAN_U64(extended_pan_id)
      }));
    return result;
  }

  CMD_ERROR(StatusableResponse) ZigbeeNetworkProcessor::setApsUseExtendedPanID(const uint64_t extended_pan_id) {
    Logger::info(TAG(), "Setting adapter APS use extended PAN ID");
    const auto result = writeItem(
      ZCD_NV_APS_USE_EXT_PANID,
      std::vector({
        TO_VECTOR_ARG_LITTLE_ENDIAN_U64(extended_pan_id)
      }));
    return result;
  }

  CMD_ERROR(StatusableResponse) ZigbeeNetworkProcessor::setPreconfiguredKeysEnabled(const bool enabled) {
    Logger::info(TAG(), "Enabling adapter preconfigured keys");
    const auto result = writeItem(
      ZCD_NV_PRECFGKEYS_ENABLE,
      std::vector({static_cast<uint8_t>(enabled ? 0x01 : 0x00)}));
    return result;
  }

  CMD_ERROR(StatusableResponse) ZigbeeNetworkProcessor::setPreconfiguredKeys(const std::array<uint8_t, 16>& key) {
    Logger::info(TAG(), "Setting adapter preconfigured keys");
    const auto version = getVersion();

    if (std::set<uint8_t>({ZNP_VERSION_ZSTACK_3x0, ZNP_VERSION_ZSTACK_30x}).contains(version->product_id)) {
      const auto result = writeItem(
        ZCD_NV_PRECFGKEYS,
        std::vector(key.begin(), key.end()));
    } else {
      const auto write_prec_keys_result = writeConfiguration(CONF_PROP_PRECFGKEYS, std::vector(key.begin(), key.end()));
      RETURN_ON_FALSY(write_prec_keys_result);

      const auto write_legacy_result = writeItem(ZCD_NV_LEGACY_TCLK_TABLE_START_12, std::vector<uint8_t>{
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x5a, 0x69, 0x67, 0x42, 0x65, 0x65, 0x41, 0x6c,
        0x6c, 0x69, 0x61, 0x6e, 0x63, 0x65, 0x30, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
      });
      RETURN_ON_FALSY(write_legacy_result);
    }

    return StatusableResponse::success();
  }

  CMD_ERROR(StatusableResponse) ZigbeeNetworkProcessor::registerApplicationEndpoint(const AfEndpointDescription& description) {
    Logger::info(TAG(), "Registering application endpoints");
    return StatusableResponse::parse(request(afRegister(description)));
  }

  CMD_ERROR(GetDeviceInfoResponse) ZigbeeNetworkProcessor::getDeviceInfo() {
    Logger::info(TAG(), "Getting device info");
    return GetDeviceInfoResponse::parse(request(utilGetDeviceInfo()));
  }

  CMD_ERROR(StatusableResponse) ZigbeeNetworkProcessor::getActiveEndpoints(const uint16_t destination_address,
    const uint16_t network_address_of_interest
  ) {
    Logger::info(TAG(), "Getting adapter active endpoints");
    Logger::trace(TAG(), "[Send]: ZDO_ACTIVE_EP_REQ >>>");
    const auto result = request(zdoActiveEndpointRequest(zdp_transaction_id, destination_address, network_address_of_interest));
    Logger::trace(TAG(), "[Recv]: ZDO_ACTIVE_EP_REQ <<<");
    return StatusableResponse::parse(result);
  }

  CMD_ERROR(SysOsalNvReadResponse<ZnpMemoryAlignment>) ZigbeeNetworkProcessor::getMemoryAlignment() {
    Logger::info(TAG(), "Setting adapter memory alignment");
    const auto result = readItem(ZCD_NV_NWKKEY);
    RETURN_ON_FALSY(result);

    return CMD_ERROR(SysOsalNvReadResponse<ZnpMemoryAlignment>)::value({
      {true},
      static_cast<ZnpMemoryAlignment>(result->data.size())
    });
  }

  CMD_ERROR(StatusableResponse) ZigbeeNetworkProcessor::setZdoDirectCallback(const bool enabled) {
    Logger::info(TAG(), "Setting ZDO direct callback");
    const auto result = writeItem(
      ZCD_NV_ZDO_DIRECT_CB,
      std::vector({static_cast<uint8_t>(enabled ? 0x01 : 0x00)}));
    return result;
  }

  CMD_ERROR(StatusableResponse) ZigbeeNetworkProcessor::setChannelList(const std::set<ZnpChannelMask> &channel_list) {
    Logger::info(TAG(), "Setting adapter channel list");
    uint32_t bitmask = 0;
    for (const auto channel : channel_list) {
      bitmask |= channel;
    }

    auto channel_buffer = std::vector<uint8_t>(4);
    SET_VECTOR_AT_LITTLE_ENDIAN_U32(channel_buffer, 0, bitmask);
    const auto result = writeItem(
      ZCD_NV_CHANLIST,
      channel_buffer);
    return result;
  }

  CMD_ERROR(StatusableResponse) ZigbeeNetworkProcessor::deleteNetworkInformationBlock() {
    Logger::info(TAG(), "Deleting the network information block");
    const auto result = deleteItem(ZCD_NV_NIB);

    switch (result->status) {
      case SYS_OSAL_NV_DELETE_SUCCESS:
      case SYS_OSAL_NV_DELETE_NO_ACTION_TAKEN:
        return StatusableResponse::success();
      default:
        return StatusableResponse::failure();
    }
  }

  Either<StatusableResponse, ZnpCommandError> ZigbeeNetworkProcessor::setBdbCommissioningChannel(bool is_primary,
    const std::set<ZnpChannelMask> &channels) {
    Logger::trace(TAG(), "[Send]: APP_CNF_BDB_SET_CHANNEL >>>");
    const auto result = request(appCnfBdbSetChannel(is_primary, channels));
    Logger::trace(TAG(), "[Recv]: APP_CNF_BDB_SET_CHANNEL <<<");
    return StatusableResponse::parse(result);
  }

  Either<StatusableResponse, ZnpCommandError> ZigbeeNetworkProcessor::startBdbCommissioning(ZnpCommissioningMode mode) {
    Logger::trace(TAG(), "[Send]: APP_CNF_BDB_START_COMMISSIONING >>>");
    const auto result = request(appCnfBdbStartCommissioning(mode));
    Logger::trace(TAG(), "[Recv]: APP_CNF_BDB_START_COMMISSIONING <<<");
    return StatusableResponse::parse(result);
  }

  Either<StatusableResponse, ZnpCommandError> ZigbeeNetworkProcessor::writeConfiguration(
    const ConfigurationProperty property,
    const std::vector<uint8_t> &data
  ) {
    Logger::trace(TAG(), "[Send]: SAPI_WRITE_CONFIGURATION >>>");
    const auto result = request(sapiWriteConfiguration(property, data));
    Logger::trace(TAG(), "[Recv]: SAPI_WRITE_CONFIGURATION <<<");
    return StatusableResponse::parse(result);
  }

  Either<StatusableResponse, ZnpCommandError> ZigbeeNetworkProcessor::writeConfiguration(
    const ConfigurationProperty property,
    const std::vector<uint8_t>::const_iterator &begin,
    const std::vector<uint8_t>::const_iterator &end
  ) {
    Logger::trace(TAG(), "[Send]: SAPI_WRITE_CONFIGURATION >>>");
    const auto result = request(sapiWriteConfiguration(property, begin, end));
    Logger::trace(TAG(), "[Recv]: SAPI_WRITE_CONFIGURATION <<<");
    return StatusableResponse::parse(result);
  }
}
