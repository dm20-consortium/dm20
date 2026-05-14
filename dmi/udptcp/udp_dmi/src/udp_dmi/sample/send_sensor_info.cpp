#include <thread>

#include <gflags/gflags.h>
#include <chrono>

#include "udp_dmi/socket_sender.hpp"
#include <cool4_api_dataclass/sensor_info.hpp>
#include <dmi_utils/creator.hpp>
#include <cool4_api_dataclass/dataclasses_with_header.hpp>
#include <cool4_api_dataclass/config.h>

DEFINE_string(ipaddr, "127.0.0.1", "IP address of the target");
DEFINE_int32(port, 54348, "Port of the target");
DEFINE_uint32(
    frame_rate_max_msec,
    33,
    "If the number of data is larger than data_num_per_frame, the data is devided into mutiple frames."
    "the devided data is transmitted earlier than the normal cycle that defined in time_query."
    "This parameter is the frame rate used in this case.");
DEFINE_uint32(data_num_per_frame, 1, "Data num per frame (<= 1)");
DEFINE_uint32(info_num, 1, "Data num in send message");

using udp_dmi::SocketSender;
using dmi_utils::Creator;
using cool4_api_dataclass::DataClassesWithHeader;
using DataType = cool4_api_dataclass::SensorInfo;

int main(int argc, char** argv) {
  gflags::SetUsageMessage(
      "A sample that send sensor info as UDP datagram");
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
  for (auto& e : msg) {
    uint64_t value = 0;
    e.unit_id = value++;

    e.sensor_id = value++;
    e.sensor_type = value++;

    e.position.geodetic_srid = value++;
    e.position.latitude = value++;
    e.position.longitude = value++;
    e.position.altitude = value++;
    e.position.projective_srid = value++;
    e.position.prc_x = value++;
    e.position.prc_y = value++;
    e.position.prc_z = value++;
    e.position.crp_id = value++;
    e.position.dx_crp = value++;
    e.position.dy_crp = value++;
    e.position.dh_crp = value++;
    e.position.lane_count = value++;
    e.position.lane_position = value++;
    e.position.lane_lateral_position = value++;
    e.position.crp_id_begin = value++;
    e.position.crp_id_end = value++;
    e.position.lane_vertical_position = value++;
    e.position.lane_id = value++;
    e.position.dx_lane = value++;
    e.position.dy_lane = value++;
    e.position.dh_lane = value++;
    e.position.semi_axis_length_major = value++;
    e.position.semi_axis_length_minor = value++;
    e.position.orientation = value++;
    e.position.altitude_accuracy = value++;

    e.detectable_capability_num = 3;
    e.detectable_capabilities.resize(e.detectable_capability_num);
    for (uint8_t i = 0; i < e.detectable_capability_num; ++i) {
      e.detectable_capabilities[i].detectable_class = value++;
      e.detectable_capabilities[i].vertices_num = 3;
      for (uint8_t j = 0; j < e.detectable_capabilities[i].vertices_num; ++j) {
        e.detectable_capabilities[i].vertices_x_list.push_back(value++);
      }
      for (uint8_t j = 0; j < e.detectable_capabilities[i].vertices_num; ++j) {
        e.detectable_capabilities[i].vertices_y_list.push_back(value++);
      }
      e.detectable_capabilities[i].confidence = value++;
      e.detectable_capabilities[i].minimal_detectable_size = value++;
    }
    e.sensor_status = value++;
  }

  std::thread working([&] {
    while (true) {
      const auto now = std::chrono::system_clock::now();
      const auto msec = uint64_t(now.time_since_epoch().count() / 1000000.0);
      for (auto& e : msg) e.position.lane_id = msec;
      converter->ToByteArrayList(msg);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  });

  working.join();

  return EXIT_SUCCESS;
}
