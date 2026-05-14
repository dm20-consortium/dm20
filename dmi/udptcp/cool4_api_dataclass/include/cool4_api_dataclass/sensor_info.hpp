#ifndef COOL4_API_DATACLASS_SENSOR_INFO_HPP_
#define COOL4_API_DATACLASS_SENSOR_INFO_HPP_
#include <array>
#include <map>

#include <boost/optional.hpp>

#include "cool4_api_dataclass/bytearray.hpp"
#include "cool4_api_dataclass/byte_stacker.hpp"
#include "cool4_api_dataclass/byte_extractor.hpp"
#include "cool4_api_dataclass/object_info.hpp"

namespace cool4_api_dataclass {

struct DetectableCapability {
  DetectableCapability();
  ~DetectableCapability() = default;

  boost::optional<ByteArray> ToByteArray() const;
  static boost::optional<DetectableCapability> FromByteArray(const ByteArray& byte_array, size_t& idx);
  friend std::ostream& operator<<(std::ostream& os, const DetectableCapability& o);

  uint8_t detectable_class;
  uint8_t vertices_num;
  std::vector<int> vertices_x_list;
  std::vector<int> vertices_y_list;
  uint8_t confidence;
  uint16_t minimal_detectable_size;
};

struct SensorInfo {
  using Array = std::vector<SensorInfo>;
  static constexpr size_t ElementNum = 71;
  static constexpr size_t DetectableCapabilityNum = 8;

  SensorInfo();
  ~SensorInfo() = default;

  boost::optional<ByteArray> ToByteArray() const;
  static boost::optional<SensorInfo> FromByteArray(const ByteArray& byte_array, size_t& idx);
  friend std::ostream& operator<<(std::ostream& os, const SensorInfo& o);

  unsigned long long unit_id;
  uint8_t sensor_id;
  uint8_t sensor_type;
  Location position;
  unsigned long long time;
  uint8_t detectable_capability_num;
  std::vector<DetectableCapability> detectable_capabilities;
  uint8_t sensor_status;
};

}  // namespace cool4_api_dataclass
#endif  // COOL4_API_DATACLASS_SENSOR_INFO_HPP_
