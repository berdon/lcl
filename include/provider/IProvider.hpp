/**
 * Tags: provider
 */
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>

namespace lcl::provider {
    class IDelegate;

    struct Device {
        std::string id;
        std::string name;
        bool isOnline;
    };

    class IProvider {
    public:
        virtual ~IProvider() = default;

        [[nodiscard]] virtual std::vector<Device> GetDevices() const = 0;

        [[nodiscard]] virtual std::optional<Device> GetDevice(std::string_view id) const = 0;

        virtual void SetDelegate(std::shared_ptr<IDelegate> delegate) = 0;
    };
} // namespace lcl::provider
