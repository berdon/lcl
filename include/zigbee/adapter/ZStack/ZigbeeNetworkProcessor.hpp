#pragma once
#include "ZnpTypes.hpp"
#include "asio/serial_port.hpp"
#include "asio/ip/tcp.hpp"
#include "async/task.hpp"
#include "result/Either.hpp"
#include "zigbee/adapter/IAdapter.hpp"

namespace lcl::zigbee::adapter::zstack {
  constexpr uint8_t SOF = 0xFE;

  class ZigbeeNetworkProcessor {
    ConnectionUri connection_uri;
    ConnectionOptions connection_options;

    /**
     * A valid TCP socket if {@link connection_uri.connection_type} is {@link ConnectionType::CONNECTION_TYPE_TCP}.
     */
    std::optional<asio::ip::tcp::socket> socket;

    /**
     * A valid Serial Port if {@link connection_uri.connection_type} is {@link ConnectionType::CONNECTION_TYPE_USB}.
     */
    std::optional<asio::serial_port> serial_port;
    asio::io_context io_context;

    /**
     * Thread used for asynchronously reading device responses.
     */
    std::thread read_thread;

    /**
     * A queue of read synchronous responses that haven't been handled yet.
     */
    std::vector<RawZnpResponse> responses;

    /**
     * A lock guarding {@link responses}.
     */
    mutable std::mutex response_mutex;

    /**
     * TODO: The current ZDP transaction ID.
     */
    const uint8_t zdp_transaction_id = 0x00;

    /**
     * The current adapter's ZNP version.
     */
    std::optional<SysVersionResponse> version = std::nullopt;

  public:
    [[nodiscard]] ZigbeeNetworkProcessor(const std::string &connection_string, ConnectionOptions connection_options);
    ~ZigbeeNetworkProcessor();

    TaskErrean<AdapterError> connect();
    void startAsyncRead();
    void asyncRead();
    void handleRead(const std::array<uint8_t, 256>& buffer, uint8_t size, std::vector<uint8_t> &pending_buffer);

    bool isInterPan() const {
      return is_inter_pan;
    }

    TaskEitherCmd<SysPingResponse> ping();
    TaskEitherCmd<SysResetCallback> reset(bool soft_reset = true);
    TaskEitherCmd<SysVersionResponse> getVersion(bool force_reload = false);
    TaskEitherCmd<StatusableResponse> setStartupOptions(const ZnpStartupOptions& options);
    TaskEitherCmd<SysOsalNvReadResponse<ZnpStartupOptions>> getStartupOptions();
    TaskEitherCmd<StatusableResponse> setLogicalType(LogicalType);
    TaskEitherCmd<StatusableResponse> setPanID(uint16_t pan_id);
    TaskEitherCmd<StatusableResponse> setExtendedPanID(uint64_t extended_pan_id);
    TaskEitherCmd<StatusableResponse> setApsUseExtendedPanID(uint64_t extended_pan_id);
    TaskEitherCmd<StatusableResponse> setPreconfiguredKeysEnabled(bool enabled);
    TaskEitherCmd<StatusableResponse> setPreconfiguredKeys(const std::array<uint8_t, 16>& key);
    TaskEitherCmd<StatusableResponse> registerApplicationEndpoint(const AfEndpointDescription& description);
    TaskEitherCmd<GetDeviceInfoResponse> getDeviceInfo();
    TaskEitherCmd<StatusableResponse> getActiveEndpoints(uint16_t destination_address, uint16_t network_address_of_interest);
    TaskEitherCmd<SysOsalNvReadResponse<ZnpMemoryAlignment>> getMemoryAlignment();
    TaskEitherCmd<StatusableResponse> setZdoDirectCallback(bool enabled);
    TaskEitherCmd<StatusableResponse> setChannelList(const std::set<ZnpChannelMask> &channel_list);
    TaskEitherCmd<StatusableResponse> deleteNetworkInformationBlock();
    TaskEitherCmd<StatusableResponse> setBdbCommissioningChannel(bool is_primary, const std::set<ZnpChannelMask> &channels);
    TaskEitherCmd<StatusableResponse> startBdbCommissioning(ZnpCommissioningMode mode);

  private:
    bool is_inter_pan = false;

    void configureUsbDevice();
    void configureTcpDevice();

    /**
     * Sends a ZNP request to the connected adapter.
     * TODO: Error handling
     *
     * @param command The command to send
     * @return A response for the request
     */
    [[nodiscard]] async::Task<RawZnpResponse> sendRequest(const ZnpCommand& command);

    /**
     * Initializes a Z-Stack non-volatile memory "item".
     *
     * @param configuration The configuration ID
     * @param total_length The size of the data to be written
     * @param begin A starting vector iterator
     * @param end An ending vector iterator
     * @return A {@link Either<StatusableResponse, ZnpCommandError>}
     */
    [[nodiscard]] TaskEitherCmd<StatusableResponse> initializeItem(DeviceConfiguration configuration,
                                                               uint16_t total_length,
                                                               const std::vector<uint8_t>::const_iterator &begin,
                                                               const std::vector<uint8_t>::const_iterator &end);

    /**
     * Writes data to a Z-Stack non-volatile memory "item". This can handle initializing the memory if need be.
     *
     * @param configuration The configuration ID
     * @param auto_initialize Whether to automatically initialize the data if the entry doesn't exist
     * @return A {@link Either<StatusableResponse, ZnpCommandError>}
     */
    [[nodiscard]] TaskEitherCmd<StatusableResponse> writeItem(DeviceConfiguration configuration,
                                                          const std::vector<uint8_t> &,
                                                          bool auto_initialize = true);

    /**
     * Reads data from a Z-Stack non-volatile memory "item".
     * @param configuration The configuration ID
     * @return A {@link Either<StatusableResponse, ZnpCommandError>}
     */
    [[nodiscard]] TaskEitherCmd<SysOsalNvReadResponse<std::vector<uint8_t>>> readItem(DeviceConfiguration configuration);

    /**
     * Deletes a Z-Stack non-volatile memory "item".
     *
     * @param configuration The configuration ID
     * @return A {@link Either<SysOsalNvDeleteResponse, ZnpCommandError>}
     */
    [[nodiscard]] TaskEitherCmd<SysOsalNvDeleteResponse> deleteItem(DeviceConfiguration configuration);

    /**
     * Returns the length of a Z-Stack non-volatile memory "item".
     *
     * @param configuration The configuration ID
     * @return A {@link Either<SysOsalNvLengthResponse, ZnpCommandError>}
     */
    [[nodiscard]] TaskEitherCmd<SysOsalNvLengthResponse> getItemLength(DeviceConfiguration configuration);

    /**
     *
     * @param property 
     * @param data
     * @return
     */
    [[nodiscard]] TaskEitherCmd<StatusableResponse> writeConfiguration(ConfigurationProperty property,
                                                                   const std::vector<uint8_t> &data);

    [[nodiscard]] TaskEitherCmd<StatusableResponse> writeConfiguration(ConfigurationProperty property,
                                                                   const std::vector<uint8_t>::const_iterator &begin,
                                                                   const std::vector<uint8_t>::const_iterator &end);
  };
}
