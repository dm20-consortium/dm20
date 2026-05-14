#include <iostream>
#include <vector>
#include <string>
#include <is/StringUtil.h>
#include <geos_c.h>
#include <algorithm>
#include <is/DmException.h>
#include "./CoordStrFormat/CoordStrFormat.h"

using namespace std;

void notice_function(const char *message, ...){ printf("%s", message); } // 依存ライブラリの出力用関数
void initialize(){ initGEOS(notice_function, notice_function); }         // 依存ライブラリの初期化処理
void finish(){ finishGEOS(); }                                           // 依存ライブラリの終了処理

/**
* 引数1のGeometry上の、引数2のGeometryに最も近いPOINTを返却する。
* @author    Shinichi Kusayama
* @date      2023/01/10
* @param     引数1=geometry, 引数2=geometry
* @return    geometry
*/
extern "C" vector< vector<string> > UF_UNION(vector< vector<string> > argList)
{
  //printf("UF_UNION called!\n");
  vector< vector<string> > retList; // ISに返却する結果2次元配列(retList[row][col])

  // 依存ライブラリの初期化処理
  
  int srId1;
  vector<string> ret;
  string retString;
  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    IS::StringUtil stringUtil;
    string a1;
    try {
      a1 = argList.at(i).at(0); // 引数1
    } catch (invalid_argument) {
       throw CastException("Data type is not match!");
    } catch (out_of_range) {
       throw CastException("Insufficient number of arguments!");
    }

    // WKTからSRIDを分離する
    srId1 = stringUtil.getSRID(a1);
    if (srId1 != 0) a1 = stringUtil.split(a1,";").at(1);

    // Geometry文字列を大文字に変換
    string geomStUpp1 = a1;
    transform(geomStUpp1.begin(),geomStUpp1.end(),geomStUpp1.begin(),::toupper);

    // Geometryの型チェック
    vector<string> geomNameList;
    geomNameList.push_back(stringUtil.split(geomStUpp1,"(").at(0));
    for (int c = 0; c < geomNameList.size(); c++) {
      string geomName = geomNameList.at(c);
      geomName = stringUtil.split(geomName,"(").at(0);
      string::size_type left = geomName.find_first_not_of(" ");
      geomName = stringUtil.split(geomName.substr(left)," ").at(0);
      //if ((geomName != "POINT") && (geomName != "LINESTRING") && (geomName != "POLYGON") &&
         // (geomName != "MULTIPOINT") && (geomName != "MULTILINESTRING") && (geomName != "MULTIPOLYGON")) {
         //throw CastException("parse error - invalid geometry.");
      //}
    }

    // GeometryのEMPTY判定
    // EMPTYは空データを返却
    if (geomStUpp1.find("EMPTY") != std::string::npos) {
      ret.push_back("");
      retList.push_back(ret);
      continue;
    }

    // WKT -> Geometry
    GEOSGeometry* g1;
    GEOSGeometry* g2;
    if (i == 0) {
    g1 = GEOSGeomFromWKT(a1.c_str());
    } else {
    g1 = GEOSGeomFromWKT(retString.c_str());
    }
    g2 = GEOSGeomFromWKT(a1.c_str());
    if (g1 == NULL) {
      
      throw CastException("GEOS ERROR. WKT to Geom failed.");
    }

    // GeometryにSRIDを設定
    GEOSSetSRID(g1,srId1);
    GEOSSetSRID(g2,srId1);

    GEOSGeometry* uniongeo = GEOSUnion(g1,g2);

    // Geometry -> WKT
    GEOSWKTWriter *writer = GEOSWKTWriter_create();
    //GEOSWKTWriter_setOutputDimension(writer,dimention);
    retString = GEOSWKTWriter_write(writer,uniongeo);
    GEOSWKTWriter_destroy(writer);

    // Geometryの破棄
    GEOSGeom_destroy(g1);
    GEOSGeom_destroy(g2);
    GEOSGeom_destroy(uniongeo);

  }
    // 表示形式に整形
    retString = stringFormatGeosToDm2(retString);
    if (srId1 != 0) retString.insert(0,"SRID=" + to_string(srId1) + ";");
    ret.push_back(retString);
    retList.push_back(ret);

  // 依存ライブラリの終了処理
  
  //printf("UF_UNION end!\n");
  return retList;
}
