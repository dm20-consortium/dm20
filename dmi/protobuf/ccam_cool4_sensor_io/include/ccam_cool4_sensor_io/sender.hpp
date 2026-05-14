#ifndef CCAM_COOL4_SENSOR_IO_SOCKET_HPP_
#define CCAM_COOL4_SENSOR_IO_SOCKET_HPP_
#include <zlib.h>

#include <memory>
#include <string>

#include <boost/asio.hpp>
#include <glog/logging.h>

using IoService = boost::asio::io_service;
using Udp = boost::asio::ip::udp;

namespace ccam_cool4_sensor_io {
namespace socket {

template<class T>
class Sender {
 public:
  using SharedPtr = std::shared_ptr<Sender>;

  struct Config {
    Config(
        const std::string& in_host,
        const int in_port,
        const bool in_enable_crc = false)
      : host(in_host),
        port(in_port),
        enable_crc(in_enable_crc) {}

    const std::string host;
    const int port;
    const bool enable_crc;
  };

  Sender(const Config& conf) : enable_crc_(conf.enable_crc) {
    if (enable_crc_) LOG(INFO) << "CRC enabled";
    resolver_.reset(new Udp::resolver(io_service_));
    Udp::resolver::query query(Udp::v4(), conf.host, std::to_string(conf.port));

    endpoint_.reset(new Udp::endpoint(*resolver_->resolve(query)));

    socket_.reset(new Udp::socket(io_service_));
    socket_->open(Udp::v4());
  }

  ~Sender() = default;

  void Send(const T& msgs) {
    std::string msgs_string;
    if (!msgs.SerializeToString(&msgs_string)) {
      return;
    }

    std::vector<uint8_t> buf(msgs_string.begin(), msgs_string.end());

    if (enable_crc_)
    {
      const uint32_t crc_value = crc32(
          0,
          reinterpret_cast<const Bytef*>(msgs_string.data()),
          msgs_string.size());

      VLOG(1) << "crc: " << crc_value;

      for (int i = 0; i < 4; ++i)
      {
        buf.push_back(static_cast<uint8_t>((crc_value >> (i * 8)) & 0xff));
      }
    }
    VLOG(1) << "msg len: " << buf.size();

    socket_->send_to(boost::asio::buffer(buf), *endpoint_);
  }

 private:
  IoService io_service_;
  std::shared_ptr<Udp::resolver> resolver_;
  std::shared_ptr<Udp::socket> socket_;
  std::shared_ptr<Udp::endpoint> endpoint_;
  const bool enable_crc_;
};

}
}
#endif
