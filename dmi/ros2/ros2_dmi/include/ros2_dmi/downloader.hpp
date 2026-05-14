#ifndef ROS2_DMI_DOWNLOADER_HPP_
#define ROS2_DMI_DOWNLOADER_HPP_
#include <rclcpp/rclcpp.hpp>

#include <is/DmManager.h>

#include "ros2_dmi/visibility.h"
#include "ros2_dmi/publisher.hpp"

namespace ros2_dmi {

/**
 * @brief A node that handle DM updation event then publish converted message to the others
 */
class Downloader : public rclcpp::Node {
 public:
  /**
   * @brief Initialize DB event handler and publisher
   *
   * @param options ROS node option
   */
  ROS2_DMI_PUBLIC
  explicit Downloader(const rclcpp::NodeOptions& options=rclcpp::NodeOptions());

  /**
   * @brief Disconnect from DB
   */
  virtual ~Downloader();

 private:
  /**
   * @brief Publish DM object as ROS message
   *
   * @param rs
   */
  void DmEventCallback(ResultSet rs);

  /**
   * @brief station id
   */
  int station_id_;

  /**
   * @brief Kept continuous query ID to cancel when this node shut down.
   */
  int mng_id_;

  /**
   * @brief Connection object
   */
  Connection* con_;

  /**
   * @brief Publisher object that convert DM object as ROS message then publish it
   */
  IPublisher::SharedPtr pub_;

  /**
   * @brief Supported topic type by DM table name
   */
  const std::map<std::string, std::string> topic_type_by_table_name_ = {
      {"object_info_0_8_1", "ObjectInfoArray"},
      {"freespace_info_0_8_1", "FreespaceInfoArray"},
      {"signal_info", "SignalInfoArray"},
      {"sensor_info_0_8_1", "SensorInfoArray"}
    };
};

}

#endif
