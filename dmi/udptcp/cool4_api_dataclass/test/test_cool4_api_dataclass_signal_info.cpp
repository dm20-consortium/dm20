/**
 * @file test_cool4_api_dataclass_signal_info.cpp
 *
 * cool4_api_dataclass::LightInformationクラスのテスト
 * cool4_api_dataclass::SignalInfoクラスのテスト
 */
#include "cool4_api_dataclass/signal_info.hpp"

#include <gtest/gtest.h>


using cool4_api_dataclass::LightInformation;
using cool4_api_dataclass::SignalInfo;
using cool4_api_dataclass::ByteArray;

/**
 * @brief デフォルトコンストラクタを用いたインスタンス化を正常に行うことができるかを検証する
 *
 * - 例外がないこと
 * - 初期値が想定どおり(下記)であること
 *         main_light: 0
 *         arrow_light: 0
 *         min_time_to_change: 65535
 *         max_time_to_change: 65535
 *
 */
TEST(Cool4ApiDataclassLightInfomation, DefaultConstructor) {
  // setup
  // exercise
  auto target = LightInformation();

  // verify
  EXPECT_EQ(0, target.main_light);
  EXPECT_EQ(0, target.arrow_light);
  EXPECT_EQ(65535, target.min_time_to_change);
  EXPECT_EQ(65535, target.max_time_to_change);

  // teadown
  // nothing to do
}

/**
 * @brief 引数ありコンストラクタを用いたインスタンス化を正常に行うことができるかを検証する
 *
 * - 例外がないこと
 *
 * - {1, 2, 3, 4}を与えた場合，下記のとおりになること
 *
 *       main_light: 1
 *       arrow_light: 2
 *       min_time_to_change: 3
 *       max_time_to_change: 4
 *
 */
TEST(Cool4ApiDataclassLightInformation, Constructor) {
  // setup
  // exercise
  auto target = LightInformation({1, 2, 3, 4});

  // verify
  EXPECT_EQ(1, target.main_light);
  EXPECT_EQ(2, target.arrow_light);
  EXPECT_EQ(3, target.min_time_to_change);
  EXPECT_EQ(4, target.max_time_to_change);

  // teardown
  // nothing to do
}

/**
 *@brief バイト配列への変換処理が正しく機能するか
 *
 * - {1, 2, 3, 4}を与えたときバイト配列が{1, 2, 0, 3, 0, 4}となること
 *
 */
TEST(Cool4ApiDataclassLightInformation, TobyteArray) {
  // setup
  auto target = LightInformation({1, 2, 3, 4});

  // exercise
  const auto out = target.ToByteArray();

  // verify
  EXPECT_NE(boost::none, out);
  const auto contents = out.get();
  EXPECT_EQ(6, static_cast<int>(contents.size()));
  EXPECT_EQ(1, contents[0]);
  EXPECT_EQ(2, contents[1]);
  EXPECT_EQ(0, contents[2]);
  EXPECT_EQ(3, contents[3]);
  EXPECT_EQ(0, contents[4]);
  EXPECT_EQ(4, contents[5]);

  // teardown
  // nothing to do
}

/**
 * @brief バイト配列からのインスタンス化処理が正しく機能するかどうか
 *
 * - {1, 2, 3, 4, 5, 6}を入力したとき以下のような値でインスタンス化されること
 *
 *       main_light: 1
 *       arrow_light: 2
 *       min_time_to_change: 772
 *       max_time_to_change: 1286
 *
 */
TEST(Cool4ApiDataclassLightInformation, FromByteArray) {
  // setup
  ByteArray input = {1, 2, 3, 4, 5, 6};
  size_t idx = 0;

  // exercise
  const auto out = LightInformation::FromByteArray(input, idx);

  // verify
  EXPECT_NE(boost::none, out);
  const auto contents = out.get();
  EXPECT_EQ(1, contents.main_light);
  EXPECT_EQ(2, contents.arrow_light);
  EXPECT_EQ(772, contents.min_time_to_change);
  EXPECT_EQ(1286, contents.max_time_to_change);

  // teardown
  // nothing to do
}

/**
 * @brief 出力ストリームへの表示が正しくできること
 *
 * - 以下のような値を持つLightInformationのインスタンスをストリームへわたす
 *
 *       main_light: 1
 *       arrow_light: 2
 *       min_time_to_change: 3
 *       max_time_to_change: 4
 *
 * - ストリームの内容が1,2,3,4,であること
 */
TEST(Cool4ApiDataclassLightInformation, Print) {
  // setup
  std::stringbuf buf;
  std::streambuf* prev = std::cout.rdbuf(&buf);
  auto target = LightInformation({1, 2, 3, 4});

  // exercise
  std::cout << target;

  // verify
  std::cout.rdbuf(prev);
  EXPECT_EQ("1,2,3,4,", buf.str());

  // teardown
  // nothing to do
}

/**
 * @brief デフォルトコンストラクタを用いたインスタンス化を正常に行うことができるかを検証する
 *
 * - 例外がないこと
 * - 初期値が想定どおり(下記)であること
 *       
 *       time: 0
 *       state: 0
 *       specific_control_flags: 0
 *       event_count: 0
 *       count_down_stop_flag: 0
 *       light_information_list[0].main_light: 0
 *       light_information_list[0].arrow_light: 0
 *       light_information_list[0].min_time_to_change: 65535
 *       light_information_list[0].max_time_to_change: 65535
 *        ...
 *       light_information_list[11].main_light: 0
 *       light_information_list[11].arrow_light: 0
 *       light_information_list[11].min_time_to_change: 65535
 *       light_information_list[11].max_time_to_change: 65535
 */
TEST(Cool4ApiDataclassSignalInfo, DefaultConstructor) {
  // exercise
  auto target = SignalInfo();

  // verify
  EXPECT_EQ(0, target.crp_id);
  ASSERT_EQ(SignalInfo::SignalIdNum, target.signal_id_list.size());
  for (size_t i = 0; i < SignalInfo::SignalIdNum; ++i) {
    EXPECT_EQ(0, target.signal_id_list[i]);
  }
  EXPECT_EQ(0, target.time);
  EXPECT_EQ(0, target.state);
  EXPECT_EQ(0, target.specific_control_flags);
  EXPECT_EQ(0, target.event_count);
  EXPECT_EQ(0, target.count_down_stop_flag);
  ASSERT_EQ(SignalInfo::LightInformationNum, target.light_information_list.size());
  for (size_t j = 0; j < SignalInfo::LightInformationNum; ++j) {
    EXPECT_EQ(0, target.light_information_list[j].main_light);
    EXPECT_EQ(0, target.light_information_list[j].arrow_light);
    EXPECT_EQ(65535, target.light_information_list[j].min_time_to_change);
    EXPECT_EQ(65535, target.light_information_list[j].max_time_to_change);
  }

  // teardown
  // nothing to do
}

/**
 * @brief バイト配列への変換処理が正しく機能するか
 *
 * - 以下の値を与えたとき
 *
 *       time: 1
 *       state: 2
 *       specific_control_flags: 3
 *       event_count: 4
 *       count_down_stop_flag: 5
 *       light_information_list[0].main_light: 6
 *       light_information_list[0].arrow_light: 7
 *       light_information_list[0].min_time_to_change: 8
 *       light_information_list[0].max_time_to_change: 9
 *        ...
 *       light_information_list[11].main_light: 59
 *       light_information_list[11].arrow_light: 60
 *       light_information_list[11].min_time_to_change: 61
 *       light_information_list[11].max_time_to_change: 62
 *
 * - バイト配列に代入元のバイトサイズに応じた値が代入されること
 *
 */
TEST(Cool4ApiDataclassSignalInfo, ToByteArray) {
  // setup
  auto target = SignalInfo();
  uint8_t i = 1;
  target.crp_id = i++;
  for (size_t j = 0; j < SignalInfo::SignalIdNum; ++j) {
    target.signal_id_list[j] = i++;
  }
  target.time = i++;
  target.state = i++;
  target.specific_control_flags = i++;
  target.event_count = i++;
  target.count_down_stop_flag = i++;
  for (size_t k = 0; k < SignalInfo::LightInformationNum; ++k) {
    target.light_information_list[k].main_light = i++;
    target.light_information_list[k].arrow_light = i++;
    target.light_information_list[k].min_time_to_change = i++;
    target.light_information_list[k].max_time_to_change = i++;
  }

  // exercise
  const auto out = target.ToByteArray();

  // verify
  EXPECT_NE(boost::none, out);
  const auto contents = out.get();
  EXPECT_EQ(96, static_cast<int>(contents.size()));
  EXPECT_EQ(0, static_cast<int>(contents[0]));
  EXPECT_EQ(0, static_cast<int>(contents[1]));
  EXPECT_EQ(0, static_cast<int>(contents[2]));
  EXPECT_EQ(1, static_cast<int>(contents[3]));
  EXPECT_EQ(2, static_cast<int>(contents[4]));
  EXPECT_EQ(3, static_cast<int>(contents[5]));
  EXPECT_EQ(4, static_cast<int>(contents[6]));
  EXPECT_EQ(5, static_cast<int>(contents[7]));
  EXPECT_EQ(6, static_cast<int>(contents[8]));
  EXPECT_EQ(7, static_cast<int>(contents[9]));
  EXPECT_EQ(8, static_cast<int>(contents[10]));
  EXPECT_EQ(9, static_cast<int>(contents[11]));
  EXPECT_EQ(0, static_cast<int>(contents[12]));
  EXPECT_EQ(0, static_cast<int>(contents[13]));
  EXPECT_EQ(0, static_cast<int>(contents[14]));
  EXPECT_EQ(0, static_cast<int>(contents[15]));
  EXPECT_EQ(0, static_cast<int>(contents[16]));
  EXPECT_EQ(0, static_cast<int>(contents[17]));
  EXPECT_EQ(0, static_cast<int>(contents[18]));
  EXPECT_EQ(10, static_cast<int>(contents[19]));
  EXPECT_EQ(11, static_cast<int>(contents[20]));
  EXPECT_EQ(12, static_cast<int>(contents[21]));
  EXPECT_EQ(13, static_cast<int>(contents[22]));
  EXPECT_EQ(14, static_cast<int>(contents[23]));
  EXPECT_EQ(15, static_cast<int>(contents[24]));
  EXPECT_EQ(16, static_cast<int>(contents[25]));
  EXPECT_EQ(0, static_cast<int>(contents[26]));
  EXPECT_EQ(17, static_cast<int>(contents[27]));
  EXPECT_EQ(0, static_cast<int>(contents[28]));
  EXPECT_EQ(18, static_cast<int>(contents[29]));
  EXPECT_EQ(19, static_cast<int>(contents[30]));
  EXPECT_EQ(20, static_cast<int>(contents[31]));
  EXPECT_EQ(0, static_cast<int>(contents[32]));
  EXPECT_EQ(21, static_cast<int>(contents[33]));
  EXPECT_EQ(0, static_cast<int>(contents[34]));
  EXPECT_EQ(22, static_cast<int>(contents[35]));
  EXPECT_EQ(23, static_cast<int>(contents[36]));
  EXPECT_EQ(24, static_cast<int>(contents[37]));
  EXPECT_EQ(0, static_cast<int>(contents[38]));
  EXPECT_EQ(25, static_cast<int>(contents[39]));
  EXPECT_EQ(0, static_cast<int>(contents[40]));
  EXPECT_EQ(26, static_cast<int>(contents[41]));
  EXPECT_EQ(27, static_cast<int>(contents[42]));
  EXPECT_EQ(28, static_cast<int>(contents[43]));
  EXPECT_EQ(0, static_cast<int>(contents[44]));
  EXPECT_EQ(29, static_cast<int>(contents[45]));
  EXPECT_EQ(0, static_cast<int>(contents[46]));
  EXPECT_EQ(30, static_cast<int>(contents[47]));
  EXPECT_EQ(31, static_cast<int>(contents[48]));
  EXPECT_EQ(32, static_cast<int>(contents[49]));
  EXPECT_EQ(0, static_cast<int>(contents[50]));
  EXPECT_EQ(33, static_cast<int>(contents[51]));
  EXPECT_EQ(0, static_cast<int>(contents[52]));
  EXPECT_EQ(34, static_cast<int>(contents[53]));
  EXPECT_EQ(35, static_cast<int>(contents[54]));
  EXPECT_EQ(36, static_cast<int>(contents[55]));
  EXPECT_EQ(0, static_cast<int>(contents[56]));
  EXPECT_EQ(37, static_cast<int>(contents[57]));
  EXPECT_EQ(0, static_cast<int>(contents[58]));
  EXPECT_EQ(38, static_cast<int>(contents[59]));
  EXPECT_EQ(39, static_cast<int>(contents[60]));
  EXPECT_EQ(40, static_cast<int>(contents[61]));
  EXPECT_EQ(0, static_cast<int>(contents[62]));
  EXPECT_EQ(41, static_cast<int>(contents[63]));
  EXPECT_EQ(0, static_cast<int>(contents[64]));
  EXPECT_EQ(42, static_cast<int>(contents[65]));
  EXPECT_EQ(43, static_cast<int>(contents[66]));
  EXPECT_EQ(44, static_cast<int>(contents[67]));
  EXPECT_EQ(0, static_cast<int>(contents[68]));
  EXPECT_EQ(45, static_cast<int>(contents[69]));
  EXPECT_EQ(0, static_cast<int>(contents[70]));
  EXPECT_EQ(46, static_cast<int>(contents[71]));
  EXPECT_EQ(47, static_cast<int>(contents[72]));
  EXPECT_EQ(48, static_cast<int>(contents[73]));
  EXPECT_EQ(0, static_cast<int>(contents[74]));
  EXPECT_EQ(49, static_cast<int>(contents[75]));
  EXPECT_EQ(0, static_cast<int>(contents[76]));
  EXPECT_EQ(50, static_cast<int>(contents[77]));
  EXPECT_EQ(51, static_cast<int>(contents[78]));
  EXPECT_EQ(52, static_cast<int>(contents[79]));
  EXPECT_EQ(0, static_cast<int>(contents[80]));
  EXPECT_EQ(53, static_cast<int>(contents[81]));
  EXPECT_EQ(0, static_cast<int>(contents[82]));
  EXPECT_EQ(54, static_cast<int>(contents[83]));
  EXPECT_EQ(55, static_cast<int>(contents[84]));
  EXPECT_EQ(56, static_cast<int>(contents[85]));
  EXPECT_EQ(0, static_cast<int>(contents[86]));
  EXPECT_EQ(57, static_cast<int>(contents[87]));
  EXPECT_EQ(0, static_cast<int>(contents[88]));
  EXPECT_EQ(58, static_cast<int>(contents[89]));
  EXPECT_EQ(59, static_cast<int>(contents[90]));
  EXPECT_EQ(60, static_cast<int>(contents[91]));
  EXPECT_EQ(0, static_cast<int>(contents[92]));
  EXPECT_EQ(61, static_cast<int>(contents[93]));
  EXPECT_EQ(0, static_cast<int>(contents[94]));
  EXPECT_EQ(62, static_cast<int>(contents[95]));

  // teardown
  // nothing to do
}

/**
 * @brief バイト配列からのインスタンス化処理が正しく機能するかどうか
 *
 * - {1, 2, ... , 95, 96}を入力したとき以下のような値でインスタンス化されること
 *
 *       crp_id: 0x01020304
 *       signal_id_list[]: {5, 6, 7, 8, 9, 10, 11, 12}
 *       time: 0x0D0E0F1011121314
 *       state: 21
 *       specific_control_flags: 22
 *       event_count: 23
 *       countdown_stop_flag: 24
 *       light_information_list[0]: {25,26,0x1B1C,0x1D1E}
 *       light_information_list[1]: {31,32,0x2122,0x2324}
 *         ...
 *       light_information_list[11]: {91,92,0x5D5E,0x5F60}
 *
 */
TEST(Cool4ApiDataclassSignalInfo, FromByteArray) {
  // setup
  ByteArray input;
  for(uint8_t i = 1; i <= 96; i++) input.push_back(i);
  size_t idx = 0;

  // exercise
  const auto out = SignalInfo::FromByteArray(input, idx);

  // verify
  EXPECT_NE(boost::none, out);
  const auto contents = out.get();
  EXPECT_EQ(0x01020304, contents.crp_id);
  EXPECT_EQ(0x05, contents.signal_id_list[0]);
  EXPECT_EQ(0x06, contents.signal_id_list[1]);
  EXPECT_EQ(0x07, contents.signal_id_list[2]);
  EXPECT_EQ(0x08, contents.signal_id_list[3]);
  EXPECT_EQ(0x09, contents.signal_id_list[4]);
  EXPECT_EQ(0x0A, contents.signal_id_list[5]);
  EXPECT_EQ(0x0B, contents.signal_id_list[6]);
  EXPECT_EQ(0x0C, contents.signal_id_list[7]);
  EXPECT_EQ(0x0D0E0F1011121314, contents.time);
  EXPECT_EQ(0x15, contents.state);
  EXPECT_EQ(0x16, contents.specific_control_flags);
  EXPECT_EQ(0x17, contents.event_count);
  EXPECT_EQ(0x18, contents.count_down_stop_flag);
  EXPECT_EQ(0x19, contents.light_information_list[0].main_light);
  EXPECT_EQ(0x1A, contents.light_information_list[0].arrow_light);
  EXPECT_EQ(0x1B1C, contents.light_information_list[0].min_time_to_change);
  EXPECT_EQ(0x1D1E, contents.light_information_list[0].max_time_to_change);
  EXPECT_EQ(0x1F, contents.light_information_list[1].main_light);
  EXPECT_EQ(0x20, contents.light_information_list[1].arrow_light);
  EXPECT_EQ(0x2122, contents.light_information_list[1].min_time_to_change);
  EXPECT_EQ(0x2324, contents.light_information_list[1].max_time_to_change);
  EXPECT_EQ(0x25, contents.light_information_list[2].main_light);
  EXPECT_EQ(0x26, contents.light_information_list[2].arrow_light);
  EXPECT_EQ(0x2728, contents.light_information_list[2].min_time_to_change);
  EXPECT_EQ(0x292A, contents.light_information_list[2].max_time_to_change);
  EXPECT_EQ(0x2B, contents.light_information_list[3].main_light);
  EXPECT_EQ(0x2C, contents.light_information_list[3].arrow_light);
  EXPECT_EQ(0x2D2E, contents.light_information_list[3].min_time_to_change);
  EXPECT_EQ(0x2F30, contents.light_information_list[3].max_time_to_change);
  EXPECT_EQ(0x31, contents.light_information_list[4].main_light);
  EXPECT_EQ(0x32, contents.light_information_list[4].arrow_light);
  EXPECT_EQ(0x3334, contents.light_information_list[4].min_time_to_change);
  EXPECT_EQ(0x3536, contents.light_information_list[4].max_time_to_change);
  EXPECT_EQ(0x37, contents.light_information_list[5].main_light);
  EXPECT_EQ(0x38, contents.light_information_list[5].arrow_light);
  EXPECT_EQ(0x393A, contents.light_information_list[5].min_time_to_change);
  EXPECT_EQ(0x3B3C, contents.light_information_list[5].max_time_to_change);
  EXPECT_EQ(0x3D, contents.light_information_list[6].main_light);
  EXPECT_EQ(0x3E, contents.light_information_list[6].arrow_light);
  EXPECT_EQ(0x3F40, contents.light_information_list[6].min_time_to_change);
  EXPECT_EQ(0x4142, contents.light_information_list[6].max_time_to_change);
  EXPECT_EQ(0x43, contents.light_information_list[7].main_light);
  EXPECT_EQ(0x44, contents.light_information_list[7].arrow_light);
  EXPECT_EQ(0x4546, contents.light_information_list[7].min_time_to_change);
  EXPECT_EQ(0x4748, contents.light_information_list[7].max_time_to_change);
  EXPECT_EQ(0x49, contents.light_information_list[8].main_light);
  EXPECT_EQ(0x4A, contents.light_information_list[8].arrow_light);
  EXPECT_EQ(0x4B4C, contents.light_information_list[8].min_time_to_change);
  EXPECT_EQ(0x4D4E, contents.light_information_list[8].max_time_to_change);
  EXPECT_EQ(0x4F, contents.light_information_list[9].main_light);
  EXPECT_EQ(0x50, contents.light_information_list[9].arrow_light);
  EXPECT_EQ(0x5152, contents.light_information_list[9].min_time_to_change);
  EXPECT_EQ(0x5354, contents.light_information_list[9].max_time_to_change);
  EXPECT_EQ(0x55, contents.light_information_list[10].main_light);
  EXPECT_EQ(0x56, contents.light_information_list[10].arrow_light);
  EXPECT_EQ(0x5758, contents.light_information_list[10].min_time_to_change);
  EXPECT_EQ(0x595A, contents.light_information_list[10].max_time_to_change);
  EXPECT_EQ(0x5B, contents.light_information_list[11].main_light);
  EXPECT_EQ(0x5C, contents.light_information_list[11].arrow_light);
  EXPECT_EQ(0x5D5E, contents.light_information_list[11].min_time_to_change);
  EXPECT_EQ(0x5F60, contents.light_information_list[11].max_time_to_change);

  // teardown
  // nothing to do
}

/**
 * @brief 出力ストリームへの表示が正しくできること
 *
 * - 以下のような値を持つSignalInfoのインスタンスをストリームへわたす
 *
 *       crp_id: 1
 *       signal_id_list[]: {2, 3, 4, 5, 6, 7, 8, 9}
 *       time: 10
 *       state: 11
 *       specific_control_flags: 12
 *       event_count: 13
 *       countdown_stop_flag: 14
 *       light_information_list[0]: {15,16,17,18}
 *       light_information_list[1]: {19,20,21,22}
 *         ...
 *       light_information_list[11]: {59,60,61,62}
 *
 * - ストリームの内容が1,2,3, ... ,61,62,であること
 */
TEST(Cool4ApiDataclassSignalInfo, Print) {
  // setup
  SignalInfo target = SignalInfo();
  uint8_t i = 1;
  target.crp_id = i++;
  for (size_t j = 0; j < SignalInfo::SignalIdNum; ++j) {
    target.signal_id_list[j] = i++;
  }
  target.time = i++;
  target.state = i++;
  target.specific_control_flags = i++;
  target.event_count = i++;
  target.count_down_stop_flag = i++;
  for (size_t k = 0; k < SignalInfo::LightInformationNum; ++k) {
    target.light_information_list[k].main_light = i++;
    target.light_information_list[k].arrow_light = i++;
    target.light_information_list[k].min_time_to_change = i++;
    target.light_information_list[k].max_time_to_change = i++;
  }
  std::stringbuf buf;
  std::streambuf* prev = std::cout.rdbuf(&buf);

  // exercise
  std::cout << target;

  // verify
  EXPECT_EQ("1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,", buf.str());

  // teardown
  std::cout.rdbuf(prev);
}
