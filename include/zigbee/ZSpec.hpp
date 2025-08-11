#include <cstdint>
/**
 * Tags: zigbee
 * References:
 * [1]: https://github.com/Koenkk/zigbee-herdsman/blob/master/src/zspec/consts.ts
 */
namespace lcl::zigbee {
  /**
   * The network ID of the coordinator in a ZigBee network is 0x0000. [1]
   */
  constexpr uint16_t COORDINATOR_ADDRESS = 0x0000;

  /**
   * A blank IEEE address? [1]
   */
  constexpr uint64_t BLANK_EUI64 = 0xFFFFFFFFFFFFFFFF;
  /**
    * A distinguished network ID that will never be assigned to any node. It is used to indicate the absence of a node
    * ID. [1]
    */
  constexpr uint16_t NULL_NODE_ID = 0xFFFF;
  /**
   * A distinguished binding index used to indicate the absence of a binding. [1]
   */
  constexpr uint8_t NULL_BINDING = 0xFF;
}