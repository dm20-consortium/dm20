#ifndef ROS2_DMI_OBJECT_INFO_CONVERTER_HPP_
#define ROS2_DMI_OBJECT_INFO_CONVERTER_HPP_
#include <boost/array.hpp>
#include <boost/optional.hpp>

#include <dm_object_info_msgs/msg/object_info_array.hpp>
#include <is/ResultSet.h>
#include <is/DmUtil.h>

#include "ros2_dmi/converter_base.hpp"
#include "ros2_dmi/converter_util.hpp"

namespace {
/**
 * @brief Number of columns
 */
constexpr uint32_t kObjectInfoTupleNum = 88;
const char* const kObjectInfoTupleName = "object_info_0_8_1";

/**
 * @brief Max number of object class list
 */
constexpr uint32_t kObjectClassMaxNum = 4;
constexpr uint32_t kObjectClassPropertyNum = 4;
}

namespace ros2_dmi {

using namespace dm_object_info_msgs::msg;

/**
 * @brief A converter for ObjectInfo
 */
template<>
struct Converter<ObjectInfo> {
  /**
   * @brief Convert DM result set to ObjectInfo
   *
   * @param rs DM event object
   *
   * @return ObjectInfo or boost::none
   */
  static boost::optional<ObjectInfo> ResultSetToRos(ResultSet& rs) {
    if (!rs.next()) return boost::none;

    ObjectInfo msg;

    try {
      msg.id.value = rs.getUnsignedLongLong("object_id");
      msg.time.value = static_cast<unsigned long long>(rs.getLong("time"));
      msg.revision.value = static_cast<uint8_t>(rs.getInt("revision"));

      std::vector<ObjectClass> object_class;
      const std::vector<std::string> object_class_pref = {
        "object_class_id",
        "object_class_confidence",
        "object_class_subclass_type",
        "object_class_subclass_confidence"};
      for (uint32_t i = 0; i < kObjectClassMaxNum; ++i) {
        std::vector<int> data;
        for (const auto& pref : object_class_pref) {
          std::stringstream ss;
          ss << pref << "_" << i;
          data.push_back(rs.getInt(ss.str()));
        }
        ObjectClass oc;
        oc.id.value = data[0];
        oc.confidence.value = data[1];
        oc.subclass_type.value = data[2];
        oc.subclass_confidence.value = data[3];
        object_class.push_back(oc);
      }
      msg.object_class = object_class;

      msg.existency.value = rs.getInt("existency");

      msg.object_location = Getter<Location>::ResultSetToRos(rs);

      msg.ref_point.value = rs.getInt("reference_point");

      msg.direction.value.value = rs.getInt("move_direction_value");
      msg.direction.accuracy.value = rs.getInt("move_direction_accuracy");

      msg.speed.value.value = rs.getInt("speed_value");
      msg.speed.accuracy.value = rs.getInt("speed_accuracy");

      msg.yaw_rate.value.value = rs.getInt("yaw_rate_value");
      msg.yaw_rate.accuracy.value = rs.getInt("yaw_rate_accuracy");

      msg.acceleration.value.value = rs.getInt("acceleration_value");
      msg.acceleration.accuracy.value = rs.getInt("acceleration_accuracy");

      msg.orientation.value.value = rs.getInt("orientation_value");
      msg.orientation.accuracy.value = rs.getInt("orientation_accuracy");

      msg.size.length.value.value = rs.getInt("length_value");
      msg.size.length.accuracy.value = rs.getInt("length_accuracy");
      msg.size.width.value.value = rs.getInt("width_value");
      msg.size.width.accuracy.value = rs.getInt("width_accuracy");
      msg.size.height.value.value = rs.getInt("height_value");
      msg.size.height.accuracy.value = rs.getInt("height_accuracy");

      msg.static_status.value = rs.getInt("static_status");

      msg.shift_position.value = rs.getInt("shift_position");

      msg.steering_angle_front.value = rs.getInt("steering_angle_front");
      msg.steering_angle_rear.value = rs.getInt("steering_angle_rear");

      msg.brake_state.value = rs.getInt("brake_state");
      msg.auxiliary_brake_state.value = rs.getInt("auxiliary_brake_state");

      msg.throttle_position.value = rs.getInt("throttle_position");

      msg.exterior_lights.value = rs.getInt("exterior_lights");

      msg.control_system_states.adaptive_cruise_control_system.value = rs.getInt(
          "adaptive_cruise_control_system");
      msg.control_system_states.cooperative_adaptive_cruise_control_system.value = rs.getInt(
          "cooperative_adaptive_cruise_control_system");
      msg.control_system_states.pre_crash_safety_system.value = rs.getInt(
          "pre_crash_safety_system");
      msg.control_system_states.antilock_brake_system.value = rs.getInt(
          "antilock_brake_system");
      msg.control_system_states.traction_control_system.value = rs.getInt(
          "traction_control_system");
      msg.control_system_states.electronic_stability_control_system.value = rs.getInt(
          "electronic_stability_control_system");
      msg.control_system_states.lane_keeping_assist_system.value = rs.getInt(
          "lane_keeping_assist_system");
      msg.control_system_states.lane_departure_warning_system.value = rs.getInt(
          "lane_departure_warning_system");

      msg.vehicle_size_type.value = rs.getInt("vehicle_size_type");
      msg.vehicle_role.value = rs.getInt("vehicle_role");
      msg.vehicle_extended_info.value = rs.getInt("vehicle_extended_info");

      msg.tracking_status.value = rs.getInt("tracking_status");
      msg.detection_count.value = rs.getInt("detection_count");
      msg.lost_count.value = rs.getInt("lost_count");
      msg.object_age.value = rs.getInt("object_age");

      msg.towing_vehicle.value = rs.getUnsignedLongLong("towing_vehicle");

      msg.information_source_list = Getter<std::vector<ObjectId>>::ResultSetToRos(rs);
    } catch (const std::exception& e) {
      std::cerr << "[ResultSet->ObjectInfo]" << e.what() << std::endl
        << "We ignore this row" << std::endl;
      return boost::none;
    } catch (const CastException& e) {
      std::cerr << "[ResultSet->ObjectInfo] Cast Exception" << std::endl
        << "We ignore this row" << std::endl;
      return boost::none;
    }

    return msg;
  }

  /**
   * @brief Convert DM result set to ObjectInfoArray
   *
   * @param rs DM event object
   *
   * @return ObjectInfoArray or boost::none
   */
  static boost::optional<ObjectInfo> TupleSetToRos(IS::TupleSet& ts) {
    boost::optional<ObjectInfo> msg = Converter<ObjectInfo>::TupleToRos(ts.getTuple(0));
    return msg;
  }

  /**
   * @brief Convert DM Tuple to ObjectInfo
   *
   * @param tpl DM event object
   *
   * @return ObjectInfo or boost::none
   */
  static boost::optional<ObjectInfo> TupleToRos(IS::Tuple& tpl) {
    ObjectInfo msg;
    unsigned int idx = 0;
    try {
      msg.id.value = ExtractTupleValue<unsigned long long>(idx, tpl);
      msg.time.value = ExtractTupleValue<long>(++idx, tpl);
      msg.revision.value = ExtractTupleValue<int>(++idx, tpl);

      std::vector<ObjectClass> object_class;
      for (uint32_t i = 0; i < kObjectClassMaxNum; ++i) {
        std::vector<int> data;
        for (size_t j = 0; j < kObjectClassPropertyNum; ++j) {
          const int v = ExtractTupleValue<int>(++idx, tpl);
          data.push_back(v);
        }
        ObjectClass oc;
        oc.id.value = data[0];
        oc.confidence.value = data[1];
        oc.subclass_type.value = data[2];
        oc.subclass_confidence.value = data[3];
        object_class.push_back(oc);
      }
      msg.object_class = object_class;

      msg.existency.value = ExtractTupleValue<int>(++idx, tpl);

      msg.object_location = Getter<Location>::TupleToRos(tpl, idx);

      msg.ref_point.value = ExtractTupleValue<int>(++idx, tpl);

      msg.direction.value.value = ExtractTupleValue<int>(++idx, tpl);
      msg.direction.accuracy.value = ExtractTupleValue<int>(++idx, tpl);

      msg.speed.value.value = ExtractTupleValue<int>(++idx, tpl);
      msg.speed.accuracy.value = ExtractTupleValue<int>(++idx, tpl);

      msg.yaw_rate.value.value = ExtractTupleValue<int>(++idx, tpl);
      msg.yaw_rate.accuracy.value = ExtractTupleValue<int>(++idx, tpl);

      msg.acceleration.value.value = ExtractTupleValue<int>(++idx, tpl);
      msg.acceleration.accuracy.value = ExtractTupleValue<int>(++idx, tpl);

      msg.orientation.value.value = ExtractTupleValue<int>(++idx, tpl);
      msg.orientation.accuracy.value = ExtractTupleValue<int>(++idx, tpl);

      msg.size.length.value.value = ExtractTupleValue<int>(++idx, tpl);
      msg.size.length.accuracy.value = ExtractTupleValue<int>(++idx, tpl);
      msg.size.width.value.value = ExtractTupleValue<int>(++idx, tpl);
      msg.size.width.accuracy.value = ExtractTupleValue<int>(++idx, tpl);
      msg.size.height.value.value = ExtractTupleValue<int>(++idx, tpl);
      msg.size.height.accuracy.value = ExtractTupleValue<int>(++idx, tpl);

      msg.static_status.value = ExtractTupleValue<int>(++idx, tpl);

      msg.shift_position.value = ExtractTupleValue<int>(++idx, tpl);
      msg.steering_angle_front.value = ExtractTupleValue<int>(++idx, tpl);
      msg.steering_angle_rear.value = ExtractTupleValue<int>(++idx, tpl);
      msg.brake_state.value = ExtractTupleValue<int>(++idx, tpl);
      msg.auxiliary_brake_state.value = ExtractTupleValue<int>(++idx, tpl);
      msg.throttle_position.value = ExtractTupleValue<int>(++idx, tpl);
      msg.exterior_lights.value = ExtractTupleValue<int>(++idx, tpl);

      msg.control_system_states.adaptive_cruise_control_system.value = ExtractTupleValue<int>(++idx, tpl);
      msg.control_system_states.cooperative_adaptive_cruise_control_system.value = ExtractTupleValue<int>(++idx, tpl);
      msg.control_system_states.pre_crash_safety_system.value = ExtractTupleValue<int>(++idx, tpl);
      msg.control_system_states.antilock_brake_system.value = ExtractTupleValue<int>(++idx, tpl);
      msg.control_system_states.traction_control_system.value = ExtractTupleValue<int>(++idx, tpl);
      msg.control_system_states.electronic_stability_control_system.value = ExtractTupleValue<int>(++idx, tpl);
      msg.control_system_states.lane_keeping_assist_system.value = ExtractTupleValue<int>(++idx, tpl);
      msg.control_system_states.lane_departure_warning_system.value = ExtractTupleValue<int>(++idx, tpl);

      msg.vehicle_size_type.value = ExtractTupleValue<int>(++idx, tpl);
      msg.vehicle_role.value = ExtractTupleValue<int>(++idx, tpl);
      msg.vehicle_extended_info.value = ExtractTupleValue<int>(++idx, tpl);
      msg.towing_vehicle.value = ExtractTupleValue<unsigned long long>(++idx, tpl);

      msg.tracking_status.value = ExtractTupleValue<int>(++idx, tpl);
      msg.detection_count.value = ExtractTupleValue<int>(++idx, tpl);
      msg.lost_count.value = ExtractTupleValue<int>(++idx, tpl);
      msg.object_age.value = ExtractTupleValue<int>(++idx, tpl);

      msg.information_source_list = Getter<std::vector<ObjectId>>::TupleToRos(tpl, idx);
    } catch (const std::exception& e) {
      std::cerr << "[Tuple->ObjectInfo]" << e.what() << std::endl
        << "We ignore this row" << std::endl;
      return boost::none;
    } catch (const CastException& e) {
      std::cerr << "[Tuple->ObjectInfo] Cast Exception" << std::endl
        << "We ignore this row" << std::endl;
      return boost::none;
    }

    return msg;
  }


  /**
   * @brief Convert ObjectInfo to DM tuple object
   *
   * @param msg Input ObjectInfo
   * @param stamp A timestamp to be registered to each cell
   *
   * @return vector of DM tuple object
   */
  static std::vector<IS::Tuple> RosToTuple(
      const ObjectInfo& msg, const long stamp) {
    int idx = -1;
    IS::Tuple tuple(kObjectInfoTupleNum);
    tuple.setTableName(kObjectInfoTupleName);
    tuple.setValue(++idx, static_cast<unsigned long long>(msg.id.value), stamp);
    tuple.setValue(++idx, static_cast<long>(msg.time.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.revision.value), stamp);
    std::vector<ObjectClass> object_class(msg.object_class);
    if (object_class.size() < kObjectClassMaxNum) {
      object_class.resize(kObjectClassMaxNum);
    }
    for (uint32_t i = 0; i < kObjectClassMaxNum; ++i) {
      tuple.setValue(++idx, static_cast<int>(object_class[i].id.value), stamp);
      tuple.setValue(++idx, static_cast<int>(object_class[i].confidence.value), stamp);
      tuple.setValue(++idx, static_cast<int>(object_class[i].subclass_type.value), stamp);
      tuple.setValue(++idx, static_cast<int>(object_class[i].subclass_confidence.value), stamp);
    }
    tuple.setValue(++idx, static_cast<int>(msg.existency.value), stamp);
    Getter<Location>::RosToTuple(msg.object_location, tuple, stamp, ++idx);
    tuple.setValue(++idx, static_cast<int>(msg.ref_point.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.direction.value.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.direction.accuracy.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.speed.value.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.speed.accuracy.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.yaw_rate.value.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.yaw_rate.accuracy.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.acceleration.value.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.acceleration.accuracy.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.orientation.value.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.orientation.accuracy.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.size.length.value.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.size.length.accuracy.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.size.width.value.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.size.width.accuracy.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.size.height.value.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.size.height.accuracy.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.static_status.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.shift_position.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.steering_angle_front.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.steering_angle_rear.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.brake_state.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.auxiliary_brake_state.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.throttle_position.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.exterior_lights.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.control_system_states.adaptive_cruise_control_system.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.control_system_states.cooperative_adaptive_cruise_control_system.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.control_system_states.pre_crash_safety_system.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.control_system_states.antilock_brake_system.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.control_system_states.traction_control_system.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.control_system_states.electronic_stability_control_system.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.control_system_states.lane_keeping_assist_system.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.control_system_states.lane_departure_warning_system.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.vehicle_size_type.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.vehicle_role.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.vehicle_extended_info.value), stamp);
    tuple.setValue(++idx, static_cast<unsigned long long>(msg.towing_vehicle.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.tracking_status.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.detection_count.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.lost_count.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.object_age.value), stamp);

    Getter<std::vector<ObjectId>>::RosToTuple(msg.information_source_list, tuple, stamp, ++idx);

    std::vector<IS::Tuple> tuples;
    tuples.push_back(tuple);

    return tuples;
  }
};

/**
 * @brief A converter for ObjectInfoArray
 */
template<>
struct Converter<ObjectInfoArray> {
  /**
   * @brief Convert DM result set to ObjectInfoArray
   *
   * @param rs DM event object
   *
   * @return ObjectInfoArray or boost::none
   */
  static boost::optional<ObjectInfoArray> ResultSetToRos(ResultSet& rs) {
    ObjectInfoArray msg;
    while (true) {
      boost::optional<ObjectInfo> m = Converter<ObjectInfo>::ResultSetToRos(rs);
      if (m == boost::none) break;
      msg.array.push_back(m.get());
    }
    if (msg.array.empty()) return boost::none;
    return msg;
  }

  /**
   * @brief Convert DM result set to ObjectInfoArray
   *
   * @param rs DM event object
   *
   * @return ObjectInfoArray or boost::none
   */
  static boost::optional<ObjectInfoArray> TupleSetToRos(IS::TupleSet& ts) {
    ObjectInfoArray msg;
    for (int idx = 0; idx < ts.size(); idx++) {
      boost::optional<ObjectInfo> m = Converter<ObjectInfo>::TupleToRos(ts.getTuple(idx));
      if (m == boost::none) break;
      msg.array.push_back(m.get());
    }
    if (msg.array.empty()) return boost::none;
    return msg;
  }

  /**
   * @brief Convert ObjectInfoArray to DM tuple object
   *
   * @param msg Input ObjectInfoArray
   * @param stamp A timestamp to be registered to each cell
   *
   * @return vector of DM tuple object
   */
  static std::vector<IS::Tuple> RosToTuple(
      const ObjectInfoArray& msg, const long stamp) {
    std::vector<IS::Tuple> out;
    for (auto& e : msg.array) {
      std::vector<IS::Tuple> rows = Converter<ObjectInfo>::RosToTuple(e, stamp);
      if (rows.empty()) break;
      out.insert(out.end(), rows.begin(), rows.end());
    }
    return out;
  }
};

}

#endif
