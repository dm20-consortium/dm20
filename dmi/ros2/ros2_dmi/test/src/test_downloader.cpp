#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include <is/DmManager.h>
#include <is/Tuple.h>
#include <rclcpp/rclcpp.hpp>
#include "std_msgs/msg/string.hpp"

#include "ros2_dmi/downloader.hpp"
#include "test_subscriber_util.hpp"
#include "rosmsg_freespace_info.hpp"
#include "ros2_dmi/freespace_info_converter.hpp"

namespace ros2_dmi {
namespace test {

/**
 * @brief テストデータをDMにアップロードするクラス
*/
class TestUploader : public rclcpp::Node {
 public:
  /**
   * @brief ros2_dmi::test::TestUploaderコンストラクター
  */
  TestUploader() : Node("test_data_uploader") {
    try {
      socket_ = DmManager::getDatagramSocket("127.0.0.1", 9002, "dm2sampleuser", "dm2samplepassword",
                                             false);
    } catch (const ConnectionFailedException& e) {
    throw std::runtime_error(e.getMessage());
    }
    FreespaceInfoArray test_in = test_util::CreateFreespaceInfoArray();
    const long now = DmUtil::getTimeMillisec();
    tuples_ = Converter<FreespaceInfoArray>::RosToTuple(test_in, now);

    pub_ = this->create_publisher<std_msgs::msg::String>("test_uploader", 10);
    timer_ = this->create_wall_timer(
        100ms, std::bind(&TestUploader::timer_callback, this));
  }

 private:
 /**
  * @brief DMにデータを送信するタイマーコールバック関数
 */
  void timer_callback() {
    auto msg = std_msgs::msg::String();
    msg.data = "uploading";
    socket_->sendStreamData("freespace_info_0_8_1", tuples_);
    pub_->publish(msg);
  }

  DatagramSocket* socket_;
  std::vector<IS::Tuple> tuples_;
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
};

/**
 * @brief Downloaderクラス生成を検証する
 * - 入力値:
 *  - topic name: test_freespace_info
 *  - 他のパラメータはlaunchファイルと同じ:
 *    - station_id: 0
 *    - ip_addr: 127.0.0.1
 *    - port: 9001
 *    - dm_user: dm2sampleuser
 *    - pass: dm2samplepassword
 *    - query: master freespace_info select * from freespace_info [range 300 msec]
 *
 * - 期待する結果:
 *  - Downloaderクラスが正常に生成できること
 *  - ノードのpublisherを作成し、FreespaceInfoメッセージを発行すること（受信ノードで確認）
 */
TEST(TestDownloaderConstructor, Normal) {
  // setup
  // パラメータの設定
  std::vector<rclcpp::Parameter> params = {
    rclcpp::Parameter("station_id", 0),
    rclcpp::Parameter("ip_addr", std::string("127.0.0.1")),
    rclcpp::Parameter("port", 9001),
    rclcpp::Parameter("dm_user", std::string("dm2sampleuser")),
    rclcpp::Parameter("pass", std::string("dm2samplepassword")),
    rclcpp::Parameter("query", std::string(
        "master freespace_info_0_8_1 select * from freespace_info_0_8_1 [range 300 msec]")),
    rclcpp::Parameter("topic_name", std::string("test_freespace_info"))};
  rclcpp::NodeOptions options;
  options.parameter_overrides(params);

  // ros初期化、node,executorの設定
  rclcpp::init(0, 0);
  auto test_uploader = std::make_shared<TestUploader>();
  auto downloader_node = std::make_shared<Downloader>(options);
  auto sub_node = std::make_shared<test_util::TestSubscriber<FreespaceInfoArray>>(
      "test_freespace_info");

  rclcpp::executors::MultiThreadedExecutor exec;
  exec.add_node(test_uploader);
  exec.add_node(downloader_node);
  exec.add_node(sub_node);

  // excercise
  bool is_pub_received = false;
  const rclcpp::Time start_time = rclcpp::Clock(RCL_SYSTEM_TIME).now();
  rclcpp::Time end_time;
  const double subscribe_wait_time = 5.0;  // topic受信待ち時間 [sec]

  while (rclcpp::ok()) {
    exec.spin_once(std::chrono::seconds(1));

    // データが受信された場合、ループを中止
    is_pub_received = sub_node->ReceiveStatus();
    if (is_pub_received) break;

    // 設定された時間を超えた場合、ループを中止
    end_time = rclcpp::Clock(RCL_SYSTEM_TIME).now();
    if ((end_time - start_time).seconds() > subscribe_wait_time) break;
  }

  // verify
  EXPECT_TRUE(is_pub_received);
  EXPECT_NO_THROW(auto test_downloader_node = std::make_shared<Downloader>(options));

  // teardown
  rclcpp::shutdown();
}

/**
 * @brief queryパラメータでmaster句とtable句が異なるとき、
 * Downloaderクラスの生成が失敗することを検証する
 * - 入力値:
 *  - queryのmaster nameとtable nameが違うタイプに設定
 *  - 他のは正常系と同じ
 * 
 * - 期待結果:
 *  - runtime error
 */
TEST(TestDownloaderConstructor, TableMasterNameDiff) {
  // setup
  // パラメータの設定
  std::vector<rclcpp::Parameter> params = {
      rclcpp::Parameter("station_id", 0),
      rclcpp::Parameter("ip_addr", std::string("127.0.0.1")),
      rclcpp::Parameter("port", 9001),
      rclcpp::Parameter("dm_user", std::string("dm2sampleuser")),
      rclcpp::Parameter("pass", std::string("dm2samplepassword")),
      rclcpp::Parameter("query", std::string(
         "master freespace_info select * from signal_info [range 300 msec]"))};
  rclcpp::NodeOptions options;
  options.parameter_overrides(params);

  // ros初期化
  rclcpp::init(0, 0);

  // excercise
  // verify
  EXPECT_THROW(auto downloader_node = std::make_shared<Downloader>(options),
               std::runtime_error);

  // shutdown
  rclcpp::shutdown();
}

/**
 * @brief queryパラメータのtable句がサポートしないタイプのとき、
 * Downloaderクラスの生成が失敗することを検証する
 * - 入力値:
 *  - サポートされていないtable句に設定
 *  - 他のは正常系と同じ
 * 
 * - 期待結果:
 * - runtime error
 */
TEST(TestDownloader, UnsupportedType) {
  // setup
  // パラメータの設定
  std::vector<rclcpp::Parameter> params = {
      rclcpp::Parameter("station_id", 0),
      rclcpp::Parameter("ip_addr", std::string("127.0.0.1")),
      rclcpp::Parameter("port", 9001),
      rclcpp::Parameter("dm_user", std::string("dm2sampleuser")),
      rclcpp::Parameter("pass", std::string("dm2samplepassword")),
      rclcpp::Parameter("query", std::string(
          "master new_type select * from mew_type [range 300 msec]"))};
  rclcpp::NodeOptions options;
  options.parameter_overrides(params);

  // ros初期化
  rclcpp::init(0, 0);

  // excercise
  // verify
  EXPECT_THROW(auto downloader_node = std::make_shared<Downloader>(options),
               std::runtime_error);

  // shutdown
  rclcpp::shutdown();
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

}   // namespace test
}   // namespace ros2_dmi
