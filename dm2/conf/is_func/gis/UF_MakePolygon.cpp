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
* 与えられた外環で形成されるポリゴンを生成する。
* @author    Nagoya University
* @date      2020/03/09
* @param     引数1=geometry(LINESTRING)
* @return    geometry(POLYGON)
*/
extern "C" vector< vector<string> > UF_MAKEPOLYGON(vector< vector<string> > argList)
{
  //printf("UF_MakePolygon called!\n");
  vector< vector<string> > retList; // ISに返却する結果2次元配列(retList[row][col])

  // 依存ライブラリの初期化処理
  

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    IS::StringUtil stringUtil;
    vector<string> ret;
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
    if (geomName != "LINESTRING") {
       throw CastException("parse error - invalid geometry.");
    }

    // POINTのリストを作成
    vector<string> value, pointList;
    stringUtil.getStringInBracket(a1, value);
    stringUtil.split(value.at(0), ",", pointList);

    // エラー判定
    bool error = false;
    string errString = "";
    if (pointList.size() < 4) {
      // 座標数が4以下
      errString = "ERROR: shell must have at least 4 points";
      error = true;
    }
    else if (pointList.front().compare(pointList.back()) != 0) {
      // 始点と終点が一致していない
      errString = "ERROR: shell must be closed";
      error = true;
    }
    if (error == true) {
       throw CastException(errString);
    }

    // WKT -> Geometry
    GEOSGeometry* g1;
    g1 = GEOSGeomFromWKT(a1.c_str());
    if (g1 == NULL) {
      
      throw CastException("GEOS ERROR. WKT to Geom failed.");
    }

    // GeometryにSRIDを設定
    GEOSSetSRID(g1,srId);

    // Geometryのシーケンスを取得
    const GEOSCoordSequence *g1SeqCon = GEOSGeom_getCoordSeq(g1);

    // Geometryのdimension取得
    unsigned int dim = 2;
    GEOSCoordSeq_getDimensions(g1SeqCon, &dim);

    // LINESTRINGからLINGを生成
    GEOSCoordSequence* seq = GEOSCoordSeq_clone(g1SeqCon);
    GEOSGeometry* gRing = GEOSGeom_createLinearRing(seq);

    // LINGからPOLYGONを生成
    GEOSGeometry* gPol = GEOSGeom_createPolygon(gRing, NULL, 0);

    // Geometry -> WKT
    GEOSWKTWriter *writer = GEOSWKTWriter_create();
    GEOSWKTWriter_setOutputDimension(writer,dim);
    string retString = GEOSWKTWriter_write(writer,gPol);
    GEOSWKTWriter_destroy(writer);

    // Geometryの破棄
    GEOSGeom_destroy(g1);
    GEOSGeom_destroy(gPol);

    // 表示形式に整形
    retString = stringFormatGeosToDm2(retString);
    if (srId != 0) retString.insert(0,"SRID=" + to_string(srId) + ";");
    ret.push_back(retString);
    retList.push_back(ret);
  }

  // 依存ライブラリの終了処理
  
  //printf("UF_MakePolygon end!\n");
  return retList;
}
