/**
 * @file test_dmi_utils_creator.cpp
 *
 * dmi_utils::Creatorクラスのテスト
 */
#include <thread>
#include <chrono>

#include <gflags/gflags.h>

#include "dmi_utils/creator.hpp"
#include <cool4_api_dataclass/dataclasses_with_header.hpp>
#include <cool4_api_dataclass/object_info.hpp>

#include <gtest/gtest.h>


using dmi_utils::Creator;
using DataType = cool4_api_dataclass::ObjectInfo;

/**
 * @brief 引数ありコンストラクタを用いたインスタンス化を正常に行うことができるかを検証する
 *
 * - コンストラクタの実行を確認
 *
 */
TEST(FrameCreator, Constructor_no_throw) {
  // setup
  cool4_api_dataclass::ByteArray my_data;
  auto some_func = [&](const std::vector<cool4_api_dataclass::ByteArray>& rs){
    my_data.push_back(rs[0][0]);
  };

  // exercise
  // verify
  EXPECT_NO_THROW(Creator<DataType>::SharedPtr target(
    new Creator<DataType>({
    0,
    6,
    0,
    5},
    some_func)));

  // teardown
  // nothing to do
}

/**
 * @brief コンバートデータをarrayリストに変換することを検証する
 * - ラムダ式コールバック関数にて取得した値が想定どおり下記となることを確認する
 *       {0,6,0,0,1,1,1,1,2,3,4,1,1,2,3,4,5,6,7,8,1,0x41,0x48,0x00,0x00}
 */
TEST(FrameCreator, ToByteArrayList) {
  // setup
  std::vector<cool4_api_dataclass::ByteArray> my_data;
  auto some_func = [&](const std::vector<cool4_api_dataclass::ByteArray>& rs){
    my_data = rs;
  };

  Creator<DataType>::SharedPtr target(
    new Creator<DataType>({
    0,
    6,
    0,
    5},
    some_func));

  size_t idx = 0;
  DataType::Array input;
  input.push_back(DataType::FromByteArray(
    {1,2,3,4,1,1,2,3,4,5,6,7,8,1,0x41,0x48,0x00,0x00}, idx).get());

  // exercise
  target->ToByteArrayList(input);

  // verify
  EXPECT_EQ(229, my_data.at(0).size());
  EXPECT_EQ(0, my_data[0][0]);
  EXPECT_EQ(6, my_data[0][1]);
  EXPECT_EQ(0, my_data[0][2]);
  EXPECT_EQ(0, my_data[0][3]);
  EXPECT_EQ(1, my_data[0][4]);
  EXPECT_EQ(1, my_data[0][5]);
  EXPECT_EQ(1, my_data[0][6]);
  EXPECT_EQ(1, my_data[0][7]);
  EXPECT_EQ(2, my_data[0][8]);
  EXPECT_EQ(3, my_data[0][9]);
  EXPECT_EQ(4, my_data[0][10]);
  EXPECT_EQ(1, my_data[0][11]);
  EXPECT_EQ(1, my_data[0][12]);
  EXPECT_EQ(2, my_data[0][13]);
  EXPECT_EQ(3, my_data[0][14]);
  EXPECT_EQ(4, my_data[0][15]);
  EXPECT_EQ(5, my_data[0][16]);
  EXPECT_EQ(6, my_data[0][17]);
  EXPECT_EQ(7, my_data[0][18]);
  EXPECT_EQ(8, my_data[0][19]);
  EXPECT_EQ(1, my_data[0][20]);
  EXPECT_EQ(0x41, my_data[0][21]);
  EXPECT_EQ(0x48, my_data[0][22]);
  EXPECT_EQ(0x00, my_data[0][23]);
  EXPECT_EQ(0x00, my_data[0][24]);

  // teardown
  // nothing to do
}
