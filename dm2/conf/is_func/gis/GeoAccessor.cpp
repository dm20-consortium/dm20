#include "GeoAccessor.h"
#include <is/DmException.h>

using namespace std;

void notice_function(const char *message, ...){ printf("%s", message); } // 依存ライブラリの出力用関数
void initialize(){ initGEOS(notice_function, notice_function); }         // 依存ライブラリの初期化処理
void finish(){ finishGEOS(); }                                           // 依存ライブラリの終了処理

/**
* WKTから指定次元の座標をリストで取得する。
* @author    Nagoya University
* @date      2020/03/11
* @param     wktText=WKT(SRIDなし), targetDimension=1(X):2(Y):3(Z)
* @return    指定次元の座標リスト
*/
vector<double> GeoAccessor(string wktText, int targetDimension)
{
  vector<double> retList;

  // targetDimensionの入力チェック
  if ((targetDimension < 1) || (targetDimension > 3)) {
    throw CastException("target dim error: " + to_string(targetDimension));
  }

  // 依存ライブラリの初期化処理
  

  // WKT -> Geometry
  GEOSGeometry* g1;
  g1 = GEOSGeomFromWKT(wktText.c_str());
  if (g1 == NULL) {
    
    throw CastException("GEOS ERROR. WKT to Geom failed.");
  }

  // 内包するGeometryの数を取得
  unsigned int ngeoms = GEOSGetNumGeometries(g1);

  // 内包する全てのGeometryを処理
  for (int i=0; i<ngeoms; i++) {
    // N番目のGeometryを取得
    const GEOSGeometry* geom = GEOSGetGeometryN(g1, i);

    // N番目のGeometryのシーケンスを取得
    const GEOSCoordSequence* geomSeq;
    const GEOSGeometry* ring;
    int type = GEOSGeomTypeId(geom);
    if (type == GEOS_POLYGON) {
      // POLYGONはリングからシーケンスを取得
      ring = GEOSGetExteriorRing(geom);
      geomSeq = GEOSGeom_getCoordSeq(ring);
    } else {
      // POLYGON以外はそのまま取得
      geomSeq = GEOSGeom_getCoordSeq(geom);
    }

    // Geometryのサイズと次元を取得
    unsigned int seqSize, seqDim;
    GEOSCoordSeq_getSize(geomSeq, &seqSize);
    GEOSCoordSeq_getDimensions(geomSeq, &seqDim);

    // 取得対象の次元が存在しない場合はスキップ
    if ((targetDimension == 3) && (seqDim != 3)) {
      continue;
    }

    // Geometry内の全座標から値を取得
    for (int j=0; j < seqSize; j++) {

      double getNum=0.0;

      if (targetDimension == 1) {
        // Xを取得
        GEOSCoordSeq_getX(geomSeq, j, &getNum);
      }
      else if (targetDimension == 2) {
        // Yを取得
        GEOSCoordSeq_getY(geomSeq, j, &getNum);
      }
      else if (targetDimension == 3) {
        // Zを取得
        GEOSCoordSeq_getZ(geomSeq, j, &getNum);
      }
      retList.push_back(getNum);
    }
  }

  // Geometryの破棄
  GEOSGeom_destroy(g1);

  // 依存ライブラリの終了処理
  
  // リストを返却
  return retList;
}
