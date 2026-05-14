/**
 * @file test_socket.cpp
 * @brief socket.hppの単体テスト
 *
 */

#include<stdlib.h>
#include<gtest/gtest.h>
#include"utils_of_test.hpp"


using IOService = boost::asio::io_service;
using Udp = boost::asio::ip::udp;

/**
 * @brief コールバック関数が１回以上呼ばれることをチェックする。
 * @note  各コールバック関数の終了時にtrueに設定されます。
 *
 */
static bool callback_check;

/**
 * @brief Receiver検証のため用意するインターフェイス
 *
 */
class FakeUploadTest{
public:
  
  /**
   * @brief Receiverを実体化時、レジスト用のコールバック関数。
   * @note 渡したメッセージの長さを検証する。
   *
   */
  static void notReallyUpload(const std::vector<IS::Tuple>& v)
  {
    // check if there is any data in v
    EXPECT_GT(v.size(), 0);
    callback_check = true;
  }
};

/**
 * @brief 受信クラスがSensingMessage型で実体化してから、コールバック関数を正しく呼ばれるかどうかを検証する
 *
 * @note テスト実行する時、ネットワークと接続されていること
 *
 */
TEST(ReceiverTest, ReceiverCallbackActionTest)
{
  // set up
  
  const int p = 7777 ;
  callback_check = false;
  protobuf_dmi::utils::Config conf;
  
  std::function<void(const std::vector<IS::Tuple>)> f_p = FakeUploadTest::notReallyUpload;
  // action
  
  protobuf_dmi::Receiver<SensingMessage>::SharedPtr recv;  

  // verification
  EXPECT_NO_THROW(recv.reset(
    new protobuf_dmi::Receiver<SensingMessage>(
      p,
      {
        std::make_shared<protobuf_dmi::SensingMessageDmBridge>(protobuf_dmi::SensingMessageDmBridge(conf))
      },
      f_p
    )
  ));

  
  // wait until the recv is successfully created
  std::this_thread::sleep_for(std::chrono::milliseconds(TEST_SPIN_MSEC/2));
  
  std::string send_cmd = "ccam_cool4_sensor_io_sample_send_msg --port 7777 --rate_msec " + to_string(SEND_INTERVAL_MSEC) + " --object_info_num " + to_string(TEST_OBJ_NUM) + " &"; 
  system(send_cmd.c_str());
  
  // will be shut down after TEST_SPIN_MSEC
  std::this_thread::sleep_for(std::chrono::milliseconds(TEST_SPIN_MSEC));
  
  killProcessesByName("ccam_cool4_sensor_io_sample_send_msg");
  EXPECT_TRUE(callback_check);
  // teardown
}


