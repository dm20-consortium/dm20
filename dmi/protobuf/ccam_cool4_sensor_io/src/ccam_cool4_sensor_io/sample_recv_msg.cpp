#include <functional>
#include <thread>
#include <chrono>

#include <gflags/gflags.h>
#include <google/protobuf/text_format.h>

#include "sensor_io.pb.h"
#include "ccam_cool4_sensor_io/receiver.hpp"

using ccam_cool4_sensor_io::socket::Receiver;

void MessageEvent(const SensingMessage& msg) {
  VLOG(2) << msg.DebugString();
}


DEFINE_int32(port, 31876, "Port of the target");
DEFINE_bool(enable_crc, false, "A flag that indicate whether using CRC or not");


int main(int argc, char** argv) {
  gflags::SetUsageMessage(
      "A sample that receive CCAM-CooL4 Sensor IO UDP datagram");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  Receiver<SensingMessage>::SharedPtr socket(new Receiver<SensingMessage>(
        FLAGS_port,
        std::bind(&MessageEvent, std::placeholders::_1),
        FLAGS_enable_crc));

  std::thread working([&] {
      while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    });;

  working.join();
  return EXIT_SUCCESS;
}
