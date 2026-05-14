#ifndef ROS2_DMI_CONVERTER_UTIL_HPP_
#define ROS2_DMI_CONVERTER_UTIL_HPP_
#include <boost/array.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <dm_object_info_msgs/msg/location.hpp>
#include <dm_object_info_msgs/msg/object_id.hpp>
#include <is/ResultSet.h>
#include <is/DmUtil.h>
#include <is/Tuple.h>
#include <is/TupleSet.h>

namespace ros2_dmi {

using dm_object_info_msgs::msg::Location;
using dm_object_info_msgs::msg::ObjectId;


template<typename T>
inline T ExtractTupleValue(const uint32_t idx, IS::Tuple& t) {
  // anyはis/Tuple.hで規定しているものを使う
  any any_val;
  t.getValueByIdx(idx, any_val);
  return any_cast<T>(any_val);
}

template<class T>
std::vector<T> ExtractTupleValues(const uint32_t idx, IS::Tuple& tpl)
{
  std::vector<T> values;
  try {
    values = ExtractTupleValue<std::vector<T>>(idx, tpl);
  } catch (...) {
    const std::vector<any> tmp = ExtractTupleValue<std::vector<any>>(idx, tpl);
    for (const auto& e : tmp)
    {
      values.push_back(any_cast<T>(e));
    }
  }

  return values;
}


/**
 * @brief A class to convert small object between ROS and DM
 *
 * The difference of the Converter class is whether rs.next() call or not.
 *
 * This class trust a cursor of rs is valid.
 *
 * @tparam T ROS message type
 */
template<class T>
struct Getter {
};

/**
 * @brief A converter for Location
 */
template<>
struct Getter<Location> {
  /**
   * @brief Convert DM result set to Location
   *
   * @param rs DM event object
   * @param pref Prefix to access data
   *
   * @return Location
   */
  static Location ResultSetToRos(ResultSet& rs, const std::string pref=std::string("")) {
    Location msg;
    msg.geodetic_srid.value = rs.getInt(pref + "geodetic_srid");
    msg.latitude.value = rs.getInt(pref + "latitude");
    msg.longitude.value = rs.getInt(pref + "longitude");
    msg.altitude.value = rs.getInt(pref + "altitude");
    msg.projective_srid.value = rs.getInt(pref + "projective_srid");
    msg.prc_x = rs.getInt(pref + "prc_x");
    msg.prc_y = rs.getInt(pref + "prc_y");
    msg.prc_z.value = rs.getInt(pref + "prc_z");
    msg.crp_id.value = rs.getInt(pref + "crp_id");
    msg.dx_crp.value = rs.getInt(pref + "dx_crp");
    msg.dy_crp.value = rs.getInt(pref + "dy_crp");
    msg.dh_crp.value = rs.getInt(pref + "dh_crp");
    msg.lane_count.value = rs.getInt(pref + "lane_count");
    msg.lane_position.value = rs.getInt(pref + "lane_position");
    msg.lane_lateral_position.value = rs.getInt(pref + "lane_lateral_position");
    msg.crp_id_begin.value = rs.getInt(pref + "crp_id_begin");
    msg.crp_id_end.value = rs.getInt(pref + "crp_id_end");
    msg.lane_vertical_position.value = rs.getInt(pref + "lane_vertical_position");
    msg.lane_id.value = rs.getUnsignedLongLong(pref + "lane_id");
    msg.dx_lane.value = rs.getInt(pref + "dx_lane");
    msg.dy_lane.value = rs.getInt(pref + "dy_lane");
    msg.dh_lane.value = rs.getInt(pref + "dh_lane");
    msg.semi_axis_length_major.value = rs.getInt(pref + "semi_axis_length_major");
    msg.semi_axis_length_minor.value = rs.getInt(pref + "semi_axis_length_minor");
    msg.orientation.value = rs.getInt(pref + "ellipse_orientation");
    msg.altitude_accuracy.value = rs.getInt(pref + "altitude_accuracy");
    return msg;
  }

  /**
   * @brief Insert Location message properties into tuple
   *
   * @param msg Location
   * @param tuple (out) tuple
   * @param stamp A timestamp to be registered to each cell
   * @param idx A base index to start fill tuple
   */
  static void RosToTuple(
      const Location& msg, IS::Tuple& tuple, const long stamp, int& idx) {
    tuple.setValue(idx, static_cast<int>(msg.geodetic_srid.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.latitude.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.longitude.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.altitude.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.projective_srid.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.prc_x), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.prc_y), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.prc_z.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.crp_id.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.dx_crp.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.dy_crp.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.dh_crp.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.lane_count.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.lane_position.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.lane_lateral_position.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.crp_id_begin.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.crp_id_end.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.lane_vertical_position.value), stamp);
    tuple.setValue(++idx, static_cast<unsigned long long>(msg.lane_id.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.dx_lane.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.dy_lane.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.dh_lane.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.semi_axis_length_major.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.semi_axis_length_minor.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.orientation.value), stamp);
    tuple.setValue(++idx, static_cast<int>(msg.altitude_accuracy.value), stamp);
  }

//////////////////
  static Location TupleToRos(
      IS::Tuple& tuple, unsigned int& idx) {
    Location msg;

    msg.geodetic_srid.value = ExtractTupleValue<int>(++idx, tuple);
    msg.latitude.value = ExtractTupleValue<int>(++idx, tuple);
    msg.longitude.value = ExtractTupleValue<int>(++idx, tuple);
    msg.altitude.value = ExtractTupleValue<int>(++idx, tuple);
    msg.projective_srid.value = ExtractTupleValue<int>(++idx, tuple);
    msg.prc_x = ExtractTupleValue<int>(++idx, tuple);
    msg.prc_y = ExtractTupleValue<int>(++idx, tuple);
    msg.prc_z.value = ExtractTupleValue<int>(++idx, tuple);
    msg.crp_id.value = ExtractTupleValue<int>(++idx, tuple);
    msg.dx_crp.value = ExtractTupleValue<int>(++idx, tuple);
    msg.dy_crp.value = ExtractTupleValue<int>(++idx, tuple);
    msg.dh_crp.value = ExtractTupleValue<int>(++idx, tuple);
    msg.lane_count.value = ExtractTupleValue<int>(++idx, tuple);
    msg.lane_position.value = ExtractTupleValue<int>(++idx, tuple);
    msg.lane_lateral_position.value = ExtractTupleValue<int>(++idx, tuple);
    msg.crp_id_begin.value = ExtractTupleValue<int>(++idx, tuple);
    msg.crp_id_end.value = ExtractTupleValue<int>(++idx, tuple);
    msg.lane_vertical_position.value = ExtractTupleValue<int>(++idx, tuple);
    msg.lane_id.value = ExtractTupleValue<unsigned long long>(++idx, tuple);
    msg.dx_lane.value = ExtractTupleValue<int>(++idx, tuple);
    msg.dy_lane.value = ExtractTupleValue<int>(++idx, tuple);
    msg.dh_lane.value = ExtractTupleValue<int>(++idx, tuple);
    msg.semi_axis_length_major.value = ExtractTupleValue<int>(++idx, tuple);
    msg.semi_axis_length_minor.value = ExtractTupleValue<int>(++idx, tuple);
    msg.orientation.value = ExtractTupleValue<int>(++idx, tuple);
    msg.altitude_accuracy.value = ExtractTupleValue<int>(++idx, tuple);

    return msg;
  }

};
//////////////////


/**
 * @brief A converter for vector of ObjectId
 */
template<>
struct Getter<std::vector<ObjectId>> {
  /**
   * @brief Convert DM result set to vector of ObjectId
   *
   * @param rs DM event object
   *
   * @return vector(ObjectId)
   */
  static std::vector<ObjectId> ResultSetToRos(ResultSet& rs) {
    std::vector<ObjectId> msg;

    auto info_src_list = rs.getVectorUnsignedLongLong("information_source_list");
    for (const auto& e : info_src_list) {
      ObjectId id;
      id.value = e;
      msg.push_back(id);
    }
    return msg;
  }

  /**
   * @brief Insert vector of ObjectId message properties into tuple
   *
   * @param msg vector(ObjectId)
   * @param tuple (out) tuple
   * @param stamp A timestamp to be registered to each cell
   * @param idx A base index to start fill tuple
   */
  static void RosToTuple(
      const std::vector<ObjectId>& msg, IS::Tuple& tuple, const long stamp, int& idx) {
    std::vector<unsigned long long> info_src_list;
    for (const auto& e : msg) {
      info_src_list.push_back(e.value);
    }
    tuple.setValue(idx, info_src_list, stamp);
  }

  /**
   * @brief Convert DM tuples to vector of ObjectId
   *        extracted info src list from tuples.
   *
   * @param tpl DM tuples
   *
   * @return vector(ObjectId)
   */
  static std::vector<ObjectId> TupleToRos(IS::Tuple& tpl, unsigned int& idx) {
    std::vector<ObjectId> msg;
    any any_val;

    tpl.getValueByIdx(++idx, any_val);
    const std::vector<unsigned long long> id_vec = any_cast<std::vector<unsigned long long>>(any_val);

    for (const auto& id : id_vec) {
      ObjectId m;
      m.value = id;
      msg.push_back(m);
    }

    return msg;
  }
};

}
#endif
