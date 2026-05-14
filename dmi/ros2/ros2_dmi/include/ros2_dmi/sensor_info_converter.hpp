#ifndef ROS2_DMI_SENSOR_INFO_CONVERTER_HPP_
#define ROS2_DMI_SENSOR_INFO_CONVERTER_HPP_
#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <dm_freespace_info_msgs/msg/freespace_info_array.hpp>
#include <dm_sensor_info_msgs/msg/sensor_info_array.hpp>
#include <is/ResultSet.h>

#include "ros2_dmi/converter_base.hpp"
#include "ros2_dmi/converter_util.hpp"

namespace {
/**
 * @brief Number of columns
 */
constexpr uint32_t kSensorInfoTupleNum = 71;
const char* const kSensorInfoTupleName = "sensor_info_0_8_1";

constexpr uint32_t kDetectableCapabilityNum = 8;
constexpr uint32_t kDetectableDomainPointNum = 16;
}

namespace ros2_dmi {

using namespace dm_object_info_msgs::msg;
using namespace dm_freespace_info_msgs::msg;
using namespace dm_sensor_info_msgs::msg;

using Ulong = unsigned long long;


template<>
struct Converter<SensorInfo> {
  static boost::optional<SensorInfo> ResultSetToRos(
      ResultSet& rs) {
    if (!rs.next()) return boost::none;

    SensorInfo msg;
    try {
      msg.unit_id.value = rs.getUnsignedLongLong("unit_id");
      msg.sensor_id = static_cast<uint8_t>(
          rs.getInt("sensor_id"));
      msg.sensor_type.value = static_cast<uint8_t>(
          rs.getInt("sensor_type"));
      msg.position = Getter<Location>::ResultSetToRos(rs);
      msg.time.value = static_cast<unsigned long long>(rs.getLong("time"));
      // 検知性能の変換は少々手がかかる
      const std::array<std::string, 5> detectable_capability_pref = {
        "detectable_classes",
        "detectable_domain_x_list",
        "detectable_domain_y_list",
        "confidence",
        "minimal_detectable_size"};
      for (uint32_t i = 0; i < kDetectableCapabilityNum; ++i) {
        std::vector<int> scalar_values;
        std::vector<std::vector<int>> values_list;
        for (uint32_t j = 0; j < detectable_capability_pref.size(); ++j) {
          std::stringstream ss;
          ss << detectable_capability_pref[j] << "_" << i;
          // 基本Intで値を得るが，一部Vector(Int)
          if (j == 1 || j == 2) {
            const auto v = rs.getVectorInt(ss.str());
            values_list.push_back(v);
          } else {
            scalar_values.push_back(rs.getInt(ss.str()));
          }

        }
        DetectableCapability dc;
        dc.detectable_classes.value = scalar_values[0];
        dc.confidence.value = scalar_values[1];
        dc.minimal_detectable_size.value = scalar_values[2];
        const bool valid_size = values_list[0].size() == values_list[1].size();
        if (valid_size) {
          for (size_t i = 0; i < values_list[0].size(); ++i) {
            OffsetPointXy p;
            p.dx.value = values_list[0][i];
            p.dy.value = values_list[1][i];
            dc.detectable_domain[i] = p;
          }
        }
        msg.detectable_capabilities[i] = dc;
      }
      msg.sensor_status.value = static_cast<uint8_t>(
          rs.getInt("sensor_status"));
    } catch (const std::exception& e) {
      std::cerr << "[ResultSet->SensorInfo]" << e.what() << std::endl
        << "We ignore this row" << std::endl;
      return boost::none;
    } catch (const CastException& e) {
      std::cerr << "[ResultSet->SensorInfo] Cast Exception" << std::endl
        << "We ignore this row" << std::endl;
      return boost::none;
    }

    return msg;
  }


  static std::vector<IS::Tuple> RosToTuple(
      const SensorInfo& msg,
      const long stamp) {
    int idx = -1;
    IS::Tuple tuple(kSensorInfoTupleNum);
    tuple.setTableName(kSensorInfoTupleName);

    tuple.setValue(
        ++idx,
        static_cast<Ulong>(msg.unit_id.value),
        stamp);
    tuple.setValue(++idx, static_cast<int>(msg.sensor_id), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.sensor_type.value), stamp);

    Getter<Location>::RosToTuple(msg.position, tuple, stamp, ++idx);

    tuple.setValue(++idx, static_cast<long>(msg.time.value), stamp);

    for (const auto& e : msg.detectable_capabilities) {
      tuple.setValue(++idx, static_cast<int>(e.detectable_classes.value), stamp);

      std::vector<int> xs(kDetectableDomainPointNum);
      std::vector<int> ys(kDetectableDomainPointNum);
      for (size_t i = 0; i < kDetectableDomainPointNum; ++i) {
        xs[i] = static_cast<int>(e.detectable_domain[i].dx.value);
        ys[i] = static_cast<int>(e.detectable_domain[i].dy.value);
      }
      tuple.setValue(++idx, xs, stamp);
      tuple.setValue(++idx, ys, stamp);

      tuple.setValue(++idx, static_cast<int>(e.confidence.value), stamp);
      tuple.setValue(++idx, static_cast<int>(e.minimal_detectable_size.value), stamp);
    }

    tuple.setValue(++idx, static_cast<int>(msg.sensor_status.value), stamp);

    std::vector<IS::Tuple> tuples;
    tuples.push_back(tuple);
    return tuples;
  }

  static boost::optional<SensorInfo> TupleSetToRos(
      IS::TupleSet& ts) {
    boost::optional<SensorInfo> msg = Converter<SensorInfo>::TupleToRos(
        ts.getTuple(0));
    return msg;
  }

  static boost::optional<SensorInfo> TupleToRos(
      IS::Tuple& tpl) {
    SensorInfo msg;
    uint32_t idx = 0;
    try {
      msg.unit_id.value = ExtractTupleValue<Ulong>(idx, tpl);
      msg.sensor_id = ExtractTupleValue<int>(++idx, tpl);
      msg.sensor_type.value = ExtractTupleValue<int>(++idx, tpl);
      msg.position = Getter<Location>::TupleToRos(tpl, idx);
      msg.time.value = ExtractTupleValue<long>(++idx, tpl);
      for (auto& m : msg.detectable_capabilities) {
        m.detectable_classes.value = ExtractTupleValue<int>(++idx, tpl);
        const auto xs = ExtractTupleValue<std::vector<int>>(++idx, tpl);
        const auto ys = ExtractTupleValue<std::vector<int>>(++idx, tpl);
        for (size_t i = 0; i < m.detectable_domain.size(); ++i) {
          auto p = OffsetPointXy();
          p.dx.value = xs[i];
          p.dy.value = ys[i];
          m.detectable_domain[i] = p;
        }
        m.confidence.value = ExtractTupleValue<int>(++idx, tpl);
        m.minimal_detectable_size.value = ExtractTupleValue<int>(++idx, tpl);
      }
      msg.sensor_status.value = ExtractTupleValue<int>(++idx, tpl);
    } catch (const std::exception& e) {
      std::cerr << "[Tuple->SensorInfo]" << e.what() << std::endl
        << "We ignore this row" << std::endl;
      return boost::none;
    } catch (const CastException& e) {
      std::cerr << "[Tuple->SensorInfo] Cast Exception" << std::endl
        << "We ignore this row" << std::endl;
      return boost::none;
    }

    return msg;
  }


};


template<>
struct Converter<SensorInfoArray> {
  static boost::optional<SensorInfoArray> ResultSetToRos(
      ResultSet& rs) {
    SensorInfoArray msg;
    while (true) {
      boost::optional<SensorInfo> m = Converter<SensorInfo>::ResultSetToRos(rs);
      if (m == boost::none) break;
      msg.array.push_back(m.get());
    }
    if (msg.array.empty()) return boost::none;
    return msg;
  }

  static std::vector<IS::Tuple> RosToTuple(
      const SensorInfoArray& msg,
      const long stamp) {
    std::vector<IS::Tuple> out;
    for (auto& e : msg.array) {
      std::vector<IS::Tuple> rows = Converter<SensorInfo>::RosToTuple(e, stamp);
      if (rows.empty()) break;
      out.insert(out.end(), rows.begin(), rows.end());
    }
    return out;
  }

  static boost::optional<SensorInfoArray> TupleSetToRos(
      IS::TupleSet& ts) {
    SensorInfoArray msg;
    for (int idx = 0; idx < ts.size(); idx++) {
      boost::optional<SensorInfo> m = Converter<SensorInfo>::TupleToRos(ts.getTuple(idx));
      if (m == boost::none) break;
      msg.array.push_back(m.get());
    }
    if (msg.array.empty()) return boost::none;
    return msg;
  }

};

}
#endif
