#include <thread>
#include <chrono>

#include <gflags/gflags.h>

#include "udp_dmi/socket_sender.hpp"
#include <cool4_api_dataclass/signal_info.hpp>
#include <dmi_utils/creator.hpp>
#include <cool4_api_dataclass/dataclasses_with_header.hpp>
#include <cool4_api_dataclass/config.h>

DEFINE_string(ipaddr, "127.0.0.1", "IP address of the target");
DEFINE_int32(port, 54347, "Port of the target");
DEFINE_uint32(
    frame_rate_max_msec,
    33,
    "If the number of data is larger than data_num_per_frame, the data is devided into mutiple frames."
    "the devided data is transmitted earlier than the normal cycle that defined in time_query."
    "This parameter is the frame rate used in this case.");
DEFINE_uint32(data_num_per_frame, 14, "Data num per frame (<= 14)");
DEFINE_uint32(info_num, 14, "Data num in send message");

using udp_dmi::SocketSender;
using dmi_utils::Creator;
using cool4_api_dataclass::DataClassesWithHeader;
using cool4_api_dataclass::SignalInfo;
using DataType = cool4_api_dataclass::SignalInfo;

int main(int argc, char** argv) {
  gflags::SetUsageMessage(
      "A sample that send freespace_info info as UDP datagram");
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


  // サンプル信号データ
  typename DataType::Array msg(FLAGS_info_num);

  // 交差点ID（適当）
  for (size_t i = 0; i < msg.size(); ++i) msg[i].crp_id = i;

  // 四辻の交差点
  // 車両x2 歩行者x2
  // Uターン禁止
  // 北方路を1として時計回り
  for (auto& e : msg)
    e.signal_id_list = {0x12, 0x13, 0x14, 0x31, 0x32, 0x34};
//msg[1].signal_id_list = {0x21, 0x23, 0x24, 0x41, 0x42, 0x43, 0, 0};
//msg[2].signal_id_list = {0x10, 0x30, 0, 0, 0, 0, 0, 0};
//msg[3].signal_id_list = {0x20, 0x40, 0, 0, 0, 0, 0, 0};

  // 車両信号状態
  for (auto& m : msg) m.light_information_list[0] = {3, 0, 200, 250};
//msg[0].light_information_list.push_back({5, 0, 150, 200});
//msg[0].light_information_list.push_back({9, 0, 50, 100});
//msg[1].light_information_list.push_back({5, 0, 150, 200});
//msg[1].light_information_list.push_back({9, 0, 50, 100});
//msg[1].light_information_list.push_back({3, 0, 200, 250});
//msg[2].light_information_list.push_back({5, 0, 150, 200});
//msg[2].light_information_list.push_back({9, 0, 50, 100});
//msg[2].light_information_list.push_back({3, 0, 200, 250});
//msg[3].light_information_list.push_back({5, 0, 150, 200});
//msg[3].light_information_list.push_back({9, 0, 50, 100});
//msg[3].light_information_list.push_back({3, 0, 200, 250});

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
      converter->ToByteArrayList(msg);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  });

  working.join();

  return EXIT_SUCCESS;
}
