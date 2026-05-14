#ifndef PROTOBUF_DMI_UTILS_HPP_
#define PROTOBUF_DMI_UTILS_HPP_
#include <cstdint>
#include <vector>
#include <string>

namespace protobuf_dmi
{
namespace utils
{

struct Config
{
public:
  Config();
  ~Config() = default;

  uint32_t rsu_id;
  uint32_t sensor_id;
};

bool CreateLogDirIfNotExists(const std::string& dir_path);

std::pair<int, int> CalcLoopNumber(
    const int max_number, const int input_data_number);

}  // utils
}  // protobuf_dmi
#endif  // PROTOBUF_DMI_UTILS_HPP_
