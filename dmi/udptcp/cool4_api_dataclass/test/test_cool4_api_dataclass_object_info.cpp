/**
 * @file test_cool4_api_dataclass_object_info.cpp
 *
 * cool4_api_dataclass::ObjectClassクラスのテスト
 * cool4_api_dataclass::ValueWithAccuracyクラスのテスト
 */
#include "cool4_api_dataclass/object_info.hpp"

#include <gtest/gtest.h>


using cool4_api_dataclass::ObjectClass;
using cool4_api_dataclass::ValueWithAccuracy;
using cool4_api_dataclass::ByteArray;

/**
 * @brief デフォルトコンストラクタを用いたインスタンス化を正常に行うことができるかを検証する
 *
 * - 例外がないこと
 * - 初期値が想定どおり(すべて0)であること
 *
 */
TEST(Cool4ApiDataclassObjectClass, DefaultConstructor) {
  // exercise
  const auto target = ObjectClass();

  // verify
  EXPECT_EQ(0, target.id);
  EXPECT_EQ(0, target.confidence);
  EXPECT_EQ(0, target.subclass_type);
  EXPECT_EQ(0, target.subclass_confidence);

  // teardown
  // nothing to do
}

/**
 * @brief バイト配列への変換処理が正しく機能するか
 *
 * - 以下のような値を与えたときバイト配列が{1, 2, 3, 4}となること
 *
 *        id: 1
 *        confidence: 2
 *        subclass_type: 3
 *        subclass_confidence: 4
 *
 */
TEST(Cool4ApiDataclassObjectClass, ToByteArray) {
  // setup
  auto target = ObjectClass();
  target.id = 1;
  target.confidence = 2;
  target.subclass_type = 3;
  target.subclass_confidence = 4;

  // exercise
  const auto out = target.ToByteArray();

  // verify
  EXPECT_NE(boost::none, out);
  const auto contents = out.get();
  EXPECT_EQ(4, static_cast<int>(contents.size()));
  EXPECT_EQ(1, contents[0]);
  EXPECT_EQ(2, contents[1]);
  EXPECT_EQ(3, contents[2]);
  EXPECT_EQ(4, contents[3]);

  //teardown
  //nothing to do
}

/**
 * @brief バイト配列からのインスタンス化処理が正しく機能するかどうか
 *
 * - {1, 2, 3, 4}を入力したとき以下のような値でインスタンス化されること
 *
 *       id: 1
 *       confidence: 2
 *       subclass_type: 3
 *       subclass_confidence: 4
 *
 */
TEST(Cool4ApiDataclassObjectClass, FromByteArray) {
  // setup
  ByteArray input = {1, 2, 3, 4};
  size_t idx = 0;

  // exercise
  const auto out = ObjectClass::FromByteArray(input, idx);

  // verify
  EXPECT_NE(boost::none, out);
  const auto contents = out.get();
  EXPECT_EQ(1, contents.id);
  EXPECT_EQ(2, contents.confidence);
  EXPECT_EQ(3, contents.subclass_type);
  EXPECT_EQ(4, contents.subclass_confidence);

  // teardown
  // nothing to do
}

/**
 * @brief 出力ストリームへの表示が正しくできること
 *
 * - 以下のような値をもつObjectClassのインスタンスをストリームへわたす
 *
 *       id: 1
 *       confidence: 2
 *       subclass_type: 3
 *       subclass_confidence: 4
 *
 * - ストリームの内容が1,2,3,4,であること
 */
TEST(Cool4ApiDataclassObjectClass, Print) {
  // setup
  std::stringbuf buf;
  std::streambuf* prev = std::cout.rdbuf(&buf);
  ObjectClass target = ObjectClass();
  target.id = 1;
  target.confidence = 2;
  target.subclass_type = 3;
  target.subclass_confidence = 4;
  
  // exercize
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
 * - 想定どおりのデータ型になっていること
 * - 初期値が想定どおり(すべて0)であること
 *
 */
TEST(Cool4ApiDataclassValueWithAccuracy, DefaultConstructor1) {
  // <uint8_t,int32_t>で確認
  // setup
  uint8_t tmp1;
  int32_t tmp2;
  const std::type_info& infoA = typeid(tmp1);
  const std::type_info& infoB = typeid(tmp2);

  // exercise
  auto target = ValueWithAccuracy<uint8_t,int32_t>();

  // verify
  EXPECT_EQ(0, target.value);
  EXPECT_EQ(0, target.accuracy);
  EXPECT_EQ(infoA, typeid(target.value));
  EXPECT_EQ(infoB, typeid(target.accuracy));

  //teardown
  // nothing to do
}
TEST(Cool4ApiDataclassValueWithAccuracy, DefaultConstructor2) {
  // <int32_t,uint8_t>で確認
  // setup
  int32_t tmp1;
  uint8_t tmp2;
  const std::type_info& infoA = typeid(tmp1);
  const std::type_info& infoB = typeid(tmp2);

  // exercise
  auto target = ValueWithAccuracy<int32_t,uint8_t>();

  // verify
  EXPECT_EQ(0, target.value);
  EXPECT_EQ(0, target.accuracy);
  EXPECT_EQ(infoA, typeid(target.value));
  EXPECT_EQ(infoB, typeid(target.accuracy));

  //teardown
  //nothing to do
}

/**
 * @brief 引数ありコンストラクタを用いたインスタンス化を正常に行うことができるかを検証する
 *
 * - 想定どおりのデータ型になっていること
 *
 * - (1,2)を与えた場合，下記の通りになること
 *
 *       value: 1
 *       accuracy: 2
 *
 */
TEST(Cool4ApiDataclassValueWithAccuracy, Constructor1) {
  // <uint8_t,int32_t>で確認
  // setup
  uint8_t tmp1;
  int32_t tmp2;
  const std::type_info& infoA = typeid(tmp1);
  const std::type_info& infoB = typeid(tmp2);

  // exercise
  auto target = ValueWithAccuracy<uint8_t,int32_t>(1,2);

  // verify
  EXPECT_EQ(1, target.value);
  EXPECT_EQ(2, target.accuracy);
  EXPECT_EQ(infoA, typeid(target.value));
  EXPECT_EQ(infoB, typeid(target.accuracy));

  //teardown
  //nothing to do
}
TEST(Cool4ApiDataclassValueWithAccuracy, Constructor2) {
  // <int32_t,uint8_t>で確認
  // setup
  int32_t tmp1;
  uint8_t tmp2;
  const std::type_info& infoA = typeid(tmp1);
  const std::type_info& infoB = typeid(tmp2);

  // exercise
  auto target = ValueWithAccuracy<int32_t,uint8_t>(1,2);

  // verify
  EXPECT_EQ(1, target.value);
  EXPECT_EQ(2, target.accuracy);
  EXPECT_EQ(infoA, typeid(target.value));
  EXPECT_EQ(infoB, typeid(target.accuracy));

  //teardown
  //nothing to do
}

/**
 * @brief バイト配列への変換処理が正しく機能するか
 *
 * - 以下の値を与えた場合、バイト配列が{0,0,0,1,2}となること
 *
 *       Value<int32_t>: 1
 *       Accuracy<uint8_t>: 2
 *
 */
TEST(Cool4ApiDataclassValueWithAccuracy, ToByteArray) {
  // setup
  auto target = ValueWithAccuracy<int32_t,uint8_t>(1,2);

  // exercise
  const auto out = target.ToByteArray();

  // verify
  EXPECT_NE(boost::none, out);
  const auto contents = out.get();
  EXPECT_EQ(5, static_cast<int>(contents.size()));
  EXPECT_EQ(0, contents[0]);
  EXPECT_EQ(0, contents[1]);
  EXPECT_EQ(0, contents[2]);
  EXPECT_EQ(1, contents[3]);
  EXPECT_EQ(2, contents[4]);

  //teardown
  //nothing to do
}

/**
 * @brief バイト配列からのインスタンス化処理が正しく機能するかどうか
 *
 * - バイト配列が{1,2,3,4}のとき、以下のデータ型であれば以下の値になること
 *
 *       value<int16_t>: 258
 *       accuracy<uint16_t>: 772
 *
 */
TEST(Cool4ApiDataclassValueWithAccuracy, FromByteArray) {
  // setup
  ByteArray input = {1, 2, 3, 4};
  size_t idx = 0;

  // exercise
  const auto out = ValueWithAccuracy<int16_t, uint16_t>::FromByteArray(input, idx);

  // verify
  EXPECT_NE(boost::none, out);
  const auto contents = out.get();
  EXPECT_EQ(258, contents.value);
  EXPECT_EQ(772, contents.accuracy);

  // teardown
  // nothing to do
}

/**
 * @brief 出力ストリームへの表示が正しくできること
 *
 * - 以下のような値をもつValueWithAccuracyのインスタンスをストリームへわたす
 *
 *       value: 1
 *       accuracy: 2
 *
 * - ストリームの内容が1,2,であること
 */
TEST(Cool4ApiDataclassValueWithAccuracy, Print) {
  // setup
  auto target = ValueWithAccuracy<int32_t,uint8_t>(1,2);
  std::stringbuf buf;
  std::streambuf* prev = std::cout.rdbuf(&buf);
  
  // exercise
  std::cout << target;

  // verify
  EXPECT_EQ("1,2,", buf.str());

  // teardown
  std::cout.rdbuf(prev);
}
