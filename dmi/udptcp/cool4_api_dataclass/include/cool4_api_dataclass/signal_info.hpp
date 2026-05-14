/**
 * @file signal_info.hpp
 */
#ifndef COOL4_API_DATACLASS_SIGNAL_INFO_HPP_
#define COOL4_API_DATACLASS_SIGNAL_INFO_HPP_
#include <vector>

#include <boost/optional.hpp>

#include "cool4_api_dataclass/bytearray.hpp"
#include "cool4_api_dataclass/object_info.hpp"

namespace cool4_api_dataclass {

/**
 * @brief A data object that present light informatin in SignalInfo
 *
 * See below link for more details about this properties
 *
 * https://www.meti.go.jp/meti_lib/report/2021FY/000305.pdf
 */
struct LightInformation {
  /**
   * @brief Initialize with unknown value
   */
  LightInformation()
    : main_light(0),
      arrow_light(0),
      min_time_to_change(65535),
      max_time_to_change(65535) {}

  /**
   * @brief Initialize with specified values
   *
   * @param in_main_light Main color ID
   * @param in_arrow_light Arrow light ID
   * @param in_min_time_to_change Min time to change
   * @param in_max_time_to_change Max time to change
   */
  LightInformation(
      const uint8_t in_main_light,
      const uint8_t in_arrow_light,
      const uint16_t in_min_time_to_change,
      const uint16_t in_max_time_to_change)
    : main_light(in_main_light),
      arrow_light(in_arrow_light),
      min_time_to_change(in_min_time_to_change),
      max_time_to_change(in_max_time_to_change) {}

  /**
   * @brief Default destructor
   */
  ~LightInformation() = default;

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
  static boost::optional<LightInformation> FromByteArray(
      const ByteArray& byte_array, size_t& idx);

  /**
   * @brief A function for output to stdout, stderr
   *
   * @param os Output stream
   * @param o Own object
   *
   * @return Stream
   */
  friend std::ostream& operator<<(std::ostream& os, const LightInformation& o);

  uint8_t main_light;
  uint8_t arrow_light;
  uint16_t min_time_to_change;
  uint16_t max_time_to_change;
};

/**
 * @brief A data object that present SignalInfo
 *
 * See below link for more details about this properties
 *
 * https://www.meti.go.jp/meti_lib/report/2021FY/000305.pdf
 */
struct SignalInfo {
  using Array = std::vector<SignalInfo>;

  enum class MainColor : uint8_t {
    kUnknown = 0,
    kDark,
    kFlashingRed,
    kRedLight,
    kReserved1,
    kPermissiveGreen,
    kReserved2,
    kPermissiveYellow,
    kReserved3,
    kFlashingYellow
  };

  /**
   * @brief Number of properties
   */
  static constexpr size_t ElementNum = 55;

  /**
   * @brief Size of signal ID list
   */
  static constexpr size_t SignalIdNum = 8;
  using SignalIdList = std::array<uint8_t, SignalIdNum>;

  /**
   * @brief Size of light informatin list
   */
  static constexpr size_t LightInformationNum = 12;
  using LightInformationList = std::array<LightInformation, LightInformationNum>;

  /**
   * @brief Initialize with unknown value
   */
  SignalInfo()
    : crp_id(0),
      signal_id_list(SignalIdList()),
      time(0),
      state(0),
      specific_control_flags(0),  // unknown value is under consideration
      event_count(0),  // unknown value is under consideration
      count_down_stop_flag(0),
      light_information_list(LightInformationList()) {}

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
  static boost::optional<SignalInfo> FromByteArray(
      const ByteArray& byte_array, size_t& idx);

  /**
   * @brief A function for output to stdout, stderr
   *
   * @param os Output stream
   * @param o Own object
   *
   * @return Stream
   */
  friend std::ostream& operator<<(std::ostream& os, const SignalInfo& o);

  uint32_t crp_id;
  SignalIdList signal_id_list;
  uint64_t time;
  uint8_t state;
  uint8_t specific_control_flags;
  uint8_t event_count;
  uint8_t count_down_stop_flag;
  LightInformationList light_information_list;
};

}
#endif
