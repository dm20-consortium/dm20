#include "cool4_api_dataclass/freespace_info.hpp"

#include <sstream>
#include <vector>
#include <string>

#include "cool4_api_dataclass/byte_stacker.hpp"
#include "cool4_api_dataclass/byte_extractor.hpp"
#include "cool4_api_dataclass/object_info.hpp"

namespace cool4_api_dataclass {


boost::optional<ByteArray> FreespaceInfo::ToByteArray() {
  ByteArray out;
  StackValue<unsigned long long>(this->id, out);
  StackValue<unsigned long long>(this->time, out);
  StackValue<uint8_t>(this->revision, out);

  StackValue<uint8_t>(this->detection_method, out);
  StackValue<uint8_t>(this->detectable_class, out);

  const auto bytes_vertices_begin = this->vertices_begin.ToByteArray().get();
  out.insert(out.end(), bytes_vertices_begin.begin(), bytes_vertices_begin.end());

  StackValue<uint8_t>(this->vertices_num, out);

  for (uint8_t i = 0; i < this->vertices_num; ++i)
    StackValue<int32_t>(this->vertices_x_list[i], out);

  for (uint8_t i = 0; i < this->vertices_num; ++i)
    StackValue<int32_t>(this->vertices_y_list[i], out);

  const auto bytes_pos_begin = this->position_begin.ToByteArray().get();
  out.insert(out.end(), bytes_pos_begin.begin(), bytes_pos_begin.end());
  const auto bytes_pos_end = this->position_end.ToByteArray().get();
  out.insert(out.end(), bytes_pos_end.begin(), bytes_pos_end.end());

  const auto bytes_length = this->length.ToByteArray().get();
  out.insert(out.end(), bytes_length.begin(), bytes_length.end());

  StackValue<unsigned long long>(this->id_begin, out);
  StackValue<unsigned long long>(this->id_end, out);

  StackValue<uint8_t>(this->existency, out);
  StackValue<uint16_t>(this->minimal_detectable_size, out);

  for (const auto& e : this->info_src_list) {
    StackValue<unsigned long long>(e, out);
  }

  return out;
}

boost::optional<FreespaceInfo> FreespaceInfo::FromByteArray(const ByteArray& byte_array, size_t& idx) {
  FreespaceInfo out;

  out.id = ExtractValue<unsigned long long>(byte_array, idx);
  out.time = ExtractValue<unsigned long long>(byte_array, idx);
  out.revision = ExtractValue<uint8_t>(byte_array, idx);

  out.detection_method = ExtractValue<uint8_t>(byte_array, idx);
  out.detectable_class = ExtractValue<uint8_t>(byte_array, idx);

  out.vertices_begin = Location::FromByteArray(byte_array, idx).get();
  out.vertices_num = ExtractValue<uint8_t>(byte_array, idx);
  for (size_t i = 0; i < out.vertices_num; ++i)
    out.vertices_x_list.push_back(ExtractValue<int32_t>(byte_array, idx));

  for (size_t i = 0; i < out.vertices_num; ++i)
    out.vertices_y_list.push_back(ExtractValue<int32_t>(byte_array, idx));

  out.position_begin = Location::FromByteArray(byte_array, idx).get();
  out.position_end = Location::FromByteArray(byte_array, idx).get();

  out.length = LengthType::FromByteArray(byte_array, idx).get();

  out.id_begin = ExtractValue<unsigned long long>(byte_array, idx);
  out.id_end = ExtractValue<unsigned long long>(byte_array, idx);

  out.existency = ExtractValue<uint8_t>(byte_array, idx);
  out.minimal_detectable_size = ExtractValue<uint16_t>(byte_array, idx);

  for (size_t i = 0; i < FreespaceInfo::InformationSourceListLength; ++i) {
    out.info_src_list[i] = ExtractValue<unsigned long long>(byte_array, idx);
  }

  return out;
}

std::ostream& operator<<(std::ostream& os, const FreespaceInfo& o) {
  os
    << (unsigned long)o.id << ","
    << (unsigned long)o.time << ","
    << (unsigned)o.revision << ","
    << (unsigned)o.detection_method << ","
    << (unsigned)o.detectable_class << ","
    << o.vertices_begin
    << (unsigned)o.vertices_num;
  for (const auto& e : o.vertices_x_list) os << e << ",";
  for (const auto& e : o.vertices_y_list) os << e << ",";
  os
    << o.position_begin
    << o.position_end
    << o.length
    << (unsigned long)o.id_begin << ","
    << (unsigned long)o.id_end << ","
    << (unsigned)o.existency << ","
    << (unsigned)o.minimal_detectable_size << ",";
  for (const auto& e : o.info_src_list) {
    os << (unsigned long)e << ",";
  }

  return os;
}

}
