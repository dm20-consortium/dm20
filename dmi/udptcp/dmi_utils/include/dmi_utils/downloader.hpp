/**
 * @file downloader.hpp
 */
#ifndef DMI_UTILS_DOWNLOADER_HPP_
#define DMI_UTILS_DOWNLOADER_HPP_
#include <chrono>
#include <string>
#include <fstream>
#include <functional>
#include <vector>
#include <memory>
#include <thread>

#include <glog/logging.h>
#include <boost/optional.hpp>
#include <is/ResultSet.h>
#include <is/DmManager.h>

#include "dmi_utils/dm_bridge.hpp"


namespace dmi_utils {

/**
 * @brief A class that connect to DM.
 *
 * It set query to DM then receive the result as a ResultSet.
 *
 * This class handle the receive event then convert ResultSet as specified data class specified with templete parameter.
 *
 * @tparam T Data class
 */
template<class T>
class Downloader {
 public:
  using SharedPtr = std::shared_ptr<Downloader>;
  using CallbackType = std::function<void(const std::vector<T>& dataclasses)>;

  /**
   * @brief Config parameters
   */
  struct Config {
    /**
     * @brief Initialize with specified parameters
     *
     * @param in_ipaddr IP address that host DM
     * @param in_port Port number to receive data from DM
     * @param in_user User name to access DB in DM
     * @param in_pass Password to access DB in DM
     * @param in_connection_try_count Number of connection attempts to DM
     * @param in_timer_query Query string to create timer
     *
     *
     * @param in_register_query Query string to get information like ObjectInfo
     */
    Config(
      const std::string& in_ipaddr,
      const int in_port,
      const std::string& in_user,
      const std::string& in_pass,
      const uint32_t in_connection_try_count,
      const std::string& in_register_query)
      : ipaddr(in_ipaddr),
        port(in_port),
        user(in_user),
        pass(in_pass),
        connection_try_count(in_connection_try_count),
        register_query(in_register_query) {}

    const std::string& ipaddr;
    const int port;
    const std::string& user;
    const std::string& pass;
    const uint32_t connection_try_count;
    const std::string& register_query;
  };

  /**
   * @brief Make relationship with DM
   *
   * @param conf Downloader config object
   * @param user_callback A callback function called when new message event has occured
   */
  Downloader(
      const Config& conf,
      CallbackType user_callback)
      : user_callback_(user_callback) {
    uint32_t get_db_con_count = 0;
    while (get_db_con_count < conf.connection_try_count) {
      try {
        con_ = DmManager::getDBConnection(conf.ipaddr, conf.port, conf.user, conf.pass);
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

    /*
    registered_query_id_ = con_->registerQuery(
        conf.register_query, std::bind(&Downloader<T>::Update, this, std::placeholders::_1));
    */
    using UpdateFunctionType = void (Downloader<T>::*)(ResultSet);
    registered_query_id_ = con_->registerQuery(conf.register_query, std::bind(static_cast<UpdateFunctionType>(&Downloader<T>::Update), this, std::placeholders::_1));
  }

  /**
   * @brief Make relationship with DM
   *
   * @param conf Downloader config object
   * @param user_callback A callback function called when new message event has occured
   */
  Downloader(CallbackType user_callback) {
    user_callback_ = user_callback;
  }

  /**
   * @brief Cange query before shut this class down
   */
  ~Downloader() {
    con_->reconnect();
    con_->cancelQuery(registered_query_id_);
    con_->disconnect();
    delete con_;
  }

  /**
   * @brief Convert ResultSet to data class then call user callback function
   *
   * @param rs Query result
   */
  void Update(IS::TupleSet ts) {
    const auto recv_sec = std::chrono::system_clock::now().time_since_epoch().count();
    VLOG(1) << "recv time: " << recv_sec;
    std::vector<T> dataclasses;
    /* TODO: Call FromTupleSet(ts).*/
    for (int idx = 0; idx < ts.size(); idx++) {
      const auto o = DmBridge<T>::FromTuple(ts.getTuple(idx));
      if (o == boost::none) {
        LOG(WARNING) << "Failed to convert. Ignore this tuple";
        continue;
      }
      dataclasses.push_back(o.get());
    }
    const double conv_sec = (std::chrono::system_clock::now().time_since_epoch().count() - recv_sec) / 1000000000.0;
    VLOG(1) << "tuple num: " << static_cast<unsigned>(dataclasses.size());
    VLOG(1) << "convert takt: " << conv_sec;
    user_callback_(dataclasses);
  }

 private:
  /**
   * @brief Convert ResultSet to data class then call user callback function
   *
   * @param rs Query result
   */
  void Update(ResultSet rs) {
    const auto recv_sec = std::chrono::system_clock::now().time_since_epoch().count();
    VLOG(1) << "recv time: " << recv_sec;
    std::vector<T> dataclasses;
    while (rs.next()) {
      const auto o = DmBridge<T>::FromResultSet(rs);
      if (o == boost::none) {
        LOG(WARNING) << "Failed to convert. Ignore this tuple";
        continue;
      }
      dataclasses.push_back(o.get());
    }
    const double conv_sec = (std::chrono::system_clock::now().time_since_epoch().count() - recv_sec) / 1000000000.0;
    VLOG(1) << "tuple num: " << static_cast<unsigned>(dataclasses.size());
    VLOG(1) << "convert takt: " << conv_sec;
    user_callback_(dataclasses);
  }

  /**
   * @brief Connection object with DM
   */
  Connection* con_;

  /**
   * @brief Query ID provided by DM
   */
  int registered_query_id_;

  /**
   * @brief User callback function
   */
  CallbackType user_callback_;
};

}

#endif

