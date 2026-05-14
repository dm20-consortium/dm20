#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <string>
#include <chrono>
#include <thread>

#include <rclcpp/rclcpp.hpp>

#include "ros2_dmi/publisher.hpp"


namespace ros2_dmi {
namespace test {

/**
 * @brief PubilsherFactoryテスト用クラス
 * - データタイプに合わせて、Publisherを生成する
 */
class TestPublisherWrapper : public rclcpp::Node {
 public:
  /**
   * @brief PublisherFactoryを利用してTopicを生成する
   *  - ノード名：test_publisher_wapper
   *  -  topic名：test_pub
   * 
   * @param data_type 生成するTopicのデータタイプ
   */
  explicit TestPublisherWrapper(const std::string& data_type) : Node("test_publisher_wapper") {
    pub_ = PublisherFactory::Create("test_pub", data_type, this);
  }

  /**
   * @brief PublisherのインスタンスポインターがNullかどうかを検証する
   * 
   * @return true PublisherのインスタンスポインターがNullである
   * @return false PublisherのインスタンスポインターがNullではない
   */
  bool IsNull() {
    if (pub_ == nullptr) return true;
    return false;
  }

 private:
  /**
   * @brief Publisherのインスタンスポインター
   * 
   */
  IPublisher::SharedPtr pub_;
};

/**
 * @brief データタイプがObjectInfoArrayの場合、
 * PublisherFactoryがObjectInfoArray型のPublisherインスタンスを生成することを検証する
 * 
 * - 入力値：文字列「ObjectInfoArray」
 * 
 * - 期待値：ObjectInfoArray型のPublisherが生成されること
 */
TEST(TestPublisherFactory, ObjectInfoArray) {
  // setup
  rclcpp::init(0, 0);

  // exercise
  auto pub = std::make_shared<TestPublisherWrapper>("ObjectInfoArray");
  std::this_thread::sleep_for(std::chrono::seconds(3));

  // verify
  const auto pub_info = pub->get_publishers_info_by_topic("test_pub");


  EXPECT_EQ(pub_info.size(), 1UL);
  EXPECT_EQ(pub_info.at(0).topic_type(), "dm_object_info_msgs/msg/ObjectInfoArray");
  // teardown
  rclcpp::shutdown();
}

/**
 * @brief データタイプがFreespaceInfoArrayの場合、
 * PublisherFactoryがFreespaceInfoArray型のPublisherインスタンスを生成することを検証する
 * 
 * - 入力値：文字列「FreespaceInfoArray」
 * 
 * - 期待値：FreespaceInfoArray型のPublisherが生成されること
 */
TEST(TestPublisherFactory, FreespaceInfoArray) {
  // setup
  rclcpp::init(0, 0);

  // exercise
  auto pub = std::make_shared<TestPublisherWrapper>("FreespaceInfoArray");
  std::this_thread::sleep_for(std::chrono::seconds(3));

  // verify
  const auto pub_info = pub->get_publishers_info_by_topic("test_pub");

  EXPECT_EQ(pub_info.size(), 1UL);
  EXPECT_EQ(pub_info.at(0).topic_type(), "dm_freespace_info_msgs/msg/FreespaceInfoArray");

  // teardown
  rclcpp::shutdown();
}

/**
 * @brief データタイプがSignalInfoArrayの場合、
 * PublisherFactoryがSignalInfoArray型のPublisherインスタンスを生成することを検証する
 * 
 * - 入力値：文字列「SignalInfoArray」
 * 
 * - 期待値：SignalInfoArray型のPublisherが生成されること
 */
TEST(TestPublisherFactory, SignalInfoArray) {
  // setup
  rclcpp::init(0, 0);

  // excercise
  auto pub = std::make_shared<TestPublisherWrapper>("SignalInfoArray");
  std::this_thread::sleep_for(std::chrono::seconds(3));

  // verify
  const auto pub_info = pub->get_publishers_info_by_topic("test_pub");

  EXPECT_EQ(pub_info.size(), 1UL);
  EXPECT_EQ(pub_info.at(0).topic_type(), "dm_signal_info_msgs/msg/SignalInfoArray");

  // teardown
  rclcpp::shutdown();
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

}  // namespace test
}  // namespace ros2_dmi
