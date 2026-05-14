#include <gtest/gtest.h>

#include <memory>

#include <rclcpp/rclcpp.hpp>

#include "ros2_dmi/subscriber.hpp"
#include "resultset_freespace_info.hpp"

namespace ros2_dmi {
namespace test {

/**
 * @brief Subscriberr初期化用クラス
 * 
 */
class TestSubscriberWrapper : public rclcpp::Node {
 public:
  /**
  * @brief Subscriberrを初期化し、インスタンスを生成する
  * 
  */
  TestSubscriberWrapper() : Node("test_subscriber_warpper") {
    sub_.reset(new Subscriber<FreespaceInfoArray>("tested_sub", this));
  }
 private:
 /**
  * @brief Subscriberインスタンスポインター
  * 
  */
  SubscriberBase::SharedPtr sub_;
};

/**
 * @brief Subscriberのコンストラクタを検証する
 * 
 * - テンプレートデータはFreespaceInfo型を利用する
 * - 他の型のデータも同じ結果になるため、FreespaceInfoを利用する
 */
TEST(TestSubscriberSubscriber, Normal) {
  // setup
  rclcpp::init(0, 0);

  // exercise
  auto tested_sub = std::make_shared<TestSubscriberWrapper>();

  // verify
  const auto sub_info = tested_sub->get_subscriptions_info_by_topic("tested_sub");

  EXPECT_EQ(sub_info.size(), 1UL);
  EXPECT_EQ(sub_info.at(0).topic_type(), "dm_freespace_info_msgs/msg/FreespaceInfoArray");

  // teardown
  rclcpp::shutdown();
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

}  // namespace test
}  // namespace ros2_dmi
