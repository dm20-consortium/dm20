#include <thread>
#include <chrono>

#include <gflags/gflags.h>

#include "udp_dmi/socket_sender.hpp"
#include <cool4_api_dataclass/freespace_info.hpp>
#include <dmi_utils/creator.hpp>
#include <cool4_api_dataclass/dataclasses_with_header.hpp>
#include <cool4_api_dataclass/config.h>

DEFINE_string(ipaddr, "127.0.0.1", "IP address of the target");
DEFINE_int32(port, 54346, "Port of the target");
DEFINE_uint32(
    frame_rate_max_msec,
    33,
    "If the number of data is larger than data_num_per_frame, the data is devided into mutiple frames."
    "the devided data is transmitted earlier than the normal cycle that defined in time_query."
    "This parameter is the frame rate used in this case.");
DEFINE_uint32(data_num_per_frame, 3, "Data num per frame (<= 6)");
DEFINE_uint32(info_num, 3, "Data num in send message");

using udp_dmi::SocketSender;
using dmi_utils::Creator;
using cool4_api_dataclass::DataClassesWithHeader;
using DataType = cool4_api_dataclass::FreespaceInfo;

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


  // サンプルフリースペースデータ
  typename DataType::Array msg(FLAGS_info_num);
  int v = 0;
  for (size_t i = 0; i < msg.size(); ++i) {
    // 物標ID（認識物標，機器ID固定，番号インクリメント）
    msg[i].id = ++v;
    msg[i].detection_method = ++v;
    msg[i].detectable_class = ++v;

    // 位置（頂点）
    msg[i].vertices_begin.projective_srid = ++v;
    msg[i].vertices_begin.prc_x = ++v;
    msg[i].vertices_begin.prc_y = ++v;
    msg[i].vertices_begin.prc_z = ++v;
    msg[i].vertices_num = 3;
    for (uint8_t j = 0; j < msg[i].vertices_num; ++j) {
      msg[i].vertices_x_list.push_back(++v);
      msg[i].vertices_y_list.push_back(++v);
    }

    // 位置
    msg[i].position_begin.geodetic_srid = ++v;
    msg[i].position_begin.latitude = ++v;
    msg[i].position_begin.longitude = ++v;
    msg[i].position_end.geodetic_srid = ++v;
    msg[i].position_end.latitude = ++v;
    msg[i].position_end.longitude = ++v;

    // 機器ID
    msg[i].info_src_list[0] = ++v;
  }

  std::thread working([&] {
    while (true) {
      // 時刻の更新
      const auto now = std::chrono::system_clock::now();
      const auto msec = uint64_t(now.time_since_epoch().count() / 1000000.0);
      // const uint64_t timestampits = msec - 1072882800000;
      for (auto& e : msg) e.time = msec;
      converter->ToByteArrayList(msg);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  });

  working.join();

  return EXIT_SUCCESS;
}
