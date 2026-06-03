#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <chrono>

#include <is/Tuple.h>

#include "ros2_dmi/object_info_converter.hpp"
#include "resultset_object_info.hpp"
#include "rosmsg_object_info.hpp"
#include "any_compat.hpp"
#include "test_optional_helper.hpp"

namespace ros2_dmi {
namespace test {

/**
 * @brief ResultSetをObjectInfo型のROSメッセージに変換することを検証する
 *
 */

TEST(TestConverterObjectInfo, RsToRosNormal) {
  // setup
  test_util::CandidateTestCase_o data = test_util::CandidateTestCase_o();
  ResultSet test_in = test_util::CreateResultSet_o(data);
  

  // excercise
  const auto test_out = Converter<ObjectInfo>::ResultSetToRos(
      test_in).get();

  // verify
  test_util::ExpectValueEq(test_out.id.value, 12341234UL);
  test_util::ExpectValueEq(test_out.time.value, 5000000UL);
  test_util::ExpectValueEq(test_out.information_source_list[0].value, 56785678UL);

  // teardown
}

/**
 * @brief 不正な値を持つResultSetデータをObjectInfo型のROSメッセージに変換すると、
 * boost::noneが返却されることを検証する
 * 
 * - 入力値: idの値が"test"のResultSetデータ
 * 
 * - 期待する返却値:　boost::none
 */

TEST(TestConverterObjectInfo, RsToRosAbnormal) {
  // setup
  test_util::CandidateTestCase_o data = test_util::CandidateTestCase_o();
  data.Update_o("object_id", {"test"});
  ResultSet test_in = test_util::CreateResultSet_o(data);
  

  // exercise
  const auto test_out_ros2msg_abnormal =
      Converter<ObjectInfo>::ResultSetToRos(test_in);

  // verify
  EXPECT_FALSE(test_out_ros2msg_abnormal);

  // teardown
}

/**
 * @brief ObjectInfo型のROSメッセージからTuple変換することを検証する
 */

TEST(TestConverterObjectInfo, RosToTupleNormal) {
  // setup
  ObjectInfoArray test_data = test_util::CreateObjectInfoArray();
  const ObjectInfo test_in = test_data.array[0];
  

  // excercise
  auto out = Converter<ObjectInfo>::RosToTuple(
      test_in, 0)[0];  // test_out_tupleは変更しない, getValue()関数を利用するため、constに設定できない

  // verify
  any val;
  long timestamp = 0;
  out.getValue(0, val, timestamp);
  test_util::ExpectValueEq(305402420UL, dm_any::any_cast<unsigned long long>(val));

  out.getValue(1, val, timestamp);
  test_util::ExpectValueEq(5000000, dm_any::any_cast<long>(val));

  out.getValue(87, val, timestamp);
  std::vector<unsigned long long> val1 = dm_any::any_cast<std::vector<unsigned long long>>(val);
  test_util::ExpectValueEq(1450727032UL, val1[0]);

  // teardown
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

}  // namespace test
}  // namespace ros2_dmi
