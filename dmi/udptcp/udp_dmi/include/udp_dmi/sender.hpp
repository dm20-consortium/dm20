#ifndef UDP_DMI_SENDER_HPP_
#define UDP_DMI_SENDER_HPP_
#include <memory>
#include <functional>

#include <is/TupleSet.h>

#include <dmi_utils/base.hpp>
#include <dmi_utils/creator.hpp>
#include <dmi_utils/downloader.hpp>
#include "udp_dmi/socket_sender.hpp"

namespace udp_dmi {
namespace system {

template<class T>
class Sender : public dmi_utils::BaseWithSendInterface {
 public:
  using SharedPtr = std::shared_ptr<Sender>;

  Sender(
      const SocketSender::Config& socket_conf,
      const typename dmi_utils::Creator<T>::Config& frame_conf,
      const typename dmi_utils::Downloader<T>::Config& dm2_conf)
    : socket_(
          new SocketSender(socket_conf)),
      frame_(
          new dmi_utils::Creator<T>(
            frame_conf,
            std::bind(
              &SocketSender::Send,
              socket_.get(),
              std::placeholders::_1))),
      downloader_(
          new dmi_utils::Downloader<T>(
            dm2_conf,
            std::bind(
              &dmi_utils::Creator<T>::ToByteArrayList,
              frame_.get(),
              std::placeholders::_1))) {}

  Sender(
      const SocketSender::Config& socket_conf,
      const typename dmi_utils::Creator<T>::Config& frame_conf)
    : socket_(
          new SocketSender(socket_conf)),
      frame_(
          new dmi_utils::Creator<T>(
            frame_conf,
            std::bind(
              &SocketSender::Send,
              socket_.get(),
              std::placeholders::_1))),
      downloader_(
          new dmi_utils::Downloader<T>(
            std::bind(
              &dmi_utils::Creator<T>::ToByteArrayList,
              frame_.get(),
              std::placeholders::_1))) {}

  void Send(IS::TupleSet& ts) override {
    downloader_->Update(ts);
  }

 private:
  SocketSender::SharedPtr socket_;
  typename dmi_utils::Creator<T>::SharedPtr frame_;
  typename dmi_utils::Downloader<T>::SharedPtr downloader_;
};

}
}
#endif
