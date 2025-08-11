/**
 * Tags: either
 */

#pragma once

#include <variant>
#include <concepts>
#include <cstdint>
#include <string>
#include <type_traits>
#include <optional>
#include <any>

namespace lcl {
#define RETURN_ON_FALSY(EITHER) if (!EITHER) return EITHER

  template <typename T>
  concept ErrorCode = std::same_as<T, std::uint8_t> ||
                      (std::is_enum_v<T> && std::same_as<std::underlying_type_t<T>, std::uint8_t>);

  // Type-erased wrapper for any value type
  struct AnyValue {
    std::any value;

    template <typename T>
    AnyValue(T val) : value(std::move(val)) {}

    template <typename T>
    [[nodiscard]] std::optional<T> get() const {
      try {
        return std::any_cast<T>(value);
      } catch (const std::bad_any_cast&) {
        return std::nullopt;
      }
    }
  };

  // Type-erased wrapper for any Error type
  struct AnyError {
    std::any error;

    using ErrorCodeType = std::uint8_t; // Define ErrorCodeType for ErrorType concept
    using ValueType = void;             // Define ValueType as void for ErrorType concept

    template <typename T>
    AnyError(T err) : error(std::move(err)) {}

    // Declare methods without implementation
    [[nodiscard]] std::string getErrorCodeAsString() const;
    [[nodiscard]] std::string getMessage() const;
  };

  template <ErrorCode TErrorCode, typename T>
  struct Error {
    TErrorCode code{};
    std::string message;
    T value;
    std::optional<AnyError> innerError = std::nullopt;

    using ErrorCodeType = TErrorCode;
    using ValueType = T;
  };

  template <ErrorCode TErrorCode>
  struct Error<TErrorCode, void> {
    TErrorCode code{};
    std::string message;
    std::optional<AnyError> innerError = std::nullopt;

    using ErrorCodeType = TErrorCode;
    using ValueType = void;
  };

  // Implement AnyError methods after Error is defined
  inline std::string AnyError::getErrorCodeAsString() const {
    try {
      if (auto* err = std::any_cast<Error<std::uint8_t, void>>(&error)) {
        return std::to_string(err->code);
      }
      // Add more specific casts for known enum types if needed
      return "Unknown error code";
    } catch (const std::bad_any_cast&) {
      return "Unknown error code";
    }
  }

  inline std::string AnyError::getMessage() const {
    try {
      if (auto* err = std::any_cast<Error<std::uint8_t, void>>(&error)) {
        return err->message;
      }
      // Add more specific casts for known types if needed
      return "Unknown error";
    } catch (const std::bad_any_cast&) {
      return "Unknown error";
    }
  }

  template <typename T>
  concept ErrorType = requires(T t) {
    typename T::ErrorCodeType;
    typename T::ValueType;
    requires ErrorCode<typename T::ErrorCodeType>;
    requires std::same_as<T, AnyError> ||
             std::is_base_of_v<
                 std::conditional_t<
                     std::is_void_v<typename T::ValueType>,
                     Error<typename T::ErrorCodeType, void>,
                     Error<typename T::ErrorCodeType, typename T::ValueType>>,
                 T>;
  };

  template <typename TResult, ErrorType TError>
  struct Either {
    std::variant<TResult, TError> data;

    // Constructor for variant
    Either(std::variant<TResult, TError> variant) : data(std::move(variant)) {}

    // Conversion constructor for TResult
    Either(const TResult& result) : data(std::in_place_type<TResult>, result) {}

    [[nodiscard]] static Either value(TResult result) {
      return Either(std::variant<TResult, TError>(std::in_place_type<TResult>, result));
    }

    [[nodiscard]] static Either error(TError err) {
      return Either(std::variant<TResult, TError>(std::in_place_type<TError>, std::move(err)));
    }

    [[nodiscard]] bool isSuccess() const {
      return std::holds_alternative<TResult>(data);
    }

    [[nodiscard]] bool isError() const {
      return std::holds_alternative<TError>(data);
    }

    // Implicit conversion to bool for truthiness testing
    [[nodiscard]] explicit operator bool() const {
      return isSuccess();
    }

    [[nodiscard]] std::optional<TResult> value() const {
      if (!isSuccess()) {
        return std::nullopt;
      }

      return std::get<TResult>(data);
    }

    [[nodiscard]] TResult valueOrElse(TResult result) const {
      if (!isSuccess()) {
        return result;
      }

      return std::get<TResult>(data);
    }

    [[nodiscard]] TResult valueOrThrow() const {
      if (!isSuccess()) {
        throw std::exception();
      }

      return std::get<TResult>(data);
    }

    [[nodiscard]] TError errorOrThrow() const {
      if (!isError()) {
        throw std::exception();
      }

      return std::get<TError>(data);
    }

    [[nodiscard]] std::optional<TError> error() const {
      if (!isError()) {
        return std::nullopt;
      }

      return std::get<TError>(data);
    }

    [[nodiscard]] TError errorOrElse(TError error) const {
      if (!isError()) {
        return error;
      }

      return std::get<TError>(data);
    }

    // Conversion constructor for different value types and error types
    template <typename U, ErrorType E>
    Either(const Either<U, E>& other) {
      if (other.isSuccess()) {
        if constexpr (std::is_convertible_v<U, TResult>) {
          data = TResult{*other.value()};
        } else {
          throw std::bad_cast(); // Or handle differently if conversion not possible
        }
      } else {
        if constexpr (std::is_same_v<TError, AnyError>) {
          data = AnyError{other.errorOrThrow()};
        } else {
          throw std::bad_cast(); // Or handle differently if error types incompatible
        }
      }
    }

    // Dereference operator to get value or throw
    [[nodiscard]] TResult& operator*() {
      return const_cast<TResult&>(static_cast<const Either*>(this)->operator*());
    }

    [[nodiscard]] const TResult& operator*() const {
      if (!isSuccess()) {
        throw std::exception();
      }
      return std::get<TResult>(data);
    }

    // Arrow operator to access members of TResult
    [[nodiscard]] TResult* operator->() {
      return const_cast<TResult*>(static_cast<const Either*>(this)->operator->());
    }

    [[nodiscard]] const TResult* operator->() const {
      if (!isSuccess()) {
        throw std::exception();
      }
      return &std::get<TResult>(data);
    }
  };
}