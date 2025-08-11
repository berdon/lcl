# Lcl - A WIP smart-home...thing

A vibe-coded and hand tuned attempt at a smart-home "software" suite.

This is very much a work in-progress with currently only dev/WIP support for evaluating the Zigbee Z-Stack adapter:

```bash
lcl test-zstack
```

##### Example Output:

```bash
/Users/auhanson/workspace/github/lcl/cmake-build-debug/lcl test-zstack
[08/11 06:13:29.710] [INFO] [ZigbeeNetworkProcessor] Pinging adapter
[08/11 06:13:29.711] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_PING >>>
[08/11 06:13:29.711] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:13:29.711] [TRACE] [ZigbeeNetworkProcessor] FE 00 21 01 20 | 254 0 33 1 32
[08/11 06:16:35.330] [TRACE] [ZigbeeNetworkProcessor] Reading [7]
[08/11 06:16:35.330] [TRACE] [ZigbeeNetworkProcessor] FE 02 61 01 59 06 3D | 254 2 97 1 89 6 61
[08/11 06:16:35.330] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 01, 01, 03, 59 06
[08/11 06:16:35.341] [TRACE] [ZigbeeNetworkProcessor] [Recv]: SYS_PING <<<
[08/11 06:16:35.341] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_VERSION >>>
[08/11 06:16:35.341] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:35.341] [TRACE] [ZigbeeNetworkProcessor] FE 00 21 02 23 | 254 0 33 2 35
[08/11 06:16:35.346] [TRACE] [ZigbeeNetworkProcessor] Reading [15]
[08/11 06:16:35.346] [TRACE] [ZigbeeNetworkProcessor] FE 0A 61 02 02 01 02 07 01 14 64 34 01 00 2B | 254 10 97 2 2 1 2 7 1 20 100 52 1 0 43
[08/11 06:16:35.346] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 02, 01, 03, 02 01 02 07 01 14 64 34 01 00
[08/11 06:16:35.352] [TRACE] [ZigbeeNetworkProcessor] [Recv]: SYS_VERSION <<<
[08/11 06:16:35.352] [INFO] [ZigbeeNetworkProcessor] Setting adapter memory alignment
[08/11 06:16:35.352] [DEBUG] [ZigbeeNetworkProcessor] Reading NV Item [130]
[08/11 06:16:35.352] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_GET_LENGTH_REQ >>>
[08/11 06:16:35.352] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:35.352] [TRACE] [ZigbeeNetworkProcessor] FE 02 21 13 82 00 B2 | 254 2 33 19 130 0 178
[08/11 06:16:35.357] [TRACE] [ZigbeeNetworkProcessor] Reading [7]
[08/11 06:16:35.357] [TRACE] [ZigbeeNetworkProcessor] FE 02 61 13 18 00 68 | 254 2 97 19 24 0 104
[08/11 06:16:35.357] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 13, 01, 03, 18 00
[08/11 06:16:35.364] [TRACE] [ZigbeeNetworkProcessor] [Recv]: SYS_GET_LENGTH_REQ <<<
[08/11 06:16:35.365] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_READ_EXT >>>
[08/11 06:16:35.365] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:35.365] [TRACE] [ZigbeeNetworkProcessor] FE 03 21 08 82 00 00 A8 | 254 3 33 8 130 0 0 168
[08/11 06:16:35.371] [TRACE] [ZigbeeNetworkProcessor] Reading [31]
[08/11 06:16:35.371] [TRACE] [ZigbeeNetworkProcessor] FE 1A 61 08 00 18 00 01 03 05 07 09 0B 0D 0F 00 02 04 06 08 0A 0C 0D E7 01 00 08 29 00 00 AF | 254 26 97 8 0 24 0 1 3 5 7 9 11 13 15 0 2 4 6 8 10 12 13 231 1 0 8 41 0 0 175
[08/11 06:16:35.371] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 08, 01, 03, 00 18 00 01 03 05 07 09 0B 0D 0F 00 02 04 06 08 0A 0C 0D E7 01 00 08 29 00 00
[08/11 06:16:35.375] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_READ_EXT <<<
[08/11 06:16:35.375] [INFO] [USER] Deleting network block information
[08/11 06:16:35.376] [INFO] [ZigbeeNetworkProcessor] Deleting the network information block
[08/11 06:16:35.376] [DEBUG] [ZigbeeNetworkProcessor] Deleting NV Item [33]
[08/11 06:16:35.376] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_GET_LENGTH_REQ >>>
[08/11 06:16:35.376] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:35.376] [TRACE] [ZigbeeNetworkProcessor] FE 02 21 13 21 00 11 | 254 2 33 19 33 0 17
[08/11 06:16:35.380] [TRACE] [ZigbeeNetworkProcessor] Reading [7]
[08/11 06:16:35.380] [TRACE] [ZigbeeNetworkProcessor] FE 02 61 13 74 00 04 | 254 2 97 19 116 0 4
[08/11 06:16:35.380] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 13, 01, 03, 74 00
[08/11 06:16:35.388] [TRACE] [ZigbeeNetworkProcessor] [Recv]: SYS_GET_LENGTH_REQ <<<
[08/11 06:16:35.388] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_DELETE >>>
[08/11 06:16:35.388] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:35.388] [TRACE] [ZigbeeNetworkProcessor] FE 04 21 12 21 00 74 00 62 | 254 4 33 18 33 0 116 0 98
[08/11 06:16:35.399] [TRACE] [ZigbeeNetworkProcessor] Reading [6]
[08/11 06:16:35.399] [TRACE] [ZigbeeNetworkProcessor] FE 01 61 12 00 72 | 254 1 97 18 0 114
[08/11 06:16:35.399] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 12, 01, 03, 00
[08/11 06:16:35.399] [TRACE] [ZigbeeNetworkProcessor] [Recv]: SYS_OSAL_NV_DELETE <<<
[08/11 06:16:35.399] [INFO] [ZigbeeNetworkProcessor] Setting adapter startup options
[08/11 06:16:35.399] [DEBUG] [ZigbeeNetworkProcessor] Writing NV Item [3]
[08/11 06:16:35.399] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_GET_LENGTH_REQ >>>
[08/11 06:16:35.399] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:35.399] [TRACE] [ZigbeeNetworkProcessor] FE 02 21 13 03 00 33 | 254 2 33 19 3 0 51
[08/11 06:16:35.404] [TRACE] [ZigbeeNetworkProcessor] Reading [7]
[08/11 06:16:35.404] [TRACE] [ZigbeeNetworkProcessor] FE 02 61 13 01 00 71 | 254 2 97 19 1 0 113
[08/11 06:16:35.404] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 13, 01, 03, 01 00
[08/11 06:16:35.412] [TRACE] [ZigbeeNetworkProcessor] [Recv]: SYS_GET_LENGTH_REQ <<<
[08/11 06:16:35.412] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT >>>
[08/11 06:16:35.412] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:35.412] [TRACE] [ZigbeeNetworkProcessor] FE 07 21 1D 03 00 00 00 01 00 03 3A | 254 7 33 29 3 0 0 0 1 0 3 58
[08/11 06:16:35.417] [TRACE] [ZigbeeNetworkProcessor] Reading [6]
[08/11 06:16:35.417] [TRACE] [ZigbeeNetworkProcessor] FE 01 61 1D 00 7D | 254 1 97 29 0 125
[08/11 06:16:35.417] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 1D, 01, 03, 00
[08/11 06:16:35.423] [TRACE] [ZigbeeNetworkProcessor] Writing 1 bytes
[08/11 06:16:35.423] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT <<<
[08/11 06:16:35.423] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_RESET_REQ (async) >>>
[08/11 06:16:35.423] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:35.423] [TRACE] [ZigbeeNetworkProcessor] FE 01 41 00 01 41 | 254 1 65 0 1 65
[08/11 06:16:35.433] [TRACE] [ZigbeeNetworkProcessor] Reading [1]
[08/11 06:16:35.433] [TRACE] [ZigbeeNetworkProcessor] 00 | 0
[08/11 06:16:37.535] [TRACE] [ZigbeeNetworkProcessor] Reading [11]
[08/11 06:16:37.535] [TRACE] [ZigbeeNetworkProcessor] FE 06 41 80 00 02 01 02 07 01 C0 | 254 6 65 128 0 2 1 2 7 1 192
[08/11 06:16:37.535] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 80, 01, 02, 00 02 01 02 07 01
[08/11 06:16:37.541] [TRACE] [ZigbeeNetworkProcessor] [Recv]: SYS_RESET_IND (async) <<<
[08/11 06:16:37.541] [INFO] [ZigbeeNetworkProcessor] Setting adapter startup options
[08/11 06:16:37.541] [DEBUG] [ZigbeeNetworkProcessor] Writing NV Item [3]
[08/11 06:16:37.541] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_GET_LENGTH_REQ >>>
[08/11 06:16:37.541] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.541] [TRACE] [ZigbeeNetworkProcessor] FE 02 21 13 03 00 33 | 254 2 33 19 3 0 51
[08/11 06:16:37.546] [TRACE] [ZigbeeNetworkProcessor] Reading [7]
[08/11 06:16:37.546] [TRACE] [ZigbeeNetworkProcessor] FE 02 61 13 01 00 71 | 254 2 97 19 1 0 113
[08/11 06:16:37.546] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 13, 01, 03, 01 00
[08/11 06:16:37.553] [TRACE] [ZigbeeNetworkProcessor] [Recv]: SYS_GET_LENGTH_REQ <<<
[08/11 06:16:37.553] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT >>>
[08/11 06:16:37.553] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.553] [TRACE] [ZigbeeNetworkProcessor] FE 07 21 1D 03 00 00 00 01 00 00 39 | 254 7 33 29 3 0 0 0 1 0 0 57
[08/11 06:16:37.558] [TRACE] [ZigbeeNetworkProcessor] Reading [6]
[08/11 06:16:37.558] [TRACE] [ZigbeeNetworkProcessor] FE 01 61 1D 00 7D | 254 1 97 29 0 125
[08/11 06:16:37.558] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 1D, 01, 03, 00
[08/11 06:16:37.565] [TRACE] [ZigbeeNetworkProcessor] Writing 1 bytes
[08/11 06:16:37.565] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT <<<
[08/11 06:16:37.565] [INFO] [USER] Clearing startup options
[08/11 06:16:37.565] [INFO] [ZigbeeNetworkProcessor] Setting adapter startup options
[08/11 06:16:37.565] [DEBUG] [ZigbeeNetworkProcessor] Writing NV Item [3]
[08/11 06:16:37.565] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_GET_LENGTH_REQ >>>
[08/11 06:16:37.565] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.565] [TRACE] [ZigbeeNetworkProcessor] FE 02 21 13 03 00 33 | 254 2 33 19 3 0 51
[08/11 06:16:37.569] [TRACE] [ZigbeeNetworkProcessor] Reading [7]
[08/11 06:16:37.569] [TRACE] [ZigbeeNetworkProcessor] FE 02 61 13 01 00 71 | 254 2 97 19 1 0 113
[08/11 06:16:37.569] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 13, 01, 03, 01 00
[08/11 06:16:37.575] [TRACE] [ZigbeeNetworkProcessor] [Recv]: SYS_GET_LENGTH_REQ <<<
[08/11 06:16:37.575] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT >>>
[08/11 06:16:37.575] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.575] [TRACE] [ZigbeeNetworkProcessor] FE 07 21 1D 03 00 00 00 01 00 00 39 | 254 7 33 29 3 0 0 0 1 0 0 57
[08/11 06:16:37.580] [TRACE] [ZigbeeNetworkProcessor] Reading [6]
[08/11 06:16:37.580] [TRACE] [ZigbeeNetworkProcessor] FE 01 61 1D 00 7D | 254 1 97 29 0 125
[08/11 06:16:37.580] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 1D, 01, 03, 00
[08/11 06:16:37.585] [TRACE] [ZigbeeNetworkProcessor] Writing 1 bytes
[08/11 06:16:37.585] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT <<<
[08/11 06:16:37.585] [INFO] [USER] Setting the logical type to coordinator
[08/11 06:16:37.585] [INFO] [ZigbeeNetworkProcessor] Setting adapter logical type
[08/11 06:16:37.585] [DEBUG] [ZigbeeNetworkProcessor] Writing NV Item [135]
[08/11 06:16:37.585] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_GET_LENGTH_REQ >>>
[08/11 06:16:37.585] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.585] [TRACE] [ZigbeeNetworkProcessor] FE 02 21 13 87 00 B7 | 254 2 33 19 135 0 183
[08/11 06:16:37.593] [TRACE] [ZigbeeNetworkProcessor] Reading [7]
[08/11 06:16:37.593] [TRACE] [ZigbeeNetworkProcessor] FE 02 61 13 01 00 71 | 254 2 97 19 1 0 113
[08/11 06:16:37.593] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 13, 01, 03, 01 00
[08/11 06:16:37.597] [TRACE] [ZigbeeNetworkProcessor] [Recv]: SYS_GET_LENGTH_REQ <<<
[08/11 06:16:37.597] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT >>>
[08/11 06:16:37.597] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.597] [TRACE] [ZigbeeNetworkProcessor] FE 07 21 1D 87 00 00 00 01 00 00 BD | 254 7 33 29 135 0 0 0 1 0 0 189
[08/11 06:16:37.610] [TRACE] [ZigbeeNetworkProcessor] Reading [6]
[08/11 06:16:37.610] [TRACE] [ZigbeeNetworkProcessor] FE 01 61 1D 00 7D | 254 1 97 29 0 125
[08/11 06:16:37.610] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 1D, 01, 03, 00
[08/11 06:16:37.619] [TRACE] [ZigbeeNetworkProcessor] Writing 1 bytes
[08/11 06:16:37.620] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT <<<
[08/11 06:16:37.620] [INFO] [USER] Enabling ZDO direct callback
[08/11 06:16:37.620] [INFO] [ZigbeeNetworkProcessor] Setting ZDO direct callback
[08/11 06:16:37.620] [DEBUG] [ZigbeeNetworkProcessor] Writing NV Item [143]
[08/11 06:16:37.620] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_GET_LENGTH_REQ >>>
[08/11 06:16:37.620] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.620] [TRACE] [ZigbeeNetworkProcessor] FE 02 21 13 8F 00 BF | 254 2 33 19 143 0 191
[08/11 06:16:37.628] [TRACE] [ZigbeeNetworkProcessor] Reading [7]
[08/11 06:16:37.628] [TRACE] [ZigbeeNetworkProcessor] FE 02 61 13 01 00 71 | 254 2 97 19 1 0 113
[08/11 06:16:37.628] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 13, 01, 03, 01 00
[08/11 06:16:37.631] [TRACE] [ZigbeeNetworkProcessor] [Recv]: SYS_GET_LENGTH_REQ <<<
[08/11 06:16:37.631] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT >>>
[08/11 06:16:37.631] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.631] [TRACE] [ZigbeeNetworkProcessor] FE 07 21 1D 8F 00 00 00 01 00 01 B4 | 254 7 33 29 143 0 0 0 1 0 1 180
[08/11 06:16:37.644] [TRACE] [ZigbeeNetworkProcessor] Reading [6]
[08/11 06:16:37.644] [TRACE] [ZigbeeNetworkProcessor] FE 01 61 1D 00 7D | 254 1 97 29 0 125
[08/11 06:16:37.644] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 1D, 01, 03, 00
[08/11 06:16:37.654] [TRACE] [ZigbeeNetworkProcessor] Writing 1 bytes
[08/11 06:16:37.654] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT <<<
[08/11 06:16:37.655] [INFO] [USER] Setting the channel lists
[08/11 06:16:37.655] [INFO] [ZigbeeNetworkProcessor] Setting adapter channel list
[08/11 06:16:37.655] [DEBUG] [ZigbeeNetworkProcessor] Writing NV Item [132]
[08/11 06:16:37.655] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_GET_LENGTH_REQ >>>
[08/11 06:16:37.655] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.655] [TRACE] [ZigbeeNetworkProcessor] FE 02 21 13 84 00 B4 | 254 2 33 19 132 0 180
[08/11 06:16:37.663] [TRACE] [ZigbeeNetworkProcessor] Reading [7]
[08/11 06:16:37.663] [TRACE] [ZigbeeNetworkProcessor] FE 02 61 13 04 00 74 | 254 2 97 19 4 0 116
[08/11 06:16:37.663] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 13, 01, 03, 04 00
[08/11 06:16:37.667] [TRACE] [ZigbeeNetworkProcessor] [Recv]: SYS_GET_LENGTH_REQ <<<
[08/11 06:16:37.667] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT >>>
[08/11 06:16:37.667] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.667] [TRACE] [ZigbeeNetworkProcessor] FE 0A 21 1D 84 00 00 00 04 00 00 10 00 00 A6 | 254 10 33 29 132 0 0 0 4 0 0 16 0 0 166
[08/11 06:16:37.680] [TRACE] [ZigbeeNetworkProcessor] Reading [6]
[08/11 06:16:37.681] [TRACE] [ZigbeeNetworkProcessor] FE 01 61 1D 00 7D | 254 1 97 29 0 125
[08/11 06:16:37.681] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 1D, 01, 03, 00
[08/11 06:16:37.690] [TRACE] [ZigbeeNetworkProcessor] Writing 4 bytes
[08/11 06:16:37.690] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT <<<
[08/11 06:16:37.690] [INFO] [USER] Setting the pan ID
[08/11 06:16:37.690] [INFO] [ZigbeeNetworkProcessor] Setting adapter PAN ID
[08/11 06:16:37.690] [DEBUG] [ZigbeeNetworkProcessor] Writing NV Item [131]
[08/11 06:16:37.690] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_GET_LENGTH_REQ >>>
[08/11 06:16:37.690] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.690] [TRACE] [ZigbeeNetworkProcessor] FE 02 21 13 83 00 B3 | 254 2 33 19 131 0 179
[08/11 06:16:37.699] [TRACE] [ZigbeeNetworkProcessor] Reading [7]
[08/11 06:16:37.699] [TRACE] [ZigbeeNetworkProcessor] FE 02 61 13 02 00 72 | 254 2 97 19 2 0 114
[08/11 06:16:37.699] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 13, 01, 03, 02 00
[08/11 06:16:37.700] [TRACE] [ZigbeeNetworkProcessor] [Recv]: SYS_GET_LENGTH_REQ <<<
[08/11 06:16:37.701] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT >>>
[08/11 06:16:37.701] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.701] [TRACE] [ZigbeeNetworkProcessor] FE 08 21 1D 83 00 00 00 02 00 64 1A CB | 254 8 33 29 131 0 0 0 2 0 100 26 203
[08/11 06:16:37.714] [TRACE] [ZigbeeNetworkProcessor] Reading [6]
[08/11 06:16:37.714] [TRACE] [ZigbeeNetworkProcessor] FE 01 61 1D 00 7D | 254 1 97 29 0 125
[08/11 06:16:37.714] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 1D, 01, 03, 00
[08/11 06:16:37.722] [TRACE] [ZigbeeNetworkProcessor] Writing 2 bytes
[08/11 06:16:37.722] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT <<<
[08/11 06:16:37.722] [INFO] [USER] Setting the extended pan ID
[08/11 06:16:37.722] [INFO] [ZigbeeNetworkProcessor] Setting adapter extended PAN ID
[08/11 06:16:37.722] [DEBUG] [ZigbeeNetworkProcessor] Writing NV Item [45]
[08/11 06:16:37.722] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_GET_LENGTH_REQ >>>
[08/11 06:16:37.722] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.722] [TRACE] [ZigbeeNetworkProcessor] FE 02 21 13 2D 00 1D | 254 2 33 19 45 0 29
[08/11 06:16:37.731] [TRACE] [ZigbeeNetworkProcessor] Reading [7]
[08/11 06:16:37.731] [TRACE] [ZigbeeNetworkProcessor] FE 02 61 13 08 00 78 | 254 2 97 19 8 0 120
[08/11 06:16:37.731] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 13, 01, 03, 08 00
[08/11 06:16:37.735] [TRACE] [ZigbeeNetworkProcessor] [Recv]: SYS_GET_LENGTH_REQ <<<
[08/11 06:16:37.735] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT >>>
[08/11 06:16:37.735] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.735] [TRACE] [ZigbeeNetworkProcessor] FE 0E 21 1D 2D 00 00 00 08 00 DD DD DD DD DD DD DD DD 17 | 254 14 33 29 45 0 0 0 8 0 221 221 221 221 221 221 221 221 23
[08/11 06:16:37.749] [TRACE] [ZigbeeNetworkProcessor] Reading [6]
[08/11 06:16:37.749] [TRACE] [ZigbeeNetworkProcessor] FE 01 61 1D 00 7D | 254 1 97 29 0 125
[08/11 06:16:37.749] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 1D, 01, 03, 00
[08/11 06:16:37.758] [TRACE] [ZigbeeNetworkProcessor] Writing 8 bytes
[08/11 06:16:37.758] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT <<<
[08/11 06:16:37.758] [INFO] [USER] Setting the APS use extended pan ID
[08/11 06:16:37.758] [INFO] [ZigbeeNetworkProcessor] Setting adapter APS use extended PAN ID
[08/11 06:16:37.758] [DEBUG] [ZigbeeNetworkProcessor] Writing NV Item [71]
[08/11 06:16:37.758] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_GET_LENGTH_REQ >>>
[08/11 06:16:37.758] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.758] [TRACE] [ZigbeeNetworkProcessor] FE 02 21 13 47 00 77 | 254 2 33 19 71 0 119
[08/11 06:16:37.764] [TRACE] [ZigbeeNetworkProcessor] Reading [7]
[08/11 06:16:37.764] [TRACE] [ZigbeeNetworkProcessor] FE 02 61 13 08 00 78 | 254 2 97 19 8 0 120
[08/11 06:16:37.764] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 13, 01, 03, 08 00
[08/11 06:16:37.770] [TRACE] [ZigbeeNetworkProcessor] [Recv]: SYS_GET_LENGTH_REQ <<<
[08/11 06:16:37.770] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT >>>
[08/11 06:16:37.770] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.770] [TRACE] [ZigbeeNetworkProcessor] FE 0E 21 1D 47 00 00 00 08 00 DD DD DD DD DD DD DD DD 7D | 254 14 33 29 71 0 0 0 8 0 221 221 221 221 221 221 221 221 125
[08/11 06:16:37.778] [TRACE] [ZigbeeNetworkProcessor] Reading [6]
[08/11 06:16:37.778] [TRACE] [ZigbeeNetworkProcessor] FE 01 61 1D 00 7D | 254 1 97 29 0 125
[08/11 06:16:37.778] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 1D, 01, 03, 00
[08/11 06:16:37.781] [TRACE] [ZigbeeNetworkProcessor] Writing 8 bytes
[08/11 06:16:37.781] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT <<<
[08/11 06:16:37.781] [INFO] [USER] Enabling preconfigured keys
[08/11 06:16:37.781] [INFO] [ZigbeeNetworkProcessor] Enabling adapter preconfigured keys
[08/11 06:16:37.781] [DEBUG] [ZigbeeNetworkProcessor] Writing NV Item [99]
[08/11 06:16:37.781] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_GET_LENGTH_REQ >>>
[08/11 06:16:37.781] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.781] [TRACE] [ZigbeeNetworkProcessor] FE 02 21 13 63 00 53 | 254 2 33 19 99 0 83
[08/11 06:16:37.789] [TRACE] [ZigbeeNetworkProcessor] Reading [7]
[08/11 06:16:37.789] [TRACE] [ZigbeeNetworkProcessor] FE 02 61 13 01 00 71 | 254 2 97 19 1 0 113
[08/11 06:16:37.789] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 13, 01, 03, 01 00
[08/11 06:16:37.793] [TRACE] [ZigbeeNetworkProcessor] [Recv]: SYS_GET_LENGTH_REQ <<<
[08/11 06:16:37.793] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT >>>
[08/11 06:16:37.793] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.793] [TRACE] [ZigbeeNetworkProcessor] FE 07 21 1D 63 00 00 00 01 00 00 59 | 254 7 33 29 99 0 0 0 1 0 0 89
[08/11 06:16:37.806] [TRACE] [ZigbeeNetworkProcessor] Reading [6]
[08/11 06:16:37.806] [TRACE] [ZigbeeNetworkProcessor] FE 01 61 1D 00 7D | 254 1 97 29 0 125
[08/11 06:16:37.806] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 1D, 01, 03, 00
[08/11 06:16:37.814] [TRACE] [ZigbeeNetworkProcessor] Writing 1 bytes
[08/11 06:16:37.814] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT <<<
[08/11 06:16:37.814] [INFO] [USER] Setting preconfigured keys
[08/11 06:16:37.814] [INFO] [ZigbeeNetworkProcessor] Setting adapter preconfigured keys
[08/11 06:16:37.814] [DEBUG] [ZigbeeNetworkProcessor] Writing NV Item [98]
[08/11 06:16:37.814] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_GET_LENGTH_REQ >>>
[08/11 06:16:37.814] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.814] [TRACE] [ZigbeeNetworkProcessor] FE 02 21 13 62 00 52 | 254 2 33 19 98 0 82
[08/11 06:16:37.819] [TRACE] [ZigbeeNetworkProcessor] Reading [7]
[08/11 06:16:37.819] [TRACE] [ZigbeeNetworkProcessor] FE 02 61 13 10 00 60 | 254 2 97 19 16 0 96
[08/11 06:16:37.819] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 13, 01, 03, 10 00
[08/11 06:16:37.825] [TRACE] [ZigbeeNetworkProcessor] [Recv]: SYS_GET_LENGTH_REQ <<<
[08/11 06:16:37.825] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT >>>
[08/11 06:16:37.825] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.825] [TRACE] [ZigbeeNetworkProcessor] FE 16 21 1D 62 00 00 00 10 00 01 03 05 07 09 0B 0D 0F 00 02 04 06 08 0A 0C 0D 5B | 254 22 33 29 98 0 0 0 16 0 1 3 5 7 9 11 13 15 0 2 4 6 8 10 12 13 91
[08/11 06:16:37.833] [TRACE] [ZigbeeNetworkProcessor] Reading [6]
[08/11 06:16:37.833] [TRACE] [ZigbeeNetworkProcessor] FE 01 61 1D 00 7D | 254 1 97 29 0 125
[08/11 06:16:37.833] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 1D, 01, 03, 00
[08/11 06:16:37.836] [TRACE] [ZigbeeNetworkProcessor] Writing 16 bytes
[08/11 06:16:37.836] [TRACE] [ZigbeeNetworkProcessor] [Send]: SYS_OSAL_NV_WRITE_EXT <<<
[08/11 06:16:37.836] [INFO] [USER] Setting primary BDB Commissioning channels
[08/11 06:16:37.836] [TRACE] [ZigbeeNetworkProcessor] [Send]: APP_CNF_BDB_SET_CHANNEL >>>
[08/11 06:16:37.836] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.836] [TRACE] [ZigbeeNetworkProcessor] FE 05 2F 08 01 00 10 00 00 33 | 254 5 47 8 1 0 16 0 0 51
[08/11 06:16:37.841] [TRACE] [ZigbeeNetworkProcessor] Reading [6]
[08/11 06:16:37.841] [TRACE] [ZigbeeNetworkProcessor] FE 01 6F 08 00 66 | 254 1 111 8 0 102
[08/11 06:16:37.841] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 08, 0F, 03, 00
[08/11 06:16:37.848] [TRACE] [ZigbeeNetworkProcessor] [Recv]: APP_CNF_BDB_SET_CHANNEL <<<
[08/11 06:16:37.848] [INFO] [USER] Setting secondary BDB Commissioning channels
[08/11 06:16:37.848] [TRACE] [ZigbeeNetworkProcessor] [Send]: APP_CNF_BDB_SET_CHANNEL >>>
[08/11 06:16:37.848] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.848] [TRACE] [ZigbeeNetworkProcessor] FE 05 2F 08 00 00 00 00 00 22 | 254 5 47 8 0 0 0 0 0 34
[08/11 06:16:37.852] [TRACE] [ZigbeeNetworkProcessor] Reading [6]
[08/11 06:16:37.853] [TRACE] [ZigbeeNetworkProcessor] FE 01 6F 08 00 66 | 254 1 111 8 0 102
[08/11 06:16:37.853] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 08, 0F, 03, 00
[08/11 06:16:37.859] [TRACE] [ZigbeeNetworkProcessor] [Recv]: APP_CNF_BDB_SET_CHANNEL <<<
[08/11 06:16:37.859] [INFO] [USER] Starting BDB Commissioning commissioning
[08/11 06:16:37.859] [TRACE] [ZigbeeNetworkProcessor] [Send]: APP_CNF_BDB_START_COMMISSIONING >>>
[08/11 06:16:37.859] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:37.859] [TRACE] [ZigbeeNetworkProcessor] FE 01 2F 05 04 2F | 254 1 47 5 4 47
[08/11 06:16:40.156] [TRACE] [ZigbeeNetworkProcessor] Reading [14]
[08/11 06:16:40.156] [TRACE] [ZigbeeNetworkProcessor] FE 01 6F 05 00 6B FE 03 4F 80 01 02 04 CB | 254 1 111 5 0 107 254 3 79 128 1 2 4 203
[08/11 06:16:40.156] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 05, 0F, 03, 00
[08/11 06:16:40.156] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 80, 0F, 02, 01 02 04
[08/11 06:16:40.167] [TRACE] [ZigbeeNetworkProcessor] [Recv]: APP_CNF_BDB_START_COMMISSIONING <<<
[08/11 06:16:40.433] [TRACE] [ZigbeeNetworkProcessor] Reading [6]
[08/11 06:16:40.433] [TRACE] [ZigbeeNetworkProcessor] FE 01 45 C0 08 8C | 254 1 69 192 8 140
[08/11 06:16:40.434] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: C0, 05, 02, 08
[08/11 06:16:41.011] [TRACE] [ZigbeeNetworkProcessor] Reading [14]
[08/11 06:16:41.011] [TRACE] [ZigbeeNetworkProcessor] FE 01 45 C0 09 8D FE 03 4F 80 00 02 00 CE | 254 1 69 192 9 141 254 3 79 128 0 2 0 206
[08/11 06:16:41.011] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: C0, 05, 02, 09
[08/11 06:16:41.011] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 80, 0F, 02, 00 02 00
[08/11 06:16:50.170] [INFO] [ZigbeeNetworkProcessor] Getting adapter active endpoints
[08/11 06:16:50.170] [TRACE] [ZigbeeNetworkProcessor] [Send]: ZDO_ACTIVE_EP_REQ >>>
[08/11 06:16:50.171] [TRACE] [ZigbeeNetworkProcessor] Writing: 
[08/11 06:16:50.171] [TRACE] [ZigbeeNetworkProcessor] FE 05 25 05 00 FF FF FF FF 25 | 254 5 37 5 0 255 255 255 255 37
[08/11 06:16:50.176] [TRACE] [ZigbeeNetworkProcessor] Reading [6]
[08/11 06:16:50.176] [TRACE] [ZigbeeNetworkProcessor] FE 01 65 05 00 61 | 254 1 101 5 0 97
[08/11 06:16:50.176] [TRACE] [ZigbeeNetworkProcessor] <<< FRAME: 05, 05, 03, 00
[08/11 06:16:50.181] [TRACE] [ZigbeeNetworkProcessor] [Recv]: ZDO_ACTIVE_EP_REQ <<<
Zigbee adapter started

Process finished with exit code 0
```

## Goals / Tasks / Roadmap

- [🕓] Get native Zigbee support
- [  ] API Server + CLI for local control
- [  ] Frontend
- [  ] Alexa integration
- [  ] More...