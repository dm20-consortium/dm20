/**
 * @file test_dmi_utils_uploader.cpp
 *
 * dmiUtls::Uploaderクラスのテスト
 */
#include <glog/logging.h>
#include "dmi_utils/uploader.hpp"

#include <gtest/gtest.h>

using dmi_utils::Uploader;
using cool4_api_dataclass::ObjectInfo;
using cool4_api_dataclass::DataClassesWithHeader;
using cool4_api_dataclass::ValueWithAccuracy;

using DataType = ObjectInfo;


/**
 * @brief 引数コンストラクタを用いたインスタンス化を正常に行うことができるかを検証する
 *
 * - DBsystemを起動しておくこと
 * - コンストラクタの実行を確認する
 *
 */
// コンストラクタにて正常系(DBsystem起動時)を通る場合
TEST(DmiUtilsUploader, Constructor_no_throw) {
  // setup
  Uploader<DataType, DataClassesWithHeader>::SharedPtr target;

  // exercise and verify
  EXPECT_NO_THROW(target.reset(new Uploader<DataType, DataClassesWithHeader>({
    "127.0.0.1",
    9002,
    "dm2sampleuser",
    "dm2samplepassword",
    "object_info_0_8_1",
    3})));

  // teardown
  // nothing to do
}
// コンストラクタにて異常系を通る場合
TEST(DmiUtilsUploader, Constructor_any_throw) {
  // setup
  Uploader<DataType, DataClassesWithHeader>::SharedPtr target;
  // exercise
  // verify
  EXPECT_ANY_THROW(target.reset(new Uploader<DataType, DataClassesWithHeader>({
    "127.0.0.1",
    9002,
    "dm2sampleuser",
    "",
    "object_info_0_8_1",
    3})));

  // teardown
  // nothing to do
}

/**
 * @brief Upload実行が想定どおりに実行できることを検証する
 *
 * - DBsystemを起動しておくこと
 * - Upload関数の実行を確認する
 * - コールバック関数がないため出力確認は不可
 *
 */
TEST(DmiUtilsUploader, Upload) {
  // setup
  Uploader<DataType, DataClassesWithHeader>::SharedPtr target(
      new Uploader<DataType, DataClassesWithHeader>({
    "127.0.0.1",
    9002,
    "dm2sampleuser",
    "dm2samplepassword",
    "object_info_0_8_1",
    3}));
  DataClassesWithHeader<DataType> input;
  input.header.version_major = 1;
  input.header.version_minor = 2;
  input.header.version_patch = 3;
  input.header.seq = 4;
  input.header.frame_id = 5;
  input.header.frame_num = 6;
  input.header.info_num = 7;
  input.dataclasses.push_back(DataType());

  std::vector<IS::Tuple> tuples;
  for(const auto& e : input.dataclasses) {
    const long stamp = DmUtil::getTimeMillisec();
    IS::Tuple tuple(DataType::ElementNum);
    uint32_t idx = 0;
    dmi_utils::DmBridge<DataType>::FromDataclass(e, stamp, idx, tuple);
    tuples.push_back(tuple);
  }

  // exercise
  // verify
  EXPECT_NO_THROW(target->Upload(tuples));

  // teardown
  // nothing to do
  
}
