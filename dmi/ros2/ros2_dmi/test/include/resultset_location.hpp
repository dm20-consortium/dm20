#ifndef TEST_INCLUDE_RESULTSET_LOCATION_INFO_HPP_
#define TEST_INCLUDE_RESULTSET_LOCATION_INFO_HPP_

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

  std::vector<std::string> CreateNameList_location() {
    return {
      "test_geodetic_srid",
      "test_latitude",
      "test_longitude",
      "test_altitude",
      "test_projective_srid",
      "test_prc_x",
      "test_prc_y",
      "test_prc_z",
      "test_crp_id",
      "test_dx_crp",
      "test_dy_crp",
      "test_dh_crp",
      "test_lane_count",
      "test_lane_position",
      "test_lane_lateral_position",
      "test_crp_id_begin",
      "test_crp_id_end",
      "test_lane_vertical_position",
      "test_lane_id",
      "test_dx_lane",
      "test_dy_lane",
      "test_dh_lane",
      "test_semi_axis_length_major",
      "test_semi_axis_length_minor",
      "test_ellipse_orientation",
      "test_altitude_accuracy"};
  }

  std::vector<std::string> CreateTypeList_location() {
    return {
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
      "int"};
  }

  ResultSetMetaData CreateQueryResultMetaData_location() {
    return {10, CreateNameList_location(), CreateTypeList_location()};
  }

  QueryResult CreateIsNulls_location() {
    QueryResult o;
    const auto name_list = CreateNameList_location();
    for (const auto name : name_list) {
      o[name] = "0";
    }
    return o;
  }

  QueryResult CreateTimestamps_location() {
    return CreateIsNulls_location();
  }

  // テストで変更したいやつだけ引数にする
  QueryResult CreateDatas_location(
    const std::string& test_geodetic_srid,
    const std::string& test_latitude,
    const std::string& test_longitude,
    const std::string& test_altitude,
    const std::string& test_projective_srid,
    const std::string& test_prc_x,
    const std::string& test_prc_y,
    const std::string& test_prc_z,
    const std::string& test_crp_id,
    const std::string& test_dx_crp,
    const std::string& test_dy_crp,
    const std::string& test_dh_crp,
    const std::string& test_lane_count,
    const std::string& test_lane_position,
    const std::string& test_lane_lateral_position,
    const std::string& test_crp_id_begin,
    const std::string& test_crp_id_end,
    const std::string& test_lane_vertical_position,
    const std::string& test_lane_id,
    const std::string& test_dx_lane,
    const std::string& test_dy_lane,
    const std::string& test_dh_lane,
    const std::string& test_semi_axis_length_major,
    const std::string& test_semi_axis_length_minor,
    const std::string& test_ellipse_orientation,
    const std::string& test_altitude_accuracy) {
    QueryResult o;
    const auto name_list = CreateNameList_location();
    for (const auto name : name_list) {
      if (name == "test_geodetic_srid") o[name] = test_geodetic_srid;
      else if (name == "test_latitude") o[name] = test_latitude;
      else if (name == "test_longitude") o[name] = test_longitude;
      else if (name == "test_altitude") o[name] = test_altitude;
      else if (name == "test_projective_srid") o[name] = test_projective_srid;
      else if (name == "test_prc_x") o[name] = test_prc_x;
      else if (name == "test_prc_y") o[name] = test_prc_y;
      else if (name == "test_prc_z") o[name] = test_prc_z;
      else if (name == "test_crp_id") o[name] = test_crp_id;
      else if (name == "test_dx_crp") o[name] = test_dx_crp;
      else if (name == "test_dy_crp") o[name] = test_dy_crp;
      else if (name == "test_dh_crp") o[name] = test_dh_crp;
      else if (name == "test_lane_count") o[name] = test_lane_count;
      else if (name == "test_lane_position") o[name] = test_lane_position;
      else if (name == "test_lane_lateral_position") o[name] = test_lane_lateral_position;
      else if (name == "test_crp_id_begin") o[name] = test_crp_id_begin;
      else if (name == "test_crp_id_end") o[name] = test_crp_id_end;
      else if (name == "test_lane_vertical_position") o[name] = test_lane_vertical_position;
      else if (name == "test_lane_id") o[name] = test_lane_id;
      else if (name == "test_dx_lane") o[name] = test_dx_lane;
      else if (name == "test_dy_lane") o[name] = test_dy_lane;
      else if (name == "test_dh_lane") o[name] = test_dh_lane;
      else if (name == "test_semi_axis_length_major") o[name] = test_semi_axis_length_major;
      else if (name == "test_semi_axis_length_minor") o[name] = test_semi_axis_length_minor;
      else if (name == "test_ellipse_orientation") o[name] = test_ellipse_orientation;
      else if (name == "test_altitude_accuracy") o[name] = test_altitude_accuracy;
      else o[name] = "0";
    }
    return o;
  }

  /**
   * @brief Locationメッセージに対応するResultSetデータの初期値
   * 
   */
  struct CandidateTestCase_location {
    CandidateTestCase_location() {
      // 初期値は有効とされている値を設定する
      map["test_geodetic_srid"] = {"456"};
      map["test_latitude"] = {"789"};
      map["test_longitude"] = {"3322"};
      map["test_altitude"] = {"125"};
      map["test_projective_srid"] = {"1523"};
      map["test_prc_x"] = {"1633"};
      map["test_prc_y"] = {"1454"};
      map["test_prc_z"] = {"1963"};
      map["test_crp_id"] = {"523"};
      map["test_dx_crp"] = {"633"};
      map["test_dy_crp"] = {"454"};
      map["test_dh_crp"] = {"963"};
      map["test_lane_count"] = {"84"};
      map["test_lane_position"] = {"-27"};
      map["test_lane_lateral_position"] = {"159"};
      map["test_crp_id_begin"] = {"753"};
      map["test_crp_id_end"] = {"486"};
      map["test_lane_vertical_position"] = {"426"};
      map["test_lane_id"] = {"6314"};
      map["test_dx_lane"] = {"842"};
      map["test_dy_lane"] = {"862"};
      map["test_dh_lane"] = {"9852"};
      map["test_semi_axis_length_major"] = {"336"};
      map["test_semi_axis_length_minor"] = {"4567"};
      map["test_ellipse_orientation"] = {"7654"};
      map["test_altitude_accuracy"] = {"987"};
      length = 1;
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
    void Update_location(const std::string& key, const std::vector<std::string>& values) {
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
   * @brief Locationメッセージテスト用のResultSetデータの作成
   * 
   * @param input 初期値とキーを記入したunordered_map
   * @return Locationに対応する項目が含まれたResultSet 
   */
  ResultSet CreateResultSet_location(CandidateTestCase_location& input) {
    std::vector<QueryResult> datas_list;
    std::vector<QueryResult> timestamps_list;
    std::vector<QueryResult> is_nulls_list;
    
    for (size_t i = 0; i < input.length; ++i) {
      datas_list.push_back(CreateDatas_location(
        input.map["test_geodetic_srid"][i],
        input.map["test_latitude"][i],
        input.map["test_longitude"][i],
        input.map["test_altitude"][i],
        input.map["test_projective_srid"][i],
        input.map["test_prc_x"][i],
        input.map["test_prc_y"][i],
        input.map["test_prc_z"][i],
        input.map["test_crp_id"][i],
        input.map["test_dx_crp"][i],
        input.map["test_dy_crp"][i],
        input.map["test_dh_crp"][i],
        input.map["test_lane_count"][i],
        input.map["test_lane_position"][i],
        input.map["test_lane_lateral_position"][i],
        input.map["test_crp_id_begin"][i],
        input.map["test_crp_id_end"][i],
        input.map["test_lane_vertical_position"][i],
        input.map["test_lane_id"][i],
        input.map["test_dx_lane"][i],
        input.map["test_dy_lane"][i],
        input.map["test_dh_lane"][i],
        input.map["test_semi_axis_length_major"][i],
        input.map["test_semi_axis_length_minor"][i],
        input.map["test_ellipse_orientation"][i],
        input.map["test_altitude_accuracy"][i]));
      timestamps_list.push_back(CreateTimestamps_location());
      is_nulls_list.push_back(CreateIsNulls_location());
    }
    return ResultSet(datas_list, timestamps_list, is_nulls_list, CreateQueryResultMetaData_location());
  }

}  // namespace test_util
}  // namespace ros2_dmi

#endif  // TEST_INCLUDE_RESULTSET_LOCATION_INFO_HPP_
