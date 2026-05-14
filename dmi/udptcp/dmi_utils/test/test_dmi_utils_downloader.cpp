/**
 * @file test_dmi_utils_downloader.cpp
 *
 * dmi_utils::Downloaderクラスのテスト
 */
#include "dmi_utils/downloader.hpp"
#include "dmi_utils/creator.hpp"
#include <cool4_api_dataclass/object_info.hpp>


#include <gtest/gtest.h>


using dmi_utils::Downloader;
using cool4_api_dataclass::FreespaceInfo;
using cool4_api_dataclass::ObjectInfo;
using dmi_utils::Creator;
using DataType = cool4_api_dataclass::ObjectInfo;

/**
 * @brief 引数ありコンストラクタを用いたインスタンス化を正常，異常に行うことができるかを検証する
 *
 * - DBsystemを起動しておくこと
 * - コンストラクタの実行を確認
 *
 */
// コンストラクタにて正常系(DBsystem起動時)を通る場合
TEST(DmiUtilsDownloader, Constructor_no_throw) {
  // setup
  ObjectInfo my_data;
  auto some_func = [&](const std::vector<ObjectInfo>& rs){
    my_data = rs[0];
  };

  // exercise
  // verify
  EXPECT_NO_THROW(Downloader<ObjectInfo>::SharedPtr target(
    new Downloader<ObjectInfo>({
      "127.0.0.1",
      9001,
      "dm2sampleuser",
      "dm2samplepassword",
      3,
      "master sysTimer100msec select * from object_info [range 300 msec]"},
      some_func)));

  // teardown
  // nothing to do
}

// コンストラクタにて異常系を通る場合
TEST(DmiUtilsDownloader, Constructor_any_throw) {
  // setup
  ObjectInfo my_data;
  auto some_func = [&](const std::vector<ObjectInfo>& rs){
    my_data = rs[0];
  };

  // exercise
  // verify
  EXPECT_ANY_THROW(Downloader<ObjectInfo>::SharedPtr target(
    new Downloader<ObjectInfo>({
      "127.0.0.1",
      9001,
      "dm2sampleuser",
      "",
      3,
      "master sysTimer100msec select * from object_info [range 300 msec]"},
      some_func)));

  // teardown
  // nothing to do
}

/**
 * @brief Uploadの実行の検証
 *
 * - DBsystemを起動しておくこと
 * - src/udp_dmi/sample/send_object_info.cppの値が、Downloaderのコールバック関数の引数に以下の値として入ってきていることを確認
 *
 *       id: 0x8000000012345678
 *       info_src_list[0]: 0x12345678
 *       length.value: 2500
 *       width.value: 1500
 *       height.value: 1500
 *       time: 時刻(固定値でないため確認対象外とする)
 *       他: コンストラクタ時と同様の値
 *
 */
TEST(DmiUtilsDownloader, Update) {
  // setup
  std::vector<DataType> my_data;
  auto some_func = [&](const std::vector<DataType>& rs){
    my_data = rs;
  };

  Downloader<DataType>::SharedPtr target(
    new Downloader<DataType>({
      "127.0.0.1",
      9001,
      "dm2sampleuser",
      "dm2samplepassword",
      3,
      "master sysTimer100msec select * from object_info_0_8_1 [range 300 msec]"},
      some_func));

  // exercise
  // DBsystem起動時に自動で読み込まれる関数のため宣言後しばらく待機
  int i = 0;
  while(my_data.size() == 0 && i < 30) {
    sleep(1);
    i++;
  }
  
  // verify
  ASSERT_NE(0, my_data.size());
  EXPECT_EQ(0x8000000012345678, my_data[0].id);
  // timeは一意の値に定まらないため除外
//  EXPECT_EQ(621513486032, my_data[0].time);
  EXPECT_EQ(0, my_data[0].object_classes[0].id);
  EXPECT_EQ(0, my_data[0].object_classes[0].confidence);
  EXPECT_EQ(0, my_data[0].object_classes[0].subclass_type);
  EXPECT_EQ(0, my_data[0].object_classes[0].subclass_confidence);
  EXPECT_EQ(0, my_data[0].object_classes[1].id);
  EXPECT_EQ(0, my_data[0].object_classes[1].confidence);
  EXPECT_EQ(0, my_data[0].object_classes[1].subclass_type);
  EXPECT_EQ(0, my_data[0].object_classes[1].subclass_confidence);
  EXPECT_EQ(0, my_data[0].object_classes[2].id);
  EXPECT_EQ(0, my_data[0].object_classes[2].confidence);
  EXPECT_EQ(0, my_data[0].object_classes[2].subclass_type);
  EXPECT_EQ(0, my_data[0].object_classes[2].subclass_confidence);
  EXPECT_EQ(0, my_data[0].object_classes[3].id);
  EXPECT_EQ(0, my_data[0].object_classes[3].confidence);
  EXPECT_EQ(0, my_data[0].object_classes[3].subclass_type);
  EXPECT_EQ(0, my_data[0].object_classes[3].subclass_confidence);
  EXPECT_EQ(0, my_data[0].existency);
  EXPECT_EQ(0, my_data[0].object_location.geodetic_srid);
  EXPECT_EQ(356895014, my_data[0].object_location.latitude);
  EXPECT_EQ(1396917336, my_data[0].object_location.longitude);
  EXPECT_EQ(800001, my_data[0].object_location.altitude);
  EXPECT_EQ(0, my_data[0].object_location.crp_id);
  EXPECT_EQ(-132768, my_data[0].object_location.dx_crp);
  EXPECT_EQ(-132768, my_data[0].object_location.dy_crp);
  EXPECT_EQ(-132768, my_data[0].object_location.dh_crp);
  EXPECT_EQ(15, my_data[0].object_location.lane_count);
  EXPECT_EQ(17, my_data[0].object_location.lane_position);
  EXPECT_EQ(101, my_data[0].object_location.lane_lateral_position);
  EXPECT_EQ(0, my_data[0].object_location.crp_id_begin);
  EXPECT_EQ(0, my_data[0].object_location.crp_id_end);
  EXPECT_EQ(10001, my_data[0].object_location.lane_vertical_position);
  EXPECT_EQ(0, my_data[0].object_location.lane_id);
  EXPECT_EQ(-132768, my_data[0].object_location.dx_lane);
  EXPECT_EQ(-132768, my_data[0].object_location.dy_lane);
  EXPECT_EQ(-132768, my_data[0].object_location.dh_lane);
  EXPECT_EQ(4095, my_data[0].object_location.semi_axis_length_major);
  EXPECT_EQ(4095, my_data[0].object_location.semi_axis_length_minor);
  EXPECT_EQ(36001, my_data[0].object_location.orientation);
  EXPECT_EQ(20001, my_data[0].object_location.altitude_accuracy);
  EXPECT_EQ(0, my_data[0].ref_point);
  EXPECT_EQ(36001, my_data[0].move_direction.value);
  EXPECT_EQ(9001, my_data[0].move_direction.accuracy);
  EXPECT_EQ(16383, my_data[0].speed.value);
  EXPECT_EQ(16383, my_data[0].speed.accuracy);
  EXPECT_EQ(32767, my_data[0].yaw_rate.value);
  EXPECT_EQ(32767, my_data[0].yaw_rate.accuracy);
  EXPECT_EQ(2001, my_data[0].acceleration.value);
  EXPECT_EQ(1001, my_data[0].acceleration.accuracy);
  EXPECT_EQ(36001, my_data[0].orientation.value);
  EXPECT_EQ(9001, my_data[0].orientation.accuracy);
  EXPECT_EQ(2500, my_data[0].length.value);
  EXPECT_EQ(65535, my_data[0].length.accuracy);
  EXPECT_EQ(1500, my_data[0].width.value);
  EXPECT_EQ(65535, my_data[0].width.accuracy);
  EXPECT_EQ(1500, my_data[0].height.value);
  EXPECT_EQ(65535, my_data[0].height.accuracy);
  // EXPECT_EQ(0, my_data[0].color);
  EXPECT_EQ(7, my_data[0].shift_position);
  EXPECT_EQ(91, my_data[0].steering_angle_front);
  EXPECT_EQ(91, my_data[0].steering_angle_rear);
  EXPECT_EQ(0, my_data[0].brake_state);
  EXPECT_EQ(0, my_data[0].auxiliary_brake_state);
  EXPECT_EQ(255, my_data[0].throttle_position);
  EXPECT_EQ(0, my_data[0].exterior_lights);
  EXPECT_EQ(0, my_data[0].acc_state);
  EXPECT_EQ(0, my_data[0].c_acc_state);
  EXPECT_EQ(0, my_data[0].pcs_state);
  EXPECT_EQ(0, my_data[0].abs_state);
  EXPECT_EQ(0, my_data[0].trc_state);
  EXPECT_EQ(0, my_data[0].esc_state);
  EXPECT_EQ(0, my_data[0].lka_state);
  EXPECT_EQ(0, my_data[0].ldw_state);
  EXPECT_EQ(15, my_data[0].vehicle_role);
  EXPECT_EQ(255, my_data[0].vehicle_extended_info);
  EXPECT_EQ(0, my_data[0].towing_vehicle);
  EXPECT_EQ(0x12345678, my_data[0].info_src_list[0]);
  EXPECT_EQ(0, my_data[0].info_src_list[1]);
  EXPECT_EQ(0, my_data[0].info_src_list[2]);
  EXPECT_EQ(0, my_data[0].info_src_list[3]);

  // teardown
  // nothing to do
}
