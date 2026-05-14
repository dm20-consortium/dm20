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
* 緯度経度をXY平面直角座標に変換。対象は、日本のSRID（JGD2011)のみ。
* @author    Nagoya University
* @date      2020/03/19
* @param     引数1=geometry(LINESTRING), 引数2=Geometry(POINT)
* @return    double
*/
extern "C" vector<vector<string>> UF_LINE_LOCATE_POINT(vector<vector<string>> argList)
{
  const int JGD2011_SRID_GEO=6668; //地理座標のSRID

  //printf("UF_Line_Locate_Point called!\n");
  vector<vector<string>> retList; // ISに返却する結果2次元配列(retList[row][col])

  // 地理座標のSRIDから引数2のSRIDに変換する
  string db_name = "dm2_db";
  string db_user = "dm2";
  string db_pass = "dm2";
  pqxx::connection con("dbname=" + db_name + " user=" + db_user + " password=" + db_pass);
  pqxx::work T(con);
  // SRID指定あり
  string query1 = "";
  query1.append("SELECT ");
  query1.append("  ST_Line_Locate_Point( ");
  query1.append("    ST_GeomFromText('$1', $2), ");
  query1.append("    ST_GeomFromText('$3', $4)  ");
  query1.append("  ); ");
  // SRID指定なし
  string query2 = "";
  query2.append("SELECT ");
  query2.append("  ST_Line_Locate_Point( ");
  query2.append("    ST_GeomFromText('$1'), ");
  query2.append("    ST_GeomFromText('$2')  ");
  query2.append("  ); ");

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    IS::StringUtil stringUtil;
    vector<string> ret;
    string retString = "";
    string a1;
    string a2;
    try {
      a1 = argList.at(i).at(0); // 引数1
      a2 = argList.at(i).at(1); // 引数2
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }

    // WKTからSRIDを分離する
    int srId1 = stringUtil.getSRID(a1);
    if (srId1 != 0) a1 = stringUtil.split(a1,";").at(1);
    int srId2 = stringUtil.getSRID(a2);
    if (srId2 != 0) a2 = stringUtil.split(a2,";").at(1);

    // Geometry文字列を大文字に変換
    string geomStUpp1 = a1;
    transform(geomStUpp1.begin(),geomStUpp1.end(),geomStUpp1.begin(),::toupper);
    string geomStUpp2 = a2;
    transform(geomStUpp2.begin(),geomStUpp2.end(),geomStUpp2.begin(),::toupper);

    // Geometryの型チェック
    vector<string> geomNameList;
    geomNameList.push_back(stringUtil.split(geomStUpp1,"(").at(0));
    geomNameList.push_back(stringUtil.split(geomStUpp2,"(").at(0));
    for (int c = 0; c < geomNameList.size(); c++) {
      string geomName = geomNameList.at(c);
      geomName = stringUtil.split(geomName,"(").at(0);
      string::size_type left = geomName.find_first_not_of(" ");
      geomName = stringUtil.split(geomName.substr(left)," ").at(0);
      if (c == 0) {
        // 引数1はLINESTRINGのみ
        if (geomName != "LINESTRING") {
          throw CastException("1st arg isn't a line.");
        }
      } else{
        // 引数2はPOINTのみ
        if (geomName != "POINT") {
          throw CastException("2nd arg isn't a point.");
        }
      }
    }

    // 引数1と引数2のSRIDが同一か確認
    if (srId1 != srId2) {
      throw CastException("Operation on mixed SRID geometries.");
    }

    // GeometryのEMPTY判定
    if ((geomStUpp1.find("EMPTY") != std::string::npos) || (geomStUpp2.find("EMPTY") != std::string::npos)) {
      throw CastException("point offset out of range");
    }

    // クエリ作成
    string q;
    if (srId1 != 0) {
      q = query1;
      stringUtil.replace(q, "$1", a1);
      stringUtil.replace(q, "$2", to_string(srId1));
      stringUtil.replace(q, "$3", a2);
      stringUtil.replace(q, "$4", to_string(srId2));
    } else {
      q = query2;
      stringUtil.replace(q, "$1", a1);
      stringUtil.replace(q, "$2", a2);
    }

    try {
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

  //printf("UF_Line_Locate_Point end!\n");
  return retList;
}
