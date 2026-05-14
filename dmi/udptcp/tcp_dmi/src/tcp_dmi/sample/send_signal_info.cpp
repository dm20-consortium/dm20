#include <iostream>
#include <thread>
#include <chrono>

#include <boost/asio.hpp>
#include <gflags/gflags.h>
#include <cool4_api_dataclass/signal_info.hpp>
#include <dmi_utils/creator.hpp>
#include <cool4_api_dataclass/dataclasses_with_header.hpp>
#include "tcp_dmi/config.h"

using cool4_api_dataclass::SignalInfo;
using dmi_utils::Creator;

DEFINE_string(ipaddr, "127.0.0.1", "IP address of the target");
DEFINE_int32(port, 54347, "Port of the target");

int main(int argc, char** argv) {
  gflags::SetUsageMessage(
      "A sample of the TCP client that send signal info to specified IP and port");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  namespace asio = boost::asio;
  using tcp = asio::ip::tcp;

  asio::io_service io_service;
  tcp::socket socket(io_service);

  tcp::endpoint endpoint(
      asio::ip::address::from_string(FLAGS_ipaddr),
      FLAGS_port);

  socket.connect(endpoint);

  std::cout << "Connected!" << std::endl;

  Creator<SignalInfo>::SharedPtr converter(
      new Creator<SignalInfo>({
        tcp_dmi_VERSION_MAJOR,
        tcp_dmi_VERSION_MINOR,
        tcp_dmi_VERSION_PATCH,
        1},
        [&](const std::vector<cool4_api_dataclass::ByteArray>& bytearray)
        {
          std::cout << "got bytearray" << std::endl;
          for (const auto& e : bytearray)
          {
            asio::write(socket, asio::buffer(e));
          }
        }));

  // Test Data
  typename SignalInfo::Array msg(1);
  for (size_t i = 0; i < msg.size(); ++i) msg[i].crp_id = i;
  for (auto& e : msg) e.signal_id_list = {0x12, 0x13, 0x14, 0x31, 0x32, 0x34};
  for (auto& m : msg) m.light_information_list[0] = {3, 0, 200, 250};

  std::thread working([&] {
    while (true) {
      // 時刻の更新
      const auto now = std::chrono::system_clock::now();
      const auto msec = uint64_t(now.time_since_epoch().count() / 1000000.0);
      // const uint64_t timestampits = msec - 1072882800000;
      for (auto& e : msg) e.time = msec;
      // イベントカウントの更新
      // 実際は変化したタイミング
      for (auto& e : msg) e.event_count += 1;
      std::cout << "ev cnt: " << static_cast<int>(msg[0].event_count) << std::endl;
      converter->ToByteArrayList(msg);
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      std::cout << "send" << std::endl;
    }
  });
  std::cout << "working..." << std::endl;

  working.join();

  return EXIT_SUCCESS;
}

