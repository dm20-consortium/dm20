#include <thread>

#include <gflags/gflags.h>
#include <chrono>

#include "udp_dmi/socket_sender.hpp"
#include <cool4_api_dataclass/object_info.hpp>
#include <dmi_utils/creator.hpp>
#include <cool4_api_dataclass/dataclasses_with_header.hpp>
#include <cool4_api_dataclass/config.h>

DEFINE_string(ipaddr, "127.0.0.1", "IP address of the target");
DEFINE_int32(port, 54345, "Port of the target");
DEFINE_uint32(
    frame_rate_max_msec,
    33,
    "If the number of data is larger than data_num_per_frame, the data is devided into mutiple frames."
    "the devided data is transmitted earlier than the normal cycle that defined in time_query."
    "This parameter is the frame rate used in this case.");
DEFINE_uint32(data_num_per_frame, 6, "Data num per frame (<= 6)");
DEFINE_uint32(info_num, 6, "Data num in send message");

using udp_dmi::SocketSender;
using dmi_utils::Creator;
using cool4_api_dataclass::DataClassesWithHeader;
using DataType = cool4_api_dataclass::ObjectInfo;

int main(int argc, char** argv) {
  gflags::SetUsageMessage(
      "A sample that send object info as UDP datagram");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  SocketSender::SharedPtr sender(
      new SocketSender({
        FLAGS_ipaddr,
        FLAGS_port,
        FLAGS_frame_rate_max_msec,
        static_cast<uint32_t>(100 / FLAGS_frame_rate_max_msec)}));

  Creator<DataType>::SharedPtr converter(
      new Creator<DataType>({
        cool4_api_dataclass_VERSION_MAJOR,
        cool4_api_dataclass_VERSION_MINOR,
        cool4_api_dataclass_VERSION_PATCH,
        FLAGS_data_num_per_frame},
        std::bind(&SocketSender::Send, sender.get(), std::placeholders::_1)));


  // サンプル物標データ
  typename DataType::Array msg(FLAGS_info_num);
  // 物標ID（認識物標，機器ID固定，番号インクリメント）
  for (size_t i = 0; i < msg.size(); ++i) {
    msg[i].id = 0x8000000012345678 + (i << 32);
  }
  // 機器ID
  for (auto& e : msg) e.info_src_list[0] = 0x12345678;

  // 位置
  for (auto& e : msg) {
    e.object_location.geodetic_srid = 0;
    e.object_location.latitude = 35.6895014 * 10000000.0;
    e.object_location.longitude = 139.6917337 * 10000000.0;
  }

  // 大きさ
  for (auto& e : msg) {
    e.length.value = 2500;
    e.width.value = 1500;
    e.height.value = 1500;
  }

  std::thread working([&] {
    while (true) {
      // 時刻の更新
      const auto now = std::chrono::system_clock::now();
      const auto msec = uint64_t(now.time_since_epoch().count() / 1000000.0);
      for (auto& e : msg) e.time = msec;
      converter->ToByteArrayList(msg);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  });

  working.join();

  return EXIT_SUCCESS;
}
