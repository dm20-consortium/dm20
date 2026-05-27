#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <chrono>

#include <is/Tuple.h>
#include <is/TupleSet.h>

#include "ros2_dmi/freespace_info_converter.hpp"
#include "resultset_freespace_info.hpp"
#include "rosmsg_freespace_info.hpp"
#include "any_compat.hpp"
#include "test_optional_helper.hpp"

namespace ros2_dmi {
namespace test {

/**
 * @brief ResultSetをFreespaceInfoArray型のROSメッセージに変換することを検証する
 *
 * - 入力値: resultset
 *  - map["id"] = {"12341234", "12341235"};
 *  - map["time"] = {"5000000", "5000001"};
 *  - map["existency"] = {"27", "28"};
 *  - map["minimal_detectable_size"] = {"123", "124"};
 *  - map["position_begin_geodetic_srid"] = {"456", "457"};
 *  - map["position_begin_latitude"] = {"789", "790"};
 *  - map["position_begin_longitude"] = {"3322", "3323"};
 *  - map["position_begin_altitude"] = {"125", "126"};
 *  - map["position_begin_crp_id"] = {"523", "524"};
 *  - map["position_begin_dx_crp"] = {"633", "634"};
 *  - map["position_begin_dy_crp"] = {"454", "455"};
 *  - map["position_begin_dh_crp"] = {"963", "964"};
 *  - map["position_begin_lane_count"] = {"84", "85"};
 *  - map["position_begin_lane_position"] = {"-27", "-28"};
 *  - map["position_begin_lane_lateral_position"] = {"159", "160"};
 *  - map["position_begin_crp_id_begin"] = {"753", "754"};
 *  - map["position_begin_crp_id_end"] = {"486", "487"};
 *  - map["position_begin_lane_vertical_position"] = {"426", "427"};
 *  - map["position_begin_lane_id"] = {"6314", "6315"};
 *  - map["position_begin_dx_lane"] = {"842", "843"};
 *  - map["position_begin_dy_lane"] = {"862", "863"};
 *  - map["position_begin_dh_lane"] = {"9852", "9853"};
 *  - map["position_begin_semi_axis_length_major"] = {"336", "337"};
 *  - map["position_begin_semi_axis_length_minor"] = {"4567", "4568"};
 *  - map["position_begin_ellipse_orientation"] = {"7654", "7655"};
 *  - map["position_begin_altitude_accuracy"] = {"987", "988"};
 *  - map["position_end_geodetic_srid"] = {"654", "655"};
 *  - map["position_end_latitude"] = {"321", "322"};
 *  - map["position_end_longitude"] = {"12541", "12542"};
 *  - map["position_end_altitude"] = {"6652", "6653"};
 *  - map["position_end_crp_id"] = {"1100", "1101"};
 *  - map["position_end_dx_crp"] = {"120", "121"};
 *  - map["position_end_dy_crp"] = {"3698", "3699"};
 *  - map["position_end_dh_crp"] = {"8563", "8564"};
 *  - map["position_end_lane_count"] = {"10", "11"};
 *  - map["position_end_lane_position"] = {"-72", "-73"};
 *  - map["position_end_lane_lateral_position"] = {"73", "74"};
 *  - map["position_end_crp_id_begin"] = {"6521", "6522"};
 *  - map["position_end_crp_id_end"] = {"4589", "4590"};
 *  - map["position_end_lane_vertical_position"] = {"3214", "3215"};
 *  - map["position_end_lane_id"] = {"74523", "74524"};
 *  - map["position_end_dx_lane"] = {"652", "653"};
 *  - map["position_end_dy_lane"] = {"8521", "8522"};
 *  - map["position_end_dh_lane"] = {"6332", "6333"};
 *  - map["position_end_semi_axis_length_major"] = {"1225", "1226"};
 *  - map["position_end_semi_axis_length_minor"] = {"63268", "63269"};
 *  - map["position_end_ellipse_orientation"] = {"9633", "9634"};
 *  - map["position_end_altitude_accuracy"] = {"1202", "1203"};
 *  - map["length_value"] = {"1010", "1011"};
 *  - map["length_accuracy"] = {"2022", "2023"};
 *  - map["id_begin"] = {"652", "653"};
 *  - map["id_end"] = {"98563", "98564"};
 *  - map["information_source_list"] = {"[321654]", "[321655]"};
      length = 2;
 *
 * - 期待する返却値:
 *  - array[0].id.value == 12341234, 
 *  - array[0].time.value == 5000000, 
 *  - array[0].existency.value == 27, 
 *  - array[0].minimal_detectable_size.value == 123, 
 *  - array[0].position_begin.geodetic_srid.value == 456, 
 *  - array[0].position_begin.latitude.value == 789, 
 *  - array[0].position_begin.longitude.value == 3322,
 *  - array[0].position_begin.altitude.value == 125, 
 *  - array[0].position_begin.crp_id.value == 523, 
 *  - array[0].position_begin.dx_crp.value == 633, 
 *  - array[0].position_begin.dy_crp.value == 454, 
 *  - array[0].position_begin.dh_crp.value == 963, 
 *  - array[0].position_begin.lane_count.value == 84, 
 *  - array[0].position_begin.lane_position.value == -27, 
 *  - array[0].position_begin.lane_lateral_position.value == 159, 
 *  - array[0].position_begin.crp_id_begin.value == 753, 
 *  - array[0].position_begin.crp_id_end.value == 486, 
 *  - array[0].position_begin.lane_vertical_position.value == 426, 
 *  - array[0].position_begin.lane_id.value == 6314, 
 *  - array[0].position_begin.dx_lane.value == 842, 
 *  - array[0].position_begin.dy_lane.value == 862, 
 *  - array[0].position_begin.dh_lane.value == 9852, 
 *  - array[0].position_begin.semi_axis_length_major.value == 336, 
 *  - array[0].position_begin.semi_axis_length_minor.value == 4567, 
 *  - array[0].position_begin.orientation.value == 7654, 
 *  - array[0].position_begin.altitude_accuracy.value == 987, 
 *  - array[0].position_end.geodetic_srid.value == 654, 
 *  - array[0].position_end.latitude.value == 321, 
 *  - array[0].position_end.longitude.value == 12541, 
 *  - array[0].position_end.altitude.value == 6652, 
 *  - array[0].position_end.crp_id.value == 1100, 
 *  - array[0].position_end.dx_crp.value == 120, 
 *  - array[0].position_end.dy_crp.value == 3698, 
 *  - array[0].position_end.dh_crp.value == 8563, 
 *  - array[0].position_end.lane_count.value == 10, 
 *  - array[0].position_end.lane_position.value == -72, 
 *  - array[0].position_end.lane_lateral_position.value == 73, 
 *  - array[0].position_end.crp_id_begin.value == 6521, 
 *  - array[0].position_end.crp_id_end.value == 4589, 
 *  - array[0].position_end.lane_vertical_position.value == 3214, 
 *  - array[0].position_end.lane_id.value == 74523, 
 *  - array[0].position_end.dx_lane.value == 652, 
 *  - array[0].position_end.dy_lane.value == 8521, 
 *  - array[0].position_end.dh_lane.value == 6332, 
 *  - array[0].position_end.semi_axis_length_major.value == 1225, 
 *  - array[0].position_end.semi_axis_length_minor.value == 63268, 
 *  - array[0].position_end.orientation.value == 9633, 
 *  - array[0].position_end.altitude_accuracy.value == 1202, 
 *  - array[0].length.value.value == 1010, 
 *  - array[0].length.accuracy.value == 2022, 
 *  - array[0].id_begin.value == 652, 
 *  - array[0].id_end.value == 98563, 
 *  - array[0].information_source_list[0].value == 321654, 
 * 
 *  - array[1].id.value = 12341235UL,
 *  - array[1].time.value = 5000001UL,
 *  - array[1].existency.value = 28U,
 *  - array[1].minimal_detectable_size.value = 124U,
 *  - array[1].position_begin.geodetic_srid.value = 457U,
 *  - array[1].position_begin.latitude.value = 790,
 *  - array[1].position_begin.longitude.value = 3323,
 *  - array[1].position_begin.altitude.value = 126,
 *  - array[1].position_begin.crp_id.value = 524U,
 *  - array[1].position_begin.dx_crp.value = 634,
 *  - array[1].position_begin.dy_crp.value = 455,
 *  - array[1].position_begin.dh_crp.value = 964,
 *  - array[1].position_begin.lane_count.value = 85U,
 *  - array[1].position_begin.lane_position.value = -28,
 *  - array[1].position_begin.lane_lateral_position.value = 160U,
 *  - array[1].position_begin.crp_id_begin.value = 754U,
 *  - array[1].position_begin.crp_id_end.value = 487U,
 *  - array[1].position_begin.lane_vertical_position.value = 427U,
 *  - array[1].position_begin.lane_id.value = 6315UL,
 *  - array[1].position_begin.dx_lane.value = 843,
 *  - array[1].position_begin.dy_lane.value = 863,
 *  - array[1].position_begin.dh_lane.value = 9853,
 *  - array[1].position_begin.semi_axis_length_major.value = 337U,
 *  - array[1].position_begin.semi_axis_length_minor.value = 4568U,
 *  - array[1].position_begin.orientation.value = 7655U,
 *  - array[1].position_begin.altitude_accuracy.value = 988U,
 *  - array[1].position_end.geodetic_srid.value = 655U,
 *  - array[1].position_end.latitude.value = 322,
 *  - array[1].position_end.longitude.value = 12542,
 *  - array[1].position_end.altitude.value = 6653,
 *  - array[1].position_end.crp_id.value = 1101U,
 *  - array[1].position_end.dx_crp.value = 121,
 *  - array[1].position_end.dy_crp.value = 3699,
 *  - array[1].position_end.dh_crp.value = 8564,
 *  - array[1].position_end.lane_count.value = 11U,
 *  - array[1].position_end.lane_position.value = -73,
 *  - array[1].position_end.lane_lateral_position.value = 74U,
 *  - array[1].position_end.crp_id_begin.value = 6522U,
 *  - array[1].position_end.crp_id_end.value = 4590U,
 *  - array[1].position_end.lane_vertical_position.value = 3215U,
 *  - array[1].position_end.lane_id.value = 74524UL,
 *  - array[1].position_end.dx_lane.value = 653,
 *  - array[1].position_end.dy_lane.value = 8522,
 *  - array[1].position_end.dh_lane.value = 6333,
 *  - array[1].position_end.semi_axis_length_major.value = 1226U,
 *  - array[1].position_end.semi_axis_length_minor.value = 63269U,
 *  - array[1].position_end.orientation.value = 9634U,
 *  - array[1].position_end.altitude_accuracy.value = 1203U,
 *  - array[1].length.value.value = 1011U,
 *  - array[1].length.accuracy.value = 2023U,
 *  - array[1].id_begin.value = 653UL,
 *  - array[1].id_end.value = 98564UL,
 *  - array[1].information_source_list[0].value = 321655UL,
 */

TEST(TestConverterFreespaceInfoArray, RsToRosNormal) {
  // setup
  // データの作成
  test_util::CandidateTestCase_f data = test_util::CandidateTestCase_f();
  ResultSet test_in = test_util::CreateResultSet_f(data);
  

  // exercise
  // データを変換する
  const auto test_out =
      Converter<FreespaceInfoArray>::ResultSetToRos(
          test_in).get();

  // verify
  test_util::ExpectValueEq(test_out.array[0].id.value, 12341234UL);
  test_util::ExpectValueEq(test_out.array[0].time.value, 5000000UL);
  test_util::ExpectValueEq(test_out.array[0].existency.value, 27U);
  test_util::ExpectValueEq(test_out.array[0].minimal_detectable_size.value, 123U);
  test_util::ExpectValueEq(test_out.array[0].position_begin.geodetic_srid.value, 456U);
  test_util::ExpectValueEq(test_out.array[0].position_begin.latitude.value, 789);
  test_util::ExpectValueEq(test_out.array[0].position_begin.longitude.value, 3322);
  test_util::ExpectValueEq(test_out.array[0].position_begin.altitude.value, 125);
  test_util::ExpectValueEq(test_out.array[0].position_begin.crp_id.value, 523U);
  test_util::ExpectValueEq(test_out.array[0].position_begin.dx_crp.value, 633);
  test_util::ExpectValueEq(test_out.array[0].position_begin.dy_crp.value, 454);
  test_util::ExpectValueEq(test_out.array[0].position_begin.dh_crp.value, 963);
  test_util::ExpectValueEq(test_out.array[0].position_begin.lane_count.value, 84U);
  test_util::ExpectValueEq(test_out.array[0].position_begin.lane_position.value, -27);
  test_util::ExpectValueEq(test_out.array[0].position_begin.lane_lateral_position.value, 159U);
  test_util::ExpectValueEq(test_out.array[0].position_begin.crp_id_begin.value, 753U);
  test_util::ExpectValueEq(test_out.array[0].position_begin.crp_id_end.value, 486U);
  test_util::ExpectValueEq(test_out.array[0].position_begin.lane_vertical_position.value, 426U);
  test_util::ExpectValueEq(test_out.array[0].position_begin.lane_id.value, 6314UL);
  test_util::ExpectValueEq(test_out.array[0].position_begin.dx_lane.value, 842);
  test_util::ExpectValueEq(test_out.array[0].position_begin.dy_lane.value, 862);
  test_util::ExpectValueEq(test_out.array[0].position_begin.dh_lane.value, 9852);
  test_util::ExpectValueEq(test_out.array[0].position_begin.semi_axis_length_major.value, 336U);
  test_util::ExpectValueEq(test_out.array[0].position_begin.semi_axis_length_minor.value, 4567U);
  test_util::ExpectValueEq(test_out.array[0].position_begin.orientation.value, 7654U);
  test_util::ExpectValueEq(test_out.array[0].position_begin.altitude_accuracy.value, 987U);
  test_util::ExpectValueEq(test_out.array[0].position_end.geodetic_srid.value, 654U);
  test_util::ExpectValueEq(test_out.array[0].position_end.latitude.value, 321);
  test_util::ExpectValueEq(test_out.array[0].position_end.longitude.value, 12541);
  test_util::ExpectValueEq(test_out.array[0].position_end.altitude.value, 6652);
  test_util::ExpectValueEq(test_out.array[0].position_end.crp_id.value, 1100U);
  test_util::ExpectValueEq(test_out.array[0].position_end.dx_crp.value, 120);
  test_util::ExpectValueEq(test_out.array[0].position_end.dy_crp.value, 3698);
  test_util::ExpectValueEq(test_out.array[0].position_end.dh_crp.value, 8563);
  test_util::ExpectValueEq(test_out.array[0].position_end.lane_count.value, 10U);
  test_util::ExpectValueEq(test_out.array[0].position_end.lane_position.value, -72);
  test_util::ExpectValueEq(test_out.array[0].position_end.lane_lateral_position.value, 73U);
  test_util::ExpectValueEq(test_out.array[0].position_end.crp_id_begin.value, 6521U);
  test_util::ExpectValueEq(test_out.array[0].position_end.crp_id_end.value, 4589U);
  test_util::ExpectValueEq(test_out.array[0].position_end.lane_vertical_position.value, 3214U);
  test_util::ExpectValueEq(test_out.array[0].position_end.lane_id.value, 74523UL);
  test_util::ExpectValueEq(test_out.array[0].position_end.dx_lane.value, 652);
  test_util::ExpectValueEq(test_out.array[0].position_end.dy_lane.value, 8521);
  test_util::ExpectValueEq(test_out.array[0].position_end.dh_lane.value, 6332);
  test_util::ExpectValueEq(test_out.array[0].position_end.semi_axis_length_major.value, 1225U);
  test_util::ExpectValueEq(test_out.array[0].position_end.semi_axis_length_minor.value, 63268U);
  test_util::ExpectValueEq(test_out.array[0].position_end.orientation.value, 9633U);
  test_util::ExpectValueEq(test_out.array[0].position_end.altitude_accuracy.value, 1202U);
  test_util::ExpectValueEq(test_out.array[0].length.value.value, 1010U);
  test_util::ExpectValueEq(test_out.array[0].length.accuracy.value, 2022U);
  test_util::ExpectValueEq(test_out.array[0].id_begin.value, 652UL);
  test_util::ExpectValueEq(test_out.array[0].id_end.value, 98563UL);
  test_util::ExpectValueEq(test_out.array[0].information_source_list[0].value, 321654UL);

  test_util::ExpectValueEq(test_out.array[1].id.value, 12341235UL);
  test_util::ExpectValueEq(test_out.array[1].time.value, 5000001UL);
  test_util::ExpectValueEq(test_out.array[1].existency.value, 28U);
  test_util::ExpectValueEq(test_out.array[1].minimal_detectable_size.value, 124U);
  test_util::ExpectValueEq(test_out.array[1].position_begin.geodetic_srid.value, 457U);
  test_util::ExpectValueEq(test_out.array[1].position_begin.latitude.value, 790);
  test_util::ExpectValueEq(test_out.array[1].position_begin.longitude.value, 3323);
  test_util::ExpectValueEq(test_out.array[1].position_begin.altitude.value, 126);
  test_util::ExpectValueEq(test_out.array[1].position_begin.crp_id.value, 524U);
  test_util::ExpectValueEq(test_out.array[1].position_begin.dx_crp.value, 634);
  test_util::ExpectValueEq(test_out.array[1].position_begin.dy_crp.value, 455);
  test_util::ExpectValueEq(test_out.array[1].position_begin.dh_crp.value, 964);
  test_util::ExpectValueEq(test_out.array[1].position_begin.lane_count.value, 85U);
  test_util::ExpectValueEq(test_out.array[1].position_begin.lane_position.value, -28);
  test_util::ExpectValueEq(test_out.array[1].position_begin.lane_lateral_position.value, 160U);
  test_util::ExpectValueEq(test_out.array[1].position_begin.crp_id_begin.value, 754U);
  test_util::ExpectValueEq(test_out.array[1].position_begin.crp_id_end.value, 487U);
  test_util::ExpectValueEq(test_out.array[1].position_begin.lane_vertical_position.value, 427U);
  test_util::ExpectValueEq(test_out.array[1].position_begin.lane_id.value, 6315UL);
  test_util::ExpectValueEq(test_out.array[1].position_begin.dx_lane.value, 843);
  test_util::ExpectValueEq(test_out.array[1].position_begin.dy_lane.value, 863);
  test_util::ExpectValueEq(test_out.array[1].position_begin.dh_lane.value, 9853);
  test_util::ExpectValueEq(test_out.array[1].position_begin.semi_axis_length_major.value, 337U);
  test_util::ExpectValueEq(test_out.array[1].position_begin.semi_axis_length_minor.value, 4568U);
  test_util::ExpectValueEq(test_out.array[1].position_begin.orientation.value, 7655U);
  test_util::ExpectValueEq(test_out.array[1].position_begin.altitude_accuracy.value, 988U);
  test_util::ExpectValueEq(test_out.array[1].position_end.geodetic_srid.value, 655U);
  test_util::ExpectValueEq(test_out.array[1].position_end.latitude.value, 322);
  test_util::ExpectValueEq(test_out.array[1].position_end.longitude.value, 12542);
  test_util::ExpectValueEq(test_out.array[1].position_end.altitude.value, 6653);
  test_util::ExpectValueEq(test_out.array[1].position_end.crp_id.value, 1101U);
  test_util::ExpectValueEq(test_out.array[1].position_end.dx_crp.value, 121);
  test_util::ExpectValueEq(test_out.array[1].position_end.dy_crp.value, 3699);
  test_util::ExpectValueEq(test_out.array[1].position_end.dh_crp.value, 8564);
  test_util::ExpectValueEq(test_out.array[1].position_end.lane_count.value, 11U);
  test_util::ExpectValueEq(test_out.array[1].position_end.lane_position.value, -73);
  test_util::ExpectValueEq(test_out.array[1].position_end.lane_lateral_position.value, 74U);
  test_util::ExpectValueEq(test_out.array[1].position_end.crp_id_begin.value, 6522U);
  test_util::ExpectValueEq(test_out.array[1].position_end.crp_id_end.value, 4590U);
  test_util::ExpectValueEq(test_out.array[1].position_end.lane_vertical_position.value, 3215U);
  test_util::ExpectValueEq(test_out.array[1].position_end.lane_id.value, 74524UL);
  test_util::ExpectValueEq(test_out.array[1].position_end.dx_lane.value, 653);
  test_util::ExpectValueEq(test_out.array[1].position_end.dy_lane.value, 8522);
  test_util::ExpectValueEq(test_out.array[1].position_end.dh_lane.value, 6333);
  test_util::ExpectValueEq(test_out.array[1].position_end.semi_axis_length_major.value, 1226U);
  test_util::ExpectValueEq(test_out.array[1].position_end.semi_axis_length_minor.value, 63269U);
  test_util::ExpectValueEq(test_out.array[1].position_end.orientation.value, 9634U);
  test_util::ExpectValueEq(test_out.array[1].position_end.altitude_accuracy.value, 1203U);
  test_util::ExpectValueEq(test_out.array[1].length.value.value, 1011U);
  test_util::ExpectValueEq(test_out.array[1].length.accuracy.value, 2023U);
  test_util::ExpectValueEq(test_out.array[1].id_begin.value, 653UL);
  test_util::ExpectValueEq(test_out.array[1].id_end.value, 98564UL);
  test_util::ExpectValueEq(test_out.array[1].information_source_list[0].value, 321655UL);
  // teardown
}

/**
 * @brief 不正な値を持つResultSetデータをFreespaceInfoArray型のROSメッセージに変換すると、
 * boost::noneが返却されることを検証する
 * 
 * - 入力値: idの値が"test"のResultSetデータ
 * 
 * - 期待する返却値:　boost::none
 */

TEST(TestConverterFreespaceInfo, RsToRosAbnormal) {
  // setup
  test_util::CandidateTestCase_f data = test_util::CandidateTestCase_f();
  data.Update_f("id", {"test", "test2"});
  ResultSet test_in = test_util::CreateResultSet_f(data);
  

  // exercise
  const auto test_out_ros2msg_abnormal =
      Converter<FreespaceInfoArray>::ResultSetToRos(
          test_in);

  // verify
  EXPECT_FALSE(test_out_ros2msg_abnormal);

  // teardown
}

/**
 * @brief FreespaceInfoArray型のROSメッセージからTuple変換することを検証する
 * 
 * - 入力値: Ros2メッセージ:
 *   - array[0].id.value = 31415926535;
 *   - array[0].time.value = 1;
 *   - array[0].existency.value = 2;
 *   - array[0].minimal_detectable_size.value = 3;
 *   - array[0].position_begin.geodetic_srid.value = 4326;
 *   - array[0].position_begin.latitude.value = 365814685;
 *   - array[0].position_begin.longitude.value = 1366428183;
 *   - array[0].position_begin.altitude.value = 820;
 *   - array[0].position_begin.crp_id.value = 4;
 *   - array[0].position_begin.dx_crp.value = 5;
 *   - array[0].position_begin.dy_crp.value = 6;
 *   - array[0].position_begin.dh_crp.value = 7;
 *   - array[0].position_begin.lane_count.value = 8;
 *   - array[0].position_begin.lane_position.value = 9;
 *   - array[0].position_begin.lane_lateral_position.value = 10;
 *   - array[0].position_begin.crp_id_begin.value = 11;
 *   - array[0].position_begin.crp_id_end.value = 12;
 *   - array[0].position_begin.lane_vertical_position.value = 13;
 *   - array[0].position_begin.lane_id.value = 11112222;
 *   - array[0].position_begin.dx_lane.value = 14;
 *   - array[0].position_begin.dy_lane.value = 15;
 *   - array[0].position_begin.dh_lane.value = 16;
 *   - array[0].position_begin.semi_axis_length_major.value = 17;
 *   - array[0].position_begin.semi_axis_length_minor.value = 18;
 *   - array[0].position_begin.orientation.value = 19;
 *   - array[0].position_begin.altitude_accuracy.value = 20;
 *   - array[0].position_end.geodetic_srid.value = 4326;
 *   - array[0].position_end.latitude.value = 365815238;
 *   - array[0].position_end.longitude.value = 1366428678;
 *   - array[0].position_end.altitude.value = 820;
 *   - array[0].position_end.crp_id.value = 21;
 *   - array[0].position_end.dx_crp.value = 22;
 *   - array[0].position_end.dy_crp.value = 23;
 *   - array[0].position_end.dh_crp.value = 24;
 *   - array[0].position_end.lane_count.value = 25;
 *   - array[0].position_end.lane_position.value = 26;
 *   - array[0].position_end.lane_lateral_position.value = 27;
 *   - array[0].position_end.crp_id_begin.value = 28;
 *   - array[0].position_end.crp_id_end.value = 29;
 *   - array[0].position_end.lane_vertical_position.value = 30;
 *   - array[0].position_end.lane_id.value = 22223333;
 *   - array[0].position_end.dx_lane.value = 31;
 *   - array[0].position_end.dy_lane.value = 32;
 *   - array[0].position_end.dh_lane.value = 33;
 *   - array[0].position_end.semi_axis_length_major.value = 34;
 *   - array[0].position_end.semi_axis_length_minor.value = 35;
 *   - array[0].position_end.orientation.value = 36;
 *   - array[0].position_end.altitude_accuracy.value = 37;
 *   - array[0].length.value.value = 30;
 *   - array[0].length.accuracy.value = 38;
 *   - array[0].id_begin.value = 33334444;
 *   - array[0].id_end.value = 44445555;
 *   - array[0].information_source_list[0].value = 55556666;
 * 
 *   - array[1].id.value = 31415926536;
 *   - array[1].time.value = 2;
 *   - array[1].existency.value = 3;
 *   - array[1].minimal_detectable_size.value = 4;
 *   - array[1].position_begin.geodetic_srid.value = 4325;
 *   - array[1].position_begin.latitude.value = 365814684;
 *   - array[1].position_begin.longitude.value = 1366428182;
 *   - array[1].position_begin.altitude.value = 821;
 *   - array[1].position_begin.crp_id.value = 5;
 *   - array[1].position_begin.dx_crp.value = 6;
 *   - array[1].position_begin.dy_crp.value = 7;
 *   - array[1].position_begin.dh_crp.value = 8;
 *   - array[1].position_begin.lane_count.value = 9;
 *   - array[1].position_begin.lane_position.value = 10;
 *   - array[1].position_begin.lane_lateral_position.value = 11;
 *   - array[1].position_begin.crp_id_begin.value = 12;
 *   - array[1].position_begin.crp_id_end.value = 13;
 *   - array[1].position_begin.lane_vertical_position.value = 14;
 *   - array[1].position_begin.lane_id.value = 11112223;
 *   - array[1].position_begin.dx_lane.value = 15;
 *   - array[1].position_begin.dy_lane.value = 16;
 *   - array[1].position_begin.dh_lane.value = 17;
 *   - array[1].position_begin.semi_axis_length_major.value = 18;
 *   - array[1].position_begin.semi_axis_length_minor.value = 19;
 *   - array[1].position_begin.orientation.value = 20;
 *   - array[1].position_begin.altitude_accuracy.value = 21;
 *   - array[1].position_end.geodetic_srid.value = 4327;
 *   - array[1].position_end.latitude.value = 365815237;
 *   - array[1].position_end.longitude.value = 1366428677;
 *   - array[1].position_end.altitude.value = 821;
 *   - array[1].position_end.crp_id.value = 22;
 *   - array[1].position_end.dx_crp.value = 23;
 *   - array[1].position_end.dy_crp.value = 24;
 *   - array[1].position_end.dh_crp.value = 25;
 *   - array[1].position_end.lane_count.value = 26;
 *   - array[1].position_end.lane_position.value = 27;
 *   - array[1].position_end.lane_lateral_position.value = 28;
 *   - array[1].position_end.crp_id_begin.value = 29;
 *   - array[1].position_end.crp_id_end.value = 30;
 *   - array[1].position_end.lane_vertical_position.value = 31;
 *   - array[1].position_end.lane_id.value = 22223334;
 *   - array[1].position_end.dx_lane.value = 32;
 *   - array[1].position_end.dy_lane.value = 33;
 *   - array[1].position_end.dh_lane.value = 34;
 *   - array[1].position_end.semi_axis_length_major.value = 35;
 *   - array[1].position_end.semi_axis_length_minor.value = 36;
 *   - array[1].position_end.orientation.value = 37;
 *   - array[1].position_end.altitude_accuracy.value = 38;
 *   - array[1].length.value.value = 31;
 *   - array[1].length.accuracy.value = 39;
 *   - array[1].id_begin.value = 33334445;
 *   - array[1].id_end.value = 44445556;
 *   - array[0].information_source_list[0].value = 55556667;
 * 
 *   - timestamp = 0;
 * 
 * - 期待する返却値: result:
 *  -  result.at(0).at(0) == "0000031415926535"
 *  -  result.at(0).at(1) == 1
 *  -  result.at(0).at(2) == 2
 *  -  result.at(0).at(3) == 3
 *  -  result.at(0).at(4) == 4326
 *  -  result.at(0).at(5) == 365814685
 *  -  result.at(0).at(6) == 1366428183
 *  -  result.at(0).at(7) == 820
 *  -  result.at(0).at(8) == 4
 *  -  result.at(0).at(9) == 5
 *  -  result.at(0).at(10) == 6
 *  -  result.at(0).at(11) == 7
 *  -  result.at(0).at(12) == 8
 *  -  result.at(0).at(13) == 9
 *  -  result.at(0).at(14) == 10
 *  -  result.at(0).at(15) == 11
 *  -  result.at(0).at(16) == 12
 *  -  result.at(0).at(17) == 13
 *  -  result.at(0).at(18) == "0000000011112222"
 *  -  result.at(0).at(19) == 14
 *  -  result.at(0).at(20) == 15
 *  -  result.at(0).at(21) == 16
 *  -  result.at(0).at(22) == 17
 *  -  result.at(0).at(23) == 18
 *  -  result.at(0).at(24) == 19
 *  -  result.at(0).at(25) == 20
 *  -  result.at(0).at(26) == 4326
 *  -  result.at(0).at(27) == 365815238
 *  -  result.at(0).at(28) == 1366428678
 *  -  result.at(0).at(29) == 820
 *  -  result.at(0).at(30) == 21
 *  -  result.at(0).at(31) == 22
 *  -  result.at(0).at(32) == 23
 *  -  result.at(0).at(33) == 24
 *  -  result.at(0).at(34) == 25
 *  -  result.at(0).at(35) == 26
 *  -  result.at(0).at(36) == 27
 *  -  result.at(0).at(37) == 28
 *  -  result.at(0).at(38) == 29
 *  -  result.at(0).at(39) == 30
 *  -  result.at(0).at(40) == "0000000022223333"
 *  -  result.at(0).at(41) == 31
 *  -  result.at(0).at(42) == 32
 *  -  result.at(0).at(43) == 33
 *  -  result.at(0).at(44) == 34
 *  -  result.at(0).at(45) == 35
 *  -  result.at(0).at(46) == 36
 *  -  result.at(0).at(47) == 37
 *  -  result.at(0).at(48) == 30
 *  -  result.at(0).at(49) == 38
 *  -  result.at(0).at(50) == "0000000033334444"
 *  -  result.at(0).at(51) == "0000000044445555"
 *  -  result.at(0).at(52).at(0) == "0000000055556666"
 * 
 *  -  result.at(1).at(0) == "0000031415926536"
 *  -  result.at(1).at(1) == 2
 *  -  result.at(1).at(2) == 3
 *  -  result.at(1).at(3) == 4
 *  -  result.at(1).at(4) == 4325
 *  -  result.at(1).at(5) == 365814684
 *  -  result.at(1).at(6) == 1366428182
 *  -  result.at(1).at(7) == 821
 *  -  result.at(1).at(8) == 5
 *  -  result.at(1).at(9) == 6
 *  -  result.at(1).at(10) == 7
 *  -  result.at(1).at(11) == 8
 *  -  result.at(1).at(12) == 9
 *  -  result.at(1).at(13) == 10
 *  -  result.at(1).at(14) == 11
 *  -  result.at(1).at(15) == 12
 *  -  result.at(1).at(16) == 13
 *  -  result.at(1).at(17) == 14
 *  -  result.at(1).at(18) == "0000000011112223"
 *  -  result.at(1).at(19) == 15
 *  -  result.at(1).at(20) == 16
 *  -  result.at(1).at(21) == 17
 *  -  result.at(1).at(22) == 18
 *  -  result.at(1).at(23) == 19
 *  -  result.at(1).at(24) == 20
 *  -  result.at(1).at(25) == 21
 *  -  result.at(1).at(26) == 4327
 *  -  result.at(1).at(27) == 365815237
 *  -  result.at(1).at(28) == 1366428677
 *  -  result.at(1).at(29) == 821
 *  -  result.at(1).at(30) == 22
 *  -  result.at(1).at(31) == 23
 *  -  result.at(1).at(32) == 24
 *  -  result.at(1).at(33) == 25
 *  -  result.at(1).at(34) == 26
 *  -  result.at(1).at(35) == 27
 *  -  result.at(1).at(36) == 28
 *  -  result.at(1).at(37) == 29
 *  -  result.at(1).at(38) == 30
 *  -  result.at(1).at(39) == 31
 *  -  result.at(1).at(40) == "0000000022223334"
 *  -  result.at(1).at(41) == 32
 *  -  result.at(1).at(42) == 33
 *  -  result.at(1).at(43) == 34
 *  -  result.at(1).at(44) == 35
 *  -  result.at(1).at(45) == 36
 *  -  result.at(1).at(46) == 37
 *  -  result.at(1).at(47) == 38
 *  -  result.at(1).at(48) == 31
 *  -  result.at(1).at(49) == 39
 *  -  result.at(1).at(50) == "0000000033334445"
 *  -  result.at(1).at(51) == "0000000044445556"
 *  -  result.at(1).at(52).at(0) == "0000000055556667"
  
 */

TEST(TestConverterFreespaceInfo, RosToTupleNormal) {
  // setup
  FreespaceInfoArray test_in = test_util::CreateFreespaceInfoArray();

  

  // exercise
  auto test_out = Converter<FreespaceInfoArray>::RosToTuple(test_in, 0);

  // verify
  std::vector<any> val1;
  std::vector<long> timestamp1;
  std::vector<any> val2;
  std::vector<long> timestamp2;
  const int data_nums = 92;
  val1.resize(data_nums);
  timestamp1.resize(data_nums);
  val2.resize(data_nums);
  timestamp2.resize(data_nums);

  for (int i = 0; i < data_nums; i++) {
    test_out.at(0).getValue(i, val1.at(i), timestamp1.at(i));
    test_out.at(1).getValue(i, val2.at(i), timestamp2.at(i));
  }

  for (int i = 0; i < data_nums; i++) {
    test_util::ExpectValueEq(0, timestamp1.at(i));
    test_util::ExpectValueEq(0, timestamp2.at(i));
  }

   int idx = -1;
  test_util::ExpectValueEq(dm_any::any_cast<unsigned long long>(val1.at(++idx)), 31415926535);
  test_util::ExpectValueEq(dm_any::any_cast<long>(val1.at(++idx)), 1);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 0);  // revision
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 0, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 0, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 0, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 900000001, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 1800000001, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 800001, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 0, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 0, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 0, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 800001, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 0, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), -132768, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), -132768, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), -132768, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 15, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 17, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 101, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 0, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 0, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 10001, idx);
  test_util::ExpectValueEq(dm_any::any_cast<unsigned long long>(val1.at(++idx)), 0, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), -132768, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), -132768, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), -132768, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 4095, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 4095, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 28800, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 20001, idx);
  test_util::ExpectValueEq(dm_any::any_cast<std::vector<int>>(val1.at(++idx)).size(), 15, idx);
  test_util::ExpectValueEq(dm_any::any_cast<std::vector<int>>(val1.at(++idx)).size(), 15, idx);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 4326);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 365814685);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 1366428183);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 820);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 0);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 0);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 0);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 800001);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 4);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 5);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 6);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 7);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 8);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 9);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 10);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 11);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 12);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 13);
  test_util::ExpectValueEq(dm_any::any_cast<unsigned long long>(val1.at(++idx)), 11112222);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 14);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 15);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 16);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 17);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 18);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 19);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 20);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 4326);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 365815238);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 1366428678);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 820);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 0);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 0);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 0);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 800001);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 21);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 22);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 23);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 24);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 25);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 26);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 27);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 28);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 29);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 30);
  test_util::ExpectValueEq(dm_any::any_cast<unsigned long long>(val1.at(++idx)), 22223333);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 31);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 32);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 33);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 34);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 35);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 36);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 37);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 30);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 38);
  test_util::ExpectValueEq(dm_any::any_cast<unsigned long long>(val1.at(++idx)), 33334444);
  test_util::ExpectValueEq(dm_any::any_cast<unsigned long long>(val1.at(++idx)), 44445555);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 2);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(++idx)), 3);
  test_util::ExpectValueEq(dm_any::any_cast<std::vector<unsigned long long>>(val1.at(++idx)).at(0),
            55556666);

//test_util::ExpectValueEq(dm_any::any_cast<string>(val2.at(0)), "0000031415926536");
//test_util::ExpectValueEq(dm_any::any_cast<long>(val2.at(1)), 2);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(2)), 3);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(3)), 4);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(4)), 4325);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(5)), 365814684);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(6)), 1366428182);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(7)), 821);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(8)), 5);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(9)), 6);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(10)), 7);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(11)), 8);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(12)), 9);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(13)), 10);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(14)), 11);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(15)), 12);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(16)), 13);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(17)), 14);
//test_util::ExpectValueEq(dm_any::any_cast<string>(val2.at(18)), "0000000011112223");
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(19)), 15);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(20)), 16);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(21)), 17);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(22)), 18);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(23)), 19);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(24)), 20);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(25)), 21);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(26)), 4327);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(27)), 365815237);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(28)), 1366428677);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(29)), 821);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(30)), 22);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(31)), 23);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(32)), 24);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(33)), 25);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(34)), 26);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(35)), 27);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(36)), 28);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(37)), 29);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(38)), 30);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(39)), 31);
//test_util::ExpectValueEq(dm_any::any_cast<string>(val2.at(40)), "0000000022223334");
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(41)), 32);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(42)), 33);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(43)), 34);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(44)), 35);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(45)), 36);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(46)), 37);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(47)), 38);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(48)), 31);
//test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(49)), 39);
//test_util::ExpectValueEq(dm_any::any_cast<string>(val2.at(50)), "0000000033334445");
//test_util::ExpectValueEq(dm_any::any_cast<string>(val2.at(51)), "0000000044445556");
//test_util::ExpectValueEq(dm_any::any_cast<std::vector<string>>(val2.at(52)).at(0),
//          "0000000055556667");
  // teardown
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

}  // namespace test
}  // namespace ros2_dmi
