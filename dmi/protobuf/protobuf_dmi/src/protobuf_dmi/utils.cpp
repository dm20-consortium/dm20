#include "protobuf_dmi/utils.hpp"

#include <boost/filesystem.hpp>
#include <glog/logging.h>

using Path = boost::filesystem::path;
using ErrorCode = boost::system::error_code;

namespace protobuf_dmi
{
namespace utils
{

Config::Config()
  : rsu_id(0), sensor_id(0)
{
}

bool CreateLogDirIfNotExists(const std::string& dir_path) {
  const Path path = Path(dir_path);

  ErrorCode exists_error;
  const bool exists = boost::filesystem::exists(path, exists_error);
  // すでにある
  if (exists) return true;

  ErrorCode creation_error;
  const bool created = boost::filesystem::create_directories(path, creation_error);
  // つくれた
  if (created) return true;

  return false;
}

std::pair<int, int> CalcLoopNumber(
    const int max_number, const int input_data_number)
{
  int loop_num_to_set = 0;
  int loop_num_to_set_empty = 0;

  if (input_data_number > max_number)
  {
    loop_num_to_set = max_number;
  }
  else
  {
    loop_num_to_set = input_data_number;
    loop_num_to_set_empty = max_number - input_data_number;
  }

  return {loop_num_to_set, loop_num_to_set_empty};
}

};
};
