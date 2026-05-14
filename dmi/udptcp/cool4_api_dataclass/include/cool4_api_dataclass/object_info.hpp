/**
 * @file object_info.hpp
 */
#ifndef COOL4_API_DATACLASS_OBJECT_INFO_HPP_
#define COOL4_API_DATACLASS_OBJECT_INFO_HPP_
#include <array>
#include <map>

#include <boost/optional.hpp>

#include "cool4_api_dataclass/bytearray.hpp"
#include "cool4_api_dataclass/byte_stacker.hpp"
#include "cool4_api_dataclass/byte_extractor.hpp"

namespace cool4_api_dataclass {

/**
 * @brief A data object that present ObjectClass
 *
 * See below link for more details about this properties
 *
 * https://www.meti.go.jp/meti_lib/report/2021FY/000305.pdf
 */
struct ObjectClass {
  /**
   * @brief Initialize properties with unknown value
   */
  ObjectClass() : id(0), confidence(0), subclass_type(0), subclass_confidence(0) {}

  /**
   * @brief Default destructor
   */
  ~ObjectClass() = default;

  enum class ClassId : uint8_t {
    kUnknown,
    kVehicle,
    kPerson,
    kAnimal,
    kOther,
  };

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
  static boost::optional<ObjectClass> FromByteArray(const ByteArray& byte_array, size_t& idx);

  /**
   * @brief A function for output to stdout, stderr
   *
   * @param os Output stream
   * @param o Own object
   *
   * @return Stream
   */
  friend std::ostream& operator<<(std::ostream& os, const ObjectClass& o);


  uint8_t id;
  uint8_t confidence;
  uint8_t subclass_type;
  uint8_t subclass_confidence;
};


/**
 * @brief A data object that present Location
 *
 * See below link for more details about this properties
 *
 * https://www.meti.go.jp/meti_lib/report/2021FY/000305.pdf
 */
struct Location {
  /**
   * @brief Initialize properties with unknown value
   */
  Location()
    : geodetic_srid(0),
      latitude(900000001),
      longitude(1800000001),
      altitude(800001),
      projective_srid(0),
      prc_x(-132768),
      prc_y(-132768),
      prc_z(-132768),
      crp_id(0),
      dx_crp(-132768),
      dy_crp(-132768),
      dh_crp(-132768),
      lane_count(15),
      lane_position(17),
      lane_lateral_position(101),
      crp_id_begin(0),
      crp_id_end(0),
      lane_vertical_position(10001),
      lane_id(0),
      dx_lane(-132768),
      dy_lane(-132768),
      dh_lane(-132768),
      semi_axis_length_major(4095),
      semi_axis_length_minor(4095),
      orientation(36001),
      altitude_accuracy(20001) {}

  /**
   * @brief Default destructor
   */
  ~Location() = default;

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
  static boost::optional<Location> FromByteArray(const ByteArray& byte_array, size_t& idx);

  /**
   * @brief A function for output to stdout, stderr
   *
   * @param os Output stream
   * @param o Own object
   *
   * @return Stream
   */
  friend std::ostream& operator<<(std::ostream& os, const Location& o);

  enum class SridValue : uint32_t {
    kWgs84 = 4326,
    kJgd2011 = 6668,
    kRpc01 = 6669,
    kRpc02 = 6670,
    kRpc03 = 6671,
    kRpc04 = 6672,
    kRpc05 = 6673,
    kRpc06 = 6674,
    kRpc07 = 6675,
    kRpc08 = 6676,
    kRpc09 = 6677,
    kRpc10 = 6678,
    kRpc11 = 6679,
    kRpc12 = 6680,
    kRpc13 = 6681,
    kRpc14 = 6682,
    kRpc15 = 6683,
    kRpc16 = 6684,
    kRpc17 = 6685,
    kRpc18 = 6686,
    kRpc19 = 6687,
  };

  uint32_t geodetic_srid;
  int32_t latitude;
  int32_t longitude;
  int32_t altitude;

  uint32_t projective_srid;
  int32_t prc_x;
  int32_t prc_y;
  int32_t prc_z;

  uint32_t crp_id;
  int32_t dx_crp;
  int32_t dy_crp;
  int32_t dh_crp;

  uint8_t lane_count;
  int8_t lane_position;
  uint8_t lane_lateral_position;
  uint32_t crp_id_begin;
  uint32_t crp_id_end;
  uint16_t lane_vertical_position;

  unsigned long long lane_id;
  int32_t dx_lane;
  int32_t dy_lane;
  int32_t dh_lane;

  uint16_t semi_axis_length_major;
  uint16_t semi_axis_length_minor;
  uint16_t orientation;
  uint16_t altitude_accuracy;
};

/**
 * @brief A template class that have value and accuracy
 *
 * @tparam ValueType Type of value
 * @tparam AccuracyType Type of accuracy
 */
template<typename ValueType, typename AccuracyType>
struct ValueWithAccuracy {
  /**
   * @brief Initialize with zero value
   */
  ValueWithAccuracy() : value(ValueType(0)), accuracy(AccuracyType(0)) {}

  /**
   * @brief Initialize with specified values
   *
   * @param v Value
   * @param a Accuracy
   */
  ValueWithAccuracy(const ValueType& v, const AccuracyType& a) : value(v), accuracy(a) {}

  /**
   * @brief Convert own information to byte array
   *
   * @return Byte array or none
   */
  boost::optional<ByteArray> ToByteArray() const {
    ByteArray out;
    StackValue<ValueType>(this->value, out);
    StackValue<AccuracyType>(this->accuracy, out);
    return out;
  }

  /**
   * @brief Construct data by using byte array
   *
   * @param byte_array Input data
   *
   * @return Data object or none
   */
  static boost::optional<ValueWithAccuracy<ValueType, AccuracyType>> FromByteArray(
      const ByteArray& byte_array, size_t& idx) {
    ValueWithAccuracy<ValueType, AccuracyType> out;
    out.value = ExtractValue<ValueType>(byte_array, idx);
    out.accuracy = ExtractValue<AccuracyType>(byte_array, idx);
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
  friend std::ostream& operator<<(
      std::ostream& os, const ValueWithAccuracy<ValueType, AccuracyType>& o) {
    os << (unsigned)o.value << "," << (unsigned)o.accuracy << ",";
    return os;
  }

  ValueType value;
  AccuracyType accuracy;
};

/**
 * @brief A data object that present ObjectInfo
 *
 * See below link for more details about this properties
 *
 * https://www.meti.go.jp/meti_lib/report/2021FY/000305.pdf
 */
struct ObjectInfo {
  using Array = std::vector<ObjectInfo>;

  /**
   * @brief Number of properties
   */
  static constexpr size_t ElementNum = 88;

  /**
   * @brief Size of object class list
   */
  static constexpr size_t ObjectClassLength = 4;
  using ObjectClassFixedArray = std::array<ObjectClass, ObjectClassLength>;

  /**
   * @brief Size of information source list
   */
  static constexpr size_t InformationSourceListLength = 4;
  using InformationSourceList = std::array<unsigned long long, InformationSourceListLength>;

  /**
   * @brief Initialize properties with unknown value
   */
  ObjectInfo()
    : id(0),
      time(0),
      revision(0),
      object_classes(ObjectClassFixedArray()),
      existency(0),
      object_location(Location()),
      ref_point(0),
      move_direction(36001, 9001),
      speed(16383, 16383),
      yaw_rate(32767, 32767),
      acceleration(2001, 1001),
      orientation(36001, 9001),
      length(65535, 65535),
      width(65535, 65535),
      height(65535, 65535),
      static_status(0),
      shift_position(7),
      steering_angle_front(91),
      steering_angle_rear(91),
      brake_state(0),
      auxiliary_brake_state(0),
      throttle_position(255),
      exterior_lights(0),
      acc_state(0),
      c_acc_state(0),
      pcs_state(0),
      abs_state(0),
      trc_state(0),
      esc_state(0),
      lka_state(0),
      ldw_state(0),
      vehicle_size_type(0),
      vehicle_role(15),
      vehicle_extended_info(255),
      towing_vehicle(0),
      info_src_list({0, 0, 0, 0}) {}

  /**
   * @brief Default destructor
   */
  virtual ~ObjectInfo() = default;

  /**
   * @brief Convert own information to byte array
   *
   * @return Byte array or none
   */
  virtual boost::optional<ByteArray> ToByteArray() ;

  /**
   * @brief Construct data by using byte array
   *
   * @param byte_array Input data
   *
   * @return Data object or none
   */
  static boost::optional<ObjectInfo> FromByteArray(const ByteArray& byte_array, size_t& idx);

  /**
   * @brief A function for output to stdout, stderr
   *
   * @param os Output stream
   * @param o Own object
   *
   * @return Stream
   */
  friend std::ostream& operator<<(std::ostream& os, const ObjectInfo& o);

  unsigned long long id;
  unsigned long long time;
  uint8_t revision;
  ObjectClassFixedArray object_classes;
  uint8_t existency;
  Location object_location;
  uint8_t ref_point;

  ValueWithAccuracy<uint16_t, uint16_t> move_direction;
  ValueWithAccuracy<int16_t, uint16_t> speed;
  ValueWithAccuracy<int16_t, uint16_t> yaw_rate;
  ValueWithAccuracy<int16_t, uint16_t> acceleration;
  ValueWithAccuracy<uint16_t, uint16_t> orientation;

  ValueWithAccuracy<int16_t, uint16_t> length;
  ValueWithAccuracy<int16_t, uint16_t> width;
  ValueWithAccuracy<int16_t, uint16_t> height;

  uint16_t static_status;

  uint8_t shift_position;
  int8_t steering_angle_front;
  int8_t steering_angle_rear;
  uint8_t brake_state;
  uint8_t auxiliary_brake_state;
  uint8_t throttle_position;
  uint8_t exterior_lights;

  uint8_t acc_state;
  uint8_t c_acc_state;
  uint8_t pcs_state;
  uint8_t abs_state;
  uint8_t trc_state;
  uint8_t esc_state;
  uint8_t lka_state;
  uint8_t ldw_state;

  uint8_t vehicle_size_type;
  uint8_t vehicle_role;
  uint8_t vehicle_extended_info;
  unsigned long long towing_vehicle;

  uint8_t tracking_status;
  uint16_t detection_count;
  uint8_t lost_count;
  uint16_t object_age;

  InformationSourceList info_src_list;
};

}

#endif
