/**
 * @file byte_extractor.hpp
 *
 * Extract any of defined type from byte array
 */
#ifndef COOL4_API_DATACLASS_BYTE_EXTRACTOR_HPP_
#define COOL4_API_DATACLASS_BYTE_EXTRACTOR_HPP_
#include "cool4_api_dataclass/bytearray.hpp"

#include <iostream>
#include <algorithm>

namespace cool4_api_dataclass {

/**
 * @brief Extract a value from a sequence of bytes based on a specified position
 *
 * @tparam T Value type to be extracted.
 * @param array Sequence of bytes
 * @param idx Base position in a sequence of bytes
 *
 * @note Big endian
 *
 * @return Extracted value
 */
template<class T>
inline T ExtractValue(const ByteArray& array, size_t& idx) {
  static_assert(sizeof(T) != sizeof(uint8_t), "size of template type must greater than 1");
  std::vector<uint8_t> elements(sizeof(T));
  for (size_t i = 0; i < sizeof(T); ++i) {
    elements[i] = ExtractValue<uint8_t>(array, idx);
  }
  std::reverse(elements.begin(), elements.end());
  return static_cast<T>(*(reinterpret_cast<T*>(&elements[0])));
}

template<class T>
inline T ExtractSwappedValue(const ByteArray& array, size_t& idx) {
  static_assert(sizeof(T) != sizeof(uint8_t), "size of template type must greater than 1");
  std::vector<uint8_t> elements(sizeof(T));
  for (size_t i = 0; i < sizeof(T); ++i) {
    elements[i] = ExtractValue<uint8_t>(array, idx);
  }
  return static_cast<T>(*(reinterpret_cast<T*>(&elements[0])));
}

/**
 * @brief Extract a unsigned byte from a sequence of bytes based on a specified position
 *
 * @param array Sequence of bytes
 * @param idx Base position in a sequence of bytes
 *
 * @return Extracted value
 */
template<>
inline uint8_t ExtractValue<uint8_t>(const ByteArray& array, size_t& idx) {
  const uint8_t value = static_cast<uint8_t>(array[idx]);
  idx += sizeof(uint8_t);
  return value;
}

/**
 * @brief Extract a signed byte from a sequence of bytes based on a specified position
 *
 * @param array Sequence of bytes
 * @param idx Base position in a sequence of bytes
 *
 * @return Extracted value
 */
template<>
inline int8_t ExtractValue<int8_t>(const ByteArray& array, size_t& idx) {
  const int8_t value = static_cast<uint8_t>(array[idx]);
  idx += sizeof(int8_t);
  return value;
}

}
#endif
