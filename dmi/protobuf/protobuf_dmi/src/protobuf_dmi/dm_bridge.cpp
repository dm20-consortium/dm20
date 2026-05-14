#include "protobuf_dmi/dm_bridge.hpp"


namespace protobuf_dmi
{

/*
bit 
0~15  協調型路側機の機器ID
16~31 協調型路側機の機器ID
32~47 センサー部IFから得た物標ID
48~55 センサー部の識別番号
56~63 0b010000000000
 */
unsigned long long CalcObjectId(
    const ObjectInformation& o,
    const protobuf_dmi::utils::Config& conf)
{
  unsigned long long object_id = static_cast<unsigned long long>(conf.rsu_id);
  object_id += static_cast<unsigned long long>(
      static_cast<uint16_t>(o.object_id())) << 32;
  object_id += static_cast<unsigned long long>(
      static_cast<uint8_t>(conf.sensor_id)) << 48;
  object_id |= 0x8000000000000000;
  return object_id;
}

void SetSensorLocationToTuple(
    uint32_t& idx, const long stamp,
    const SensorInformation& s, IS::Tuple& t)
{
  SetTupleValue<int>(idx, stamp, kGeodeticSystemCodeJgd2011, t);
  SetTupleValue<int>(++idx, stamp, s.latitude(), t);
  SetTupleValue<int>(++idx, stamp, s.longitude(), t);
  SetTupleValue<int>(++idx, stamp, s.altitude(), t);
  SetTupleValue<int>(++idx, stamp, 0, t);  // projective srid
  SetTupleValue<int>(
      ++idx, stamp, kDefaultDistanceValue, t);  // prc_x
  SetTupleValue<int>(
      ++idx, stamp, kDefaultDistanceValue, t);  // prc_y
  SetTupleValue<int>(
      ++idx, stamp, kDefaultAltitude, t);  // prc_z
  SetTupleValue<int>(++idx, stamp, 0, t);  // crp_id
  SetTupleValue<int>(
      ++idx, stamp, kDefaultDistanceValue, t);  // dx_crp
  SetTupleValue<int>(
      ++idx, stamp, kDefaultDistanceValue, t);  // dy_crp
  SetTupleValue<int>(
      ++idx, stamp, kDefaultAltitude, t);  // dz_crp
  SetTupleValue<int>(
      ++idx, stamp, kDefaultLaneCount, t);  // lane_count
  SetTupleValue<int>(
      ++idx, stamp, kDefaultLanePosition, t);  // lane_position
  SetTupleValue<int>(++idx, stamp, 101, t);  // lane_lateral_position
  SetTupleValue<int>(++idx, stamp, 0, t);  // crp_id_begin
  SetTupleValue<int>(++idx, stamp, 0, t);  // crp_id_end
  SetTupleValue<int>(++idx, stamp, 10001, t);  // lane_vertical_position
  SetTupleValue<unsigned long long>(++idx, stamp, 0, t);  // lane_id
  SetTupleValue<int>(++idx, stamp, kDefaultDistanceValue, t);  // dx_lane
  SetTupleValue<int>(++idx, stamp, kDefaultDistanceValue, t);  // dy_lane
  SetTupleValue<int>(++idx, stamp, kDefaultDistanceValue, t);  // dh_lane
  SetTupleValue<int>(
      ++idx, stamp, kDefaultSemiAxisLength, t);  // semi_axis_length_major
  SetTupleValue<int>(
      ++idx, stamp, kDefaultSemiAxisLength, t);  // semi_axis_length_minor
  SetTupleValue<int>(++idx, stamp, kDefaultAzimuth, t);  // ellipse_orientation
  SetTupleValue<int>(++idx, stamp, kDefaultAzimuthAccuracy, t);  // altitude_accuracy
}

void SetPositionToTuple(
    uint32_t& idx, const long stamp,
    const Position& p, IS::Tuple& t)
{
  SetTupleValue<int>(idx, stamp, kGeodeticSystemCodeJgd2011, t);
  SetTupleValue<int>(++idx, stamp, p.latitude(), t);
  SetTupleValue<int>(++idx, stamp, p.longitude(), t);
  SetTupleValue<int>(++idx, stamp, p.altitude(), t);
  SetTupleValue<int>(++idx, stamp, 0, t);  // projective srid
  SetTupleValue<int>(++idx, stamp, kDefaultDistanceValue, t);  // prc_x
  SetTupleValue<int>(++idx, stamp, kDefaultDistanceValue, t);  // prc_y
  SetTupleValue<int>(++idx, stamp, kDefaultDistanceValue, t);  // prc_z
  SetTupleValue<int>(++idx, stamp, 0, t);  // crp_id
  SetTupleValue<int>(++idx, stamp, kDefaultDistanceValue, t);  // dx_crp
  SetTupleValue<int>(++idx, stamp, kDefaultDistanceValue, t);  // dy_crp
  SetTupleValue<int>(++idx, stamp, kDefaultDistanceValue, t);  // dz_crp
  SetTupleValue<int>(++idx, stamp, kDefaultLaneCount, t);  // lane_count
  SetTupleValue<int>(++idx, stamp, kDefaultLanePosition, t);  // lane_position
  SetTupleValue<int>(++idx, stamp, kDefaultLaneLateralPosition, t);  // lane_lateral_position
  SetTupleValue<int>(++idx, stamp, 0, t);  // crp_id_begin
  SetTupleValue<int>(++idx, stamp, 0, t);  // crp_id_end
  SetTupleValue<int>(++idx, stamp, kDefaultDistanceRatio, t);  // lane_vertical_position
  SetTupleValue<unsigned long long>(++idx, stamp, 0, t);  // lane_id
  SetTupleValue<int>(++idx, stamp, kDefaultDistanceValue, t);  // dx_lane
  SetTupleValue<int>(++idx, stamp, kDefaultDistanceValue, t);  // dy_lane
  SetTupleValue<int>(++idx, stamp, kDefaultDistanceValue, t);  // dh_lane
  SetTupleValueOptional<int>(
      ++idx,
      stamp,
      std::bind(&Position::has_semi_major_axis_length, &p),
      std::bind(&Position::semi_major_axis_length, &p),
      kDefaultSemiAxisLength,
      t);
  SetTupleValueOptional<int>(
      ++idx,
      stamp,
      std::bind(&Position::has_semi_minor_axis_length, &p),
      std::bind(&Position::semi_minor_axis_length, &p),
      kDefaultSemiAxisLength,
      t);
  SetTupleValueOptional<int>(
      ++idx,
      stamp,
      std::bind(&Position::has_semi_major_orientation, &p),
      std::bind(&Position::semi_major_orientation, &p),
      kDefaultAzimuth,
      t);
  SetTupleValueOptional<int>(
      ++idx,
      stamp,
      std::bind(&Position::has_altitude_accuracy, &p),
      std::bind(&Position::altitude_accuracy, &p),
      kDefaultAltitudeAccuracy,
      t);
}

std::pair<int, int> GetObjectClassIdAndSubclassTypeId(
    const ObjectClass& oc)
{
  int class_type = static_cast<int>(CooL4ObjectClass::kUnknown);
  int subclass_type = 0;
  if (oc.has_vehicle_subclass_type())
  {
    class_type = static_cast<int>(CooL4ObjectClass::kVehicle);
    subclass_type = oc.vehicle_subclass_type();
  }
  else if (oc.has_train_subclass_type())
  {
    class_type = static_cast<int>(CooL4ObjectClass::kTrain);
    subclass_type = oc.train_subclass_type();
  }
  else if (oc.has_motorcycle_subclass_type())
  {
    class_type = static_cast<int>(CooL4ObjectClass::kMotorcycle);
    subclass_type = oc.motorcycle_subclass_type();
  }
  else if (oc.has_light_vehicle_subclass_type())
  {
    class_type = static_cast<int>(CooL4ObjectClass::kLightVehicle);
    subclass_type = oc.light_vehicle_subclass_type();
  }
  else if (oc.has_person_subclass_type())
  {
    class_type = static_cast<int>(CooL4ObjectClass::kPerson);
    subclass_type = oc.person_subclass_type();
  }
  else if (oc.has_animal_subclass_type())
  {
    class_type = static_cast<int>(CooL4ObjectClass::kAnimal);
    subclass_type = oc.animal_subclass_type();
  }
  else if (oc.has_nfo_subclass_type())
  {
    class_type = static_cast<int>(CooL4ObjectClass::kNotFixedObject);
    subclass_type = oc.nfo_subclass_type();
  }
  else if (oc.has_fo_subclass_type())
  {
    class_type = static_cast<int>(CooL4ObjectClass::kFixedObject);
    subclass_type = oc.fo_subclass_type();
  }

  return {class_type, subclass_type};
}


SensorInfoDmBridge::SensorInfoDmBridge(const utils::Config& conf)
  : conf_(conf),
    tuple_num_(71),
    stream_name_("sensor_info_0_8_1")
{
}

void SensorInfoDmBridge::ToTuple (
    const SensingMessage& in,
    const long stamp,
    std::vector<IS::Tuple>& out)
{
  const unsigned long long base_time = in.sensing_time();

  for (const auto& s : in.sensor_info())
  {
    IS::Tuple tuple(tuple_num_);
    tuple.setTableName(stream_name_);
    uint32_t idx = 0;

    SetTupleValue<unsigned long long>(idx, stamp, static_cast<unsigned long long>(conf_.rsu_id), tuple);
    SetTupleValue<int>(++idx, stamp, static_cast<int>(conf_.sensor_id), tuple);
    SetTupleValue<int>(++idx, stamp, s.type(), tuple);

    SetSensorLocationToTuple(++idx, stamp, s, tuple);

    SetTupleValue<long>(++idx, stamp, static_cast<long>(base_time), tuple);

    const auto dc_loop_number_pair = utils::CalcLoopNumber(
        SensorInfoDmBridge::DetectCapabilityMaxNum,
        s.detect_capabilities().size());

    for (int i = 0; i < dc_loop_number_pair.first; ++i)
    {
      DetectCapability dc = s.detect_capabilities(i);

      SetTupleValue<int>(++idx, stamp, dc.detectable_classes(), tuple);

      std::vector<int> dxs;
      std::vector<int> dys;
      for (const auto& p : dc.poly_points())
      {
        dxs.push_back(p.dx());
        dys.push_back(p.dy());
      }
      SetTupleValue<std::vector<int>>(++idx, stamp, dxs, tuple);
      SetTupleValue<std::vector<int>>(++idx, stamp, dys, tuple);

      SetTupleValue<int>(++idx, stamp, dc.confidence(), tuple);
      SetTupleValue<int>(++idx, stamp, dc.detectable_size(), tuple);
    }

    for (int i = 0; i < dc_loop_number_pair.second; ++i)
    {
      SetTupleValue<int>(++idx, stamp, 0, tuple);
      SetTupleValue<std::vector<int>>(++idx, stamp, std::vector<int>(), tuple);
      SetTupleValue<std::vector<int>>(++idx, stamp, std::vector<int>(), tuple);
      SetTupleValue<int>(++idx, stamp, 0, tuple);
      SetTupleValue<int>(++idx, stamp, 0, tuple);
    }

    SetTupleValue<int>(++idx, stamp, s.sensor_status(), tuple);

    out.push_back(tuple);
  }
}

ObjectInfoDmBridge::ObjectInfoDmBridge(const utils::Config& conf)
  : conf_(conf),
    tuple_num_(88),
    stream_name_("object_info_0_8_1")
{
}

void ObjectInfoDmBridge::ToTuple(
    const SensingMessage& in,
    const long stamp,
    std::vector<IS::Tuple>& out)
{
  const unsigned long long base_time = in.sensing_time();

  for (const auto& o : in.object_infos())
  {
    IS::Tuple tuple(tuple_num_);
    tuple.setTableName(stream_name_);
    uint32_t idx = 0;
    SetTupleValue<unsigned long long>(idx, stamp, static_cast<unsigned long long>(CalcObjectId(o, conf_)), tuple);
    const unsigned long long time = base_time + o.time_of_measurement();
    SetTupleValue<long>(++idx, stamp, time, tuple);

    SetTupleValue<int>(++idx, stamp, static_cast<int>(0), tuple);  // revision

    const auto loop_number_pair = utils::CalcLoopNumber(
        ObjectInfoDmBridge::ObjectClassMaxNum,
        o.object_classes().size());
    for (int i = 0; i < loop_number_pair.first; ++i)
    {
      ObjectClass oc = o.object_classes(i);
      const auto class_and_sub = GetObjectClassIdAndSubclassTypeId(oc);
      SetTupleValue<int>(++idx, stamp, class_and_sub.first, tuple);
      SetTupleValue<int>(++idx, stamp, oc.class_confidence(), tuple);
      SetTupleValue<int>(++idx, stamp, class_and_sub.second, tuple);
      SetTupleValue<int>(++idx, stamp, oc.subclass_confidence(), tuple);
    }
    for (int i = 0; i < loop_number_pair.second; ++i)
    {
      SetTupleValue<int>(++idx, stamp, 0, tuple);
      SetTupleValue<int>(++idx, stamp, 0, tuple);
      SetTupleValue<int>(++idx, stamp, 0, tuple);
      SetTupleValue<int>(++idx, stamp, 0, tuple);
    }
    SetTupleValue<int>(++idx, stamp, o.confidence(), tuple);

    SetPositionToTuple(++idx, stamp, o.position(), tuple);

    SetTupleValue<int>(++idx, stamp, o.ref_point(), tuple);

    SetTupleValueOptional<int>(
        ++idx, stamp,
        std::bind(&ObjectInformation::has_heading, &o),
        std::bind(&ObjectInformation::heading, &o),
        kDefaultAzimuth, tuple);
    SetTupleValueOptional<int>(
        ++idx, stamp,
        std::bind(&ObjectInformation::has_heading_accuracy, &o),
        std::bind(&ObjectInformation::heading_accuracy, &o),
        kDefaultAzimuthAccuracy, tuple);
    SetTupleValueOptional<int>(
        ++idx, stamp,
        std::bind(&ObjectInformation::has_speed, &o),
        std::bind(&ObjectInformation::speed, &o),
        16383, tuple);
    SetTupleValueOptional<int>(
        ++idx, stamp,
        std::bind(&ObjectInformation::has_speed_accuracy, &o),
        std::bind(&ObjectInformation::speed_accuracy, &o),
        16383, tuple);
    SetTupleValueOptional<int>(
        ++idx, stamp,
        std::bind(&ObjectInformation::has_yaw_rate, &o),
        std::bind(&ObjectInformation::yaw_rate, &o),
        32767, tuple);
    SetTupleValueOptional<int>(
        ++idx, stamp,
        std::bind(&ObjectInformation::has_yaw_rate_accuracy, &o),
        std::bind(&ObjectInformation::yaw_rate_accuracy, &o),
        32767, tuple);
    SetTupleValueOptional<int>(
        ++idx, stamp,
        std::bind(&ObjectInformation::has_acceleration, &o),
        std::bind(&ObjectInformation::acceleration, &o),
        2001, tuple);
    SetTupleValueOptional<int>(
        ++idx, stamp,
        std::bind(&ObjectInformation::has_acceleration_accuracy, &o),
        std::bind(&ObjectInformation::acceleration_accuracy, &o),
        1001, tuple);
    SetTupleValueOptional<int>(
        ++idx, stamp,
        std::bind(&ObjectInformation::has_orientation, &o),
        std::bind(&ObjectInformation::orientation, &o),
        kDefaultAzimuth, tuple);
    SetTupleValueOptional<int>(
        ++idx, stamp,
        std::bind(&ObjectInformation::has_orientation_accuracy, &o),
        std::bind(&ObjectInformation::orientation_accuracy, &o),
        kDefaultAzimuthAccuracy, tuple);

    SetTupleValueOptional<int>(
        ++idx, stamp,
        std::bind(&ObjectInformation::has_length, &o),
        std::bind(&ObjectInformation::length, &o),
        kDefaultObjectDimension, tuple);
    SetTupleValueOptional<int>(
        ++idx, stamp,
        std::bind(&ObjectInformation::has_length_accuracy, &o),
        std::bind(&ObjectInformation::length_accuracy, &o),
        kDefaultObjectDimension, tuple);
    SetTupleValueOptional<int>(
        ++idx, stamp,
        std::bind(&ObjectInformation::has_width, &o),
        std::bind(&ObjectInformation::width, &o),
        kDefaultObjectDimension, tuple);
    SetTupleValueOptional<int>(
        ++idx, stamp,
        std::bind(&ObjectInformation::has_width_accuracy, &o),
        std::bind(&ObjectInformation::width_accuracy, &o),
        kDefaultObjectDimension, tuple);
    SetTupleValueOptional<int>(
        ++idx, stamp,
        std::bind(&ObjectInformation::has_height, &o),
        std::bind(&ObjectInformation::height, &o),
        kDefaultObjectDimension, tuple);
    SetTupleValueOptional<int>(
        ++idx, stamp,
        std::bind(&ObjectInformation::has_height_accuracy, &o),
        std::bind(&ObjectInformation::height_accuracy, &o),
        kDefaultObjectDimension, tuple);

    SetTupleValue<int>(++idx, stamp, o.static_status(), tuple);

    SetTupleValue<int>(++idx, stamp, 7, tuple);  // shift_position
    SetTupleValue<int>(++idx, stamp, 91, tuple);  // steering_angle_front
    SetTupleValue<int>(++idx, stamp, 91, tuple);  // steering_angle_rear
    SetTupleValue<int>(++idx, stamp, 0, tuple);  // brake_state
    SetTupleValue<int>(++idx, stamp, 0, tuple);  // auxiliary_brake_state
    SetTupleValue<int>(++idx, stamp, 255, tuple);  // throttle_position
    SetTupleValue<int>(++idx, stamp, 0, tuple);  // exterior_lights
    SetTupleValue<int>(++idx, stamp, 0, tuple);  // adaptive_cruise_control_system
    SetTupleValue<int>(++idx, stamp, 0, tuple);  // c-accs
    SetTupleValue<int>(++idx, stamp, 0, tuple);  // pre_crash_safety_system
    SetTupleValue<int>(++idx, stamp, 0, tuple);  // abs
    SetTupleValue<int>(++idx, stamp, 0, tuple);  // tcs
    SetTupleValue<int>(++idx, stamp, 0, tuple);  // escs
    SetTupleValue<int>(++idx, stamp, 0, tuple);  // lkas
    SetTupleValue<int>(++idx, stamp, 0, tuple);  // ldws
    SetTupleValue<int>(++idx, stamp, 0, tuple);  // vehicle_size_type
    SetTupleValue<int>(++idx, stamp, 15, tuple);  // vehicle_role
    SetTupleValue<int>(++idx, stamp, 255, tuple);  // vehicle_extended_info
    SetTupleValue<unsigned long long>(++idx, stamp, 0, tuple);  // towing vehicle
    SetTupleValue<int>(++idx, stamp, o.tracking_status(), tuple);
    SetTupleValue<int>(++idx, stamp, o.detection_count(), tuple);
    SetTupleValue<int>(++idx, stamp, o.lost_count(), tuple);
    SetTupleValue<int>(++idx, stamp, o.object_age(), tuple);
    const std::vector<unsigned long long> info_src_list = {static_cast<unsigned long long>(conf_.rsu_id)};
    SetTupleValue<std::vector<unsigned long long>>(++idx, stamp, info_src_list, tuple);

    out.push_back(tuple);
  }
}

FreespaceInfoDmBridge::FreespaceInfoDmBridge(const utils::Config& conf)
  : conf_(conf),
    tuple_num_(92),
    stream_name_("freespace_info_0_8_1")
{
}

void FreespaceInfoDmBridge::ToTuple (
    const SensingMessage& in,
    const long stamp,
    std::vector<IS::Tuple>& out)
{
  const unsigned long long base_time = in.sensing_time();

  for (const auto& fs : in.freespace_infos())
  {
    IS::Tuple tuple(tuple_num_);
    tuple.setTableName(stream_name_);
    uint32_t idx = 0;

    // TODO Calc freespace ID
    SetTupleValue<unsigned long long>(idx, stamp, 0, tuple);  // id

    const unsigned long long time = base_time + fs.time_of_measurement();
    SetTupleValue<long>(++idx, stamp, time, tuple);
    SetTupleValue<int>(++idx, stamp, static_cast<int>(0), tuple);  // revision

    SetTupleValue<int>(++idx, stamp, kFreespaceDetectionMethod, tuple);
    SetTupleValue<int>(++idx, stamp, 0, tuple);  // detectable_classes
    SetPositionToTuple(++idx, stamp, fs.position(), tuple);

    std::vector<int> dxs;
    std::vector<int> dys;
    for (const auto& p : fs.poly_points())
    {
      dxs.push_back(p.dx());
      dys.push_back(p.dy());
    }
    SetTupleValue<std::vector<int>>(++idx, stamp, dxs, tuple);
    SetTupleValue<std::vector<int>>(++idx, stamp, dys, tuple);

    SetPositionToTuple(++idx, stamp, Position(), tuple);  // position begin
    SetPositionToTuple(++idx, stamp, Position(), tuple);  // position end

    SetTupleValue<int>(++idx, stamp, kDefaultDistanceValue, tuple);  // length
    SetTupleValue<int>(++idx, stamp, 0, tuple);  // length_accuracy
    SetTupleValue<unsigned long long>(++idx, stamp, 0, tuple);  // id_begin
    SetTupleValue<unsigned long long>(++idx, stamp, 0, tuple);  // id_end

    SetTupleValue<int>(++idx, stamp, fs.confidence(), tuple);
    SetTupleValue<int>(++idx, stamp, fs.detectable_size(), tuple);
    SetTupleValue<std::vector<unsigned long long>>(
        ++idx, stamp, {static_cast<unsigned long long>(conf_.rsu_id)}, tuple);

    out.push_back(tuple);
  }
}

SensingMessageDmBridge::SensingMessageDmBridge(const utils::Config& conf)
  : converters_(
      {
        std::make_shared<SensorInfoDmBridge>(
            SensorInfoDmBridge(conf)),
        std::make_shared<ObjectInfoDmBridge>(
            ObjectInfoDmBridge(conf)),
        std::make_shared<FreespaceInfoDmBridge>(
            FreespaceInfoDmBridge(conf))
      })
{
}

void SensingMessageDmBridge::ToTuple (
    const SensingMessage& in,
    const long stamp,
    std::vector<IS::Tuple>& out)
{
  for (auto e : this->converters_)
  {
    e->ToTuple(in, stamp, out);
  }
}

}  // namespace protobuf_dmi
