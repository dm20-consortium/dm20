#include <glog/logging.h>
#include <gflags/gflags.h>

#include "udp_dmi/sender.hpp"
#include <cool4_api_dataclass/sensor_info.hpp>
#include <cool4_api_dataclass/config.h>
#include <dmi_utils/filesystem.hpp>


using DataType = cool4_api_dataclass::SensorInfo;
using dmi_utils::CreateLogDirIfNotExists;


DEFINE_string(target_ipaddr, "127.0.0.1", "IP address of the target");
DEFINE_int32(target_port, 54348, "Port of the target");
DEFINE_string(dm_ipaddr, "127.0.0.1", "DM IP");
DEFINE_int32(dm_port, 9001, "DM port");
DEFINE_string(dm_user, "dm2sampleuser", "Username for DM");
DEFINE_string(dm_pass, "", "Password for DM");
DEFINE_string(
    continuous_query,
    "master sysTimer100msec select * from sensor_info_0_8_1 [range 300 msec]",
    "A query for DM objects");
DEFINE_uint32(
    dm_connection_try_count,
    3,
    "Attempt to connect a specified number of time");
DEFINE_uint32(data_num_per_frame, 1, "Data num per frame (<= 1)");
DEFINE_uint32(
    frame_rate_max_msec,
    33,
    "If the number of data is larger than data_num_per_frame, the data is devided into mutiple frames."
    "the devided data is transmitted earlier than the normal cycle that defined in time_query."
    "This parameter is the frame rate used in this case.");

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  gflags::SetUsageMessage(
      "UDP-DMI from_dm: download data from DM then send as UDP message");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  if (!FLAGS_log_dir.empty() && !CreateLogDirIfNotExists(FLAGS_log_dir)) {
    std::cerr << "Failed to create log dir (" << FLAGS_log_dir << ")" << std::endl;
    return EXIT_FAILURE;
  }

  udp_dmi::system::Sender<DataType>::SharedPtr system(
      new udp_dmi::system::Sender<DataType>(
        {
          FLAGS_target_ipaddr,
          FLAGS_target_port,
          FLAGS_frame_rate_max_msec,
          static_cast<uint32_t>(100 / FLAGS_frame_rate_max_msec)
        }, {
          cool4_api_dataclass_VERSION_MAJOR,
          cool4_api_dataclass_VERSION_MINOR,
          cool4_api_dataclass_VERSION_PATCH,
          FLAGS_data_num_per_frame
        }, {
          FLAGS_dm_ipaddr,
          FLAGS_dm_port,
          FLAGS_dm_user,
          FLAGS_dm_pass,
          FLAGS_dm_connection_try_count,
          FLAGS_continuous_query
        }));

  LOG(INFO)
    << "transmitting bytes to " << FLAGS_target_ipaddr
    << ":" << FLAGS_target_port << " ...";
  system->Spin();

  return EXIT_SUCCESS;
}
