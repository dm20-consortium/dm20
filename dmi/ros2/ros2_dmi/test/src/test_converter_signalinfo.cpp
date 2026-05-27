#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <chrono>

#include <is/Tuple.h>
#include <is/TupleSet.h>

#include "ros2_dmi/signal_info_converter.hpp"
#include "resultset_signal_info.hpp"
#include "rosmsg_signal_info.hpp"
#include "any_compat.hpp"
#include "test_optional_helper.hpp"


namespace ros2_dmi {
namespace test {

/**
 * @brief ResultSetをFreespaceInfoArray型のROSメッセージに変換することを検証する
 *
 * - 入力値: 
 *  - map["crp_id"] = {"123", "124"};
 *  - map["id"] = {"[200,201]", "[202,203]"};
 *  - map["time"] = {"789", "790"};
 *  - map["state"] = {"110", "111"};
 *  - map["specific_control_flags"] = {"120", "121"};
 *  - map["event_count"] = {"130", "131"};
 *  - map["count_down_stop_flag"] = {"219", "220"};
 *  - map["light_info_main_0"] = {"142", "143"};
 *  - map["light_info_arrow_0"] = {"65", "66"};
 *  - map["light_info_min_time_to_change_0"] = {"741", "742"};
 *  - map["light_info_max_time_to_change_0"] = {"852", "853"};
 *  - map["light_info_main_1"] = {"195", "196"};
 *  - map["light_info_arrow_1"] = {"147", "148"};
 *  - map["light_info_min_time_to_change_1"] = {"258", "259"};
 *  - map["light_info_max_time_to_change_1"] = {"369", "370"};
 *  - map["light_info_main_2"] = {"159", "160"};
 *  - map["light_info_arrow_2"] = {"241", "242"};
 *  - map["light_info_min_time_to_change_2"] = {"963", "964"};
 *  - map["light_info_max_time_to_change_2"] = {"852", "853"};
 *  - map["light_info_main_3"] = {"229", "230"};
 *  - map["light_info_arrow_3"] = {"189", "190"};
 *  - map["light_info_min_time_to_change_3"] = {"4546", "4547"};
 *  - map["light_info_max_time_to_change_3"] = {"7879", "7880"};
 *  - map["light_info_main_4"] = {"25", "26"};
 *  - map["light_info_arrow_4"] = {"133", "134"};
 *  - map["light_info_min_time_to_change_4"] = {"9876", "9877"};
 *  - map["light_info_max_time_to_change_4"] = {"4321", "4322"};
 *  - map["light_info_main_5"] = {"56", "57"};
 *  - map["light_info_arrow_5"] = {"153", "154"};
 *  - map["light_info_min_time_to_change_5"] = {"486", "487"};
 *  - map["light_info_max_time_to_change_5"] = {"9632", "9633"};
 *  - map["light_info_main_6"] = {"73", "74"};
 *  - map["light_info_arrow_6"] = {"245", "246"};
 *  - map["light_info_min_time_to_change_6"] = {"5632", "5633"};
 *  - map["light_info_max_time_to_change_6"] = {"8965", "8966"};
 *  - map["light_info_main_7"] = {"174", "175"};
 *  - map["light_info_arrow_7"] = {"169", "170"};
 *  - map["light_info_min_time_to_change_7"] = {"1379", "1380"};
 *  - map["light_info_max_time_to_change_7"] = {"3791", "3792"};
 *  - map["light_info_main_8"] = {"233", "234"};
 *  - map["light_info_arrow_8"] = {"177", "178"};
 *  - map["light_info_min_time_to_change_8"] = {"4862", "4863"};
 *  - map["light_info_max_time_to_change_8"] = {"8624", "8625"};
 *  - map["light_info_main_9"] = {"104", "105"};
 *  - map["light_info_arrow_9"] = {"182", "183"};
 *  - map["light_info_min_time_to_change_9"] = {"6539", "6540"};
 *  - map["light_info_max_time_to_change_9"] = {"8579", "8580"};
 *  - map["light_info_main_10"] = {"27", "28"};
 *  - map["light_info_arrow_10"] = {"3", "4"};
 *  - map["light_info_min_time_to_change_10"] = {"6135", "6136"};
 *  - map["light_info_max_time_to_change_10"] = {"4321", "4322"};
 *  - map["light_info_main_11"] = {"154", "155"};
 *  - map["light_info_arrow_11"] = {"203", "204"};
 *  - map["light_info_min_time_to_change_11"] = {"12365", "12366"};
 *  - map["light_info_max_time_to_change_11"] = {"18589", "18590"};
 *  - length = 2;
 *
 * - 期待する返却値:
 *  - crp_id.value = 123U
 *  - signal_id_list[0].value = 200U
 *  - signal_id_list[1].value = 201U
 *  - time.value = 789UL
 *  - state.value = 110
 *  - specific_control_flags.value = 120
 *  - event_count.value = 130
 *  - count_down_stop_flag.value = 219
 *  - signal_light_info_list[0].main_light.value = 142
 *  - signal_light_info_list[0].arrow_light.value = 65
 *  - signal_light_info_list[0].min_time_to_change.value = 741
 *  - signal_light_info_list[0].max_time_to_change.value = 852
 *  - signal_light_info_list[1].main_light.value = 195
 *  - signal_light_info_list[1].arrow_light.value = 147
 *  - signal_light_info_list[1].min_time_to_change.value = 258
 *  - signal_light_info_list[1].max_time_to_change.value = 369
 *  - signal_light_info_list[2].main_light.value = 159
 *  - signal_light_info_list[2].arrow_light.value = 241
 *  - signal_light_info_list[2].min_time_to_change.value = 963
 *  - signal_light_info_list[2].max_time_to_change.value = 852
 *  - signal_light_info_list[3].main_light.value = 229
 *  - signal_light_info_list[3].arrow_light.value = 189
 *  - signal_light_info_list[3].min_time_to_change.value = 4546
 *  - signal_light_info_list[3].max_time_to_change.value = 7879
 *  - signal_light_info_list[4].main_light.value = 25
 *  - signal_light_info_list[4].arrow_light.value = 133
 *  - signal_light_info_list[4].min_time_to_change.value = 9876
 *  - signal_light_info_list[4].max_time_to_change.value = 4321
 *  - signal_light_info_list[5].main_light.value = 56
 *  - signal_light_info_list[5].arrow_light.value = 153
 *  - signal_light_info_list[5].min_time_to_change.value = 486
 *  - signal_light_info_list[5].max_time_to_change.value = 9632
 *  - signal_light_info_list[6].main_light.value = 73
 *  - signal_light_info_list[6].arrow_light.value = 245
 *  - signal_light_info_list[6].min_time_to_change.value = 5632
 *  - signal_light_info_list[6].max_time_to_change.value = 8965
 *  - signal_light_info_list[7].main_light.value = 174
 *  - signal_light_info_list[7].arrow_light.value = 169
 *  - signal_light_info_list[7].min_time_to_change.value = 1379
 *  - signal_light_info_list[7].max_time_to_change.value = 3791
 *  - signal_light_info_list[8].main_light.value = 233
 *  - signal_light_info_list[8].arrow_light.value = 177
 *  - signal_light_info_list[8].min_time_to_change.value = 4862
 *  - signal_light_info_list[8].max_time_to_change.value = 8624
 *  - signal_light_info_list[9].main_light.value = 104
 *  - signal_light_info_list[9].arrow_light.value = 182
 *  - signal_light_info_list[9].min_time_to_change.value = 6539
 *  - signal_light_info_list[9].max_time_to_change.value = 8579
 *  - signal_light_info_list[10].main_light.value = 27
 *  - signal_light_info_list[10].arrow_light.value = 3
 *  - signal_light_info_list[10].min_time_to_change.value = 6135
 *  - signal_light_info_list[10].max_time_to_change.value = 4321
 *  - signal_light_info_list[11].main_light.value = 154
 *  - signal_light_info_list[11].arrow_light.value = 203
 *  - signal_light_info_list[11].min_time_to_change.value = 12365
 *  - signal_light_info_list[11].max_time_to_change.value = 18589
 */

TEST(TestConverterSignalInfo, RsToRosNomal) {
  // setup
  test_util::CandidateTestCase_s data = test_util::CandidateTestCase_s();
  ResultSet test_in = test_util::CreateResultSet_s(data);

  // exercise
  const auto test_out = Converter<SignalInfo>::ResultSetToRos(test_in).get();

  test_util::ExpectValueEq(test_out.crp_id.value, 123U);
  test_util::ExpectValueEq(test_out.signal_id_list[0].value, 200U);
  test_util::ExpectValueEq(test_out.signal_id_list[1].value, 201U);
  test_util::ExpectValueEq(test_out.time.value, 789UL);
  test_util::ExpectValueEq(test_out.state.value, 110);
  test_util::ExpectValueEq(test_out.specific_control_flags.value, 120);
  test_util::ExpectValueEq(test_out.event_count.value, 130);
  test_util::ExpectValueEq(test_out.count_down_stop_flag.value, 219);
  test_util::ExpectValueEq(test_out.signal_light_info_list[0].main_light.value, 142);
  test_util::ExpectValueEq(test_out.signal_light_info_list[0].arrow_light.value, 65);
  test_util::ExpectValueEq(test_out.signal_light_info_list[0].min_time_to_change.value, 741);
  test_util::ExpectValueEq(test_out.signal_light_info_list[0].max_time_to_change.value, 852);
  test_util::ExpectValueEq(test_out.signal_light_info_list[1].main_light.value, 195);
  test_util::ExpectValueEq(test_out.signal_light_info_list[1].arrow_light.value, 147);
  test_util::ExpectValueEq(test_out.signal_light_info_list[1].min_time_to_change.value, 258);
  test_util::ExpectValueEq(test_out.signal_light_info_list[1].max_time_to_change.value, 369);
  test_util::ExpectValueEq(test_out.signal_light_info_list[2].main_light.value, 159);
  test_util::ExpectValueEq(test_out.signal_light_info_list[2].arrow_light.value, 241);
  test_util::ExpectValueEq(test_out.signal_light_info_list[2].min_time_to_change.value, 963);
  test_util::ExpectValueEq(test_out.signal_light_info_list[2].max_time_to_change.value, 852);
  test_util::ExpectValueEq(test_out.signal_light_info_list[3].main_light.value, 229);
  test_util::ExpectValueEq(test_out.signal_light_info_list[3].arrow_light.value, 189);
  test_util::ExpectValueEq(test_out.signal_light_info_list[3].min_time_to_change.value, 4546);
  test_util::ExpectValueEq(test_out.signal_light_info_list[3].max_time_to_change.value, 7879);
  test_util::ExpectValueEq(test_out.signal_light_info_list[4].main_light.value, 25);
  test_util::ExpectValueEq(test_out.signal_light_info_list[4].arrow_light.value, 133);
  test_util::ExpectValueEq(test_out.signal_light_info_list[4].min_time_to_change.value, 9876);
  test_util::ExpectValueEq(test_out.signal_light_info_list[4].max_time_to_change.value, 4321);
  test_util::ExpectValueEq(test_out.signal_light_info_list[5].main_light.value, 56);
  test_util::ExpectValueEq(test_out.signal_light_info_list[5].arrow_light.value, 153);
  test_util::ExpectValueEq(test_out.signal_light_info_list[5].min_time_to_change.value, 486);
  test_util::ExpectValueEq(test_out.signal_light_info_list[5].max_time_to_change.value, 9632);
  test_util::ExpectValueEq(test_out.signal_light_info_list[6].main_light.value, 73);
  test_util::ExpectValueEq(test_out.signal_light_info_list[6].arrow_light.value, 245);
  test_util::ExpectValueEq(test_out.signal_light_info_list[6].min_time_to_change.value, 5632);
  test_util::ExpectValueEq(test_out.signal_light_info_list[6].max_time_to_change.value, 8965);
  test_util::ExpectValueEq(test_out.signal_light_info_list[7].main_light.value, 174);
  test_util::ExpectValueEq(test_out.signal_light_info_list[7].arrow_light.value, 169);
  test_util::ExpectValueEq(test_out.signal_light_info_list[7].min_time_to_change.value, 1379);
  test_util::ExpectValueEq(test_out.signal_light_info_list[7].max_time_to_change.value, 3791);
  test_util::ExpectValueEq(test_out.signal_light_info_list[8].main_light.value, 233);
  test_util::ExpectValueEq(test_out.signal_light_info_list[8].arrow_light.value, 177);
  test_util::ExpectValueEq(test_out.signal_light_info_list[8].min_time_to_change.value, 4862);
  test_util::ExpectValueEq(test_out.signal_light_info_list[8].max_time_to_change.value, 8624);
  test_util::ExpectValueEq(test_out.signal_light_info_list[9].main_light.value, 104);
  test_util::ExpectValueEq(test_out.signal_light_info_list[9].arrow_light.value, 182);
  test_util::ExpectValueEq(test_out.signal_light_info_list[9].min_time_to_change.value, 6539);
  test_util::ExpectValueEq(test_out.signal_light_info_list[9].max_time_to_change.value, 8579);
  test_util::ExpectValueEq(test_out.signal_light_info_list[10].main_light.value, 27);
  test_util::ExpectValueEq(test_out.signal_light_info_list[10].arrow_light.value, 3);
  test_util::ExpectValueEq(test_out.signal_light_info_list[10].min_time_to_change.value, 6135);
  test_util::ExpectValueEq(test_out.signal_light_info_list[10].max_time_to_change.value, 4321);
  test_util::ExpectValueEq(test_out.signal_light_info_list[11].main_light.value, 154);
  test_util::ExpectValueEq(test_out.signal_light_info_list[11].arrow_light.value, 203);
  test_util::ExpectValueEq(test_out.signal_light_info_list[11].min_time_to_change.value, 12365);
  test_util::ExpectValueEq(test_out.signal_light_info_list[11].max_time_to_change.value, 18589);
}

/**
 * @brief 不正な値を持つResultSetデータをFreespaceInfoArray型のROSメッセージに変換すると、
 * boost::noneが返却されることを検証する
 * 
 * - 入力値: idの値が"test"のResultSetデータ
 * 
 * - 期待する返却値:　boost::none
 */
TEST(TestConverterSignalInfo, RsToRosAbnomal) {
  // setup
  test_util::CandidateTestCase_s data = test_util::CandidateTestCase_s();
  data.Update_s("crp_id", {"test"});
  ResultSet test_in = test_util::CreateResultSet_s(data);

  // exercise
  const auto test_out = Converter<SignalInfo>::ResultSetToRos(test_in);

  // verify
  EXPECT_FALSE(test_out);

  // teardown
}

/**
 * @brief FreespaceInfoArray型のROSメッセージからTuple変換することを検証する
 * 
 * - 入力値: Ros2メッセージ:
 *  - crp_id.value = 123U
 *  - signal_id_list[0].value = 200U
 *  - signal_id_list[1].value = 201U
 *  - time.value = 789UL
 *  - state.value = 110
 *  - specific_control_flags.value = 120
 *  - event_count.value = 130
 *  - count_down_stop_flag.value = 219
 *  - signal_light_info_list[0].main_light.value = 142
 *  - signal_light_info_list[0].arrow_light.value = 65
 *  - signal_light_info_list[0].min_time_to_change.value = 741
 *  - signal_light_info_list[0].max_time_to_change.value = 852
 *  - signal_light_info_list[1].main_light.value = 195
 *  - signal_light_info_list[1].arrow_light.value = 147
 *  - signal_light_info_list[1].min_time_to_change.value = 258
 *  - signal_light_info_list[1].max_time_to_change.value = 369
 *  - signal_light_info_list[2].main_light.value = 159
 *  - signal_light_info_list[2].arrow_light.value = 241
 *  - signal_light_info_list[2].min_time_to_change.value = 963
 *  - signal_light_info_list[2].max_time_to_change.value = 852
 *  - signal_light_info_list[3].main_light.value = 229
 *  - signal_light_info_list[3].arrow_light.value = 189
 *  - signal_light_info_list[3].min_time_to_change.value = 4546
 *  - signal_light_info_list[3].max_time_to_change.value = 7879
 *  - signal_light_info_list[4].main_light.value = 25
 *  - signal_light_info_list[4].arrow_light.value = 133
 *  - signal_light_info_list[4].min_time_to_change.value = 9876
 *  - signal_light_info_list[4].max_time_to_change.value = 4321
 *  - signal_light_info_list[5].main_light.value = 56
 *  - signal_light_info_list[5].arrow_light.value = 153
 *  - signal_light_info_list[5].min_time_to_change.value = 486
 *  - signal_light_info_list[5].max_time_to_change.value = 9632
 *  - signal_light_info_list[6].main_light.value = 73
 *  - signal_light_info_list[6].arrow_light.value = 245
 *  - signal_light_info_list[6].min_time_to_change.value = 5632
 *  - signal_light_info_list[6].max_time_to_change.value = 8965
 *  - signal_light_info_list[7].main_light.value = 174
 *  - signal_light_info_list[7].arrow_light.value = 169
 *  - signal_light_info_list[7].min_time_to_change.value = 1379
 *  - signal_light_info_list[7].max_time_to_change.value = 3791
 *  - signal_light_info_list[8].main_light.value = 233
 *  - signal_light_info_list[8].arrow_light.value = 177
 *  - signal_light_info_list[8].min_time_to_change.value = 4862
 *  - signal_light_info_list[8].max_time_to_change.value = 8624
 *  - signal_light_info_list[9].main_light.value = 104
 *  - signal_light_info_list[9].arrow_light.value = 182
 *  - signal_light_info_list[9].min_time_to_change.value = 6539
 *  - signal_light_info_list[9].max_time_to_change.value = 8579
 *  - signal_light_info_list[10].main_light.value = 27
 *  - signal_light_info_list[10].arrow_light.value = 3
 *  - signal_light_info_list[10].min_time_to_change.value = 6135
 *  - signal_light_info_list[10].max_time_to_change.value = 4321
 *  - signal_light_info_list[11].main_light.value = 154
 *  - signal_light_info_list[11].arrow_light.value = 203
 *  - signal_light_info_list[11].min_time_to_change.value = 12365
 *  - signal_light_info_list[11].max_time_to_change.value = 18589
 * 
 * - 期待する返却値: result:
 *  - result.at(0) = 123
 *  - result.at(1).at(0) = 200
 *  - result.at(1).at(1) = 201
 *  - result.at(2) = 789
 *  - result.at(3) = 110
 *  - result.at(4) = 120
 *  - result.at(5) = 130
 *  - result.at(6) = 219
 *  - result.at(7) = 142
 *  - result.at(8) = 65
 *  - result.at(9) = 741
 *  - result.at(10) = 852
 *  - result.at(11) = 195
 *  - result.at(12) = 147
 *  - result.at(13) = 258
 *  - result.at(14) = 369
 *  - result.at(15) = 159
 *  - result.at(16) = 241
 *  - result.at(17) = 963
 *  - result.at(18) = 852
 *  - result.at(19) = 229
 *  - result.at(20) = 189
 *  - result.at(21) = 4546
 *  - result.at(22) = 7879
 *  - result.at(23) = 25
 *  - result.at(24) = 133
 *  - result.at(25) = 9876
 *  - result.at(26) = 4321
 *  - result.at(27) = 56
 *  - result.at(28) = 153
 *  - result.at(29) = 486
 *  - result.at(30) = 9632
 *  - result.at(31) = 73
 *  - result.at(32) = 245
 *  - result.at(33) = 5632
 *  - result.at(34) = 8965
 *  - result.at(35) = 174
 *  - result.at(36) = 169
 *  - result.at(37) = 1379
 *  - result.at(38) = 3791
 *  - result.at(39) = 233
 *  - result.at(40) = 177
 *  - result.at(41) = 4862
 *  - result.at(42) = 8624
 *  - result.at(43) = 104
 *  - result.at(44) = 182
 *  - result.at(45) = 6539
 *  - result.at(46) = 8579
 *  - result.at(47) = 27
 *  - result.at(48) = 3
 *  - result.at(49) = 6135
 *  - result.at(50) = 4321
 *  - result.at(51) = 154
 *  - result.at(52) = 203
 *  - result.at(53) = 12365
 *  - result.at(54) = 18589
 */
TEST(TestConverterSignalInfo, RosToTupleNormal) {
  // setup
  SignalInfoArray test_data = test_util::CreateSignalInfoArray();
  SignalInfo test_in = test_data.array[0];

  // exercise
  auto test_out_tuple = Converter<SignalInfo>::RosToTuple(test_in, 0);

  // verify
  std::vector<any> val;
  std::vector<long> timestamp;
  const int data_nums = 55;
  val.resize(data_nums);
  timestamp.resize(data_nums);

  for (int i = 0; i < data_nums; i++) {
    test_out_tuple.at(0).getValue(i, val.at(i), timestamp.at(i));
  }

  for (int i = 0; i < data_nums; i++) {
    test_util::ExpectValueEq(0, timestamp.at(i));
  }

  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(0)), 123);
  test_util::ExpectValueEq(dm_any::any_cast<std::vector<int>>(val.at(1)).at(0), 200);
  test_util::ExpectValueEq(dm_any::any_cast<std::vector<int>>(val.at(1)).at(1), 201);
  test_util::ExpectValueEq(dm_any::any_cast<long>(val.at(2)), 789);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(3)), 110);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(4)), 120);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(5)), 130);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(6)), 219);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(7)), 142);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(8)), 65);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(9)), 741);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(10)), 852);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(11)), 195);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(12)), 147);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(13)), 258);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(14)), 369);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(15)), 159);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(16)), 241);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(17)), 963);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(18)), 852);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(19)), 229);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(20)), 189);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(21)), 4546);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(22)), 7879);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(23)), 25);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(24)), 133);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(25)), 9876);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(26)), 4321);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(27)), 56);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(28)), 153);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(29)), 486);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(30)), 9632);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(31)), 73);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(32)), 245);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(33)), 5632);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(34)), 8965);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(35)), 174);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(36)), 169);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(37)), 1379);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(38)), 3791);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(39)), 233);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(40)), 177);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(41)), 4862);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(42)), 8624);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(43)), 104);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(44)), 182);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(45)), 6539);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(46)), 8579);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(47)), 27);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(48)), 3);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(49)), 6135);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(50)), 4321);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(51)), 154);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(52)), 203);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(53)), 12365);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val.at(54)), 18589);

  // teardown
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

}  // namespace test
}  // namespace ros2_dmi
