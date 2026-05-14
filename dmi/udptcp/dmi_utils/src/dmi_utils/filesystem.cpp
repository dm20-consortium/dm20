#include "dmi_utils/filesystem.hpp"

#include <string>
#include <iostream>

#include <boost/filesystem.hpp>
#include <glog/logging.h>

using Path = boost::filesystem::path;
using ErrorCode = boost::system::error_code;

namespace dmi_utils {

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

}
