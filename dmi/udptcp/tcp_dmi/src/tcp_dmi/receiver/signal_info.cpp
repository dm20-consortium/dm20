#include <gflags/gflags.h>
#include <glog/logging.h>
#include <cool4_api_dataclass/signal_info.hpp>
#include <dmi_utils/filesystem.hpp>
#include "tcp_dmi/server.hpp"

using dmi_utils::CreateLogDirIfNotExists;
using cool4_api_dataclass::SignalInfo;
template<class T>
using DataClasses = cool4_api_dataclass::DataClassesWithHeader<T>;


DEFINE_string(dm_ipaddr, "127.0.0.1", "DM IP");
DEFINE_int32(dm_port, 9002, "DM port");
DEFINE_string(dm_user, "dm2sampleuser", "Username for DM");
DEFINE_string(dm_pass, "", "Password for DM");
DEFINE_uint32(connection_try_count, 3, "The number of connection attempts");
DEFINE_int32(receive_port, 54347, "Port number to receive data");


int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  gflags::SetUsageMessage(
      "TCP-DMI :receive tcp data then send it to DM as DM tuple message");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  if (!FLAGS_log_dir.empty() && !CreateLogDirIfNotExists(FLAGS_log_dir)) {
    std::cerr << "Failed to create log dir (" << FLAGS_log_dir << ")" << std::endl;
    return EXIT_FAILURE;
  }

  tcp_dmi::Server<SignalInfo, DataClasses>::SharedPtr system(
      new tcp_dmi::Server<SignalInfo, DataClasses>(
        {
          FLAGS_dm_ipaddr,
          FLAGS_dm_port,
          FLAGS_dm_user,
          FLAGS_dm_pass,
          "signal_info",
          FLAGS_connection_try_count
        },
        100,
        FLAGS_receive_port));

  std::thread working([] {
    while (true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  });

  LOG(INFO) << "receiving bytes from " << FLAGS_receive_port;

  system->Spin();
  working.join();

  return EXIT_SUCCESS;
}
