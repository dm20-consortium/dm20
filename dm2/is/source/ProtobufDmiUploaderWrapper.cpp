#include "ProtobufDmiUploaderWrapper.h"

#include <log4cxx/logger.h>
#include <yaml-cpp/yaml.h>

#include "Settings.h"
#include "QueueManager.h"

namespace yaml = YAML;

namespace
{
const char* const kLibPath = "/usr/local/lib/libprotobuf_dmi.so";

std::pair<std::string, std::string> GetIpAddrAndUserName(
    const yaml::Node& common_node)
{
  const std::string ipaddr = common_node["ipAddr"].as<std::string>();
  const std::string user = common_node["user"].as<std::string>();

  return {ipaddr, user};
}

}  // anonymous namespace

namespace IS
{

std::string ProtobufDMIUploaderWrapper::ipaddr_;
std::string ProtobufDMIUploaderWrapper::user_;

ProtobufDMIUploaderWrapper::ProtobufDMIUploaderWrapper()
  : logger_(log4cxx::Logger::getLogger(
        "ProtobufDMIUploaderWrapper")),
    read_error_(false)
{
}

void ProtobufDMIUploaderWrapper::init()
{
  // load library
  void* handle = dlopen(kLibPath, RTLD_NOW);
  if (!handle)
  {
    const char* const error_msg = dlerror();
    logger_->error("Failed to load the shared object: " + std::string(kLibPath) + " [detail: " + std::string(error_msg) + "]");
    read_error_ = true;
    return;
  }

  // set functions up
  CreateReceiverFuncPtr create_receiver = reinterpret_cast<CreateReceiverFuncPtr>(dlsym(handle, "create"));
  spin_ = reinterpret_cast<SpinFuncPtr>(dlsym(handle, "spin"));
  DestroyFuncPtr destroy = reinterpret_cast<DestroyFuncPtr>(dlsym(handle, "destroy"));
  if (!create_receiver || !spin_ || !destroy)
  {
    logger_->error("Failed to set functions up");
    read_error_ = true;
    return;
  }

  // load setting
  const std::string conf_dirpath = IS::Settings::get_instance().getConfigDirectory();
  const std::string conf_filepath = IS::Settings::get_instance().CONF_YAML_DMI;
  const std::string conf_filepath_full = conf_dirpath + conf_filepath;
  if (!IS::Settings::get_instance().IsFileExist(conf_filepath_full))
  {
    logger_->error("open failed dmi conf: " + conf_filepath_full);
    read_error_ = true;
    dlclose(handle);
    return;
  }

  yaml::Node node = YAML::LoadFile(conf_filepath_full);

  // const auto ip_and_user = GetIpAddrAndUserName(node["common"]);
  // ipaddr_ = ip_and_user.first;
  // user_ = ip_and_user.second;
  ipaddr_ = node["common"]["ipAddr"].as<std::string>();
  user_ = node["common"]["user"].as<std::string>();

  for (const auto& e : node["dmi"]["protobuf"])
  {
    if (e["type"].as<std::string>() != "receiver") continue;

    std::string message_type = e["message_type"].as<std::string>();
    int port = e["receive_port"].as<int>();
    int rsu_id = e["rsu_id"].as<int>();
    int sensor_id = e["sensor_id"].as<int>();

    instances_by_msg_[message_type].push_back((*create_receiver)(
        message_type,
        port,
        rsu_id,
        sensor_id,
        &ProtobufDMIUploaderWrapper::eventCallback));
    logger_->info(
        std::string("initialize protobuf-dmi with port:") + e["receive_port"].as<std::string>() + std::string(",rsu_id:") + e["rsu_id"].as<std::string>() + std::string(",sensor_id:") + e["sensor_id"].as<std::string>());
  }

  if (instances_by_msg_.empty())
  {
    logger_->warn("No protobuf-dmi plugins are loaded");
    read_error_ = true;
    dlclose(handle);
    return;
  }

  dlclose(handle);
}

void ProtobufDMIUploaderWrapper::start()
{
  if (read_error_) return;

  for (const auto& e : instances_by_msg_)
  {
    const std::string& msg_type = e.first;
    threads_by_msg_[msg_type].push_back(std::thread(
        &ProtobufDMIUploaderWrapper::spin, this, msg_type));
    // threads_by_msg_[msg_type].detach();
  }
  for (auto& e : threads_by_msg_)
  {
    const std::string& msg_type = e.first;
    for (auto& v : threads_by_msg_[msg_type])
    {
      v.detach();
    }
  }

}

void ProtobufDMIUploaderWrapper::stop()
{
}

void ProtobufDMIUploaderWrapper::spin(const std::string& msg_type)
{
  for (auto& e : instances_by_msg_[msg_type])
  {
    (*spin_)(e);
  }
}

void ProtobufDMIUploaderWrapper::eventCallback(
    std::vector<Tuple> &tuples)
{
  std::map<std::string, std::vector<IS::Tuple>> tuples_by_name;
  for (auto& tuple : tuples)
  {
    tuples_by_name[tuple.getTableName()].push_back(tuple);
  }
  for (auto& name_and_tuples : tuples_by_name)
  {
    IS::QueueManager::get_instance().addQueueProc(
        name_and_tuples.second, ipaddr_, user_);
  }
}

}  // namespace IS
