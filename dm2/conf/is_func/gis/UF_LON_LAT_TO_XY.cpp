#include <iostream>
#include <vector>
#include <string>
#include <pqxx/pqxx>
#include <algorithm>
#include <is/StringUtil.h>
#include <is/DmException.h>

using namespace std;
using namespace pqxx;

/**
* 緯度経度をXY平面直角座標に変換。対象は、日本のSRID（XY)のみ。
*/
extern "C" vector<vector<string>> UF_LON_LAT_TO_XY(vector<vector<string>> argList)
{
  //printf("UF_LON_LAT_TO_XY called!\n");
  vector<vector<string>> retList; // ISに返却する結果2次元配列(retList[row][col])

  // 地理座標のSRIDから引数2のSRIDに変換する
  string db_name = "dm2_db";
  string db_user = "dm2";
  string db_pass = "dm2";
  pqxx::connection con("dbname=" + db_name + " user=" + db_user + " password=" + db_pass);
  pqxx::work T(con);
  string query = "";
  query.append("SELECT ");
  query.append("  ST_AsEwkt( ");
  query.append("    ST_Transform( ST_GeomFromText('SRID=$1;POINT($2 $3)'), $4) ");
  query.append("  ); ");

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    IS::StringUtil stringUtil;
    vector<string> ret;
    string retString = "";
    int a1,a4;
    double a2,a3;
    try {
      a1 = stoi(argList.at(i).at(0)); // 引数1
      a2 = stod(argList.at(i).at(1)); // 引数2
      a3 = stod(argList.at(i).at(2)); // 引数3
      a4 = stoi(argList.at(i).at(3)); // 引数4
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }

    try {
      // クエリ作成
      string q = query;
      stringUtil.replace(q, "$1", to_string(a1));
      stringUtil.replace(q, "$2", to_string(a2));
      stringUtil.replace(q, "$3", to_string(a3));
      stringUtil.replace(q, "$4", to_string(a4));

      // クエリ実行
      pqxx::result R1(T.exec(q));

      // 結果取得
      for (result::const_iterator c = R1.begin(); c != R1.end(); ++c) {
        retString = c[0].as(string());
      }
    } catch (const exception &e) {
      throw CastException("sql exec failed: " + std::string(e.what()));
    }

    ret.push_back(retString);
    retList.push_back(ret);
  }

  T.commit();
  //con.disconnect();

  //printf("UF_LON_LAT_TO_XY end!\n");
  return retList;
}
