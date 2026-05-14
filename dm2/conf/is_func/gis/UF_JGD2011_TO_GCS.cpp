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
* XY平面直角座標を緯度経度に変換。対象は、日本のSRID（JGD2011)のみ。
* @author    Nagoya University
* @date      2020/03/09
* @param     引数1=geometry
* @return    geometry
*/
extern "C" vector<vector<string>> UF_JGD2011_TO_GCS(vector<vector<string>> argList)
{
  const int JGD2011_SRID_GEO=6668;  // 地理座標のSRID
  const int JGD2011_SRID_XY_S=6669; // XY直角平面座標のSRID - START
  const int JGD2011_SRID_XY_E=6687; // XY直角平面座標のSRID - END

  //printf("UF_JGD2011_TO_GCS called!\n");
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
  query.append("    ST_Transform( ST_GeomFromText('$1', $2), $3) ");
  query.append("  ); ");

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    IS::StringUtil stringUtil;
    vector<string> ret;
    string retString = "";
    string a1;
    try {
      a1 = argList.at(i).at(0); // 引数1
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }

    // WKTからSRIDを分離する
    int srId = stringUtil.getSRID(a1);
    if (srId != 0) a1 = stringUtil.split(a1,";").at(1);

    // Geometry文字列を大文字に変換
    string geomStUpp1 = a1;
    transform(geomStUpp1.begin(),geomStUpp1.end(),geomStUpp1.begin(),::toupper);

    // Geometryの型チェック
    string geomName = geomStUpp1;
    geomName = stringUtil.split(geomName,"(").at(0);
    string::size_type left = geomName.find_first_not_of(" ");
    geomName = stringUtil.split(geomName.substr(left)," ").at(0);
    if ((geomName != "POINT") && (geomName != "LINESTRING") && (geomName != "POLYGON") &&
        (geomName != "MULTIPOINT") && (geomName != "MULTILINESTRING") && (geomName != "MULTIPOLYGON")) {
      throw CastException("parse error - invalid geometry.");
    }

    // XY直角平面座標のSRIDか確認する
    if ((srId < JGD2011_SRID_XY_S) || (srId > JGD2011_SRID_XY_E)) {
      throw CastException("SRID error: " + to_string(srId));
    }

    try {
      // クエリ作成
      string q = query;
      stringUtil.replace(q, "$1", a1);
      stringUtil.replace(q, "$2", to_string(srId));
      stringUtil.replace(q, "$3", to_string(JGD2011_SRID_GEO));

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

  //printf("UF_JGD2011_TO_GCS end!\n");
  return retList;
}
