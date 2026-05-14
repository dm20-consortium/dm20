/**
 * @file test_cool4_api_dataclass_byte_extractor.cpp
 *
 * cool4_api_dataclass内のテスト
 */
#include <cstdint>
#include <algorithm>

#include "cool4_api_dataclass/byte_extractor.hpp"

#include <gtest/gtest.h>


using cool4_api_dataclass::ByteArray;

/**
 * @brief 指定された位置に基づいてバイト配列から値を取り出すことを検証する
 *
 * - {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA}のバイト配列からuint16_t, int64_tのクラスにおいてidxの位置に応じて適切な値が出ることを確認する
 *
 *       uint16_tクラス idx = 8の場合 0x99AA
 *
 *       int64_tクラス idx = 0の場合 0x1122334455667788
 *
 */
TEST(Cool4ApiDataclassByteExtractor, ExtractValue_T_1) {
  // setup
  ByteArray input = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA};
  size_t idx = 8;

  // excercise
  const auto out = cool4_api_dataclass::ExtractValue<uint16_t>(input, idx);

  // verify
  EXPECT_EQ(0x99AA, out);

  // teardown
  //nothing to do
}
TEST(Cool4ApiDataclassByteExtractor, ExtractValue_T_2) {
  // setup
  ByteArray input = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA};
  size_t idx = 0;

  // excercise
  const auto out = cool4_api_dataclass::ExtractValue<int64_t>(input, idx);

  // verify
  EXPECT_EQ(0x1122334455667788, out);

  // teardown
  //nothing to do
}

/**
 * @brief 指定された位置に基づいてバイト配列から値を取り出すことを検証する
 *
 * - {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA}のバイト配列からuint8_tのクラスにおいて想定どおりの値が取り出せること
 *
 *       uint8_tクラス idx = 0の場合 0x11
 *
 */
TEST(Cool4ApiDataclassByteExtractor, ExtractValue_uint8_t) {
  // setup
  ByteArray input = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA};
  size_t idx = 0;

  // excercise
  const auto out = cool4_api_dataclass::ExtractValue<uint8_t>(input, idx);

  // verify
  EXPECT_EQ(0x11, out);

  // teardown
  //nothing to do
}
/**
 * @brief 指定された位置に基づいてバイト配列から値を取り出すことを検証する
 *
 * - {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA}のバイト配列からint8_tのクラスにおいて想定どおりの値が取り出せること
 *
 *       int8_tクラス idx = 9の場合 0xAA
 *
 */
TEST(Cool4ApiDataclassByteExtractor, ExtractValue_int8_t) {
  // setup
  ByteArray input = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA};
  size_t idx = 9;

  // excercise
  const auto out = cool4_api_dataclass::ExtractValue<int8_t>(input, idx);

  // verify
  EXPECT_EQ((int8_t)0xAA, out);

  // teardown
  //nothing to do
}
