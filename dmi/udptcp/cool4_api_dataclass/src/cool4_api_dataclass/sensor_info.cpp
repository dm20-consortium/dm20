#include "cool4_api_dataclass/sensor_info.hpp"

namespace cool4_api_dataclass {

DetectableCapability::DetectableCapability()
  : detectable_class(0),
    vertices_num(0),
    vertices_x_list(std::vector<int>()),
    vertices_y_list(std::vector<int>()),
    confidence(0),
    minimal_detectable_size(0) {}

boost::optional<ByteArray> DetectableCapability::ToByteArray() const {
  ByteArray out;
  StackValue<uint8_t>(this->detectable_class, out);
  StackValue<uint8_t>(this->vertices_num, out);
  for (uint8_t i = 0; i < this->vertices_num; ++i) {
    StackValue<int>(this->vertices_x_list[i], out);
  }
  for (uint8_t i = 0; i < this->vertices_num; ++i) {
    StackValue<int>(this->vertices_y_list[i], out);
  }

  StackValue<uint8_t>(this->confidence, out);
  StackValue<uint16_t>(this->minimal_detectable_size, out);

  return out;
}

boost::optional<DetectableCapability> DetectableCapability::FromByteArray(const ByteArray& byte_array, size_t& idx) {
  DetectableCapability out;

  out.detectable_class = ExtractValue<uint8_t>(byte_array, idx);
  out.vertices_num = ExtractValue<uint8_t>(byte_array, idx);
  for (uint8_t i = 0; i < out.vertices_num; ++i) {
    out.vertices_x_list.push_back(ExtractValue<int>(byte_array, idx));
  }
  for (uint8_t i = 0; i < out.vertices_num; ++i) {
    out.vertices_y_list.push_back(ExtractValue<int>(byte_array, idx));
  }

  out.confidence = ExtractValue<uint8_t>(byte_array, idx);
  out.minimal_detectable_size = ExtractValue<uint16_t>(byte_array, idx);

  return out;
}

std::ostream& operator<<(std::ostream& os, const DetectableCapability& o) {
  os
    << (unsigned)o.detectable_class << ","
    << (unsigned)o.vertices_num << ",";
  for (uint8_t i = 0; i < o.vertices_num; ++i) {
    os << (int)o.vertices_x_list[i] << ",";
  }
  for (uint8_t i = 0; i < o.vertices_num; ++i) {
    os << (int)o.vertices_y_list[i] << ",";
  }
  os
    << (unsigned)o.confidence << ","
    << (unsigned)o.minimal_detectable_size << ",";

  return os;
}

SensorInfo::SensorInfo()
  : unit_id(0),
    sensor_id(0),
    sensor_type(0),
    position(Location()),
    time(0),
    detectable_capability_num(0),
    detectable_capabilities(std::vector<DetectableCapability>()),
    sensor_status(0) {}

boost::optional<ByteArray> SensorInfo::ToByteArray() const {
  ByteArray out;
  StackValue<unsigned long long>(this->unit_id, out);
  StackValue<uint8_t>(this->sensor_id, out);
  StackValue<uint8_t>(this->sensor_type, out);

  const auto bytes_obj_loc = this->position.ToByteArray().get();
  out.insert(out.end(), bytes_obj_loc.begin(), bytes_obj_loc.end());

  StackValue<unsigned long long>(this->time, out);

  StackValue<uint8_t>(this->detectable_capability_num, out);
  for (uint8_t i = 0; i < this->detectable_capability_num; ++i) {
    const auto bytes = this->detectable_capabilities[i].ToByteArray().get();
    out.insert(out.end(), bytes.begin(), bytes.end());
  }

  StackValue<uint8_t>(this->sensor_status, out);


  return out;
}

boost::optional<SensorInfo> SensorInfo::FromByteArray(const ByteArray& byte_array, size_t& idx) {
  SensorInfo out;
  out.unit_id = ExtractValue<unsigned long long>(byte_array, idx);
  out.sensor_id = ExtractValue<uint8_t>(byte_array, idx);
  out.sensor_type = ExtractValue<uint8_t>(byte_array, idx);

  out.position = Location::FromByteArray(byte_array, idx).get();

  out.time = ExtractValue<unsigned long long>(byte_array, idx);

  out.detectable_capability_num = ExtractValue<uint8_t>(byte_array, idx);
  for (uint8_t i = 0; i < out.detectable_capability_num; ++i) {
    out.detectable_capabilities.push_back(DetectableCapability::FromByteArray(byte_array, idx).get());
  }

  out.sensor_status = ExtractValue<uint8_t>(byte_array, idx);

  return out;
}

std::ostream& operator<<(std::ostream& os, const SensorInfo& o) {
  os
    << (unsigned)o.unit_id << ","
    << (unsigned)o.sensor_id << ","
    << (unsigned)o.sensor_type << ","
    << o.position
    << (unsigned long)o.time << ","
    << (unsigned)o.detectable_capability_num << ",";
  for (uint8_t i = 0; i < o.detectable_capability_num; ++i) {
    os << o.detectable_capabilities[i];
  }
  os
    << (unsigned)o.sensor_status << ",";

  return os;
}

}  // namespace cool4_api_dataclass
