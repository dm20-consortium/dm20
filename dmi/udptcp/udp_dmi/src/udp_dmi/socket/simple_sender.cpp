#include "udp_dmi/socket_sender.hpp"

#include <chrono>
#include <thread>
#include <iostream>

#include <glog/logging.h>

namespace udp_dmi {

SocketSender::SocketSender(const Config& conf)
  : rate_msec_(conf.rate_msec), send_limit_count_(conf.send_limit_count) {
  socket_.reset(new Udp::socket(io_service_));
  socket_->open(Udp::v4());

  endpoint_.reset(new Udp::endpoint(
        boost::asio::ip::address::from_string(conf.host),
        conf.port));
}

void SocketSender::Send(const std::vector<cool4_api_dataclass::ByteArray>& bytearray) {
  const auto send_sec = std::chrono::system_clock::now().time_since_epoch().count();
  VLOG(1) << "send time: " << send_sec;
  uint32_t send_count = 0;
  for (size_t i = 0; i < bytearray.size(); ++i) {
    // すべてのデータを送信しようとすると，上位のデータ発行周期に間に合わないことがある
    // そのため，送信回数には制限を設ける
    if (i == send_limit_count_) break;

    socket_->send_to(boost::asio::buffer(bytearray[i]), *endpoint_);
    ++send_count;

    // 余計なスリープを挟まないために，最後のデータを送信し終えたら脱ループ
    if (i == bytearray.size() - 1) break;

    std::this_thread::sleep_for(std::chrono::milliseconds(rate_msec_));
  }
}

}
