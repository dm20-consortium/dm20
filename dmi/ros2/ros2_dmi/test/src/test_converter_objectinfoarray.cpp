#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <chrono>

#include <is/Tuple.h>

#include "ros2_dmi/object_info_converter.hpp"
#include "resultset_object_info.hpp"
#include "rosmsg_object_info.hpp"

namespace ros2_dmi {
namespace test {

/**
 * @brief ResultSetをObjectInfoArray型のROSメッセージに変換することを検証する
 */
TEST(TestConverterObjectInfo, RsToRosNormal) {
  // setup
  test_util::CandidateTestCase_o data = test_util::CandidateTestCase_o();
  ResultSet test_in = test_util::CreateResultSet_o(data);

  // exercise
  const auto test_out = Converter<ObjectInfoArray>::ResultSetToRos(test_in).get();

  // verify
  EXPECT_EQ(test_out.array[0].id.value, 12341234UL);
  EXPECT_EQ(test_out.array[0].time.value, 5000000UL);
  EXPECT_EQ(test_out.array[0].information_source_list[0].value, 56785678U);

  // teardown
}

/**
 * @brief 不正な値を持つResultSetデータをObjectInfoArray型のROSメッセージに変換すると、
 * boost::noneが返却されることを検証する
 * 
 * - 入力値: idの値が"test"のResultSetデータ
 * 
 * - 期待する返却値:　boost::none
 */

TEST(TestConverterObjectInfo, RsToRosAbnormal) {
  // setup
  test_util::CandidateTestCase_o data = test_util::CandidateTestCase_o();
  data.Update_o("object_id", {"test", "test2"});
  ResultSet test_in = test_util::CreateResultSet_o(data);

  // exercise
  const auto test_out_ros2msg_abnormal = Converter<ObjectInfoArray>::ResultSetToRos(test_in);

  // verify
  EXPECT_EQ(test_out_ros2msg_abnormal, boost::none);

  // teardown
}

/**
 * @brief ObjectInfoArray型のROSメッセージからTuple変換することを検証する
 * 
 */
TEST(TestConverterObjectInfo, RosToTupleNormal) {
  // setup
  ObjectInfoArray test_in = test_util::CreateObjectInfoArray();

  // exercise
  auto test_out_tuple = Converter<ObjectInfoArray>::RosToTuple(test_in, 0);

  // verify
  std::vector<any> val1;
  std::vector<long> timestamp1;
  std::vector<any> val2;
  std::vector<long> timestamp2;
  const int data_nums = 88;
  val1.resize(data_nums);
  timestamp1.resize(data_nums);
  val2.resize(data_nums);
  timestamp2.resize(data_nums);


  for (int i = 0; i < data_nums; i++) {
    test_out_tuple.at(0).getValue(i, val1.at(i), timestamp1.at(i));
    test_out_tuple.at(1).getValue(i, val2.at(i), timestamp2.at(i));
  }

  for (int i = 0; i < data_nums; i++) {
    EXPECT_EQ(0, timestamp1.at(i));
    EXPECT_EQ(0, timestamp2.at(i));
  }

  EXPECT_EQ(std::experimental::any_cast<unsigned long long>(val1.at(0)), 305402420);
  EXPECT_EQ(std::experimental::any_cast<long>(val1.at(1)), 5000000L);
  EXPECT_EQ(std::experimental::any_cast<std::vector<unsigned long long>>(val1.at(87)).at(0),
            1450727032UL);


  EXPECT_EQ(std::experimental::any_cast<unsigned long long>(val2.at(0)), 305402421);
  EXPECT_EQ(std::experimental::any_cast<long>(val2.at(1)), 5000001L);
  EXPECT_EQ(std::experimental::any_cast<std::vector<unsigned long long>>(val2.at(87)).at(0),
            1450727033UL);

  // teardown
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

}  // namespace test
}  // namespace ros2_dmi
