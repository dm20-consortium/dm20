#include "protobuf_dmi/protobuf_dmi.hpp"

#include <glog/logging.h>
#include <gflags/gflags.h>

DEFINE_string(
    target_ipaddr,
    "127.0.0.1",
    "IP address of the target");
DEFINE_string(dm_ipaddr, "127.0.0.1", "DM IP");
DEFINE_int32(dm_port, 9002, "DM port");
DEFINE_string(dm_user, "dm2sampleuser", "Username for DM");
DEFINE_string(dm_pass, "", "Password for DM");
DEFINE_uint32(
    dm_connection_try_count,
    3,
    "Attempt to connect a specified number of time");
DEFINE_int32(
    receive_port,
    31876,
    "Reception port for proto");
DEFINE_uint32(rsu_id, 0x12345678, "The Road Side Unit ID");
DEFINE_uint32(sensor_id, 0x01, "The Sensor ID of RSU");

using namespace protobuf_dmi;

using DataType = DATA_TYPE;
using BridgeType = BRIDGE_TYPE;

int main(int argc, char** argv)
{
  google::InitGoogleLogging(argv[0]);
  gflags::SetUsageMessage(
      "UDP-DMI from_dm: download data from DM then send as UDP message");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  if (!FLAGS_log_dir.empty() && !utils::CreateLogDirIfNotExists(FLAGS_log_dir)) {
    std::cerr << "Failed to create log dir (" << FLAGS_log_dir << ")" << std::endl;
    return EXIT_FAILURE;
  }

  utils::Config conf;
  conf.rsu_id = FLAGS_rsu_id;
  conf.sensor_id = FLAGS_sensor_id;

  Uploader::SharedPtr uploader(
      new Uploader(
        {
          FLAGS_dm_ipaddr,
          FLAGS_dm_port,
          FLAGS_dm_user,
          FLAGS_dm_pass,
          FLAGS_dm_connection_try_count
        }));
  Receiver<DataType>::SharedPtr receiver(
      new Receiver<DataType>(
        FLAGS_receive_port,
        {
          std::make_shared<BridgeType>(BridgeType(conf))
        },
        std::bind(
          &Uploader::Upload,
          uploader.get(),
          std::placeholders::_1)));

  std::thread wait_loop(
    []
    {
      while (true)
      {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(10));
      }
    });
  wait_loop.join();

  return EXIT_SUCCESS;
}
