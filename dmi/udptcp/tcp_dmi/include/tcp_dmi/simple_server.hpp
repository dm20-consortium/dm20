#ifndef TCP_DMI_SIMPLE_SERVER_HPP_
#define TCP_DMI_SIMPLE_SERVER_HPP_

#include <iostream>
#include <functional>
#include <memory>
#include <array>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/optional.hpp>
#include <glog/logging.h>
#include "tcp_dmi/config.h"
#include <cool4_api_dataclass/bytearray.hpp>

namespace tcp_dmi {

using IoService = boost::asio::io_service;
using Tcp = boost::asio::ip::tcp;
using Buffer = std::array<cool4_api_dataclass::ByteType, BUFFER_SIZE>;
using ErrorCode = boost::system::error_code;
using cool4_api_dataclass::ByteArray;

template<class DataType, template<class> class DataPackage>
class SimpleServer {
 public:
  using SharedPtr = std::shared_ptr<SimpleServer>;
  using MessageCallbackType = std::function<void(const DataPackage<DataType>& data)>;
  using DisconnectionCallbackType = std::function<void()>;

  SimpleServer(
      const int port,
      MessageCallbackType message_callback,
      DisconnectionCallbackType disconnection_callback)
    : port_(port),
      message_callback_(message_callback),
      disconnection_callback_(disconnection_callback) {
    acceptor_.reset(new Tcp::acceptor(io_service_, Tcp::endpoint(Tcp::v4(), port_)));
    socket_.reset(new Tcp::socket(io_service_));

    AsyncAccept();
    LOG(INFO) << "TCP-DMI has initialized";
  }

  void Run() { io_service_.run(); }

  void Reset() {
    socket_->close();
    acceptor_->close();
    acceptor_.reset(new Tcp::acceptor(io_service_, Tcp::endpoint(Tcp::v4(), port_)));
    socket_.reset(new Tcp::socket(io_service_));
  }

  void OnAccept(const ErrorCode& error) {
    LOG(INFO) << "accept" << std::endl;
    if (error) {
      LOG(ERROR) << error.message();
      return;
    }

    AsyncRead();
  }

  void MessageEvent(const ErrorCode& error, size_t bytes_transferred) {
    VLOG(2) << "read size: " << (int)bytes_transferred << std::endl;
    // 何らかのエラーの場合は再度読み込みを試みる
    if (error && error != boost::asio::error::eof) {
      std::cout << error.message() << std::endl;
      AsyncRead();
      return;
    }
    // クライアントが落ちた場合は，ソケットのリセットを行い待機する
    if (error == boost::asio::error::connection_reset ||
        error == boost::asio::error::eof) {
      LOG(WARNING) << "disconnected: " << error.message() << std::endl;
      disconnection_callback_();
      Reset();
      AsyncAccept();
      return;
    }

    const ByteArray data(recv_buffer_.begin(), recv_buffer_.begin() + bytes_transferred);
    const auto frame = DataPackage<DataType>::FromByteArray(data);
    if (frame == boost::none) {
      LOG(WARNING) << "failed to convert" << std::endl;
      return;
    }

    if (VLOG_IS_ON(1)) {
      const auto header = frame.get().header;
      for (const auto& e : frame.get().dataclasses) {
        VLOG(1) << header << e;
      }
    }

    message_callback_(frame.get());

    // response ?

    AsyncRead();
  }

  void AsyncRead() {
    boost::asio::async_read(
        *socket_,
        boost::asio::buffer(recv_buffer_),
        boost::asio::transfer_at_least(1),
        boost::bind(
          &SimpleServer<DataType, DataPackage>::MessageEvent,
          this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

  void AsyncWrite() {
    boost::asio::async_write(
        *socket_,
        resp_buffer_,
        boost::bind(
          &SimpleServer<DataType, DataPackage>::Response,
          this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

  void AsyncAccept() {
    acceptor_->async_accept(
        *socket_,
        boost::bind(
          &SimpleServer<DataType, DataPackage>::OnAccept,
          this,
          boost::asio::placeholders::error));
  }

  void Response() {
    // nothing to do
  }

  // テスト用
  void Stop() {
    io_service_.stop();
  }

 private:
  const int port_;
  MessageCallbackType message_callback_;
  DisconnectionCallbackType disconnection_callback_;
  std::shared_ptr<Tcp::socket> socket_;
  std::shared_ptr<Tcp::acceptor> acceptor_;
  Buffer recv_buffer_;
  Buffer resp_buffer_;

  IoService io_service_;
};

}


#endif
