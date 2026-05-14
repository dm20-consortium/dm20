#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <chrono>

#include <is/Tuple.h>
#include <is/TupleSet.h>

#include "ros2_dmi/freespace_info_converter.hpp"
#include "resultset_freespace_info.hpp"
#include "rosmsg_freespace_info.hpp"

namespace ros2_dmi {
namespace test {

/**
 * @brief ResultSetをFreespaceInfo型のROSメッセージに変換することを検証する
 *
 * - 入力値: resultset
 *  - map["id"] = {"12341234"};
 *  - map["time"] = {"5000000"};
 *  - map["existency"] = {"27"};
 *  - map["minimal_detectable_size"] = {"123"};
 *  - map["position_begin_geodetic_srid"] = {"456"};
 *  - map["position_begin_latitude"] = {"789"};
 *  - map["position_begin_longitude"] = {"3322"};
 *  - map["position_begin_altitude"] = {"125"};
 *  - map["position_begin_crp_id"] = {"523"};
 *  - map["position_begin_dx_crp"] = {"633"};
 *  - map["position_begin_dy_crp"] = {"454"};
 *  - map["position_begin_dh_crp"] = {"963"};
 *  - map["position_begin_lane_count"] = {"84"};
 *  - map["position_begin_lane_position"] = {"-27"};
 *  - map["position_begin_lane_lateral_position"] = {"159"};
 *  - map["position_begin_crp_id_begin"] = {"753"};
 *  - map["position_begin_crp_id_end"] = {"486"};
 *  - map["position_begin_lane_vertical_position"] = {"426"};
 *  - map["position_begin_lane_id"] = {"6314"};
 *  - map["position_begin_dx_lane"] = {"842"};
 *  - map["position_begin_dy_lane"] = {"862"};
 *  - map["position_begin_dh_lane"] = {"9852"};
 *  - map["position_begin_semi_axis_length_major"] = {"336"};
 *  - map["position_begin_semi_axis_length_minor"] = {"4567"};
 *  - map["position_begin_ellipse_orientation"] = {"7654"};
 *  - map["position_begin_altitude_accuracy"] = {"987"};
 *  - map["position_end_geodetic_srid"] = {"654"};
 *  - map["position_end_latitude"] = {"321"};
 *  - map["position_end_longitude"] = {"12541"};
 *  - map["position_end_altitude"] = {"6652"};
 *  - map["position_end_crp_id"] = {"1100"};
 *  - map["position_end_dx_crp"] = {"120"};
 *  - map["position_end_dy_crp"] = {"3698"};
 *  - map["position_end_dh_crp"] = {"8563"};
 *  - map["position_end_lane_count"] = {"10"};
 *  - map["position_end_lane_position"] = {"-72"};
 *  - map["position_end_lane_lateral_position"] = {"73"};
 *  - map["position_end_crp_id_begin"] = {"6521"};
 *  - map["position_end_crp_id_end"] = {"4589"};
 *  - map["position_end_lane_vertical_position"] = {"3214"};
 *  - map["position_end_lane_id"] = {"74523"};
 *  - map["position_end_dx_lane"] = {"652"};
 *  - map["position_end_dy_lane"] = {"8521"};
 *  - map["position_end_dh_lane"] = {"6332"};
 *  - map["position_end_semi_axis_length_major"] = {"1225"};
 *  - map["position_end_semi_axis_length_minor"] = {"63268"};
 *  - map["position_end_ellipse_orientation"] = {"9633"};
 *  - map["position_end_altitude_accuracy"] = {"1202"};
 *  - map["length_value"] = {"1010"};
 *  - map["length_accuracy"] = {"2022"};
 *  - map["id_begin"] = {"652"};
 *  - map["id_end"] = {"98563"};
 *  - map["information_source_list"] = {"[321654]"};
 *  - length = 1;
 *
 * - 期待する返却値:
 *  - id.value == 12341234, 
 *  - time.value == 5000000, 
 *  - existency.value == 27, 
 *  - minimal_detectable_size.value == 123, 
 *  - position_begin.geodetic_srid.value == 456, 
 *  - position_begin.latitude.value == 789, 
 *  - position_begin.longitude.value == 3322,
 *  - position_begin.altitude.value == 125, 
 *  - position_begin.crp_id.value == 523, 
 *  - position_begin.dx_crp.value == 633, 
 *  - position_begin.dy_crp.value == 454, 
 *  - position_begin.dh_crp.value == 963, 
 *  - position_begin.lane_count.value == 84, 
 *  - position_begin.lane_position.value == -27, 
 *  - position_begin.lane_lateral_position.value == 159, 
 *  - position_begin.crp_id_begin.value == 753, 
 *  - position_begin.crp_id_end.value == 486, 
 *  - position_begin.lane_vertical_position.value == 426, 
 *  - position_begin.lane_id.value == 6314, 
 *  - position_begin.dx_lane.value == 842, 
 *  - position_begin.dy_lane.value == 862, 
 *  - position_begin.dh_lane.value == 9852, 
 *  - position_begin.semi_axis_length_major.value == 336, 
 *  - position_begin.semi_axis_length_minor.value == 4567, 
 *  - position_begin.orientation.value == 7654, 
 *  - position_begin.altitude_accuracy.value == 987, 
 *  - position_end.geodetic_srid.value == 654, 
 *  - position_end.latitude.value == 321, 
 *  - position_end.longitude.value == 12541, 
 *  - position_end.altitude.value == 6652, 
 *  - position_end.crp_id.value == 1100, 
 *  - position_end.dx_crp.value == 120, 
 *  - position_end.dy_crp.value == 3698, 
 *  - position_end.dh_crp.value == 8563, 
 *  - position_end.lane_count.value == 10, 
 *  - position_end.lane_position.value == -72, 
 *  - position_end.lane_lateral_position.value == 73, 
 *  - position_end.crp_id_begin.value == 6521, 
 *  - position_end.crp_id_end.value == 4589, 
 *  - position_end.lane_vertical_position.value == 3214, 
 *  - position_end.lane_id.value == 74523, 
 *  - position_end.dx_lane.value == 652, 
 *  - position_end.dy_lane.value == 8521, 
 *  - position_end.dh_lane.value == 6332, 
 *  - position_end.semi_axis_length_major.value == 1225, 
 *  - position_end.semi_axis_length_minor.value == 63268, 
 *  - position_end.orientation.value == 9633, 
 *  - position_end.altitude_accuracy.value == 1202, 
 *  - length.value.value == 1010, 
 *  - length.accuracy.value == 2022, 
 *  - id_begin.value == 652, 
 *  - id_end.value == 98563, 
 *  - information_source_list[0].value == 321654, 
 */

TEST(TestConverterFreespaceInfo, RsToRosNormal) {
  // setup
  test_util::CandidateTestCase_f data = test_util::CandidateTestCase_f();
  ResultSet test_in = test_util::CreateResultSet_f(data);
  

  // exercise
  const auto test_out =
      Converter<FreespaceInfo>::ResultSetToRos(
          test_in).get();

  // verify
  EXPECT_EQ(test_out.id.value, 12341234UL);
  EXPECT_EQ(test_out.time.value, 5000000UL);
  EXPECT_EQ(test_out.existency.value, 27U);
  EXPECT_EQ(test_out.minimal_detectable_size.value, 123U);
  EXPECT_EQ(test_out.position_begin.geodetic_srid.value, 456U);
  EXPECT_EQ(test_out.position_begin.latitude.value, 789);
  EXPECT_EQ(test_out.position_begin.longitude.value, 3322);
  EXPECT_EQ(test_out.position_begin.altitude.value, 125);
  EXPECT_EQ(test_out.position_begin.crp_id.value, 523U);
  EXPECT_EQ(test_out.position_begin.dx_crp.value, 633);
  EXPECT_EQ(test_out.position_begin.dy_crp.value, 454);
  EXPECT_EQ(test_out.position_begin.dh_crp.value, 963);
  EXPECT_EQ(test_out.position_begin.lane_count.value, 84U);
  EXPECT_EQ(test_out.position_begin.lane_position.value, -27);
  EXPECT_EQ(test_out.position_begin.lane_lateral_position.value, 159U);
  EXPECT_EQ(test_out.position_begin.crp_id_begin.value, 753U);
  EXPECT_EQ(test_out.position_begin.crp_id_end.value, 486U);
  EXPECT_EQ(test_out.position_begin.lane_vertical_position.value, 426U);
  EXPECT_EQ(test_out.position_begin.lane_id.value, 6314UL);
  EXPECT_EQ(test_out.position_begin.dx_lane.value, 842);
  EXPECT_EQ(test_out.position_begin.dy_lane.value, 862);
  EXPECT_EQ(test_out.position_begin.dh_lane.value, 9852);
  EXPECT_EQ(test_out.position_begin.semi_axis_length_major.value, 336U);
  EXPECT_EQ(test_out.position_begin.semi_axis_length_minor.value, 4567U);
  EXPECT_EQ(test_out.position_begin.orientation.value, 7654U);
  EXPECT_EQ(test_out.position_begin.altitude_accuracy.value, 987U);
  EXPECT_EQ(test_out.position_end.geodetic_srid.value, 654U);
  EXPECT_EQ(test_out.position_end.latitude.value, 321);
  EXPECT_EQ(test_out.position_end.longitude.value, 12541);
  EXPECT_EQ(test_out.position_end.altitude.value, 6652);
  EXPECT_EQ(test_out.position_end.crp_id.value, 1100U);
  EXPECT_EQ(test_out.position_end.dx_crp.value, 120);
  EXPECT_EQ(test_out.position_end.dy_crp.value, 3698);
  EXPECT_EQ(test_out.position_end.dh_crp.value, 8563);
  EXPECT_EQ(test_out.position_end.lane_count.value, 10U);
  EXPECT_EQ(test_out.position_end.lane_position.value, -72);
  EXPECT_EQ(test_out.position_end.lane_lateral_position.value, 73U);
  EXPECT_EQ(test_out.position_end.crp_id_begin.value, 6521U);
  EXPECT_EQ(test_out.position_end.crp_id_end.value, 4589U);
  EXPECT_EQ(test_out.position_end.lane_vertical_position.value, 3214U);
  EXPECT_EQ(test_out.position_end.lane_id.value, 74523UL);
  EXPECT_EQ(test_out.position_end.dx_lane.value, 652);
  EXPECT_EQ(test_out.position_end.dy_lane.value, 8521);
  EXPECT_EQ(test_out.position_end.dh_lane.value, 6332);
  EXPECT_EQ(test_out.position_end.semi_axis_length_major.value, 1225U);
  EXPECT_EQ(test_out.position_end.semi_axis_length_minor.value, 63268U);
  EXPECT_EQ(test_out.position_end.orientation.value, 9633U);
  EXPECT_EQ(test_out.position_end.altitude_accuracy.value, 1202U);
  EXPECT_EQ(test_out.length.value.value, 1010U);
  EXPECT_EQ(test_out.length.accuracy.value, 2022U);
  EXPECT_EQ(test_out.id_begin.value, 652UL);
  EXPECT_EQ(test_out.id_end.value, 98563UL);
  EXPECT_EQ(test_out.information_source_list[0].value, 321654UL);

  // teardown
}

/**
 * @brief 不正な値を持つResultSetデータをFreespaceInfo型のROSメッセージに変換すると、
 * boost::noneが返却されることを検証する
 * 
 * - 入力値: idの値が"test"のResultSetデータ
 * 
 * - 期待する返却値:　boost::none
 */

TEST(TestConverterFreespaceInfo, RsToRosAbnormal) {
  // setup
  test_util::CandidateTestCase_f data = test_util::CandidateTestCase_f();
  data.Update_f("id", {"test"});
  ResultSet test_in = test_util::CreateResultSet_f(data);
  

  // exercise
  const auto test_out_ros2msg_abnormal =
      Converter<FreespaceInfo>::ResultSetToRos(
          test_in);

  // verify
  EXPECT_EQ(test_out_ros2msg_abnormal, boost::none);

  // teardown
}



int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

}  // namespace test
}  // namespace ros2_dmi
