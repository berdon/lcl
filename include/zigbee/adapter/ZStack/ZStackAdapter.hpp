/**
* Tags: provider, zigbee, zstack
 */
#pragma once
#include <string_view>
#include <async/task.hpp>

#include <zigbee/adapter/IAdapter.hpp>

#include "ZigbeeNetworkProcessor.hpp"

namespace lcl::zigbee::adapter {
  using namespace zstack;

  class ZStackAdapter : public IAdapter {
    ZigbeeNetworkProcessor network_processor;
    std::set<Capability> capabilities;
    std::thread read_thread;

    std::optional<SysVersionResponse> version = std::nullopt;
    std::optional<ZnpMemoryAlignment> memory_alignment = std::nullopt;

  public:
    explicit ZStackAdapter(const std::string &connection, ConnectionOptions options);
    ~ZStackAdapter() override = default;

    [[nodiscard]] TaskErrean<AdapterError> start() override;
  private:
    TaskErrean<ZStackError> initializeProcessor();
    TaskErrean<ZStackError> beginCommissioning(const ZnpNetworkOptions &network_options, bool fail_on_collision, bool write_configuration_flag);
    TaskErrean<ZStackError> updateCommissioningItems(const ZnpNetworkOptions &network_options);
    TaskErrean<ZStackError> clearAdapter();

    /**
     * Configures the [ZigbeeNetworkProcessor] to clear internal state then restarts.
     * @return true if the process succeeded
     */
    TaskErrean<ZStackError> clearZigbeeNetworkProcessor();
  };
} // namespace lcl::provider