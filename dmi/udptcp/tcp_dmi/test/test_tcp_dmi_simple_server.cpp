/**
 * @file test_tcp_dmi_simple_server.cpp
 *
 * tcp_dmi::SimpleServerクラスのテスト
 */
#include "tcp_dmi/simple_server.hpp"
#include <cool4_api_dataclass/signal_info.hpp>
#include <cool4_api_dataclass/dataclasses_with_header.hpp>

#include <gflags/gflags.h>

#include <thread>
#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/optional.hpp>

#include <iostream>

using cool4_api_dataclass::ByteArray;
using cool4_api_dataclass::DataClassesWithHeader;
using cool4_api_dataclass::SignalInfo;
using cool4_api_dataclass::Header;
using tcp_dmi::SimpleServer;
using ErrorCode = boost::system::error_code;

using DataType = Header;

#define PORT_NUM  54348

/**
 * @brief コンストラクタを用いたインスタンス化を正常に行うことができるかを検証する
 *
 * - 例外がないこと
 * - DBsystemを起動しておくこと
 * - コンストラクタの実行を確認する
 *
 */
TEST(TcpDmiSimpleServer, Constructor) {
  // setup
  using setServer = SimpleServer<DataType, DataClassesWithHeader>;
  setServer::MessageCallbackType on_message = [](const DataClassesWithHeader<DataType>&){};
  setServer::DisconnectionCallbackType on_disconnect = [](){};

  // exercise
  // verify
  EXPECT_NO_THROW(setServer::SharedPtr target(new SimpleServer<DataType, DataClassesWithHeader>(PORT_NUM, on_message, on_disconnect)));
  
  // teadown
  // nothing to do
}

/**
 * @brief Run実行が想定どおりに実行できることを検証する
 *
 * - DBsystemを起動しておくこと
 * - Run関数の実行を確認する
 *
 */
TEST(TcpDmiSimpleServer, Run) {
  // setup
  using setServer = SimpleServer<DataType, DataClassesWithHeader>;
  setServer::MessageCallbackType on_message = [](const DataClassesWithHeader<DataType>&){};
  setServer::DisconnectionCallbackType on_disconnect = [](){};

  SimpleServer<DataType, DataClassesWithHeader>::SharedPtr target(new SimpleServer<DataType, DataClassesWithHeader>(PORT_NUM, on_message, on_disconnect));

  // exercise
  std::thread th([&]() {
    target->Run();
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  target->Stop();
  th.join();
  
  // verify
  SUCCEED();

  // exercise
  // teardown
  // nothing to do
}

/**
 *@brief Reset実行が想定どおりに実行できることを検証する
 *
 * - DBsystemを起動しておくこと
 * - Reset関数の実行を確認する
 *
 */
TEST(TcpDmiSimpleServer, Reset) {
  // setup
  using setServer = SimpleServer<DataType, DataClassesWithHeader>;
  setServer::MessageCallbackType on_message = [](const DataClassesWithHeader<DataType>&){};
  setServer::DisconnectionCallbackType on_disconnect = [](){};

  SimpleServer<DataType, DataClassesWithHeader>::SharedPtr target(new SimpleServer<DataType, DataClassesWithHeader>(PORT_NUM, on_message, on_disconnect));

  // exercise
  // verify
  EXPECT_NO_THROW(target->Reset());

  // teardown
  // nothing to do
}

/**
 *@brief OnAccept実行が想定どおりに実行できることを検証する
 *
 * - DBsystemを起動しておくこと
 * - Reset関数の実行を確認する
 *
 */
TEST(TcpDmiSimpleServer, OnAcceptNoError) {
  // setup
  using setServer = SimpleServer<DataType, DataClassesWithHeader>;
  setServer::MessageCallbackType on_message = [](const DataClassesWithHeader<DataType>&){};
  setServer::DisconnectionCallbackType on_disconnect = [](){};

  SimpleServer<DataType, DataClassesWithHeader>::SharedPtr target(new SimpleServer<DataType, DataClassesWithHeader>(PORT_NUM, on_message, on_disconnect));

  // exercise
  ErrorCode err = boost::system::error_code();

  // verify
  EXPECT_NO_THROW(target->OnAccept(err));

  // teardown
  // nothing to do
}

TEST(TcpDmiSimpleServer, OnAcceptError) {
  // setup
  using setServer = SimpleServer<DataType, DataClassesWithHeader>;
  setServer::MessageCallbackType on_message = [](const DataClassesWithHeader<DataType>&){};
  setServer::DisconnectionCallbackType on_disconnect = [](){};

  SimpleServer<DataType, DataClassesWithHeader>::SharedPtr target(new SimpleServer<DataType, DataClassesWithHeader>(PORT_NUM, on_message, on_disconnect));

  // exercise
  ErrorCode err = boost::system::error_code();
  err = boost::asio::error::eof;

  // verify
  EXPECT_NO_THROW(target->OnAccept(err));

  // teardown
  // nothing to do
}

TEST(TcpDmiSimpleServer, MessageEventError) {
  // setup
  DataClassesWithHeader<DataType> signal_info;
  uint8_t msg_num = 0;
  uint8_t disconnect_msg = 0;

  using setServer = SimpleServer<DataType, DataClassesWithHeader>;
  setServer::MessageCallbackType on_message = [&](const DataClassesWithHeader<DataType>& rs)
  {
    signal_info = rs;
    msg_num = 1;
  };
  setServer::DisconnectionCallbackType on_disconnect = [&]()
  {
    disconnect_msg = 1;
  };

  SimpleServer<DataType, DataClassesWithHeader>::SharedPtr target(new SimpleServer<DataType, DataClassesWithHeader>(PORT_NUM, on_message, on_disconnect));

  // exercise
  ErrorCode err = boost::system::error_code();
  err = boost::asio::error::fault;

  // verify
  EXPECT_NO_THROW(target->MessageEvent(err, 0));
  EXPECT_EQ(0, msg_num);
  EXPECT_EQ(0, disconnect_msg);

  // teardown
  // nothing to do
}

TEST(TcpDmiSimpleServer, MessageEventEof) {
  // setup
  DataClassesWithHeader<DataType> signal_info;
  uint8_t disconnect_msg = 0;
  uint8_t msg_num = 0;

  using setServer = SimpleServer<DataType, DataClassesWithHeader>;
  setServer::MessageCallbackType on_message = [&](const DataClassesWithHeader<DataType>& rs)
  {
    signal_info = rs;
    msg_num = 1;
  };
  setServer::DisconnectionCallbackType on_disconnect = [&]()
  {
    disconnect_msg = 1;
  };

  SimpleServer<DataType, DataClassesWithHeader>::SharedPtr target(new SimpleServer<DataType, DataClassesWithHeader>(PORT_NUM, on_message, on_disconnect));

  // exercise
  ErrorCode err = boost::system::error_code();
  err = boost::asio::error::eof;

  // verify
  EXPECT_NO_THROW(target->MessageEvent(err, 0));
  EXPECT_EQ(0, msg_num);
  EXPECT_EQ(1, disconnect_msg);

  // teardown
  // nothing to do
}

TEST(TcpDmiSimpleServer, MessageEventNoFrame) {
  // setup
  DataClassesWithHeader<DataType> signal_info;
  uint8_t disconnect_msg = 0;
  uint8_t msg_num = 0;

  using setServer = SimpleServer<DataType, DataClassesWithHeader>;
  setServer::MessageCallbackType on_message = [&](const DataClassesWithHeader<DataType>& rs)
  {
    signal_info = rs;
    msg_num = 1;
  };
  setServer::DisconnectionCallbackType on_disconnect = [&]()
  {
    disconnect_msg = 1;
  };

  SimpleServer<DataType, DataClassesWithHeader>::SharedPtr target(new SimpleServer<DataType, DataClassesWithHeader>(PORT_NUM, on_message, on_disconnect));

  // exercise
  ErrorCode err = boost::system::error_code();
  err.clear();

  // verify
  EXPECT_NO_THROW(target->MessageEvent(err, 0));
  EXPECT_EQ(0, msg_num);
  EXPECT_EQ(0, disconnect_msg);

  // teardown
  // nothing to do
}

TEST(TcpDmiSimpleServer, MessageEventSuccess) {
  // setup
  DataClassesWithHeader<DataType> signal_info;
  uint8_t disconnect_msg = 0;
  uint8_t msg_num = 0;

  using setServer = SimpleServer<DataType, DataClassesWithHeader>;
  setServer::MessageCallbackType on_message = [&](const DataClassesWithHeader<DataType>& rs)
  {
    signal_info = rs;
    msg_num = 1;
  };
  setServer::DisconnectionCallbackType on_disconnect = [&]()
  {
    disconnect_msg = 1;
  };

  SimpleServer<DataType, DataClassesWithHeader>::SharedPtr target(new SimpleServer<DataType, DataClassesWithHeader>(PORT_NUM, on_message, on_disconnect));

  // exercise
  ErrorCode err = boost::system::error_code();
  err.clear();

  // verify
  EXPECT_NO_THROW(target->MessageEvent(err, 7));
  EXPECT_EQ(1, msg_num);
  EXPECT_EQ(0, disconnect_msg);

  // teardown
  // nothing to do
}
