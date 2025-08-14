/**
* Tags: zigbee, adapter
 */
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>

#include "result/Either.hpp"
#include "result/TaskErrean.hpp"

namespace lcl::zigbee::adapter {
  enum ErrorCodeAdapter: uint8_t {
    EC_ADAPTER_UNKNOWN,
    EC_ADAPTER_FAILED_TO_START
  };

  struct AdapterError: Error<ErrorCodeAdapter, void> {
    using ErrorCodeType = ErrorCodeAdapter;
    using ValueType = void;
  };

  class IAdapter {
  public:
    virtual ~IAdapter() = default;
    virtual TaskErrean<AdapterError> start() = 0;
  };
} // namespace lcl::provider
