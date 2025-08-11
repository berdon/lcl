# Z-Stack

Z-Stack is Texas Instruments Zigbee stack.

## Reference Material

Some (maybe out of date) reference material can be found [here](https://software-dl.ti.com/simplelink/esd/plugins/simplelink_zigbee_sdk_plugin/1.60.00.14/docs/zigbee_user_guide/html/zigbee/index.html). In particular, these are extremely useful:
- [CC26x2 Technical Reference Manual](https://www.ti.com/lit/pdf/swcu117h)
- [SimpleLink SDK](https://www.ti.com/tool/SIMPLELINK-LOWPOWER-SDK) - Download the SIMPLELINK-LOWPOWER-F2-SDK SDK and then access the documentation
    - The `Z-Stack Monitor and Test API.pdf` (locally [here](file:///Applications/ti/simplelink_cc13xx_cc26xx_sdk_8_30_01_01/docs/zigbee/Z-Stack%20Monitor%20and%20Test%20API.pdf)) is very useful

The [Zigbee specification](https://csa-iot.org/wp-content/uploads/2023/04/05-3474-23-csg-zigbee-specification-compressed.pdf) can be useful. It's still not expressly clear where the boundary between zigbee and the implementation is. ZNP (Zigbee Network Processor) is...maybe _actual_ implementation of TI's Z-Stack.

`zigbee-herdsman`, the underlying python package used by `zigbee2mqtt`, is also _extremely_ valuable. I think it probably mostly consumed the zigbee protocol from [Zigbee specification](https://csa-iot.org/wp-content/uploads/2023/04/05-3474-23-csg-zigbee-specification-compressed.pdf) and the various implementation docs (such as TI's above).