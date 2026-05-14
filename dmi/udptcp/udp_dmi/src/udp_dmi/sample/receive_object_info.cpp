#include <functional>
#include <chrono>
#include <thread>

#include <gflags/gflags.h>

#include <cool4_api_dataclass/object_info.hpp>
#include <cool4_api_dataclass/header.hpp>
#include <cool4_api_dataclass/dataclasses_with_header.hpp>
#include "udp_dmi/socket_receiver.hpp"

using cool4_api_dataclass::ObjectInfo;
using cool4_api_dataclass::Header;
using cool4_api_dataclass::ByteArray;
using udp_dmi::SocketReceiver;
using cool4_api_dataclass::DataClassesWithHeader;

void PrintReceiptObjectInfo(const DataClassesWithHeader<ObjectInfo>& dataclass) {
  const auto now = std::chrono::system_clock::now();
  const unsigned long long recv_time_msec = static_cast<unsigned long long>(
      now.time_since_epoch().count() / 1000000.0);

  // 受信時刻とデータを出す場合
  std::cout << "[" << std::fixed << recv_time_msec << "] " << dataclass << std::endl;

  // 受信時刻と生成時刻の差を出す場合
  // const unsigned long long gen_time_msec = dataclass.dataclasses[0].time;
  // std::cout << recv_time_msec << "," << gen_time_msec << "," << recv_time_msec - gen_time_msec << std::endl;
}

DEFINE_int32(receive_port, 54345, "Port number to receive data");


int main(int argc, char** argv) {
  gflags::SetUsageMessage(
      "A sample that receive object info UDP datagram");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  SocketReceiver<ObjectInfo, DataClassesWithHeader>::SharedPtr recv(new SocketReceiver<ObjectInfo, DataClassesWithHeader>(FLAGS_receive_port,
        std::bind(&PrintReceiptObjectInfo, std::placeholders::_1)));

  std::thread working([] {
    while (true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  });

  std::cout
    << "receiving bytes from " << FLAGS_receive_port << std::endl;
  working.join();
  return EXIT_SUCCESS;
}
