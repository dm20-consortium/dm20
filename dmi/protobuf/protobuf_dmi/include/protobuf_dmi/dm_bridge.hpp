#ifndef PROTOBUF_DMI_DM_BRIDGE_HPP_
#define PROTOBUF_DMI_DM_BRIDGE_HPP_
#include <vector>
#include <memory>

#include <ccam_cool4_sensor_io/sensor_io.pb.h>
#include <is/Tuple.h>

#include "protobuf_dmi/utils.hpp"

#if __cplusplus < 201703L || defined(USE_EXP_ANY)
  #include <experimental/any>
  using std::experimental::any;
  using std::experimental::any_cast;
#else
  #include <any>
#endif           

namespace protobuf_dmi
{
constexpr uint32_t kGeodeticSystemCodeJgd2011 = 6668;                                                

constexpr int32_t kDefaultAltitude = 20001;
constexpr int32_t kDefaultAltitudeAccuracy = 20001;
constexpr int32_t kDefaultDistanceValue = -132768;
constexpr int32_t kDefaultLaneCount = 15;
constexpr int32_t kDefaultLanePosition = 17;
constexpr int32_t kDefaultLaneLateralPosition = 101;
constexpr int32_t kDefaultDistanceRatio = 10001;
constexpr uint32_t kDefaultSemiAxisLength = 4095;
constexpr uint32_t kDefaultAzimuth = 28800;
constexpr uint32_t kDefaultAzimuthAccuracy = 7201;
constexpr uint32_t kDefaultObjectDimension = 65535;

constexpr int kFreespaceDetectionMethod = 1;

template<typename T>
inline T ExtractTupleValue(const uint32_t idx, IS::Tuple& t) 
{
  // anyはis/Tuple.hで規定しているものを使う
  any any_val;
  t.getValueByIdx(idx, any_val);
  return any_cast<T>(any_val);
}

template<typename T>
inline void SetTupleValue(const uint32_t idx, const long stamp, const T& value, IS::Tuple& t)
{
  t.setValue(idx, static_cast<T>(value), stamp);
}


template<typename T>
inline void SetTupleValueOptional(
    const uint32_t idx,
    const long stamp,
    const std::function<bool()>& check_func,
    const std::function<T()>& get_func,
    const T& default_value,
    IS::Tuple& t)
{
  T value;
  if (check_func()) {
    value = get_func();
  }
  else {
    value = default_value;
  }
  t.setValue(idx, static_cast<T>(value), stamp);
}

unsigned long long CalcObjectId(
    const ObjectInformation& o,
    const protobuf_dmi::utils::Config& conf);

void SetSensorLocationToTuple(
    uint32_t& idx, const long stamp,
    const SensorInformation& s, IS::Tuple& t);

void SetPositionToTuple(
    uint32_t& idx, const long stamp,
    const Position& p, IS::Tuple& t);

enum class CooL4ObjectClass
{
kUnknown = 0,
kVehicle = 1,
kTrain = 2,
kMotorcycle = 3,
kLightVehicle = 4,
kPerson = 5,
kAnimal = 6,
kNotFixedObject = 7,
kFixedObject = 8
};


std::pair<int, int> GetObjectClassIdAndSubclassTypeId(
    const ObjectClass& oc);

template<class T>
class DmBridge
{
public:
  using SharedPtr = std::shared_ptr<DmBridge<T>>;
  using SharedPtrArray = std::vector<DmBridge<T>::SharedPtr>;

  virtual ~DmBridge() = default;
  virtual void ToTuple(
      const T& in,
      const long stamp,
      std::vector<IS::Tuple>& out) = 0;
};



class SensorInfoDmBridge final : public DmBridge<SensingMessage>
{
public:
  static constexpr int DetectCapabilityMaxNum = 8;

  SensorInfoDmBridge(const utils::Config& conf);
  ~SensorInfoDmBridge() override = default;

  void ToTuple(
      const SensingMessage& in,
      const long stamp,
      std::vector<IS::Tuple>& out) override;

private:
  const utils::Config conf_;
  const int tuple_num_;
  const std::string stream_name_;
};

class ObjectInfoDmBridge final : public DmBridge<SensingMessage>
{
public:
  static constexpr int ObjectClassMaxNum = 4;

  ObjectInfoDmBridge(const utils::Config& conf);
  ~ObjectInfoDmBridge() override = default;

  void ToTuple(
      const SensingMessage& in,
      const long stamp,
      std::vector<IS::Tuple>& out) override;

private:
  const utils::Config conf_;
  const int tuple_num_;
  const std::string stream_name_;
};

class FreespaceInfoDmBridge final : public DmBridge<SensingMessage>
{
public:
  FreespaceInfoDmBridge(const utils::Config& conf);
  ~FreespaceInfoDmBridge() override = default;

  void ToTuple(
      const SensingMessage& in,
      const long stamp,
      std::vector<IS::Tuple>& out) override;

private:
  const utils::Config conf_;
  const int tuple_num_;
  const std::string stream_name_;
};


class SensingMessageDmBridge final : public DmBridge<SensingMessage>
{
public:
  SensingMessageDmBridge(const utils::Config& conf);
  ~SensingMessageDmBridge() override = default;

  void ToTuple(
      const SensingMessage& in,
      const long stamp,
      std::vector<IS::Tuple>& out) override;

private:
  const DmBridge<SensingMessage>::SharedPtrArray converters_;
};

}  // namespace protobuf_dmi
#endif  // PROTOBUF_DMI_DM_BRIDGE_HPP_
