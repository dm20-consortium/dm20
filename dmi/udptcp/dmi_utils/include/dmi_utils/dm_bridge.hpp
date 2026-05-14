/**
 * @file dm_bridge.hpp
 */
#ifndef DMI_UTILS_DM_BRIDGE_HPP_
#define DMI_UTILS_DM_BRIDGE_HPP_
#include <sstream>

#include <boost/optional.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <is/DmUtil.h>
#include <is/ResultSet.h>
#include <is/Tuple.h>
#include <is/TupleSet.h>
#include <glog/logging.h>

#include <cool4_api_dataclass/object_info.hpp>
#include <cool4_api_dataclass/freespace_info.hpp>
#include <cool4_api_dataclass/signal_info.hpp>
#include <cool4_api_dataclass/sensor_info.hpp>

namespace dmi_utils {

template<typename T>
inline T ExtractTupleValue(const uint32_t idx, IS::Tuple& t) {
  // anyはis/Tuple.hで規定しているものを使う
  any any_val;
  t.getValueByIdx(idx, any_val);
  T val;
  try {
    val = any_cast<T>(any_val);
  } catch (const std::exception& e) {
    VLOG(3)
      << "failed to cast at table(" << t.getTableName() << ") idx: " << idx << std::endl;

    if (VLOG_IS_ON(4)) {
      t.dumpAny(any_val);
    }
    throw std::exception();
  }
  return val;
}

template<class T>
std::vector<T> ExtractTupleValues(const uint32_t idx, IS::Tuple& tpl)
{
  std::vector<T> values;
  try {
    values = ExtractTupleValue<std::vector<T>>(idx, tpl);
  } catch (...) {
    const std::vector<any> tmp = ExtractTupleValue<std::vector<any>>(idx, tpl);
    for (const auto& e : tmp)
    {
      values.push_back(any_cast<T>(e));
    }
  }

  return values;
}

const char* const kTupleNameObjectInfo = "object_info_0_8_1";
const char* const kTupleNameFreespaceInfo = "freespace_info_0_8_1";
const char* const kTupleNameSensorInfo = "sensor_info_0_8_1";
const char* const kTupleNameSignalInfo = "signal_info";

}

namespace dmi_utils {

using cool4_api_dataclass::ObjectClass;
using cool4_api_dataclass::ObjectInfo;
using cool4_api_dataclass::Location;
using cool4_api_dataclass::ValueWithAccuracy;
using cool4_api_dataclass::FreespaceInfo;
using cool4_api_dataclass::LightInformation;
using cool4_api_dataclass::SignalInfo;
using cool4_api_dataclass::SensorInfo;
using cool4_api_dataclass::DetectableCapability;

/**
 * @brief Base struct
 *
 * @tparam T Object type
 */
template<class T>
struct DmBridge {};

/**
 * @brief Bridge class between Tuple and ObjectClass
 */
template<>
struct DmBridge<ObjectClass> {
  /**
   * @brief Construct a instance of this class with query result
   *
   * @param rs Query result
   * @param idx index of object class
   *
   * @note
   *
   * object_infoストリームのObjectClassは複数個設定できる関係上，列名に番号が振られている
   *
   * 統一感は損なわれるが，仕方なくidxを設定できるようにした
   *
   * @return A instance of this class or none
   */
  static boost::optional<ObjectClass> FromResultSet(ResultSet& rs, const int idx = 0) {
    ObjectClass out;
    std::vector<uint8_t*> out_indicator = {
      &(out.id),
      &(out.confidence),
      &(out.subclass_type),
      &(out.subclass_confidence)};

    const std::vector<std::string> object_class_pref = {
      "object_class_id",
      "object_class_confidence",
      "object_class_subclass_type",
      "object_class_subclass_confidence"};

    for (size_t i = 0; i < object_class_pref.size(); ++i) {
      std::stringstream ss;
      ss << object_class_pref[i] << "_" << idx;
      *(out_indicator[i]) = rs.getInt(ss.str());
    }

    return out;
  }

  static boost::optional<ObjectClass> FromTuple(IS::Tuple& tpl, unsigned int& begin_idx) {
    ObjectClass out;
    std::vector<uint8_t*> out_indicator = {
      &(out.id),
      &(out.confidence),
      &(out.subclass_type),
      &(out.subclass_confidence)};

    const std::vector<std::string> object_class_pref = {
      "object_class_id",
      "object_class_confidence",
      "object_class_subclass_type",
      "object_class_subclass_confidence"};

    for (size_t i = 0; i < object_class_pref.size(); ++i) {
      *(out_indicator[i]) = ExtractTupleValue<int>(++begin_idx, tpl);
    }

    return out;
  }

  static void FromDataclass(
      const ObjectClass& in, const long stamp, uint32_t& idx, IS::Tuple& out) {
    out.setValue(idx++, static_cast<int>(in.id), stamp);
    out.setValue(idx++, static_cast<int>(in.confidence), stamp);
    out.setValue(idx++, static_cast<int>(in.subclass_type), stamp);
    out.setValue(idx++, static_cast<int>(in.subclass_confidence), stamp);
  }
};

/**
 * @brief Bridge class between Tuple and Location
 */
template<>
struct DmBridge<Location> {
  /**
   * @brief Construct a instance of this class with query result
   *
   * @param rs Query result
   * @param pref prefix of column name
   *
   * @note
   *
   * ストリームによってはアクセスするときの名前が変わるので，接頭詞をつけられるようにしてある
   *
   * @return A instance of this class or none
   */
  static boost::optional<Location> FromResultSet(
      ResultSet& rs, const std::string& pref = std::string()) {
    Location out;
    out.geodetic_srid = rs.getInt(pref + "geodetic_srid");
    out.latitude = rs.getInt(pref + "latitude");
    out.longitude = rs.getInt(pref + "longitude");
    out.altitude = rs.getInt(pref + "altitude");
    out.projective_srid = rs.getInt(pref + "projective_srid");
    out.prc_x = rs.getInt(pref + "prc_x");
    out.prc_y = rs.getInt(pref + "prc_y");
    out.prc_z = rs.getInt(pref + "prc_z");
    out.crp_id = rs.getInt(pref + "crp_id");
    out.dx_crp = rs.getInt(pref + "dx_crp");
    out.dy_crp = rs.getInt(pref + "dy_crp");
    out.dh_crp = rs.getInt(pref + "dh_crp");
    out.lane_count = rs.getInt(pref + "lane_count");
    out.lane_position = rs.getInt(pref + "lane_position");
    out.lane_lateral_position = rs.getInt(pref + "lane_lateral_position");
    out.crp_id_begin = rs.getInt(pref + "crp_id_begin");
    out.crp_id_end = rs.getInt(pref + "crp_id_end");
    out.lane_vertical_position = rs.getInt(pref + "lane_vertical_position");
    out.lane_id = rs.getUnsignedLongLong(pref + "lane_id");
    out.dx_lane = rs.getInt(pref + "dx_lane");
    out.dy_lane = rs.getInt(pref + "dy_lane");
    out.dh_lane = rs.getInt(pref + "dh_lane");
    out.semi_axis_length_major = rs.getInt(pref + "semi_axis_length_major");
    out.semi_axis_length_minor = rs.getInt(pref + "semi_axis_length_minor");
    out.orientation = rs.getInt(pref + "ellipse_orientation");
    out.altitude_accuracy = rs.getInt(pref + "altitude_accuracy");

    return out;
  }

  static boost::optional<Location> FromTuple(IS::Tuple& tuple, unsigned int& idx) {
    Location out;
    out.geodetic_srid = ExtractTupleValue<int>(++idx, tuple);
    out.latitude = ExtractTupleValue<int>(++idx, tuple);
    out.longitude = ExtractTupleValue<int>(++idx, tuple);
    out.altitude = ExtractTupleValue<int>(++idx, tuple);
    out.projective_srid = ExtractTupleValue<int>(++idx, tuple);
    out.prc_x = ExtractTupleValue<int>(++idx, tuple);
    out.prc_y = ExtractTupleValue<int>(++idx, tuple);
    out.prc_z = ExtractTupleValue<int>(++idx, tuple);
    out.crp_id = ExtractTupleValue<int>(++idx, tuple);
    out.dx_crp = ExtractTupleValue<int>(++idx, tuple);
    out.dy_crp = ExtractTupleValue<int>(++idx, tuple);
    out.dh_crp = ExtractTupleValue<int>(++idx, tuple);
    out.lane_count = ExtractTupleValue<int>(++idx, tuple);
    out.lane_position = ExtractTupleValue<int>(++idx, tuple);
    out.lane_lateral_position = ExtractTupleValue<int>(++idx, tuple);
    out.crp_id_begin = ExtractTupleValue<int>(++idx, tuple);
    out.crp_id_end = ExtractTupleValue<int>(++idx, tuple);
    out.lane_vertical_position = ExtractTupleValue<int>(++idx, tuple);
    out.lane_id = ExtractTupleValue<unsigned long long>(++idx, tuple);
    out.dx_lane = ExtractTupleValue<int>(++idx, tuple);
    out.dy_lane = ExtractTupleValue<int>(++idx, tuple);
    out.dh_lane = ExtractTupleValue<int>(++idx, tuple);
    out.semi_axis_length_major = ExtractTupleValue<int>(++idx, tuple);
    out.semi_axis_length_minor = ExtractTupleValue<int>(++idx, tuple);
    out.orientation = ExtractTupleValue<int>(++idx, tuple);
    out.altitude_accuracy = ExtractTupleValue<int>(++idx, tuple);

    return out;
  }

  static void FromDataclass(const Location& in, const long stamp, uint32_t& idx, IS::Tuple& out) {
    out.setValue(idx++, static_cast<int>(in.geodetic_srid), stamp);
    out.setValue(idx++, static_cast<int>(in.latitude), stamp);
    out.setValue(idx++, static_cast<int>(in.longitude), stamp);
    out.setValue(idx++, static_cast<int>(in.altitude), stamp);

    out.setValue(idx++, static_cast<int>(in.projective_srid), stamp);
    out.setValue(idx++, static_cast<int>(in.prc_x), stamp);
    out.setValue(idx++, static_cast<int>(in.prc_y), stamp);
    out.setValue(idx++, static_cast<int>(in.prc_z), stamp);

    out.setValue(idx++, static_cast<int>(in.crp_id), stamp);
    out.setValue(idx++, static_cast<int>(in.dx_crp), stamp);
    out.setValue(idx++, static_cast<int>(in.dy_crp), stamp);
    out.setValue(idx++, static_cast<int>(in.dh_crp), stamp);

    out.setValue(idx++, static_cast<int>(in.lane_count), stamp);
    out.setValue(idx++, static_cast<int>(in.lane_position), stamp);
    out.setValue(idx++, static_cast<int>(in.lane_lateral_position), stamp);
    out.setValue(idx++, static_cast<int>(in.crp_id_begin), stamp);
    out.setValue(idx++, static_cast<int>(in.crp_id_end), stamp);
    out.setValue(idx++, static_cast<int>(in.lane_vertical_position), stamp);

    out.setValue(idx++, in.lane_id, stamp);
    out.setValue(idx++, static_cast<int>(in.dx_lane), stamp);
    out.setValue(idx++, static_cast<int>(in.dy_lane), stamp);
    out.setValue(idx++, static_cast<int>(in.dh_lane), stamp);

    out.setValue(idx++, static_cast<int>(in.semi_axis_length_major), stamp);
    out.setValue(idx++, static_cast<int>(in.semi_axis_length_minor), stamp);
    out.setValue(idx++, static_cast<int>(in.orientation), stamp);
    out.setValue(idx++, static_cast<int>(in.altitude_accuracy), stamp);
  }
};

/**
 * @brief Bridge class between Tuple and ValueWithAccuracy<uint16_t, uint16_t>
 */
template<>
struct DmBridge<ValueWithAccuracy<uint16_t, uint16_t>> {
  /**
   * @brief Construct a instance of this class with query result
   *
   * @param rs Query result
   * @param name Base name of column
   *
   * ex) If you need speed_value and speed_accuracy, set speed as the name
   *
   * @return A instance of this class or none
   */
  static boost::optional<ValueWithAccuracy<uint16_t, uint16_t>> FromResultSet(ResultSet& rs, const std::string& name = std::string()) {
    ValueWithAccuracy<uint16_t, uint16_t> out;
    out.value = static_cast<uint16_t>(rs.getInt(name + "_value"));
    out.accuracy = static_cast<uint16_t>(rs.getInt(name + "_accuracy"));
    return out;
  }

  static boost::optional<ValueWithAccuracy<uint16_t, uint16_t>> FromTuple(IS::Tuple& tpl, unsigned int& begin_idx) {
    ValueWithAccuracy<uint16_t, uint16_t> out;
    out.value = static_cast<uint16_t>(ExtractTupleValue<int>(++begin_idx, tpl));
    out.accuracy = static_cast<uint16_t>(ExtractTupleValue<int>(++begin_idx, tpl));
    return out;
  }

  static void FromDataclass(const ValueWithAccuracy<uint16_t, uint16_t>& in, const long stamp, uint32_t& idx, IS::Tuple& out) {
    out.setValue(idx++, static_cast<int>(in.value), stamp);
    out.setValue(idx++, static_cast<int>(in.accuracy), stamp);
  }
};

/**
 * @brief Bridge class between Tuple and ValueWithAccuracy<int16_t, uint16_t>
 */
template<>
struct DmBridge<ValueWithAccuracy<int16_t, uint16_t>> {
  /**
   * @brief Construct a instance of this class with query result
   *
   * @param rs Query result
   * @param name Base name of column
   *
   * ex) If you need speed_value and speed_accuracy, set speed as the name
   *
   * @return A instance of this class or none
   */
  static boost::optional<ValueWithAccuracy<int16_t, uint16_t>> FromResultSet(ResultSet& rs, const std::string& name = std::string()) {
    ValueWithAccuracy<int16_t, uint16_t> out;
    out.value = static_cast<int16_t>(rs.getInt(name + "_value"));
    out.accuracy = static_cast<uint16_t>(rs.getInt(name + "_accuracy"));
    return out;
  }

  static boost::optional<ValueWithAccuracy<int16_t, uint16_t>> FromTuple(IS::Tuple& tpl, unsigned int& begin_idx) {
    ValueWithAccuracy<int16_t, uint16_t> out;
    out.value = static_cast<int16_t>(ExtractTupleValue<int>(++begin_idx, tpl));
    out.accuracy = static_cast<uint16_t>(ExtractTupleValue<int>(++begin_idx, tpl));
    return out;
  }

  static void FromDataclass(const ValueWithAccuracy<int16_t, uint16_t>& in, const long stamp, uint32_t& idx, IS::Tuple& out) {
    out.setValue(idx++, static_cast<int>(in.value), stamp);
    out.setValue(idx++, static_cast<int>(in.accuracy), stamp);
  }
};

/**
 * @brief Bridge class between Tuple and ObjectInfo
 */
template<>
struct DmBridge<ObjectInfo> {
  /**
   * @brief Construct a instance of this class with query result
   *
   * @param rs Query result
   *
   * @return A instance of this class or none
   */
  static boost::optional<ObjectInfo> FromResultSet(ResultSet& rs) {
    ObjectInfo out;

    out.id = rs.getUnsignedLongLong("object_id");
    out.time = static_cast<uint64_t>(rs.getLong("time"));
    out.revision = static_cast<uint8_t>(rs.getInt("revision"));

    for (size_t i = 0; i < out.object_classes.size(); ++i) {
      const auto opt = DmBridge<ObjectClass>::FromResultSet(rs, i);
      if (opt == boost::none) continue;
      out.object_classes[i] = opt.get();
    }

    out.existency = static_cast<uint8_t>(rs.getInt("existency"));
    out.object_location = DmBridge<Location>::FromResultSet(rs).get();
    out.ref_point = static_cast<uint8_t>(rs.getInt("reference_point"));

    out.move_direction = DmBridge<ValueWithAccuracy<uint16_t, uint16_t>>::FromResultSet(
        rs, "move_direction").get();
    out.speed = DmBridge<ValueWithAccuracy<int16_t, uint16_t>>::FromResultSet(
        rs, "speed").get();
    out.yaw_rate = DmBridge<ValueWithAccuracy<int16_t, uint16_t>>::FromResultSet(
        rs, "yaw_rate").get();
    out.acceleration = DmBridge<ValueWithAccuracy<int16_t, uint16_t>>::FromResultSet(
        rs, "acceleration").get();
    out.orientation = DmBridge<ValueWithAccuracy<uint16_t, uint16_t>>::FromResultSet(
        rs, "orientation").get();
    out.length = DmBridge<ValueWithAccuracy<int16_t, uint16_t>>::FromResultSet(
        rs, "length").get();
    out.width = DmBridge<ValueWithAccuracy<int16_t, uint16_t>>::FromResultSet(
        rs, "width").get();
    out.height = DmBridge<ValueWithAccuracy<int16_t, uint16_t>>::FromResultSet(
        rs, "height").get();

    out.static_status = static_cast<uint16_t>(rs.getInt("static_status"));

    out.shift_position = static_cast<uint8_t>(rs.getInt("shift_position"));

    out.steering_angle_front = static_cast<int8_t>(rs.getInt("steering_angle_front"));
    out.steering_angle_rear = static_cast<int8_t>(rs.getInt("steering_angle_rear"));

    out.brake_state = static_cast<uint8_t>(rs.getInt("brake_state"));
    out.auxiliary_brake_state = static_cast<uint8_t>(rs.getInt("auxiliary_brake_state"));

    out.throttle_position = static_cast<uint8_t>(rs.getInt("throttle_position"));
    out.exterior_lights = static_cast<uint8_t>(rs.getInt("exterior_lights"));

    out.acc_state = static_cast<uint8_t>(rs.getInt("adaptive_cruise_control_system"));
    out.c_acc_state = static_cast<uint8_t>(
        rs.getInt("cooperative_adaptive_cruise_control_system"));
    out.pcs_state = static_cast<uint8_t>(rs.getInt("pre_crash_safety_system"));
    out.abs_state = static_cast<uint8_t>(rs.getInt("antilock_brake_system"));
    out.trc_state = static_cast<uint8_t>(rs.getInt("traction_control_system"));
    out.esc_state = static_cast<uint8_t>(rs.getInt("electronic_stability_control_system"));
    out.lka_state = static_cast<uint8_t>(rs.getInt("lane_keeping_assist_system"));
    out.ldw_state = static_cast<uint8_t>(rs.getInt("lane_departure_warning_system"));

    out.vehicle_size_type = static_cast<uint8_t>(rs.getInt("vehicle_size_type"));
    out.vehicle_role = static_cast<uint8_t>(rs.getInt("vehicle_role"));
    out.vehicle_extended_info = static_cast<uint8_t>(rs.getInt("vehicle_extended_info"));
    out.towing_vehicle = rs.getUnsignedLongLong("towing_vehicle");

    out.tracking_status = static_cast<uint8_t>(rs.getInt("tracking_status"));
    out.detection_count = static_cast<uint16_t>(rs.getInt("detection_count"));
    out.lost_count = static_cast<uint8_t>(rs.getInt("lost_count"));
    out.object_age = static_cast<uint16_t>(rs.getInt("object_age"));

    const std::vector<unsigned long long> in_info_src_list = rs.getVectorUnsignedLongLong("information_source_list");
    std::array<unsigned long long, 4> info_src_list;
    std::copy(in_info_src_list.begin(), in_info_src_list.end(), info_src_list.begin());
    out.info_src_list = info_src_list;

    return out;
  }

  static boost::optional<ObjectInfo> FromTuple(IS::Tuple& tpl) {
    ObjectInfo out;
    unsigned int begin_idx = 0;

    out.id = ExtractTupleValue<unsigned long long>(begin_idx, tpl);
    out.time = static_cast<uint64_t>(ExtractTupleValue<long>(++begin_idx, tpl));
    out.revision = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));

    for (size_t i = 0; i < out.object_classes.size(); ++i) {
      const auto opt = DmBridge<ObjectClass>::FromTuple(tpl, begin_idx);
      if (opt == boost::none) continue;
      out.object_classes[i] = opt.get();
    }

    out.existency = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));

    out.object_location = DmBridge<Location>::FromTuple(tpl, begin_idx).get();

    out.ref_point = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));

    out.move_direction = DmBridge<ValueWithAccuracy<uint16_t, uint16_t>>::FromTuple(
        tpl, begin_idx).get();
    out.speed = DmBridge<ValueWithAccuracy<int16_t, uint16_t>>::FromTuple(
        tpl, begin_idx).get();
    out.yaw_rate = DmBridge<ValueWithAccuracy<int16_t, uint16_t>>::FromTuple(
        tpl, begin_idx).get();
    out.acceleration = DmBridge<ValueWithAccuracy<int16_t, uint16_t>>::FromTuple(
        tpl, begin_idx).get();
    out.orientation = DmBridge<ValueWithAccuracy<uint16_t, uint16_t>>::FromTuple(
        tpl, begin_idx).get();
    out.length = DmBridge<ValueWithAccuracy<int16_t, uint16_t>>::FromTuple(
        tpl, begin_idx).get();
    out.width = DmBridge<ValueWithAccuracy<int16_t, uint16_t>>::FromTuple(
        tpl, begin_idx).get();
    out.height = DmBridge<ValueWithAccuracy<int16_t, uint16_t>>::FromTuple(
        tpl, begin_idx).get();

    out.static_status = static_cast<uint16_t>(ExtractTupleValue<int>(++begin_idx, tpl));

    out.shift_position = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));

    out.steering_angle_front = static_cast<int8_t>(ExtractTupleValue<int>(++begin_idx, tpl));
    out.steering_angle_rear = static_cast<int8_t>(ExtractTupleValue<int>(++begin_idx, tpl));

    out.brake_state = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));
    out.auxiliary_brake_state = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));

    out.throttle_position = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));
    out.exterior_lights = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));

    out.acc_state = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));
    out.c_acc_state = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));
    out.pcs_state = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));
    out.abs_state = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));
    out.trc_state = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));
    out.esc_state = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));
    out.lka_state = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));
    out.ldw_state = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));

    out.vehicle_size_type = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));
    out.vehicle_role = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));
    out.vehicle_extended_info = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));

    out.towing_vehicle = ExtractTupleValue<unsigned long long>(++begin_idx, tpl);

    out.tracking_status = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));
    out.detection_count = static_cast<uint16_t>(ExtractTupleValue<int>(++begin_idx, tpl));
    out.lost_count = static_cast<uint8_t>(ExtractTupleValue<int>(++begin_idx, tpl));
    out.object_age = static_cast<uint16_t>(ExtractTupleValue<int>(++begin_idx, tpl));

    std::vector<unsigned long long> info_src_list = ExtractTupleValues<unsigned long long>(++begin_idx, tpl);
    const int il = info_src_list.size() <= out.info_src_list.size() ? info_src_list.size() : out.info_src_list.size();
    for (int i = 0; i < il; ++i) {
      out.info_src_list[i] = info_src_list[i];
    }

    return out;
  }

  static void FromDataclass(const ObjectInfo& in, const long stamp, uint32_t& idx, IS::Tuple& out) {
    out.setTableName(kTupleNameObjectInfo);
    out.setValue(idx++, static_cast<unsigned long long>(in.id), stamp);
    out.setValue(idx++, static_cast<long>(in.time), stamp);
    out.setValue(idx++, static_cast<int>(in.revision), stamp);

    for (const auto& e : in.object_classes) {
      DmBridge<ObjectClass>::FromDataclass(e, stamp, idx, out);
    }

    out.setValue(idx++, static_cast<int>(in.existency), stamp);

    DmBridge<Location>::FromDataclass(in.object_location, stamp, idx, out);

    out.setValue(idx++, static_cast<int>(in.ref_point), stamp);

    DmBridge<ValueWithAccuracy<uint16_t, uint16_t>>::FromDataclass(
        in.move_direction, stamp, idx, out);
    DmBridge<ValueWithAccuracy<int16_t, uint16_t>>::FromDataclass(
        in.speed, stamp, idx, out);
    DmBridge<ValueWithAccuracy<int16_t, uint16_t>>::FromDataclass(
        in.yaw_rate, stamp, idx, out);
    DmBridge<ValueWithAccuracy<int16_t, uint16_t>>::FromDataclass(
        in.acceleration, stamp, idx, out);
    DmBridge<ValueWithAccuracy<uint16_t, uint16_t>>::FromDataclass(
        in.orientation, stamp, idx, out);

    DmBridge<ValueWithAccuracy<int16_t, uint16_t>>::FromDataclass(
        in.length, stamp, idx, out);
    DmBridge<ValueWithAccuracy<int16_t, uint16_t>>::FromDataclass(
        in.width, stamp, idx, out);
    DmBridge<ValueWithAccuracy<int16_t, uint16_t>>::FromDataclass(
        in.height, stamp, idx, out);

    out.setValue(idx++, static_cast<int>(in.static_status), stamp);
    out.setValue(idx++, static_cast<int>(in.shift_position), stamp);
    out.setValue(idx++, static_cast<int>(in.steering_angle_front), stamp);
    out.setValue(idx++, static_cast<int>(in.steering_angle_rear), stamp);
    out.setValue(idx++, static_cast<int>(in.brake_state), stamp);
    out.setValue(idx++, static_cast<int>(in.auxiliary_brake_state), stamp);
    out.setValue(idx++, static_cast<int>(in.throttle_position), stamp);
    out.setValue(idx++, static_cast<int>(in.exterior_lights), stamp);

    out.setValue(idx++, static_cast<int>(in.acc_state), stamp);
    out.setValue(idx++, static_cast<int>(in.c_acc_state), stamp);
    out.setValue(idx++, static_cast<int>(in.pcs_state), stamp);
    out.setValue(idx++, static_cast<int>(in.abs_state), stamp);
    out.setValue(idx++, static_cast<int>(in.trc_state), stamp);
    out.setValue(idx++, static_cast<int>(in.esc_state), stamp);
    out.setValue(idx++, static_cast<int>(in.lka_state), stamp);
    out.setValue(idx++, static_cast<int>(in.ldw_state), stamp);

    out.setValue(idx++, static_cast<int>(in.vehicle_size_type), stamp);
    out.setValue(idx++, static_cast<int>(in.vehicle_role), stamp);
    out.setValue(idx++, static_cast<int>(in.vehicle_extended_info), stamp);
    out.setValue(idx++, static_cast<unsigned long long>(in.towing_vehicle), stamp);

    out.setValue(idx++, static_cast<int>(in.tracking_status), stamp);
    out.setValue(idx++, static_cast<int>(in.detection_count), stamp);
    out.setValue(idx++, static_cast<int>(in.lost_count), stamp);
    out.setValue(idx++, static_cast<int>(in.object_age), stamp);

    std::vector<unsigned long long> v(4);
    std::copy(in.info_src_list.begin(), in.info_src_list.end(), v.begin());
    out.setValue(idx++, v, stamp);
  }
};

/**
 * @brief Bridge class between Tuple and FreespaceInfo
 */
template<>
struct DmBridge<FreespaceInfo> {
  /**
   * @brief Construct a instance of this class with query result
   *
   * @param rs Query result
   *
   * @return A instance of this class or none
   */
  static boost::optional<FreespaceInfo> FromResultSet(ResultSet& rs) {
    FreespaceInfo out;

    out.id = rs.getUnsignedLongLong("id");
    out.time = static_cast<uint64_t>(rs.getLong("time"));
    out.revision = static_cast<uint8_t>(rs.getInt("revision"));

    out.detection_method = static_cast<uint8_t>(rs.getInt("detection_method"));
    out.detectable_class = static_cast<uint8_t>(rs.getInt("detectable_classes"));

    out.vertices_begin = DmBridge<Location>::FromResultSet(rs, "vertices_begin_").get();

    out.vertices_x_list = rs.getVectorInt("vertices_dx_list");
    out.vertices_y_list = rs.getVectorInt("vertices_dy_list");
    out.vertices_num = static_cast<uint8_t>(out.vertices_x_list.size());

    out.position_begin = DmBridge<Location>::FromResultSet(rs, "position_begin_").get();
    out.position_end = DmBridge<Location>::FromResultSet(rs, "position_end_").get();

    out.length = DmBridge<FreespaceInfo::LengthType>::FromResultSet(rs, "length").get();

    out.id_begin = rs.getUnsignedLongLong("id_begin");
    out.id_end = rs.getUnsignedLongLong("id_end");

    out.existency = static_cast<uint8_t>(rs.getInt("existency"));
    out.minimal_detectable_size = static_cast<uint16_t>(rs.getInt("minimal_detectable_size"));

    const auto in_info_src_list = rs.getVectorUnsignedLongLong("information_source_list");
    std::copy(in_info_src_list.begin(), in_info_src_list.end(), out.info_src_list.begin());

    return out;
  }

  static boost::optional<FreespaceInfo> FromTuple(IS::Tuple& tpl) {
    FreespaceInfo out;
    unsigned int idx = 0;

    out.id = ExtractTupleValue<unsigned long long>(idx, tpl);
    out.time = static_cast<unsigned long long>(ExtractTupleValue<long>(++idx, tpl));
    out.revision = static_cast<uint8_t>(ExtractTupleValue<int>(++idx, tpl));

    out.detection_method = static_cast<uint8_t>(ExtractTupleValue<int>(++idx, tpl));
    out.detectable_class = static_cast<uint8_t>(ExtractTupleValue<int>(++idx, tpl));

    out.vertices_begin = DmBridge<Location>::FromTuple(tpl, idx).get();

    out.vertices_x_list = ExtractTupleValues<int>(++idx, tpl);
    out.vertices_y_list = ExtractTupleValues<int>(++idx, tpl);
    out.vertices_num = out.vertices_x_list.size();

    out.position_begin = DmBridge<Location>::FromTuple(tpl, idx).get();
    out.position_end = DmBridge<Location>::FromTuple(tpl, idx).get();

    out.length = DmBridge<FreespaceInfo::LengthType>::FromTuple(tpl, idx).get();

    out.id_begin = ExtractTupleValue<unsigned long long>(++idx, tpl);
    out.id_end = ExtractTupleValue<unsigned long long>(++idx, tpl);

    out.existency = static_cast<uint8_t>(ExtractTupleValue<int>(++idx, tpl));
    out.minimal_detectable_size = static_cast<uint16_t>(ExtractTupleValue<int>(++idx, tpl));

    std::vector<unsigned long long> info_src_list = ExtractTupleValues<unsigned long long>(++idx, tpl);
    const int il = info_src_list.size() <= out.info_src_list.size() ? info_src_list.size() : out.info_src_list.size();
    for (int i = 0; i < il; ++i) {
      out.info_src_list[i] = info_src_list[i];
    }

    return out;
  }

  static void FromDataclass(
      const FreespaceInfo& in, const long stamp, uint32_t& idx, IS::Tuple& out) {
    out.setTableName(kTupleNameFreespaceInfo);
    out.setValue(idx++, static_cast<unsigned long long>(in.id), stamp);
    out.setValue(idx++, static_cast<long>(in.time), stamp);
    out.setValue(idx++, static_cast<int>(in.revision), stamp);
    out.setValue(idx++, static_cast<int>(in.detection_method), stamp);
    out.setValue(idx++, static_cast<int>(in.detectable_class), stamp);

    DmBridge<Location>::FromDataclass(in.vertices_begin, stamp, idx, out);

    out.setValue(idx++, in.vertices_x_list, stamp);
    out.setValue(idx++, in.vertices_y_list, stamp);

    DmBridge<Location>::FromDataclass(in.position_begin, stamp, idx, out);
    DmBridge<Location>::FromDataclass(in.position_end, stamp, idx, out);

    DmBridge<FreespaceInfo::LengthType>::FromDataclass(in.length, stamp, idx, out);

    out.setValue(idx++, static_cast<unsigned long long>(in.id_begin), stamp);
    out.setValue(idx++, static_cast<unsigned long long>(in.id_end), stamp);

    out.setValue(idx++, static_cast<int>(in.existency), stamp);
    out.setValue(idx++, static_cast<int>(in.minimal_detectable_size), stamp);

    std::vector<unsigned long long> v(in.info_src_list.size());
    std::copy(in.info_src_list.begin(), in.info_src_list.end(), v.begin());
    out.setValue(idx++, v, stamp);
  }
};

/**
 * @brief Bridge class between Tuple and LightInformation
 */
template<>
struct DmBridge<LightInformation> {
  /**
   * @brief Construct a instance of this class with query result
   *
   * @param rs Query result
   *
   * @return A instance of this class or none
   */
  static boost::optional<LightInformation> FromResultSet(ResultSet& rs, const uint32_t idx = 0) {
    LightInformation out;
    const std::vector<std::string> signal_light_info_pref = {
      "light_info_main",
      "light_info_arrow",
      "light_info_min_time_to_change",
      "light_info_max_time_to_change"};
    std::vector<int> data;
    for (const auto& pref : signal_light_info_pref) {
      std::stringstream ss;
      ss << pref << "_" << idx;
      data.push_back(rs.getInt(ss.str()));
    }
    out.main_light = static_cast<uint8_t>(data[0]);
    out.arrow_light = static_cast<uint8_t>(data[1]);
    out.min_time_to_change = static_cast<uint16_t>(data[2]);
    out.max_time_to_change = static_cast<uint16_t>(data[3]);
    return out;
  }

  static boost::optional<LightInformation> FromTuple(IS::Tuple& tuple, unsigned int& begin_idx) {
    LightInformation out;
    any any_val;
    const std::vector<std::string> signal_light_info_pref = {
      "light_info_main",
      "light_info_arrow",
      "light_info_min_time_to_change",
      "light_info_max_time_to_change"};
    std::vector<int> data;
    for (size_t i = 0; i < signal_light_info_pref.size(); ++i) {
      tuple.getValueByIdx(++begin_idx, any_val);
      data.push_back(any_cast<int>(any_val));
    }
    out.main_light = static_cast<uint8_t>(data[0]);
    out.arrow_light = static_cast<uint8_t>(data[1]);
    out.min_time_to_change = static_cast<uint16_t>(data[2]);
    out.max_time_to_change = static_cast<uint16_t>(data[3]);
    return out;
  }

  static void FromDataclass(
      const LightInformation& in, const long stamp, uint32_t& idx, IS::Tuple& out) {
    out.setValue(idx++, static_cast<int>(in.main_light), stamp);
    out.setValue(idx++, static_cast<int>(in.arrow_light), stamp);
    out.setValue(idx++, static_cast<int>(in.min_time_to_change), stamp);
    out.setValue(idx++, static_cast<int>(in.max_time_to_change), stamp);
  }
};

/**
 * @brief Bridge class between Tuple and SignalInfo
 */
template<>
struct DmBridge<SignalInfo> {
  /**
   * @brief Construct a instance of this class with query result
   *
   * @param rs Query result
   *
   * @return A instance of this class or none
   */
  static boost::optional<SignalInfo> FromResultSet(ResultSet& rs) {
    SignalInfo out;
    out.crp_id = static_cast<uint32_t>(rs.getInt("crp_id"));
    const auto signal_id_list = rs.getVectorInt("id");
    VLOG(3) << "signal id num: " << (unsigned)(signal_id_list.size()) << std::endl;
    for (size_t i = 0; i < signal_id_list.size(); ++i) {
      out.signal_id_list[i] = static_cast<uint8_t>(signal_id_list[i]);
    }
    for (size_t i = signal_id_list.size(); i < SignalInfo::SignalIdNum; ++i) {
      out.signal_id_list[i] = 0;
    }
    out.time = static_cast<uint64_t>(rs.getLong("time"));
    out.state = rs.getInt("state");
    out.specific_control_flags = rs.getInt("specific_control_flags");
    out.event_count = rs.getInt("event_count");
    out.count_down_stop_flag = rs.getInt("count_down_stop_flag");
    for (size_t i = 0; i < SignalInfo::LightInformationNum; ++i) {
      out.light_information_list[i] = DmBridge<LightInformation>::FromResultSet(
          rs, static_cast<uint32_t>(i)).get();
    }
    return out;
  }

  static boost::optional<SignalInfo> FromTuple(IS::Tuple& tpl) {
    SignalInfo out;
    any any_val;
    unsigned int begin_idx = 0;

    tpl.getValueByIdx(begin_idx, any_val);
    out.crp_id = static_cast<uint32_t>(any_cast<int>(any_val));

    tpl.getValueByIdx(++begin_idx, any_val);
    const std::type_info& typeInfo = any_val.type();
    if (typeInfo == typeid(std::vector<any>)) {
      const auto signal_id_list = any_cast<std::vector<any>>(any_val);
      for (size_t i = 0; i < signal_id_list.size(); ++i) {
        if (i == SignalInfo::SignalIdNum) break;
        out.signal_id_list[i] = (static_cast<uint8_t>(any_cast<int>(signal_id_list[i])));
      }
    } else {
      const auto signal_id_list = any_cast<std::vector<int>>(any_val);
      for (size_t i = 0; i < signal_id_list.size(); ++i) {
        if (i == SignalInfo::SignalIdNum) break;
        out.signal_id_list[i] = (static_cast<uint8_t>(signal_id_list[i]));
      }
    }

    tpl.getValueByIdx(++begin_idx, any_val);
    out.time = static_cast<uint64_t>(any_cast<long>(any_val));
    tpl.getValueByIdx(++begin_idx, any_val);
    out.state = any_cast<int>(any_val);
    tpl.getValueByIdx(++begin_idx, any_val);
    out.specific_control_flags = any_cast<int>(any_val);
    tpl.getValueByIdx(++begin_idx, any_val);
    out.event_count = any_cast<int>(any_val);
    tpl.getValueByIdx(++begin_idx, any_val);
    out.count_down_stop_flag = any_cast<int>(any_val);
    for (size_t i = 0; i < SignalInfo::LightInformationNum; ++i) {
      out.light_information_list[i] = DmBridge<LightInformation>::FromTuple(tpl, begin_idx).get();
    }
    return out;
  }

  static void FromDataclass(
      const SignalInfo& in, const long stamp, uint32_t& idx, IS::Tuple& out) {
    VLOG(2) << in;
    out.setTableName(kTupleNameSignalInfo);
    out.setValue(idx++, static_cast<int>(in.crp_id), stamp);

    // Tupleはいちど決めたサイズを守らないとエラーになるので，ちゃんとする
    std::vector<int> signal_id_list;
    for (const auto e : in.signal_id_list) {
      signal_id_list.push_back(static_cast<int>(e));
    }
    out.setValue(idx++, signal_id_list, stamp);

    out.setValue(idx++, static_cast<long>(in.time), stamp);
    out.setValue(idx++, static_cast<int>(in.state), stamp);
    out.setValue(idx++, static_cast<int>(in.specific_control_flags), stamp);
    out.setValue(idx++, static_cast<int>(in.event_count), stamp);
    out.setValue(idx++, static_cast<int>(in.count_down_stop_flag), stamp);

    size_t light_info_num = in.light_information_list.size();
    if (light_info_num > SignalInfo::LightInformationNum) {
      LOG(WARNING)
        << "too many light information("
        << light_info_num
        << ") in signal info. reduce it as the limitation("
        << SignalInfo::LightInformationNum << ")";
      light_info_num = SignalInfo::LightInformationNum;
    }
    for (size_t i = 0; i < light_info_num; ++i) {
      DmBridge<LightInformation>::FromDataclass(in.light_information_list[i], stamp, idx, out);
    }
    // 要素が足りない場合は初期値のLightInformationで埋める．これでTupleのエラーを回避する
    for (size_t i = 0; i < SignalInfo::LightInformationNum - light_info_num; ++i) {
      DmBridge<LightInformation>::FromDataclass(LightInformation(), stamp, idx, out);
    }
  }
};

template<>
struct DmBridge<DetectableCapability> {
  static boost::optional<DetectableCapability> FromResultSet(ResultSet& rs, const size_t idx=0) {
    std::stringstream ss;
    ss << "_" << idx;
    const std::string suffix = ss.str();

    DetectableCapability out;
    out.detectable_class = static_cast<uint8_t>(
        rs.getInt("detectable_classes" + suffix));
    out.vertices_x_list = rs.getVectorInt(
        "detectable_domain_x_list" + suffix);
    out.vertices_y_list = rs.getVectorInt(
        "detectable_domain_y_list" + suffix);
    out.vertices_num = out.vertices_x_list.size();
    out.confidence = static_cast<uint8_t>(
        rs.getInt("confidence" + suffix));
    out.minimal_detectable_size = static_cast<uint16_t>(
        rs.getInt("minimal_detectable_size" + suffix));

    return out;
  }

  static boost::optional<DetectableCapability> FromTuple(uint32_t& idx, IS::Tuple& tpl) {
    DetectableCapability out;
    out.detectable_class = static_cast<uint8_t>(
        ExtractTupleValue<int>(++idx, tpl));
    out.vertices_x_list = ExtractTupleValues<int>(++idx, tpl);
    out.vertices_y_list = ExtractTupleValues<int>(++idx, tpl);
    out.vertices_num = static_cast<uint8_t>(
        out.vertices_x_list.size());
    out.confidence = static_cast<uint8_t>(
        ExtractTupleValue<int>(++idx, tpl));
    out.minimal_detectable_size = static_cast<uint16_t>(
        ExtractTupleValue<int>(++idx, tpl));

    return out;
  }

  static void FromDataclass(
      const DetectableCapability& in, const long stamp, uint32_t& idx, IS::Tuple& out) {
    out.setValue(
        idx++,
        static_cast<int>(in.detectable_class),
        stamp);
    out.setValue(
        idx++,
        static_cast<std::vector<int>>(in.vertices_x_list),
        stamp);
    out.setValue(
        idx++,
        static_cast<std::vector<int>>(in.vertices_y_list),
        stamp);
    out.setValue(
        idx++,
        static_cast<int>(in.confidence),
        stamp);
    out.setValue(
        idx++,
        static_cast<int>(in.minimal_detectable_size),
        stamp);
  }
};

template<>
struct DmBridge<SensorInfo> {
  static boost::optional<SensorInfo> FromResultSet(ResultSet& rs) {
    SensorInfo out;
    out.unit_id = rs.getUnsignedLongLong("unit_id");
    out.sensor_id = static_cast<uint8_t>(
        rs.getInt("sensor_id"));
    out.sensor_type = static_cast<uint8_t>(
        rs.getInt("sensor_type"));
    out.position = DmBridge<Location>::FromResultSet(rs).get();
    out.time = static_cast<unsigned long long>(
        rs.getLong("time"));

    for (size_t i = 0; i < SensorInfo::DetectableCapabilityNum; ++i) {
      out.detectable_capabilities.push_back(
          DmBridge<DetectableCapability>::FromResultSet(
            rs, i).get());
    }
    out.detectable_capability_num = out.detectable_capabilities.size();

    out.sensor_status = static_cast<uint8_t>(
        rs.getInt("sensor_status"));

    return out;
  }

  static boost::optional<SensorInfo> FromTuple(IS::Tuple& tpl) {
    uint32_t idx = 0;
    SensorInfo out;
    out.unit_id = ExtractTupleValue<unsigned long long>(idx, tpl);
    out.sensor_id = static_cast<uint8_t>(ExtractTupleValue<int>(++idx, tpl));
    out.sensor_type = static_cast<uint8_t>(ExtractTupleValue<int>(++idx, tpl));

    out.position = DmBridge<Location>::FromTuple(tpl, idx).get();

    out.time = static_cast<unsigned long long>(ExtractTupleValue<long>(++idx, tpl));

    out.detectable_capability_num = SensorInfo::DetectableCapabilityNum;
    for (uint8_t i = 0; i < out.detectable_capability_num; ++i) {
      out.detectable_capabilities.push_back(
          DmBridge<DetectableCapability>::FromTuple(
            idx, tpl).get());
    }

    out.sensor_status = static_cast<uint8_t>(ExtractTupleValue<int>(++idx, tpl));
    return out;
  }

  static void FromDataclass(
      const SensorInfo& in, const long stamp, uint32_t& idx, IS::Tuple& out) {
    out.setTableName(kTupleNameSensorInfo);
    out.setValue(idx++, in.unit_id, stamp);
    out.setValue(
        idx++, static_cast<int>(in.sensor_id), stamp);
    out.setValue(
        idx++, static_cast<int>(in.sensor_type), stamp);
    DmBridge<Location>::FromDataclass(
        in.position, stamp, idx, out);
    out.setValue(
        idx++, static_cast<long>(in.time), stamp);
    const size_t dcn = in.detectable_capability_num;
    for (size_t i = 0; i < dcn; ++i) {
      DmBridge<DetectableCapability>::FromDataclass(
          in.detectable_capabilities[i], stamp, idx, out);
    }
    for (size_t i = dcn; i < SensorInfo::DetectableCapabilityNum; ++i) {
      DmBridge<DetectableCapability>::FromDataclass(
          DetectableCapability(), stamp, idx, out);
    }
    out.setValue(
        idx++, static_cast<int>(in.sensor_status), stamp);
  }
};

}
#endif
