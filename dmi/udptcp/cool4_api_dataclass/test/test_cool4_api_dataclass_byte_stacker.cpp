/**
 * @file test_cool4_api_dataclass_byte_stacker.cpp
 *
 * cool4_api_dataclass内のテスト
 */
#include <cstdint>
#include <algorithm>

#include "cool4_api_dataclass/byte_stacker.hpp"

#include <gtest/gtest.h>


using cool4_api_dataclass::ByteArray;

/**
 * @brief スタック値がバイト配列に入ることの検証をする
 *
 * - uint16_t, int64_tにおいてスタック値がバイト配列に入ることの確認
 *
 *       スタック値: 0x1122 -> バイト配列: {0x11, 0x22}
 *
 *       スタック値: 0x1122334455667788 -> バイト配列: {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}
 *
 */
TEST(Cool4ApiDataclassByteStacker, StackValue_T_1) {
  // setup
  uint16_t input = 0x1122;
  ByteArray out(0);

  // exericse
  cool4_api_dataclass::StackValue<uint16_t>(input, out);

  // verify
  EXPECT_EQ(0x11, out[0]);
  EXPECT_EQ(0x22, out[1]);

  // teardown
  // nothing to do
}
TEST(Cool4ApiDataclassByteStacker, StackValue_T_2) {
  // setup
  int64_t input = 0x1122334455667788;
  ByteArray out(0);

  // exericse
  cool4_api_dataclass::StackValue<int64_t>(input, out);

  // verify
  EXPECT_EQ(0x11, out[0]);
  EXPECT_EQ(0x22, out[1]);
  EXPECT_EQ(0x33, out[2]);
  EXPECT_EQ(0x44, out[3]);
  EXPECT_EQ(0x55, out[4]);
  EXPECT_EQ(0x66, out[5]);
  EXPECT_EQ(0x77, out[6]);
  EXPECT_EQ(0x88, out[7]);

  // teardown
  // nothing to do
}

/**
 * @brief スタック値がバイト配列に入ることの検証をする
 *
 * - uint8_tにおいてスタック値がバイト配列に入ることの確認
 *
 *       スタック値: 0x11 -> バイト配列: {0x11}
 *
 */
TEST(Cool4ApiDataclassByteStacker, StackValue_uint8_t) {
  // setup
  uint8_t input = 0x11;
  ByteArray out(0);

  // exericse
  cool4_api_dataclass::StackValue<uint8_t>(input, out);

  // verify
  EXPECT_EQ(0x11, out[0]);

  // teardown
  // nothing to do
}

/**
 * @brief スタック値がバイト配列に入ることの検証をする
 *
 * - int8_tにおいてスタック値がバイト配列に入ることの確認
 *
 *       スタック値: 0x11 -> バイト配列: {0x11}
 *
 */
TEST(Cool4ApiDataclassByteStacker, StackValue_int8_t) {
  // setup
  int8_t input = 0x11;
  ByteArray out(0);

  // exericse
  cool4_api_dataclass::StackValue<int8_t>(input, out);

  // verify
  EXPECT_EQ(0x11, out[0]);

  // teardown
  // nothing to do
}
