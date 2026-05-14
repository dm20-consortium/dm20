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
* ジオメトリの組み合わせ境界の閉包を返却する。
* @author    Nagoya University
* @date      2020/03/04
* @param     引数1=geometry
* @return    geometry
*/
extern "C" vector< vector<string> > UF_BOUNDARY(vector< vector<string> > argList)
{
  //printf("UF_Boundary called!\n");
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

    // WKTからSRIDを取得して削除する
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

    // GeometryのEMPTY判定
    if (geomStUpp1.find("EMPTY") != std::string::npos) {
      ret.push_back(geomName + " EMPTY");
      retList.push_back(ret);
      continue;
    }

    // WKT -> Geometry
    GEOSGeometry* g1;
    g1 = GEOSGeomFromWKT(a1.c_str());
    if (g1 == NULL) {
      
      throw CastException("GEOS ERROR. WKT to Geom failed.");
    }

    // GeometryにSRIDを設定
    GEOSSetSRID(g1,srId);

    // ジオメトリの組み合わせ境界の閉包を取得
    GEOSGeometry* retGm = GEOSBoundary(g1);
    // GeometryにSRIDを設定
    GEOSSetSRID(retGm,srId);

    // Geometryのdimension取得
    unsigned int dim = 2;
    if (GEOSHasZ(retGm)) dim = 3;

    // Geometry -> WKT
    GEOSWKTWriter *writer = GEOSWKTWriter_create();
    GEOSWKTWriter_setOutputDimension(writer,dim);
    string retString = GEOSWKTWriter_write(writer,retGm);
    GEOSWKTWriter_destroy(writer);

    // Geometryの破棄
    GEOSGeom_destroy(g1);
    GEOSGeom_destroy(retGm);

    // 表示形式に整形
    retString = stringFormatGeosToDm2(retString);
    if (srId != 0) retString.insert(0,"SRID=" + to_string(srId) + ";");
    ret.push_back(retString);
    retList.push_back(ret);
  }

  // 依存ライブラリの終了処理
  
  //printf("UF_Boundary end!\n");
  return retList;
}
