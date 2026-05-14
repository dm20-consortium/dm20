#include <gtest/gtest.h>

#include <memory>

#include <rclcpp/rclcpp.hpp>

#include "ros2_dmi/publisher.hpp"
#include "resultset_freespace_info.hpp"
#include "test_subscriber_util.hpp"


namespace ros2_dmi {
namespace test {

/**
 * @brief FreespaceInfo用のPublisherクラス
 * 
 */
class TestPublisherWrapper : public rclcpp::Node {
 public:
  /**
   * @brief TestPublisherWrapperコンストラクタ
   * - PublisherFactory()で、FreespaceInfoのPublisherのインスタンスを生成する
   */
  TestPublisherWrapper() : Node("test_publisher_wapper") {
    tested_pub_ = PublisherFactory::Create("topicname", "FreespaceInfoArray", this);
    timer_ = this->create_wall_timer(
        50ms, std::bind(&TestPublisherWrapper::timer_callback, this));
  }

  /**
   * @brief ResultSetを設定する
   * 
   * @param rs ResultSet
   */
  void SetResultSet(ResultSet rs) {
    rs_ = rs;
  }

 private:
  /**
   * @brief タイマーのコールバック関数
   *  - Publish()を呼び出す
   */
  void timer_callback() {
    if (tested_pub_ == nullptr) return;
    tested_pub_->Publish(rs_);
  }

  /**
   * @brief Publisherインスタンスポインター
   */
  IPublisher::SharedPtr tested_pub_;

  /**
   * @brief Publisher入力用ResultSet
   */
  ResultSet rs_;
  /**
   * @brief タイマーのポインター
   */
  rclcpp::TimerBase::SharedPtr timer_;
};

/**
 * @brief ResultSetをセットしてROSトピックをPublishすることを検証する
 * 
 * - 入力値: 正常範囲内のデータ（resultset_fのデフォルト設定）
 * 
 * - 期待する結果: FreespaceInfoをpublishする
 */
TEST(TestPublisherPublish, Normal) {
  // setup
  // rosの初期化、nodeとexecutorの設定
  rclcpp::init(0, 0);
  auto tested_pub = std::make_shared<TestPublisherWrapper>();
  auto test_sub_pub = std::make_shared<test_util::TestSubscriber<FreespaceInfoArray>>("topicname");
  rclcpp::executors::MultiThreadedExecutor exec;
  exec.add_node(tested_pub);
  exec.add_node(test_sub_pub);

  // ResultSetデータの作成
  test_util::CandidateTestCase_f data = test_util::CandidateTestCase_f();
  ResultSet test_in = test_util::CreateResultSet_f(data);
  tested_pub->SetResultSet(test_in);

  // excercise
  bool is_pub_received = false;
  const rclcpp::Time start_time = rclcpp::Clock(RCL_SYSTEM_TIME).now();
  rclcpp::Time end_time;
  const double subscribe_wait_time = 10.0;  // topic受信待ち時間 [sec]

  // 受信
  while (rclcpp::ok()) {
    exec.spin_once(std::chrono::seconds(1));
    is_pub_received = test_sub_pub->ReceiveStatus();

    if (is_pub_received) break;

    end_time = rclcpp::Clock(RCL_SYSTEM_TIME).now();
    if ((end_time - start_time).seconds()> subscribe_wait_time) break;
  }

  // verify
  EXPECT_TRUE(is_pub_received);

  // teardown
  rclcpp::shutdown();
}

/**
 * @brief 不正なResultSetをセットしたときROSトピックを発行しないことを検証する
 * 
 * - 入力値: idのデータは正常範囲外、他の値は正常範囲内
 * 
 * - 期待する結果: 変換したROSトピックを発行しない（受信できない）
 */
TEST(TestPublisherPublish, Abnormal) {
  // setup
  // ros初期化 node,executor設定
  rclcpp::init(0, 0);
  auto tested_pub = std::make_shared<TestPublisherWrapper>();
  auto test_sub_pub = std::make_shared<test_util::TestSubscriber<FreespaceInfoArray>>("topicname");

  rclcpp::executors::MultiThreadedExecutor exec;
  exec.add_node(tested_pub);
  exec.add_node(test_sub_pub);

  // 入力データの設定
  test_util::CandidateTestCase_f data = test_util::CandidateTestCase_f();
  data.Update_f("id", {"test"});
  ResultSet test_in = test_util::CreateResultSet_f(data);
  tested_pub->SetResultSet(test_in);

  // excercise
  // 受信
  bool is_pub_received = false;
  const rclcpp::Time start_time = rclcpp::Clock(RCL_SYSTEM_TIME).now();
  rclcpp::Time end_time;
  const double subscribe_wait_time = 5.0;  // topic受信待ち時間 [sec]
  while (rclcpp::ok()) {
    exec.spin_once(std::chrono::seconds(1));
    is_pub_received = test_sub_pub->ReceiveStatus();

    end_time = rclcpp::Clock(RCL_SYSTEM_TIME).now();
    if ((end_time - start_time).seconds()> subscribe_wait_time) break;
  }

  // verify
  EXPECT_FALSE(is_pub_received);

  // teardown
  rclcpp::shutdown();
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

}  // namespace test
}  // namespace ros2_dmi
