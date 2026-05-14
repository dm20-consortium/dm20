#ifndef TEST_INCLUDE_RESULTSET_SIGNAL_INFO_HPP_
#define TEST_INCLUDE_RESULTSET_SIGNAL_INFO_HPP_

#include <chrono>
#include <functional>
#include <thread>
#include <map>
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

#include <is/ResultSet.h>

using QueryResult = std::unordered_map<std::string, std::string>;

namespace ros2_dmi {
namespace test_util {

  std::vector<std::string> CreateNameList_s() {
    return {
      "crp_id",
      "id",
      "time",
      "state",
      "specific_control_flags",
      "event_count",
      "count_down_stop_flag",
      "light_info_main_0",
      "light_info_arrow_0",
      "light_info_min_time_to_change_0",
      "light_info_max_time_to_change_0",
      "light_info_main_1",
      "light_info_arrow_1",
      "light_info_min_time_to_change_1",
      "light_info_max_time_to_change_1",
      "light_info_main_2",
      "light_info_arrow_2",
      "light_info_min_time_to_change_2",
      "light_info_max_time_to_change_2",
      "light_info_main_3",
      "light_info_arrow_3",
      "light_info_min_time_to_change_3",
      "light_info_max_time_to_change_3",
      "light_info_main_4",
      "light_info_arrow_4",
      "light_info_min_time_to_change_4",
      "light_info_max_time_to_change_4",
      "light_info_main_5",
      "light_info_arrow_5",
      "light_info_min_time_to_change_5",
      "light_info_max_time_to_change_5",
      "light_info_main_6",
      "light_info_arrow_6",
      "light_info_min_time_to_change_6",
      "light_info_max_time_to_change_6",
      "light_info_main_7",
      "light_info_arrow_7",
      "light_info_min_time_to_change_7",
      "light_info_max_time_to_change_7",
      "light_info_main_8",
      "light_info_arrow_8",
      "light_info_min_time_to_change_8",
      "light_info_max_time_to_change_8",
      "light_info_main_9",
      "light_info_arrow_9",
      "light_info_min_time_to_change_9",
      "light_info_max_time_to_change_9",
      "light_info_main_10",
      "light_info_arrow_10",
      "light_info_min_time_to_change_10",
      "light_info_max_time_to_change_10",
      "light_info_main_11",
      "light_info_arrow_11",
      "light_info_min_time_to_change_11",
      "light_info_max_time_to_change_11"};
  }

  std::vector<std::string> CreateTypeList_s() {
    return {
      "int",
      "vector(int)",
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
      "int"};
  }

  ResultSetMetaData CreateQueryResultMetaData_s() {
    return {10, CreateNameList_s(), CreateTypeList_s()};
  }

  QueryResult CreateIsNulls_s() {
    QueryResult o;
    const auto name_list = CreateNameList_s();
    for (const auto name : name_list) {
      o[name] = "0";
    }
    return o;
  }

  QueryResult CreateTimestamps_s() {
    return CreateIsNulls_s();
  }

  // テストで変更したいやつだけ引数にする
  QueryResult CreateDatas_s(
    const std::string& crp_id,
    const std::string& id,
    const std::string& time,
    const std::string& state,
    const std::string& specific_control_flags,
    const std::string& event_count,
    const std::string& count_down_stop_flag,
    const std::string& light_info_main_0,
    const std::string& light_info_arrow_0,
    const std::string& light_info_min_time_to_change_0,
    const std::string& light_info_max_time_to_change_0,
    const std::string& light_info_main_1,
    const std::string& light_info_arrow_1,
    const std::string& light_info_min_time_to_change_1,
    const std::string& light_info_max_time_to_change_1,
    const std::string& light_info_main_2,
    const std::string& light_info_arrow_2,
    const std::string& light_info_min_time_to_change_2,
    const std::string& light_info_max_time_to_change_2,
    const std::string& light_info_main_3,
    const std::string& light_info_arrow_3,
    const std::string& light_info_min_time_to_change_3,
    const std::string& light_info_max_time_to_change_3,
    const std::string& light_info_main_4,
    const std::string& light_info_arrow_4,
    const std::string& light_info_min_time_to_change_4,
    const std::string& light_info_max_time_to_change_4,
    const std::string& light_info_main_5,
    const std::string& light_info_arrow_5,
    const std::string& light_info_min_time_to_change_5,
    const std::string& light_info_max_time_to_change_5,
    const std::string& light_info_main_6,
    const std::string& light_info_arrow_6,
    const std::string& light_info_min_time_to_change_6,
    const std::string& light_info_max_time_to_change_6,
    const std::string& light_info_main_7,
    const std::string& light_info_arrow_7,
    const std::string& light_info_min_time_to_change_7,
    const std::string& light_info_max_time_to_change_7,
    const std::string& light_info_main_8,
    const std::string& light_info_arrow_8,
    const std::string& light_info_min_time_to_change_8,
    const std::string& light_info_max_time_to_change_8,
    const std::string& light_info_main_9,
    const std::string& light_info_arrow_9,
    const std::string& light_info_min_time_to_change_9,
    const std::string& light_info_max_time_to_change_9,
    const std::string& light_info_main_10,
    const std::string& light_info_arrow_10,
    const std::string& light_info_min_time_to_change_10,
    const std::string& light_info_max_time_to_change_10,
    const std::string& light_info_main_11,
    const std::string& light_info_arrow_11,
    const std::string& light_info_min_time_to_change_11,
    const std::string& light_info_max_time_to_change_11) {
    QueryResult o;
    const auto name_list = CreateNameList_s();
    for (const auto name : name_list) {
      if (name == "crp_id") o[name] = crp_id;
      else if (name == "id") o[name] = id;
      else if (name == "time") o[name] = time;
      else if (name == "state") o[name] = state;
      else if (name == "specific_control_flags") o[name] = specific_control_flags;
      else if (name == "event_count") o[name] = event_count;
      else if (name == "count_down_stop_flag") o[name] = count_down_stop_flag;
      else if (name == "light_info_main_0") o[name] = light_info_main_0;
      else if (name == "light_info_arrow_0") o[name] = light_info_arrow_0;
      else if (name == "light_info_min_time_to_change_0") o[name] = light_info_min_time_to_change_0;
      else if (name == "light_info_max_time_to_change_0") o[name] = light_info_max_time_to_change_0;
      else if (name == "light_info_main_1") o[name] = light_info_main_1;
      else if (name == "light_info_arrow_1") o[name] = light_info_arrow_1;
      else if (name == "light_info_min_time_to_change_1") o[name] = light_info_min_time_to_change_1;
      else if (name == "light_info_max_time_to_change_1") o[name] = light_info_max_time_to_change_1;
      else if (name == "light_info_main_2") o[name] = light_info_main_2;
      else if (name == "light_info_arrow_2") o[name] = light_info_arrow_2;
      else if (name == "light_info_min_time_to_change_2") o[name] = light_info_min_time_to_change_2;
      else if (name == "light_info_max_time_to_change_2") o[name] = light_info_max_time_to_change_2;
      else if (name == "light_info_main_3") o[name] = light_info_main_3;
      else if (name == "light_info_arrow_3") o[name] = light_info_arrow_3;
      else if (name == "light_info_min_time_to_change_3") o[name] = light_info_min_time_to_change_3;
      else if (name == "light_info_max_time_to_change_3") o[name] = light_info_max_time_to_change_3;
      else if (name == "light_info_main_4") o[name] = light_info_main_4;
      else if (name == "light_info_arrow_4") o[name] = light_info_arrow_4;
      else if (name == "light_info_min_time_to_change_4") o[name] = light_info_min_time_to_change_4;
      else if (name == "light_info_max_time_to_change_4") o[name] = light_info_max_time_to_change_4;
      else if (name == "light_info_main_5") o[name] = light_info_main_5;
      else if (name == "light_info_arrow_5") o[name] = light_info_arrow_5;
      else if (name == "light_info_min_time_to_change_5") o[name] = light_info_min_time_to_change_5;
      else if (name == "light_info_max_time_to_change_5") o[name] = light_info_max_time_to_change_5;
      else if (name == "light_info_main_6") o[name] = light_info_main_6;
      else if (name == "light_info_arrow_6") o[name] = light_info_arrow_6;
      else if (name == "light_info_min_time_to_change_6") o[name] = light_info_min_time_to_change_6;
      else if (name == "light_info_max_time_to_change_6") o[name] = light_info_max_time_to_change_6;
      else if (name == "light_info_main_7") o[name] = light_info_main_7;
      else if (name == "light_info_arrow_7") o[name] = light_info_arrow_7;
      else if (name == "light_info_min_time_to_change_7") o[name] = light_info_min_time_to_change_7;
      else if (name == "light_info_max_time_to_change_7") o[name] = light_info_max_time_to_change_7;
      else if (name == "light_info_main_8") o[name] = light_info_main_8;
      else if (name == "light_info_arrow_8") o[name] = light_info_arrow_8;
      else if (name == "light_info_min_time_to_change_8") o[name] = light_info_min_time_to_change_8;
      else if (name == "light_info_max_time_to_change_8") o[name] = light_info_max_time_to_change_8;
      else if (name == "light_info_main_9") o[name] = light_info_main_9;
      else if (name == "light_info_arrow_9") o[name] = light_info_arrow_9;
      else if (name == "light_info_min_time_to_change_9") o[name] = light_info_min_time_to_change_9;
      else if (name == "light_info_max_time_to_change_9") o[name] = light_info_max_time_to_change_9;
      else if (name == "light_info_main_10") o[name] = light_info_main_10;
      else if (name == "light_info_arrow_10") o[name] = light_info_arrow_10;
      else if (name == "light_info_min_time_to_change_10") o[name] = light_info_min_time_to_change_10;
      else if (name == "light_info_max_time_to_change_10") o[name] = light_info_max_time_to_change_10;
      else if (name == "light_info_main_11") o[name] = light_info_main_11;
      else if (name == "light_info_arrow_11") o[name] = light_info_arrow_11;
      else if (name == "light_info_min_time_to_change_11") o[name] = light_info_min_time_to_change_11;
      else if (name == "light_info_max_time_to_change_11") o[name] = light_info_max_time_to_change_11;
      else o[name] = "0";
    }
    return o;
  }

  /**
   * @brief SignalInfoArrayメッセージに対応するResultSetデータの初期値
   * 
   */
  struct CandidateTestCase_s {
    CandidateTestCase_s() {
      // 初期値は有効とされている値を設定する
      map["crp_id"] = {"123", "124"};
      map["id"] = {"[200,201]", "[202,203]"};
      map["time"] = {"789", "790"};
      map["state"] = {"110", "111"};
      map["specific_control_flags"] = {"120", "121"};
      map["event_count"] = {"130", "131"};
      map["count_down_stop_flag"] = {"219", "220"};
      map["light_info_main_0"] = {"142", "143"};
      map["light_info_arrow_0"] = {"65", "66"};
      map["light_info_min_time_to_change_0"] = {"741", "742"};
      map["light_info_max_time_to_change_0"] = {"852", "853"};
      map["light_info_main_1"] = {"195", "196"};
      map["light_info_arrow_1"] = {"147", "148"};
      map["light_info_min_time_to_change_1"] = {"258", "259"};
      map["light_info_max_time_to_change_1"] = {"369", "370"};
      map["light_info_main_2"] = {"159", "160"};
      map["light_info_arrow_2"] = {"241", "242"};
      map["light_info_min_time_to_change_2"] = {"963", "964"};
      map["light_info_max_time_to_change_2"] = {"852", "853"};
      map["light_info_main_3"] = {"229", "230"};
      map["light_info_arrow_3"] = {"189", "190"};
      map["light_info_min_time_to_change_3"] = {"4546", "4547"};
      map["light_info_max_time_to_change_3"] = {"7879", "7880"};
      map["light_info_main_4"] = {"25", "26"};
      map["light_info_arrow_4"] = {"133", "134"};
      map["light_info_min_time_to_change_4"] = {"9876", "9877"};
      map["light_info_max_time_to_change_4"] = {"4321", "4322"};
      map["light_info_main_5"] = {"56", "57"};
      map["light_info_arrow_5"] = {"153", "154"};
      map["light_info_min_time_to_change_5"] = {"486", "487"};
      map["light_info_max_time_to_change_5"] = {"9632", "9633"};
      map["light_info_main_6"] = {"73", "74"};
      map["light_info_arrow_6"] = {"245", "246"};
      map["light_info_min_time_to_change_6"] = {"5632", "5633"};
      map["light_info_max_time_to_change_6"] = {"8965", "8966"};
      map["light_info_main_7"] = {"174", "175"};
      map["light_info_arrow_7"] = {"169", "170"};
      map["light_info_min_time_to_change_7"] = {"1379", "1380"};
      map["light_info_max_time_to_change_7"] = {"3791", "3792"};
      map["light_info_main_8"] = {"233", "234"};
      map["light_info_arrow_8"] = {"177", "178"};
      map["light_info_min_time_to_change_8"] = {"4862", "4863"};
      map["light_info_max_time_to_change_8"] = {"8624", "8625"};
      map["light_info_main_9"] = {"104", "105"};
      map["light_info_arrow_9"] = {"182", "183"};
      map["light_info_min_time_to_change_9"] = {"6539", "6540"};
      map["light_info_max_time_to_change_9"] = {"8579", "8580"};
      map["light_info_main_10"] = {"27", "28"};
      map["light_info_arrow_10"] = {"3", "4"};
      map["light_info_min_time_to_change_10"] = {"6135", "6136"};
      map["light_info_max_time_to_change_10"] = {"4321", "4322"};
      map["light_info_main_11"] = {"154", "155"};
      map["light_info_arrow_11"] = {"203", "204"};
      map["light_info_min_time_to_change_11"] = {"12365", "12366"};
      map["light_info_max_time_to_change_11"] = {"18589", "18590"};
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
    void Update_s(const std::string& key, const std::vector<std::string>& values) {
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
   * @brief SignalInfoArrayメッセージテスト用のResultSetデータの作成
   * 
   * @param input 初期値とキーを記入したunordered_map
   * @return SignalInfoArrayに対応する項目が含まれたResultSet 
   */
  ResultSet CreateResultSet_s(CandidateTestCase_s& input) {
    std::vector<QueryResult> datas_list;
    std::vector<QueryResult> timestamps_list;
    std::vector<QueryResult> is_nulls_list;
    
    for (size_t i = 0; i < input.length; ++i) {
      datas_list.push_back(CreateDatas_s(
        input.map["crp_id"][i],
        input.map["id"][i],
        input.map["time"][i],
        input.map["state"][i],
        input.map["specific_control_flags"][i],
        input.map["event_count"][i],
        input.map["count_down_stop_flag"][i],
        input.map["light_info_main_0"][i],
        input.map["light_info_arrow_0"][i],
        input.map["light_info_min_time_to_change_0"][i],
        input.map["light_info_max_time_to_change_0"][i],
        input.map["light_info_main_1"][i],
        input.map["light_info_arrow_1"][i],
        input.map["light_info_min_time_to_change_1"][i],
        input.map["light_info_max_time_to_change_1"][i],
        input.map["light_info_main_2"][i],
        input.map["light_info_arrow_2"][i],
        input.map["light_info_min_time_to_change_2"][i],
        input.map["light_info_max_time_to_change_2"][i],
        input.map["light_info_main_3"][i],
        input.map["light_info_arrow_3"][i],
        input.map["light_info_min_time_to_change_3"][i],
        input.map["light_info_max_time_to_change_3"][i],
        input.map["light_info_main_4"][i],
        input.map["light_info_arrow_4"][i],
        input.map["light_info_min_time_to_change_4"][i],
        input.map["light_info_max_time_to_change_4"][i],
        input.map["light_info_main_5"][i],
        input.map["light_info_arrow_5"][i],
        input.map["light_info_min_time_to_change_5"][i],
        input.map["light_info_max_time_to_change_5"][i],
        input.map["light_info_main_6"][i],
        input.map["light_info_arrow_6"][i],
        input.map["light_info_min_time_to_change_6"][i],
        input.map["light_info_max_time_to_change_6"][i],
        input.map["light_info_main_7"][i],
        input.map["light_info_arrow_7"][i],
        input.map["light_info_min_time_to_change_7"][i],
        input.map["light_info_max_time_to_change_7"][i],
        input.map["light_info_main_8"][i],
        input.map["light_info_arrow_8"][i],
        input.map["light_info_min_time_to_change_8"][i],
        input.map["light_info_max_time_to_change_8"][i],
        input.map["light_info_main_9"][i],
        input.map["light_info_arrow_9"][i],
        input.map["light_info_min_time_to_change_9"][i],
        input.map["light_info_max_time_to_change_9"][i],
        input.map["light_info_main_10"][i],
        input.map["light_info_arrow_10"][i],
        input.map["light_info_min_time_to_change_10"][i],
        input.map["light_info_max_time_to_change_10"][i],
        input.map["light_info_main_11"][i],
        input.map["light_info_arrow_11"][i],
        input.map["light_info_min_time_to_change_11"][i],
        input.map["light_info_max_time_to_change_11"][i]));
      timestamps_list.push_back(CreateTimestamps_s());
      is_nulls_list.push_back(CreateIsNulls_s());
    }
    return ResultSet(datas_list, timestamps_list, is_nulls_list, CreateQueryResultMetaData_s());
  }

}  // namespace test_util
}  // namespace ros2_dmi

#endif  // TEST_INCLUDE_RESULTSET_SIGNAL_INFO_HPP_
