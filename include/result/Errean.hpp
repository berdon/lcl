/**
* Errean - A wrapper around std::optional with inverted boolean conversion
 * Tags: errean, optional, error-handling
 */

#pragma once

#include <optional>
#include <type_traits>

namespace lcl {
  template <typename T>
  class Errean {
  public:
    using value_type = T;

    // Constructors
    Errean() = default;

    template <typename U = T>
    Errean(U&& value) : opt(std::forward<U>(value)) {}

    Errean(std::nullopt_t) : opt(std::nullopt) {}

    // Constructor for bool: true maps to nullopt, false maps to value (if T is bool)
    Errean(const bool value) {
      opt = std::nullopt;
    }

    // Assignment operators
    Errean& operator=(std::nullopt_t) {
      opt = std::nullopt;
      return *this;
    }

    template <typename U = T>
    Errean& operator=(U&& value) {
      opt = std::forward<U>(value);
      return *this;
    }

    // Assignment for bool: true maps to nullopt, false maps to value (if T is bool)
    Errean& operator=(bool value) {
      if constexpr (std::is_same_v<T, bool>) {
        if (!value) {
          opt = false; // false becomes a valid bool value
        } else {
          opt = std::nullopt; // true becomes nullopt
        }
      } else {
        static_assert(std::is_same_v<T, bool>, "Errean::operator=(bool) only valid for Errean<bool>");
      }
      return *this;
    }

    // Accessors
    [[nodiscard]] T& operator*() {
      return *opt;
    }

    [[nodiscard]] const T& operator*() const {
      return *opt;
    }

    [[nodiscard]] T* operator->() {
      return opt.operator->();
    }

    [[nodiscard]] const T* operator->() const {
      return opt.operator->();
    }

    // Status checks
    [[nodiscard]] bool has_value() const {
      return opt.has_value();
    }

    [[nodiscard]] explicit operator bool() const {
      return !opt.has_value(); // Inverted boolean conversion
    }

    // Value access
    [[nodiscard]] T& value() {
      return opt.value();
    }

    [[nodiscard]] const T& value() const {
      return opt.value();
    }

    template <typename U>
    [[nodiscard]] T value_or(U&& default_value) const {
      return opt.value_or(std::forward<U>(default_value));
    }

    // Reset
    void reset() {
      opt.reset();
    }

  private:
    std::optional<T> opt;
  };
}