#include <iostream>
#include <vector>
#include <string>
#include <is/StringUtil.h>
#include <geos_c.h>
#include <is/DmException.h>
#include "./CoordStrFormat/CoordStrFormat.h"

using namespace std;

void notice_function(const char *message, ...){ printf("%s", message); } // 依存ライブラリの出力用関数
void initialize(){ initGEOS(notice_function, notice_function); }         // 依存ライブラリの初期化処理
void finish(){ finishGEOS(); }                                           // 依存ライブラリの終了処理

/**
* 引数の座標からPOINTを生成する。
* @author    Nagoya University
* @date      2020/03/13
* @param     引数1=double(X), 引数2=double(Y), [引数3=double(Z)]
* @return    geometry(POINT)
*/
extern "C" vector< vector<string> > UF_MAKEPOINT(vector< vector<string> > argList)
{
  //printf("UF_MakePoint called!\n");
  vector< vector<string> > retList; // ISに返却する結果2次元配列(retList[row][col])

  // 依存ライブラリの初期化処理
  

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    double x,y,z;
    unsigned int dimention = 2;
    try {
      x = stod(argList.at(i).at(0)); // 引数1
      y = stod(argList.at(i).at(1)); // 引数2
      z = 0.0;

      // Z座標の有無を判定
      if (argList.at(i).size() == 3) {
        z = stod(argList.at(i).at(2)); // 引数3
        dimention = 3;
      }
    } catch (invalid_argument) {
       throw CastException("Data type is not match!");
    } catch (out_of_range) {
       throw CastException("Insufficient number of arguments!");
    }

    // 返却するPOINT用のシーケンスを作成
    unsigned int size = 1;
    GEOSCoordSequence* retSeq = GEOSCoordSeq_create(size, dimention);
    GEOSCoordSeq_setX(retSeq, 0, x);
    GEOSCoordSeq_setY(retSeq, 0, y);
    if (dimention == 3) {
      GEOSCoordSeq_setZ(retSeq, 0, z);
    }

    // シーケンスをGeometryに変換
    GEOSGeometry* point = GEOSGeom_createPoint(retSeq);

    // Geometry -> WKT
    GEOSWKTWriter *writer = GEOSWKTWriter_create();
    GEOSWKTWriter_setOutputDimension(writer,dimention);
    string retString = GEOSWKTWriter_write(writer,point);
    GEOSWKTWriter_destroy(writer);

    // Geometryの破棄
    GEOSGeom_destroy(point);

    // 表示形式に整形
    retString = stringFormatGeosToDm2(retString);
    ret.push_back(retString);
    retList.push_back(ret);
  }

  // 依存ライブラリの終了処理
  
  //printf("UF_MakePoint end!\n");
  return retList;
}
