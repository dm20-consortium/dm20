#include "protobuf_dmi/uploader.hpp"

#include <is/DmManager.h>
#include <is/Tuple.h>
#include <glog/logging.h>

#include "protobuf_dmi/dm_bridge.hpp"


namespace protobuf_dmi
{

Uploader::Uploader(const Config& conf)
{
  // 何度か接続を試みる
  // このクラスの初期化はDMが起動していることが前提だが，
  // systemd等で自動起動する場合，依存を記述していたとしてもDMより先にこのクラスが実体化することがある
  // そのため，1秒周期で何度か接続トライをすることで確実に起動するようにした
  uint32_t get_db_con_count = 0;
  while (get_db_con_count < conf.connection_try_count) {
    try {
      socket_ = DmManager::getDatagramSocket(
          conf.ipaddr, conf.port, conf.user, conf.pass, false);
    } catch (const ConnectionFailedException& e) {
      get_db_con_count += 1;
      std::this_thread::sleep_for(std::chrono::seconds(1));
      continue;
    }
    break;
  }
  if (get_db_con_count == conf.connection_try_count) {
    LOG(ERROR) << "failed to connect to DM (" << conf.ipaddr << ":" << conf.port << ")";
    throw std::runtime_error("Failed to connect DM");
  }
  LOG(INFO) << "connected to DM (" << conf.ipaddr << ":" << conf.port << ")";
}

void Uploader::Upload(const std::vector<IS::Tuple>& tuples)
{
  std::vector<IS::Tuple> mutable_tuples = tuples;

  std::map<std::string, std::vector<IS::Tuple>> tuples_by_stream_name;
  for (auto tuple : mutable_tuples)
  {
    tuples_by_stream_name[tuple.getTableName()].push_back(
        tuple);
  }

  const auto send_sec = std::chrono::system_clock::now().time_since_epoch().count();
  VLOG(1) << "send time: " << send_sec;

  for (auto key_and_value : tuples_by_stream_name)
  {
    socket_->sendStreamData(
        key_and_value.first,
        key_and_value.second);
  }
}

}  // protobuf_dmi
