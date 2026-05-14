#ifndef TEST_INCLUDE_ROSMSG_SIGNAL_INFO_HPP_
#define TEST_INCLUDE_ROSMSG_SIGNAL_INFO_HPP_

#include <vector>

#include <dm_signal_info_msgs/msg/signal_info_array.hpp>

namespace ros2_dmi {
namespace test_util {
/**
 * @brief SignalInfoArray型のRosメッセージデータを作成する
 * 
 * @return SignalInfoArray型のRosメッセージデータ
 */
SignalInfoArray CreateSignalInfoArray() {
  SignalInfo data_1;
  data_1.crp_id.value = 123;
  std::vector<SignalId> id_list_1(2);
  id_list_1[0].value = 200;
  id_list_1[1].value = 201;
  data_1.signal_id_list = id_list_1;
  data_1.time.value = 789;
  data_1.state.value = 110;
  data_1.specific_control_flags.value = 120;
  data_1.event_count.value = 130;
  data_1.count_down_stop_flag.value = 219;
  std::vector<SignalLightInfo> signal_light_info_list_1;
  signal_light_info_list_1.resize(12);

  signal_light_info_list_1[0].main_light.value = 142;
  signal_light_info_list_1[0].arrow_light.value = 65;
  signal_light_info_list_1[0].min_time_to_change.value = 741;
  signal_light_info_list_1[0].max_time_to_change.value = 852;
  signal_light_info_list_1[1].main_light.value = 195;
  signal_light_info_list_1[1].arrow_light.value = 147;
  signal_light_info_list_1[1].min_time_to_change.value = 258;
  signal_light_info_list_1[1].max_time_to_change.value = 369;
  signal_light_info_list_1[2].main_light.value = 159;
  signal_light_info_list_1[2].arrow_light.value = 241;
  signal_light_info_list_1[2].min_time_to_change.value = 963;
  signal_light_info_list_1[2].max_time_to_change.value = 852;
  signal_light_info_list_1[3].main_light.value = 229;
  signal_light_info_list_1[3].arrow_light.value = 189;
  signal_light_info_list_1[3].min_time_to_change.value = 4546;
  signal_light_info_list_1[3].max_time_to_change.value = 7879;
  signal_light_info_list_1[4].main_light.value = 25;
  signal_light_info_list_1[4].arrow_light.value = 133;
  signal_light_info_list_1[4].min_time_to_change.value = 9876;
  signal_light_info_list_1[4].max_time_to_change.value = 4321;
  signal_light_info_list_1[5].main_light.value = 56;
  signal_light_info_list_1[5].arrow_light.value = 153;
  signal_light_info_list_1[5].min_time_to_change.value = 486;
  signal_light_info_list_1[5].max_time_to_change.value = 9632;
  signal_light_info_list_1[6].main_light.value = 73;
  signal_light_info_list_1[6].arrow_light.value = 245;
  signal_light_info_list_1[6].min_time_to_change.value = 5632;
  signal_light_info_list_1[6].max_time_to_change.value = 8965;
  signal_light_info_list_1[7].main_light.value = 174;
  signal_light_info_list_1[7].arrow_light.value = 169;
  signal_light_info_list_1[7].min_time_to_change.value = 1379;
  signal_light_info_list_1[7].max_time_to_change.value = 3791;
  signal_light_info_list_1[8].main_light.value = 233;
  signal_light_info_list_1[8].arrow_light.value = 177;
  signal_light_info_list_1[8].min_time_to_change.value = 4862;
  signal_light_info_list_1[8].max_time_to_change.value = 8624;
  signal_light_info_list_1[9].main_light.value = 104;
  signal_light_info_list_1[9].arrow_light.value = 182;
  signal_light_info_list_1[9].min_time_to_change.value = 6539;
  signal_light_info_list_1[9].max_time_to_change.value = 8579;
  signal_light_info_list_1[10].main_light.value = 27;
  signal_light_info_list_1[10].arrow_light.value = 3;
  signal_light_info_list_1[10].min_time_to_change.value = 6135;
  signal_light_info_list_1[10].max_time_to_change.value = 4321;
  signal_light_info_list_1[11].main_light.value = 154;
  signal_light_info_list_1[11].arrow_light.value = 203;
  signal_light_info_list_1[11].min_time_to_change.value = 12365;
  signal_light_info_list_1[11].max_time_to_change.value = 18589;
  data_1.signal_light_info_list = signal_light_info_list_1;


  SignalInfo data_2;
  data_2.crp_id.value = 124;
  std::vector<SignalId> id_list_2(2);
  id_list_2[0].value = 202;
  id_list_2[1].value = 203;
  data_2.signal_id_list = id_list_2;
  data_2.time.value = 790;
  data_2.state.value = 111;
  data_2.specific_control_flags.value = 121;
  data_2.event_count.value = 131;
  data_2.count_down_stop_flag.value = 220;
  std::vector<SignalLightInfo> signal_light_info_list_2;
  signal_light_info_list_2.resize(12);

  signal_light_info_list_2[0].main_light.value = 143;
  signal_light_info_list_2[0].arrow_light.value = 66;
  signal_light_info_list_2[0].min_time_to_change.value = 742;
  signal_light_info_list_2[0].max_time_to_change.value = 853;
  signal_light_info_list_2[1].main_light.value = 196;
  signal_light_info_list_2[1].arrow_light.value = 148;
  signal_light_info_list_2[1].min_time_to_change.value = 259;
  signal_light_info_list_2[1].max_time_to_change.value = 370;
  signal_light_info_list_2[2].main_light.value = 160;
  signal_light_info_list_2[2].arrow_light.value = 242;
  signal_light_info_list_2[2].min_time_to_change.value = 964;
  signal_light_info_list_2[2].max_time_to_change.value = 853;
  signal_light_info_list_2[3].main_light.value = 230;
  signal_light_info_list_2[3].arrow_light.value = 190;
  signal_light_info_list_2[3].min_time_to_change.value = 4547;
  signal_light_info_list_2[3].max_time_to_change.value = 7880;
  signal_light_info_list_2[4].main_light.value = 27;
  signal_light_info_list_2[4].arrow_light.value = 134;
  signal_light_info_list_2[4].min_time_to_change.value = 9877;
  signal_light_info_list_2[4].max_time_to_change.value = 4322;
  signal_light_info_list_2[5].main_light.value = 57;
  signal_light_info_list_2[5].arrow_light.value = 154;
  signal_light_info_list_2[5].min_time_to_change.value = 487;
  signal_light_info_list_2[5].max_time_to_change.value = 9633;
  signal_light_info_list_2[6].main_light.value = 74;
  signal_light_info_list_2[6].arrow_light.value = 246;
  signal_light_info_list_2[6].min_time_to_change.value = 5633;
  signal_light_info_list_2[6].max_time_to_change.value = 8966;
  signal_light_info_list_2[7].main_light.value = 175;
  signal_light_info_list_2[7].arrow_light.value = 170;
  signal_light_info_list_2[7].min_time_to_change.value = 1380;
  signal_light_info_list_2[7].max_time_to_change.value = 3792;
  signal_light_info_list_2[8].main_light.value = 234;
  signal_light_info_list_2[8].arrow_light.value = 178;
  signal_light_info_list_2[8].min_time_to_change.value = 4863;
  signal_light_info_list_2[8].max_time_to_change.value = 8625;
  signal_light_info_list_2[9].main_light.value = 105;
  signal_light_info_list_2[9].arrow_light.value = 183;
  signal_light_info_list_2[9].min_time_to_change.value = 6540;
  signal_light_info_list_2[9].max_time_to_change.value = 8580;
  signal_light_info_list_2[10].main_light.value = 28;
  signal_light_info_list_2[10].arrow_light.value = 4;
  signal_light_info_list_2[10].min_time_to_change.value = 6136;
  signal_light_info_list_2[10].max_time_to_change.value = 4322;
  signal_light_info_list_2[11].main_light.value = 155;
  signal_light_info_list_2[11].arrow_light.value = 204;
  signal_light_info_list_2[11].min_time_to_change.value = 12366;
  signal_light_info_list_2[11].max_time_to_change.value = 18590;
  data_2.signal_light_info_list = signal_light_info_list_2;

  SignalInfoArray msg;
  msg.array.push_back(data_1);
  msg.array.push_back(data_2);

  return msg;
}

}  // namespace test_util
}  // namespace ros2_dmi

#endif  // TEST_INCLUDE_ROSMSG_SIGNAL_INFO_HPP_