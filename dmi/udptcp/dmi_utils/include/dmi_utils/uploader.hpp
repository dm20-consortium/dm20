#ifndef DMI_UTILS_UPLOADER_HPP_
#define DMI_UTILS_UPLOADER_HPP_
/**
 * @file uploader.hpp
 */
#include <string>
#include <vector>
#include <memory>

#include <is/DatagramSocket.h>
#include <is/Tuple.h>
#include <is/DmManager.h>
#include <is/DmUtil.h>

#include "dmi_utils/dm_bridge.hpp"
#include <cool4_api_dataclass/dataclasses_with_header.hpp>


namespace dmi_utils {

/**
 * @brief 
 *
 * @tparam ElementType データとして保持しているときのクラス
 * @tparam template<class> class DataPackage ElementTypeを保持するときに使うクラス
 *         メーカ個別の仕様によっては，共通部と検知ターゲットからElementTypeのリストを作ることがあり，このようなメッセージ１に対して出力複数であるようなケースは，テンプレート引数一つでは対応できないため，拡張した
 */
template<class ElementType, template<class> class DataPackage>
class Uploader {
 public:
  struct Config {
    Config(
      const std::string& in_ipaddr,
      const int in_port,
      const std::string& in_user,
      const std::string& in_pass,
      const std::string& in_stream_name,
      const uint32_t in_connection_try_count)
      : ipaddr(in_ipaddr),
        port(in_port),
        user(in_user),
        pass(in_pass),
        stream_name(in_stream_name),
        connection_try_count(in_connection_try_count) {}

    std::string ipaddr;
    int port;
    std::string user;
    std::string pass;
    std::string stream_name;
    uint32_t connection_try_count;
  };

  using SharedPtr = std::shared_ptr<Uploader>;

  explicit Uploader(const Config& conf)
      : stream_name_(conf.stream_name) {
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

  ~Uploader() = default;

  void Upload(std::vector<IS::Tuple>& tuples) {
    const auto send_sec = std::chrono::system_clock::now().time_since_epoch().count();
    VLOG(2) << "send time: " << send_sec;
    socket_->sendStreamData(stream_name_, tuples);
  }

 private:
  DatagramSocket* socket_;
  const std::string stream_name_;
};

}

#endif
