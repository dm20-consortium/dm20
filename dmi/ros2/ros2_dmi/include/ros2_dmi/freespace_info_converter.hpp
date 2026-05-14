#ifndef ROS2_DMI_FREESPACE_INFO_CONVERTER_HPP_
#define ROS2_DMI_FREESPACE_INFO_CONVERTER_HPP_
#include <string>

#include <boost/array.hpp>
#include <boost/optional.hpp>

#include <dm_freespace_info_msgs/msg/freespace_info_array.hpp>

#include <is/ResultSet.h>
#include <is/DmUtil.h>

#include "ros2_dmi/converter_base.hpp"
#include "ros2_dmi/converter_util.hpp"

namespace {
/**
 * @brief Number of columns
 */
constexpr uint32_t kFreespaceTupleNum = 92;
const char* const kFreespaceInfoTupleName = "freespace_info_0_8_1";

}

namespace ros2_dmi {

using namespace dm_freespace_info_msgs::msg;


/**
 * @brief A converter for FreespaceInfo
 */
template<>
struct Converter<FreespaceInfo> {
  /**
   * @brief Convert DM result set to FreespaceInfo
   *
   * @param rs DM event object
   *
   * @return FreespaceInfo or boost::none
   */
  static boost::optional<FreespaceInfo> ResultSetToRos(ResultSet& rs) {
    if (!rs.next()) return boost::none;

    FreespaceInfo msg;
    try {
      msg.id.value = static_cast<uint64_t>(rs.getUnsignedLongLong("id"));
      msg.time.value = static_cast<uint64_t>(rs.getLong("time"));
      msg.revision.value = static_cast<uint8_t>(rs.getInt("revision"));
      msg.detection_method.value = rs.getInt("detection_method");
      msg.detectable_classes.value = rs.getInt("detectable_classes");
      msg.vertices.begin = Getter<Location>::ResultSetToRos(rs, "vertices_begin_");
      const std::vector<int> dx_list = rs.getVectorInt("vertices_dx_list");
      const std::vector<int> dy_list = rs.getVectorInt("vertices_dy_list");
      const bool valid_points = dx_list.size() == dy_list.size();
      if (valid_points) {
        for (size_t i = 0; i < dx_list.size(); ++i) {
          OffsetPointXy p;
          p.dx.value = dx_list[i];
          p.dy.value = dy_list[i];
          msg.vertices.array.array[i] = p;
        }
      }
      msg.position_begin = Getter<Location>::ResultSetToRos(rs, "position_begin_");
      msg.position_end = Getter<Location>::ResultSetToRos(rs, "position_end_");
      msg.length.value.value = rs.getInt("length_value");
      msg.length.accuracy.value = rs.getInt("length_accuracy");
      msg.id_begin.value = rs.getUnsignedLongLong("id_begin");
      msg.id_end.value = rs.getUnsignedLongLong("id_end");
      msg.existency.value = rs.getInt("existency");
      msg.minimal_detectable_size.value = rs.getInt("minimal_detectable_size");
      msg.information_source_list = Getter<std::vector<ObjectId>>::ResultSetToRos(rs);
    } catch (const std::exception& e) {
      std::cerr << "[ResultSet->FreespaceInfo]" << e.what() << std::endl
        << "We ignore this row" << std::endl;
      return boost::none;
    } catch (const CastException& e) {
      std::cerr << "[ResultSet->FreespaceInfo] Cast Exception" << std::endl
        << "We ignore this row" << std::endl;
      return boost::none;
    }
    return msg;
  }

  /**
   * @brief Convert DM result set to ObjectInfoArray
   *
   * @param rs DM event object
   *
   * @return ObjectInfoArray or boost::none
   */
  static boost::optional<FreespaceInfo> TupleSetToRos(IS::TupleSet& ts) {
    boost::optional<FreespaceInfo> msg = Converter<FreespaceInfo>::TupleToRos(ts.getTuple(0));
    return msg;
  }


  /**
   * @brief Convert DM result set to FreespaceInfo
   *
   * @param rs DM event object
   *
   * @return FreespaceInfo or boost::none
   */
  static boost::optional<FreespaceInfo> TupleToRos(IS::Tuple& tpl) {
    FreespaceInfo msg;
    try {
      unsigned int idx = 0;
      msg.id.value = ExtractTupleValue<unsigned long long>(idx, tpl);
      msg.time.value = ExtractTupleValue<long>(++idx, tpl);
      msg.revision.value = ExtractTupleValue<int>(++idx, tpl);

      msg.detection_method.value = ExtractTupleValue<int>(++idx, tpl);
      msg.detectable_classes.value = ExtractTupleValue<int>(++idx, tpl);

      msg.vertices.begin = Getter<Location>::TupleToRos(tpl, idx);
      const std::vector<int> dx_list = ExtractTupleValue<std::vector<int>>(++idx, tpl);
      const std::vector<int> dy_list = ExtractTupleValue<std::vector<int>>(++idx, tpl);
      const bool valid_size = dx_list.size() == dy_list.size();
      if (valid_size) {
        for (size_t i = 0; i < dx_list.size(); ++i) {
          OffsetPointXy p;
          p.dx.value = dx_list[i];
          p.dy.value = dy_list[i];
          msg.vertices.array.array[i] = p;
        }
      }
      msg.position_begin = Getter<Location>::TupleToRos(tpl, idx);
      msg.position_end = Getter<Location>::TupleToRos(tpl, idx);

      msg.length.value.value = ExtractTupleValue<int>(++idx, tpl);
      msg.length.accuracy.value = ExtractTupleValue<int>(++idx, tpl);

      msg.id_begin.value = ExtractTupleValue<unsigned long long>(++idx, tpl);
      msg.id_end.value = ExtractTupleValue<unsigned long long>(++idx, tpl);

      msg.existency.value = ExtractTupleValue<int>(++idx, tpl);
      msg.minimal_detectable_size.value = ExtractTupleValue<int>(++idx, tpl);

      msg.information_source_list = Getter<std::vector<ObjectId>>::TupleToRos(tpl, idx);
    } catch (const std::exception& e) {
      std::cerr << "[Tuple->FreespaceInfo]" << e.what() << std::endl
        << "We ignore this row" << std::endl;
      return boost::none;
    } catch (const CastException& e) {
      std::cerr << "[Tuple->FreespaceInfo] Cast Exception" << std::endl
        << "We ignore this row" << std::endl;
      return boost::none;
    }

    return msg;
  }

  /**
   * @brief Convert FreespaceInfo to DM tuple object
   *
   * @param msg Input FreespaceInfo
   * @param stamp A timestamp to be registered to each cell
   *
   * @return vector of DM tuple object
   */
  static std::vector<IS::Tuple> RosToTuple(const FreespaceInfo& msg, const long stamp) {
    int idx = -1;
    IS::Tuple tuple(kFreespaceTupleNum);
    tuple.setTableName(kFreespaceInfoTupleName);
    tuple.setValue(++idx, static_cast<unsigned long long>(msg.id.value), stamp);
    tuple.setValue(++idx, static_cast<long>(msg.time.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.revision.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.detection_method.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.detectable_classes.value), stamp);
    Getter<Location>::RosToTuple(msg.vertices.begin, tuple, stamp, ++idx);
    std::vector<int> dx_list;
    std::vector<int> dy_list;
    for (const auto& e : msg.vertices.array.array) {
      dx_list.push_back(e.dx.value);
      dy_list.push_back(e.dy.value);
    }
    tuple.setValue(++idx, dx_list, stamp);
    tuple.setValue(++idx, dy_list, stamp);
    Getter<Location>::RosToTuple(msg.position_begin, tuple, stamp, ++idx);
    Getter<Location>::RosToTuple(msg.position_end, tuple, stamp, ++idx);
    tuple.setValue(++idx, static_cast<int>(msg.length.value.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.length.accuracy.value), stamp);
    tuple.setValue(++idx, static_cast<unsigned long long>(msg.id_begin.value), stamp);
    tuple.setValue(++idx, static_cast<unsigned long long>(msg.id_end.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.existency.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.minimal_detectable_size.value), stamp);
    Getter<std::vector<ObjectId>>::RosToTuple(msg.information_source_list, tuple, stamp, ++idx);

    std::vector<IS::Tuple> tuples;
    tuples.push_back(tuple);

    return tuples;
  }
};

/**
 * @brief A converter for FreespaceInfoArray
 */
template<>
struct Converter<FreespaceInfoArray> {
  /**
   * @brief Convert DM result set to FreespaceInfoArray
   *
   * @param rs DM event object
   *
   * @return FreespaceInfoArray or boost::none
   */
  static boost::optional<FreespaceInfoArray> ResultSetToRos(ResultSet& rs) {
    FreespaceInfoArray msg;
    while (true) {
      boost::optional<FreespaceInfo> m = Converter<FreespaceInfo>::ResultSetToRos(rs);
      if (m == boost::none) break;
      msg.array.push_back(m.get());
    }
    if (msg.array.empty()) return boost::none;
    return msg;
  }

  /**
   * @brief Convert DM result set to FreespaceInfoArray
   *
   * @param rs DM event object
   *
   * @return FreespaceInfoArray or boost::none
   */
  static boost::optional<FreespaceInfoArray> TupleSetToRos(IS::TupleSet& ts) {
    FreespaceInfoArray msg;
    for (int idx = 0; idx < ts.size(); idx++) {
      boost::optional<FreespaceInfo> m = Converter<FreespaceInfo>::TupleToRos(ts.getTuple(idx));
      if (m == boost::none) break;
      msg.array.push_back(m.get());
    }
    if (msg.array.empty()) return boost::none;
    return msg;
  }

  /**
   * @brief Convert FreespaceInfoArray to DM tuple object
   *
   * @param msg Input FreespaceInfoArray
   * @param stamp A timestamp to be registered to each cell
   *
   * @return vector of DM tuple object
   */
  static std::vector<IS::Tuple> RosToTuple(const FreespaceInfoArray& msg, const long stamp) {
    std::vector<IS::Tuple> out;
    for (auto& e : msg.array) {
      std::vector<IS::Tuple> rows = Converter<FreespaceInfo>::RosToTuple(e, stamp);
      if (rows.empty()) break;
      out.insert(out.end(), rows.begin(), rows.end());
    }
    return out;
  }
};


}
#endif
