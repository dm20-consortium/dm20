#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <chrono>

#include <is/Tuple.h>
#include <is/TupleSet.h>

#include "ros2_dmi/converter_util.hpp"
#include "resultset_objectid.hpp"
#include "any_compat.hpp"


namespace ros2_dmi {
namespace test {

/**
 * @brief ResultSetをFreespaceInfoArray型のROSメッセージに変換することを検証する
 *
 * - 入力値: 
 *  - map["information_source_list"] = {"[13579,24680],666 888"};
 *  - length = 1;
 *
 * - 期待する返却値: result
 *  - result.at(0).value = 0x13579UL
 *  - result.at(1).value = 0x24680UL
 *  - result.at(2).value = 0x666888UL
 */
TEST(TestGetterObjectId, RsToRosNormal) {
  // setup
  test_util::CandidateTestCase_objectid data = test_util::CandidateTestCase_objectid();
  ResultSet test_in = test_util::CreateResultSet_objectid(data);
  test_in.next();

  // exercise
  const auto test_out = Getter<std::vector<ObjectId>>::ResultSetToRos(test_in);

  // verify
  EXPECT_EQ(test_out.at(0).value, 13579);
  EXPECT_EQ(test_out.at(1).value, 24680);

  // teardown
}

/**
 * @brief FreespaceInfoArray型のROSメッセージからTuple変換することを検証する
 * 
 * - 入力値: Ros2メッセージ: input
 *  - input.at(0).value = 13579UL
 *  - input.at(1).value = 24680UL
 * 
 * - 
 * - 期待する返却値: result:
 * - result.at(0) = "13579"
 * - result.at(1) = "24680"
}
 */
TEST(TestGetterObjectId, RosToTuple) {
  // setup
  std::vector<ObjectId> test_in;
  ObjectId data1;
  ObjectId data2;
  data1.value = 13579;
  data2.value = 24680;
  test_in.push_back(data1);
  test_in.push_back(data2);

  const int data_nums = 2;
  IS::Tuple test_out(data_nums);

// exercise
  int dummy = 0;
  Getter<std::vector<ObjectId>>::RosToTuple(test_in, test_out, 0, dummy);

// verify
  any val;
  long timestamp;
  test_out.getValue(0, val, timestamp);

  EXPECT_EQ(dm_any::any_cast<std::vector<unsigned long long>>(val).at(0), 13579);
  EXPECT_EQ(dm_any::any_cast<std::vector<unsigned long long>>(val).at(1), 24680);
}



int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

}  // namespace test
}  // namespace ros2_dmi
