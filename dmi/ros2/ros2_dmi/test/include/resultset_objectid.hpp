#ifndef TEST_INCLUDE_RESULTSET_OBJECTID_HPP_
#define TEST_INCLUDE_RESULTSET_OBJECTID_HPP_

#include <chrono>
#include <functional>
#include <thread>
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

#include <is/ResultSet.h>

using QueryResult = std::unordered_map<std::string, std::string>;

namespace ros2_dmi {
namespace test_util {

  std::vector<std::string> CreateNameList_objectid() {
    return {"information_source_list"};
  }

  std::vector<std::string> CreateTypeList_objectid() {
    return {"vector(ulong)"};
  }

  ResultSetMetaData CreateQueryResultMetaData_objectid() {
    return {10, CreateNameList_objectid(), CreateTypeList_objectid()};
  }

  QueryResult CreateIsNulls_objectid() {
    QueryResult o;
    const auto name_list = CreateNameList_objectid();
    for (const auto name : name_list) {
      o[name] = "0";
    }
    return o;
  }

  QueryResult CreateTimestamps_objectid() {
    return CreateIsNulls_objectid();
  }

  // テストで変更したいやつだけ引数にする
  QueryResult CreateDatas_objectid(
    const std::string& information_source_list) {
    QueryResult o;
    const auto name_list = CreateNameList_objectid();
    for (const auto name : name_list) {
      if (name == "information_source_list") o[name] = information_source_list;
      else o[name] = "0";
    }
    return o;
  }

  /**
   * @brief ObjectIdメッセージに対応するResultSetデータの初期値
   * 
   */
  struct CandidateTestCase_objectid {
    CandidateTestCase_objectid() {
      // 初期値は有効とされている値を設定する
      map["information_source_list"] = {"[13579,24680]"};
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
    void Update_objectid(const std::string& key, const std::vector<std::string>& values) {
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
   * @brief ObjectIdメッセージテスト用のResultSetデータの作成
   * 
   * @param input 初期値とキーを記入したunordered_map
   * @return ObjectIdに対応する項目が含まれたResultSet 
   */
  ResultSet CreateResultSet_objectid(CandidateTestCase_objectid& input) {
    std::vector<QueryResult> datas_list;
    std::vector<QueryResult> timestamps_list;
    std::vector<QueryResult> is_nulls_list;
    
    for (size_t i = 0; i < input.length; ++i) {
      datas_list.push_back(CreateDatas_objectid(input.map["information_source_list"][i]));
      timestamps_list.push_back(CreateTimestamps_objectid());
      is_nulls_list.push_back(CreateIsNulls_objectid());
    }
    return ResultSet(datas_list, timestamps_list, is_nulls_list, CreateQueryResultMetaData_objectid());
  }

}  // namespace test_util
}  // namespace ros2_dmi

#endif  // TEST_INCLUDE_RESULTSET_OBJECTID_HPP_
