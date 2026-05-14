#ifndef ROS2_DMI_SUBSCRIBER_HPP_
#define ROS2_DMI_SUBSCRIBER_HPP_
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <rclcpp/rclcpp.hpp>

#include "ros2_dmi/sensor_info_converter.hpp"
#include "ros2_dmi/signal_info_converter.hpp"
#include "ros2_dmi/object_info_converter.hpp"
#include "ros2_dmi/freespace_info_converter.hpp"

namespace ros2_dmi {

/**
 * @brief A subscription base class
 */
class SubscriberBase {
 public:
  /**
   * @brief Alias of shared pointer
   */
  using SharedPtr = std::shared_ptr<SubscriberBase>;

  /**
   * @brief Alias of user function type
   */
  using CallbackType = std::function<void (const std::vector<IS::Tuple>&)>;

  /**
   * @brief Default constructor
   */
  SubscriberBase() = default;

  /**
   * @brief Default destructor
   */
  ~SubscriberBase() = default;

  /**
   * @brief Register user function that called when ROS message event was occured and converted
   *
   * @param callback user function
   */
  void RegisterUserCallback(const CallbackType& callback) {
    user_func_ = callback;
  };

 protected:
  /**
   * @brief User function object
   */
  CallbackType user_func_;
};

/**
 * @brief A ROS subscriber that propagate event with converted data
 *
 * @tparam T ROS message type
 */
template<typename T>
class Subscriber : public SubscriberBase {
 public:
  /**
   * @brief Initialize subscriber
   *
   * @param topic_name topic name to subscribe
   * @param node Node object ptr
   */
  Subscriber(
      const std::string& topic_name,
      rclcpp::Node* node)
      : SubscriberBase() {
    auto qos = rclcpp::QoS(rclcpp::KeepLast(10)).reliability(RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT);
    sub_ = node->create_subscription<T>(
        topic_name, qos,
        std::bind(&Subscriber<T>::Update, this, std::placeholders::_1));
  }

 private:
  /**
   * @brief ROS message event.
   * It convert ROS message data to DM tuple vector, then call user function
   *
   * @param msg
   */
  void Update(const typename T::SharedPtr msg) {
    const long now_msec = static_cast<long>(DmUtil::getTimeMillisec());
    const std::vector<IS::Tuple> tuples = Converter<T>::RosToTuple(*msg, now_msec);

    if (tuples.empty()) return;

    user_func_(tuples);
  }

  /**
   * @brief ROS subscripter object
   */
  typename rclcpp::Subscription<T>::SharedPtr sub_;
};


/**
 * @brief Factory method to create variant type subscriber
 */
class SubscriberFactory {
 public:
   /**
    * @brief Create specified type subscriber
    *
    * @param topic_name A topic name to be subscribed
    * @param topic_type A topic type to be subscribed
    * @param node Node object ptr
    *
    * @return SubscriberBase class
    */
  static SubscriberBase::SharedPtr Create(
      const std::string& topic_name,
      const std::string& topic_type,
      rclcpp::Node* node) {
    SubscriberBase::SharedPtr sub;
    if (topic_type == "ObjectInfoArray") {
      sub.reset(new Subscriber<ObjectInfoArray>(
            topic_name, node));
    } else if (topic_type == "FreespaceInfoArray") {
      sub.reset(new Subscriber<FreespaceInfoArray>(
            topic_name, node));
    } else if (topic_type == "SignalInfoArray") {
      sub.reset(new Subscriber<SignalInfoArray>(
            topic_name, node));
    } else if (topic_type == "SensorInfoArray") {
      sub.reset(new Subscriber<SensorInfoArray>(
            topic_name, node));
    } else {
      RCLCPP_ERROR(node->get_logger(), "%s is not supported", topic_type.c_str());
    }

    return sub;
  }
};

}

#endif
