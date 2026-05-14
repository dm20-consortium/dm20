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
extern "C" vector<vector<string>> UF_GET_FREESPACE_BY_LANE_REL(vector<vector<string>> argList)
{
  //printf("UF_GET_FREESPACE_BY_LANE_REL called!\n");
  vector<vector<string>> retList; // ISに返却する結果2次元配列(retList[row][col])

  // 地理座標のSRIDから引数2のSRIDに変換する
  string db_name = "dm2_db";
  string db_user = "dm2";
  string db_pass = "dm2";
  pqxx::connection con("dbname=" + db_name + " user=" + db_user + " password=" + db_pass);
  pqxx::work T(con);
  string query = "";
  query.append("SELECT ");
  query.append("ST_AsEwkt(ST_Union(geometry)) as geometry ");
  query.append("FROM ( ");
  query.append("  SELECT ");
  query.append("  ST_Intersection( ");
  query.append("    ST_Buffer( ");
  query.append("      ST_Line_Substring(center_line, ");
  query.append("        ST_Line_Locate_Point(center_line, ");
  query.append("          ST_ClosestPoint(center_line,  ");
  query.append("            ST_TransScale(start_point, $2 / 100, $3 / 100, 1, 1)  ");
  query.append("          ) ");
  query.append("        )  ");
  query.append("      , 1) ");
  query.append("    , lane_width,'endcap=square') ");
  query.append("  , lane_area) AS geometry ");
  query.append("  FROM lane_geometry ");
  query.append("  WHERE lane_id = $1 ");
  query.append("  UNION ");
  query.append("  SELECT ");
  query.append("  ST_Intersection( ");
  query.append("    ST_Buffer( ");
  query.append("      ST_Line_Substring(center_line, 0,  ");
  query.append("        ST_Line_Locate_Point(center_line, ");
  query.append("          ST_ClosestPoint(center_line,  ");
  query.append("            ST_TransScale(start_point, $5 / 100, $6 / 100, 1, 1)  ");
  query.append("          ) ");
  query.append("        )  ");
  query.append("      ) ");
  query.append("    , lane_width,'endcap=square') ");
  query.append("  , lane_area) AS geometry ");
  query.append("  FROM lane_geometry ");
  query.append("  WHERE lane_id = $4 ");
  query.append("  UNION ");
  query.append("  SELECT  ");
  query.append("  mbr AS geometry ");
  query.append("  FROM junction_from_10hop, st_face ");
  query.append("  WHERE dest_lane_id = $4 AND path_lane[1] = $1 ");
  query.append("    AND face_id = any(path_lane) ");
  query.append("    AND face_id NOT IN ($1, $4) ");
  query.append(") TBL ");

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    IS::StringUtil stringUtil;
    vector<string> ret;
    string retString = "";
    string lane1,lane2;
    int dx1,dy1,dx2,dy2;
    try {
      lane1 = argList.at(i).at(0); // 引数1
      dx1 = stoi(argList.at(i).at(1)); // 引数2
      dy1 = stoi(argList.at(i).at(2)); // 引数3
      lane2 = argList.at(i).at(3); // 引数4
      dx2 = stoi(argList.at(i).at(4)); // 引数5
      dy2 = stoi(argList.at(i).at(5)); // 引数6
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }
    try {
      // クエリ作成
      string q = query;
      stringUtil.replace(q, "$1", lane1);
      stringUtil.replace(q, "$2", to_string(dx1));
      stringUtil.replace(q, "$3", to_string(dy1));
      stringUtil.replace(q, "$4", lane2);
      stringUtil.replace(q, "$5", to_string(dx2));
      stringUtil.replace(q, "$6", to_string(dy2));
      //cout << q << endl;
      // クエリ実行
      pqxx::result R1(T.exec(q));

      // 結果取得
      for (result::const_iterator c = R1.begin(); c != R1.end(); ++c) {
        retString = c[0].as(string());
      }
    } catch (const exception &e) {
      throw CastException("sql exec failed: " + std::string(e.what()));
    }
    //cout << retString << endl;
    ret.push_back(retString);
    retList.push_back(ret);

  }

  T.commit();
  //con.disconnect();

  //printf("UF_GET_FREESPACE_BY_LANE_REL end!\n");
  return retList;
}
