/**
 * @file byte_stacker.hpp
 */
#ifndef COOL4_API_DATACLASS_BYTE_STACKER_HPP_
#define COOL4_API_DATACLASS_BYTE_STACKER_HPP_
#include "cool4_api_dataclass/bytearray.hpp"

#include <iostream>

namespace cool4_api_dataclass {

/**
 * @brief Stack values into byte strings
 *
 * @tparam T Value type
 * @param value Input value
 * @param data Output byte strings
 *
 * @note Big endian
 */
template<typename T>
inline void StackValue(const T& value, ByteArray& data) {
  const T* tmp = &value;
  ByteType* cursor = reinterpret_cast<ByteType*>(const_cast<T*>(tmp));
  // ビッグエンディアン
  for (size_t i = 0; i < sizeof(T); ++i) {
    data.push_back(cursor[sizeof(T) - 1 - i]);
  }
}

/**
 * @brief Stack a unsigned byte into byte strings
 *
 * @param value Input value
 * @param data Output byte strings
 */
template<>
inline void StackValue<uint8_t>(const uint8_t& value, ByteArray& data) {
  data.push_back(value);
}

/**
 * @brief Stack a signed byte into byte strings
 *
 * @param value Input value
 * @param data Output byte strings
 */
template<>
inline void StackValue<int8_t>(const int8_t& value, ByteArray& data) {
  data.push_back(value);
}

}

#endif
