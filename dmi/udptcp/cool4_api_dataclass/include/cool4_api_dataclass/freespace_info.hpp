/**
 * @file freespace_info.hpp
 */
#ifndef COOL4_API_DATACLASS_FREESPACE_INFO_HPP_
#define COOL4_API_DATACLASS_FREESPACE_INFO_HPP_
#include <array>

#include <boost/optional.hpp>

#include "cool4_api_dataclass/bytearray.hpp"
#include "cool4_api_dataclass/object_info.hpp"


namespace cool4_api_dataclass {

/**
 * @brief A data object that present FreespaceInfo
 *
 * See below link for more details about this properties
 *
 * https://www.meti.go.jp/meti_lib/report/2021FY/000305.pdf
 */
struct FreespaceInfo {
  using Array = std::vector<FreespaceInfo>;
  using LengthType = ValueWithAccuracy<uint16_t, uint16_t>;

  /**
   * @brief Number of properties
   */
  static constexpr size_t ElementNum = 92;

  /**
   * @brief Size of information source list
   */
  static constexpr size_t InformationSourceListLength = 4;
  using InformationSourceList = std::array<unsigned long long, InformationSourceListLength>;

  /**
   * @brief Initialize properties with unknown value
   */
  FreespaceInfo()
    : id(0),
      time(0),
      revision(0),
      detection_method(0),
      detectable_class(0),
      vertices_begin(Location()),
      vertices_num(0),
      vertices_x_list(std::vector<int32_t>()),
      vertices_y_list(std::vector<int32_t>()),
      position_begin(Location()),
      position_end(Location()),
      length(65535, 65535),
      id_begin(0),
      id_end(0),
      existency(0),
      minimal_detectable_size(65535),
      info_src_list({0, 0, 0, 0}) {}

  /**
   * @brief Default destructor
   */
  virtual ~FreespaceInfo() = default;

  /**
   * @brief Convert own information to byte array
   *
   * @return Byte array or none
   */
  virtual boost::optional<ByteArray> ToByteArray();

  /**
   * @brief Construct data by using byte array
   *
   * @param byte_array Input data
   *
   * @return Data object or none
   */
  static boost::optional<FreespaceInfo> FromByteArray(const ByteArray& byte_array, size_t& idx);

  /**
   * @brief A function for output to stdout, stderr
   *
   * @param os Output stream
   * @param o Own object
   *
   * @return Stream
   */
  friend std::ostream& operator<<(std::ostream& os, const FreespaceInfo& o);

  unsigned long long id;
  unsigned long long time;
  uint8_t revision;
  uint8_t detection_method;
  uint8_t detectable_class;
  Location vertices_begin;
  uint8_t vertices_num;
  std::vector<int32_t> vertices_x_list;
  std::vector<int32_t> vertices_y_list;
  Location position_begin;
  Location position_end;
  LengthType length;
  unsigned long long id_begin;
  unsigned long long id_end;
  uint8_t existency;
  uint16_t minimal_detectable_size;
  InformationSourceList info_src_list;
};

}

#endif
