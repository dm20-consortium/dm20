/**
 * @file simple_receiver.hpp
 */
#ifndef UDP_DMI_SOCKET_RECEIVER_HPP_
#define UDP_DMI_SOCKET_RECEIVER_HPP_
#include "udp_dmi/config.h"
#include <array>
#include <memory>
#include <functional>
#include <thread>

#include <boost/optional.hpp>
#include <boost/asio.hpp>
#include <glog/logging.h>

#include <cool4_api_dataclass/header.hpp>
#include <cool4_api_dataclass/dataclasses_with_header.hpp>
#include <cool4_api_dataclass/bytearray.hpp>

using IoService = boost::asio::io_service;
using Udp = boost::asio::ip::udp;

namespace udp_dmi {

/**
 * @brief Receive UDP packet then convert it to data class object
 *
 * @tparam T Data class type(ObjectInfo, FreespaceInfo, SignalInfo)
 */
template<class DataClass, template<class> class DataPackage>
class SocketReceiver {
 public:
  using SharedPtr = std::shared_ptr<SocketReceiver>;
  using CallbackType = std::function<void(const DataPackage<DataClass>& dataclass)>;

  SocketReceiver(const int port, CallbackType user_callback) : user_callback_(user_callback) {
    endpoint_.reset(new Udp::endpoint(boost::asio::ip::udp::v4(), port));

    worker_.reset(new IoService::work(io_service_));
    socket_.reset(new Udp::socket(io_service_, *endpoint_));

    remote_endpoint_.reset(new Udp::endpoint());

    RegisterNextEvent();

    auto run_func = [this]() {
      while (!io_service_.stopped()) {
        io_service_.run();
      }
    };
    udp_thread_.reset(new std::thread(run_func));
  }

  ~SocketReceiver() {
    io_service_.stop();
    udp_thread_->join();
  }

 private:
  /**
   * @brief Convert message to data class object then call user callback function
   *
   * @param err Error code
   * @param len Length of recept message
   */
  void MessageEvent(const boost::system::error_code& err, size_t len) {
    (void)err;
    const auto recv_sec = std::chrono::system_clock::now().time_since_epoch().count();
    VLOG(2) << "recv time: " << recv_sec;

    const cool4_api_dataclass::ByteArray bytes(receive_buffer_.begin(), receive_buffer_.begin() + len);

    const auto frame = DataPackage<DataClass>::FromByteArray(bytes);
    if (frame == boost::none) {
      RegisterNextEvent();
      return;
    }
    if (VLOG_IS_ON(1)) {
      const auto header = frame.get().header;
      for (const auto& e : frame.get().dataclasses) {
        VLOG(1) << header << e;
      }
    }

    VLOG(2) << "data num: " << static_cast<unsigned>(frame.get().dataclasses.size());

    const double conv_sec = (std::chrono::system_clock::now().time_since_epoch().count() - recv_sec) / 1000000000.0;
    VLOG(2) << "convert takt: " << conv_sec;

    user_callback_(frame.get());

    RegisterNextEvent();
  }

  void RegisterNextEvent() {
    socket_->async_receive_from(
        boost::asio::buffer(
          receive_buffer_),
          *remote_endpoint_, std::bind(
            &SocketReceiver<DataClass, DataPackage>::MessageEvent,
            this,
            std::placeholders::_1,
            std::placeholders::_2));
  }

  IoService io_service_;
  std::shared_ptr<IoService::work> worker_;
  std::shared_ptr<Udp::endpoint> endpoint_;
  std::shared_ptr<Udp::endpoint> remote_endpoint_;
  std::shared_ptr<Udp::socket> socket_;
  std::array<cool4_api_dataclass::ByteType, BUFFER_SIZE> receive_buffer_;
  std::shared_ptr<std::thread> udp_thread_;

  CallbackType user_callback_;
};

}
#endif
