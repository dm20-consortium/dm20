/**
 * @file dataclasses_with_header.hpp
 *
 * Data container that have Dataclass array and header
 */
#ifndef COOL4_API_DATACLASS_DATACLASSES_WITH_HEADER_HPP_
#define COOL4_API_DATACLASS_DATACLASSES_WITH_HEADER_HPP_
#include <vector>

#include <boost/optional.hpp>

#include "cool4_api_dataclass/bytearray.hpp"
#include "cool4_api_dataclass/header.hpp"

namespace cool4_api_dataclass {

/**
 * @brief A data container for information with header object.
 *
 * @tparam T Information type(ex: ObjectInfo, FreespaceInfo, SignalInfo)
 */
template<class T>
struct DataClassesWithHeader {
  /**
   * @brief Constructor
   */
  DataClassesWithHeader() = default;

  /**
   * @brief Destructor
   */
  ~DataClassesWithHeader() = default;

  /**
   * @brief Convert own information to byte array
   *
   * @return Byte array or none
   */
  boost::optional<ByteArray> ToByteArray() {
    ByteArray out;

    const auto bytes_header = this->header.ToByteArray().get();
    out.insert(out.end(), bytes_header.begin(), bytes_header.end());

    for (auto data : dataclasses) {
      auto bytes_data = data.ToByteArray().get();
      out.insert(out.end(), bytes_data.begin(), bytes_data.end());
    }

    return out;
  }

  /**
   * @brief Construct data by using byte array
   *
   * @param byte_array Input data
   *
   * @return Data with header object or none
   */
  static boost::optional<DataClassesWithHeader<T>> FromByteArray(const ByteArray& byte_array) {
    DataClassesWithHeader<T> out;
    size_t idx = 0;
    const auto header = Header::FromByteArray(byte_array, idx);
    if (header == boost::none) {
      return boost::none;
    }
    out.header = header.get();

    for (uint8_t i = 0; i < out.header.info_num; ++i) {
      const auto dataclass = T::FromByteArray(byte_array, idx);
      if (dataclass == boost::none) return boost::none;
      out.dataclasses.push_back(dataclass.get());
    }

    return out;
  }

  /**
   * @brief A function for output to stdout, stderr
   *
   * @param os Output stream
   * @param o Own object
   *
   * @return Stream
   */
  friend std::ostream& operator<<(std::ostream& os, const DataClassesWithHeader<T>& o) {
    os << o.header;
    for (const auto& e : o.dataclasses) os << e;
    return os;
  }

  /**
   * @brief Header object
   */
  Header header;

  /**
   * @brief Data object list
   */
  std::vector<T> dataclasses;
};

}

#endif
