/**
 * Tags: main
 */
#include <cli/cli.hpp>
#include <iostream>

int main(int argc, char const* argv[])
{
  lcl::cli::CLI cli;

  try {
    auto command = cli.ParseArguments(argc, argv);
    auto result = cli.ExecuteCommand(command, std::cout).await_resume();

    if (!result.success) {
      std::cerr << "Error: " << result.errorMessage << '\n';
      return 1;
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}