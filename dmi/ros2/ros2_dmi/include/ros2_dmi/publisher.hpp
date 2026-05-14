#ifndef ROS2_DMI_PUBLISHER_HPP_
#define ROS2_DMI_PUBLISHER_HPP_
#include <chrono>
#include <memory>

#include <boost/optional.hpp>
#include <is/Tuple.h>
#include <is/TupleSet.h>

#include <rclcpp/rclcpp.hpp>

#include "ros2_dmi/sensor_info_converter.hpp"
#include "ros2_dmi/signal_info_converter.hpp"
#include "ros2_dmi/object_info_converter.hpp"
#include "ros2_dmi/freespace_info_converter.hpp"

namespace ros2_dmi {

/**
 * @brief ROS publisher interface class
 */
class IPublisher {
 public:
  /**
   * @brief Shared pointer alias
   */
  using SharedPtr = std::shared_ptr<IPublisher>;

  /**
   * @brief Destructor
   */
  virtual ~IPublisher() = default;

  /**
   * @brief Publish ResultSet info as ROS message
   *
   * @param rs Result set created by DM
   */
  virtual void Publish(ResultSet& rs) = 0;

  /**
   * @brief Publish tuples info as ROS message
   *
   * @param tpl tuples created by DM
   */
  virtual void Publish(IS::TupleSet& ts) = 0;

};

/**
 * @brief A class that:
 *  - extract message info from Resultset then
 *  - publish as ROS message
 *
 * @tparam T Message type
 */
template<typename T>
class Publisher : public IPublisher {
 public:
  /**
   * @brief Create a publisher onto node
   *
   * @param topic_name Topic name
   * @param node Node object
   */
  Publisher(const std::string& topic_name, rclcpp::Node* node) {
    pub_ = node->create_publisher<T>(topic_name, 1);
  }

  /**
   * @brief Publish ResultSet info as ROS message
   *
   * @param rs Result set created by DM
   */
  void Publish(ResultSet& rs) override {
    boost::optional<T> msg = Converter<T>::ResultSetToRos(rs);
    if (msg == boost::none) return;

    pub_->publish(msg.get());
  }

  /**
   * @brief Publish tuples info as ROS message
   *
   * @param tpl Tuples created by DM
   */
  void Publish(IS::TupleSet& ts) override {
    boost::optional<T> msg = Converter<T>::TupleSetToRos(ts);
    if (msg == boost::none) return;

    pub_->publish(msg.get());
  }


 private:
  /**
   * @brief ROS publisher
   */
  typename rclcpp::Publisher<T>::SharedPtr pub_;
};

/**
 * @brief A factory class that create publisher.
 *
 * Several ROS message types will be supported.
 */
class PublisherFactory {
 public:
   /**
    * @brief Create specified typed publisher
    *
    * @param topic_name Topic name for publish
    * @param topic_type Topic type of ROS message
    * @param node Node object ptr
    *
    * @return Base class of Publisher
    */
  static IPublisher::SharedPtr Create(
      const std::string& topic_name,
      const std::string& topic_type,
      rclcpp::Node* node) {
    IPublisher::SharedPtr pub;
    if (topic_type == "ObjectInfoArray") {
      pub.reset(new Publisher<ObjectInfoArray>(
            topic_name, node));
    } else if (topic_type == "FreespaceInfoArray") {
      pub.reset(new Publisher<FreespaceInfoArray>(
            topic_name, node));
    } else if (topic_type == "SignalInfoArray") {
      pub.reset(new Publisher<SignalInfoArray>(
            topic_name, node));
    } else if (topic_type == "SensorInfoArray") {
      pub.reset(new Publisher<SensorInfoArray>(
            topic_name, node));
    } else {
      RCLCPP_ERROR(node->get_logger(), "%s is not supported", topic_type.c_str());
    }
    return pub;
  }
};

}

#endif
