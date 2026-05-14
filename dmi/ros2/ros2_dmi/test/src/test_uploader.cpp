#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include <rclcpp/rclcpp.hpp>

#include "ros2_dmi/uploader.hpp"
#include "ros2_dmi/downloader.hpp"
#include "test_subscriber_util.hpp"
#include "rosmsg_freespace_info.hpp"

namespace ros2_dmi {
namespace test {

/**
 * @brief テスト用データをPublishするクラス
 */
class TestDataPublisher : public rclcpp::Node {
 public:
  /**
   * @brief ros2_dmi::test::TestDataPublisherコンストラクタ
   * アップロードデータとそのpublisher作成
   */
  TestDataPublisher() : Node("test_data_publisher") {
    data_ = test_util::CreateFreespaceInfoArray();
    pub_ = this->create_publisher<FreespaceInfoArray>("upload_data", 10);
    timer_ = this->create_wall_timer(
        100ms, std::bind(&TestDataPublisher::timer_callback, this));
  }

 private:
  /**
   * @brief タイマーのコールバック関数、データをpublishする 
   */
  void timer_callback() {
    pub_->publish(data_);
  }
  FreespaceInfoArray data_;
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<FreespaceInfoArray>::SharedPtr pub_;
};

/**
 * @brief UploaderがデータをDMに送信することを検証する
 * UploaderがデータをDMに送信して、Downloaderをデータを受信する
 * データを受信した場合、Uploaderの送信が成功だと判断する
 * 
 * - 入力値: FreespaceInfoArray型のROSメッセージ
 * 
 * - 期待値: DownloaderからFreespaceInfoArray型のデータを受信する 
 */
TEST(TestUploader, TestUploader) {
  // setup
  // Uploader用パラメータの設定
  std::vector<rclcpp::Parameter> up_params = {
    rclcpp::Parameter("ip_addr", std::string("127.0.0.1")),
    rclcpp::Parameter("port", 9002),
    rclcpp::Parameter("dm_user", std::string("dm2sampleuser")),
    rclcpp::Parameter("pass", std::string("dm2samplepassword")),
    rclcpp::Parameter("target_table", std::string("freespace_info_0_8_1")),
    rclcpp::Parameter("topic_name", std::string("upload_data")),
    rclcpp::Parameter("topic_type", std::string("FreespaceInfoArray"))};
  rclcpp::NodeOptions up_options;
  up_options.parameter_overrides(up_params);

  // Downloader用のパラメータ設定
  std::vector<rclcpp::Parameter> down_params = {
    rclcpp::Parameter("station_id", 0),
    rclcpp::Parameter("ip_addr", std::string("127.0.0.1")),
    rclcpp::Parameter("port", 9001),
    rclcpp::Parameter("dm_user", std::string("dm2sampleuser")),
    rclcpp::Parameter("pass", std::string("dm2samplepassword")),
    rclcpp::Parameter("query", std::string(
        "master freespace_info_0_8_1 select * from freespace_info_0_8_1 [range 300 msec]")),
    rclcpp::Parameter("topic_name", std::string("test_freespace_info"))};
  rclcpp::NodeOptions down_options;
  down_options.parameter_overrides(down_params);

  rclcpp::init(0, 0);
  auto data_publisher = std::make_shared<TestDataPublisher>();
  auto uploader = std::make_shared<Uploader>(up_options);
  auto downloader = std::make_shared<Downloader>(down_options);
  auto sub = std::make_shared<test_util::TestSubscriber<FreespaceInfoArray>>("test_freespace_info");

  rclcpp::executors::MultiThreadedExecutor exec;
  exec.add_node(data_publisher);
  exec.add_node(uploader);
  exec.add_node(downloader);
  exec.add_node(sub);

  // exercise
  bool is_pub_received = false;
  const rclcpp::Time start_time = rclcpp::Clock(RCL_SYSTEM_TIME).now();
  rclcpp::Time end_time;
  const double subscribe_wait_time = 5.0;  // topic受信待ち時間 [sec]
  while (rclcpp::ok()) {
    exec.spin_once(std::chrono::seconds(1));

    // データが受信された場合、ループを中止
    is_pub_received = sub->ReceiveStatus();
    if (is_pub_received) break;

    // 設定された時間を超えた場合、ループを中止
    end_time = rclcpp::Clock(RCL_SYSTEM_TIME).now();
    if ((end_time - start_time).seconds() > subscribe_wait_time) break;
  }

  // verify
  EXPECT_TRUE(is_pub_received);
  EXPECT_NO_THROW(auto test_uploader = std::make_shared<Uploader>(up_options));

  // teardown
  rclcpp::shutdown();
}

/**
 * @brief 不正なIPアドレスとportが設定された時に、
 * Uploaderクラスの生成が失敗になるかどうかを検証する
 * 
 * - 入力値: ip_addrとportに不正な値を設定
 * 
 * - 期待値: runtime_errorが返却される
 */
TEST(TestUploader, ConnectFailed) {
  // setup
  std::vector<rclcpp::Parameter> up_params = {
    rclcpp::Parameter("ip_addr", std::string("wang1")),
    rclcpp::Parameter("port", "wang2"),
    rclcpp::Parameter("dm_user", std::string("dm2sampleuser")),
    rclcpp::Parameter("pass", std::string("dm2samplepassword")),
    rclcpp::Parameter("target_table", std::string("freespace_info_0_8_1")),
    rclcpp::Parameter("topic_name", std::string("upload_data")),
    rclcpp::Parameter("topic_type", std::string("FreespaceInfoArray"))};
  rclcpp::NodeOptions up_options;
  up_options.parameter_overrides(up_params);
  rclcpp::init(0, 0);

  // exercise
  // verify
  EXPECT_THROW(auto test_uploader = std::make_shared<Uploader>(up_options), std::runtime_error);

  // teardown
  rclcpp::shutdown();
}

/**
 * @brief topic_typeに不正なデータを入力する時に、
 * Uploaderクラスの生成が失敗になるかどうかのを確認
 * 
 * - 入力値: topic_typeに不正な値（”ABCInfoArray”）を設定
 * 
 * - 期待値: runtime_error
 */
TEST(TestUploader, TypeError) {
  // setup
  std::vector<rclcpp::Parameter> up_params = {
    rclcpp::Parameter("ip_addr", std::string("127.0.0.1")),
    rclcpp::Parameter("port", 9002),
    rclcpp::Parameter("dm_user", std::string("dm2sampleuser")),
    rclcpp::Parameter("pass", std::string("dm2samplepassword")),
    rclcpp::Parameter("target_table", std::string("object_info")),
    rclcpp::Parameter("topic_name", std::string("upload_data")),
    rclcpp::Parameter("topic_type", std::string("ABCInfoArray"))};
  rclcpp::NodeOptions up_options;
  up_options.parameter_overrides(up_params);
  rclcpp::init(0, 0);

  // exercise
  // verify
  EXPECT_THROW(auto test_uploader = std::make_shared<Uploader>(up_options), std::runtime_error);

  // teardown
  rclcpp::shutdown();
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

}   // namespace test
}   // namespace ros2_dmi
