#ifndef ROS2_DMI_CONVERTER_HPP_
#define ROS2_DMI_CONVERTER_HPP_
#include <string>

#include <boost/array.hpp>
#include <boost/optional.hpp>

#include <dm_signal_info_msgs/msg/signal_info_array.hpp>

#include <is/ResultSet.h>
#include <is/DmUtil.h>

#include "ros2_dmi/converter_base.hpp"
#include "ros2_dmi/converter_util.hpp"

namespace {
/**
 * @brief Number of columns
 */
constexpr uint32_t kSignalInfoTupleNum = 55;
const char* const kSignalInfoTupleName = "signal_info";

/**
 * @brief Max number of light information in SignalInfo
 */
constexpr uint32_t kSignalLightInfoMaxNum = 12;
constexpr uint32_t kSignalLightInfoPropertyNum = 4;
}

namespace ros2_dmi {

using namespace dm_signal_info_msgs::msg;


/**
 * @brief A converter for SignalInfo
 */
template<>
struct Converter<SignalInfo> {
  /**
   * @brief Convert DM result set to SignalInfo
   *
   * @param rs DM event object
   *
   * @return SignalInfo or boost::none
   */
  static boost::optional<SignalInfo> ResultSetToRos(ResultSet& rs) {
    if (!rs.next()) return boost::none;

    SignalInfo msg;
    try {
      msg.crp_id.value = static_cast<uint32_t>(rs.getInt("crp_id"));
      const auto dm_id_list = rs.getVectorInt("id");
      std::vector<SignalId> id_list(dm_id_list.size());
      for (size_t i = 0; i < id_list.size(); ++i) {
        id_list[i].value = static_cast<uint8_t>(dm_id_list[i]);
      }
      msg.signal_id_list = id_list;
      msg.time.value = static_cast<uint64_t>(rs.getLong("time"));
      msg.state.value = rs.getInt("state");
      msg.specific_control_flags.value = rs.getInt("specific_control_flags");
      msg.event_count.value = rs.getInt("event_count");
      msg.count_down_stop_flag.value = rs.getInt("count_down_stop_flag");

      std::vector<SignalLightInfo> signal_light_info_list;
      const std::vector<std::string> signal_light_info_pref = {
        "light_info_main",
        "light_info_arrow",
        "light_info_min_time_to_change",
        "light_info_max_time_to_change"};
      for (uint32_t i = 0; i < kSignalLightInfoMaxNum; ++i) {
        std::vector<int> data;
        for (const auto& pref : signal_light_info_pref) {
          std::stringstream ss;
          ss << pref << "_" << i;
          data.push_back(rs.getInt(ss.str()));
        }
        SignalLightInfo sli;
        // 不明値を含むデータの変換は行わない
        if (data[0] == MainLightIndication::UNKNOWN) continue;
        if (data[2] == MinTimeToChange::UNKNOWN) continue;
        if (data[3] == MaxTimeToChange::UNKNOWN) continue;
        sli.main_light.value = data[0];
        sli.arrow_light.value = data[1];
        sli.min_time_to_change.value = data[2];
        sli.max_time_to_change.value = data[3];
        signal_light_info_list.push_back(sli);
      }
      msg.signal_light_info_list = signal_light_info_list;
    } catch (const std::exception& e) {
      std::cerr << "[ResultSet->SignalInfo]" << e.what() << std::endl
        << "We ignore this row" << std::endl;
      return boost::none;
    } catch (const CastException& e) {
      std::cerr << "[ResultSet->SignalInfo] Cast Exception" << std::endl
        << "We ignore this row" << std::endl;
      return boost::none;
    }

    return msg;
  }

  /**
   * @brief Convert DM result set to SignalInfoArray
   *
   * @param rs DM event object
   *
   * @return SignalInfoArray or boost::none
   */
  static boost::optional<SignalInfo> TupleSetToRos(IS::TupleSet& ts) {
    boost::optional<SignalInfo> msg = Converter<SignalInfo>::TupleToRos(ts.getTuple(0));
    return msg;
  }

  /**
   * @brief Convert DM result set to SignalInfo
   *
   * @param rs DM event object
   *
   * @return SignalInfo or boost::none
   */
  static boost::optional<SignalInfo> TupleToRos(IS::Tuple& tpl) {
    SignalInfo msg;
    try {
      unsigned int idx = 0;
      msg.crp_id.value = ExtractTupleValue<int>(idx, tpl);

      const std::vector<int> dm_id_list = ExtractTupleValues<int>(++idx, tpl);
      std::vector<SignalId> id_list(dm_id_list.size());
      for (size_t i = 0; i < id_list.size(); ++i) {
        id_list[i].value = static_cast<uint8_t>(dm_id_list[i]);
      }
      msg.signal_id_list = id_list;

      msg.time.value = ExtractTupleValue<long>(++idx, tpl);
      msg.state.value = ExtractTupleValue<int>(++idx, tpl);
      msg.specific_control_flags.value = ExtractTupleValue<int>(++idx, tpl);
      msg.event_count.value = ExtractTupleValue<int>(++idx, tpl);
      msg.count_down_stop_flag.value = ExtractTupleValue<int>(++idx, tpl);

      std::vector<SignalLightInfo> signal_light_info_list;
      for (uint32_t i = 0; i < kSignalLightInfoMaxNum; ++i) {
        std::vector<int> data;
        for (size_t j = 0; j < kSignalLightInfoPropertyNum; ++j) {
          data.push_back(ExtractTupleValue<int>(++idx, tpl));
        }
        SignalLightInfo sli;
        // 不明値を含むデータの変換は行わない
        if (data[0] == MainLightIndication::UNKNOWN) continue;
        if (data[2] == MinTimeToChange::UNKNOWN) continue;
        if (data[3] == MaxTimeToChange::UNKNOWN) continue;
        sli.main_light.value = data[0];
        sli.arrow_light.value = data[1];
        sli.min_time_to_change.value = data[2];
        sli.max_time_to_change.value = data[3];
        signal_light_info_list.push_back(sli);
      }
      msg.signal_light_info_list = signal_light_info_list;
    } catch (const std::exception& e) {
      std::cerr << "[Tuple->SignalInfo]" << e.what() << std::endl
        << "We ignore this row" << std::endl;
      return boost::none;
    } catch (const CastException& e) {
      std::cerr << "[Tuple->SignalInfo] Cast Exception" << std::endl
        << "We ignore this row" << std::endl;
      return boost::none;
    }

    return msg;
  }

  /**
   * @brief Convert SignalInfo to DM tuple object
   *
   * @param msg Input SignalInfo
   * @param stamp A timestamp to be registered to each cell
   *
   * @return vector of DM tuple object
   */
  static std::vector<IS::Tuple> RosToTuple(const SignalInfo& msg, const long stamp) {
    IS::Tuple tuple(kSignalInfoTupleNum);
    tuple.setTableName(kSignalInfoTupleName);
    tuple.setValue(0, static_cast<int>(msg.crp_id.value), stamp);
    std::vector<int> id_list(msg.signal_id_list.size());
    for (size_t i = 0; i < id_list.size(); ++i) {
      id_list[i] = msg.signal_id_list[i].value;
    }
    tuple.setValue(1, id_list, stamp);
    tuple.setValue(2, static_cast<long>(msg.time.value), stamp);
    tuple.setValue(3, static_cast<int>(msg.state.value), stamp);
    tuple.setValue(4, static_cast<int>(msg.specific_control_flags.value), stamp);
    tuple.setValue(5, static_cast<int>(msg.event_count.value), stamp);
    tuple.setValue(6, static_cast<int>(msg.count_down_stop_flag.value), stamp);
    std::vector<SignalLightInfo> signal_light_info_list(msg.signal_light_info_list);
    if (signal_light_info_list.size() < kSignalLightInfoMaxNum) {
      signal_light_info_list.resize(kSignalLightInfoMaxNum);
    }
    for (uint32_t i = 0; i < kSignalLightInfoMaxNum; ++i) {
      tuple.setValue((i*4)+7+0, static_cast<int>(signal_light_info_list[i].main_light.value), stamp);
      tuple.setValue((i*4)+7+1, static_cast<int>(signal_light_info_list[i].arrow_light.value), stamp);
      tuple.setValue((i*4)+7+2, static_cast<int>(signal_light_info_list[i].min_time_to_change.value), stamp);
      tuple.setValue((i*4)+7+3, static_cast<int>(signal_light_info_list[i].max_time_to_change.value), stamp);
    }

    std::vector<IS::Tuple> tuples;
    tuples.push_back(tuple);

    return tuples;
  }
};

/**
 * @brief A converter for SignalInfoArray
 */
template<>
struct Converter<SignalInfoArray> {
  /**
   * @brief Convert DM result set to SignalInfoArray
   *
   * @param rs DM event object
   * @param debug_info (out) Unique info
   * @param enable_diag If it's true, validate required parameters before conversion
   * @param output If true, output diag result to stderr
   *
   * @return SignalInfoArray or boost::none
   */
  static boost::optional<SignalInfoArray> ResultSetToRos(ResultSet& rs) {
    SignalInfoArray msg;
    while (true) {
      boost::optional<SignalInfo> m = Converter<SignalInfo>::ResultSetToRos(rs);
      if (m == boost::none) break;
      msg.array.push_back(m.get());
    }
    if (msg.array.empty()) return boost::none;
    return msg;
  }

  /**
   * @brief Convert DM result set to SignalInfoArray
   *
   * @param rs DM event object
   *
   * @return SignalInfoArray or boost::none
   */
  static boost::optional<SignalInfoArray> TupleSetToRos(IS::TupleSet& ts) {
    SignalInfoArray msg;
    for (int idx = 0; idx < ts.size(); idx++) {
      boost::optional<SignalInfo> m = Converter<SignalInfo>::TupleToRos(ts.getTuple(idx));
      if (m == boost::none) break;
      msg.array.push_back(m.get());
    }
    if (msg.array.empty()) return boost::none;
    return msg;
  }

  /**
   * @brief Convert SignalInfoArray to DM tuple object
   *
   * @param msg Input SignalInfoArray
   * @param stamp A timestamp to be registered to each cell
   *
   * @return vector of DM tuple object
   */
  static std::vector<IS::Tuple> RosToTuple(const SignalInfoArray& msg, const long stamp) {
    std::vector<IS::Tuple> out;
    for (auto& e : msg.array) {
      std::vector<IS::Tuple> rows = Converter<SignalInfo>::RosToTuple(e, stamp);
      if (rows.empty()) break;
      out.insert(out.end(), rows.begin(), rows.end());
    }
    return out;
  }
};

}

#endif
