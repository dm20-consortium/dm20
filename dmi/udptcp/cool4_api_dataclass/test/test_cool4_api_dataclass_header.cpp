/**
 * @file test_cool4_api_dataclass_header.cpp
 *
 * cool4_api_dataclass::Headerクラスのテスト
 */
#include "cool4_api_dataclass/header.hpp"

#include <gtest/gtest.h>


using cool4_api_dataclass::Header;
using cool4_api_dataclass::ByteArray;

/**
 * @brief デフォルトコンストラクタを用いたインスタンス化を正常に行うことができるかを検証する
 *
 * - 例外がないこと
 * - 初期値が想定どおり(すべて0)であること
 *
 */
TEST(Cool4ApiDataclassHeader, DefaultConstructor) {
  // setup
  auto target = Header::SharedPtr();

  // exercise
  ASSERT_NO_THROW(target.reset(new Header()));

  // verify
  EXPECT_EQ(0, target->version_major);
  EXPECT_EQ(0, target->version_minor);
  EXPECT_EQ(0, target->version_patch);
  EXPECT_EQ(0, target->seq);
  EXPECT_EQ(0, target->frame_id);
  EXPECT_EQ(0, target->frame_num);
  EXPECT_EQ(0, target->info_num);

  // teardown
  // nothing to do
}

/**
 * @brief 引数ありコンストラクタを用いたインスタンス化を正常に行うことができるかを検証する
 *
 * - 例外がないこと
 *
 * - {1, 2, 3, 4, 5, 6, 7}を与えた場合，下記の通りになること
 *
 *       version_major: 1
 *       version_minor: 2
 *       version_patch: 3
 *       seq: 4
 *       frame_id: 5
 *       frame_num: 6
 *       info_num: 7
 *
 */
TEST(Cool4ApiDataclassHeader, Constructor) {
  // setup
  auto target = Header::SharedPtr();

  // exercise
  ASSERT_NO_THROW(target.reset(new Header({1, 2, 3, 4, 5, 6, 7})));

  // verify
  EXPECT_EQ(1, target->version_major);
  EXPECT_EQ(2, target->version_minor);
  EXPECT_EQ(3, target->version_patch);
  EXPECT_EQ(4, target->seq);
  EXPECT_EQ(5, target->frame_id);
  EXPECT_EQ(6, target->frame_num);
  EXPECT_EQ(7, target->info_num);

  // teardown
  // nothing to do
}

/**
 * @brief バイト配列への変換処理が正しく機能するか
 *
 * - {1, 2, 3, 4, 5, 6, 7}を与えたときバイト配列が{1, 2, 3, 4, 5, 6, 7}となること
 *
 */
TEST(Cool4ApiDataclassHeader, ToByteArray) {
  // setup
  Header::SharedPtr target(new Header({1, 2, 3, 4, 5, 6, 7}));

  // exercise
  const auto out = target->ToByteArray();

  //verify
  EXPECT_NE(boost::none, out);
  const auto contents = out.get();
  EXPECT_EQ(7, static_cast<int>(contents.size()));
  EXPECT_EQ(1, contents[0]);
  EXPECT_EQ(2, contents[1]);
  EXPECT_EQ(3, contents[2]);
  EXPECT_EQ(4, contents[3]);
  EXPECT_EQ(5, contents[4]);
  EXPECT_EQ(6, contents[5]);
  EXPECT_EQ(7, contents[6]);

  // teardown
  // nothing to do
}

/**
 * @brief バイト配列からのインスタンス化処理が正しく機能するかどうか
 *
 * - {1, 2, 3, 4, 5, 6, 7}を入力したとき以下のような値でインスタンス化されること
 *
 *       version_major: 1
 *       version_minor: 2
 *       version_patch: 3
 *       seq: 4
 *       frame_id: 5
 *       frame_num: 6
 *       info_num: 7
 *
 */
TEST(Cool4ApiDataclassHeader, FromByteArray) {
  // setup
  ByteArray input = {1, 2, 3, 4, 5, 6, 7};
  size_t idx = 0;

  // exercise
  const auto out = Header::FromByteArray(input, idx);

  // verify
  EXPECT_NE(boost::none, out);
  const auto contents = out.get();
  EXPECT_EQ(1, contents.version_major);
  EXPECT_EQ(2, contents.version_minor);
  EXPECT_EQ(3, contents.version_patch);
  EXPECT_EQ(4, contents.seq);
  EXPECT_EQ(5, contents.frame_id);
  EXPECT_EQ(6, contents.frame_num);
  EXPECT_EQ(7, contents.info_num);

  // teardown
  // nothing to do
}

/**
 * @brief 出力ストリームへの表示が正しくできること
 *
 * - 以下のような値をもつHeaderのインスタンスをストリームへわたす
 *
 *       version_major: 1
 *       version_minor: 2
 *       version_patch: 3
 *       seq: 4
 *       frame_id: 5
 *       frame_num: 6
 *       info_num: 7
 *
 * - ストリームの内容が1,2,3,4,5,6,7,であること
 */
TEST(Cool4ApiDataclassHeader, Print) {
  // setup
  std::stringbuf buf;
  std::streambuf* prev = std::cout.rdbuf(&buf);
  auto target = Header(1,2,3,4,5,6,7);

  // exercise
  std::cout << target;

  // verify
  std::cout.rdbuf(prev);
  EXPECT_EQ("1,2,3,4,5,6,7,",buf.str());

  // teardown
  // nothing to do
}
