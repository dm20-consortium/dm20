/**                                                                         
 * @file test_udp_dmi_receiver.cpp
 *
 * udp_dmi::SocketReceiverクラスのテスト
 */
#include "udp_dmi/socket_receiver.hpp"
#include <cool4_api_dataclass/object_info.hpp>

#include <gtest/gtest.h>


using udp_dmi::SocketReceiver;
using cool4_api_dataclass::ObjectInfo;
using cool4_api_dataclass::DataClassesWithHeader;

/**
 * @brief 引数付きコンストラクタを用いたインスタンス化を正常に行うことができるかを検証する
 *
 * - コンストラクタの実行を確認する
 *
 */
TEST(UdpDmiSocketReceiver, Constructor) {
  // setup
  DataClassesWithHeader<ObjectInfo> my_data;
  auto some_func = [&](const DataClassesWithHeader<ObjectInfo>& rs){
    my_data = rs;
  };
  SocketReceiver<ObjectInfo, DataClassesWithHeader>::SharedPtr target;

  // exercise
  // verify
  EXPECT_NO_THROW(target.reset(new SocketReceiver<ObjectInfo, DataClassesWithHeader>(
    54348,
    some_func)));

  // teardown
  // nothing to do
}
