//
// Created by Austin Hanson on 8/7/25.
//

#ifndef ZIGBEEERROR_H
#define ZIGBEEERROR_H
#include <cstdint>

namespace lcl::zigbee {
  enum ErrorCodeZigbee: uint8_t {
    EC_ZIGBEE_INVALID_PAN_ID,
  };

  struct CommissioningError {

  };
}

#endif //ZIGBEEERROR_H
