#include "cool4_api_dataclass/object_info.hpp"

#include <sstream>
#include <vector>
#include <string>


#include "cool4_api_dataclass/byte_stacker.hpp"
#include "cool4_api_dataclass/byte_extractor.hpp"

namespace cool4_api_dataclass {

boost::optional<ByteArray> ObjectClass::ToByteArray() const {
  ByteArray out;
  StackValue<uint8_t>(this->id, out);
  StackValue<uint8_t>(this->confidence, out);
  StackValue<uint8_t>(this->subclass_type, out);
  StackValue<uint8_t>(this->subclass_confidence, out);
  return out;
}

boost::optional<ObjectClass> ObjectClass::FromByteArray(const ByteArray& byte_array, size_t& idx) {
  ObjectClass out;
  out.id = ExtractValue<uint8_t>(byte_array, idx);
  out.confidence = ExtractValue<uint8_t>(byte_array, idx);
  out.subclass_type = ExtractValue<uint8_t>(byte_array, idx);
  out.subclass_confidence = ExtractValue<uint8_t>(byte_array, idx);
  return out;
}

std::ostream& operator<<(std::ostream& os, const ObjectClass& o) {
  os
    << (unsigned)o.id << ","
    << (unsigned)o.confidence << ","
    << (unsigned)o.subclass_type << ","
    << (unsigned)o.subclass_confidence << ",";
  return os;
}

boost::optional<ByteArray> Location::ToByteArray() const {
  ByteArray out;

  StackValue<uint32_t>(this->geodetic_srid, out);
  StackValue<int32_t>(this->latitude, out);
  StackValue<int32_t>(this->longitude, out);
  StackValue<int32_t>(this->altitude, out);

  // v0.7.0
  StackValue<uint32_t>(this->projective_srid, out);
  StackValue<int32_t>(this->prc_x, out);
  StackValue<int32_t>(this->prc_y, out);
  StackValue<int32_t>(this->prc_z, out);

  StackValue<uint32_t>(this->crp_id, out);
  StackValue<int32_t>(this->dx_crp, out);
  StackValue<int32_t>(this->dy_crp, out);
  StackValue<int32_t>(this->dh_crp, out);

  StackValue<uint8_t>(this->lane_count, out);
  StackValue<int8_t>(this->lane_position, out);
  StackValue<uint8_t>(this->lane_lateral_position, out);
  StackValue<uint32_t>(this->crp_id_begin, out);
  StackValue<uint32_t>(this->crp_id_end, out);
  StackValue<uint16_t>(this->lane_vertical_position, out);

  StackValue<unsigned long long>(this->lane_id, out);
  StackValue<int32_t>(this->dx_lane, out);
  StackValue<int32_t>(this->dy_lane, out);
  StackValue<int32_t>(this->dh_lane, out);

  StackValue<uint16_t>(this->semi_axis_length_major, out);
  StackValue<uint16_t>(this->semi_axis_length_minor, out);
  StackValue<uint16_t>(this->orientation, out);
  StackValue<uint16_t>(this->altitude_accuracy, out);

  return out;
}

boost::optional<Location> Location::FromByteArray(const ByteArray& byte_array, size_t& idx) {
  Location out;

  out.geodetic_srid = ExtractValue<uint32_t>(byte_array, idx);
  out.latitude = ExtractValue<int32_t>(byte_array, idx);
  out.longitude = ExtractValue<int32_t>(byte_array, idx);
  out.altitude = ExtractValue<int32_t>(byte_array, idx);

  // v0.7.0
  out.projective_srid = ExtractValue<uint32_t>(byte_array, idx);
  out.prc_x = ExtractValue<int32_t>(byte_array, idx);
  out.prc_y = ExtractValue<int32_t>(byte_array, idx);
  out.prc_z = ExtractValue<int32_t>(byte_array, idx);

  out.crp_id = ExtractValue<uint32_t>(byte_array, idx);
  out.dx_crp = ExtractValue<int32_t>(byte_array, idx);
  out.dy_crp = ExtractValue<int32_t>(byte_array, idx);
  out.dh_crp = ExtractValue<int32_t>(byte_array, idx);

  out.lane_count = ExtractValue<uint8_t>(byte_array, idx);
  out.lane_position = ExtractValue<int8_t>(byte_array, idx);
  out.lane_lateral_position = ExtractValue<uint8_t>(byte_array, idx);
  out.crp_id_begin = ExtractValue<uint32_t>(byte_array, idx);
  out.crp_id_end = ExtractValue<uint32_t>(byte_array, idx);
  out.lane_vertical_position = ExtractValue<uint16_t>(byte_array, idx);

  out.lane_id = ExtractValue<unsigned long long>(byte_array, idx);
  out.dx_lane = ExtractValue<int32_t>(byte_array, idx);
  out.dy_lane = ExtractValue<int32_t>(byte_array, idx);
  out.dh_lane = ExtractValue<int32_t>(byte_array, idx);

  out.semi_axis_length_major = ExtractValue<uint16_t>(byte_array, idx);
  out.semi_axis_length_minor = ExtractValue<uint16_t>(byte_array, idx);
  out.orientation = ExtractValue<uint16_t>(byte_array, idx);
  out.altitude_accuracy = ExtractValue<uint16_t>(byte_array, idx);

  return out;
}

std::ostream& operator<<(std::ostream& os, const Location& o) {
  os << (unsigned)o.geodetic_srid << ","
     << o.latitude << ","
     << o.longitude << ","
     << o.altitude << ","
     << (unsigned)o.projective_srid << ","
     << o.prc_x << ","
     << o.prc_y << ","
     << o.prc_z << ","
     << (unsigned)o.crp_id << ","
     << o.dx_crp << ","
     << o.dy_crp << ","
     << o.dh_crp << ","
     << (unsigned)o.lane_count << ","
     << (unsigned)o.lane_position << ","
     << (unsigned)o.lane_lateral_position << ","
     << (unsigned)o.crp_id_begin << ","
     << (unsigned)o.crp_id_end << ","
     << o.lane_vertical_position << ","
     << (unsigned long)o.lane_id << ","
     << o.dx_lane << ","
     << o.dy_lane << ","
     << o.dh_lane << ","
     << (unsigned)o.semi_axis_length_major << ","
     << (unsigned)o.semi_axis_length_minor << ","
     << o.orientation << ","
     << (unsigned)o.altitude_accuracy << ",";
  return os;
}


boost::optional<ByteArray> ObjectInfo::ToByteArray() {
  ByteArray out;

  StackValue<unsigned long long>(this->id, out);
  StackValue<unsigned long long>(this->time, out);
  StackValue<uint8_t>(this->revision, out);

  for (const auto& e : this->object_classes) {
    const auto bytes_obj_class = e.ToByteArray().get();
    out.insert(out.end(), bytes_obj_class.begin(), bytes_obj_class.end());
  }
  StackValue<uint8_t>(this->existency, out);
  const auto bytes_obj_loc = this->object_location.ToByteArray().get();
  out.insert(out.end(), bytes_obj_loc.begin(), bytes_obj_loc.end());

  StackValue<uint8_t>(this->ref_point, out);

  const auto bytes_move_direction = this->move_direction.ToByteArray().get();
  out.insert(out.end(), bytes_move_direction.begin(), bytes_move_direction.end());

  const auto bytes_speed = this->speed.ToByteArray().get();
  out.insert(out.end(), bytes_speed.begin(), bytes_speed.end());

  const auto bytes_yaw_rate = this->yaw_rate.ToByteArray().get();
  out.insert(out.end(), bytes_yaw_rate.begin(), bytes_yaw_rate.end());

  const auto bytes_acceleration = this->acceleration.ToByteArray().get();
  out.insert(out.end(), bytes_acceleration.begin(), bytes_acceleration.end());

  const auto bytes_orientation = this->orientation.ToByteArray().get();
  out.insert(out.end(), bytes_orientation.begin(), bytes_orientation.end());

  const auto bytes_length = this->length.ToByteArray().get();
  out.insert(out.end(), bytes_length.begin(), bytes_length.end());
  const auto bytes_width = this->width.ToByteArray().get();
  out.insert(out.end(), bytes_width.begin(), bytes_width.end());
  const auto bytes_height = this->height.ToByteArray().get();
  out.insert(out.end(), bytes_height.begin(), bytes_height.end());

  StackValue<uint16_t>(this->static_status, out);

  StackValue<uint8_t>(this->shift_position, out);
  StackValue<int8_t>(this->steering_angle_front, out);
  StackValue<int8_t>(this->steering_angle_rear, out);
  StackValue<uint8_t>(this->brake_state, out);
  StackValue<uint8_t>(this->auxiliary_brake_state, out);
  StackValue<uint8_t>(this->throttle_position, out);
  StackValue<uint8_t>(this->exterior_lights, out);

  StackValue<uint8_t>(this->acc_state, out);
  StackValue<uint8_t>(this->c_acc_state, out);
  StackValue<uint8_t>(this->pcs_state, out);
  StackValue<uint8_t>(this->abs_state, out);
  StackValue<uint8_t>(this->trc_state, out);
  StackValue<uint8_t>(this->esc_state, out);
  StackValue<uint8_t>(this->lka_state, out);
  StackValue<uint8_t>(this->ldw_state, out);

  StackValue<uint8_t>(this->vehicle_size_type, out);
  StackValue<uint8_t>(this->vehicle_role, out);
  StackValue<uint8_t>(this->vehicle_extended_info, out);
  StackValue<unsigned long long>(this->towing_vehicle, out);

  StackValue<uint8_t>(this->tracking_status, out);
  StackValue<uint16_t>(this->detection_count, out);
  StackValue<uint8_t>(this->lost_count, out);
  StackValue<uint16_t>(this->object_age, out);


  for (const auto& e : this->info_src_list) {
    StackValue<unsigned long long>(e, out);
  }

  return out;
}

boost::optional<ObjectInfo> ObjectInfo::FromByteArray(const ByteArray& byte_array, size_t& idx) {
  ObjectInfo out;
  out.id = ExtractValue<unsigned long long>(byte_array, idx);
  out.time = ExtractValue<unsigned long long>(byte_array, idx);
  out.revision = ExtractValue<uint8_t>(byte_array, idx);

  for (size_t i = 0; i < ObjectClassLength; ++i) {
    out.object_classes[i] = ObjectClass::FromByteArray(byte_array, idx).get();
  }

  out.existency = ExtractValue<uint8_t>(byte_array, idx);
  out.object_location = Location::FromByteArray(byte_array, idx).get();

  out.ref_point = ExtractValue<uint8_t>(byte_array, idx);

  out.move_direction = ValueWithAccuracy<uint16_t, uint16_t>::FromByteArray(
      byte_array, idx).get();
  out.speed = ValueWithAccuracy<int16_t, uint16_t>::FromByteArray(
      byte_array, idx).get();
  out.yaw_rate = ValueWithAccuracy<int16_t, uint16_t>::FromByteArray(
      byte_array, idx).get();
  out.acceleration = ValueWithAccuracy<int16_t, uint16_t>::FromByteArray(
      byte_array, idx).get();
  out.orientation = ValueWithAccuracy<uint16_t, uint16_t>::FromByteArray(
      byte_array, idx).get();
  out.length = ValueWithAccuracy<int16_t, uint16_t>::FromByteArray(
      byte_array, idx).get();
  out.width = ValueWithAccuracy<int16_t, uint16_t>::FromByteArray(
      byte_array, idx).get();
  out.height = ValueWithAccuracy<int16_t, uint16_t>::FromByteArray(
      byte_array, idx).get();

  out.static_status = ExtractValue<uint16_t>(byte_array, idx);

  out.shift_position = ExtractValue<uint8_t>(byte_array, idx);
  out.steering_angle_front = ExtractValue<int8_t>(byte_array, idx);
  out.steering_angle_rear = ExtractValue<int8_t>(byte_array, idx);
  out.brake_state = ExtractValue<uint8_t>(byte_array, idx);
  out.auxiliary_brake_state = ExtractValue<uint8_t>(byte_array, idx);
  out.throttle_position = ExtractValue<uint8_t>(byte_array, idx);
  out.exterior_lights = ExtractValue<uint8_t>(byte_array, idx);

  out.acc_state = ExtractValue<uint8_t>(byte_array, idx);
  out.c_acc_state = ExtractValue<uint8_t>(byte_array, idx);
  out.pcs_state = ExtractValue<uint8_t>(byte_array, idx);
  out.abs_state = ExtractValue<uint8_t>(byte_array, idx);
  out.trc_state = ExtractValue<uint8_t>(byte_array, idx);
  out.esc_state = ExtractValue<uint8_t>(byte_array, idx);
  out.lka_state = ExtractValue<uint8_t>(byte_array, idx);
  out.ldw_state = ExtractValue<uint8_t>(byte_array, idx);

  out.vehicle_size_type = ExtractValue<uint8_t>(byte_array, idx);
  out.vehicle_role = ExtractValue<uint8_t>(byte_array, idx);
  out.vehicle_extended_info = ExtractValue<uint8_t>(byte_array, idx);
  out.towing_vehicle = ExtractValue<unsigned long long>(byte_array, idx);

  out.tracking_status = ExtractValue<uint8_t>(byte_array, idx);
  out.detection_count = ExtractValue<uint16_t>(byte_array, idx);
  out.lost_count = ExtractValue<uint8_t>(byte_array, idx);
  out.object_age = ExtractValue<uint16_t>(byte_array, idx);

  for (size_t i = 0; i < ObjectInfo::InformationSourceListLength; ++i) {
    out.info_src_list[i] = ExtractValue<unsigned long long>(byte_array, idx);
  }

  return out;
}


std::ostream& operator<<(std::ostream& os, const ObjectInfo& o) {
  os
    << (unsigned long)o.id << ","
    << (unsigned long)o.time << ","
    << (unsigned)o.revision << ",";
  for (const auto& e : o.object_classes) os << e;
  os
    << (unsigned)o.existency << ","
    << o.object_location
    << (unsigned)o.ref_point << ","
    << o.move_direction
    << o.speed
    << o.yaw_rate
    << o.acceleration
    << o.orientation
    << o.length
    << o.width
    << o.height
    << (unsigned)o.static_status << ","
    << (unsigned)o.shift_position << ","
    << (unsigned)o.steering_angle_front << ","
    << (unsigned)o.steering_angle_rear << ","
    << (unsigned)o.brake_state << ","
    << (unsigned)o.auxiliary_brake_state << ","
    << (unsigned)o.throttle_position << ","
    << (unsigned)o.exterior_lights << ","
    << (unsigned)o.acc_state << ","
    << (unsigned)o.c_acc_state << ","
    << (unsigned)o.pcs_state << ","
    << (unsigned)o.abs_state << ","
    << (unsigned)o.trc_state << ","
    << (unsigned)o.esc_state << ","
    << (unsigned)o.lka_state << ","
    << (unsigned)o.ldw_state << ","
    << (unsigned)o.vehicle_size_type << ","
    << (unsigned)o.vehicle_role << ","
    << (unsigned)o.vehicle_extended_info << ","
    << (unsigned long)o.towing_vehicle << ","
    << (unsigned)o.tracking_status << ","
    << (unsigned)o.detection_count << ","
    << (unsigned)o.lost_count << ","
    << (unsigned)o.object_age << ",";
  for (const auto& e : o.info_src_list) {
    os << (unsigned long)e << ",";
  }

  return os;
}

}
