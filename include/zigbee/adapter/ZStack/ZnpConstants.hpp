/**
 * Tags: zigbee, z-stack
 * References:
 * [1]: https://csa-iot.org/wp-content/uploads/2023/04/05-3474-23-csg-zigbee-specification-compressed.pdf
 * [2]: https://github.com/Koenkk/zigbee-herdsman/blob/master/src/adapter/z-stack/constants/common.ts#L143
 * [3]: /Applications/ti/simplelink_cc13xx_cc26xx_sdk_8_30_01_01/source/ti/zstack/mt/mt.h (see Z-Stack readme)
 */

#pragma once
#include <cstdint>

namespace lcl::zigbee::adapter::zstack {
  enum ZnpProductVersion {
    ZNP_VERSION_ZSTACK_12 = 0,
    ZNP_VERSION_ZSTACK_3x0 = 1,
    ZNP_VERSION_ZSTACK_30x = 2,
  };

  /**
   * MonitorTest command IDs [3]
   */
  enum MtCommandId: uint8_t {
    MT_APP_CNF_SET_DEFAULT_REMOTE_ENDDEVICE_TIMEOUT   = 0x01,
    MT_APP_CNF_SET_ENDDEVICETIMEOUT                   = 0x02,
    MT_APP_CNF_SET_ALLOWREJOIN_TC_POLICY              = 0x03,
    MT_APP_CNF_BDB_ADD_INSTALLCODE                    = 0x04,
    MT_APP_CNF_BDB_START_COMMISSIONING                = 0x05,
    MT_APP_CNF_BDB_SET_JOINUSESINSTALLCODEKEY         = 0x06,
    MT_APP_CNF_BDB_SET_ACTIVE_DEFAULT_CENTRALIZED_KEY = 0x07,
    MT_APP_CNF_BDB_SET_CHANNEL                        = 0x08,
    MT_APP_CNF_BDB_SET_TC_REQUIRE_KEY_EXCHANGE        = 0x09,
    MT_APP_CNF_BDB_RECOVER_NWK                        = 0x0A,
    MT_APP_CNF_BDB_SET_DEFAULT_PARENT_INFO            = 0x0B,
    MT_APP_CNF_SET_POLL_RATE_TYPE                     = 0x0C,
    MT_APP_CNF_BDB_COMMISSIONING_NOTIFICATION         = 0x80,
    MT_APP_CNF_SET_NWK_FRAME_COUNTER                  = 0xFF,

    SAPI_WRITE_CONFIGURATION = 0x05,

    SYS_RESET_REQ         = 0x00,
    SYS_PING              = 0x01,
    SYS_VERSION           = 0x02,
    SYS_OSAL_NV_ITEM_INIT = 0x07,
    SYS_OSAL_NV_READ      = 0x08,
    SYS_OSAL_NV_WRITE     = 0x09,
    SYS_OSAL_NV_DELETE    = 0x12,
    SYS_OSAL_NV_LENGTH    = 0x13,
    SYS_OSAL_NV_READ_EXT  = 0x1C,
    SYS_OSAL_NV_WRITE_EXT = 0x1D,
    SYS_RESET_IND         = 0x80,

    UTIL_SET_PAN_ID       = 0x02,
    UTIL_SET_CHANNELS     = 0x03,
    UTIL_GET_DEVICE_INFO  = 0x00,

    AF_REGISTER           = 0x00,

    /**
     * The Active_EP_req command is generated from a local device wishing to acquire the list of endpoints on a
     * remote device with simple descriptors. This command SHALL be unicast either to the remote device itself or to an
     * alternative device that contains the discovery information of the remote device. [1]
     */
    ZDO_ACTIVE_EP_REQ     = 0x05
  };

  enum Subsystem: uint8_t {
    RESERVED = 0,

    /**
     * The SYS interface (MT_SYS) provides the application processor with a low level interface to the ZNP hardware and
     * software.
     */
    SUBSYSTEM_SYS = 1,
    SUBSYSTEM_MAC = 2,
    SUBSYSTEM_NWK = 3,

    /**
     * The AF (MT_AF) and ZDO (MT_ZDO) interfaces feature the complete ZigBee interface and can be used to create a full
     * range of ZigBee compliant applications. The AF (Application Framework) interface allows the application processor
     * to register its application with the ZNP and send and receive data.
     */
    SUBSYSTEM_AF = 4,

    /**
     * The AF (MT_AF) and ZDO (MT_ZDO) interfaces feature the complete ZigBee interface and can be used to create a full
     * range of ZigBee compliant applications. The ZDO (ZigBee Device Object) interface provides various ZigBee
     * management functions like device and service discovery.
     */
    SUBSYSTEM_ZDO = 5,
    SUBSYSTEM_SAPI = 6,

    /**
     * The UTIL (MT_UTIL) interface provides support functionalities such as setting PAN-ID, getting device info,
     * getting NV info, subscribing callbacks…etc.
     */
    SUBSYSTEM_UTIL = 7,
    SUBSYSTEM_DEBUG = 8,

    /**
     * The APP CONF (MT_APP_CNF) interface provides support for BDB functionality such as set Install Codes, Primary or
     * Secondary Channel, trigger different commissioning methods and other Trust Center configurations.
     */
    SUBSYSTEM_APP = 9,
    SUBSYSTEM_APP_CNF = 15,
    SUBSYSTEM_GREENPOWER = 21,
  };

  enum Type: uint8_t {
    POLL = 0,
    SREQ = 1,
    AREQ = 2,
    SRSP = 3,
  };

  enum Capability: uint16_t {
    MT_CAP_SYS    = 0x0001,
    MT_CAP_MAC    = 0x0002,
    MT_CAP_NWK    = 0x0004,
    MT_CAP_AF     = 0x0008,
    MT_CAP_ZDO    = 0x0010,
    MT_CAP_SAPI   = 0x0020,
    MT_CAP_UTIL   = 0x0040,
    MT_CAP_DEBUG  = 0x0080,
    MT_CAP_APP    = 0x0100,
    MT_CAP_ZOAD   = 0x1000,
  };

  enum SysResetReason: uint8_t {
    SYS_RESET_REASON_POWER_UP = 0x00,
    SYS_RESET_REASON_EXTERNAL = 0x01,
    SYS_RESET_REASON_WATCH_DOG = 0x02,
  };

  enum LogicalType: uint8_t {
    LOGICAL_TYPE_COORDINATOR = 0x00,
    LOGICAL_TYPE_ROUTER = 0x01,
    LOGICAL_TYPE_END_DEVICE = 0x02,
  };

  enum DeviceConfiguration: uint16_t {
    /**
     * Configuration ID: 0x0003; Size: 1 byte; Default value: 0
     * This parameter controls the device startup options. This is a bit mask of the following values
     * Bit position 	              7 	                     6-2 	          1 	                    0
     * Description 	ZCD_STARTOPT_CLEAR_NWK_FRAME_COUNTER 	Reserved 	STARTOPT_CLEAR_STATE 	STARTOPT_CLEAR_CONFIG
     */
    ZCD_NV_STARTUP_OPTION = 0x0003,

    /**
     * Configuration ID: 0x0083; Size: 2 bytes; Default value: 0xFFFF
     * This parameter identifies the ZigBee network. This should be set to a value between 0 and 0x3FFF. Networks that
     * exist in the same vicinity must have different values for this parameter. It can be set to a special value of
     * 0xFFFF to indicate “don’t care”.
     */
    ZCD_NV_PAN_ID         = 0x0083,

    /**
     * Configuration ID: 0x0087; Size: 1 byte; Default value: 0x00
     * This is the logical type of the device in the ZigBee network. This can be set to a COORDINATOR (0x00), ROUTER (0x01) or ENDDEVICE (0x02).
     * Note: This parameter is read by the ZNP device immediately when it powers up after a reset.
     */
    ZCD_NV_LOGICAL_TYPE   = 0x0087,

    /**
     * Configuration ID: 0x008F; Size: 1 byte; Default value: FALSE
     * This configures the manner in which ZDO responses (hereby referred to as callbacks) are issued to the host
     * processor. By default, this item is set to FALSE, which means that the host processor must use the
     * ZDO_MSG_CB_REGISTER command to subscribe to a specific ZDO callback in order to receive it. The ZDO callback is
     * then conveyed as part of the ZDO_MSG_CB_INCOMING command. If ZCD_NV_ZDO_DIRECT_CB is set TRUE, then the host
     * processor will receive the “verbose” response. For example, the host processor would receive the
     * ZDO_IEEE_ADDR_RSP command in response to ZDO_IEEE_ADDR_REQ.
     */
    ZCD_NV_ZDO_DIRECT_CB  = 0x008F,

    ZCD_NV_NWKKEY = 0x82,
    ZCD_NV_NIB = 0x21,
    ZCD_NV_CHANLIST = 0x84,
    ZCD_NV_EXTENDED_PAN_ID = 45,
    ZCD_NV_APS_USE_EXT_PANID = 71,
    ZCD_NV_PRECFGKEYS = 0x62,
    ZCD_NV_PRECFGKEYS_ENABLE = 0x63,

    // TODO: Used for legacy 1.2 stack [2]
    ZCD_NV_LEGACY_TCLK_TABLE_START_12 = 257
  };

  enum ConfigurationProperty: uint8_t {
    CONF_PROP_PRECFGKEYS = ZCD_NV_PRECFGKEYS
  };

  enum StartupOptionBitMask: uint8_t {
    /**
     * If this option is set, then the network frame counter is cleared for all networks.
     * Note: This should be use only for debug purposes as the network frame counters must be persistent, even after
     * Factory New resets. The usage of this option during the operation in the networks may lead to undesired
     * behaviour, such as get the ZNP device ignored by other devices in the network.
     */
    ZCD_STARTOPT_CLEAR_NWK_FRAME_COUNTER = 0b10000000,
    /**
     * If this option is set, the device will clear its previous network state (which would exist if the device had been
     * operating on a network prior to the reset). This is typically used during application development. During regular
     * device operation, this flag is typically not set, so that an accidental device reset will not cause loss of
     * network state.
     * Note: The ZNP device has two kinds of information stored in non-volatile memory. The configuration parameters
     * (listed in this section) and network state information.
     * The configuration parameters are configured by the user before start of ZigBee operation.
     * The network state information is collected by the device after it joins a network and creates bindings etc.
     * (at runtime). This is not set by the application processor. This information is stored so that if the device were
     * to reset accidentally, it can restore itself without going through all the network joining and binding process
     * again.
     * If the application processor does not wish to continue operating in the previous ZigBee network, it needs to
     * instruct the ZNP device to clear the network state information and start again based on the configuration
     * parameters. This is done by setting the STARTOPT_CLEAR_STATE bit in the startup option.
     */
    ZCD_STARTOPT_CLEAR_STATE             = 0b00000010,
    /**
     * If this option is set, the device will overwrite all the configuration parameters (except this one) with the
     * “default” values that it is programmed with. This is used to erase the existing configuration and bring the
     * device into a known state.
     * Note: The STARTOPT_CLEAR_CONFIG bit is read by the ZNP device immediately when it powers up after a reset.
     * When the configuration parameters are restored to defaults, the ZCD_NV_STARTUP_OPTION itself is not restored
     * except for clearing the STARTOPT_CLEAR_CONFIG bit.
     */
    ZCD_STARTOPT_CLEAR_CONFIG            = 0b00000001,
  };

  enum DeviceState: uint8_t {
    DEVICE_STATE_INITIALIZED_NOT_STARTED    = 0x00,
    DEVICE_STATE_INITIALIZED_NOT_CONNECTED  = 0x01,
    DEVICE_STATE_DISCOVERING                = 0x02,
    DEVICE_STATE_JOINING                    = 0x03,
    DEVICE_STATE_REJOINING                  = 0x04,
    DEVICE_STATE_JOINED                     = 0x05,
    DEVICE_STATE_STARTED_AS_DEVICE          = 0x06,
    DEVICE_STATE_STARTED_AS_ROUTER          = 0x07,
    DEVICE_STATE_STARTING_AS_COORDINATOR    = 0x08,
    DEVICE_STATE_STARTED_AS_COORDINATOR     = 0x09,
    DEVICE_STATE_LOST_INFO_ABOUT_PARENT     = 0x0A
  };

  enum SysOsalNvDeleteStatus: uint8_t {
    SYS_OSAL_NV_DELETE_SUCCESS          = 0x00,
    SYS_OSAL_NV_DELETE_NO_ACTION_TAKEN  = 0x09,
    SYS_OSAL_NV_DELETE_FAILURE          = 0x0A,
    SYS_OSAL_NV_DELETE_BAD_LENGTH       = 0x0C
  };

  enum ZnpChannel: uint8_t {
    ZNP_CHANNEL_11 = 11,
    ZNP_CHANNEL_12 = 12,
    ZNP_CHANNEL_13 = 13,
    ZNP_CHANNEL_14 = 14,
    ZNP_CHANNEL_15 = 15,
    ZNP_CHANNEL_16 = 16,
    ZNP_CHANNEL_17 = 17,
    ZNP_CHANNEL_18 = 18,
    ZNP_CHANNEL_19 = 19,
    ZNP_CHANNEL_20 = 20,
    ZNP_CHANNEL_21 = 21,
    ZNP_CHANNEL_22 = 22,
    ZNP_CHANNEL_23 = 23,
    ZNP_CHANNEL_24 = 24,
    ZNP_CHANNEL_25 = 25,
    ZNP_CHANNEL_26 = 26,
  };

  enum ZnpChannelMask: uint32_t {
    ZNP_CHANNEL_MASK_11 = 0x00000800,
    ZNP_CHANNEL_MASK_12 = 0x00001000,
    ZNP_CHANNEL_MASK_13 = 0x00002000,
    ZNP_CHANNEL_MASK_14 = 0x00004000,
    ZNP_CHANNEL_MASK_15 = 0x00008000,
    ZNP_CHANNEL_MASK_16 = 0x00010000,
    ZNP_CHANNEL_MASK_17 = 0x00020000,
    ZNP_CHANNEL_MASK_18 = 0x00040000,
    ZNP_CHANNEL_MASK_19 = 0x00080000,
    ZNP_CHANNEL_MASK_20 = 0x00100000,
    ZNP_CHANNEL_MASK_21 = 0x00200000,
    ZNP_CHANNEL_MASK_22 = 0x00400000,
    ZNP_CHANNEL_MASK_23 = 0x00800000,
    ZNP_CHANNEL_MASK_24 = 0x01000000,
    ZNP_CHANNEL_MASK_25 = 0x02000000,
    ZNP_CHANNEL_MASK_26 = 0x04000000,
  };

  enum ZnpCommissioningMode: uint8_t {
    ZNP_COMMISSIONING_MODE_INITIALIZING         = 0x00,
    ZNP_COMMISSIONING_MODE_TOUCH_LINK           = 0x01,
    ZNP_COMMISSIONING_MODE_NETWORK_STEERING     = 0x02,
    ZNP_COMMISSIONING_MODE_NETWORK_FORMATION    = 0x04,
    ZNP_COMMISSIONING_MODE_FINDING_AND_BINDING  = 0x08,
  };
}
