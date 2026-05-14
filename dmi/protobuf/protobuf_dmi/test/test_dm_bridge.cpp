/**                                                                                                  
 * @file test_dm_bridge.cpp
 * @brief dm_bridge.hpp(cpp)の単体テスト
 *
 */

#include <gtest/gtest.h>
#include "utils_of_test.hpp"
#if __cplusplus < 201703L || defined(USE_EXP_ANY)                                                    
  #include <experimental/any>
  using std::experimental::any;
  using std::experimental::any_cast;
#else
  #include <any>
#endif



/**
 * @brief フィクスチャークラステンプレート定義  
 *
 */
template <class T>
class ExtractTupleValueTest : public testing::Test
{};

using testing::Types;

/// テスト対象に使用している型の羅列です
typedef Types<int, long, unsigned long long> testTypes;


/**
 * @brief 渡された値を指定位置の値に正しく抽出できるかを検証する。
 *
 * @note 今検証できる型はコードに使用している型だけです。\n
 *       他の型を検証する場合は、testTypesに追加してください
 *
 */
TYPED_TEST_CASE(ExtractTupleValueTest, testTypes);

/**
 * @brief テスト流れを定義する
 *
 */
TYPED_TEST(ExtractTupleValueTest, ExtractActionTest)
{
  // set up
  const long stp = 1000;
  TypeParam val = 12345678;
  IS::Tuple tuple(1);

  tuple.setValue(0, val, stp);
  
  // action
  TypeParam out = protobuf_dmi::ExtractTupleValue<TypeParam>(0, tuple);

  // verification
  EXPECT_EQ(val, out);

  // tear down
}


/**
 * @brief 渡された値をTupleに正しく設置されたかどうかを検証する
 *
 * @note 今検証できる型はコードに使用している型だけです。\n
 *       他の型を検証する時、testTypesに追加してください
 *
 */
template <class T>
class SetTupleValueTest : public testing::Test
{};

/**
 * @brief テスト実例とテスト型をバンドする
 *
 */
TYPED_TEST_CASE(SetTupleValueTest, testTypes);

/**
 * @brief テスト流れを定義する
 *
 */
TYPED_TEST(SetTupleValueTest, SetActionTest)
{
  // set up
  const long stp = 1000;
  TypeParam val = 12345678;
  IS::Tuple tuple(1);
  // action

  protobuf_dmi::SetTupleValue<TypeParam>(0, stp, val, tuple);

  // verification
  any res;
  long _s = 1000;
  tuple.getValue(0, res, _s);
  EXPECT_EQ(val, any_cast<TypeParam>(res));

  // tear down

}
/**
 * @brief 渡された検査関数の結果によって、Tupleに値を正しく設定されたかどうかを検証する\n
 *　　　　ケースhasTrueTestはチェック関数がTUREを返すの場合、保存している値をTupleに書き込む\n
 *　　　　ケースhasFalseTestはチェック関数がFalseを返す場合、デフォルト値をTupleに書き込む
 *
 * @note 今検証できる型はコードに使用している型だけです。\n
 *       他の型を検証する時、testTypesに追加してください
 */
template <class T>                                                                                   
class SetTupleValueOptionalTest : public testing::Test
{};

/**
 * @brief テスト実例とテスト型をバンドする
 *
 */
TYPED_TEST_CASE(SetTupleValueOptionalTest, testTypes);

/**
 * @brief テスト流れを定義する
 *
 */
TYPED_TEST(SetTupleValueOptionalTest, hasTrueTest)
{
  // set up
  const uint32_t idx = 0;
  const long stamp = 1001;
  const std::function<bool()> check_func = [](){return true;};
  const TypeParam df_val = 0x00000004;
  const TypeParam want_val = 0x00000005;
  const std::function<TypeParam()> get_func = [want_val](){
    TypeParam a = want_val;
    return a;};
  IS::Tuple out(1);
  
  // action
  protobuf_dmi::SetTupleValueOptional(idx, stamp, check_func, get_func, df_val, out);
  // verification
  any res;
  long _s = 1000;
  out.getValue(0, res, _s);
  EXPECT_EQ(want_val, any_cast<TypeParam>(res));

  // teardown
}

/**
 * @brief テスト流れを定義する
 *
 */
TYPED_TEST(SetTupleValueOptionalTest, hasFalseTest)
{
  // set up
  const uint32_t idx = 0;
  const long stamp = 1001;
  const std::function<bool()> check_func = [](){return false;};
  const TypeParam df_val = 0x00000004;
  const TypeParam want_val = 0x00000005;
  const std::function<TypeParam()> get_func = [want_val](){
    TypeParam a = want_val;
    return a;};
  IS::Tuple out(1);
  
  // action
  protobuf_dmi::SetTupleValueOptional(idx, stamp, check_func, get_func, df_val, out);
  // verification
  any res;
  long _s = 1000;
  out.getValue(0, res, _s);
  EXPECT_EQ(df_val, any_cast<TypeParam>(res));

  // teardown

}
/**                                                                                                 
 * @brief 目標の番号を正しく計算されたかどうかを検証する
 *
 *
 */
TEST(CalcObjIdTest, ZeroTest)
{
  const ObjectInformation obj = ObjectInformation();

  const protobuf_dmi::utils::Config c;
  const long long id = protobuf_dmi::CalcObjectId(obj, c);

  ASSERT_EQ(0x8000000000000000, id);

}

/**
 * @brief 重複利用できるSensingMessageクラスを定義しているテストフィクスチャ
 * @note 利用する際、TEST_Fマクロを使ってください \n
 *       1番目位の引数に宣言されているSensingMessagetoTupleTestを渡してください \n
 *       例：　TEST_F(SensingMessagetoTupleTest, <test_case_that_want_to_use_SensingMessage>){...}
 */
class SensingMessagetoTupleTest : public ::testing::Test {
protected:

  void SetUp() override {
    _sm.set_message_id(1);
    _sm.set_protocol_version(2);
    _sm.set_message_counter(3);
    _sm.set_sensing_time(4);
    _sm.set_error_notification(5);
    _sm.set_error_code(6);

    SensorInformation* s_in = _sm.add_sensor_info();
    ObjectInformation* o_in = _sm.add_object_infos();
    PerceivedFreeSpaceInformation* f_in = _sm.add_freespace_infos();

    // SensorInformation
    s_in->set_type(ST_LIDAR);  // 2
    s_in->set_latitude(11);
    s_in->set_longitude(12);
    s_in->set_altitude(13);
    
    DetectCapability* dc = s_in->add_detect_capabilities();
    dc->set_detectable_classes(141);
    OffsetPointXY* p1 = dc->add_poly_points();
    p1->set_dx(1410);
    p1->set_dy(1411);
    OffsetPointXY* p2 = dc->add_poly_points();
    p2->set_dx(1412);
    p2->set_dy(1413);
    OffsetPointXY* p3 = dc->add_poly_points();
    p3->set_dx(1414);
    p3->set_dy(1415);
    dc->set_confidence(142);
    dc->set_detectable_size(143);

    s_in->set_sensor_status(15);
    // SensorInformation END

    // ObjectInformation 
    o_in->set_object_id(21);
    o_in->set_time_of_measurement(22);
    
    ObjectClass* obc = o_in->add_object_classes();
    obc->set_vehicle_subclass_type(VSCT_LIGHT_TRUCK); // 3
    obc->set_class_confidence(231);
    obc->set_subclass_confidence(232);

    o_in->set_confidence(24);
  
    Position* o_pos = new Position();
    o_pos->set_latitude(250);
    o_pos->set_longitude(251);
    o_pos->set_altitude(252);
    o_pos->set_semi_major_axis_length(253);
    o_pos->set_semi_minor_axis_length(254);
    o_pos->set_semi_major_orientation(255);
    o_pos->set_altitude_accuracy(256);

    o_in->set_allocated_position(o_pos);
    o_in->set_ref_point(RP_REAR_MIDWIDTH_BOTTOM); // 0~9
    o_in->set_heading(27);
    o_in->set_heading_accuracy(28);
    o_in->set_speed(29);
    o_in->set_speed_accuracy(30);
    o_in->set_yaw_rate(31);
    o_in->set_yaw_rate_accuracy(32);
    o_in->set_acceleration(33);
    o_in->set_acceleration_accuracy(34);
    o_in->set_orientation(35);
    o_in->set_orientation_accuracy(36);
    o_in->set_length(37);
    o_in->set_length_accuracy(38);
    o_in->set_width(39);
    o_in->set_width_accuracy(40);
    o_in->set_height(41);
    o_in->set_height_accuracy(42);
    o_in->set_static_status(43);
    o_in->set_tracking_status(44);
    o_in->set_detection_count(45);
    o_in->set_lost_count(46);
    o_in->set_object_age(47);    
    // ObjectInformation END

    // PerceivedFreeSpaceInformation
    f_in->set_time_of_measurement(50);

    Position* f_pos = new Position();
    f_pos->set_latitude(51);
    f_pos->set_longitude(52);
    f_pos->set_altitude(53);
    f_pos->set_semi_major_axis_length(54);
    f_pos->set_semi_minor_axis_length(55);
    f_pos->set_semi_major_orientation(56);
    f_pos->set_altitude_accuracy(57);

    f_in->set_allocated_position(f_pos);
  
    OffsetPointXY* p4 = f_in->add_poly_points();
    p4->set_dx(581);
    p4->set_dy(582);
    OffsetPointXY* p5 = f_in->add_poly_points();
    p5->set_dx(583);
    p5->set_dy(584);
    OffsetPointXY* p6 = f_in->add_poly_points();
    p6->set_dx(585);
    p6->set_dy(586);

    f_in->set_confidence(59);
    f_in->set_detectable_size(60);
    // PerceivedFreeSpaceInformation END
   }
  
  SensingMessage _sm;

};

/**
 * @brief センサーの位置情報をTupleに正しく設置したかどうかを検証する
 *
 *
 */
TEST_F(SensingMessagetoTupleTest, SetSensorLocationToTupleTest)
{
  
  // set up
  const long stp = 0x80000001;
  IS::Tuple out(26);
  const SensorInformation in = _sm.sensor_info(0);
  uint32_t idx_act = 0;
  // action
  protobuf_dmi::SetSensorLocationToTuple(idx_act, stp, in, out);

  // Verification
  any val;
  long timestamp = 0;
  int idx = 0;

  // -> SystemCode
  out.getValue(idx, val, timestamp);
  EXPECT_EQ(6668, any_cast<int>(val));
  // -> latitude
  // -> longitude
  // -> altitude
  for(int i = 0; i < 3; i++)
  {
    out.getValue(++idx, val, timestamp);
    EXPECT_EQ(11 + i, any_cast<int>(val));
  }
  // -> projective srid
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> prc_x
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(-132768, any_cast<int>(val));
  // -> prc_y
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(-132768, any_cast<int>(val));
  // -> prc_z
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(20001, any_cast<int>(val));
  // -> crp_id
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> dx_crp
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(-132768, any_cast<int>(val));
  // -> dy_crp
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(-132768, any_cast<int>(val));
  // -> dz_crp
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(20001, any_cast<int>(val));
  // -> lane_count
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(15, any_cast<int>(val));
  // -> lane_position
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(17, any_cast<int>(val));
  // -> lane_lateral_position
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(101, any_cast<int>(val));
  // -> crp_id_begin
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> crp_id_end
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> lane_vertical_position
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(10001, any_cast<int>(val));
  // -> lane_id
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<unsigned long long>(val));
  // -> dx_lane dy_lane dh_lane
  for(int i = 0; i < 3; i++)
  {
    out.getValue(++idx, val, timestamp);
    EXPECT_EQ(-132768, any_cast<int>(val));
  }
  // -> semi_axis_length_major
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(4095, any_cast<int>(val));
  // -> semi_axis_length_minor
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(4095, any_cast<int>(val));
  // -> ellipse_orientation
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(28800, any_cast<int>(val));
  // -> altitude_accuracy
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(7201, any_cast<int>(val));

  // teardown
}

/**
 * @brief 位置情報ををTupleに正しく設置したかどうかを検証する
 *
 *
 */
TEST_F(SensingMessagetoTupleTest, SetPositionToTupleTest)
{
  
  // set up
  const long stp = 0x80000001;
  IS::Tuple out(26);
  PerceivedFreeSpaceInformation fs = _sm.freespace_infos(0);
  const Position p = fs.position();
  uint32_t idx_act = 0;
  // action
  protobuf_dmi::SetPositionToTuple(idx_act, stp, p, out);

  // Verification
  any val;
  long timestamp = 0;
  int idx = 0;
  // -> kGeodeticSystemCodeJgd2011
  out.getValue(idx, val, timestamp);
  EXPECT_EQ(6668, any_cast<int>(val));
  // -> p.latitude
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(51, any_cast<int>(val));
  // -> p.longitude
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(52, any_cast<int>(val));
  // -> p.altitude
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(53, any_cast<int>(val));
  // -> p.projective srid
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> prc_x y z
  for(int i = 0; i < 3; i++)
  {
    out.getValue(++idx, val, timestamp);
    EXPECT_EQ(-132768, any_cast<int>(val));
  }
  // -> crp_id
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> crp x y z
  for(int i = 0; i < 3; i++)
  {
    out.getValue(++idx, val, timestamp);
    EXPECT_EQ(-132768, any_cast<int>(val));
  }
  // -> lane_count
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(15, any_cast<int>(val));
  // -> lane_position
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(17, any_cast<int>(val));
  // -> lane_lateral_position
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(101, any_cast<int>(val));
  // -> crp_id begin end
  for(int i = 0; i < 2; i++)
  {
    out.getValue(++idx, val, timestamp);
    EXPECT_EQ(0, any_cast<int>(val));
  }
  // -> lane_vertical_position
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(10001, any_cast<int>(val));
  // -> lane_id
  out.getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<unsigned long long>(val));
  // -> lane x y z
  for(int i = 0; i < 3; i++)
  {
    out.getValue(++idx, val, timestamp);
    EXPECT_EQ(-132768, any_cast<int>(val));
  }
  //  semi_major_axis_length semi_minor_axis_length semi_major_orientation altitude_accuracy
  for(int i = 0; i < 4; i++)
  {
    out.getValue(++idx, val, timestamp);
    EXPECT_EQ(54+i, any_cast<int>(val));
  }


}

/**
 * @brief 渡した目標類別対象から、類別番号と子類別番号を正しく抽出かどうかを検証する
 *
 * @note クラス別に検証を設置します。\n
 *       検証可能なクラスは
 *       CooL4ObjectClass::{kVehicle, kTrain, kMotorcycle, kLightVehicle,
 *        kPerson, kAnimal, kNotFixedObject, kFixedObject}
 */

/**
 * @brief 自動車のサブクラスからサブクラスとクラスの番号を正しく変換できるかどうかを検証する
 *
 * @note 新しい類別を追加したい場合、
 * INSTANTIATE_TEST_CASE_Pの三番目の引数に::testing::Values(...)に追加すれば検証可能になる
*/
class VehicleClassTest : public ::testing::TestWithParam<VehicleSubclassType> {};

/**
 * @brief 自動車クラス変換機能テストの流れを定義する
 *
 */
TEST_P(VehicleClassTest, iskVehicleTest)
{
  // set up
  ObjectClass oc;
  oc.set_vehicle_subclass_type(GetParam());
  
  std::pair<int, int> res;
  
  // action
  res = protobuf_dmi::GetObjectClassIdAndSubclassTypeId(oc);

  // verification
  EXPECT_EQ(1, res.first); //CooL4ObjectClass::kVehicle   
  EXPECT_EQ(static_cast<int>(GetParam()), res.second);

  // teardown
} 

/**
 * @brief 自動車クラス変換機能テストで検証したい値を宣言する
 *
 */
INSTANTIATE_TEST_CASE_P(VehicleSubclasses, VehicleClassTest, ::testing::Values(
 VSCT_UNKNOWN,
 VSCT_PASSENGER_CAR,
 VSCT_BUS,
 VSCT_LIGHT_TRUCK,
 VSCT_HEAVY_TRUCK,
 VSCT_TRAILER,
 VSCT_SPECIAL_VEHICLES,
 VSCT_EMERGENCY_VEHICLE,
 VSCT_AGRICULTURAL,
 VSCT_GROUP));


/**
 * @brief 車両のサブクラスからサブクラスとクラスの番号を正しく変換できるかどうかを検証する
 *
 * @note 新しい類別を追加したい場合、
 * INSTANTIATE_TEST_CASE_Pの三番目の引数に::testing::Values(...)に追加すれば検証可能になる
*/
class TrainClassTest : public ::testing::TestWithParam<TrainSubclassType> {};

/**
 * @brief 車両クラス変換機能テストの流れを定義する
 *
 */
TEST_P(TrainClassTest, iskTrainTest)
{
  // set up
  ObjectClass oc;
  oc.set_train_subclass_type(GetParam());
  
  std::pair<int, int> res;
  
  // action
  res = protobuf_dmi::GetObjectClassIdAndSubclassTypeId(oc);

  // verification
  EXPECT_EQ(2, res.first); //CooL4ObjectClass::kTrain   
  EXPECT_EQ(static_cast<int>(GetParam()), res.second);

  // teardown
} 

/**
 * @brief 車両クラス変換機能テストで検証したい値を宣言する
 *
 */
INSTANTIATE_TEST_CASE_P(TrainSubclasses, TrainClassTest, ::testing::Values(
 TSCT_UNKNOWN,
 TSCT_TRAM,
 TSCT_OTHER_TRAIN));

/**
 * @brief バイクのサブクラスからサブクラスとクラスの番号を正しく変換できるかどうかを検証する
 *
 * @note 新しい類別を追加したい場合、
 * INSTANTIATE_TEST_CASE_Pの三番目の引数に::testing::Values(...)に追加すれば検証可能になる
*/
class MotorcycleClassTest : public ::testing::TestWithParam<MotorcycleSubclassType> {};

/**
 * @brief バイククラス変換機能テストの流れを定義する
 *
 */
TEST_P(MotorcycleClassTest, iskMotorcycleTest)
{
  // set up
  ObjectClass oc;
  oc.set_motorcycle_subclass_type(GetParam());
  
  std::pair<int, int> res; 
  
  // action
  res = protobuf_dmi::GetObjectClassIdAndSubclassTypeId(oc);

  // verification
  EXPECT_EQ(3, res.first); //CooL4ObjectClass::kMotorcycle   
  EXPECT_EQ(static_cast<int>(GetParam()), res.second);

  // teardown
} 

/**
 * @brief バイククラス変換機能テストで検証したい値を宣言する
 *
 */
INSTANTIATE_TEST_CASE_P(MotorcycleSubclasses, MotorcycleClassTest, ::testing::Values(
 MSCT_UNKNOWN,
 MSCT_MOPED,
 MSCT_MOTORCYCLE,
 MSCT_GROUP));   

/**
 * @brief 軽型自動車のサブクラスからサブクラスとクラスの番号を正しく変換できるかどうかを検証する
 *
 * @note 新しい類別を追加したい場合、
 * INSTANTIATE_TEST_CASE_Pの三番目の引数に::testing::Values(...)に追加すれば検証可能になる
*/
class LightVehicleClassTest : public ::testing::TestWithParam<LightVehicleSubclassType> {};

/**
 * @brief 軽型自動車クラス変換機能テストの流れを定義する
 *
 */
TEST_P(LightVehicleClassTest, iskLightVehicleTest)
{
  // set up
  ObjectClass oc;
  oc.set_light_vehicle_subclass_type(GetParam());

  std::pair<int, int> res;

  // action
  res = protobuf_dmi::GetObjectClassIdAndSubclassTypeId(oc);

  // verification
  EXPECT_EQ(4, res.first); //CooL4ObjectClass::kLightVehicle
  EXPECT_EQ(static_cast<int>(GetParam()), res.second);

  // teardown
}

/**
 * @brief 軽型自動車クラス変換機能テストで検証したい値を宣言する
 *
 */
INSTANTIATE_TEST_CASE_P(LightVehicleSubclasses, LightVehicleClassTest, ::testing::Values(
  LVSCT_UNKNOWN,
  LVSCT_BICYCLE,
  LVSCT_RICKSHAW,
  LVSCT_CART,
  LVSCT_KICKBOARD,
  LVSCT_GROUP));

/**
 * @brief 人のサブクラスからサブクラスとクラスの番号を正しく変換できるかどうかを検証する
 *
 * @note 新しい類別を追加したい場合、
 * INSTANTIATE_TEST_CASE_Pの三番目の引数に::testing::Values(...)に追加すれば検証可能になる
*/
class PersonClassTest : public ::testing::TestWithParam<PersonSubclassType> {};

/**
 * @brief 人クラス変換機能テストの流れを定義する
 *
 */
TEST_P(PersonClassTest, iskPersonTest)
{
  // set up
  ObjectClass oc;
  oc.set_person_subclass_type(GetParam());
  
  std::pair<int, int> res; 
  
  // action
  res = protobuf_dmi::GetObjectClassIdAndSubclassTypeId(oc);

  // verification
  EXPECT_EQ(5, res.first); //CooL4ObjectClass::kPerson   
  EXPECT_EQ(static_cast<int>(GetParam()), res.second);

  // teardown
} 

/**
 * @brief 人クラス変換機能テストで検証したい値を宣言する
 *
 */
INSTANTIATE_TEST_CASE_P(PersonSubclasses, PersonClassTest, ::testing::Values(
  PSCT_UNKNOWN,
  PSCT_PEDESTRIAN,
  PSCT_WHEELCHAIR,
  PSCT_SENIOR_CAR,
  PSCT_STROLLER,
  PSCT_SKATES,
  PSCT_GROUP));   

/**
 * @brief 動物のサブクラスからサブクラスとクラスの番号を正しく変換できるかどうかを検証する
 *
 * @note 新しい類別を追加したい場合、
 * INSTANTIATE_TEST_CASE_Pの三番目の引数に::testing::Values(...)に追加すれば検証可能になる
*/
class AnimalClassTest : public ::testing::TestWithParam<AnimalSubclassType> {};

/**
 * @brief 動物クラス変換機能テストの流れを定義する
 *
 */
TEST_P(AnimalClassTest, iskAnimalTest)
{
  // set up
  ObjectClass oc;
  oc.set_animal_subclass_type(GetParam());
  
  std::pair<int, int> res; 
  
  // action
  res = protobuf_dmi::GetObjectClassIdAndSubclassTypeId(oc);

  // verification
  EXPECT_EQ(6, res.first); //CooL4ObjectClass::kAnimal   
  EXPECT_EQ(static_cast<int>(GetParam()), res.second);

  // teardown
} 

/**
 * @brief 動物クラス変換機能テストで検証したい値を宣言する
 *
 */
INSTANTIATE_TEST_CASE_P(AnimalSubclasses, AnimalClassTest, ::testing::Values(
  ASCT_UNKNOWN));   

/**
 * @brief 非固定物標のサブクラスからサブクラスとクラスの番号を正しく変換できるかどうかを検証する
 *
 * @note 新しい類別を追加したい場合、
 * INSTANTIATE_TEST_CASE_Pの三番目の引数に::testing::Values(...)に追加すれば検証可能になる
*/
class NotFixedObjectClassTest : public ::testing::TestWithParam<NfoSubclassType> {};

/**
 * @brief 非固定物標クラス変換機能テストの流れを定義する
 *
 */
TEST_P(NotFixedObjectClassTest, iskNotFixedObjectTest)
{
  // set up
  ObjectClass oc;
  oc.set_nfo_subclass_type(GetParam());
  
  std::pair<int, int> res; 
  
  // action
  res = protobuf_dmi::GetObjectClassIdAndSubclassTypeId(oc);

  // verification
  EXPECT_EQ(7, res.first); //CooL4ObjectClass::kNotFixedObject   
  EXPECT_EQ(static_cast<int>(GetParam()), res.second);

  // teardown
} 

/**
 * @brief 非固定物標クラス変換機能テストで検証したい値を宣言する
 *
 */
INSTANTIATE_TEST_CASE_P(NotFixedObjectSubclasses, NotFixedObjectClassTest, ::testing::Values(
  NFOSCT_UNKNOWN));   
  
/**
 * @brief 固定物標のサブクラスからサブクラスとクラスの番号を正しく変換できるかどうかを検証する
 *
 * @note 新しい類別を追加したい場合、
 * INSTANTIATE_TEST_CASE_Pの三番目の引数に::testing::Values(...)に追加すれば検証可能になる
*/
class FixedObjectClassTest : public ::testing::TestWithParam<FoSubclassType> {};

/**
 * @brief 固定物標クラス変換機能テストの流れを定義する
 *
 */
TEST_P(FixedObjectClassTest, iskFixedObjectTest)
{
  // set up
  ObjectClass oc;
  oc.set_fo_subclass_type(GetParam());
  
  std::pair<int, int> res; 
  
  // action
  res = protobuf_dmi::GetObjectClassIdAndSubclassTypeId(oc);

  // verification
  EXPECT_EQ(8, res.first); //CooL4ObjectClass::kFixedObject   
  EXPECT_EQ(static_cast<int>(GetParam()), res.second);

  // teardown
} 

/**
 * @brief 固定物標クラス変換機能テストで検証したい値を宣言する
 *
 */
INSTANTIATE_TEST_CASE_P(FixedObjectSubclasses, FixedObjectClassTest, ::testing::Values(
  FOSCT_UNKNOWN));  


/**                                                                                                 
 * @brief センサー情報が正しく保存されているかを検証する
 *
 *
 */
TEST_F(SensingMessagetoTupleTest, SensorInformationToTupleTest)
{
  // set up
  const long stp = 0x80000001;
  std::vector<IS::Tuple> out;
  const SensingMessage c_in = _sm;
  protobuf_dmi::utils::Config conf;
  
  protobuf_dmi::SensorInfoDmBridge sidb = protobuf_dmi::SensorInfoDmBridge(conf);
  
  
  // action
  sidb.ToTuple(c_in, stp, out);

  // verification 
  any val;
  long timestamp = 0;                                                                            
  int idx = 0;
  // conf.rsu_id
  out[0].getValue(idx, val, timestamp);
  EXPECT_EQ(0, any_cast<unsigned long long>(val));
  // conf.sensor_id
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // type
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(2, any_cast<int>(val));
  // SensorLocation
  // -> SystemCode
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(6668, any_cast<int>(val));
  // -> latitude
  // -> longitude
  // -> altitude
  for(int i = 0; i < 3; i++)
  {
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(11 + i, any_cast<int>(val));
  }
  // -> projective srid
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> prc_x
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(-132768, any_cast<int>(val));
  // -> prc_y
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(-132768, any_cast<int>(val));
  // -> prc_z
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(20001, any_cast<int>(val));
  // -> crp_id
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> dx_crp
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(-132768, any_cast<int>(val));
  // -> dy_crp
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(-132768, any_cast<int>(val));
  // -> dz_crp
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(20001, any_cast<int>(val));
  // -> lane_count
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(15, any_cast<int>(val));
  // -> lane_position
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(17, any_cast<int>(val));
  // -> lane_lateral_position
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(101, any_cast<int>(val));
  // -> crp_id_begin
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> crp_id_end
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> lane_vertical_position
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(10001, any_cast<int>(val));
  // -> lane_id
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<unsigned long long>(val));
  // -> dx_lane dy_lane dh_lane
  for(int i = 0; i < 3; i++)
  {
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(-132768, any_cast<int>(val));
  }
  // -> semi_axis_length_major
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(4095, any_cast<int>(val));
  // -> semi_axis_length_minor
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(4095, any_cast<int>(val));
  // -> ellipse_orientation
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(28800, any_cast<int>(val));
  // -> altitude_accuracy
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(7201, any_cast<int>(val));

  // base time
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(4, any_cast<long>(val));

  // detect_capabilities size = 1
    // detectable_classes
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(141, any_cast<int>(val));
    // poly_points x
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(1412, any_cast<std::vector<int>>(val)[1]);
    // poly_points y
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(1413, any_cast<std::vector<int>>(val)[1]);
    // confidence
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(142, any_cast<int>(val));
    // detectable_size
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(143, any_cast<int>(val));
  // set to empty = 7
  for(int i = 0; i < 7; i++)
  {
    // detectable_classes
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(0, any_cast<int>(val));
    // poly_points x
    out[0].getValue(++idx, val, timestamp);
    //EXPECT_EQ(0, any_cast<std::vector<int>>(val)[0]);
    // poly_points y
    out[0].getValue(++idx, val, timestamp);
    //EXPECT_EQ(0, any_cast<std::vector<int>>(val)[0]);
    // confidence
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(0, any_cast<int>(val));
    // detectable_size
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(0, any_cast<int>(val));
  }

  // sensor_status
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(15, any_cast<int>(val));

  EXPECT_EQ(70, idx);

  // teardown


}
/**
 * @brief 物標情報が正しく保存されているかを検証する
 *
 *
 */
TEST_F(SensingMessagetoTupleTest, ObjectInfoDmBridgeToTupleTest)
{
  
  // set up
  const long stp = 0x80000001;
  std::vector<IS::Tuple> out;
  const SensingMessage c_in = _sm;
  protobuf_dmi::utils::Config conf;
  
  protobuf_dmi::ObjectInfoDmBridge oidb = protobuf_dmi::ObjectInfoDmBridge(conf);
  
  
  // action
  oidb.ToTuple(c_in, stp, out);

  // verification
  any val;
  long timestamp = 0;
  int idx = 0;
  // id
  out[0].getValue(idx, val, timestamp);
  EXPECT_EQ(0x8000001500000000, any_cast<unsigned long long>(val));
  // time
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(26, any_cast<long>(val));
  // revision
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));

  // object_classes
  // class_type
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(1, any_cast<int>(val));
  // class_confidence  
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(231, any_cast<int>(val));
  // subclass_type
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(3, any_cast<int>(val));
  // subclass_type_confidence
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(232, any_cast<int>(val));
  // empty_obj_class 3*4
  for(int i = 0; i < 3 * 4; i ++)
  {
     out[0].getValue(++idx, val, timestamp);
     EXPECT_EQ(0, any_cast<int>(val));
  }
  // confidence
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(24, any_cast<int>(val));
  // Position
  // -> kGeodeticSystemCodeJgd2011
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(6668, any_cast<int>(val));
  // -> p.latitude
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(250, any_cast<int>(val));
  // -> p.longitude
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(251, any_cast<int>(val));
  // -> p.altitude
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(252, any_cast<int>(val));
  // -> p.projective srid
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> prc_x y z
  for(int i = 0; i < 3; i++)
  {
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(-132768, any_cast<int>(val));
  }
  // -> crp_id
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> crp x y z
  for(int i = 0; i < 3; i++)
  {
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(-132768, any_cast<int>(val));
  }
  // -> lane_count
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(15, any_cast<int>(val));
  // -> lane_position
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(17, any_cast<int>(val));
  // -> lane_lateral_position
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(101, any_cast<int>(val));
  // -> crp_id begin end
  for(int i = 0; i < 2; i++)
  {
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(0, any_cast<int>(val));
  }
  // -> lane_vertical_position
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(10001, any_cast<int>(val));
  // -> lane_id
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<unsigned long long>(val));
  // -> lane x y z
  for(int i = 0; i < 3; i++)
  {
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(-132768, any_cast<int>(val));
  }
  //  semi_major_axis_length semi_minor_axis_length semi_major_orientation altitude_accuracy
  for(int i = 0; i < 4; i++)
  {
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(253+i, any_cast<int>(val));
  }
  // Position End
  
  // ref_point
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(6, any_cast<int>(val));

  for(int i = 0; i < 17; i++)
  {
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(27+i, any_cast<int>(val));
  }
  // shift_position                                   
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(7, any_cast<int>(val));
  // steering_angle_front                                   
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(91, any_cast<int>(val));
  // steering_angle_rear                                   
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(91, any_cast<int>(val));
  // brake_state                                   
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // auxiliary_brake_state                                   
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // throttle_position                                   
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(255, any_cast<int>(val));
  // exterior_lights adaptive_cruise_control_system c-accs pre_crash_safety_system
  // abs tcs escs lkas ldws vehicle_size_type
  for(int i = 0; i < 10; i++)
  {
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(0, any_cast<int>(val));
  }
  // vehicle_role                                   
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(15, any_cast<int>(val));
  // vehicle_extended_info                                   
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(255, any_cast<int>(val));
  // towing vehicle                                   
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<unsigned long long>(val));
  
  // tracking_status o.detection_count lost_count object_age                             
  for(int i = 0; i < 4; i++)
  {
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(44 + i, any_cast<int>(val));
  }
  // conf_.rsu_id                                   
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<std::vector<unsigned long long>>(val)[0]);
}

/**                                                                                                 
 * @brief 走行可能領域情報が正しく保存されているかを検証する
 *
 *
 */
TEST_F(SensingMessagetoTupleTest, FreeSpaceInfomationToTupleTest)
{
  // set up
  const long stp = 0x80000001;
  std::vector<IS::Tuple> out;
  const SensingMessage c_in = _sm;
  protobuf_dmi::utils::Config conf;
  
  protobuf_dmi::FreespaceInfoDmBridge fidb = protobuf_dmi::FreespaceInfoDmBridge(conf);
  
  
  // action
  fidb.ToTuple(c_in, stp, out);

  // verification
  any val;
  long timestamp = 0;
  int idx = 0;
  // id
  out[0].getValue(idx, val, timestamp);
  EXPECT_EQ(0, any_cast<unsigned long long>(val));
  // time
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(54, any_cast<long>(val));
  // revision
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // kFreespaceDetectionMethod
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(1, any_cast<int>(val));
  // detectable_classes
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  
  // Position
  // -> kGeodeticSystemCodeJgd2011
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(6668, any_cast<int>(val));
  // -> p.latitude
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(51, any_cast<int>(val));
  // -> p.longitude
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(52, any_cast<int>(val));
  // -> p.altitude
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(53, any_cast<int>(val));
  // -> p.projective srid
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> prc_x y z
  for(int i = 0; i < 3; i++)
  {
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(-132768, any_cast<int>(val));
  }
  // -> crp_id
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> crp x y z
  for(int i = 0; i < 3; i++)
  {
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(-132768, any_cast<int>(val));
  }
  // -> lane_count
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(15, any_cast<int>(val));
  // -> lane_position
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(17, any_cast<int>(val));
  // -> lane_lateral_position
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(101, any_cast<int>(val));
  // -> crp_id begin end
  for(int i = 0; i < 2; i++)
  {
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(0, any_cast<int>(val));
  }
  // -> lane_vertical_position
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(10001, any_cast<int>(val));
  // -> lane_id
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<unsigned long long>(val));
  // -> lane x y z
  for(int i = 0; i < 3; i++)
  {
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(-132768, any_cast<int>(val));
  }
  //  semi_major_axis_length semi_minor_axis_length semi_major_orientation altitude_accuracy
  for(int i = 0; i < 4; i++)
  {
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(54+i, any_cast<int>(val));
  }
  // Position End
  
  // poly_points x
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(581, any_cast<std::vector<int>>(val)[0]);
  // poly_points y
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(582, any_cast<std::vector<int>>(val)[0]);

  // Begin&End Position
  for(int pos_i = 0; pos_i < 2; pos_i++)
  {
	  // -> kGeodeticSystemCodeJgd2011
	  out[0].getValue(++idx, val, timestamp);
	  EXPECT_EQ(6668, any_cast<int>(val));
	  // -> p.latitude
	  out[0].getValue(++idx, val, timestamp);
	  EXPECT_EQ(0, any_cast<int>(val));
	  // -> p.longitude
	  out[0].getValue(++idx, val, timestamp);
	  EXPECT_EQ(0, any_cast<int>(val));
	  // -> p.altitude
	  out[0].getValue(++idx, val, timestamp);
	  EXPECT_EQ(0, any_cast<int>(val));
	  // -> p.projective srid
	  out[0].getValue(++idx, val, timestamp);
	  EXPECT_EQ(0, any_cast<int>(val));
	  // -> prc_x y z
	  for(int i = 0; i < 3; i++)
	  {
	    out[0].getValue(++idx, val, timestamp);
	    EXPECT_EQ(-132768, any_cast<int>(val));
	  }
	  // -> crp_id
	  out[0].getValue(++idx, val, timestamp);
	  EXPECT_EQ(0, any_cast<int>(val));
	  // -> crp x y z
	  for(int i = 0; i < 3; i++)
	  {
	    out[0].getValue(++idx, val, timestamp);
	    EXPECT_EQ(-132768, any_cast<int>(val));
	  }
	  // -> lane_count
	  out[0].getValue(++idx, val, timestamp);
	  EXPECT_EQ(15, any_cast<int>(val));
	  // -> lane_position
	  out[0].getValue(++idx, val, timestamp);
	  EXPECT_EQ(17, any_cast<int>(val));
	  // -> lane_lateral_position
	  out[0].getValue(++idx, val, timestamp);
	  EXPECT_EQ(101, any_cast<int>(val));
	  // -> crp_id begin end
	  for(int i = 0; i < 2; i++)
	  {
	    out[0].getValue(++idx, val, timestamp);
	    EXPECT_EQ(0, any_cast<int>(val));
	  }
	  // -> lane_vertical_position
	  out[0].getValue(++idx, val, timestamp);
	  EXPECT_EQ(10001, any_cast<int>(val));
	  // -> lane_id
	  out[0].getValue(++idx, val, timestamp);
	  EXPECT_EQ(0, any_cast<unsigned long long>(val));
	  // -> lane x y z
	  for(int i = 0; i < 3; i++)
	  {
	    out[0].getValue(++idx, val, timestamp);
	    EXPECT_EQ(-132768, any_cast<int>(val));
	  }
	  //  semi_major_axis_length semi_minor_axis_length  
	  for(int i = 0; i < 2; i++)
	  {
	    out[0].getValue(++idx, val, timestamp);
	    EXPECT_EQ(4095, any_cast<int>(val));
	  }
	  // -> semi_major_orientation
	  out[0].getValue(++idx, val, timestamp);
	  EXPECT_EQ(28800, any_cast<int>(val));
	  // -> altitude_accuracy
	  out[0].getValue(++idx, val, timestamp);
	  EXPECT_EQ(20001, any_cast<int>(val));
  }
  // Begin&End Position End

  // lenth
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(-132768, any_cast<int>(val));
  // lenth accurancy
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // id_begin id_end
  for(int i = 0; i < 2; i++)
  {
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(0, any_cast<unsigned long long>(val));
  }
  
  // confidence 
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(59, any_cast<int>(val));
  // detectable_size 
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(60, any_cast<int>(val));
  
  // rsu_id
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<std::vector<unsigned long long>>(val)[0]);

  EXPECT_EQ(91, idx); 

  // tear down
}

/**
 * @brief 感知情報のデータが正しく保存されているかを検証する
 *
 *
 */
TEST_F(SensingMessagetoTupleTest, SensingMessageDmBridgeToTupleTest)
{
  // set up
  const long stp = 1001;

  std::vector<IS::Tuple> out;  

  protobuf_dmi::utils::Config conf;
  protobuf_dmi::SensingMessageDmBridge smdb = protobuf_dmi::SensingMessageDmBridge(conf);
  
  // action 
  smdb.ToTuple(_sm, stp, out);

  // verification
  any val;
  long timestamp = 0;                                                                            
  int idx = 0;
  // out[0] -> SensorInformation
  // conf.rsu_id
  out[0].getValue(idx, val, timestamp);
  EXPECT_EQ(0, any_cast<unsigned long long>(val));
  // conf.sensor_id
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // type
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(2, any_cast<int>(val));
  // SensorLocation
  // -> SystemCode
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(6668, any_cast<int>(val));
  // -> latitude
  // -> longitude
  // -> altitude
  for(int i = 0; i < 3; i++)
  {
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(11 + i, any_cast<int>(val));
  }
  // -> projective srid
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> prc_x
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(-132768, any_cast<int>(val));
  // -> prc_y
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(-132768, any_cast<int>(val));
  // -> prc_z
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(20001, any_cast<int>(val));
  // -> crp_id
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> dx_crp
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(-132768, any_cast<int>(val));
  // -> dy_crp
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(-132768, any_cast<int>(val));
  // -> dz_crp
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(20001, any_cast<int>(val));
  // -> lane_count
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(15, any_cast<int>(val));
  // -> lane_position
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(17, any_cast<int>(val));
  // -> lane_lateral_position
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(101, any_cast<int>(val));
  // -> crp_id_begin
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> crp_id_end
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> lane_vertical_position
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(10001, any_cast<int>(val));
  // -> lane_id
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<unsigned long long>(val));
  // -> dx_lane dy_lane dh_lane
  for(int i = 0; i < 3; i++)
  {
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(-132768, any_cast<int>(val));
  }
  // -> semi_axis_length_major
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(4095, any_cast<int>(val));
  // -> semi_axis_length_minor
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(4095, any_cast<int>(val));
  // -> ellipse_orientation
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(28800, any_cast<int>(val));
  // -> altitude_accuracy
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(7201, any_cast<int>(val));

  // base time
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(4, any_cast<long>(val));

  // detect_capabilities size = 1
    // detectable_classes
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(141, any_cast<int>(val));
    // poly_points x
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(1412, any_cast<std::vector<int>>(val)[1]);
    // poly_points y
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(1413, any_cast<std::vector<int>>(val)[1]);
    // confidence
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(142, any_cast<int>(val));
    // detectable_size
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(143, any_cast<int>(val));
  // set to empty = 7
  for(int i = 0; i < 7; i++)
  {
    // detectable_classes
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(0, any_cast<int>(val));
    // poly_points x
    out[0].getValue(++idx, val, timestamp);
    //EXPECT_EQ(0, any_cast<std::vector<int>>(val)[0]);
    // poly_points y
    out[0].getValue(++idx, val, timestamp);
    //EXPECT_EQ(0, any_cast<std::vector<int>>(val)[0]);
    // confidence
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(0, any_cast<int>(val));
    // detectable_size
    out[0].getValue(++idx, val, timestamp);
    EXPECT_EQ(0, any_cast<int>(val));
  }

  // sensor_status
  out[0].getValue(++idx, val, timestamp);
  EXPECT_EQ(15, any_cast<int>(val));

  // out[1] -> ObjectInformation
  idx = 0;
  out[1].getValue(idx, val, timestamp);
  EXPECT_EQ(0x8000001500000000, any_cast<unsigned long long>(val));
  // time
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(26, any_cast<long>(val));
  // revision
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));

  // object_classes
  // class_type
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(1, any_cast<int>(val));
  // class_confidence  
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(231, any_cast<int>(val));
  // subclass_type
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(3, any_cast<int>(val));
  // subclass_type_confidence
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(232, any_cast<int>(val));
  // empty_obj_class 3*4
  for(int i = 0; i < 3 * 4; i ++)
  {
     out[1].getValue(++idx, val, timestamp);
     EXPECT_EQ(0, any_cast<int>(val));
  }
  // confidence
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(24, any_cast<int>(val));
  // Position
  // -> kGeodeticSystemCodeJgd2011
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(6668, any_cast<int>(val));
  // -> p.latitude
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(250, any_cast<int>(val));
  // -> p.longitude
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(251, any_cast<int>(val));
  // -> p.altitude
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(252, any_cast<int>(val));
  // -> p.projective srid
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> prc_x y z
  for(int i = 0; i < 3; i++)
  {
    out[1].getValue(++idx, val, timestamp);
    EXPECT_EQ(-132768, any_cast<int>(val));
  }
  // -> crp_id
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> crp x y z
  for(int i = 0; i < 3; i++)
  {
    out[1].getValue(++idx, val, timestamp);
    EXPECT_EQ(-132768, any_cast<int>(val));
  }
  // -> lane_count
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(15, any_cast<int>(val));
  // -> lane_position
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(17, any_cast<int>(val));
  // -> lane_lateral_position
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(101, any_cast<int>(val));
  // -> crp_id begin end
  for(int i = 0; i < 2; i++)
  {
    out[1].getValue(++idx, val, timestamp);
    EXPECT_EQ(0, any_cast<int>(val));
  }
  // -> lane_vertical_position
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(10001, any_cast<int>(val));
  // -> lane_id
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<unsigned long long>(val));
  // -> lane x y z
  for(int i = 0; i < 3; i++)
  {
    out[1].getValue(++idx, val, timestamp);
    EXPECT_EQ(-132768, any_cast<int>(val));
  }
  //  semi_major_axis_length semi_minor_axis_length semi_major_orientation altitude_accuracy
  for(int i = 0; i < 4; i++)
  {
    out[1].getValue(++idx, val, timestamp);
    EXPECT_EQ(253+i, any_cast<int>(val));
  }
  // Position End
  
  // ref_point
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(6, any_cast<int>(val));

  for(int i = 0; i < 17; i++)
  {
    out[1].getValue(++idx, val, timestamp);
    EXPECT_EQ(27+i, any_cast<int>(val));
  }
  // shift_position                                   
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(7, any_cast<int>(val));
  // steering_angle_front                                   
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(91, any_cast<int>(val));
  // steering_angle_rear                                   
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(91, any_cast<int>(val));
  // brake_state                                   
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // auxiliary_brake_state                                   
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // throttle_position                                   
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(255, any_cast<int>(val));
  // exterior_lights adaptive_cruise_control_system c-accs pre_crash_safety_system
  // abs tcs escs lkas ldws vehicle_size_type
  for(int i = 0; i < 10; i++)
  {
    out[1].getValue(++idx, val, timestamp);
    EXPECT_EQ(0, any_cast<int>(val));
  }
  // vehicle_role                                   
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(15, any_cast<int>(val));
  // vehicle_extended_info                                   
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(255, any_cast<int>(val));
  // towing vehicle                                   
  out[1].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<unsigned long long>(val));
  
  // tracking_status o.detection_count lost_count object_age                             
  for(int i = 0; i < 4; i++)
  {
    out[1].getValue(++idx, val, timestamp);
    EXPECT_EQ(44 + i, any_cast<int>(val));
  }
  // conf_.rsu_id                                   
  out[1].getValue(++idx, val, timestamp);


  // out[2] -> PerceivedFreeSpaceInformation
  idx = 0;
  // id
  out[2].getValue(idx, val, timestamp);
  EXPECT_EQ(0, any_cast<unsigned long long>(val));
  // time
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(54, any_cast<long>(val));
  // revision
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // kFreespaceDetectionMethod
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(1, any_cast<int>(val));
  // detectable_classes
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  
  // Position
  // -> kGeodeticSystemCodeJgd2011
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(6668, any_cast<int>(val));
  // -> p.latitude
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(51, any_cast<int>(val));
  // -> p.longitude
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(52, any_cast<int>(val));
  // -> p.altitude
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(53, any_cast<int>(val));
  // -> p.projective srid
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> prc_x y z
  for(int i = 0; i < 3; i++)
  {
    out[2].getValue(++idx, val, timestamp);
    EXPECT_EQ(-132768, any_cast<int>(val));
  }
  // -> crp_id
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // -> crp x y z
  for(int i = 0; i < 3; i++)
  {
    out[2].getValue(++idx, val, timestamp);
    EXPECT_EQ(-132768, any_cast<int>(val));
  }
  // -> lane_count
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(15, any_cast<int>(val));
  // -> lane_position
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(17, any_cast<int>(val));
  // -> lane_lateral_position
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(101, any_cast<int>(val));
  // -> crp_id begin end
  for(int i = 0; i < 2; i++)
  {
    out[2].getValue(++idx, val, timestamp);
    EXPECT_EQ(0, any_cast<int>(val));
  }
  // -> lane_vertical_position
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(10001, any_cast<int>(val));
  // -> lane_id
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<unsigned long long>(val));
  // -> lane x y z
  for(int i = 0; i < 3; i++)
  {
    out[2].getValue(++idx, val, timestamp);
    EXPECT_EQ(-132768, any_cast<int>(val));
  }
  //  semi_major_axis_length semi_minor_axis_length semi_major_orientation altitude_accuracy
  for(int i = 0; i < 4; i++)
  {
    out[2].getValue(++idx, val, timestamp);
    EXPECT_EQ(54+i, any_cast<int>(val));
  }
  // Position End
  
  // poly_points x
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(581, any_cast<std::vector<int>>(val)[0]);
  // poly_points y
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(582, any_cast<std::vector<int>>(val)[0]);

  // Begin&End Position
  for(int pos_i = 0; pos_i < 2; pos_i++)
  {
	  // -> kGeodeticSystemCodeJgd2011
	  out[2].getValue(++idx, val, timestamp);
	  EXPECT_EQ(6668, any_cast<int>(val));
	  // -> p.latitude
	  out[2].getValue(++idx, val, timestamp);
	  EXPECT_EQ(0, any_cast<int>(val));
	  // -> p.longitude
	  out[2].getValue(++idx, val, timestamp);
	  EXPECT_EQ(0, any_cast<int>(val));
	  // -> p.altitude
	  out[2].getValue(++idx, val, timestamp);
	  EXPECT_EQ(0, any_cast<int>(val));
	  // -> p.projective srid
	  out[2].getValue(++idx, val, timestamp);
	  EXPECT_EQ(0, any_cast<int>(val));
	  // -> prc_x y z
	  for(int i = 0; i < 3; i++)
	  {
	    out[2].getValue(++idx, val, timestamp);
	    EXPECT_EQ(-132768, any_cast<int>(val));
	  }
	  // -> crp_id
	  out[2].getValue(++idx, val, timestamp);
	  EXPECT_EQ(0, any_cast<int>(val));
	  // -> crp x y z
	  for(int i = 0; i < 3; i++)
	  {
	    out[2].getValue(++idx, val, timestamp);
	    EXPECT_EQ(-132768, any_cast<int>(val));
	  }
	  // -> lane_count
	  out[2].getValue(++idx, val, timestamp);
	  EXPECT_EQ(15, any_cast<int>(val));
	  // -> lane_position
	  out[2].getValue(++idx, val, timestamp);
	  EXPECT_EQ(17, any_cast<int>(val));
	  // -> lane_lateral_position
	  out[2].getValue(++idx, val, timestamp);
	  EXPECT_EQ(101, any_cast<int>(val));
	  // -> crp_id begin end
	  for(int i = 0; i < 2; i++)
	  {
	    out[2].getValue(++idx, val, timestamp);
	    EXPECT_EQ(0, any_cast<int>(val));
	  }
	  // -> lane_vertical_position
	  out[2].getValue(++idx, val, timestamp);
	  EXPECT_EQ(10001, any_cast<int>(val));
	  // -> lane_id
	  out[2].getValue(++idx, val, timestamp);
	  EXPECT_EQ(0, any_cast<unsigned long long>(val));
	  // -> lane x y z
	  for(int i = 0; i < 3; i++)
	  {
	    out[2].getValue(++idx, val, timestamp);
	    EXPECT_EQ(-132768, any_cast<int>(val));
	  }
	  //  semi_major_axis_length semi_minor_axis_length  
	  for(int i = 0; i < 2; i++)
	  {
	    out[2].getValue(++idx, val, timestamp);
	    EXPECT_EQ(4095, any_cast<int>(val));
	  }
	  // -> semi_major_orientation
	  out[2].getValue(++idx, val, timestamp);
	  EXPECT_EQ(28800, any_cast<int>(val));
	  // -> altitude_accuracy
	  out[2].getValue(++idx, val, timestamp);
	  EXPECT_EQ(20001, any_cast<int>(val));
  }
  // Begin&End Position End

  // lenth
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(-132768, any_cast<int>(val));
  // lenth accurancy
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(0, any_cast<int>(val));
  // id_begin id_end
  for(int i = 0; i < 2; i++)
  {
    out[2].getValue(++idx, val, timestamp);
    EXPECT_EQ(0, any_cast<unsigned long long>(val));
  }
  
  // confidence 
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(59, any_cast<int>(val));
  // detectable_size 
  out[2].getValue(++idx, val, timestamp);
  EXPECT_EQ(60, any_cast<int>(val));
  
  // rsu_id
  out[2].getValue(++idx, val, timestamp);


  // teardown

}

