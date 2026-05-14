#ifndef TEST_INCLUDE_ROSMSG_OBJECT_INFO_HPP_
#define TEST_INCLUDE_ROSMSG_OBJECT_INFO_HPP_

#include <vector>

#include <dm_object_info_msgs/msg/object_info_array.hpp>

namespace ros2_dmi {
namespace test_util {
/**
 * @brief ObjectInfoArray型のRosメッセージデータを作成する
 * 
 * @return ObjectInfoArray型のRosメッセージデータ
 */
ObjectInfoArray CreateObjectInfoArray() {
    ObjectInfo data_1;
  data_1.id.value = 0x12341234;
  data_1.time.value = 5000000;
  std::vector<ObjectClass> object_class1;
  ObjectClass oc10;
  oc10.id.value = 1;
  oc10.confidence.value = 2;
  oc10.subclass_type.value = 3;
  oc10.subclass_confidence.value = 4;
  object_class1.push_back(oc10);
  ObjectClass oc11;
  oc11.id.value = 46;
  oc11.confidence.value = 136;
  oc11.subclass_type.value = 23;
  oc11.subclass_confidence.value = 69;
  object_class1.push_back(oc11);
  ObjectClass oc12;
  oc12.id.value = 55;
  oc12.confidence.value = 66;
  oc12.subclass_type.value = 77;
  oc12.subclass_confidence.value = 88;
  object_class1.push_back(oc12);
  ObjectClass oc13;
  oc13.id.value = 243;
  oc13.confidence.value = 242;
  oc13.subclass_type.value = 98;
  oc13.subclass_confidence.value = 210;
  object_class1.push_back(oc13);
  data_1.object_class = object_class1;
  data_1.existency.value = 105;
  data_1.object_location.geodetic_srid.value = 3322;
  data_1.object_location.latitude.value = 235;
  data_1.object_location.longitude.value = 666;
  data_1.object_location.altitude.value = 987;
  data_1.object_location.crp_id.value = 12343;
  data_1.object_location.dx_crp.value = 500000;
  data_1.object_location.dy_crp.value = 186;
  data_1.object_location.dh_crp.value = 6688;
  data_1.object_location.lane_count.value = 108;
  data_1.object_location.lane_position.value = 8;
  data_1.object_location.lane_lateral_position.value = 202;
  data_1.object_location.crp_id_begin.value = 9458;
  data_1.object_location.crp_id_end.value = 60000;
  data_1.object_location.lane_vertical_position.value = 12347;
  data_1.object_location.lane_id.value = 0x3254;
  data_1.object_location.dx_lane.value = 4758;
  data_1.object_location.dy_lane.value = 5689;
  data_1.object_location.dh_lane.value = 9865;
  data_1.object_location.semi_axis_length_major.value = 11200;
  data_1.object_location.semi_axis_length_minor.value = 22011;
  data_1.object_location.orientation.value = 3547;
  data_1.object_location.altitude_accuracy.value = 6999;
  data_1.ref_point.value = 6;
  data_1.direction.value.value = 57920;
  data_1.direction.accuracy.value = 2000;
  data_1.speed.value.value = 3456;
  data_1.speed.accuracy.value = 35779;
  data_1.yaw_rate.value.value = 1596;
  data_1.yaw_rate.accuracy.value = 3245;
  data_1.acceleration.value.value = 3254;
  data_1.acceleration.accuracy.value = 12541;
  data_1.orientation.value.value = 9988;
  data_1.orientation.accuracy.value = 6342;
  data_1.size.length.value.value = 8426;
  data_1.size.length.accuracy.value = 5236;
  data_1.size.width.value.value = 7845;
  data_1.size.width.accuracy.value = 6541;
  data_1.size.height.value.value = 9019;
  data_1.size.height.accuracy.value = 4321;
  // data_1.color.value = 30;
  data_1.shift_position.value = 40;
  data_1.steering_angle_front.value = 50;
  data_1.steering_angle_rear.value = 60;
  data_1.brake_state.value = 70;
  data_1.auxiliary_brake_state.value = 255;
  data_1.throttle_position.value = 90;
  data_1.exterior_lights.value = 110;
  data_1.control_system_states.adaptive_cruise_control_system.value = 119;
  data_1.control_system_states.cooperative_adaptive_cruise_control_system.value = 180;
  data_1.control_system_states.pre_crash_safety_system.value = 24;
  data_1.control_system_states.antilock_brake_system.value = 114;
  data_1.control_system_states.traction_control_system.value = 128;
  data_1.control_system_states.electronic_stability_control_system.value = 230;
  data_1.control_system_states.lane_keeping_assist_system.value = 212;
  data_1.control_system_states.lane_departure_warning_system.value = 204;
  data_1.vehicle_role.value = 176;
  data_1.vehicle_extended_info.value = 236;
  data_1.towing_vehicle.value = 0x630000;
  ObjectId id1;
  id1.value = 0x56785678;
  std::vector<ObjectId> id_msg1;
  id_msg1.push_back(id1);
  data_1.information_source_list = id_msg1;

ObjectInfo data_2;
  data_2.id.value = 0x12341235;
  data_2.time.value = 5000001;
  std::vector<ObjectClass> object_class2;
  ObjectClass oc20;
  oc20.id.value = 2;
  oc20.confidence.value = 3;
  oc20.subclass_type.value = 4;
  oc20.subclass_confidence.value = 5;
  object_class2.push_back(oc20);
  ObjectClass oc21;
  oc21.id.value = 47;
  oc21.confidence.value = 137;
  oc21.subclass_type.value = 24;
  oc21.subclass_confidence.value = 70;
  object_class2.push_back(oc21);
  ObjectClass oc22;
  oc22.id.value = 56;
  oc22.confidence.value = 67;
  oc22.subclass_type.value = 78;
  oc22.subclass_confidence.value = 89;
  object_class2.push_back(oc22);
  ObjectClass oc23;
  oc23.id.value = 244;
  oc23.confidence.value = 243;
  oc23.subclass_type.value = 99;
  oc23.subclass_confidence.value = 211;
  object_class2.push_back(oc23);
  data_2.object_class = object_class2;
  data_2.existency.value = 106;
  data_2.object_location.geodetic_srid.value = 3323;
  data_2.object_location.latitude.value = 236;
  data_2.object_location.longitude.value = 667;
  data_2.object_location.altitude.value = 988;
  data_2.object_location.crp_id.value = 12344;
  data_2.object_location.dx_crp.value = 500001;
  data_2.object_location.dy_crp.value = 187;
  data_2.object_location.dh_crp.value = 6689;
  data_2.object_location.lane_count.value = 109;
  data_2.object_location.lane_position.value = 9;
  data_2.object_location.lane_lateral_position.value = 203;
  data_2.object_location.crp_id_begin.value = 9459;
  data_2.object_location.crp_id_end.value = 60001;
  data_2.object_location.lane_vertical_position.value = 12348;
  data_2.object_location.lane_id.value = 0x3255;
  data_2.object_location.dx_lane.value = 4759;
  data_2.object_location.dy_lane.value = 5690;
  data_2.object_location.dh_lane.value = 9866;
  data_2.object_location.semi_axis_length_major.value = 11201;
  data_2.object_location.semi_axis_length_minor.value = 22012;
  data_2.object_location.orientation.value = 3548;
  data_2.object_location.altitude_accuracy.value = 7000;
  data_2.ref_point.value = 7;
  data_2.direction.value.value = 57921;
  data_2.direction.accuracy.value = 2001;
  data_2.speed.value.value = 3457;
  data_2.speed.accuracy.value = 35780;
  data_2.yaw_rate.value.value = 1597;
  data_2.yaw_rate.accuracy.value = 3246;
  data_2.acceleration.value.value = 3255;
  data_2.acceleration.accuracy.value = 12542;
  data_2.orientation.value.value = 9989;
  data_2.orientation.accuracy.value = 6343;
  data_2.size.length.value.value = 8427;
  data_2.size.length.accuracy.value = 5237;
  data_2.size.width.value.value = 7846;
  data_2.size.width.accuracy.value = 6542;
  data_2.size.height.value.value = 9020;
  data_2.size.height.accuracy.value = 4322;
  // data_2.color.value = 31;
  data_2.shift_position.value = 41;
  data_2.steering_angle_front.value = 51;
  data_2.steering_angle_rear.value = 61;
  data_2.brake_state.value = 71;
  data_2.auxiliary_brake_state.value = 254;
  data_2.throttle_position.value = 91;
  data_2.exterior_lights.value = 111;
  data_2.control_system_states.adaptive_cruise_control_system.value = 120;
  data_2.control_system_states.cooperative_adaptive_cruise_control_system.value = 181;
  data_2.control_system_states.pre_crash_safety_system.value = 25;
  data_2.control_system_states.antilock_brake_system.value = 115;
  data_2.control_system_states.traction_control_system.value = 129;
  data_2.control_system_states.electronic_stability_control_system.value = 231;
  data_2.control_system_states.lane_keeping_assist_system.value = 213;
  data_2.control_system_states.lane_departure_warning_system.value = 205;
  data_2.vehicle_role.value = 177;
  data_2.vehicle_extended_info.value = 237;
  data_2.towing_vehicle.value = 0x630001;
  ObjectId id2;
  id2.value = 0x56785679;
  std::vector<ObjectId> id_msg2;
  id_msg2.push_back(id2);
  data_2.information_source_list = id_msg2;

  ObjectInfoArray msg;
  msg.array.push_back(data_1);
  msg.array.push_back(data_2);

  return msg;
}

}  // namespace test_util
}  // namespace ros2_dmi

#endif  // TEST_INCLUDE_ROSMSG_OBJECT_INFO_HPP_
