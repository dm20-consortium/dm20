#ifndef PROTOBUF_DMI_SOCKET_HPP_
#define PROTOBUF_DMI_SOCKET_HPP_
#include <array>
#include <memory>
#include <functional>
#include <thread>
#include <vector>

#include <boost/optional.hpp>
#include <boost/asio.hpp>
#include <is/Tuple.h>

#include "protobuf_dmi/dm_bridge.hpp"
#include <glog/logging.h>
#include <ccam_cool4_sensor_io/sensor_io.pb.h>
#include <is/DmUtil.h>

#include "protobuf_dmi/dm_bridge.hpp"

using IoService = boost::asio::io_service;
using Udp = boost::asio::ip::udp;

namespace {
constexpr size_t kBufferSize = 65535;
}

namespace protobuf_dmi
{

template<class T>
class Receiver
{
public:
  using SharedPtr = std::shared_ptr<Receiver>;
  using CallbackType = std::function<void(
      const std::vector<IS::Tuple>&)>;

  Receiver(
      const int port,
      const typename DmBridge<T>::SharedPtrArray& converters,
      CallbackType user_callback)
    : converters_(converters),
      user_callback_(user_callback)
  {
    endpoint_.reset(new Udp::endpoint(boost::asio::ip::udp::v4(), port));

    worker_.reset(new IoService::work(io_service_));
    socket_.reset(new Udp::socket(io_service_, *endpoint_));

    remote_endpoint_.reset(new Udp::endpoint());

    RegisterNextEvent();

    auto run_func = [this]()
    {
      while (!io_service_.stopped())
      {
        io_service_.run();
      }
    };
    thread_.reset(new std::thread(run_func));
  }

  ~Receiver()
  {
    io_service_.stop();
    (*thread_).join();
  }

 private:
  void MessageEvent(
      const boost::system::error_code& err,
      size_t len)
  {
    (void)err;
    const auto recv_sec = std::chrono::system_clock::now().time_since_epoch().count();
    VLOG(1) << "recv time: " << recv_sec;

    const std::string message_string(
        receive_buffer_.begin(),
        receive_buffer_.begin() + len);

    T msg;
    if (!msg.ParseFromString(message_string))
    {
      LOG(WARNING) << "Failed to parse message";
      RegisterNextEvent();
      return;
    }

    const double conv_sec = (std::chrono::system_clock::now().time_since_epoch().count() - recv_sec) / 1000000000.0;
    VLOG(1) << "convert takt: " << conv_sec;

    std::vector<IS::Tuple> tuples;
    for (auto e : converters_)
    {
      e->ToTuple(
          msg,
          DmUtil::getTimeMillisec(),
          tuples);
    }

    user_callback_(tuples);

    RegisterNextEvent();
  }

  void RegisterNextEvent()
  {
    socket_->async_receive_from(
        boost::asio::buffer(
          receive_buffer_),
          *remote_endpoint_, std::bind(
            &protobuf_dmi::Receiver<T>::MessageEvent,
            this,
            std::placeholders::_1,
            std::placeholders::_2));
  }

  IoService io_service_;
  std::shared_ptr<IoService::work> worker_;
  std::shared_ptr<Udp::endpoint> endpoint_;
  std::shared_ptr<Udp::endpoint> remote_endpoint_;
  std::shared_ptr<Udp::socket> socket_;
  std::array<char, kBufferSize> receive_buffer_;
  std::shared_ptr<std::thread> thread_;

  typename DmBridge<T>::SharedPtrArray converters_;
  CallbackType user_callback_;
};

}  // namespace protobuf_dmi
#endif  // PROTOBUF_DMI_SOCKET_HPP_
