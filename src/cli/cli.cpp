/**
 * Tags: cli
 */
#include <cli/cli.hpp>
#include <stdexcept>
#include <provider/ZigbeeProvider.hpp>
#include <provider/MqttProvider.hpp>

#include "../../include/zigbee/adapter/ZStack/ZStackAdapter.hpp"

namespace lcl::cli {
    Command CLI::ParseArguments(int argc, char const* argv[]) const {
        if (argc < 2) {
            throw std::runtime_error("No command provided");
        }

        Command cmd;
        cmd.name = argv[1];

        for (int i = 2; i < argc; ++i) {
            cmd.arguments.emplace_back(argv[i]);
        }

        return cmd;
    }

    async::Task<Result> CLI::ExecuteCommand(const Command& command, std::ostream& output) const {
        Result result{true, ""};

        try {
            if (command.name == "help") {
                output << "Available commands:\n"
                       << " help - Display this help message\n"
                       << " list - List available devices\n"
                       << " test-zigbee - Test Zigbee provider\n"
                       << " test-mqtt - Test MQTT provider\n"
                       << " test-zstack - Test Z-Stack adapter\n";
            } else if (command.name == "test-zigbee") {
                auto provider = provider::ZigbeeProvider("tcp://10.0.3.111:6638");
                // Print initial devices
                auto devices = provider.GetDevices();
                output << "Zigbee devices:\n";
                for (const auto& device : devices) {
                    output << "ID: " << device.id << ", Name: " << device.name
                           << ", Online: " << (device.isOnline ? "true" : "false") << '\n';
                }
                // Wait for async operations to collect data (e.g., 5 seconds)
                std::this_thread::sleep_for(std::chrono::seconds(120));
            } else if (command.name == "test-mqtt") {
                auto provider = provider::MqttProvider("mqtt://10.0.6.105:1883");
                // Print initial devices
                auto devices = provider.GetDevices();
                output << "Initial devices:\n";
                for (const auto& device : devices) {
                    output << "ID: " << device.id << ", Name: " << device.name
                           << ", Online: " << (device.isOnline ? "true" : "false") << '\n';
                }

                // Wait for async operations to collect data (e.g., 5 seconds)
                std::this_thread::sleep_for(std::chrono::seconds(120));
            } else if (command.name == "test-zstack") {
                try {
                    auto adapter = zigbee::adapter::ZStackAdapter("usb:///dev/tty.usbserial-1411340", {});
                    if (!co_await adapter.start()) {
                        std::cerr << "Failed to start Zigbee" << std::endl;
                    }
                    else {
                        std::cout << "Zigbee adapter started" << std::endl;
                    }
                } catch (const std::exception& e) {
                    const auto i = 5;
                }
                // while (!task.handle_.done()) {
                //     std::this_thread::sleep_for(std::chrono::seconds(1000));
                // }
                // task.handle_.destroy();
            } else {
                result.success = false;
                result.errorMessage = "Unknown command: " + command.name;
            }
        } catch (const std::exception& e) {
            result.success = false;
            result.errorMessage = e.what();
        }

        co_return result;
    }
} // namespace lcl::cli