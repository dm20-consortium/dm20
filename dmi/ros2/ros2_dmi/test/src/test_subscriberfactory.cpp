#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <string>

#include <rclcpp/rclcpp.hpp>

#include "ros2_dmi/subscriber.hpp"


namespace ros2_dmi {
namespace test {

/**
 * @brief SubscriberFactoryテスト用クラス
 * - データタイプに合わせて、Subscriberを生成する
 */
class TestSubscriberWrapper : public rclcpp::Node {
 public:
  /**
   * @brief SubscriberFactoryを利用してTopicを生成する
   *  - ノード名：test_subscriber_wapper
   *  - topic名：test_sub
   * 
   * @param data_type 
   */
  explicit TestSubscriberWrapper(const std::string& data_type) : Node("test_subscriber_warpper") {
    sub_ = SubscriberFactory::Create("test_sub", data_type, this);
  }

 private:
  /**
   * @brief Subscriberのインスタンスポインター
   * 
   */
  SubscriberBase::SharedPtr sub_;
};

/**
 * @brief データタイプがObjectInfoArrayの場合、
 * SubscriberFactoryがObjectInfoArrayインスタンスを生成することを確認する
 * 
 * - 入力値: 文字列「ObjectInfoArray」
 * - 期待値: ObjectInfoArray型のSubscriberが生成されること
 */
TEST(TestSubscriberFactory, ObjectInfoArray) {
  // setup
  rclcpp::init(0, 0);

  // excercise
  auto sub = std::make_shared<TestSubscriberWrapper>("ObjectInfoArray");

  // verify
  const auto sub_info = sub->get_subscriptions_info_by_topic("test_sub");

  EXPECT_EQ(sub_info.size(), 1UL);
  EXPECT_EQ(sub_info.at(0).topic_type(), "dm_object_info_msgs/msg/ObjectInfoArray");

  // teardown
  rclcpp::shutdown();
}

/**
 * @brief データタイプがFreespaceInfoArrayの場合、
 * SubscriberFactoryがFreespaceInfoArrayインスタンスを生成することを確認する
 * 
 * - 入力値: 文字列「FreespaceInfoArray」
 * - 期待値: FreespaceInfoArray型のSubscriberが生成されること
 */
TEST(TestSubscriberFactory, FreespaceInfoArray) {
  // setup
  rclcpp::init(0, 0);

  // excercise
  auto sub = std::make_shared<TestSubscriberWrapper>("FreespaceInfoArray");

  // verify
  const auto sub_info = sub->get_subscriptions_info_by_topic("test_sub");

  EXPECT_EQ(sub_info.size(), 1UL);
  EXPECT_EQ(sub_info.at(0).topic_type(), "dm_freespace_info_msgs/msg/FreespaceInfoArray");

  // teardown
  rclcpp::shutdown();
}

/**
 * @brief データタイプがSignalInfoArrayの場合、
 * SubscriberFactoryがSignalInfoArrayインスタンスを生成することを確認する
 * 
 * - 入力値: 文字列「SignalInfoArray」
 * - 期待値: SignalInfoArray型のSubscriberが生成されること
 */
TEST(TestSubscriberFactory, SignalInfoArray) {
  // setup
  rclcpp::init(0, 0);

  // excercise
  auto sub = std::make_shared<TestSubscriberWrapper>("SignalInfoArray");

  // verify
  const auto sub_info = sub->get_subscriptions_info_by_topic("test_sub");

  EXPECT_EQ(sub_info.size(), 1UL);
  EXPECT_EQ(sub_info.at(0).topic_type(), "dm_signal_info_msgs/msg/SignalInfoArray");

  // teardown
  rclcpp::shutdown();
}


/**
 * @brief データタイプが未サポートの場合、
 * SubscriberFactoryがインスタンスを生成しないことを確認する
 * 
 * - 入力値: 文字列「TestType」
 * - 期待値: Subscriberが生成されないこと
 */
TEST(TestSubscriberFactory, Abnormal) {
  // setup
  rclcpp::init(0, 0);

  // excercise
  auto sub = std::make_shared<TestSubscriberWrapper>("TestType");

  // verify
  const auto sub_info = sub->get_subscriptions_info_by_topic("tested_pub");

  EXPECT_EQ(sub_info.size(), 0UL);

  // teardown
  rclcpp::shutdown();
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

}  // namespace test
}  // namespace ros2_dmi
