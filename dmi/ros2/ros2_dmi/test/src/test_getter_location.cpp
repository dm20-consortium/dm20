#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <chrono>

#include <is/Tuple.h>
#include <is/TupleSet.h>

#include "ros2_dmi/converter_util.hpp"
#include "resultset_location.hpp"
#include "any_compat.hpp"

namespace ros2_dmi {
namespace test {

/**
 * @brief ResultSetをFreespaceInfoArray型のROSメッセージに変換することを検証する
 */
TEST(TestGetterLocation, RsToRosNormal) {
  // setup
  test_util::CandidateTestCase_location data = test_util::CandidateTestCase_location();
  ResultSet test_in = test_util::CreateResultSet_location(data);
  test_in.next();

  // exercise
  const auto test_out = Getter<Location>::ResultSetToRos(test_in, "test_");

  // verify
  EXPECT_EQ(test_out.geodetic_srid.value, 456U);
  EXPECT_EQ(test_out.latitude.value, 789);
  EXPECT_EQ(test_out.longitude.value, 3322);
  EXPECT_EQ(test_out.altitude.value, 125);
  EXPECT_EQ(test_out.projective_srid.value, 1523U);
  EXPECT_EQ(test_out.prc_x, 1633);
  EXPECT_EQ(test_out.prc_y, 1454);
  EXPECT_EQ(test_out.prc_z.value, 1963);
  EXPECT_EQ(test_out.crp_id.value, 523U);
  EXPECT_EQ(test_out.dx_crp.value, 633);
  EXPECT_EQ(test_out.dy_crp.value, 454);
  EXPECT_EQ(test_out.dh_crp.value, 963);
  EXPECT_EQ(test_out.lane_count.value, 84U);
  EXPECT_EQ(test_out.lane_position.value, -27);
  EXPECT_EQ(test_out.lane_lateral_position.value, 159U);
  EXPECT_EQ(test_out.crp_id_begin.value, 753U);
  EXPECT_EQ(test_out.crp_id_end.value, 486U);
  EXPECT_EQ(test_out.lane_vertical_position.value, 426U);
  EXPECT_EQ(test_out.lane_id.value, 6314);
  EXPECT_EQ(test_out.dx_lane.value, 842);
  EXPECT_EQ(test_out.dy_lane.value, 862);
  EXPECT_EQ(test_out.dh_lane.value, 9852);
  EXPECT_EQ(test_out.semi_axis_length_major.value, 336U);
  EXPECT_EQ(test_out.semi_axis_length_minor.value, 4567U);
  EXPECT_EQ(test_out.orientation.value, 7654U);
  EXPECT_EQ(test_out.altitude_accuracy.value, 987U);

  // teardown
}

/**
 * @brief FreespaceInfoArray型のROSメッセージからTuple変換することを検証する
 */
TEST(TestGetterLocation, RosToTupleNormal) {
  // setup
  Location test_in;
  test_in.geodetic_srid.value = 456;
  test_in.latitude.value = 789;
  test_in.longitude.value = 3322;
  test_in.altitude.value = 125;
  test_in.projective_srid.value = 1523;
  test_in.prc_x = 1633;
  test_in.prc_y = 1454;
  test_in.prc_z.value = 1963;
  test_in.crp_id.value = 523;
  test_in.dx_crp.value = 633;
  test_in.dy_crp.value = 454;
  test_in.dh_crp.value = 963;
  test_in.lane_count.value = 84;
  test_in.lane_position.value = -27;
  test_in.lane_lateral_position.value = 159;
  test_in.crp_id_begin.value = 753;
  test_in.crp_id_end.value = 486;
  test_in.lane_vertical_position.value = 426;
  test_in.lane_id.value = 6314;
  test_in.dx_lane.value = 842;
  test_in.dy_lane.value = 862;
  test_in.dh_lane.value = 9852;
  test_in.semi_axis_length_major.value = 336;
  test_in.semi_axis_length_minor.value = 4567;
  test_in.orientation.value = 7654;
  test_in.altitude_accuracy.value = 987;

  const int data_nums = 26;
  IS::Tuple test_out(data_nums);

  // exercise
  int dummy = 0;
  Getter<Location>::RosToTuple(test_in, test_out, 0, dummy);

  // verify
  std::vector<any> val;
  std::vector<long> timestamp;
  val.resize(data_nums);
  timestamp.resize(data_nums);

  for (int i = 0; i < data_nums; i++) {
    test_out.getValue(i, val.at(i), timestamp.at(i));
  }

  for (int i = 0; i < data_nums; i++) {
    EXPECT_EQ(0, timestamp.at(i));
  }

  EXPECT_EQ(dm_any::any_cast<int>(val.at(0)), 456);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(1)), 789);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(2)), 3322);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(3)), 125);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(4)), 1523);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(5)), 1633);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(6)), 1454);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(7)), 1963);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(8)), 523);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(9)), 633);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(10)), 454);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(11)), 963);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(12)), 84);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(13)), -27);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(14)), 159);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(15)), 753);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(16)), 486);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(17)), 426);
  EXPECT_EQ(dm_any::any_cast<unsigned long long>(val.at(18)), 6314);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(19)), 842);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(20)), 862);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(21)), 9852);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(22)), 336);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(23)), 4567);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(24)), 7654);
  EXPECT_EQ(dm_any::any_cast<int>(val.at(25)), 987);

  // teardown
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

}  // namespace test
}  // namespace ros2_dmi
