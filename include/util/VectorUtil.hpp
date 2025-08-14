#pragma once

#define TO_VECTOR_ARG_LITTLE_ENDIAN_U16(val) \
static_cast<uint8_t>(((val) >> 0) & 0xFF), \
static_cast<uint8_t>(((val) >> 8) & 0xFF)

#define TO_VECTOR_ARG_LITTLE_ENDIAN_U32(val) \
static_cast<uint8_t>(((val) >> 0)  & 0xFF), \
static_cast<uint8_t>(((val) >> 8)  & 0xFF), \
static_cast<uint8_t>(((val) >> 16) & 0xFF), \
static_cast<uint8_t>(((val) >> 24) & 0xFF)

#define TO_VECTOR_ARG_LITTLE_ENDIAN_U64(val) \
static_cast<uint8_t>(((val) >> 0)  & 0xFF), \
static_cast<uint8_t>(((val) >> 8)  & 0xFF), \
static_cast<uint8_t>(((val) >> 16) & 0xFF), \
static_cast<uint8_t>(((val) >> 24) & 0xFF), \
static_cast<uint8_t>(((val) >> 32) & 0xFF), \
static_cast<uint8_t>(((val) >> 40) & 0xFF), \
static_cast<uint8_t>(((val) >> 48) & 0xFF), \
static_cast<uint8_t>(((val) >> 56) & 0xFF)

#define SET_VECTOR_AT_LITTLE_ENDIAN_U16(buffer, index, val) \
do { \
buffer.at((index) + 0) = static_cast<uint8_t>((val) & 0xFF); \
buffer.at((index) + 1) = static_cast<uint8_t>((val >> 8) & 0xFF); \
} while (0)

#define SET_VECTOR_AT_LITTLE_ENDIAN_U32(buffer, index, val) \
do { \
buffer.at((index) + 0) = static_cast<uint8_t>((val >> 0)  & 0xFF); \
buffer.at((index) + 1) = static_cast<uint8_t>((val >> 8)  & 0xFF); \
buffer.at((index) + 2) = static_cast<uint8_t>((val >> 16) & 0xFF); \
buffer.at((index) + 3) = static_cast<uint8_t>((val >> 24) & 0xFF); \
} while (0)

#define FILL_VECTOR(buffer, index, data) \
do { \
auto _index = (index); \
for (const auto& byte : data) { \
buffer.at(_index++) = byte; \
} \
} while (0)

namespace lcl::util {
  inline std::string toHexString(const std::string& input) {
    std::string hex_str;
    for (const unsigned char c : input) {
      hex_str += std::format("{:02X} ", static_cast<unsigned int>(c));
    }
    if (!hex_str.empty()) hex_str.pop_back(); // Remove trailing space
    return hex_str;
  }

  inline std::string toHexString(const std::array<uint8_t, 256> &buffer, const uint8_t size) {
    std::string hex_str;
    for (const auto *byte = buffer.begin(); byte != buffer.begin() + size; byte++) {
      hex_str += std::format("{:02X} ", static_cast<unsigned int>(*byte));
    }
    if (!hex_str.empty()) hex_str.pop_back(); // Remove trailing space
    return hex_str;
  }

  inline std::string toHexString(const std::vector<uint8_t> &buffer) {
    std::string output;
    for (const auto byte : buffer) {
      output += std::format("{:02X} ", static_cast<unsigned int>(byte));
    }
    if (!output.empty()) output.pop_back(); // Remove trailing space
    return output;
  }

  inline std::string toDecimalString(const std::vector<uint8_t> &buffer) {
    std::string output;
    for (const auto byte : buffer) {
      output += std::format("{} ", static_cast<unsigned int>(byte));
    }
    if (!output.empty()) output.pop_back(); // Remove trailing space
    return output;
  }

  inline std::string toDecimalString(const std::array<uint8_t, 256> &buffer, const uint8_t size) {
    std::string output;
    for (const auto *byte = buffer.begin(); byte != buffer.begin() + size; byte++) {
      output += std::format("{} ", static_cast<unsigned int>(*byte));
    }
    if (!output.empty()) output.pop_back(); // Remove trailing space
    return output;
  }
}