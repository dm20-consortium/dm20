#include <thread>
#include <chrono>

#include <gflags/gflags.h>
#include <google/protobuf/text_format.h>

#include "sensor_io.pb.h"
#include "ccam_cool4_sensor_io/sender.hpp"

namespace {
constexpr uint32_t kSensorInfoNumPerMsg = 1;
constexpr uint32_t kDetectCapabilityNum = 8;
constexpr uint32_t kObjectClassNum = 1;
constexpr uint32_t kPolyPointNum = 12;

// センサー
void SetHeader(int& v, SensingMessage& m)
{
  m.set_message_id(v++);
  m.set_protocol_version(v++);
  m.set_message_counter(v++);
  m.set_sensing_time(v++);
  m.set_error_notification(v++);
  m.set_error_code(v++);
}

void SetSensorInfo(
    const uint32_t sensor_info_num,
    const uint32_t detect_capability_num,
    int& v,
    SensingMessage& m)
{
  for (uint32_t isi = 0; isi < sensor_info_num; ++isi)
  {
    SensorInformation* s = m.add_sensor_info();

    s->set_type(ST_UNKNOWN);
    s->set_latitude(v++);
    s->set_longitude(v++);
    s->set_altitude(v++);

    for (uint32_t idc = 0; idc < detect_capability_num; ++idc)
    {
      DetectCapability* dc = s->add_detect_capabilities();
      dc->set_detectable_classes(v++);
      for (int ipp = 0; ipp < 4; ++ipp)
      {
        OffsetPointXY* p = dc->add_poly_points();
        p->set_dx(v++);
        p->set_dy(v++);
      }
      dc->set_confidence(v++);
      dc->set_detectable_size(v++);
    }
    s->set_sensor_status(v++);
  }
}

void SetObjectInfo(
    const uint32_t object_info_num,
    const uint32_t object_class_num,
    int& v,
    SensingMessage& m)
{
  for (uint32_t ioi = 0; ioi < object_info_num; ++ioi)
  {
    ObjectInformation* o = m.add_object_infos();

    o->set_object_id(v++);
    o->set_time_of_measurement(0);

    for (uint32_t ioc = 0; ioc < object_class_num; ++ioc)
    {
      ObjectClass* oc = o->add_object_classes();
      oc->set_vehicle_subclass_type(VSCT_UNKNOWN);
      oc->set_train_subclass_type(TSCT_UNKNOWN);
      oc->set_class_confidence(v++);
      oc->set_subclass_confidence(v++);
    }

    o->set_confidence(v++);

    Position* pos = o->mutable_position();
    pos->set_latitude(v++);
    pos->set_longitude(v++);
    pos->set_altitude(v++);
    pos->set_semi_major_axis_length(v++);
    pos->set_semi_minor_axis_length(v++);
    pos->set_semi_major_orientation(v++);
    pos->set_altitude_accuracy(v++);

    o->set_ref_point(RP_UNKNOWN);
    o->set_heading(9000 * 0.01 / 0.0125);
    o->set_heading_accuracy(0);
    o->set_speed(0);
    o->set_speed_accuracy(0);
    o->set_yaw_rate(0);
    o->set_yaw_rate_accuracy(0);
    o->set_acceleration(0);
    o->set_acceleration_accuracy(0);
    o->set_orientation(9000 * 0.01 / 0.0125);
    o->set_orientation_accuracy(0);
    o->set_length(300);
    o->set_length_accuracy(0);
    o->set_width(200);
    o->set_width_accuracy(0);
    o->set_height(100);
    o->set_height_accuracy(0);
    o->set_static_status(v++);
    o->set_tracking_status(v++);
    o->set_detection_count(v++);
    o->set_lost_count(v++);
    o->set_object_age(v++);
  }
}

void SetFreespaceInfo(
    const uint32_t freespace_info_num,
    const uint32_t poly_point_num,
    int& v,
    SensingMessage& m)
{
  for (uint32_t ifs = 0; ifs < freespace_info_num; ++ifs)
  {
    PerceivedFreeSpaceInformation* fs = m.add_freespace_infos();

    fs->set_time_of_measurement(0);

    Position* pos = fs->mutable_position();
    pos->set_latitude(v++);
    pos->set_longitude(v++);
    pos->set_altitude(v++);
    pos->set_semi_major_axis_length(v++);
    pos->set_semi_minor_axis_length(v++);
    pos->set_semi_major_orientation(v++);
    pos->set_altitude_accuracy(v++);

    for (uint32_t ipp = 0; ipp < poly_point_num; ++ipp)
    {
      OffsetPointXY* p = fs->add_poly_points();
      p->set_dx(v++);
      p->set_dy(v++);
    }

    fs->set_confidence(v++);
    fs->set_detectable_size(v++);
  }
}
}

using ccam_cool4_sensor_io::socket::Sender;

DEFINE_string(ipaddr, "127.0.0.1", "IP address of the target");
DEFINE_int32(port, 31876, "Port of the target");
DEFINE_int32(rate_msec, 100, "Rate of the loop");
DEFINE_int32(object_info_num, 1, "A number of object info per one SensingMessage");
DEFINE_bool(enable_crc, false, "A flag that indicate whether using CRC or not");

int main(int argc, char** argv)
{
  gflags::SetUsageMessage(
      "A sample that send CCAM-CooL4 Sensor IO UDP datagram");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  int vhd = 0;
  int vsi = 0;
  int voi = 0;
  int vfs = 0;
  SensingMessage m = SensingMessage();

  SetHeader(vhd, m);
  SetSensorInfo(
      kSensorInfoNumPerMsg,
      kDetectCapabilityNum,
      vsi,
      m);
  SetObjectInfo(
      FLAGS_object_info_num,
      kObjectClassNum,
      voi,
      m);
  SetFreespaceInfo(
      FLAGS_object_info_num,
      kPolyPointNum,
      vfs,
      m);

  Sender<SensingMessage>::SharedPtr socket(new Sender<SensingMessage>({
        FLAGS_ipaddr,
        FLAGS_port,
        FLAGS_enable_crc}));

  std::thread working([&] {
      while (true) {
        const auto st = std::chrono::system_clock::now();
        const auto st_nsec = st.time_since_epoch().count();
        const uint64_t st_msec = static_cast<uint64_t>(st_nsec / 1000000.0) - 1072915195000;
        m.set_sensing_time(st_msec);
        socket->Send(m);

        std::this_thread::sleep_for(std::chrono::milliseconds(FLAGS_rate_msec));
      }
    });;

  working.join();

  return EXIT_SUCCESS;
}
