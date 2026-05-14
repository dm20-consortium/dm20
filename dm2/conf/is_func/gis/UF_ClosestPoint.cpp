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
* @author    Nagoya University
* @date      2020/03/10
* @param     引数1=geometry, 引数2=geometry
* @return    geometry(POINT)
*/
extern "C" vector< vector<string> > UF_CLOSESTPOINT(vector< vector<string> > argList)
{
  //printf("UF_ClosestPoint called!\n");
  vector< vector<string> > retList; // ISに返却する結果2次元配列(retList[row][col])

  // 依存ライブラリの初期化処理
  

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    IS::StringUtil stringUtil;
    vector<string> ret;
    string a1,a2;
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
      if ((geomName != "POINT") && (geomName != "LINESTRING") && (geomName != "POLYGON") &&
          (geomName != "MULTIPOINT") && (geomName != "MULTILINESTRING") && (geomName != "MULTIPOLYGON")) {
         throw CastException("parse error - invalid geometry.");
      }
    }

    // SRIDが同一か確認
    if (srId1 != srId2) {
       throw CastException("Operation on mixed SRID geometries.");
    }

    // GeometryのEMPTY判定
    // EMPTYは空データを返却
    if ((geomStUpp1.find("EMPTY") != std::string::npos) || (geomStUpp2.find("EMPTY") != std::string::npos)) {
      ret.push_back("");
      retList.push_back(ret);
      continue;
    }

    // WKT -> Geometry
    GEOSGeometry* g1;
    GEOSGeometry* g2;
    g1 = GEOSGeomFromWKT(a1.c_str());
    g2 = GEOSGeomFromWKT(a2.c_str());
    if ((g1 == NULL) || (g2 == NULL)) {
      
      throw CastException("GEOS ERROR. WKT to Geom failed.");
    }

    // GeometryにSRIDを設定
    GEOSSetSRID(g1,srId1);
    GEOSSetSRID(g2,srId2);

    // Geometry1上のGeometry2に最も近いポイントを取得
    GEOSCoordSequence* nearSeq = GEOSNearestPoints(g1,g2);

    // Xを取得
    double x;
    GEOSCoordSeq_getX(nearSeq,0,&x);

    // Yを取得
    double y;
    GEOSCoordSeq_getY(nearSeq,0,&y);

    // 返却Geometry用のシーケンスを作成
    unsigned int size = 1;
    unsigned int dimention = 2;
    GEOSCoordSequence* retSeq = GEOSCoordSeq_create(size, dimention);
    GEOSCoordSeq_setX(retSeq, 0, x);
    GEOSCoordSeq_setY(retSeq, 0, y);

    // シーケンスをGeometryに変換
    GEOSGeometry* point = GEOSGeom_createPoint(retSeq);

    // Geometry -> WKT
    GEOSWKTWriter *writer = GEOSWKTWriter_create();
    GEOSWKTWriter_setOutputDimension(writer,dimention);
    string retString = GEOSWKTWriter_write(writer,point);
    GEOSWKTWriter_destroy(writer);

    // Geometryの破棄
    GEOSGeom_destroy(g1);
    GEOSGeom_destroy(g2);
    GEOSGeom_destroy(point);

    // 表示形式に整形
    retString = stringFormatGeosToDm2(retString);
    if (srId1 != 0) retString.insert(0,"SRID=" + to_string(srId1) + ";");
    ret.push_back(retString);
    retList.push_back(ret);
  }

  // 依存ライブラリの終了処理
  
  //printf("UF_ClosestPoint end!\n");
  return retList;
}
