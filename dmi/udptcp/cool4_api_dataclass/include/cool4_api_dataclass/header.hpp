/**
 * @file header.hpp
 */
#ifndef COOL4_API_DATACLASS_HEADER_HPP_
#define COOL4_API_DATACLASS_HEADER_HPP_
#include <boost/optional.hpp>
#include <iostream>
#include <memory>
#include <string>

#include "cool4_api_dataclass/bytearray.hpp"

namespace cool4_api_dataclass {

/**
 * @brief Header object
 *
 * See CooL4データ連携PF UDPメッセージフォーマット.xlsx for more detail about properties
 */
struct Header {
  /**
   * @brief Sum size of properties
   */
  static constexpr size_t Size = 7;

  using SharedPtr = std::shared_ptr<Header>;

  /**
   * @brief Initialize properties with zero value
   */
  Header()
    : version_major(0),
      version_minor(0),
      version_patch(0),
      seq(0),
      frame_id(0),
      frame_num(0),
      info_num(0) {}

  /**
   * @brief Initialize properties with specified values
   *
   * @param version_major_in Major version
   * @param version_minor_in Minor version
   * @param version_patch_in Patch version
   * @param seq_in Sequence number
   * @param frame_id_in Frame ID
   * @param frame_num_in Frame mumber
   * @param info_num_in Info number of contained message in this frame
   */
  Header(
      const uint8_t version_major_in,
      const uint8_t version_minor_in,
      const uint8_t version_patch_in,
      const uint8_t seq_in,
      const uint8_t frame_id_in,
      const uint8_t frame_num_in,
      const uint8_t info_num_in)
    : version_major(version_major_in),
      version_minor(version_minor_in),
      version_patch(version_patch_in),
      seq(seq_in),
      frame_id(frame_id_in),
      frame_num(frame_num_in),
      info_num(info_num_in) {}

  /**
   * @brief Convert own information to byte array
   *
   * @return Byte array or none
   */
  boost::optional<ByteArray> ToByteArray() const;

  /**
   * @brief Construct data by using byte array
   *
   * @param byte_array Input data
   *
   * @return Data object or none
   */
  static boost::optional<Header> FromByteArray(const ByteArray& byte_array, size_t& idx);

  /**
   * @brief A function for output to stdout, stderr
   *
   * @param os Output stream
   * @param o Own object
   *
   * @return Stream
   */
  friend std::ostream& operator<<(std::ostream& os, const Header& o);

  uint8_t version_major;
  uint8_t version_minor;
  uint8_t version_patch;
  uint8_t seq;
  uint8_t frame_id;
  uint8_t frame_num;
  uint8_t info_num;
};

}

#endif
