/**
 * Tags: cli
 */
#pragma once
#include <string>
#include <vector>
#include <ostream>

namespace lcl::cli {
    struct Command {
        std::string name;
        std::vector<std::string> arguments;
    };

    struct Result {
        bool success;
        std::string errorMessage;
    };

    class CLI {
    public:
        [[nodiscard]] Command ParseArguments(int argc, char const* argv[]) const;
        [[nodiscard]] Result ExecuteCommand(const Command& command, std::ostream& output) const;
    };
} // namespace lcl::cli