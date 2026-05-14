/**
 * @file test_dmi_utils_dm_bridge.cpp
 *
 * dmi_utils::DmBridgeクラスのテスト
 */
#include <is/Tuple.h>
#include <is/ResultSet.h>
#include <cstdlib>

#include "dmi_utils/dm_bridge.hpp"
#include "resultset_f.hpp"
#include "resultset_o.hpp"
#include "resultset_p.hpp"
#include "resultset_s.hpp"

#include <gtest/gtest.h>


using dmi_utils::DmBridge;
using cool4_api_dataclass::ObjectClass;
using cool4_api_dataclass::ValueWithAccuracy;
using cool4_api_dataclass::ObjectInfo;
using cool4_api_dataclass::FreespaceInfo;
using cool4_api_dataclass::LightInformation;
using cool4_api_dataclass::SignalInfo;

/**
 * @brief ObjectClassからTupleへの変換処理が正しく機能するかを検証する
 *
 * - 以下のような値を与えたとき
 *
 *       id: 1
 *       confidence: 2
 *       subclass_type: 3
 *       subclass_confidence: 4
 *
 * - Tupleが以下となること
 *
 *       values[0]: 1
 *       values[1]: 2
 *       values[2]: 3
 *       values[3]: 4
 *
 */
TEST(DmiUtlsDmBridge, FromDataclass_ObjectClass) {
  // setup
  ObjectClass input = ObjectClass();
  input.id = 1;
  input.confidence = 2;
  input.subclass_type = 3;
  input.subclass_confidence = 4;
  IS::Tuple out = IS::Tuple(4);
  uint32_t idx = 0;
  const long stamp = 0;

  // exercise
  DmBridge<ObjectClass>::FromDataclass(input, stamp, idx, out);

  // verify
  any val = 0;
  long timestamp = 0;
  for(int i = 0; i < 4; i++) {
    out.getValue(i, val, timestamp);
    EXPECT_EQ(i+1, std::experimental::any_cast<int>(val));
    EXPECT_EQ(0, timestamp);
  }

  // teardown
  // nothing to do
}

/**
 * @brief LocationからTupleへの変換処理が正しく機能するかを検証する
 *
 * - 以下の値を与えたとき
 *
 *       geodetic_srid: 1
 *       latitude: 2
 *       longitude: 3
 *       altitude: 4
 *       crp_id: 5
 *       dx_crp: 6
 *       dy_crp: 7
 *       dh_crp: 8
 *       lane_count: 9
 *       lane_position: 10
 *       lane_lateral_position: 11
 *       crp_id_begin: 12
 *       crp_id_end: 13
 *       lane_vertical_position: 14
 *       lane_id: 15
 *       dx_lane: 16
 *       dy_lane: 17
 *       dh_lane: 18
 *       semi_axis_length_major: 19
 *       semi_axis_length_minor: 20
 *       orientation: 21
 *       altitude_accuracy: 22
 *
 * - Tupleが以下の値となること
 *
 *       values[0]: 1
 *       values[1]: 2
 *       values[2]: 3
 *       values[3]: 4
 *       values[4]: 5
 *       values[5]: 6
 *       values[6]: 7
 *       values[7]: 8
 *       values[8]: 9
 *       values[9]: 10
 *       values[10]: 11
 *       values[11]: 12
 *       values[12]: 13
 *       values[13]: 14
 *       values[14]: "000000000000000F"
 *       values[15]: 16
 *       values[16]: 17
 *       values[17]: 18
 *       values[18]: 19
 *       values[19]: 20
 *       values[20]: 21
 *       values[21]: 22
 *
 */
TEST(DmiUtlsDmBridge, FromDataclass_Location) {
  // setup
  cool4_api_dataclass::Location input = cool4_api_dataclass::Location();
  int value = 0;
  input.geodetic_srid = ++value;
  input.latitude = ++value;
  input.longitude = ++value;
  input.altitude = ++value;
  input.projective_srid = ++value;
  input.prc_x = ++value;
  input.prc_y = ++value;
  input.prc_z = ++value;
  input.crp_id = ++value;
  input.dx_crp = ++value;
  input.dy_crp = ++value;
  input.dh_crp = ++value;
  input.lane_count = ++value;
  input.lane_position = ++value;
  input.lane_lateral_position = ++value;
  input.crp_id_begin = ++value;
  input.crp_id_end = ++value;
  input.lane_vertical_position = ++value;
  input.lane_id = ++value;
  input.dx_lane = ++value;
  input.dy_lane = ++value;
  input.dh_lane = ++value;
  input.semi_axis_length_major = ++value;
  input.semi_axis_length_minor = ++value;
  input.orientation = ++value;
  input.altitude_accuracy = ++value;
  IS::Tuple out = IS::Tuple(26);
  uint32_t idx = 0;
  const long stamp = 0;

  // exercise
  DmBridge<cool4_api_dataclass::Location>::FromDataclass(input, stamp, idx, out);

  // verify
  any val;
  long timestamp = 0;
  for(int i = 0; i < 26; i++) {
    out.getValue(i, val, timestamp);
    if (i == 18) {
      EXPECT_EQ(i+1, std::experimental::any_cast<unsigned long long>(val));
    } else {
      EXPECT_EQ(i+1, std::experimental::any_cast<int>(val));
    }
    EXPECT_EQ(0, timestamp);
  }

  // teardown
  // nothing to do
}

/**
 * @brief ValueWithAccuracy<uint16_t,uint16_t>からTupleへの変換処理が正しく機能するかを検証する
 *
 * - 以下の値を与えたとき
 *
 *       value: 1
 *       accuracy: 2
 *
 * - Tupleが以下となること
 *
 *       values[0]: 1
 *       values[1]: 2
 *
 */
TEST(DmiUtlsDmBridge, FromDataclass_ValueWithAccuracy_uint16_t_uint16_t) {
  // setup
  ValueWithAccuracy<uint16_t, uint16_t> input = ValueWithAccuracy<uint16_t, uint16_t>(1,2);
  IS::Tuple out = IS::Tuple(2);
  uint32_t idx = 0;
  const long stamp = 0;

  // exercise
  DmBridge<ValueWithAccuracy<uint16_t,uint16_t>>::FromDataclass(input, stamp, idx, out);

  // verify
  any val;
  long timestamp = 0;
  for(int i = 0; i < 2; i++) {
    out.getValue(i, val, timestamp);
    EXPECT_EQ(i+1, std::experimental::any_cast<int>(val));
    EXPECT_EQ(0, timestamp);
  }

  // teardown
  // nothing to do
}

/**
 * @brief ValueWithAccuracy<int16_t,uint16_t>からTupleへの変換処理が正しく機能するかを検証する
 *
 * - 以下の値を与えたとき
 *
 *       value: 1
 *       accuracy: 2
 *
 * - Tupleが以下となること
 *
 *       values[0]: 1
 *       values[1]: 2
 *
 */
TEST(DmiUtlsDmBridge, FromDataclass_ValueWithAccuracy_int16_t_uint16_t) {
  // setup
  ValueWithAccuracy<int16_t, uint16_t> input = ValueWithAccuracy<int16_t, uint16_t>(1,2);
  IS::Tuple out = IS::Tuple(2);
  uint32_t idx = 0;
  const long stamp = 0;

  // exercise
  DmBridge<ValueWithAccuracy<int16_t,uint16_t>>::FromDataclass(input, stamp, idx, out);

  // verify
  any val;
  long timestamp = 0;
  out.getValue(0, val, timestamp);
  for(int i = 0; i < 2; i++) {
    out.getValue(i, val, timestamp);
    EXPECT_EQ(i+1, std::experimental::any_cast<int>(val));
    EXPECT_EQ(0, timestamp);
  }

  // teardown
  // nothing to do
}

/**
 * @brief ObjectInfoからTupleへの変換処理が正しく機能するかを検証する
 *
 * - 以下の値を与えたとき
 *
 *       id: 1
 *       time: 2
 *       object_classes[0].id: 3
 *       object_classes[0].confidence: 4
 *       object_classes[0].subclass_type: 5
 *       object_classes[0].subclass_confidence: 6
 *       object_classes[1].id: 7
 *       object_classes[1].confidence: 8
 *       object_classes[1].subclass_type: 9
 *       object_classes[1].subclass_confidence: 10
 *       object_classes[2].id: 11
 *       object_classes[2].confidence: 12
 *       object_classes[2].subclass_type: 13
 *       object_classes[2].subclass_confidence: 14
 *       object_classes[3].id: 15
 *       object_classes[3].confidence: 16
 *       object_classes[3].subclass_type: 17
 *       object_classes[3].subclass_confidence: 18
 *       existency: 19
 *       object_location.geodetic_srid: 20
 *       object_location.latitude: 21
 *       object_location.longitude: 22
 *       object_location.altitude: 23
 *       object_location.crp_id: 24
 *       object_location.dx_crp: 25
 *       object_location.dy_crp: 26
 *       object_location.dh_crp: 27
 *       object_location.lane_count: 28
 *       object_location.lane_position: 29
 *       object_location.lane_lateral_position: 30
 *       object_location.crp_id_begin: 31
 *       object_location.crp_id_end: 32
 *       object_location.lane_vertical_position: 33
 *       object_location.lane_id: 34
 *       object_location.dx_lane: 35
 *       object_location.dy_lane: 36
 *       object_location.dh_lane: 37
 *       object_location.semi_axis_length_major: 38
 *       object_location.semi_axis_length_minor: 39
 *       object_location.orientation: 40
 *       object_location.altitude_accuracy: 41
 *       ref_point: 42
 *       move_direction: {43,44}
 *       speed: {45,46}
 *       yaw_rate: {47,48}
 *       acceleration: {49,50}
 *       orientation: {51,52}
 *       length: {53,54}
 *       width: {55,56}
 *       height: {57,58}
 *       color: 59
 *       shift_position: 60
 *       steering_angle_front: 61
 *       steering_angle_rear: 62
 *       brake_state: 63
 *       auxiliary_brake_state: 64
 *       throttle_position: 65
 *       exterior_lights: 66
 *       acc_state: 67
 *       c_acc_state: 68
 *       pcs_state: 69
 *       abs_state: 70
 *       trc_state: 71
 *       esc_state: 72
 *       lka_state: 73
 *       ldw_state: 74
 *       vehicle_role: 75
 *       vehicle_extended_info: 76
 *       towing_vehicle: 77
 *       info_src_list: {78,79,80,81}
 *
 * - Tupleが以下となること
 *
 *       values[0]: "0000000000000001"
 *       values[1]: 2
 *        ...
 *       values[32]: 33
 *       values[33]: "0000000000000022"
 *       values[34]: 35
 *        ...
 *       values[75]: 76
 *       values[76]: "000000000000004D"
 *       values[77]: ["000000000000004E","000000000000004F","0000000000000050","0000000000000051"]
 *
 */
TEST(DmiUtlsDmBridge, FromDataclass_ObjectInfo) {
  // setup
  int value = 0;
  ObjectInfo input = ObjectInfo();
  input.id = ++value;
  input.time = ++value;
  input.revision = ++value;
  input.object_classes[0].id = ++value;
  input.object_classes[0].confidence = ++value;
  input.object_classes[0].subclass_type = ++value;
  input.object_classes[0].subclass_confidence = ++value;
  input.object_classes[1].id = ++value;
  input.object_classes[1].confidence = ++value;
  input.object_classes[1].subclass_type = ++value;
  input.object_classes[1].subclass_confidence = ++value;
  input.object_classes[2].id = ++value;
  input.object_classes[2].confidence = ++value;
  input.object_classes[2].subclass_type = ++value;
  input.object_classes[2].subclass_confidence = ++value;
  input.object_classes[3].id = ++value;
  input.object_classes[3].confidence = ++value;
  input.object_classes[3].subclass_type = ++value;
  input.object_classes[3].subclass_confidence = ++value;
  input.existency = ++value;
  input.object_location.geodetic_srid = ++value;
  input.object_location.latitude = ++value;
  input.object_location.longitude = ++value;
  input.object_location.altitude = ++value;
  input.object_location.projective_srid = ++value;
  input.object_location.prc_x = ++value;
  input.object_location.prc_y = ++value;
  input.object_location.prc_z = ++value;
  input.object_location.crp_id = ++value;
  input.object_location.dx_crp = ++value;
  input.object_location.dy_crp = ++value;
  input.object_location.dh_crp = ++value;
  input.object_location.lane_count = ++value;
  input.object_location.lane_position = ++value;
  input.object_location.lane_lateral_position = ++value;
  input.object_location.crp_id_begin = ++value;
  input.object_location.crp_id_end = ++value;
  input.object_location.lane_vertical_position = ++value;
  input.object_location.lane_id = ++value;
  input.object_location.dx_lane = ++value;
  input.object_location.dy_lane = ++value;
  input.object_location.dh_lane = ++value;
  input.object_location.semi_axis_length_major = ++value;
  input.object_location.semi_axis_length_minor = ++value;
  input.object_location.orientation = ++value;
  input.object_location.altitude_accuracy = ++value;
  input.ref_point = ++value;
  input.move_direction = {++value,++value};
  input.speed = {++value,++value};
  input.yaw_rate = {++value,++value};
  input.acceleration = {++value,++value};
  input.orientation = {++value,++value};
  input.length = {++value,++value};
  input.width = {++value,++value};
  input.height = {++value,++value};
  input.static_status = ++value;
  input.shift_position = ++value;
  input.steering_angle_front = ++value;
  input.steering_angle_rear = ++value;
  input.brake_state = ++value;
  input.auxiliary_brake_state = ++value;
  input.throttle_position = ++value;
  input.exterior_lights = ++value;
  input.acc_state = ++value;
  input.c_acc_state = ++value;
  input.pcs_state = ++value;
  input.abs_state = ++value;
  input.trc_state = ++value;
  input.esc_state = ++value;
  input.lka_state = ++value;
  input.ldw_state = ++value;
  input.vehicle_size_type = ++value;
  input.vehicle_role = ++value;
  input.vehicle_extended_info = ++value;
  input.towing_vehicle = ++value;
  input.tracking_status = ++value;
  input.detection_count = ++value;
  input.lost_count = ++value;
  input.object_age = ++value;
  input.info_src_list = {++value,++value,++value,++value};
  IS::Tuple out = IS::Tuple(88);
  uint32_t idx = 0;
  const long stamp = 0;

  // exercise
  DmBridge<ObjectInfo>::FromDataclass(input, stamp, idx, out);

  // verify
  any val;
  long timestamp = 0;
  out.getValue(0, val, timestamp);
  EXPECT_EQ(1, std::experimental::any_cast<unsigned long long>(val));

  out.getValue(1, val, timestamp);
  EXPECT_EQ(2, std::experimental::any_cast<long>(val));

  out.getValue(87, val, timestamp);
  std::vector<unsigned long long> val1 = std::experimental::any_cast<std::vector<unsigned long long>>(val);
  EXPECT_EQ(88, val1[0]);

  // teardown
  // nothing to do
}

#ifdef FS
/**
 * @brief FreespaceInfoからTupleへの変換処理が正しく機能するかを検証する
 *
 * - 以下の値を与えたとき
 *
 *       id: 1
 *       time: 2
 *       existency: 3
 *       minimal_detectable_size: 4
 *       position_begin.geodetic_srid: 5
 *       position_begin.latitude: 6
 *       position_begin.longitude: 7
 *       position_begin.altitude: 8
 *       position_begin.crp_id: 9
 *       position_begin.dx_crp: 10
 *       position_begin.dy_crp: 11
 *       position_begin.dh_crp: 12
 *       position_begin.lane_count: 13
 *       position_begin.lane_position: 14
 *       position_begin.lane_lateral_position: 15
 *       position_begin.crp_id_begin: 16
 *       position_begin.crp_id_end: 17
 *       position_begin.lane_vertical_position: 18
 *       position_begin.lane_id: 19
 *       position_begin.dx_lane: 20
 *       position_begin.dy_lane: 21
 *       position_begin.dh_lane: 22
 *       position_begin.semi_axis_length_major: 23
 *       position_begin.semi_axis_length_minor: 24
 *       position_begin.orientation: 25
 *       position_begin.altitude_accuracy: 26
 *       position_end.geodetic_srid: 27
 *       position_end.latitude: 28
 *       position_end.longitude: 29
 *       position_end.altitude: 30
 *       position_end.crp_id: 31
 *       position_end.dx_crp: 32
 *       position_end.dy_crp: 33
 *       position_end.dh_crp: 34
 *       position_end.lane_count: 35
 *       position_end.lane_position: 36
 *       position_end.lane_lateral_position: 37
 *       position_end.crp_id_begin: 38
 *       position_end.crp_id_end: 39
 *       position_end.lane_vertical_position: 40
 *       position_end.lane_id: 41
 *       position_end.dx_lane: 42
 *       position_end.dy_lane: 43
 *       position_end.dh_lane: 44
 *       position_end.semi_axis_length_major: 45
 *       position_end.semi_axis_length_minor: 46
 *       position_end.orientation: 47
 *       position_end.altitude_accuracy: 48
 *       length: {49,50}
 *       id_begin: 51
 *       id_end: 52
 *       info_src_list: {53,54,55,56}
 *
 * - Tupleが以下となること
 *
 *       values[0]: "0000000000000001"
 *       values[1]: 2
 *        ...
 *       values[17]: 18
 *       values[18]: "0000000000000013"
 *       values[19]: 20
 *        ...
 *       values[39]: 40
 *       values[40]: "0000000000000029"
 *       values[41]: 42
 *        ...
 *       values[49]: 50
 *       values[50]: "0000000000000033"
 *       values[51]: "0000000000000034"
 *       values[52]: ["0000000000000035","0000000000000036","0000000000000037","0000000000000038"]
 *
 */
TEST(DmiUtlsDmBridge, FromDataclass_FreespaceInfo) {
  // setup
  FreespaceInfo input = FreespaceInfo();
  input.id = 1;
  input.time = 2;
  input.existency = 3;
  input.minimal_detectable_size = 4;
  input.position_begin.geodetic_srid = 5;
  input.position_begin.latitude = 6;
  input.position_begin.longitude = 7;
  input.position_begin.altitude = 8;
  input.position_begin.crp_id = 9;
  input.position_begin.dx_crp = 10;
  input.position_begin.dy_crp = 11;
  input.position_begin.dh_crp = 12;
  input.position_begin.lane_count = 13;
  input.position_begin.lane_position = 14;
  input.position_begin.lane_lateral_position = 15;
  input.position_begin.crp_id_begin = 16;
  input.position_begin.crp_id_end = 17;
  input.position_begin.lane_vertical_position = 18;
  input.position_begin.lane_id = 19;
  input.position_begin.dx_lane = 20;
  input.position_begin.dy_lane = 21;
  input.position_begin.dh_lane = 22;
  input.position_begin.semi_axis_length_major = 23;
  input.position_begin.semi_axis_length_minor = 24;
  input.position_begin.orientation = 25;
  input.position_begin.altitude_accuracy = 26;
  input.position_end.geodetic_srid = 27;
  input.position_end.latitude = 28;
  input.position_end.longitude = 29;
  input.position_end.altitude = 30;
  input.position_end.crp_id = 31;
  input.position_end.dx_crp = 32;
  input.position_end.dy_crp = 33;
  input.position_end.dh_crp = 34;
  input.position_end.lane_count = 35;
  input.position_end.lane_position = 36;
  input.position_end.lane_lateral_position = 37;
  input.position_end.crp_id_begin = 38;
  input.position_end.crp_id_end = 39;
  input.position_end.lane_vertical_position = 40;
  input.position_end.lane_id = 41;
  input.position_end.dx_lane = 42;
  input.position_end.dy_lane = 43;
  input.position_end.dh_lane = 44;
  input.position_end.semi_axis_length_major = 45;
  input.position_end.semi_axis_length_minor = 46;
  input.position_end.orientation = 47;
  input.position_end.altitude_accuracy = 48;
  input.length = {49,50};
  input.id_begin = 51;
  input.id_end = 52;
  input.info_src_list = {53,54,55,56};
  IS::Tuple out = IS::Tuple(56);
  uint32_t idx = 0;
  const long stamp = 0;

  // exercise
  DmBridge<FreespaceInfo>::FromDataclass(input, stamp, idx, out);

  // verify
  any val;
  long timestamp = 0;

  out.getValue(0, val, timestamp);
  EXPECT_EQ("0000000000000001", std::experimental::any_cast<string>(val));
  EXPECT_EQ(0, timestamp);
 
  out.getValue(1, val, timestamp);
  EXPECT_EQ(2, std::experimental::any_cast<long>(val));
  EXPECT_EQ(0, timestamp);

  for(int i = 2; i < 18; i++) {
    out.getValue(i, val, timestamp);
    EXPECT_EQ(i+1, std::experimental::any_cast<int>(val));
    EXPECT_EQ(0, timestamp);
  }

  out.getValue(18, val, timestamp);
  EXPECT_EQ("0000000000000013", std::experimental::any_cast<string>(val));
  EXPECT_EQ(0, timestamp);

  for(int i = 19; i < 40; i++) {
    out.getValue(i, val, timestamp);
    EXPECT_EQ(i+1, std::experimental::any_cast<int>(val));
    EXPECT_EQ(0, timestamp);
  }

  out.getValue(40, val, timestamp);
  EXPECT_EQ("0000000000000029", std::experimental::any_cast<string>(val));
  EXPECT_EQ(0, timestamp);

  for(int i = 41; i < 50; i++) {
    out.getValue(i, val, timestamp);
    EXPECT_EQ(i+1, std::experimental::any_cast<int>(val));
    EXPECT_EQ(0, timestamp);
  }

  out.getValue(50, val, timestamp);
  EXPECT_EQ("0000000000000033", std::experimental::any_cast<string>(val));
  EXPECT_EQ(0, timestamp);
  
  out.getValue(51, val, timestamp);
  EXPECT_EQ("0000000000000034", std::experimental::any_cast<string>(val));
  EXPECT_EQ(0, timestamp);
  
  out.getValue(52, val, timestamp);
  std::vector<string> val1 = std::experimental::any_cast<std::vector<string>>(val);
  EXPECT_EQ("0000000000000035", val1[0]);
  EXPECT_EQ("0000000000000036", val1[1]);
  EXPECT_EQ("0000000000000037", val1[2]);
  EXPECT_EQ("0000000000000038", val1[3]);
  EXPECT_EQ(0, timestamp);

  // teardown
  // nothing to do
}
#endif

/**
 * @brief LightInformationからTupleへの変換処理が正しく機能するかを検証する
 *
 * - {1,2,3,4}を与えたときTupleが以下となること
 *
 *       values[0]: 1
 *       values[1]: 2
 *       values[2]: 3
 *       values[3]: 4
 *
 */
TEST(DmiUtlsDmBridge, FromDataclass_LightInformation) {
  // setup
  LightInformation input = LightInformation(1,2,3,4);
  IS::Tuple out = IS::Tuple(4);
  uint32_t idx = 0;
  const long stamp = 0;

  // exercise
  DmBridge<LightInformation>::FromDataclass(input, stamp, idx, out);

  // verify
  any val;
  long timestamp;
  for(int i = 0; i < 4; i++) {
    out.getValue(i, val, timestamp);
    EXPECT_EQ(i+1, std::experimental::any_cast<int>(val));
    EXPECT_EQ(0, timestamp);
  }

  // teardown
  // nothing to do
}

/**
 * @brief SignalInfoからTupleへの変換処理が正しく機能するかを検証する
 *
 * - {1,2,3,4, ... ,62}を与えたときTupleが{1,2,3,4, ... ,62}となること
 *
 */
TEST(DmiUtlsDmBridge, FromDataclass_SignalInfo) {
  // setup
  SignalInfo input;
  input.crp_id = 1;
  input.signal_id_list = {2,3,4,5,6,7,8,9};
  input.time = 10;
  input.state = 11;
  input.specific_control_flags = 12;
  input.event_count = 13;
  input.count_down_stop_flag = 14;
  input.light_information_list[0].main_light = 15;
  input.light_information_list[0].arrow_light = 16;
  input.light_information_list[0].min_time_to_change = 17;
  input.light_information_list[0].max_time_to_change = 18;
  input.light_information_list[1].main_light = 19;
  input.light_information_list[1].arrow_light = 20;
  input.light_information_list[1].min_time_to_change = 21;
  input.light_information_list[1].max_time_to_change = 22;
  input.light_information_list[2].main_light = 23;
  input.light_information_list[2].arrow_light = 24;
  input.light_information_list[2].min_time_to_change = 25;
  input.light_information_list[2].max_time_to_change = 26;
  input.light_information_list[3].main_light = 27;
  input.light_information_list[3].arrow_light = 28;
  input.light_information_list[3].min_time_to_change = 29;
  input.light_information_list[3].max_time_to_change = 30;
  input.light_information_list[4].main_light = 31;
  input.light_information_list[4].arrow_light = 32;
  input.light_information_list[4].min_time_to_change = 33;
  input.light_information_list[4].max_time_to_change = 34;
  input.light_information_list[5].main_light = 35;
  input.light_information_list[5].arrow_light = 36;
  input.light_information_list[5].min_time_to_change = 37;
  input.light_information_list[5].max_time_to_change = 38;
  input.light_information_list[6].main_light = 39;
  input.light_information_list[6].arrow_light = 40;
  input.light_information_list[6].min_time_to_change = 41;
  input.light_information_list[6].max_time_to_change = 42;
  input.light_information_list[7].main_light = 43;
  input.light_information_list[7].arrow_light = 44;
  input.light_information_list[7].min_time_to_change = 45;
  input.light_information_list[7].max_time_to_change = 46;
  input.light_information_list[8].main_light = 47;
  input.light_information_list[8].arrow_light = 48;
  input.light_information_list[8].min_time_to_change = 49;
  input.light_information_list[8].max_time_to_change = 50;
  input.light_information_list[9].main_light = 51;
  input.light_information_list[9].arrow_light = 52;
  input.light_information_list[9].min_time_to_change = 53;
  input.light_information_list[9].max_time_to_change = 54;
  input.light_information_list[10].main_light = 55;
  input.light_information_list[10].arrow_light = 56;
  input.light_information_list[10].min_time_to_change = 57;
  input.light_information_list[10].max_time_to_change = 58;
  input.light_information_list[11].main_light = 59;
  input.light_information_list[11].arrow_light = 60;
  input.light_information_list[11].min_time_to_change = 61;
  input.light_information_list[11].max_time_to_change = 62;
  IS::Tuple out = IS::Tuple(62);
  uint32_t idx = 0;
  const long stamp = 0;

  // exercise
  DmBridge<SignalInfo>::FromDataclass(input, stamp, idx, out);

  // verify
  any val;
  long timestamp = 0;

  out.getValue(0, val, timestamp);
  EXPECT_EQ(1, std::experimental::any_cast<int>(val));
  EXPECT_EQ(0, timestamp);

  out.getValue(1, val, timestamp);
  std::vector<int> val1 = std::experimental::any_cast<std::vector<int>>(val);
  EXPECT_EQ(2, val1[0]);
  EXPECT_EQ(3, val1[1]);
  EXPECT_EQ(4, val1[2]);
  EXPECT_EQ(5, val1[3]);
  EXPECT_EQ(6, val1[4]);
  EXPECT_EQ(7, val1[5]);
  EXPECT_EQ(8, val1[6]);
  EXPECT_EQ(9, val1[7]);
  EXPECT_EQ(0, timestamp);

  out.getValue(2, val, timestamp);
  EXPECT_EQ(10, std::experimental::any_cast<long>(val));
  EXPECT_EQ(0, timestamp);

  for(int i = 3; i < 54; i++) {
    out.getValue(i, val, timestamp);
    EXPECT_EQ(i+8, std::experimental::any_cast<int>(val));
    EXPECT_EQ(0, timestamp);
  }

  // teardown
  // nothing to do
}

/**
 * @brief ResultSetからObjectClassの取り出しが正しく機能するかを検証する
 *
 * - CreateResultSet_oで定義したmapの値が以下の通り出力されることを確認する
 *
 *       id: 0x1
 *       confidence: 2
 *       subclass_type: 3
 *       subclass_confidence: 4
 *
 */
TEST(DmiUtlsDmBridge, FromResultSet_ObjectClass) {
  // setup
  test_util::CandidateTestCase_o data = test_util::CandidateTestCase_o();
  ResultSet input = test_util::CreateResultSet_o(data);
  input.next();

  // exercise
  auto out = dmi_utils::DmBridge<ObjectClass>::FromResultSet(input);

  // verify
  EXPECT_TRUE(out != boost::none);
  ObjectClass out1 = out.get();
  EXPECT_EQ(0x1, out1.id);
  EXPECT_EQ(2, out1.confidence);
  EXPECT_EQ(3, out1.subclass_type);
  EXPECT_EQ(4, out1.subclass_confidence);

  // teardown
  // nothing to do
}


/**
 * @brief ResultSetからValueWithAccuracy<uint16_t, uint16_t>の取り出しが正しく機能するかを検証する
 *
 * - CreateResultSet_oで定義したmapの値が以下の通り出力されることを確認する
 *
 *       move_direction.value: 57920
 *       move_direction.accuracy: 2000
 *
 */
TEST(DmiUtlsDmBridge, FromResultSet_ValueWithAccuracy_uint16_uint16) {
  // setup
  test_util::CandidateTestCase_o data = test_util::CandidateTestCase_o();
  ResultSet input = test_util::CreateResultSet_o(data);
  input.next();

  // exercise
  auto out = dmi_utils::DmBridge<ValueWithAccuracy<uint16_t, uint16_t>>::FromResultSet(input, "move_direction").get();

  // verify
  EXPECT_EQ(57920, out.value);
  EXPECT_EQ(2000, out.accuracy);

  // teardown
  // nothing to do
}

/**
 * @brief ResultSetからValueWithAccuracy<int16_t, uint16_t>の取り出しが正しく機能するかを検証する
 *
 * - CreateResultSet_oで定義したmapの値が以下の通り出力されることを確認する
 *
 *       acceleration.value: 3254
 *       acceleration.accuracy: 12541
 *
 */
TEST(DmiUtlsDmBridge, FromResultSet_ValueWithAccuracy_int16_uint16) {
  // setup
  test_util::CandidateTestCase_o data = test_util::CandidateTestCase_o();
  ResultSet input = test_util::CreateResultSet_o(data);
  input.next();

  // exercise
  auto out = dmi_utils::DmBridge<ValueWithAccuracy<int16_t, uint16_t>>::FromResultSet(input, "acceleration").get();

  // verify
  EXPECT_EQ(3254, out.value);
  EXPECT_EQ(12541, out.accuracy);

  // teardown
  // nothing to do
}

/**
 * @brief ResultSetからObjectInfoの取り出しが正しく機能するかを検証する
 *
 * - CreateResultSet_oで定義したmapの値が以下の通り出力されることを確認する
 *
 *       id: 0x12341234
 *       time: 5000000
 *       object_classes[0].id: 1
 *       object_classes[0].confidence: 2
 *       object_classes[0].subclass_type: 3
 *       object_classes[0].subclass_confidence: 4
 *       object_classes[1].id: 46
 *       object_classes[1].confidence: 136
 *       object_classes[1].subclass_type: 23
 *       object_classes[1].subclass_confidence: 69
 *       object_classes[2].id: 55
 *       object_classes[2].confidence: 66
 *       object_classes[2].subclass_type: 77
 *       object_classes[2].subclass_confidence: 88
 *       object_classes[3].id: 243
 *       object_classes[3].confidence: 242
 *       object_classes[3].subclass_type: 98
 *       object_classes[3].subclass_confidence: 210
 *       existency: 105
 *       object_location.geodetic_srid: 3322
 *       object_location.latitude: 235
 *       object_location.longitude: 666
 *       object_location.altitude: 987
 *       object_location.crp_id: 12343
 *       object_location.dx_crp: 500000
 *       object_location.dy_crp: 186
 *       object_location.dh_crp: 6688
 *       object_location.lane_count: 108
 *       object_location.lane_position: 8
 *       object_location.lane_lateral_position: 202
 *       object_location.crp_id_begin: 9458
 *       object_location.crp_id_end: 60000
 *       object_location.lane_vertical_position: 12347
 *       object_location.lane_id: 0x3254
 *       object_location.dx_lane: 4758
 *       object_location.dy_lane: 5689
 *       object_location.dh_lane: 9865
 *       object_location.semi_axis_length_major: 11200
 *       object_location.semi_axis_length_minor: 22011
 *       object_location.orientation: 3547
 *       object_location.altitude_accuracy: 6999
 *       ref_point: 6
 *       move_direction.value: 57920
 *       move_direction.accuracy: 2000
 *       speed.value: 3456
 *       speed.accuracy: 35779
 *       yaw_rate.value: 1596
 *       yaw_rate.accuracy: 3245
 *       acceleration.value: 3254
 *       acceleration.accuracy: 12541
 *       orientation.value: 9988
 *       orientation.accuracy: 6342
 *       length.value: 8426
 *       length.accuracy: 5236
 *       width.value: 7845
 *       width.accuracy: 6541
 *       height.value: 9019
 *       height.accuracy: 4321
 *       color: 30
 *       shift_position: 40
 *       steering_angle_front: 50
 *       steering_angle_rear: 60
 *       brake_state: 70
 *       auxiliary_brake_state: 255
 *       throttle_position: 90
 *       exterior_lights: 110
 *       acc_state: 119
 *       c_acc_state: 180
 *       pcs_state: 24
 *       abs_state: 114
 *       trc_state: 128
 *       esc_state: 230
 *       lka_state: 212
 *       ldw_state: 204
 *       vehicle_role: 176
 *       vehicle_extended_info: 236
 *       towing_vehicle: 0x630000
 *       info_src_list[0]: 0x56785678
 *       info_src_list[1]: 0
 *       info_src_list[2]: 0
 *       info_src_list[3]: 0
 *
 */
TEST(DmiUtlsDmBridge, FromResultSet_ObjectInfo) {
  // setup
  test_util::CandidateTestCase_o data = test_util::CandidateTestCase_o();
  ResultSet input = test_util::CreateResultSet_o(data);
  input.next();

  // exercise
  auto out = dmi_utils::DmBridge<ObjectInfo>::FromResultSet(input).get();

  // verify
  EXPECT_EQ(12341234, out.id);
  EXPECT_EQ(5000000, out.time);
  EXPECT_EQ(1, out.object_classes[0].id);
  EXPECT_EQ(2, out.object_classes[0].confidence);
  EXPECT_EQ(3, out.object_classes[0].subclass_type);
  EXPECT_EQ(4, out.object_classes[0].subclass_confidence);
  EXPECT_EQ(105, out.existency);
  EXPECT_EQ(0, out.object_location.geodetic_srid);
  EXPECT_EQ(235, out.object_location.latitude);
  EXPECT_EQ(666, out.object_location.longitude);
  EXPECT_EQ(987, out.object_location.altitude);
  EXPECT_EQ(12343, out.object_location.crp_id);
  EXPECT_EQ(3547, out.object_location.orientation);
  EXPECT_EQ(57920, out.move_direction.value);
  EXPECT_EQ(3456, out.speed.value);
  EXPECT_EQ(1596, out.yaw_rate.value);
  EXPECT_EQ(3254, out.acceleration.value);
  EXPECT_EQ(9988, out.orientation.value);
  EXPECT_EQ(8426, out.length.value);
  EXPECT_EQ(7845, out.width.value);
  EXPECT_EQ(9019, out.height.value);
  EXPECT_EQ(630000, out.towing_vehicle);
  EXPECT_EQ(56785678, out.info_src_list[0]);

  // teardown
  // nothing to do
}

#ifdef FS
/**
 * @brief ResultSetからFreespaceInfoの取り出しが正しく機能するかを検証する
 *
 * - CreateResultSet_fで定義したmapの値が以下の通り出力されることを確認する
 *
 *       id: 0x12341234
 *       time: 5000000
 *       existency: 27
 *       minimal_detectable_size: 123
 *       position_begin.geodetic_srid: 456
 *       position_begin.latitude: 789
 *       position_begin.longitude: 3322
 *       position_begin.altitude: 125
 *       position_begin.crp_id: 523
 *       position_begin.dx_crp: 633
 *       position_begin.dy_crp: 454
 *       position_begin.dh_crp: 963
 *       position_begin.lane_count: 84
 *       position_begin.lane_position: -27
 *       position_begin.lane_lateral_position: 159
 *       position_begin.crp_id_begin: 753
 *       position_begin.crp_id_end: 486
 *       position_begin.lane_vertical_position: 426
 *       position_begin.lane_id: 0x6314
 *       position_begin.dx_lane: 842
 *       position_begin.dy_lane: 862
 *       position_begin.dh_lane: 9852
 *       position_begin.semi_axis_length_major: 336
 *       position_begin.semi_axis_length_minor: 4567
 *       position_begin.orientation: 7654
 *       position_begin.altitude_accuracy: 987
 *       position_end.geodetic_srid: 654
 *       position_end.latitude: 321
 *       position_end.longitude: 12541
 *       position_end.altitude: 6652
 *       position_end.crp_id: 1100
 *       position_end.dx_crp: 120
 *       position_end.dy_crp: 3698
 *       position_end.dh_crp: 8563
 *       position_end.lane_count: 10
 *       position_end.lane_position: -72
 *       position_end.lane_lateral_position: 73
 *       position_end.crp_id_begin: 6521
 *       position_end.crp_id_end: 4589
 *       position_end.lane_vertical_position: 3214
 *       position_end.lane_id: 0x74523
 *       position_end.dx_lane: 652
 *       position_end.dy_lane: 8521
 *       position_end.dh_lane: 6332
 *       position_end.semi_axis_length_major: 1225
 *       position_end.semi_axis_length_minor: 63268
 *       position_end.orientation: 9633
 *       position_end.altitude_accuracy: 1202
 *       length.value: 1010
 *       length.accuracy: 2022
 *       id_begin: 0x652
 *       id_end: 0x98563
 *       info_src_list[0]: 0x321654
 *       info_src_list[1]: 0
 *       info_src_list[2]: 0
 *       info_src_list[3]: 0
 *
 */
TEST(DmiUtlsDmBridge, FromResultSet_FreespaceInfo) {
  // setup
  test_util::CandidateTestCase_f data = test_util::CandidateTestCase_f();
  ResultSet input = test_util::CreateResultSet_f(data);
  input.next();

  // exercise
  auto out = udp_dmi::dm2::DmBridge<FreespaceInfo>::FromResultSet(input).get();

  // verify
  EXPECT_EQ(0x12341234, out.id);
  EXPECT_EQ(5000000, out.time);
  EXPECT_EQ(27, out.existency);
  EXPECT_EQ(123, out.minimal_detectable_size);
  EXPECT_EQ(456, out.position_begin.geodetic_srid);
  EXPECT_EQ(789, out.position_begin.latitude);
  EXPECT_EQ(3322, out.position_begin.longitude);
  EXPECT_EQ(125, out.position_begin.altitude);
  EXPECT_EQ(523, out.position_begin.crp_id);
  EXPECT_EQ(633, out.position_begin.dx_crp);
  EXPECT_EQ(454, out.position_begin.dy_crp);
  EXPECT_EQ(963, out.position_begin.dh_crp);
  EXPECT_EQ(84, out.position_begin.lane_count);
  EXPECT_EQ(-27, out.position_begin.lane_position);
  EXPECT_EQ(159, out.position_begin.lane_lateral_position);
  EXPECT_EQ(753, out.position_begin.crp_id_begin);
  EXPECT_EQ(486, out.position_begin.crp_id_end);
  EXPECT_EQ(426, out.position_begin.lane_vertical_position);
  EXPECT_EQ(0x6314, out.position_begin.lane_id);
  EXPECT_EQ(842, out.position_begin.dx_lane);
  EXPECT_EQ(862, out.position_begin.dy_lane);
  EXPECT_EQ(9852, out.position_begin.dh_lane);
  EXPECT_EQ(336, out.position_begin.semi_axis_length_major);
  EXPECT_EQ(4567, out.position_begin.semi_axis_length_minor);
  EXPECT_EQ(7654, out.position_begin.orientation);
  EXPECT_EQ(987, out.position_begin.altitude_accuracy);
  EXPECT_EQ(654, out.position_end.geodetic_srid);
  EXPECT_EQ(321, out.position_end.latitude);
  EXPECT_EQ(12541, out.position_end.longitude);
  EXPECT_EQ(6652, out.position_end.altitude);
  EXPECT_EQ(1100, out.position_end.crp_id);
  EXPECT_EQ(120, out.position_end.dx_crp);
  EXPECT_EQ(3698, out.position_end.dy_crp);
  EXPECT_EQ(8563, out.position_end.dh_crp);
  EXPECT_EQ(10, out.position_end.lane_count);
  EXPECT_EQ(-72, out.position_end.lane_position);
  EXPECT_EQ(73, out.position_end.lane_lateral_position);
  EXPECT_EQ(6521, out.position_end.crp_id_begin);
  EXPECT_EQ(4589, out.position_end.crp_id_end);
  EXPECT_EQ(3214, out.position_end.lane_vertical_position);
  EXPECT_EQ(0x74523, out.position_end.lane_id);
  EXPECT_EQ(652, out.position_end.dx_lane);
  EXPECT_EQ(8521, out.position_end.dy_lane);
  EXPECT_EQ(6332, out.position_end.dh_lane);
  EXPECT_EQ(1225, out.position_end.semi_axis_length_major);
  EXPECT_EQ(63268, out.position_end.semi_axis_length_minor);
  EXPECT_EQ(9633, out.position_end.orientation);
  EXPECT_EQ(1202, out.position_end.altitude_accuracy);
  EXPECT_EQ(1010, out.length.value);
  EXPECT_EQ(2022, out.length.accuracy);
  EXPECT_EQ(0x652, out.id_begin);
  EXPECT_EQ(0x98563, out.id_end);
  EXPECT_EQ(0x321654, out.info_src_list[0]);
  EXPECT_EQ(0, out.info_src_list[1]);
  EXPECT_EQ(0, out.info_src_list[2]);
  EXPECT_EQ(0, out.info_src_list[3]);

  // teardown
  // nothing to do
}
#endif

/**
 * @brief ResultSetからLightInformationの取り出しが正しく機能するかを検証する
 *
 * - CreateResultSet_sで定義したmapの値が以下の通り出力されることを確認する
 *
 *       main_light_0: 142
 *       arrow_light_0: 65
 *       min_time_to_change_0: 741
 *       max_time_to_change_0: 852
 *       
 */
TEST(DmiUtlsDmBridge, FromResultSet_LightInformation) {
  // setup
  test_util::CandidateTestCase_s data = test_util::CandidateTestCase_s();
  ResultSet input = test_util::CreateResultSet_s(data);
  input.next();

  // exercise
  auto out = dmi_utils::DmBridge<LightInformation>::FromResultSet(input, 0).get();

  // verify
  EXPECT_EQ(142, out.main_light);
  EXPECT_EQ(65, out.arrow_light);
  EXPECT_EQ(741, out.min_time_to_change);
  EXPECT_EQ(852, out.max_time_to_change);

  // teardown
  // nothing to do
}

/**
 * @brief ResultSetからSignalInfoの取り出しが正しく機能するかを検証する
 *
 * - CreateResultSet_sで定義したmapの値が以下の通り出力されることを確認する
 *
 *       crp_id: 123
 *       signal_id_list[0]: 200
 *       signal_id_list[1]: 201
 *       signal_id_list[2]: 0
 *       signal_id_list[3]: 0
 *       signal_id_list[4]: 0
 *       signal_id_list[5]: 0
 *       signal_id_list[6]: 0
 *       signal_id_list[7]: 0
 *       time: 789
 *       state: 110
 *       specific_control_flags: 120
 *       event_count: 130
 *       count_down_stop_flag: 219
 *       light_information_list[0].main_light: 142
 *       light_information_list[0].arrow_light: 65
 *       light_information_list[0].min_time_to_change: 741
 *       light_information_list[0].max_time_to_change: 852
 *       light_information_list[1].main_light: 195
 *       light_information_list[1].arrow_light: 147
 *       light_information_list[1].min_time_to_change: 258
 *       light_information_list[1].max_time_to_change: 369
 *       light_information_list[2].main_light: 159
 *       light_information_list[2].arrow_light: 241
 *       light_information_list[2].min_time_to_change: 963
 *       light_information_list[2].max_time_to_change: 852
 *       light_information_list[3].main_light: 229
 *       light_information_list[3].arrow_light: 189
 *       light_information_list[3].min_time_to_change: 4546
 *       light_information_list[3].max_time_to_change: 7879
 *       light_information_list[4].main_light: 25
 *       light_information_list[4].arrow_light: 133
 *       light_information_list[4].min_time_to_change: 9876
 *       light_information_list[4].max_time_to_change: 4321
 *       light_information_list[5].main_light: 56
 *       light_information_list[5].arrow_light: 153
 *       light_information_list[5].min_time_to_change: 486
 *       light_information_list[5].max_time_to_change: 9632
 *       light_information_list[6].main_light: 73
 *       light_information_list[6].arrow_light: 245
 *       light_information_list[6].min_time_to_change: 5632
 *       light_information_list[6].max_time_to_change: 8965
 *       light_information_list[7].main_light: 174
 *       light_information_list[7].arrow_light: 169
 *       light_information_list[7].min_time_to_change: 1379
 *       light_information_list[7].max_time_to_change: 3791
 *       light_information_list[8].main_light: 233
 *       light_information_list[8].arrow_light: 177
 *       light_information_list[8].min_time_to_change: 4862
 *       light_information_list[8].max_time_to_change: 8624
 *       light_information_list[9].main_light: 104
 *       light_information_list[9].arrow_light: 182
 *       light_information_list[9].min_time_to_change: 6539
 *       light_information_list[9].max_time_to_change: 8579
 *       light_information_list[10].main_light: 27
 *       light_information_list[10].arrow_light: 3
 *       light_information_list[10].min_time_to_change: 6135
 *       light_information_list[10].max_time_to_change: 4321
 *       light_information_list[11].main_light: 154
 *       light_information_list[11].arrow_light: 203
 *       light_information_list[11].min_time_to_change: 12365
 *       light_information_list[11].max_time_to_change: 18589
 *
 */
TEST(DmiUtlsDmBridge, FromResultSet_SignalInfo) {
  // setup
  test_util::CandidateTestCase_s data = test_util::CandidateTestCase_s();
  ResultSet input = test_util::CreateResultSet_s(data);
  input.next();

  // exercise
  auto out = dmi_utils::DmBridge<SignalInfo>::FromResultSet(input).get();

  // verify
  EXPECT_EQ(123, out.crp_id);
  EXPECT_EQ(200, out.signal_id_list[0]);
  EXPECT_EQ(201, out.signal_id_list[1]);
  EXPECT_EQ(0, out.signal_id_list[2]);
  EXPECT_EQ(0, out.signal_id_list[3]);
  EXPECT_EQ(0, out.signal_id_list[4]);
  EXPECT_EQ(0, out.signal_id_list[5]);
  EXPECT_EQ(0, out.signal_id_list[6]);
  EXPECT_EQ(0, out.signal_id_list[7]);
  EXPECT_EQ(789, out.time);
  EXPECT_EQ(110, out.state);
  EXPECT_EQ(120, out.specific_control_flags);
  EXPECT_EQ(130, out.event_count);
  EXPECT_EQ(219, out.count_down_stop_flag);
  EXPECT_EQ(142, out.light_information_list[0].main_light);
  EXPECT_EQ(65, out.light_information_list[0].arrow_light);
  EXPECT_EQ(741, out.light_information_list[0].min_time_to_change);
  EXPECT_EQ(852, out.light_information_list[0].max_time_to_change);
  EXPECT_EQ(195, out.light_information_list[1].main_light);
  EXPECT_EQ(147, out.light_information_list[1].arrow_light);
  EXPECT_EQ(258, out.light_information_list[1].min_time_to_change);
  EXPECT_EQ(369, out.light_information_list[1].max_time_to_change);
  EXPECT_EQ(159, out.light_information_list[2].main_light);
  EXPECT_EQ(241, out.light_information_list[2].arrow_light);
  EXPECT_EQ(963, out.light_information_list[2].min_time_to_change);
  EXPECT_EQ(852, out.light_information_list[2].max_time_to_change);
  EXPECT_EQ(229, out.light_information_list[3].main_light);
  EXPECT_EQ(189, out.light_information_list[3].arrow_light);
  EXPECT_EQ(4546, out.light_information_list[3].min_time_to_change);
  EXPECT_EQ(7879, out.light_information_list[3].max_time_to_change);
  EXPECT_EQ(25, out.light_information_list[4].main_light);
  EXPECT_EQ(133, out.light_information_list[4].arrow_light);
  EXPECT_EQ(9876, out.light_information_list[4].min_time_to_change);
  EXPECT_EQ(4321, out.light_information_list[4].max_time_to_change);
  EXPECT_EQ(56, out.light_information_list[5].main_light);
  EXPECT_EQ(153, out.light_information_list[5].arrow_light);
  EXPECT_EQ(486, out.light_information_list[5].min_time_to_change);
  EXPECT_EQ(9632, out.light_information_list[5].max_time_to_change);
  EXPECT_EQ(73, out.light_information_list[6].main_light);
  EXPECT_EQ(245, out.light_information_list[6].arrow_light);
  EXPECT_EQ(5632, out.light_information_list[6].min_time_to_change);
  EXPECT_EQ(8965, out.light_information_list[6].max_time_to_change);
  EXPECT_EQ(174, out.light_information_list[7].main_light);
  EXPECT_EQ(169, out.light_information_list[7].arrow_light);
  EXPECT_EQ(1379, out.light_information_list[7].min_time_to_change);
  EXPECT_EQ(3791, out.light_information_list[7].max_time_to_change);
  EXPECT_EQ(233, out.light_information_list[8].main_light);
  EXPECT_EQ(177, out.light_information_list[8].arrow_light);
  EXPECT_EQ(4862, out.light_information_list[8].min_time_to_change);
  EXPECT_EQ(8624, out.light_information_list[8].max_time_to_change);
  EXPECT_EQ(104, out.light_information_list[9].main_light);
  EXPECT_EQ(182, out.light_information_list[9].arrow_light);
  EXPECT_EQ(6539, out.light_information_list[9].min_time_to_change);
  EXPECT_EQ(8579, out.light_information_list[9].max_time_to_change);
  EXPECT_EQ(27, out.light_information_list[10].main_light);
  EXPECT_EQ(3, out.light_information_list[10].arrow_light);
  EXPECT_EQ(6135, out.light_information_list[10].min_time_to_change);
  EXPECT_EQ(4321, out.light_information_list[10].max_time_to_change);
  EXPECT_EQ(154, out.light_information_list[11].main_light);
  EXPECT_EQ(203, out.light_information_list[11].arrow_light);
  EXPECT_EQ(12365, out.light_information_list[11].min_time_to_change);
  EXPECT_EQ(18589, out.light_information_list[11].max_time_to_change);

  // teardown
  // nothing to do
}
