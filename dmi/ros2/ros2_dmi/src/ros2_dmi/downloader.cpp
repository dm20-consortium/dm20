#include <functional>
#include <string>

#include "ros2_dmi/downloader.hpp"

#include <boost/algorithm/string.hpp>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp_components/register_node_macro.hpp>
#include <rcl_interfaces/msg/parameter_descriptor.hpp>

#include <is/DmManager.h>


namespace ros2_dmi {

Downloader::Downloader(const rclcpp::NodeOptions& options)
    : Node("downloader", options) {
  rcl_interfaces::msg::ParameterDescriptor param_desc;

  this->declare_parameter("station_id", 1, param_desc);
  station_id_ = this->get_parameter("station_id").as_int();
  RCLCPP_INFO(this->get_logger(), "station_id: %d", station_id_);

  this->declare_parameter("ip_addr", std::string("127.0.0.1"), param_desc);
  const std::string ip_addr = this->get_parameter("ip_addr").as_string();
  RCLCPP_INFO(this->get_logger(), "ip_addr: %s", ip_addr.c_str());

  this->declare_parameter("port", 9001, param_desc);
  const int port = this->get_parameter("port").as_int();
  RCLCPP_INFO(this->get_logger(), "port: %d", port);

  this->declare_parameter("dm_user", std::string("dm2sampleuser"), param_desc);
  const std::string dm_user = this->get_parameter("dm_user").as_string();
  RCLCPP_INFO(this->get_logger(), "dm_user: %s", dm_user.c_str());

  this->declare_parameter("pass", std::string(""), param_desc);
  const std::string pass = this->get_parameter("pass").as_string();
  RCLCPP_INFO(this->get_logger(), "pass: %s", pass.c_str());

  con_ = DmManager::getDBConnection(ip_addr, port, dm_user, pass);

  this->declare_parameter("query", std::string(""), param_desc);
  const std::string query = this->get_parameter("query").as_string();
  RCLCPP_INFO(this->get_logger(), "query: %s", query.c_str());

  // クエリ文を解析し，対応するトピックタイプを得る
  std::vector<std::string> query_elements;
  boost::split(query_elements, query, boost::is_space());
  const std::string& master_name = query_elements[1];
  const std::string& table_name = query_elements[5];
  // master句とtable句が異なる場合は非サポートとする
  if (master_name != table_name) {
    throw std::runtime_error("Sorry, master name and table name are must be the same");
  }

  if (topic_type_by_table_name_.find(table_name) == topic_type_by_table_name_.end()) {
    std::string err = "Unsupported type: ";
    err += table_name;
    throw std::runtime_error(err.c_str());
  }
  const std::string& topic_type = topic_type_by_table_name_.at(table_name);

  mng_id_ = con_->registerQuery(
      query.c_str(),
      std::bind(&ros2_dmi::Downloader::DmEventCallback, this, std::placeholders::_1));

  this->declare_parameter("topic_name", std::string(""), param_desc);
  const std::string topic_name = this->get_parameter("topic_name").as_string();
  RCLCPP_INFO(this->get_logger(), "topic_name: %s", topic_name.c_str());

  pub_ = PublisherFactory::Create(topic_name, topic_type, this);
  if (pub_ == nullptr) {
    throw std::runtime_error("Failed to create publisher");
  }

  RCLCPP_INFO(this->get_logger(), "Downloader has initialized !");
}

Downloader::~Downloader() {
  con_->reconnect();
  con_->cancelQuery(mng_id_);
  con_->disconnect();
  delete con_;
}

void Downloader::DmEventCallback(ResultSet rs) {
  if (pub_ == nullptr) return;

  pub_->Publish(rs);
}

}
RCLCPP_COMPONENTS_REGISTER_NODE(ros2_dmi::Downloader)
