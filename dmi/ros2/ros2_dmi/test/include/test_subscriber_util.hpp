#ifndef TEST_INCLUDE_SUBSCRIBER_UTIL_HPP_
#define TEST_INCLUDE_SUBSCRIBER_UTIL_HPP_

#include <rclcpp/rclcpp.hpp>

namespace ros2_dmi {
namespace test_util {
/**
 * @brief テスト用のSubscriberのテンプレート
 * - 指定したデータタイプのSubscriberを生成する
 * 
 * @tparam T 受信するデータのタイプ
 */
template <class T>
class TestSubscriber : public rclcpp::Node {
 public:
  /**
   * @brief Subscriberのインスタンスを生成する
   * 
   * @param topic_name 生成したtopicの名前
   */
  TestSubscriber(const std::string& topic_name) : Node("test_subscriber") {
    received_ = false;
    sub_ = this->create_subscription<T>(
      topic_name, rclcpp::QoS(10),
      std::bind(&TestSubscriber::SubCallBack, this, std::placeholders::_1));
  }

  /**
   * @brief 受信状態を返却する
   * 
   * @return true 受信済み
   * @return false 未受信
   */
  bool ReceiveStatus() {
    return received_;
  }

 private:
  /**
   * @brief Subscriberが登録したtopicを受信した時のコールバック関数
   * - コールバック内で受信状態をtrueに変更する
   * 
   * @param msg　受信したデータ 
   */
  void SubCallBack(const typename T::SharedPtr msg) {
    if (msg != nullptr) received_ = true;
  }

  /**
   * @brief 受信状態フラグ
   * 
   */
  bool received_;

  /**
   * @brief Subscriberインスタンスポインター
   * 
   */
  typename rclcpp::Subscription<T>::SharedPtr sub_;
};


}  // namespace test_util
}  // namespace ros2_dmi

#endif  // TEST_INCLUDE_SUBSCRIBER_UTIL_HPP_
