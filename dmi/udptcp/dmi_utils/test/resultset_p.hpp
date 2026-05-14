#include <chrono>
#include <functional>
#include <thread>
#include <map>
#include <iostream>

#include <is/ResultSet.h>

using QueryResult = std::unordered_map<std::string, std::string>;

namespace test_util {

  std::vector<std::string> CreateNameList_p() {
    return {
      "vehicle_id",
      "lane_id_list",
      "srid",
      "x_list",
      "y_list",
      "z_list",
      "velocity_list"};
  }

  std::vector<std::string> CreateTypeList_p() {
    return {
      "int",
      "vector(int)",
      "int",
      "vector(double)",
      "vector(double)",
      "vector(double)",
      "vector(double)"};
  }

  ResultSetMetaData CreateQueryResultMetaData_p() {
    return {10, CreateNameList_p(), CreateTypeList_p()};
  }

  QueryResult CreateIsNulls_p() {
    QueryResult o;
    const auto name_list = CreateNameList_p();
    for (const auto name : name_list) {
      o[name] = "0";
    }
    return o;
  }

  QueryResult CreateTimestamps_p() {
    return CreateIsNulls_p();
  }

  // テストで変更したいやつだけ引数にする
  QueryResult CreateDatas_p(
    const std::string& vehicle_id,
    const std::string& lane_id_list,
    const std::string& srid,
    const std::string& x_list,
    const std::string& y_list,
    const std::string& z_list,
    const std::string& velocity_list) {
    QueryResult o;
    const auto name_list = CreateNameList_p();
    for (const auto name : name_list) {
      if (name == "vehicle_id") o[name] = vehicle_id;
      else if (name == "lane_id_list") o[name] = lane_id_list;
      else if (name == "srid") o[name] = srid;
      else if (name == "x_list") o[name] = x_list;
      else if (name == "y_list") o[name] = y_list;
      else if (name == "z_list") o[name] = z_list;
      else if (name == "velocity_list") o[name] = velocity_list;
      else o[name] = "0";
    }
    return o;
  }

  struct CandidateTestCase_p {
    CandidateTestCase_p() {
      // 初期値は有効とされている値を設定する
      map["vehicle_id"] = {"1234"};
      map["lane_id_list"] = {"[2345]"};
      map["srid"] = {"3456"};
      map["x_list"] = {"[6789]"};
      map["y_list"] = {"[14725]"};
      map["z_list"] = {"[36985]"};
      map["velocity_list"] = {"[98765]"};
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
    void Update_p(const std::string& key, const std::vector<std::string>& values) {
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

  ResultSet CreateResultSet_p(CandidateTestCase_p& input) {
    std::vector<QueryResult> datas_list;
    std::vector<QueryResult> timestamps_list;
    std::vector<QueryResult> is_nulls_list;
    
    for (size_t i = 0; i < input.length; ++i) {
      datas_list.push_back(CreateDatas_p(
        input.map["vehicle_id"][i],
        input.map["lane_id_list"][i],
        input.map["srid"][i],
        input.map["x_list"][i],
        input.map["y_list"][i],
        input.map["z_list"][i],
        input.map["velocity_list"][i]));
      timestamps_list.push_back(CreateTimestamps_p());
      is_nulls_list.push_back(CreateIsNulls_p());
    }
    return ResultSet(datas_list, timestamps_list, is_nulls_list, CreateQueryResultMetaData_p());
  }

}
