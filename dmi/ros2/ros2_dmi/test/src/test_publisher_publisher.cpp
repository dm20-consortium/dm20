#include <gtest/gtest.h>

#include <memory>
#include <chrono>
#include <thread>

#include <rclcpp/rclcpp.hpp>

#include "ros2_dmi/publisher.hpp"
#include "resultset_freespace_info.hpp"


namespace ros2_dmi {
namespace test {

/**
 * @brief Publisher初期化用クラス
 * 
 */
class TestPublisherWrapper : public rclcpp::Node {
 public:
 /**
  * @brief Publisherを初期化し、インスタンスを生成する
  * 
  */
  TestPublisherWrapper() : Node("test_publisher_wapper") {
    pub_.reset(new Publisher<FreespaceInfoArray>("tested_pub", this));
  }

 private:
 /**
  * @brief Publisherインスタンスポインター
  * 
  */
  IPublisher::SharedPtr pub_;
};

/**
 * @brief Publisherのコンストラクタを検証する
 * 
 * - テンプレートデータはFreespaceInfoArray型を利用する
 */
TEST(TestPublisherPublisher, Normal) {
  // setup
  rclcpp::init(0, 0);

  // exercise
  auto tested_pub = std::make_shared<TestPublisherWrapper>();

  // verify
  const auto pub_info = tested_pub->get_publishers_info_by_topic("tested_pub");
  std::this_thread::sleep_for(std::chrono::seconds(3));

  EXPECT_EQ(pub_info.size(), 1UL);
  EXPECT_EQ(pub_info.at(0).topic_type(), "dm_freespace_info_msgs/msg/FreespaceInfoArray");

  // teardown
  rclcpp::shutdown();
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

}  // namespace test
}  // namespace ros2_dmi
