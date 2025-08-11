/**
 * Tags: provider
 */
#pragma once

namespace lcl::provider {

    struct Device;

    class IDelegate {
    public:
        virtual ~IDelegate() = default;

        virtual void OnNewDevice(const Device& device) = 0;
        virtual void OnDeviceChanged(const Device& device) = 0;
    };

} // namespace lcl::provider