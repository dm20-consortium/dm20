#ifndef ROS2_DMI_UPLOADER_HPP_
#define ROS2_DMI_UPLOADER_HPP_
#include <string>
#include <vector>

#include <is/DatagramSocket.h>
#include <rclcpp/rclcpp.hpp>

#include "ros2_dmi/visibility.h"
#include "ros2_dmi/subscriber.hpp"


namespace ros2_dmi {

/**
 * @brief A node that subscribe ROS message then upload converted object to DM by using isLib
 */
class Uploader : public rclcpp::Node {
 public:
  /**
   * @brief Initialize a socket to upload and subscriber
   *
   * @param options Node options
   *
   * @return none
   */
  ROS2_DMI_PUBLIC
  explicit Uploader(const rclcpp::NodeOptions& options=rclcpp::NodeOptions());

  /**
   * @brief Destructor(nothing to do)
   */
  virtual ~Uploader();

 private:
  /**
   * @brief Send tuple object to DM by using socket
   *
   * @param tuple DM data object
   */
  void Callback(const std::vector<IS::Tuple>& tuple);

  /**
   * @brief This station ID
   */
  int station_id_;

  /**
   * @brief A name of DM table that for sending DM data object
   */
  std::string target_table_;

  /**
   * @brief A socket object to send DM object data
   */
  DatagramSocket* socket_;

  /**
   * @brief A subscriber that handle ROS message then convert it as DM data object
   *
   * The updation will be notified as calling back function
   */
  SubscriberBase::SharedPtr sub_;

  /**
   * @brief 受信イベントが正常に稼働していることを標準出力で出す際に，絞るためのカウンタ
   */
  uint32_t log_count_;
};

}
#endif
