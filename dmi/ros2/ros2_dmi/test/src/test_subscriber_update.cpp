#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <is/Tuple.h>

#include "ros2_dmi/subscriber.hpp"
#include "resultset_freespace_info.hpp"
#include "rosmsg_freespace_info.hpp"
#include "test_subscriber_util.hpp"

namespace ros2_dmi {
namespace test {

/**
 * @brief テスト用FreespaceInfo型データを送信するPublisherクラス
 */
class TestPublisher : public rclcpp::Node {
 public:
  /**
   * @brief Publisherコンストラクタ
   * タイマーを用いて、定期的にFreespaceInfoデータを送信するPublisherのインスタンスを生成する
   * 
   */
  TestPublisher() : Node("test_publisher") {
    data_ = test_util::CreateFreespaceInfoArray();
    pub_ = this->create_publisher<FreespaceInfoArray>("topicname", 10);
    timer_ = this->create_wall_timer(
      500ms, std::bind(&ros2_dmi::test::TestPublisher::timer_callback, this));
  }

  /**
   * @brief 異常データを作る
   * - データのid.valueを0に設定
   * - 他の異常も同じ結果になる
   */
  void SetFailureData() {
    data_.array[0].id.value = 0;
  }

 private:
 /**
  * @brief タイマーのコールバック関数でデータを送信する
  * 
  */
  void timer_callback() {
    pub_->publish(data_);
  }

  /**
   * @brief publisherインスタンスポインター
   * 
   */
  rclcpp::Publisher<FreespaceInfoArray>::SharedPtr pub_;

  /**
   * @brief タイマーインスタンスポインター
   * 
   */
  rclcpp::TimerBase::SharedPtr timer_;

  /**
   * @brief テスト用FreespaceInfoデータ
   * 
   */
  FreespaceInfoArray data_;
};

/**
 * @brief subscriberインスタンス生成用クラス
 * 
 */
class TestSubscriberWrapper : public rclcpp::Node {
 public:
  /**
   * @brief subscriberコンストラクタを利用して、subscriberノードを生成することを検証する
   * - ノード名：test_subscriber_warpper
   * - topic名：topicname
   */
  TestSubscriberWrapper() : Node("test_subscriber_warpper") {
    received_ = false;
    sub_.reset(new Subscriber<FreespaceInfoArray>("topicname", this));
    sub_->RegisterUserCallback(std::bind(&TestSubscriberWrapper::UserCallback, this,
                                         std::placeholders::_1));
  }

  /**
   * @brief FreespaceInfoArrayメッセージ受信状況取得
   */
  bool ReceiveStatus() {
    return received_;
  }

 private:
  /**
   * @brief ユーザーコールバック関数：FreespaceInfoArrayメッセージ受信状況を更新する
   * 
   * @param tuples 受信したメッセージ
   *  tuplesが空の場合、Updateが未実行、あるいはConverterが失敗になる
   */
  void UserCallback(const std::vector<IS::Tuple>& tuples) {
    if (tuples.empty() == false) received_ = true;
  }

  /**
   * @brief subscriberポインター
   * 
   */
  SubscriberBase::SharedPtr sub_;

  /**
   * @brief FreespaceInfoメッセージ受信状態フラグ
   * - true: 受信した
   * - false: 受信していない
   */
  bool received_;
};

/**
 * @brief ROSメッセージを受信した時に、Updaterはコールバック関数を呼び出し、
 * DBGメッセージを送信することを検証する
 * 
 */
TEST(TestSubscriberUpdate, Normal) {
  // setup
  rclcpp::init(0, 0);
  auto test_pub = std::make_shared<TestPublisher>();
  auto tested_sub = std::make_shared<TestSubscriberWrapper>();

  rclcpp::executors::MultiThreadedExecutor exec;
  exec.add_node(test_pub);
  exec.add_node(tested_sub);

  bool is_updated = false;

  const rclcpp::Time start_time = rclcpp::Clock(RCL_SYSTEM_TIME).now();
  rclcpp::Time end_time;
  const double subscribe_wait_time = 5.0;  // topic受信待ち時間 [sec]

  // excercise
  while (rclcpp::ok()) {
    exec.spin_once(std::chrono::seconds(1));
    is_updated = tested_sub->ReceiveStatus();

    if (is_updated) break;

    end_time = rclcpp::Clock(RCL_SYSTEM_TIME).now();
    if ((end_time - start_time).seconds()> subscribe_wait_time) break;
  }

  // verify
  EXPECT_TRUE(is_updated);

  // teardown
  rclcpp::shutdown();
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

}  // namespace test
}  // namespace ros2_dmi
