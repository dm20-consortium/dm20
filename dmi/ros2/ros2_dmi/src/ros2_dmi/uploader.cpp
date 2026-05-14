#include "ros2_dmi/uploader.hpp"

#include <is/DmManager.h>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp_components/register_node_macro.hpp>
#include <rcl_interfaces/msg/parameter_descriptor.hpp>

namespace ros2_dmi {

Uploader::Uploader(const rclcpp::NodeOptions& options)
    : Node("uploader", options), log_count_(0) {
  rcl_interfaces::msg::ParameterDescriptor param_desc;

  this->declare_parameter("station_id", 1, param_desc);
  station_id_ = this->get_parameter("station_id").as_int();
  RCLCPP_INFO(this->get_logger(), "station_id: %d", station_id_);

  this->declare_parameter("ip_addr", std::string("127.0.0.1"), param_desc);
  const std::string ip_addr = this->get_parameter("ip_addr").as_string();
  RCLCPP_INFO(this->get_logger(), "ipaddr: %s", ip_addr.c_str());

  this->declare_parameter("port", 9002);
  const int port = this->get_parameter("port").as_int();
  RCLCPP_INFO(this->get_logger(), "port: %d", port);

  this->declare_parameter("dm_user", std::string("dm2sampleuser"), param_desc);
  const std::string dm_user = this->get_parameter("dm_user").as_string();
  RCLCPP_INFO(this->get_logger(), "dm_user: %s", dm_user.c_str());

  this->declare_parameter("pass", std::string("dm2samplepassword"), param_desc);
  const std::string pass = this->get_parameter("pass").as_string();
  RCLCPP_INFO(this->get_logger(), "pass: %s", pass.c_str());

  try {
    socket_ = DmManager::getDatagramSocket(ip_addr, port, dm_user, pass, false);
  } catch (const ConnectionFailedException& e) {
    throw std::runtime_error(e.getMessage());
  }

  this->declare_parameter("target_table", std::string(""), param_desc);
  target_table_ = this->get_parameter("target_table").as_string();
  RCLCPP_INFO(this->get_logger(), "target_table: %s", target_table_.c_str());

  this->declare_parameter("topic_name", std::string(""), param_desc);
  const std::string topic_name = this->get_parameter("topic_name").as_string();
  RCLCPP_INFO(this->get_logger(), "topic_name: %s", topic_name.c_str());

  this->declare_parameter("topic_type", std::string(""), param_desc);
  const std::string topic_type = this->get_parameter("topic_type").as_string();
  RCLCPP_INFO(this->get_logger(), "topic_type: %s", topic_type.c_str());

  sub_ = SubscriberFactory::Create(topic_name, topic_type, this);
  if (sub_ == nullptr) {
    throw std::runtime_error("Failed to create subscriber");
  }
  sub_->RegisterUserCallback(std::bind(&ros2_dmi::Uploader::Callback, this, std::placeholders::_1));
  RCLCPP_INFO(this->get_logger(), "Uploader has initialized");
}

Uploader::~Uploader() {
  // nothing to do
}

void Uploader::Callback(const std::vector<IS::Tuple>& tuples) {
  // TODO dashingではTHROTTLEなマクロが使えない．それまでは，カウンタ方式で対応
  // 10Hz周期が基本なので，1分周期で正常稼働報告するために，600データ毎にログ出力しておく
  if (++log_count_ >= 600) {
    RCLCPP_INFO(this->get_logger(), "working...");
    log_count_ = 0;
  }
  socket_->sendStreamData(target_table_, tuples);
}

}
RCLCPP_COMPONENTS_REGISTER_NODE(ros2_dmi::Uploader)
