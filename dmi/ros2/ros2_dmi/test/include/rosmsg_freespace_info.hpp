#ifndef TEST_INCLUDE_ROSMSG_FREESPACE_INFO_HPP_
#define TEST_INCLUDE_ROSMSG_FREESPACE_INFO_HPP_

#include <vector>

#include <dm_freespace_info_msgs/msg/freespace_info_array.hpp>

namespace ros2_dmi {
namespace test_util {
/**
 * @brief FreespaceInfoArray型のRosメッセージデータを作成する
 * 
 * @return FreespaceInfoArray型のRosメッセージデータ
 */
FreespaceInfoArray CreateFreespaceInfoArray() {
  FreespaceInfo data_1;
  data_1.id.value = 31415926535;
  data_1.time.value = 1;
  data_1.existency.value = 2;
  data_1.minimal_detectable_size.value = 3;
  data_1.position_begin.geodetic_srid.value = 4326;
  data_1.position_begin.latitude.value = 365814685;
  data_1.position_begin.longitude.value = 1366428183;
  data_1.position_begin.altitude.value = 820;
  data_1.position_begin.crp_id.value = 4;
  data_1.position_begin.dx_crp.value = 5;
  data_1.position_begin.dy_crp.value = 6;
  data_1.position_begin.dh_crp.value = 7;
  data_1.position_begin.lane_count.value = 8;
  data_1.position_begin.lane_position.value = 9;
  data_1.position_begin.lane_lateral_position.value = 10;
  data_1.position_begin.crp_id_begin.value = 11;
  data_1.position_begin.crp_id_end.value = 12;
  data_1.position_begin.lane_vertical_position.value = 13;
  data_1.position_begin.lane_id.value = 11112222;
  data_1.position_begin.dx_lane.value = 14;
  data_1.position_begin.dy_lane.value = 15;
  data_1.position_begin.dh_lane.value = 16;
  data_1.position_begin.semi_axis_length_major.value = 17;
  data_1.position_begin.semi_axis_length_minor.value = 18;
  data_1.position_begin.orientation.value = 19;
  data_1.position_begin.altitude_accuracy.value = 20;
  data_1.position_end.geodetic_srid.value = 4326;
  data_1.position_end.latitude.value = 365815238;
  data_1.position_end.longitude.value = 1366428678;
  data_1.position_end.altitude.value = 820;
  data_1.position_end.crp_id.value = 21;
  data_1.position_end.dx_crp.value = 22;
  data_1.position_end.dy_crp.value = 23;
  data_1.position_end.dh_crp.value = 24;
  data_1.position_end.lane_count.value = 25;
  data_1.position_end.lane_position.value = 26;
  data_1.position_end.lane_lateral_position.value = 27;
  data_1.position_end.crp_id_begin.value = 28;
  data_1.position_end.crp_id_end.value = 29;
  data_1.position_end.lane_vertical_position.value = 30;
  data_1.position_end.lane_id.value = 22223333;
  data_1.position_end.dx_lane.value = 31;
  data_1.position_end.dy_lane.value = 32;
  data_1.position_end.dh_lane.value = 33;
  data_1.position_end.semi_axis_length_major.value = 34;
  data_1.position_end.semi_axis_length_minor.value = 35;
  data_1.position_end.orientation.value = 36;
  data_1.position_end.altitude_accuracy.value = 37;
  data_1.length.value.value = 30;
  data_1.length.accuracy.value = 38;
  data_1.id_begin.value = 33334444;
  data_1.id_end.value = 44445555;
  ObjectId id1;
  id1.value = 55556666;
  std::vector<ObjectId> id_msg1;
  id_msg1.push_back(id1);
  data_1.information_source_list = id_msg1;

  FreespaceInfo data_2;
  data_2.id.value = 31415926536;
  data_2.time.value = 2;
  data_2.existency.value = 3;
  data_2.minimal_detectable_size.value = 4;
  data_2.position_begin.geodetic_srid.value = 4325;
  data_2.position_begin.latitude.value = 365814684;
  data_2.position_begin.longitude.value = 1366428182;
  data_2.position_begin.altitude.value = 821;
  data_2.position_begin.crp_id.value = 5;
  data_2.position_begin.dx_crp.value = 6;
  data_2.position_begin.dy_crp.value = 7;
  data_2.position_begin.dh_crp.value = 8;
  data_2.position_begin.lane_count.value = 9;
  data_2.position_begin.lane_position.value = 10;
  data_2.position_begin.lane_lateral_position.value = 11;
  data_2.position_begin.crp_id_begin.value = 12;
  data_2.position_begin.crp_id_end.value = 13;
  data_2.position_begin.lane_vertical_position.value = 14;
  data_2.position_begin.lane_id.value = 11112223;
  data_2.position_begin.dx_lane.value = 15;
  data_2.position_begin.dy_lane.value = 16;
  data_2.position_begin.dh_lane.value = 17;
  data_2.position_begin.semi_axis_length_major.value = 18;
  data_2.position_begin.semi_axis_length_minor.value = 19;
  data_2.position_begin.orientation.value = 20;
  data_2.position_begin.altitude_accuracy.value = 21;
  data_2.position_end.geodetic_srid.value = 4327;
  data_2.position_end.latitude.value = 365815237;
  data_2.position_end.longitude.value = 1366428677;
  data_2.position_end.altitude.value = 821;
  data_2.position_end.crp_id.value = 22;
  data_2.position_end.dx_crp.value = 23;
  data_2.position_end.dy_crp.value = 24;
  data_2.position_end.dh_crp.value = 25;
  data_2.position_end.lane_count.value = 26;
  data_2.position_end.lane_position.value = 27;
  data_2.position_end.lane_lateral_position.value = 28;
  data_2.position_end.crp_id_begin.value = 29;
  data_2.position_end.crp_id_end.value = 30;
  data_2.position_end.lane_vertical_position.value = 31;
  data_2.position_end.lane_id.value = 22223334;
  data_2.position_end.dx_lane.value = 32;
  data_2.position_end.dy_lane.value = 33;
  data_2.position_end.dh_lane.value = 34;
  data_2.position_end.semi_axis_length_major.value = 35;
  data_2.position_end.semi_axis_length_minor.value = 36;
  data_2.position_end.orientation.value = 37;
  data_2.position_end.altitude_accuracy.value = 38;
  data_2.length.value.value = 31;
  data_2.length.accuracy.value = 39;
  data_2.id_begin.value = 33334445;
  data_2.id_end.value = 44445556;
  ObjectId id2;
  id2.value = 55556667;
  std::vector<ObjectId> id_msg2;
  id_msg2.push_back(id2);
  data_2.information_source_list = id_msg2;

  FreespaceInfoArray msg;
  msg.array.push_back(data_1);
  msg.array.push_back(data_2);

  return msg;
}


}  // namespace test_util
}  // namespace ros2_dmi

#endif  //  TEST_INCLUDE_ROSMSG_FREESPACE_INFO_HPP_
