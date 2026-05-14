#include "cool4_api_dataclass/signal_info.hpp"

#include <sstream>
#include <vector>
#include <string>

#include <glog/logging.h>

#include "cool4_api_dataclass/byte_stacker.hpp"
#include "cool4_api_dataclass/byte_extractor.hpp"

namespace cool4_api_dataclass {

boost::optional<ByteArray> LightInformation::ToByteArray() const {
  ByteArray out;
  StackValue<uint8_t>(this->main_light, out);
  StackValue<uint8_t>(this->arrow_light, out);
  StackValue<uint16_t>(this->min_time_to_change, out);
  StackValue<uint16_t>(this->max_time_to_change, out);
  return out;
}

boost::optional<LightInformation> LightInformation::FromByteArray(
    const ByteArray& byte_array, size_t& idx) {
  LightInformation out;
  out.main_light = ExtractValue<uint8_t>(byte_array, idx);
  out.arrow_light = ExtractValue<uint8_t>(byte_array, idx);
  out.min_time_to_change = ExtractValue<uint16_t>(byte_array, idx);
  out.max_time_to_change = ExtractValue<uint16_t>(byte_array, idx);
  return out;
}

std::ostream& operator<<(std::ostream& os, const LightInformation& o) {
  os
    << (unsigned)o.main_light << ","
    << (unsigned)o.arrow_light << ","
    << (unsigned)o.min_time_to_change << ","
    << (unsigned)o.max_time_to_change << ",";
  return os;
}

boost::optional<ByteArray> SignalInfo::ToByteArray() const {
  ByteArray out;
  StackValue<uint32_t>(this->crp_id, out);
  const size_t id_num = this->signal_id_list.size();
  VLOG(2) << "signal id num: " << id_num << std::endl;
  for (size_t i = 0; i < id_num; ++i) {
    StackValue<uint8_t>(this->signal_id_list[i], out);
  }
  const size_t remain_id_num = 8 - id_num;
  VLOG(2) << "remain signal id num: " << remain_id_num << std::endl;
  for (size_t i = 0; i < remain_id_num; ++i) {
    StackValue<uint8_t>(0x0, out);
  }

  StackValue<uint64_t>(this->time, out);
  StackValue<uint8_t>(this->state, out);
  StackValue<uint8_t>(this->specific_control_flags, out);
  StackValue<uint8_t>(this->event_count, out);
  StackValue<uint8_t>(this->count_down_stop_flag, out);
  for (const auto& e : this->light_information_list) {
    const auto bytes_light_info = e.ToByteArray().get();
    out.insert(out.end(), bytes_light_info.begin(), bytes_light_info.end());
  }
  return out;
}

boost::optional<SignalInfo> SignalInfo::FromByteArray(
    const ByteArray& byte_array, size_t& idx) {
  if (VLOG_IS_ON(2)) {
    for (const auto& e : byte_array) std::cerr << e;
    std::cerr << std::endl;
  }
  SignalInfo out;
  VLOG(2) << "before: " << out;
  out.crp_id = ExtractValue<uint32_t>(byte_array, idx);
  for (size_t i = 0; i < SignalIdNum; ++i) {
    out.signal_id_list[i] = ExtractValue<uint8_t>(byte_array, idx);
  }
  out.time = ExtractValue<uint64_t>(byte_array, idx);
  out.state = ExtractValue<uint8_t>(byte_array, idx);
  out.specific_control_flags = ExtractValue<uint8_t>(byte_array, idx);
  out.event_count = ExtractValue<uint8_t>(byte_array, idx);
  out.count_down_stop_flag = ExtractValue<uint8_t>(byte_array, idx);
  for (size_t i = 0; i < LightInformationNum; ++i) {
    out.light_information_list[i] = LightInformation::FromByteArray(byte_array, idx).get();
  }
  VLOG(2) << "after: " << out;
  return out;
}

std::ostream& operator<<(std::ostream& os, const SignalInfo& o) {
  os << (unsigned)o.crp_id << ",";
  for (const auto& e : o.signal_id_list) {
    os << (unsigned)e << ",";
  }
  os
    << (unsigned long)o.time << ","
    << (unsigned)o.state << ","
    << (unsigned)o.specific_control_flags << ","
    << (unsigned)o.event_count << ","
    << (unsigned)o.count_down_stop_flag << ",";
  for (const auto& e : o.light_information_list) {
    os << e;
  }
  return os;
}

}
