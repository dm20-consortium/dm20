/**                                                                         
 * @file test_udp_dmi_sender.cpp
 *
 * udp_dmi::SocketSenderクラスのテスト
 */
#include "udp_dmi/socket_sender.hpp"

#include <gtest/gtest.h>

using udp_dmi::SocketSender;
using cool4_api_dataclass::ByteArray;

/*
 * @brief 引数コンストラクタを用いたインスタンス化を正常に行うことができるかを検証する
 *
 * - コンストラクタの実行を確認する
 *
 */
TEST(UdpDmiSocketSender, Constructor) {
  // setup
  // exercise
  // verify
  EXPECT_NO_THROW(SocketSender::SharedPtr target(new SocketSender({
    "127.0.0.1",
    54348,
    33,
    3})));

  // teardown
  // nothing to do
}

/*
 * @brief Send関数を問題なく実行できることを検証する
 *
 * - Send関数の実行を確認する
 * - コールバック関数がないため実行確認のみ
 *
 */
TEST(UdpDmiSocketSender, Send) {
  // setup
  SocketSender::SharedPtr target(new SocketSender({
    "127.0.0.1",
    54348,
    33,
    3}));
  std::vector<ByteArray> input(2);
  input.at(0).push_back(1);
  input.at(0).push_back(2);
  input.at(0).push_back(3);
  input.at(0).push_back(4);
  input.at(1).push_back(5);
  input.at(1).push_back(6);
  input.at(1).push_back(7);
  input.at(1).push_back(8);

  // exercise
  EXPECT_NO_THROW(target->Send(input));

  // verify
  // teardown
  // nothing to do
}
