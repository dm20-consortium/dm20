#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include <is/DmManager.h>
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
    DebugInfoWithObjectIdArray test_debug_infos;
    const long now = DmUtil::getTimeMillisec();
    tuples_ = Converter<FreespaceInfoArray, DebugInfoWithObjectIdArray>::RosToTuple(
        test_in, now, test_debug_infos);

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
  std::vector<Tuple> tuples_;
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
};

/**
 * @brief スマートポインターのDeleter関数
 * デストラクタを1回のみ呼び出すようにするため、スマートポインターのデフォルトDeleterを入れ替える
 * 
 * @param downloader_ptr ポインター
 */
void Deleter(Downloader* downloader_ptr) {
  if (downloader_ptr != nullptr) downloader_ptr = nullptr;
}

/**
 * @brief Downloaderクラスのデストラクタを実行後に、
 *  DownloaderとDMの接続が切れているかどうかを検証する
 * 
 * - GMockはpublicかつvirtualの関数しか模擬できないので、
 *   DownloaderとDMの接続を切れているかどうかのを判断するために、
 *   DownloaderノードのPublish状態によって判断する、
 * 
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
 *  - デストラクタが実行する前に、Downloaderからデータを受信できる
 *  - デストラクタが実行した後に、Downloaderからデータを受信できない
 */
TEST(TestDownloaderDestructor, Normal) {
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
  std::shared_ptr<Downloader> downloader_node(new Downloader(options), Deleter);
  auto sub_node_before = std::make_shared<test_util::TestSubscriber<FreespaceInfoArray>>(
      "test_freespace_info");
  auto sub_node_after = std::make_shared<test_util::TestSubscriber<FreespaceInfoArray>>(
      "test_freespace_info");

  rclcpp::executors::MultiThreadedExecutor exec;
  exec.add_node(test_uploader);
  exec.add_node(downloader_node);
  exec.add_node(sub_node_before);

  bool is_pub_received_before = false;
  bool is_pub_received_after = false;

  // excercise
  rclcpp::Time start_time = rclcpp::Clock(RCL_SYSTEM_TIME).now();
  rclcpp::Time end_time;
  const double subscribe_wait_time = 10.0;  // topic受信待ち時間 [sec]
  
  // デストラクタ実行前にDownloaderの送信状態を取得
  while (rclcpp::ok()) {
    exec.spin_once(std::chrono::seconds(1));
    is_pub_received_before = sub_node_before->ReceiveStatus();
    if (is_pub_received_before) break;

    end_time = rclcpp::Clock(RCL_SYSTEM_TIME).now();
    if ((end_time - start_time).seconds() > subscribe_wait_time) break;
  }

  downloader_node->~Downloader();
  exec.remove_node(sub_node_before);
  exec.add_node(sub_node_after);
  // デストラクタ実行後にDownloaderの送信状態を取得
  start_time = rclcpp::Clock(RCL_SYSTEM_TIME).now();
  while (rclcpp::ok()) {
    exec.spin_once(std::chrono::seconds(1));
    is_pub_received_after = sub_node_after->ReceiveStatus();
    if (is_pub_received_after) break;

    end_time = rclcpp::Clock(RCL_SYSTEM_TIME).now();
    if ((end_time - start_time).seconds() > subscribe_wait_time) break;
  }

  // verify
  EXPECT_TRUE(is_pub_received_before);
  EXPECT_FALSE(is_pub_received_after);

  // teardown
  rclcpp::shutdown();
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

}   // namespace test
}   // namespace ros2_dmi
