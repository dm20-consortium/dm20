#include <gflags/gflags.h>
#include <glog/logging.h>

#include <cool4_api_dataclass/freespace_info.hpp>
#include <cool4_api_dataclass/dataclasses_with_header.hpp>
#include <dmi_utils/filesystem.hpp>
#include "udp_dmi/receiver.hpp"

using DataType = cool4_api_dataclass::FreespaceInfo;
template<class T>
using DataPackage = cool4_api_dataclass::DataClassesWithHeader<T>;
using dmi_utils::CreateLogDirIfNotExists;

DEFINE_string(dm_ipaddr, "127.0.0.1", "DM IP");
DEFINE_int32(dm_port, 9002, "DM port");
DEFINE_string(dm_user, "dm2sampleuser", "Username for DM");
DEFINE_string(dm_pass, "", "Password for DM");
DEFINE_int32(receive_port, 54346, "Port number to receive data");
DEFINE_uint32(connection_try_count, 3, "The number of connection attempts");

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  gflags::SetUsageMessage(
      "UDP-DMI receive upd data then send it to DM as DM tuple message");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  if (!FLAGS_log_dir.empty() && !CreateLogDirIfNotExists(FLAGS_log_dir)) {
    std::cerr << "Failed to create log dir (" << FLAGS_log_dir << ")" << std::endl;
    return EXIT_FAILURE;
  }

  udp_dmi::system::Receiver<DataType, DataPackage>::SharedPtr system(
      new udp_dmi::system::Receiver<DataType, DataPackage>(
        {
          FLAGS_dm_ipaddr,
          FLAGS_dm_port,
          FLAGS_dm_user,
          FLAGS_dm_pass,
          "freespace_info_0_8_1",
          FLAGS_connection_try_count
        }, FLAGS_receive_port));

  LOG(INFO) << "receiving bytes from " << FLAGS_receive_port;
  system->Spin();

  return EXIT_SUCCESS;
}
