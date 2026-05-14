#ifndef TEST_INCLUDE_RESULTSET_FREESPACE_INFO_HPP_
#define TEST_INCLUDE_RESULTSET_FREESPACE_INFO_HPP_

#include <chrono>
#include <functional>
#include <thread>
#include <map>
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

#include <is/ResultSet.h>

using QueryResult = unordered_map<string, string>;

namespace ros2_dmi {
namespace test_util {

  std::vector<std::string> CreateNameList_f() {
    return {
      "id",
      "time",
      "existency",
      "minimal_detectable_size",
      "position_begin_geodetic_srid",
      "position_begin_latitude",
      "position_begin_longitude",
      "position_begin_altitude",
      "position_begin_crp_id",
      "position_begin_dx_crp",
      "position_begin_dy_crp",
      "position_begin_dh_crp",
      "position_begin_lane_count",
      "position_begin_lane_position",
      "position_begin_lane_lateral_position",
      "position_begin_crp_id_begin",
      "position_begin_crp_id_end",
      "position_begin_lane_vertical_position",
      "position_begin_lane_id",
      "position_begin_dx_lane",
      "position_begin_dy_lane",
      "position_begin_dh_lane",
      "position_begin_semi_axis_length_major",
      "position_begin_semi_axis_length_minor",
      "position_begin_ellipse_orientation",
      "position_begin_altitude_accuracy",
      "position_end_geodetic_srid",
      "position_end_latitude",
      "position_end_longitude",
      "position_end_altitude",
      "position_end_crp_id",
      "position_end_dx_crp",
      "position_end_dy_crp",
      "position_end_dh_crp",
      "position_end_lane_count",
      "position_end_lane_position",
      "position_end_lane_lateral_position",
      "position_end_crp_id_begin",
      "position_end_crp_id_end",
      "position_end_lane_vertical_position",
      "position_end_lane_id",
      "position_end_dx_lane",
      "position_end_dy_lane",
      "position_end_dh_lane",
      "position_end_semi_axis_length_major",
      "position_end_semi_axis_length_minor",
      "position_end_ellipse_orientation",
      "position_end_altitude_accuracy",
      "length_value",
      "length_accuracy",
      "id_begin",
      "id_end",
      "information_source_list"};
  }

  std::vector<std::string> CreateTypeList_f() {
    return {
      "ulong",
      "long",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "ulong",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "ulong",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "int",
      "ulong",
      "ulong",
      "vector(ulong)"};
  }

  ResultSetMetaData CreateQueryResultMetaData_f() {
    return {10, CreateNameList_f(), CreateTypeList_f()};
  }

  QueryResult CreateIsNulls_f() {
    QueryResult o;
    const auto name_list = CreateNameList_f();
    for (const auto name : name_list) {
      o[name] = "0";
    }
    return o;
  }

  QueryResult CreateTimestamps_f() {
    return CreateIsNulls_f();
  }

  // テストで変更したいやつだけ引数にする
  QueryResult CreateDatas_f(
    const std::string& id,
    const std::string& time,
    const std::string& existency,
    const std::string& minimal_detectable_size,
    const std::string& position_begin_geodetic_srid,
    const std::string& position_begin_latitude,
    const std::string& position_begin_longitude,
    const std::string& position_begin_altitude,
    const std::string& position_begin_crp_id,
    const std::string& position_begin_dx_crp,
    const std::string& position_begin_dy_crp,
    const std::string& position_begin_dh_crp,
    const std::string& position_begin_lane_count,
    const std::string& position_begin_lane_position,
    const std::string& position_begin_lane_lateral_position,
    const std::string& position_begin_crp_id_begin,
    const std::string& position_begin_crp_id_end,
    const std::string& position_begin_lane_vertical_position,
    const std::string& position_begin_lane_id,
    const std::string& position_begin_dx_lane,
    const std::string& position_begin_dy_lane,
    const std::string& position_begin_dh_lane,
    const std::string& position_begin_semi_axis_length_major,
    const std::string& position_begin_semi_axis_length_minor,
    const std::string& position_begin_ellipse_orientation,
    const std::string& position_begin_altitude_accuracy,
    const std::string& position_end_geodetic_srid,
    const std::string& position_end_latitude,
    const std::string& position_end_longitude,
    const std::string& position_end_altitude,
    const std::string& position_end_crp_id,
    const std::string& position_end_dx_crp,
    const std::string& position_end_dy_crp,
    const std::string& position_end_dh_crp,
    const std::string& position_end_lane_count,
    const std::string& position_end_lane_position,
    const std::string& position_end_lane_lateral_position,
    const std::string& position_end_crp_id_begin,
    const std::string& position_end_crp_id_end,
    const std::string& position_end_lane_vertical_position,
    const std::string& position_end_lane_id,
    const std::string& position_end_dx_lane,
    const std::string& position_end_dy_lane,
    const std::string& position_end_dh_lane,
    const std::string& position_end_semi_axis_length_major,
    const std::string& position_end_semi_axis_length_minor,
    const std::string& position_end_ellipse_orientation,
    const std::string& position_end_altitude_accuracy,
    const std::string& length_value,
    const std::string& length_accuracy,
    const std::string& id_begin,
    const std::string& id_end,
    const std::string& information_source_list) {
    QueryResult o;
    const auto name_list = CreateNameList_f();
    for (const auto name : name_list) {
      if (name == "id") o[name] = id;
      else if (name == "time") o[name] = time;
      else if (name == "existency") o[name] = existency;
      else if (name == "minimal_detectable_size") o[name] = minimal_detectable_size;
      else if (name == "position_begin_geodetic_srid") o[name] = position_begin_geodetic_srid;
      else if (name == "position_begin_latitude") o[name] = position_begin_latitude;
      else if (name == "position_begin_longitude") o[name] = position_begin_longitude;
      else if (name == "position_begin_altitude") o[name] = position_begin_altitude;
      else if (name == "position_begin_crp_id") o[name] = position_begin_crp_id;
      else if (name == "position_begin_dx_crp") o[name] = position_begin_dx_crp;
      else if (name == "position_begin_dy_crp") o[name] = position_begin_dy_crp;
      else if (name == "position_begin_dh_crp") o[name] = position_begin_dh_crp;
      else if (name == "position_begin_lane_count") o[name] = position_begin_lane_count;
      else if (name == "position_begin_lane_position") o[name] = position_begin_lane_position;
      else if (name == "position_begin_lane_lateral_position") o[name] = position_begin_lane_lateral_position;
      else if (name == "position_begin_crp_id_begin") o[name] = position_begin_crp_id_begin;
      else if (name == "position_begin_crp_id_end") o[name] = position_begin_crp_id_end;
      else if (name == "position_begin_lane_vertical_position") o[name] = position_begin_lane_vertical_position;
      else if (name == "position_begin_lane_id") o[name] = position_begin_lane_id;
      else if (name == "position_begin_dx_lane") o[name] = position_begin_dx_lane;
      else if (name == "position_begin_dy_lane") o[name] = position_begin_dy_lane;
      else if (name == "position_begin_dh_lane") o[name] = position_begin_dh_lane;
      else if (name == "position_begin_semi_axis_length_major") o[name] = position_begin_semi_axis_length_major;
      else if (name == "position_begin_semi_axis_length_minor") o[name] = position_begin_semi_axis_length_minor;
      else if (name == "position_begin_ellipse_orientation") o[name] = position_begin_ellipse_orientation;
      else if (name == "position_begin_altitude_accuracy") o[name] = position_begin_altitude_accuracy;
      else if (name == "position_end_geodetic_srid") o[name] = position_end_geodetic_srid;
      else if (name == "position_end_latitude") o[name] = position_end_latitude;
      else if (name == "position_end_longitude") o[name] = position_end_longitude;
      else if (name == "position_end_altitude") o[name] = position_end_altitude;
      else if (name == "position_end_crp_id") o[name] = position_end_crp_id;
      else if (name == "position_end_dx_crp") o[name] = position_end_dx_crp;
      else if (name == "position_end_dy_crp") o[name] = position_end_dy_crp;
      else if (name == "position_end_dh_crp") o[name] = position_end_dh_crp;
      else if (name == "position_end_lane_count") o[name] = position_end_lane_count;
      else if (name == "position_end_lane_position") o[name] = position_end_lane_position;
      else if (name == "position_end_lane_lateral_position") o[name] = position_end_lane_lateral_position;
      else if (name == "position_end_crp_id_begin") o[name] = position_end_crp_id_begin;
      else if (name == "position_end_crp_id_end") o[name] = position_end_crp_id_end;
      else if (name == "position_end_lane_vertical_position") o[name] = position_end_lane_vertical_position;
      else if (name == "position_end_lane_id") o[name] = position_end_lane_id;
      else if (name == "position_end_dx_lane") o[name] = position_end_dx_lane;
      else if (name == "position_end_dy_lane") o[name] = position_end_dy_lane;
      else if (name == "position_end_dh_lane") o[name] = position_end_dh_lane;
      else if (name == "position_end_semi_axis_length_major") o[name] = position_end_semi_axis_length_major;
      else if (name == "position_end_semi_axis_length_minor") o[name] = position_end_semi_axis_length_minor;
      else if (name == "position_end_ellipse_orientation") o[name] = position_end_ellipse_orientation;
      else if (name == "position_end_altitude_accuracy") o[name] = position_end_altitude_accuracy;
      else if (name == "length_value") o[name] = length_value;
      else if (name == "length_accuracy") o[name] = length_accuracy;
      else if (name == "id_begin") o[name] = id_begin;
      else if (name == "id_end") o[name] = id_end;
      else if (name == "information_source_list") o[name] = information_source_list;
      else o[name] = "0";
    }
    return o;
  }

  /**
   * @brief FreespaceInfoArrayメッセージに対応するResultSetデータの初期値
   * 
   */
  struct CandidateTestCase_f {
    CandidateTestCase_f() {
      // 初期値は有効とされている値を設定する
      map["id"] = {"12341234", "12341235"};
      map["time"] = {"5000000", "5000001"};
      map["existency"] = {"27", "28"};
      map["minimal_detectable_size"] = {"123", "124"};
      map["position_begin_geodetic_srid"] = {"456", "457"};
      map["position_begin_latitude"] = {"789", "790"};
      map["position_begin_longitude"] = {"3322", "3323"};
      map["position_begin_altitude"] = {"125", "126"};
      map["position_begin_crp_id"] = {"523", "524"};
      map["position_begin_dx_crp"] = {"633", "634"};
      map["position_begin_dy_crp"] = {"454", "455"};
      map["position_begin_dh_crp"] = {"963", "964"};
      map["position_begin_lane_count"] = {"84", "85"};
      map["position_begin_lane_position"] = {"-27", "-28"};
      map["position_begin_lane_lateral_position"] = {"159", "160"};
      map["position_begin_crp_id_begin"] = {"753", "754"};
      map["position_begin_crp_id_end"] = {"486", "487"};
      map["position_begin_lane_vertical_position"] = {"426", "427"};
      map["position_begin_lane_id"] = {"6314", "6315"};
      map["position_begin_dx_lane"] = {"842", "843"};
      map["position_begin_dy_lane"] = {"862", "863"};
      map["position_begin_dh_lane"] = {"9852", "9853"};
      map["position_begin_semi_axis_length_major"] = {"336", "337"};
      map["position_begin_semi_axis_length_minor"] = {"4567", "4568"};
      map["position_begin_ellipse_orientation"] = {"7654", "7655"};
      map["position_begin_altitude_accuracy"] = {"987", "988"};
      map["position_end_geodetic_srid"] = {"654", "655"};
      map["position_end_latitude"] = {"321", "322"};
      map["position_end_longitude"] = {"12541", "12542"};
      map["position_end_altitude"] = {"6652", "6653"};
      map["position_end_crp_id"] = {"1100", "1101"};
      map["position_end_dx_crp"] = {"120", "121"};
      map["position_end_dy_crp"] = {"3698", "3699"};
      map["position_end_dh_crp"] = {"8563", "8564"};
      map["position_end_lane_count"] = {"10", "11"};
      map["position_end_lane_position"] = {"-72", "-73"};
      map["position_end_lane_lateral_position"] = {"73", "74"};
      map["position_end_crp_id_begin"] = {"6521", "6522"};
      map["position_end_crp_id_end"] = {"4589", "4590"};
      map["position_end_lane_vertical_position"] = {"3214", "3215"};
      map["position_end_lane_id"] = {"74523", "74524"};
      map["position_end_dx_lane"] = {"652", "653"};
      map["position_end_dy_lane"] = {"8521", "8522"};
      map["position_end_dh_lane"] = {"6332", "6333"};
      map["position_end_semi_axis_length_major"] = {"1225", "1226"};
      map["position_end_semi_axis_length_minor"] = {"63268", "63269"};
      map["position_end_ellipse_orientation"] = {"9633", "9634"};
      map["position_end_altitude_accuracy"] = {"1202", "1203"};
      map["length_value"] = {"1010", "1011"};
      map["length_accuracy"] = {"2022", "2023"};
      map["id_begin"] = {"652", "653"};
      map["id_end"] = {"98563", "98564"};
      map["information_source_list"] = {"[321654]", "[321655]"};
      length = 2;
    }

    /**
     * @brief 任意のキーの値を更新する
     *
     * このとき，キー毎の要素数に差がうまれるとResultSetとしては不完全になる
     * 要素数に差がうまれるような更新が行われた場合は，他のキーの要素数を増やして同じにする処理を行う
     * 増やす際は，末尾の値をpush_backする(0とかはCooL4においては不正値になることがあるため)
     *
     * @param key
     * @param values
     */
    void Update_f(const std::string& key, const std::vector<std::string>& values) {
      map[key] = values;

      length = 0;
      for (const auto& e : map) {
        if (length < e.second.size()) length = e.second.size();
      }

      for (auto& e : map) {
        const size_t dl = length - e.second.size();
        for (size_t i = 0; i < dl; ++i) {
          e.second.push_back(e.second.back());
        }
      }
    }

    std::map<std::string, std::vector<std::string>> map;
    size_t length;
  };

  /**
   * @brief FreespaceInfoArrayメッセージテスト用のResultSetデータの作成
   * 
   * @param input 初期値とキーを記入したunordered_map
   * @return FreespaceInfoArrayに対応する項目が含まれたResultSet 
   */
  ResultSet CreateResultSet_f(CandidateTestCase_f& input) {
    std::vector<QueryResult> datas_list;
    std::vector<QueryResult> timestamps_list;
    std::vector<QueryResult> is_nulls_list;

    for (size_t i = 0; i < input.length; ++i) {
      datas_list.push_back(CreateDatas_f(
        input.map["id"][i],
        input.map["time"][i],
        input.map["existency"][i],
        input.map["minimal_detectable_size"][i],
        input.map["position_begin_geodetic_srid"][i],
        input.map["position_begin_latitude"][i],
        input.map["position_begin_longitude"][i],
        input.map["position_begin_altitude"][i],
        input.map["position_begin_crp_id"][i],
        input.map["position_begin_dx_crp"][i],
        input.map["position_begin_dy_crp"][i],
        input.map["position_begin_dh_crp"][i],
        input.map["position_begin_lane_count"][i],
        input.map["position_begin_lane_position"][i],
        input.map["position_begin_lane_lateral_position"][i],
        input.map["position_begin_crp_id_begin"][i],
        input.map["position_begin_crp_id_end"][i],
        input.map["position_begin_lane_vertical_position"][i],
        input.map["position_begin_lane_id"][i],
        input.map["position_begin_dx_lane"][i],
        input.map["position_begin_dy_lane"][i],
        input.map["position_begin_dh_lane"][i],
        input.map["position_begin_semi_axis_length_major"][i],
        input.map["position_begin_semi_axis_length_minor"][i],
        input.map["position_begin_ellipse_orientation"][i],
        input.map["position_begin_altitude_accuracy"][i],
        input.map["position_end_geodetic_srid"][i],
        input.map["position_end_latitude"][i],
        input.map["position_end_longitude"][i],
        input.map["position_end_altitude"][i],
        input.map["position_end_crp_id"][i],
        input.map["position_end_dx_crp"][i],
        input.map["position_end_dy_crp"][i],
        input.map["position_end_dh_crp"][i],
        input.map["position_end_lane_count"][i],
        input.map["position_end_lane_position"][i],
        input.map["position_end_lane_lateral_position"][i],
        input.map["position_end_crp_id_begin"][i],
        input.map["position_end_crp_id_end"][i],
        input.map["position_end_lane_vertical_position"][i],
        input.map["position_end_lane_id"][i],
        input.map["position_end_dx_lane"][i],
        input.map["position_end_dy_lane"][i],
        input.map["position_end_dh_lane"][i],
        input.map["position_end_semi_axis_length_major"][i],
        input.map["position_end_semi_axis_length_minor"][i],
        input.map["position_end_ellipse_orientation"][i],
        input.map["position_end_altitude_accuracy"][i],
        input.map["length_value"][i],
        input.map["length_accuracy"][i],
        input.map["id_begin"][i],
        input.map["id_end"][i],
        input.map["information_source_list"][i]));
      timestamps_list.push_back(CreateTimestamps_f());
      is_nulls_list.push_back(CreateIsNulls_f());
    }
    return ResultSet(datas_list, timestamps_list, is_nulls_list, CreateQueryResultMetaData_f());
  }

}  // namespace test_util
}  // namespace ros2_dmi

#endif  // TEST_INCLUDE_RESULTSET_FREESPACE_INFO_HPP_
