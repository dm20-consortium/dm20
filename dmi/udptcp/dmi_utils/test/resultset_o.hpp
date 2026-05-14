#include <chrono>
#include <functional>
#include <thread>
#include <iostream>

#include <is/ResultSet.h>

using QueryResult = std::unordered_map<std::string, std::string>;

namespace test_util {

  std::vector<std::string> CreateNameList_o() {
    return {
      "object_id",
      "time",
      "revision",
      "object_class_id_0",
      "object_class_confidence_0",
      "object_class_subclass_type_0",
      "object_class_subclass_confidence_0",
      "object_class_id_1",
      "object_class_confidence_1",
      "object_class_subclass_type_1",
      "object_class_subclass_confidence_1",
      "object_class_id_2",
      "object_class_confidence_2",
      "object_class_subclass_type_2",
      "object_class_subclass_confidence_2",
      "object_class_id_3",
      "object_class_confidence_3",
      "object_class_subclass_type_3",
      "object_class_subclass_confidence_3",
      "existency",
      "geodetic_srid",
      "latitude",
      "longitude",
      "altitude",
      "projective_srid",
      "prc_x",
      "prc_y",
      "prc_z",
      "crp_id",
      "dx_crp",
      "dy_crp",
      "dh_crp",
      "lane_count",
      "lane_position",
      "lane_lateral_position",
      "crp_id_begin",
      "crp_id_end",
      "lane_vertical_position",
      "lane_id",
      "dx_lane",
      "dy_lane",
      "dh_lane",
      "semi_axis_length_major",
      "semi_axis_length_minor",
      "ellipse_orientation",
      "altitude_accuracy",
      "static_status",
      "reference_point",
      "move_direction_value",
      "move_direction_accuracy",
      "speed_value",
      "speed_accuracy",
      "yaw_rate_value",
      "yaw_rate_accuracy",
      "acceleration_value",
      "acceleration_accuracy",
      "orientation_value",
      "orientation_accuracy",
      "length_value",
      "length_accuracy",
      "width_value",
      "width_accuracy",
      "height_value",
      "height_accuracy",
      "shift_position",
      "steering_angle_front",
      "steering_angle_rear",
      "brake_state",
      "auxiliary_brake_state",
      "throttle_position",
      "exterior_lights",
      "adaptive_cruise_control_system",
      "cooperative_adaptive_cruise_control_system",
      "pre_crash_safety_system",
      "antilock_brake_system",
      "traction_control_system",
      "electronic_stability_control_system",
      "lane_keeping_assist_system",
      "lane_departure_warning_system",
      "tracking_status",
      "detection_count",
      "lost_count",
      "object_age",
      "vehicle_size_type",
      "vehicle_role",
      "vehicle_extended_info",
      "towing_vehicle",
      "information_source_list"};
  }

  std::vector<std::string> CreateTypeList_o() {
    return {
      "ulong",
      "long",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "ulong",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "ulong",
      "vector(ulong)"};
  }

  ResultSetMetaData CreateQueryResultMetaData_o() {
    return {10, CreateNameList_o(), CreateTypeList_o()};
  }

  QueryResult CreateIsNulls_o() {
    QueryResult o;
    const auto name_list = CreateNameList_o();
    for (const auto name : name_list) {
      o[name] = "0";
    }
    return o;
  }

  QueryResult CreateTimestamps_o() {
    return CreateIsNulls_o();
  }

  // テストで変更したいやつだけ引数にする
  QueryResult CreateDatas_o(
    const std::string& id,
    const std::string& time,
    const std::string& revision,
    const std::string& object_class_id_0,
    const std::string& object_class_confidence_0,
    const std::string& object_class_subclass_type_0,
    const std::string& object_class_subclass_confidence_0,
    const std::string& object_class_id_1,
    const std::string& object_class_confidence_1,
    const std::string& object_class_subclass_type_1,
    const std::string& object_class_subclass_confidence_1,
    const std::string& object_class_id_2,
    const std::string& object_class_confidence_2,
    const std::string& object_class_subclass_type_2,
    const std::string& object_class_subclass_confidence_2,
    const std::string& object_class_id_3,
    const std::string& object_class_confidence_3,
    const std::string& object_class_subclass_type_3,
    const std::string& object_class_subclass_confidence_3,
    const std::string& existency,
    const std::string& geodetic_system,
    const std::string& latitude,
    const std::string& longitude,
    const std::string& altitude,
    const std::string& projective_srid,
    const std::string& prc_x,
    const std::string& prc_y,
    const std::string& prc_z,
    const std::string& crp_id,
    const std::string& dx_crp,
    const std::string& dy_crp,
    const std::string& dh_crp,
    const std::string& lane_count,
    const std::string& lane_position,
    const std::string& lane_lateral_position,
    const std::string& crp_id_begin,
    const std::string& crp_id_end,
    const std::string& lane_vertical_position,
    const std::string& lane_id,
    const std::string& dx_lane,
    const std::string& dy_lane,
    const std::string& dh_lane,
    const std::string& semi_axis_length_major,
    const std::string& semi_axis_length_minor,
    const std::string& ellipse_orientation,
    const std::string& altitude_accuracy,
    const std::string& reference_point,
    const std::string& move_direction_value,
    const std::string& move_direction_accuracy,
    const std::string& speed_value,
    const std::string& speed_accuracy,
    const std::string& yaw_rate_value,
    const std::string& yaw_rate_accuracy,
    const std::string& acceleration_value,
    const std::string& acceleration_accuracy,
    const std::string& orientation_value,
    const std::string& orientation_accuracy,
    const std::string& length_value,
    const std::string& length_accuracy,
    const std::string& width_value,
    const std::string& width_accuracy,
    const std::string& height_value,
    const std::string& height_accuracy,
    const std::string& static_status,
    const std::string& shift_position,
    const std::string& steering_angle_front,
    const std::string& steering_angle_rear,
    const std::string& brake_state,
    const std::string& auxiliary_brake_state,
    const std::string& throttle_position,
    const std::string& exterior_lights,
    const std::string& adaptive_cruise_control_system,
    const std::string& cooperative_adaptive_cruise_control_system,
    const std::string& pre_crash_safety_system,
    const std::string& antilock_brake_system,
    const std::string& traction_control_system,
    const std::string& electronic_stability_control_system,
    const std::string& lane_keeping_assist_system,
    const std::string& lane_departure_warning_system,
    const std::string& vehicle_size_type,
    const std::string& tracking_status,
    const std::string& detection_count,
    const std::string& lost_count,
    const std::string& object_age,
    const std::string& vehicle_role,
    const std::string& vehicle_extended_info,
    const std::string& towing_vehicle,
    const std::string& information_source_list) {
    QueryResult o;
    const auto name_list = CreateNameList_o();
    for (const auto name : name_list) {
      if (name == "object_id") o[name] = id;
      else if (name == "time") o[name] = time;
      else if (name == "object_class_id_0") o[name] = object_class_id_0;
      else if (name == "object_class_confidence_0") o[name] = object_class_confidence_0;
      else if (name == "object_class_subclass_type_0") o[name] = object_class_subclass_type_0;
      else if (name == "object_class_subclass_confidence_0") o[name] = object_class_subclass_confidence_0;
      else if (name == "object_class_id_1") o[name] = object_class_id_1;
      else if (name == "object_class_confidence_1") o[name] = object_class_confidence_1;
      else if (name == "object_class_subclass_type_1") o[name] = object_class_subclass_type_1;
      else if (name == "object_class_subclass_confidence_1") o[name] = object_class_subclass_confidence_1;
      else if (name == "object_class_id_2") o[name] = object_class_id_2;
      else if (name == "object_class_confidence_2") o[name] = object_class_confidence_2;
      else if (name == "object_class_subclass_type_2") o[name] = object_class_subclass_type_2;
      else if (name == "object_class_subclass_confidence_2") o[name] = object_class_subclass_confidence_2;
      else if (name == "object_class_id_3") o[name] = object_class_id_3;
      else if (name == "object_class_confidence_3") o[name] = object_class_confidence_3;
      else if (name == "object_class_subclass_type_3") o[name] = object_class_subclass_type_3;
      else if (name == "object_class_subclass_confidence_3") o[name] = object_class_subclass_confidence_3;
      else if (name == "existency") o[name] = existency;
      else if (name == "geodetic_system") o[name] = geodetic_system;
      else if (name == "latitude") o[name] = latitude;
      else if (name == "longitude") o[name] = longitude;
      else if (name == "altitude") o[name] = altitude;
      else if (name == "crp_id") o[name] = crp_id;
      else if (name == "dx_crp") o[name] = dx_crp;
      else if (name == "dy_crp") o[name] = dy_crp;
      else if (name == "dh_crp") o[name] = dh_crp;
      else if (name == "lane_count") o[name] = lane_count;
      else if (name == "lane_position") o[name] = lane_position;
      else if (name == "lane_lateral_position") o[name] = lane_lateral_position;
      else if (name == "crp_id_begin") o[name] = crp_id_begin;
      else if (name == "crp_id_end") o[name] = crp_id_end;
      else if (name == "lane_vertical_position") o[name] = lane_vertical_position;
      else if (name == "lane_id") o[name] = lane_id;
      else if (name == "dx_lane") o[name] = dx_lane;
      else if (name == "dy_lane") o[name] = dy_lane;
      else if (name == "dh_lane") o[name] = dh_lane;
      else if (name == "semi_axis_length_major") o[name] = semi_axis_length_major;
      else if (name == "semi_axis_length_minor") o[name] = semi_axis_length_minor;
      else if (name == "ellipse_orientation") o[name] = ellipse_orientation;
      else if (name == "altitude_accuracy") o[name] = altitude_accuracy;
      else if (name == "reference_point") o[name] = reference_point;
      else if (name == "move_direction_value") o[name] = move_direction_value;
      else if (name == "move_direction_accuracy") o[name] = move_direction_accuracy;
      else if (name == "speed_value") o[name] = speed_value;
      else if (name == "speed_accuracy") o[name] = speed_accuracy;
      else if (name == "yaw_rate_value") o[name] = yaw_rate_value;
      else if (name == "yaw_rate_accuracy") o[name] = yaw_rate_accuracy;
      else if (name == "acceleration_value") o[name] = acceleration_value;
      else if (name == "acceleration_accuracy") o[name] = acceleration_accuracy;
      else if (name == "orientation_value") o[name] = orientation_value;
      else if (name == "orientation_accuracy") o[name] = orientation_accuracy;
      else if (name == "length_value") o[name] = length_value;
      else if (name == "length_accuracy") o[name] = length_accuracy;
      else if (name == "width_value") o[name] = width_value;
      else if (name == "width_accuracy") o[name] = width_accuracy;
      else if (name == "height_value") o[name] = height_value;
      else if (name == "height_accuracy") o[name] = height_accuracy;
      else if (name == "shift_position") o[name] = shift_position;
      else if (name == "steering_angle_front") o[name] = steering_angle_front;
      else if (name == "steering_angle_rear") o[name] = steering_angle_rear;
      else if (name == "brake_state") o[name] = brake_state;
      else if (name == "auxiliary_brake_state") o[name] = auxiliary_brake_state;
      else if (name == "throttle_position") o[name] = throttle_position;
      else if (name == "exterior_lights") o[name] = exterior_lights;
      else if (name == "adaptive_cruise_control_system") o[name] = adaptive_cruise_control_system;
      else if (name == "cooperative_adaptive_cruise_control_system") o[name] = cooperative_adaptive_cruise_control_system;
      else if (name == "pre_crash_safety_system") o[name] = pre_crash_safety_system;
      else if (name == "antilock_brake_system") o[name] = antilock_brake_system;
      else if (name == "traction_control_system") o[name] = traction_control_system;
      else if (name == "electronic_stability_control_system") o[name] = electronic_stability_control_system;
      else if (name == "lane_keeping_assist_system") o[name] = lane_keeping_assist_system;
      else if (name == "lane_departure_warning_system") o[name] = lane_departure_warning_system;
      else if (name == "vehicle_role") o[name] = vehicle_role;
      else if (name == "vehicle_extended_info") o[name] = vehicle_extended_info;
      else if (name == "towing_vehicle") o[name] = towing_vehicle;
      else if (name == "information_source_list") o[name] = information_source_list;
      else o[name] = "0";
    }
    return o;
  }

  struct CandidateTestCase_o {
    CandidateTestCase_o() {
      // 初期値は有効とされている値を設定する
      map["object_id"] = {"12341234"};
      map["time"] = {"5000000"};
      map["object_class_id_0"] = {"1"};
      map["object_class_confidence_0"] = {"2"};
      map["object_class_subclass_type_0"] = {"3"};
      map["object_class_subclass_confidence_0"] = {"4"};
      map["object_class_id_1"] = {"46"};
      map["object_class_confidence_1"] = {"136"};
      map["object_class_subclass_type_1"] = {"23"};
      map["object_class_subclass_confidence_1"] = {"69"};
      map["object_class_id_2"] = {"55"};
      map["object_class_confidence_2"] = {"66"};
      map["object_class_subclass_type_2"] = {"77"};
      map["object_class_subclass_confidence_2"] = {"88"};
      map["object_class_id_3"] = {"243"};
      map["object_class_confidence_3"] = {"242"};
      map["object_class_subclass_type_3"] = {"98"};
      map["object_class_subclass_confidence_3"] = {"210"};
      map["existency"] = {"105"};
      map["geodetic_srid"] = {"3322"};
      map["latitude"] = {"235"};
      map["longitude"] = {"666"};
      map["altitude"] = {"987"};
      map["crp_id"] = {"12343"};
      map["dx_crp"] = {"500000"};
      map["dy_crp"] = {"186"};
      map["dh_crp"] = {"6688"};
      map["lane_count"] = {"108"};
      map["lane_position"] = {"8"};
      map["lane_lateral_position"] = {"202"};
      map["crp_id_begin"] = {"9458"};
      map["crp_id_end"] = {"60000"};
      map["lane_vertical_position"] = {"12347"};
      map["lane_id"] = {"3254"};
      map["dx_lane"] = {"4758"};
      map["dy_lane"] = {"5689"};
      map["dh_lane"] = {"9865"};
      map["semi_axis_length_major"] = {"11200"};
      map["semi_axis_length_minor"] = {"22011"};
      map["ellipse_orientation"] = {"3547"};
      map["altitude_accuracy"] = {"6999"};
      map["reference_point"] = {"6"};
      map["move_direction_value"] = {"57920"};
      map["move_direction_accuracy"] = {"2000"};
      map["speed_value"] = {"3456"};
      map["speed_accuracy"] = {"35779"};
      map["yaw_rate_value"] = {"1596"};
      map["yaw_rate_accuracy"] = {"3245"};
      map["acceleration_value"] = {"3254"};
      map["acceleration_accuracy"] = {"12541"};
      map["orientation_value"] = {"9988"};
      map["orientation_accuracy"] = {"6342"};
      map["length_value"] = {"8426"};
      map["length_accuracy"] = {"5236"};
      map["width_value"] = {"7845"};
      map["width_accuracy"] = {"6541"};
      map["height_value"] = {"9019"};
      map["height_accuracy"] = {"4321"};
      map["shift_position"] = {"40"};
      map["steering_angle_front"] = {"50"};
      map["steering_angle_rear"] = {"60"};
      map["brake_state"] = {"70"};
      map["auxiliary_brake_state"] = {"255"};
      map["throttle_position"] = {"90"};
      map["exterior_lights"] = {"110"};
      map["adaptive_cruise_control_system"] = {"119"};
      map["cooperative_adaptive_cruise_control_system"] = {"180"};
      map["pre_crash_safety_system"] = {"24"};
      map["antilock_brake_system"] = {"114"};
      map["traction_control_system"] = {"128"};
      map["electronic_stability_control_system"] = {"230"};
      map["lane_keeping_assist_system"] = {"212"};
      map["lane_departure_warning_system"] = {"204"};
      map["vehicle_role"] = {"176"};
      map["vehicle_extended_info"] = {"236"};
      map["towing_vehicle"] = {"630000"};
      map["information_source_list"] = {"[56785678]"};
      length = 1;
    }

    /**
     * @brief 任意のキーの値を更新する
     *
     * このとき，キー毎の要素数に差がうまれるとResultSetとしては不完全になる
     * 要素数に差がうまれるような更新が行われた場合は，他のキーの要素数を増やして同じにする処理を行う
     * 増やす際は，末尾の値をpush_backする(0とかはCooL4においては不正値になることがあるため)
     *
     * @param key
     * @param values
     */
    void Update_o(const std::string& key, const std::vector<std::string>& values) {
      map[key] = values;

      length = 0;
      for (const auto& e : map) {
        if (length < e.second.size()) length = e.second.size();
      }

      for (auto& e : map) {
        const size_t dl = length - e.second.size();
        for (size_t i = 0; i < dl; ++i) {
          e.second.push_back(e.second.back());
        }
      }
    }

    std::map<std::string, std::vector<std::string>> map;
    size_t length;
  };

  ResultSet CreateResultSet_o(CandidateTestCase_o& input) {
    std::vector<QueryResult> datas_list;
    std::vector<QueryResult> timestamps_list;
    std::vector<QueryResult> is_nulls_list;
    
    for (size_t i = 0; i < input.length; ++i) {
      datas_list.push_back(CreateDatas_o(
        input.map["object_id"][i],
        input.map["time"][i],
        input.map["revision"][i],
        input.map["object_class_id_0"][i],
        input.map["object_class_confidence_0"][i],
        input.map["object_class_subclass_type_0"][i],
        input.map["object_class_subclass_confidence_0"][i],
        input.map["object_class_id_1"][i],
        input.map["object_class_confidence_1"][i],
        input.map["object_class_subclass_type_1"][i],
        input.map["object_class_subclass_confidence_1"][i],
        input.map["object_class_id_2"][i],
        input.map["object_class_confidence_2"][i],
        input.map["object_class_subclass_type_2"][i],
        input.map["object_class_subclass_confidence_2"][i],
        input.map["object_class_id_3"][i],
        input.map["object_class_confidence_3"][i],
        input.map["object_class_subclass_type_3"][i],
        input.map["object_class_subclass_confidence_3"][i],
        input.map["existency"][i],
        input.map["geodetic_system"][i],
        input.map["latitude"][i],
        input.map["longitude"][i],
        input.map["altitude"][i],
        input.map["projective_srid"][i],
        input.map["prc_x"][i],
        input.map["prc_y"][i],
        input.map["prc_z"][i],
        input.map["crp_id"][i],
        input.map["dx_crp"][i],
        input.map["dy_crp"][i],
        input.map["dh_crp"][i],
        input.map["lane_count"][i],
        input.map["lane_position"][i],
        input.map["lane_lateral_position"][i],
        input.map["crp_id_begin"][i],
        input.map["crp_id_end"][i],
        input.map["lane_vertical_position"][i],
        input.map["lane_id"][i],
        input.map["dx_lane"][i],
        input.map["dy_lane"][i],
        input.map["dh_lane"][i],
        input.map["semi_axis_length_major"][i],
        input.map["semi_axis_length_minor"][i],
        input.map["ellipse_orientation"][i],
        input.map["altitude_accuracy"][i],
        input.map["reference_point"][i],
        input.map["move_direction_value"][i],
        input.map["move_direction_accuracy"][i],
        input.map["speed_value"][i],
        input.map["speed_accuracy"][i],
        input.map["yaw_rate_value"][i],
        input.map["yaw_rate_accuracy"][i],
        input.map["acceleration_value"][i],
        input.map["acceleration_accuracy"][i],
        input.map["orientation_value"][i],
        input.map["orientation_accuracy"][i],
        input.map["length_value"][i],
        input.map["length_accuracy"][i],
        input.map["width_value"][i],
        input.map["width_accuracy"][i],
        input.map["height_value"][i],
        input.map["height_accuracy"][i],
        input.map["static_status"][i],
        input.map["shift_position"][i],
        input.map["steering_angle_front"][i],
        input.map["steering_angle_rear"][i],
        input.map["brake_state"][i],
        input.map["auxiliary_brake_state"][i],
        input.map["throttle_position"][i],
        input.map["exterior_lights"][i],
        input.map["adaptive_cruise_control_system"][i],
        input.map["cooperative_adaptive_cruise_control_system"][i],
        input.map["pre_crash_safety_system"][i],
        input.map["antilock_brake_system"][i],
        input.map["traction_control_system"][i],
        input.map["electronic_stability_control_system"][i],
        input.map["lane_keeping_assist_system"][i],
        input.map["lane_departure_warning_system"][i],
        input.map["vehicle_size_type"][i],
        input.map["tracking_status"][i],
        input.map["detection_count"][i],
        input.map["lost_count"][i],
        input.map["object_age"][i],
        input.map["vehicle_role"][i],
        input.map["vehicle_extended_info"][i],
        input.map["towing_vehicle"][i],
        input.map["information_source_list"][i]));
      timestamps_list.push_back(CreateTimestamps_o());
      is_nulls_list.push_back(CreateIsNulls_o());
    }
    return ResultSet(datas_list, timestamps_list, is_nulls_list, CreateQueryResultMetaData_o());
  }

}
