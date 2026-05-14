#include "cool4_api_dataclass/header.hpp"

#include "cool4_api_dataclass/byte_stacker.hpp"
#include "cool4_api_dataclass/byte_extractor.hpp"


namespace cool4_api_dataclass {

boost::optional<ByteArray> Header::ToByteArray() const {
  ByteArray out;
  StackValue<uint8_t>(this->version_major, out);
  StackValue<uint8_t>(this->version_minor, out);
  StackValue<uint8_t>(this->version_patch, out);
  StackValue<uint8_t>(this->seq, out);
  StackValue<uint8_t>(this->frame_id, out);
  StackValue<uint8_t>(this->frame_num, out);
  StackValue<uint8_t>(this->info_num, out);
  return out;
}

boost::optional<Header> Header::FromByteArray(const ByteArray& byte_array, size_t& idx) {
  Header out;

  if(byte_array.empty())
  {
    return boost::none;
  }
  
  out.version_major = ExtractValue<uint8_t>(byte_array, idx);
  out.version_minor = ExtractValue<uint8_t>(byte_array, idx);
  out.version_patch = ExtractValue<uint8_t>(byte_array, idx);
  out.seq = ExtractValue<uint8_t>(byte_array, idx);
  out.frame_id = ExtractValue<uint8_t>(byte_array, idx);
  out.frame_num = ExtractValue<uint8_t>(byte_array, idx);
  out.info_num = ExtractValue<uint8_t>(byte_array, idx);
  return out;
}

std::ostream& operator<<(std::ostream& os, const Header& o) {
  os
    << (unsigned)o.version_major << ","
    << (unsigned)o.version_minor << ","
    << (unsigned)o.version_patch << ","
    << (unsigned)o.seq << ","
    << (unsigned)o.frame_id << ","
    << (unsigned)o.frame_num << ","
    << (unsigned)o.info_num << ",";
  return os;
}

}
