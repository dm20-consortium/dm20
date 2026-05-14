/**
 * @file simple_sender.hpp
 */
#ifndef UDP_DMI_SOCKET_SENDER_HPP_
#define UDP_DMI_SOCKET_SENDER_HPP_
#include <memory>
#include <string>
#include <vector>

#include <boost/asio.hpp>

#include <cool4_api_dataclass/bytearray.hpp>

namespace udp_dmi {

using IoService = boost::asio::io_service;
using Udp = boost::asio::ip::udp;

/**
 * @brief Send dataclass as byte array
 *
 * Supported dataclasses is:
 *
 * - ObjectInfo
 * - FreespaceInfo
 * - SignalInfo
 */
class SocketSender {
 public:
  using SharedPtr = std::shared_ptr<SocketSender>;

  /**
   * @brief SocketSender config
   */
  struct Config {
    /**
     * @brief Initialize with specified parameters
     *
     * @param in_host Host IP address to send byte array
     * @param in_port Port number to send byte array
     * @param in_rate_msec Transport ratency(msec)
     * @param in_send_limit_count Max number of date per calling Send function
     */
    Config(
        const std::string& in_host,
        const int in_port,
        const uint32_t in_rate_msec,
        const uint32_t in_send_limit_count)
      : host(in_host),
        port(in_port),
        rate_msec(in_rate_msec),
        send_limit_count(in_send_limit_count) {}

    const std::string host;
    const int port;
    const uint32_t rate_msec;
    const uint32_t send_limit_count;
  };

  /**
   * @brief Constructor
   *
   * @param conf SocketSender config
   */
  SocketSender(const Config& conf);

  /**
   * @brief Destructor
   */
  ~SocketSender() = default;

  void Send(const std::vector<cool4_api_dataclass::ByteArray>& bytearray);

 private:
  IoService io_service_;
  std::shared_ptr<Udp::socket> socket_;
  std::shared_ptr<Udp::endpoint> endpoint_;

  /**
   * @brief Transport ratency(msec)
   */
  const uint32_t rate_msec_;

  /**
   * @brief Max number of date per calling Send function
   */
  const uint32_t send_limit_count_;
};

}

#endif
