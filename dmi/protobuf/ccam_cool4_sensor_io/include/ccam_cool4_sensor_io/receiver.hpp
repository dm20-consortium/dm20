#ifndef CCAM_COOL4_SENSOR_IO_RECEIVER_HPP_
#define CCAM_COOL4_SENSOR_IO_RECEIVER_HPP_
#include <zlib.h>

#include <array>
#include <memory>
#include <functional>
#include <thread>

#include <boost/optional.hpp>
#include <boost/asio.hpp>
#include <glog/logging.h>


using IoService = boost::asio::io_service;
using Udp = boost::asio::ip::udp;

namespace {
constexpr size_t kBufferSize = 65535;
}

namespace ccam_cool4_sensor_io {
namespace socket {

template<class T>
class Receiver {
 public:
  using SharedPtr = std::shared_ptr<Receiver>;
  using CallbackType = std::function<void(const T& dataclass)>;

  Receiver(
      const int port,
      CallbackType user_callback,
      const bool enable_crc = false)
      : user_callback_(user_callback),
        enable_crc_(enable_crc) {
    if (enable_crc_) LOG(INFO) << "CRC enabled";
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

 private:
  void MessageEvent(const boost::system::error_code& err, size_t len) {
    (void)err;
    const auto recv_sec = std::chrono::system_clock::now().time_since_epoch().count();
    VLOG(2) << "recv time: " << recv_sec;
    VLOG(1) << "len: " << len;

    size_t msg_len = len;
    uint32_t receipt_crc = 0;

    if (enable_crc_)
    {
      for (int i = 0; i < 4; ++i)
      {
        receipt_crc |= (static_cast<uint32_t>(receive_buffer_[msg_len - 4 + i]) << (i * 8));
      }

      msg_len = len - 4;
      VLOG(1) << "crc: " << receipt_crc;

    }

    const std::string message_string(receive_buffer_.begin(), receive_buffer_.begin() + msg_len);

    if (enable_crc_)
    {
      const uint32_t msg_crc = crc32(
          0,
          reinterpret_cast<const Bytef*>(message_string.data()),
          message_string.size());
      if (msg_crc != receipt_crc)
      {
        LOG(WARNING) << "CRC value is differ. We ignore this message";
        RegisterNextEvent();
        return;
      }
    }

    T msg;
    if (!msg.ParseFromString(message_string)) {
      LOG(WARNING) << "Failed to parse message";
      RegisterNextEvent();
      return;
    }

    const double conv_sec = (std::chrono::system_clock::now().time_since_epoch().count() - recv_sec) / 1000000000.0;
    VLOG(2) << "convert takt: " << conv_sec;

    user_callback_(msg);

    RegisterNextEvent();
  }

  void RegisterNextEvent() {
    socket_->async_receive_from(
        boost::asio::buffer(
          receive_buffer_),
          *remote_endpoint_, std::bind(
            &ccam_cool4_sensor_io::socket::Receiver<T>::MessageEvent,
            this,
            std::placeholders::_1,
            std::placeholders::_2));
  }

  IoService io_service_;
  std::shared_ptr<IoService::work> worker_;
  std::shared_ptr<Udp::endpoint> endpoint_;
  std::shared_ptr<Udp::endpoint> remote_endpoint_;
  std::shared_ptr<Udp::socket> socket_;
  std::array<uint8_t, kBufferSize> receive_buffer_;
  std::shared_ptr<std::thread> udp_thread_;

  CallbackType user_callback_;
  const bool enable_crc_;
};

}
}
#endif
