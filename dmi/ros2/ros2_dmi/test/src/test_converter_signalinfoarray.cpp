#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <chrono>

#include <is/Tuple.h>

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
 * - 期待する返却値: result
 *  - array[0].crp_id.value = 123U
 *  - array[0].signal_id_list[0].value = 200U
 *  - array[0].signal_id_list[1].value = 201U
 *  - array[0].time.value = 789UL
 *  - array[0].state.value = 110
 *  - array[0].specific_control_flags.value = 120
 *  - array[0].event_count.value = 130
 *  - array[0].count_down_stop_flag.value = 219
 *  - array[0].signal_light_info_list[0].main_light.value = 142
 *  - array[0].signal_light_info_list[0].arrow_light.value = 65
 *  - array[0].signal_light_info_list[0].min_time_to_change.value = 741
 *  - array[0].signal_light_info_list[0].max_time_to_change.value = 852
 *  - array[0].signal_light_info_list[1].main_light.value = 195
 *  - array[0].signal_light_info_list[1].arrow_light.value = 147
 *  - array[0].signal_light_info_list[1].min_time_to_change.value = 258
 *  - array[0].signal_light_info_list[1].max_time_to_change.value = 369
 *  - array[0].signal_light_info_list[2].main_light.value = 159
 *  - array[0].signal_light_info_list[2].arrow_light.value = 241
 *  - array[0].signal_light_info_list[2].min_time_to_change.value = 963
 *  - array[0].signal_light_info_list[2].max_time_to_change.value = 852
 *  - array[0].signal_light_info_list[3].main_light.value = 229
 *  - array[0].signal_light_info_list[3].arrow_light.value = 189
 *  - array[0].signal_light_info_list[3].min_time_to_change.value = 4546
 *  - array[0].signal_light_info_list[3].max_time_to_change.value = 7879
 *  - array[0].signal_light_info_list[4].main_light.value = 25
 *  - array[0].signal_light_info_list[4].arrow_light.value = 133
 *  - array[0].signal_light_info_list[4].min_time_to_change.value = 9876
 *  - array[0].signal_light_info_list[4].max_time_to_change.value = 4321
 *  - array[0].signal_light_info_list[5].main_light.value = 56
 *  - array[0].signal_light_info_list[5].arrow_light.value = 153
 *  - array[0].signal_light_info_list[5].min_time_to_change.value = 486
 *  - array[0].signal_light_info_list[5].max_time_to_change.value = 9632
 *  - array[0].signal_light_info_list[6].main_light.value = 73
 *  - array[0].signal_light_info_list[6].arrow_light.value = 245
 *  - array[0].signal_light_info_list[6].min_time_to_change.value = 5632
 *  - array[0].signal_light_info_list[6].max_time_to_change.value = 8965
 *  - array[0].signal_light_info_list[7].main_light.value = 174
 *  - array[0].signal_light_info_list[7].arrow_light.value = 169
 *  - array[0].signal_light_info_list[7].min_time_to_change.value = 1379
 *  - array[0].signal_light_info_list[7].max_time_to_change.value = 3791
 *  - array[0].signal_light_info_list[8].main_light.value = 233
 *  - array[0].signal_light_info_list[8].arrow_light.value = 177
 *  - array[0].signal_light_info_list[8].min_time_to_change.value = 4862
 *  - array[0].signal_light_info_list[8].max_time_to_change.value = 8624
 *  - array[0].signal_light_info_list[9].main_light.value = 104
 *  - array[0].signal_light_info_list[9].arrow_light.value = 182
 *  - array[0].signal_light_info_list[9].min_time_to_change.value = 6539
 *  - array[0].signal_light_info_list[9].max_time_to_change.value = 8579
 *  - array[0].signal_light_info_list[10].main_light.value = 27
 *  - array[0].signal_light_info_list[10].arrow_light.value = 3
 *  - array[0].signal_light_info_list[10].min_time_to_change.value = 6135
 *  - array[0].signal_light_info_list[10].max_time_to_change.value = 4321
 *  - array[0].signal_light_info_list[11].main_light.value = 154
 *  - array[0].signal_light_info_list[11].arrow_light.value = 203
 *  - array[0].signal_light_info_list[11].min_time_to_change.value = 12365
 *  - array[0].signal_light_info_list[11].max_time_to_change.value = 18589
 * 
 *  - array[1].crp_id.value = 124U
 *  - array[1].signal_id_list[0].value = 202U
 *  - array[1].signal_id_list[1].value = 203U
 *  - array[1].time.value = 790UL
 *  - array[1].state.value = 111
 *  - array[1].specific_control_flags.value = 121
 *  - array[1].event_count.value = 131
 *  - array[1].count_down_stop_flag.value = 220
 *  - array[1].signal_light_info_list[0].main_light.value = 143
 *  - array[1].signal_light_info_list[0].arrow_light.value = 66
 *  - array[1].signal_light_info_list[0].min_time_to_change.value = 742
 *  - array[1].signal_light_info_list[0].max_time_to_change.value = 853
 *  - array[1].signal_light_info_list[1].main_light.value = 196
 *  - array[1].signal_light_info_list[1].arrow_light.value = 148
 *  - array[1].signal_light_info_list[1].min_time_to_change.value = 259
 *  - array[1].signal_light_info_list[1].max_time_to_change.value = 370
 *  - array[1].signal_light_info_list[2].main_light.value = 160
 *  - array[1].signal_light_info_list[2].arrow_light.value = 242
 *  - array[1].signal_light_info_list[2].min_time_to_change.value = 964
 *  - array[1].signal_light_info_list[2].max_time_to_change.value = 853
 *  - array[1].signal_light_info_list[3].main_light.value = 230
 *  - array[1].signal_light_info_list[3].arrow_light.value = 190
 *  - array[1].signal_light_info_list[3].min_time_to_change.value = 4547
 *  - array[1].signal_light_info_list[3].max_time_to_change.value = 7880
 *  - array[1].signal_light_info_list[4].main_light.value = 26
 *  - array[1].signal_light_info_list[4].arrow_light.value = 134
 *  - array[1].signal_light_info_list[4].min_time_to_change.value = 9877
 *  - array[1].signal_light_info_list[4].max_time_to_change.value = 4322
 *  - array[1].signal_light_info_list[5].main_light.value = 57
 *  - array[1].signal_light_info_list[5].arrow_light.value = 154
 *  - array[1].signal_light_info_list[5].min_time_to_change.value = 487
 *  - array[1].signal_light_info_list[5].max_time_to_change.value = 9633
 *  - array[1].signal_light_info_list[6].main_light.value = 74
 *  - array[1].signal_light_info_list[6].arrow_light.value = 246
 *  - array[1].signal_light_info_list[6].min_time_to_change.value = 5633
 *  - array[1].signal_light_info_list[6].max_time_to_change.value = 8966
 *  - array[1].signal_light_info_list[7].main_light.value = 175
 *  - array[1].signal_light_info_list[7].arrow_light.value = 170
 *  - array[1].signal_light_info_list[7].min_time_to_change.value = 1380
 *  - array[1].signal_light_info_list[7].max_time_to_change.value = 3792
 *  - array[1].signal_light_info_list[8].main_light.value = 234
 *  - array[1].signal_light_info_list[8].arrow_light.value = 178
 *  - array[1].signal_light_info_list[8].min_time_to_change.value = 4863
 *  - array[1].signal_light_info_list[8].max_time_to_change.value = 8625
 *  - array[1].signal_light_info_list[9].main_light.value = 105
 *  - array[1].signal_light_info_list[9].arrow_light.value = 183
 *  - array[1].signal_light_info_list[9].min_time_to_change.value = 6540
 *  - array[1].signal_light_info_list[9].max_time_to_change.value = 8580
 *  - array[1].signal_light_info_list[10].main_light.value = 28
 *  - array[1].signal_light_info_list[10].arrow_light.value = 4
 *  - array[1].signal_light_info_list[10].min_time_to_change.value = 6136
 *  - array[1].signal_light_info_list[10].max_time_to_change.value = 4322
 *  - array[1].signal_light_info_list[11].main_light.value = 155
 *  - array[1].signal_light_info_list[11].arrow_light.value = 204
 *  - array[1].signal_light_info_list[11].min_time_to_change.value = 12366
 *  - array[1].signal_light_info_list[11].max_time_to_change.value = 18590
 */

TEST(TestConverterSignalInfoArray, RsToRosNomal) {
  // setup
  test_util::CandidateTestCase_s data = test_util::CandidateTestCase_s();
  ResultSet test_in = test_util::CreateResultSet_s(data);

  // exercise
  const auto test_out = Converter<SignalInfoArray>::ResultSetToRos(test_in).get();

  // verify
  test_util::ExpectValueEq(test_out.array[0].crp_id.value, 123U);
  test_util::ExpectValueEq(test_out.array[0].signal_id_list[0].value, 200U);
  test_util::ExpectValueEq(test_out.array[0].signal_id_list[1].value, 201U);
  test_util::ExpectValueEq(test_out.array[0].time.value, 789UL);
  test_util::ExpectValueEq(test_out.array[0].state.value, 110);
  test_util::ExpectValueEq(test_out.array[0].specific_control_flags.value, 120);
  test_util::ExpectValueEq(test_out.array[0].event_count.value, 130);
  test_util::ExpectValueEq(test_out.array[0].count_down_stop_flag.value, 219);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[0].main_light.value, 142);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[0].arrow_light.value, 65);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[0].min_time_to_change.value, 741);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[0].max_time_to_change.value, 852);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[1].main_light.value, 195);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[1].arrow_light.value, 147);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[1].min_time_to_change.value, 258);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[1].max_time_to_change.value, 369);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[2].main_light.value, 159);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[2].arrow_light.value, 241);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[2].min_time_to_change.value, 963);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[2].max_time_to_change.value, 852);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[3].main_light.value, 229);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[3].arrow_light.value, 189);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[3].min_time_to_change.value, 4546);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[3].max_time_to_change.value, 7879);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[4].main_light.value, 25);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[4].arrow_light.value, 133);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[4].min_time_to_change.value, 9876);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[4].max_time_to_change.value, 4321);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[5].main_light.value, 56);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[5].arrow_light.value, 153);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[5].min_time_to_change.value, 486);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[5].max_time_to_change.value, 9632);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[6].main_light.value, 73);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[6].arrow_light.value, 245);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[6].min_time_to_change.value, 5632);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[6].max_time_to_change.value, 8965);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[7].main_light.value, 174);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[7].arrow_light.value, 169);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[7].min_time_to_change.value, 1379);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[7].max_time_to_change.value, 3791);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[8].main_light.value, 233);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[8].arrow_light.value, 177);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[8].min_time_to_change.value, 4862);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[8].max_time_to_change.value, 8624);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[9].main_light.value, 104);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[9].arrow_light.value, 182);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[9].min_time_to_change.value, 6539);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[9].max_time_to_change.value, 8579);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[10].main_light.value, 27);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[10].arrow_light.value, 3);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[10].min_time_to_change.value, 6135);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[10].max_time_to_change.value, 4321);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[11].main_light.value, 154);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[11].arrow_light.value, 203);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[11].min_time_to_change.value, 12365);
  test_util::ExpectValueEq(test_out.array[0].signal_light_info_list[11].max_time_to_change.value, 18589);

  test_util::ExpectValueEq(test_out.array[1].crp_id.value, 124U);
  test_util::ExpectValueEq(test_out.array[1].signal_id_list[0].value, 202U);
  test_util::ExpectValueEq(test_out.array[1].signal_id_list[1].value, 203U);
  test_util::ExpectValueEq(test_out.array[1].time.value, 790UL);
  test_util::ExpectValueEq(test_out.array[1].state.value, 111);
  test_util::ExpectValueEq(test_out.array[1].specific_control_flags.value, 121);
  test_util::ExpectValueEq(test_out.array[1].event_count.value, 131);
  test_util::ExpectValueEq(test_out.array[1].count_down_stop_flag.value, 220);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[0].main_light.value, 143);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[0].arrow_light.value, 66);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[0].min_time_to_change.value, 742);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[0].max_time_to_change.value, 853);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[1].main_light.value, 196);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[1].arrow_light.value, 148);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[1].min_time_to_change.value, 259);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[1].max_time_to_change.value, 370);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[2].main_light.value, 160);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[2].arrow_light.value, 242);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[2].min_time_to_change.value, 964);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[2].max_time_to_change.value, 853);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[3].main_light.value, 230);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[3].arrow_light.value, 190);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[3].min_time_to_change.value, 4547);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[3].max_time_to_change.value, 7880);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[4].main_light.value, 26);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[4].arrow_light.value, 134);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[4].min_time_to_change.value, 9877);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[4].max_time_to_change.value, 4322);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[5].main_light.value, 57);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[5].arrow_light.value, 154);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[5].min_time_to_change.value, 487);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[5].max_time_to_change.value, 9633);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[6].main_light.value, 74);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[6].arrow_light.value, 246);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[6].min_time_to_change.value, 5633);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[6].max_time_to_change.value, 8966);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[7].main_light.value, 175);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[7].arrow_light.value, 170);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[7].min_time_to_change.value, 1380);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[7].max_time_to_change.value, 3792);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[8].main_light.value, 234);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[8].arrow_light.value, 178);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[8].min_time_to_change.value, 4863);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[8].max_time_to_change.value, 8625);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[9].main_light.value, 105);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[9].arrow_light.value, 183);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[9].min_time_to_change.value, 6540);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[9].max_time_to_change.value, 8580);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[10].main_light.value, 28);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[10].arrow_light.value, 4);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[10].min_time_to_change.value, 6136);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[10].max_time_to_change.value, 4322);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[11].main_light.value, 155);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[11].arrow_light.value, 204);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[11].min_time_to_change.value, 12366);
  test_util::ExpectValueEq(test_out.array[1].signal_light_info_list[11].max_time_to_change.value, 18590);

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

TEST(TestConverterSignalInfoArray, RsToRosAbnomal) {
  // setup
  test_util::CandidateTestCase_s data = test_util::CandidateTestCase_s();
  data.Update_s("crp_id", {"test", "test2"});
  ResultSet test_in = test_util::CreateResultSet_s(data);

  // exercise
  const auto test_out = Converter<SignalInfoArray>::ResultSetToRos(test_in);

  // verify
  EXPECT_FALSE(test_out);
}

/**
 * @brief FreespaceInfoArray型のROSメッセージからTuple変換することを検証する
 * 
 * - 入力値: Ros2メッセージ:
 *  - array[0].crp_id.value = 123U
 *  - array[0].signal_id_list[0].value = 200U
 *  - array[0].signal_id_list[1].value = 201U
 *  - array[0].time.value = 789UL
 *  - array[0].state.value = 110
 *  - array[0].specific_control_flags.value = 120
 *  - array[0].event_count.value = 130
 *  - array[0].count_down_stop_flag.value = 219
 *  - array[0].signal_light_info_list[0].main_light.value = 142
 *  - array[0].signal_light_info_list[0].arrow_light.value = 65
 *  - array[0].signal_light_info_list[0].min_time_to_change.value = 741
 *  - array[0].signal_light_info_list[0].max_time_to_change.value = 852
 *  - array[0].signal_light_info_list[1].main_light.value = 195
 *  - array[0].signal_light_info_list[1].arrow_light.value = 147
 *  - array[0].signal_light_info_list[1].min_time_to_change.value = 258
 *  - array[0].signal_light_info_list[1].max_time_to_change.value = 369
 *  - array[0].signal_light_info_list[2].main_light.value = 159
 *  - array[0].signal_light_info_list[2].arrow_light.value = 241
 *  - array[0].signal_light_info_list[2].min_time_to_change.value = 963
 *  - array[0].signal_light_info_list[2].max_time_to_change.value = 852
 *  - array[0].signal_light_info_list[3].main_light.value = 229
 *  - array[0].signal_light_info_list[3].arrow_light.value = 189
 *  - array[0].signal_light_info_list[3].min_time_to_change.value = 4546
 *  - array[0].signal_light_info_list[3].max_time_to_change.value = 7879
 *  - array[0].signal_light_info_list[4].main_light.value = 25
 *  - array[0].signal_light_info_list[4].arrow_light.value = 133
 *  - array[0].signal_light_info_list[4].min_time_to_change.value = 9876
 *  - array[0].signal_light_info_list[4].max_time_to_change.value = 4321
 *  - array[0].signal_light_info_list[5].main_light.value = 56
 *  - array[0].signal_light_info_list[5].arrow_light.value = 153
 *  - array[0].signal_light_info_list[5].min_time_to_change.value = 486
 *  - array[0].signal_light_info_list[5].max_time_to_change.value = 9632
 *  - array[0].signal_light_info_list[6].main_light.value = 73
 *  - array[0].signal_light_info_list[6].arrow_light.value = 245
 *  - array[0].signal_light_info_list[6].min_time_to_change.value = 5632
 *  - array[0].signal_light_info_list[6].max_time_to_change.value = 8965
 *  - array[0].signal_light_info_list[7].main_light.value = 174
 *  - array[0].signal_light_info_list[7].arrow_light.value = 169
 *  - array[0].signal_light_info_list[7].min_time_to_change.value = 1379
 *  - array[0].signal_light_info_list[7].max_time_to_change.value = 3791
 *  - array[0].signal_light_info_list[8].main_light.value = 233
 *  - array[0].signal_light_info_list[8].arrow_light.value = 177
 *  - array[0].signal_light_info_list[8].min_time_to_change.value = 4862
 *  - array[0].signal_light_info_list[8].max_time_to_change.value = 8624
 *  - array[0].signal_light_info_list[9].main_light.value = 104
 *  - array[0].signal_light_info_list[9].arrow_light.value = 182
 *  - array[0].signal_light_info_list[9].min_time_to_change.value = 6539
 *  - array[0].signal_light_info_list[9].max_time_to_change.value = 8579
 *  - array[0].signal_light_info_list[10].main_light.value = 27
 *  - array[0].signal_light_info_list[10].arrow_light.value = 3
 *  - array[0].signal_light_info_list[10].min_time_to_change.value = 6135
 *  - array[0].signal_light_info_list[10].max_time_to_change.value = 4321
 *  - array[0].signal_light_info_list[11].main_light.value = 154
 *  - array[0].signal_light_info_list[11].arrow_light.value = 203
 *  - array[0].signal_light_info_list[11].min_time_to_change.value = 12365
 *  - array[0].signal_light_info_list[11].max_time_to_change.value = 18589
 * 
 *  - array[1].crp_id.value = 124U
 *  - array[1].signal_id_list[0].value = 202U
 *  - array[1].signal_id_list[1].value = 203U
 *  - array[1].time.value = 790UL
 *  - array[1].state.value = 111
 *  - array[1].specific_control_flags.value = 121
 *  - array[1].event_count.value = 131
 *  - array[1].count_down_stop_flag.value = 220
 *  - array[1].signal_light_info_list[0].main_light.value = 143
 *  - array[1].signal_light_info_list[0].arrow_light.value = 66
 *  - array[1].signal_light_info_list[0].min_time_to_change.value = 742
 *  - array[1].signal_light_info_list[0].max_time_to_change.value = 853
 *  - array[1].signal_light_info_list[1].main_light.value = 196
 *  - array[1].signal_light_info_list[1].arrow_light.value = 148
 *  - array[1].signal_light_info_list[1].min_time_to_change.value = 259
 *  - array[1].signal_light_info_list[1].max_time_to_change.value = 370
 *  - array[1].signal_light_info_list[2].main_light.value = 160
 *  - array[1].signal_light_info_list[2].arrow_light.value = 242
 *  - array[1].signal_light_info_list[2].min_time_to_change.value = 964
 *  - array[1].signal_light_info_list[2].max_time_to_change.value = 853
 *  - array[1].signal_light_info_list[3].main_light.value = 230
 *  - array[1].signal_light_info_list[3].arrow_light.value = 190
 *  - array[1].signal_light_info_list[3].min_time_to_change.value = 4547
 *  - array[1].signal_light_info_list[3].max_time_to_change.value = 7880
 *  - array[1].signal_light_info_list[4].main_light.value = 26
 *  - array[1].signal_light_info_list[4].arrow_light.value = 134
 *  - array[1].signal_light_info_list[4].min_time_to_change.value = 9877
 *  - array[1].signal_light_info_list[4].max_time_to_change.value = 4322
 *  - array[1].signal_light_info_list[5].main_light.value = 57
 *  - array[1].signal_light_info_list[5].arrow_light.value = 154
 *  - array[1].signal_light_info_list[5].min_time_to_change.value = 487
 *  - array[1].signal_light_info_list[5].max_time_to_change.value = 9633
 *  - array[1].signal_light_info_list[6].main_light.value = 74
 *  - array[1].signal_light_info_list[6].arrow_light.value = 246
 *  - array[1].signal_light_info_list[6].min_time_to_change.value = 5633
 *  - array[1].signal_light_info_list[6].max_time_to_change.value = 8966
 *  - array[1].signal_light_info_list[7].main_light.value = 175
 *  - array[1].signal_light_info_list[7].arrow_light.value = 170
 *  - array[1].signal_light_info_list[7].min_time_to_change.value = 1380
 *  - array[1].signal_light_info_list[7].max_time_to_change.value = 3792
 *  - array[1].signal_light_info_list[8].main_light.value = 234
 *  - array[1].signal_light_info_list[8].arrow_light.value = 178
 *  - array[1].signal_light_info_list[8].min_time_to_change.value = 4863
 *  - array[1].signal_light_info_list[8].max_time_to_change.value = 8625
 *  - array[1].signal_light_info_list[9].main_light.value = 105
 *  - array[1].signal_light_info_list[9].arrow_light.value = 183
 *  - array[1].signal_light_info_list[9].min_time_to_change.value = 6540
 *  - array[1].signal_light_info_list[9].max_time_to_change.value = 8580
 *  - array[1].signal_light_info_list[10].main_light.value = 28
 *  - array[1].signal_light_info_list[10].arrow_light.value = 4
 *  - array[1].signal_light_info_list[10].min_time_to_change.value = 6136
 *  - array[1].signal_light_info_list[10].max_time_to_change.value = 4322
 *  - array[1].signal_light_info_list[11].main_light.value = 155
 *  - array[1].signal_light_info_list[11].arrow_light.value = 204
 *  - array[1].signal_light_info_list[11].min_time_to_change.value = 12366
 *  - array[1].signal_light_info_list[11].max_time_to_change.value = 18590
 * 
 * - 期待する返却値: result:
 *  - result.at(0).at(0) = 123
 *  - result.at(0).at(1).at(0) = 200
 *  - result.at(0).at(1).at(1) = 201
 *  - result.at(0).at(2) = 789
 *  - result.at(0).at(3) = 110
 *  - result.at(0).at(4) = 120
 *  - result.at(0).at(5) = 130
 *  - result.at(0).at(6) = 219
 *  - result.at(0).at(7) = 142
 *  - result.at(0).at(8) = 65
 *  - result.at(0).at(9) = 741
 *  - result.at(0).at(10) = 852
 *  - result.at(0).at(11) = 195
 *  - result.at(0).at(12) = 147
 *  - result.at(0).at(13) = 258
 *  - result.at(0).at(14) = 369
 *  - result.at(0).at(15) = 159
 *  - result.at(0).at(16) = 241
 *  - result.at(0).at(17) = 963
 *  - result.at(0).at(18) = 852
 *  - result.at(0).at(19) = 229
 *  - result.at(0).at(20) = 189
 *  - result.at(0).at(21) = 4546
 *  - result.at(0).at(22) = 7879
 *  - result.at(0).at(23) = 25
 *  - result.at(0).at(24) = 133
 *  - result.at(0).at(25) = 9876
 *  - result.at(0).at(26) = 4321
 *  - result.at(0).at(27) = 56
 *  - result.at(0).at(28) = 153
 *  - result.at(0).at(29) = 486
 *  - result.at(0).at(30) = 9632
 *  - result.at(0).at(31) = 73
 *  - result.at(0).at(32) = 245
 *  - result.at(0).at(33) = 5632
 *  - result.at(0).at(34) = 8965
 *  - result.at(0).at(35) = 174
 *  - result.at(0).at(36) = 169
 *  - result.at(0).at(37) = 1379
 *  - result.at(0).at(38) = 3791
 *  - result.at(0).at(39) = 233
 *  - result.at(0).at(40) = 177
 *  - result.at(0).at(41) = 4862
 *  - result.at(0).at(42) = 8624
 *  - result.at(0).at(43) = 104
 *  - result.at(0).at(44) = 182
 *  - result.at(0).at(45) = 6539
 *  - result.at(0).at(46) = 8579
 *  - result.at(0).at(47) = 27
 *  - result.at(0).at(48) = 3
 *  - result.at(0).at(49) = 6135
 *  - result.at(0).at(50) = 4321
 *  - result.at(0).at(51) = 154
 *  - result.at(0).at(52) = 203
 *  - result.at(0).at(53) = 12365
 *  - result.at(0).at(54) = 18589

 *  - result.at(1).at(0) = 124
 *  - result.at(1).at(1).at(0) = 202
 *  - result.at(1).at(1).at(1) = 203
 *  - result.at(1).at(2) = 790
 *  - result.at(1).at(3) = 111
 *  - result.at(1).at(4) = 121
 *  - result.at(1).at(5) = 131
 *  - result.at(1).at(6) = 220
 *  - result.at(1).at(7) = 143
 *  - result.at(1).at(8) = 66
 *  - result.at(1).at(9) = 742
 *  - result.at(1).at(10) = 853
 *  - result.at(1).at(11) = 196
 *  - result.at(1).at(12) = 148
 *  - result.at(1).at(13) = 259
 *  - result.at(1).at(14) = 370
 *  - result.at(1).at(15) = 160
 *  - result.at(1).at(16) = 242
 *  - result.at(1).at(17) = 964
 *  - result.at(1).at(18) = 853
 *  - result.at(1).at(19) = 230
 *  - result.at(1).at(20) = 190
 *  - result.at(1).at(21) = 4547
 *  - result.at(1).at(22) = 7880
 *  - result.at(1).at(23) = 27
 *  - result.at(1).at(24) = 134
 *  - result.at(1).at(25) = 9877
 *  - result.at(1).at(26) = 4322
 *  - result.at(1).at(27) = 57
 *  - result.at(1).at(28) = 154
 *  - result.at(1).at(29) = 487
 *  - result.at(1).at(30) = 9633
 *  - result.at(1).at(31) = 74
 *  - result.at(1).at(32) = 246
 *  - result.at(1).at(33) = 5633
 *  - result.at(1).at(34) = 8966
 *  - result.at(1).at(35) = 175
 *  - result.at(1).at(36) = 170
 *  - result.at(1).at(37) = 1380
 *  - result.at(1).at(38) = 3792
 *  - result.at(1).at(39) = 234
 *  - result.at(1).at(40) = 178
 *  - result.at(1).at(41) = 4863
 *  - result.at(1).at(42) = 8625
 *  - result.at(1).at(43) = 105
 *  - result.at(1).at(44) = 183
 *  - result.at(1).at(45) = 6540
 *  - result.at(1).at(46) = 8580
 *  - result.at(1).at(47) = 28
 *  - result.at(1).at(48) = 4
 *  - result.at(1).at(49) = 6136
 *  - result.at(1).at(50) = 4322
 *  - result.at(1).at(51) = 155
 *  - result.at(1).at(52) = 204
 *  - result.at(1).at(53) = 12366
 *  - result.at(1).at(54) = 18590
 */
TEST(TestConverterSignalInfoArray, RosToTupleNormal) {
  // setup
  SignalInfoArray test_in = test_util::CreateSignalInfoArray();

  // exercise
  auto test_out = Converter<SignalInfoArray>::RosToTuple(test_in, 0);

  // verify
  std::vector<any> val1;
  std::vector<long> timestamp1;
  std::vector<any> val2;
  std::vector<long> timestamp2;
  const int data_nums = 55;
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

  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(0)), 123);
  test_util::ExpectValueEq(dm_any::any_cast<std::vector<int>>(val1.at(1)).at(0), 200);
  test_util::ExpectValueEq(dm_any::any_cast<std::vector<int>>(val1.at(1)).at(1), 201);
  test_util::ExpectValueEq(dm_any::any_cast<long>(val1.at(2)), 789);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(3)), 110);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(4)), 120);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(5)), 130);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(6)), 219);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(7)), 142);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(8)), 65);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(9)), 741);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(10)), 852);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(11)), 195);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(12)), 147);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(13)), 258);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(14)), 369);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(15)), 159);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(16)), 241);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(17)), 963);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(18)), 852);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(19)), 229);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(20)), 189);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(21)), 4546);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(22)), 7879);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(23)), 25);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(24)), 133);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(25)), 9876);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(26)), 4321);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(27)), 56);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(28)), 153);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(29)), 486);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(30)), 9632);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(31)), 73);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(32)), 245);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(33)), 5632);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(34)), 8965);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(35)), 174);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(36)), 169);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(37)), 1379);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(38)), 3791);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(39)), 233);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(40)), 177);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(41)), 4862);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(42)), 8624);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(43)), 104);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(44)), 182);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(45)), 6539);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(46)), 8579);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(47)), 27);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(48)), 3);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(49)), 6135);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(50)), 4321);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(51)), 154);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(52)), 203);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(53)), 12365);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val1.at(54)), 18589);

  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(0)), 124);
  test_util::ExpectValueEq(dm_any::any_cast<std::vector<int>>(val2.at(1)).at(0), 202);
  test_util::ExpectValueEq(dm_any::any_cast<std::vector<int>>(val2.at(1)).at(1), 203);
  test_util::ExpectValueEq(dm_any::any_cast<long>(val2.at(2)), 790);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(3)), 111);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(4)), 121);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(5)), 131);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(6)), 220);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(7)), 143);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(8)), 66);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(9)), 742);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(10)), 853);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(11)), 196);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(12)), 148);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(13)), 259);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(14)), 370);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(15)), 160);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(16)), 242);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(17)), 964);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(18)), 853);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(19)), 230);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(20)), 190);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(21)), 4547);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(22)), 7880);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(23)), 27);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(24)), 134);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(25)), 9877);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(26)), 4322);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(27)), 57);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(28)), 154);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(29)), 487);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(30)), 9633);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(31)), 74);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(32)), 246);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(33)), 5633);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(34)), 8966);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(35)), 175);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(36)), 170);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(37)), 1380);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(38)), 3792);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(39)), 234);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(40)), 178);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(41)), 4863);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(42)), 8625);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(43)), 105);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(44)), 183);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(45)), 6540);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(46)), 8580);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(47)), 28);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(48)), 4);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(49)), 6136);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(50)), 4322);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(51)), 155);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(52)), 204);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(53)), 12366);
  test_util::ExpectValueEq(dm_any::any_cast<int>(val2.at(54)), 18590);

  // teardown
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

}  // namespace test
}  // namespace ros2_dmi
