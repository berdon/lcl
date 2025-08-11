#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <chrono>
#include <cstdio>
#include <cstdarg>
#include <tuple>
#include <type_traits>
#include <iomanip>
#include <sstream>

namespace lcl::logger {
  enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR
  };

  class Logger {
    static LogLevel get_initial_level() {
      if (const char* env = std::getenv("LOG_LEVEL")) {
        const std::string level_str(env);
        if (level_str == "TRACE") return LogLevel::TRACE;
        if (level_str == "DEBUG") return LogLevel::DEBUG;
        if (level_str == "INFO") return LogLevel::INFO;
        if (level_str == "WARN") return LogLevel::WARN;
        if (level_str == "ERROR") return LogLevel::ERROR;
      }
      return LogLevel::INFO; // Default level
    }

    inline static LogLevel current_level = get_initial_level(); // Use inline static

    static std::string get_current_time() {
      auto now = std::chrono::system_clock::now();
      auto time = std::chrono::system_clock::to_time_t(now);
      auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
          now.time_since_epoch()) % 1000;

      std::stringstream ss;
      ss << std::put_time(std::localtime(&time), "%m/%d %H:%M:%S")
         << '.' << std::setfill('0') << std::setw(3) << ms.count();
      return ss.str();
    }

    // Helper to convert std::string to const char* for snprintf
    template<typename T>
    static auto convert_arg(const T& arg) {
      if constexpr (std::is_same_v<std::decay_t<T>, std::string>) {
        return arg.c_str();
      } else {
        return arg;
      }
    }

    // Helper to convert all arguments to a tuple of safe types
    template<typename... Args>
    static auto convert_args(const Args&... args) {
      return std::make_tuple(convert_arg(args)...);
    }

    // Helper to apply snprintf with tuple of arguments
    template<typename Tuple, std::size_t... Is>
    static std::string format_string_impl(const std::string& fmt, const Tuple& args, std::index_sequence<Is...>) {
      std::string result;
      if constexpr (sizeof...(Is) > 0) {
        // Calculate required buffer size
        int size = std::snprintf(nullptr, 0, fmt.c_str(), std::get<Is>(args)...);
        if (size < 0) {
          return fmt; // Fallback to raw fmt on error
        }
        // Allocate buffer and format
        result.resize(size);
        std::snprintf(result.data(), size + 1, fmt.c_str(), std::get<Is>(args)...);
      } else {
        result = fmt; // No args, use fmt directly
      }
      return result;
    }

    // Main formatting function
    template<typename... Args>
    static std::string format_string(const std::string& fmt, Args&&... args) {
      auto converted_args = convert_args(std::forward<Args>(args)...);
      return format_string_impl(fmt, converted_args, std::index_sequence_for<Args...>{});
    }

  public:
    static LogLevel currentLogLevel() {
      return current_level;
    }

    template<typename... Args>
    static void write(const LogLevel level, const std::string& tag, const std::string& msg, Args&&... args) {
      if (current_level > level) return;
      std::string level_str;
      switch (level) {
        case LogLevel::TRACE: level_str = "TRACE"; break;
        case LogLevel::DEBUG: level_str = "DEBUG"; break;
        case LogLevel::INFO: level_str = "INFO"; break;
        case LogLevel::WARN: level_str = "WARN"; break;
        case LogLevel::ERROR: level_str = "ERROR"; break;
      }

      // Format the message using the helper function
      std::string formatted_msg = format_string(msg, std::forward<Args>(args)...);

      // Output using std::cerr with fixed format
      std::cerr << "[" << get_current_time() << "] [" << level_str << "] [" << tag << "] " << formatted_msg << std::endl << std::flush;
    }

    static void set_level(const LogLevel call_level) {
      current_level = call_level;
    }

    template<typename... Args>
    static void trace(const std::string& tag, const std::string& msg, Args&&... args) {
      write(LogLevel::TRACE, tag, msg, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void debug(const std::string& tag, const std::string& msg, Args&&... args) {
      write(LogLevel::DEBUG, tag, msg, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void info(const std::string& tag, const std::string& msg, Args&&... args) {
      write(LogLevel::INFO, tag, msg, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void warn(const std::string& tag, const std::string& msg, Args&&... args) {
      write(LogLevel::WARN, tag, msg, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void error(const std::string& tag, const std::string& msg, Args&&... args) {
      write(LogLevel::ERROR, tag, msg, std::forward<Args>(args)...);
    }

    template<typename Ex, typename... Args>
    static void exception(const std::string& tag, const Ex& ex, const std::string& msg, Args&&... args) {
      if (current_level > LogLevel::ERROR) return;

      // Format the message using the helper function
      const std::string formatted_msg = format_string(msg, std::forward<Args>(args)...);

      // Output using std::cerr with fixed format
      std::cerr << "[" << get_current_time() << "] [EXCEPTION] [" << tag << "] " << formatted_msg << " Exception: " << ex.what() << std::endl << std::flush;
    }
  };
}