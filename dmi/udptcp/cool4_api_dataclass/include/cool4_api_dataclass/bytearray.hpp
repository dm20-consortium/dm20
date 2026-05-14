/**
 * @file bytearray.hpp
 */
#ifndef COOL4_API_DATACLASS_BYTEARRAY_HPP_
#define COOL4_API_DATACLASS_BYTEARRAY_HPP_
#include <vector>

namespace cool4_api_dataclass {

/**
 * @brief Alias of bytearray elements
 */
using ByteType = unsigned char;

/**
 * @brief Alias of bytearray
 */
using ByteArray = std::vector<ByteType>;

}

#endif
