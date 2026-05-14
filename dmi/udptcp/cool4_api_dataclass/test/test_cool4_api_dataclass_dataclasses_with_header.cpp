/**
 * @file test_cool4_api_dataclass_dataclasses_with_header.cpp
 *
 * cool4_api_dataclass::DataClassWithHeader<T>クラスのテスト
 */
#include "cool4_api_dataclass/dataclasses_with_header.hpp"
#include "cool4_api_dataclass/object_info.hpp"

#include <gtest/gtest.h>


using cool4_api_dataclass::DataClassesWithHeader;
using cool4_api_dataclass::ByteArray;
using cool4_api_dataclass::ValueWithAccuracy;

/**
 * @brief デフォルトコンストラクタを用いたインスタンス化を正常に行うことができるかを検証する
 *
 * - 例外がないこと
 * - 初期値が想定どおり(Header,vectorがデフォルト)であること
 * - 型指定のvectorが別パターンの型を入れることができること
 *
 */
TEST(Cool4ApiDataclassDataClassesWithHeader, DefaultConstructor) {
  // setup
  uint8_t tmp1;
  int32_t tmp2;
  const std::type_info& infoA = typeid(tmp1);
  const std::type_info& infoB = typeid(tmp2);

  //exercise
  auto target1 = DataClassesWithHeader<uint8_t>();
  auto target2 = DataClassesWithHeader<int32_t>();

  //verify
  EXPECT_EQ(0, target1.header.version_major);
  EXPECT_EQ(0, target1.header.version_minor);
  EXPECT_EQ(0, target1.header.version_patch);
  EXPECT_EQ(0, target1.header.seq);
  EXPECT_EQ(0, target1.header.frame_id);
  EXPECT_EQ(0, target1.header.frame_num);
  EXPECT_EQ(0, target1.header.info_num);
  EXPECT_EQ(infoA, typeid(target1.dataclasses[0]));
  EXPECT_EQ(infoB, typeid(target2.dataclasses[0]));

  //teardown
  //nothing to do
}

/**
 * @brief バイト配列への変換処理が正しく機能するか
 *
 * - 以下のような値を与えたときバイト配列が{1,2,3,4,5,6,7,8,9}となること
 *
 *       version_major: 1
 *       version_minor: 2
 *       version_patch: 3
 *       seq: 4
 *       frame_id: 5
 *       frame_num: 6
 *       info_num: 7
 *       dataclasses<ValueWithAccuracy<uint8_t,int8_t>>: (8,9)
 *
 * - 以下のような値を与えたときバイト配列が{1,2,3,4,5,6,7,8,9,10,11,12,13}となること
 *
 *       version_major: 1
 *       version_minor: 2
 *       version_patch: 3
 *       seq: 4
 *       frame_id: 5
 *       frame_num: 6
 *       info_num: 7
 *       dataclasses<ValueWithAccuracy<uint8_t,int8_t>>: (8,9),(10,11),(12,13)
 *
 */
TEST(Cool4ApiDataclassDataClassesWithHeader, ToByteArray1) {
  // setup
  DataClassesWithHeader<ValueWithAccuracy<uint8_t,int8_t>> target = DataClassesWithHeader<ValueWithAccuracy<uint8_t,int8_t>>();
  target.header.version_major = 1;
  target.header.version_minor = 2;
  target.header.version_patch = 3;
  target.header.seq = 4;
  target.header.frame_id = 5;
  target.header.frame_num = 6;
  target.header.info_num = 7;
  target.dataclasses.push_back(ValueWithAccuracy<uint8_t,int8_t>(8,9));

  // exercise
  auto out = target.ToByteArray();

  // verify
  EXPECT_NE(boost::none, out);
  const auto contents = out.get();
  EXPECT_EQ(9, static_cast<int>(contents.size()));
  EXPECT_EQ(1, contents[0]);
  EXPECT_EQ(2, contents[1]);
  EXPECT_EQ(3, contents[2]);
  EXPECT_EQ(4, contents[3]);
  EXPECT_EQ(5, contents[4]);
  EXPECT_EQ(6, contents[5]);
  EXPECT_EQ(7, contents[6]);
  EXPECT_EQ(8, contents[7]);
  EXPECT_EQ(9, contents[8]);

  // teadown
  // nothing to do
}
TEST(Cool4ApiDataclassDataClassesWithHeader, ToByteArray2) {
  // setup
  DataClassesWithHeader<ValueWithAccuracy<uint8_t,int8_t>> target = DataClassesWithHeader<ValueWithAccuracy<uint8_t,int8_t>>();
  target.header.version_major = 1;
  target.header.version_minor = 2;
  target.header.version_patch = 3;
  target.header.seq = 4;
  target.header.frame_id = 5;
  target.header.frame_num = 6;
  target.header.info_num = 7;
  target.dataclasses.push_back(ValueWithAccuracy<uint8_t,int8_t>(8,9));
  target.dataclasses.push_back(ValueWithAccuracy<uint8_t,int8_t>(10,11));
  target.dataclasses.push_back(ValueWithAccuracy<uint8_t,int8_t>(12,13));

  // exercise
  auto out = target.ToByteArray();

  // verify
  EXPECT_NE(boost::none, out);
  const auto contents = out.get();
  EXPECT_EQ(13, static_cast<int>(contents.size()));
  EXPECT_EQ(1,contents[0]);
  EXPECT_EQ(2,contents[1]);
  EXPECT_EQ(3,contents[2]);
  EXPECT_EQ(4,contents[3]);
  EXPECT_EQ(5,contents[4]);
  EXPECT_EQ(6,contents[5]);
  EXPECT_EQ(7,contents[6]);
  EXPECT_EQ(8,contents[7]);
  EXPECT_EQ(9,contents[8]);
  EXPECT_EQ(10,contents[9]);
  EXPECT_EQ(11,contents[10]);
  EXPECT_EQ(12,contents[11]);
  EXPECT_EQ(13,contents[12]);

  // teadown
  // nothing to do
}

/**
 * @brief バイト配列からのインスタンス化処理が正しく機能するかどうか
 *
 * - {1,2,3,4,5,6,7,8,9,10,11}を入力したとき以下のような値でインスタンス化されること
 *
 *       header.version_major: 1
 *       header.version_minor: 2
 *       header.version_patch: 3
 *       header.seq: 4
 *       header.frame_id: 5
 *       header.frame_num: 6
 *       header.info_num: 7
 *       dataclasses[0].value: 8
 *       dataclasses[0].accuracy: 9
 *       dataclasses[1].value: 10
 *       dataclasses[1].accuracy: 11
 *
 */
TEST(Cool4ApiDataclassDataClassesWithHeader, FromByteArray) {
  // setup
  ByteArray input = {1,2,3,4,5,6,7,8,9,10,11};

  // exercise
  const auto out = DataClassesWithHeader<ValueWithAccuracy<uint8_t,int8_t>>::FromByteArray(input);

  // verify
  EXPECT_NE(boost::none, out);
  const auto contents = out.get();
  EXPECT_EQ(1, contents.header.version_major);
  EXPECT_EQ(2, contents.header.version_minor);
  EXPECT_EQ(3, contents.header.version_patch);
  EXPECT_EQ(4, contents.header.seq);
  EXPECT_EQ(5, contents.header.frame_id);
  EXPECT_EQ(6, contents.header.frame_num);
  EXPECT_EQ(7, contents.header.info_num);
  EXPECT_EQ(8, contents.dataclasses[0].value);
  EXPECT_EQ(9, contents.dataclasses[0].accuracy);
  EXPECT_EQ(10, contents.dataclasses[1].value);
  EXPECT_EQ(11, contents.dataclasses[1].accuracy);

  // teardown
  // nothing to do
}

/**
 * @brief 出力ストリームへの表示が正しくできること
 *
 * - 以下のような値をもつDataClassesWithHeaderのインスタンスをストリームへわたす
 *
 *       header.version_major: 1
 *       header.version_minor: 2
 *       header.version_patch: 3
 *       header.seq: 4
 *       header.frame_id: 5
 *       header.frame_num: 6
 *       header.info_num: 7
 *       dataclasses[0].value: 8
 *       dataclasses[0].accuracy: 9
 *
 * - ストリームの内容が1,2,3,4,5,6,7,8,9,であること
 */
TEST(Cool4ApiDataclassDataClassesWithHeader, Print) {
  // setup
  auto target = DataClassesWithHeader<ValueWithAccuracy<uint8_t,int8_t>>();
  target.header.version_major = 1;
  target.header.version_minor = 2;
  target.header.version_patch = 3;
  target.header.seq = 4;
  target.header.frame_id = 5;
  target.header.frame_num = 6;
  target.header.info_num = 7;
  target.dataclasses.push_back(ValueWithAccuracy<uint8_t,int8_t>(8,9));
  std::stringbuf buf;
  std::streambuf*prev = std::cout.rdbuf(&buf);

  // exercise
  std::cout << target;

  // verify
  EXPECT_EQ("1,2,3,4,5,6,7,8,9,", buf.str());

  // teardown
  std::cout.rdbuf(prev);
}

  //
