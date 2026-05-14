#include <iostream>
#include <vector>
#include <string>
#include <is/DmUtil.h>
#include <is/StringUtil.h>
#include <geos_c.h>
#include <float.h>
#include <math.h>
#include <algorithm>
#include <is/DmException.h>
#include "./CoordStrFormat/CoordStrFormat.h"

using namespace std;

void notice_function(const char *message, ...){ printf("%s", message); } // 依存ライブラリの出力用関数
void initialize(){ initGEOS(notice_function, notice_function); }         // 依存ライブラリの初期化処理
void finish(){ finishGEOS(); }                                           // 依存ライブラリの終了処理

/**
* 引数のポイントからLineを生成する
* @author    Nagoya University
* @date      2020/03/09
* @param     引数1=geometry
* @return    geometry
*/
extern "C" vector< vector<string> > UF_MAKELINE_UNION(vector< vector<string> > argList)
{
  vector< vector<string> > retList; // ISに返却する結果2次元配列(retList[row][col])

  IS::StringUtil stringUtil;
  vector<string> ret;
  string retString;

  // 依存ライブラリの初期化処理
  
  // 入力された各行に対して操作を行う

  vector< pair<int,string> > orderAndGeoList; // OrderNo制御用にペアにする
  for(int a=0; a<argList.size(); a++)
  {
    int OrderNo;
    string geo;
    try {
      geo = argList.at(a).at(0); // 引数1
      OrderNo = stoi(argList.at(a).at(1)); // 引数2
    } catch (invalid_argument) {
       throw CastException("Data type is not match!");
    } catch (out_of_range) {
       throw CastException("Insufficient number of arguments!");
    }
    orderAndGeoList.push_back( make_pair(OrderNo,geo) );
  }

  // OrderNoの昇順でソート
  sort(orderAndGeoList.begin(), orderAndGeoList.end());

  // 以降リファクタリングの余地あり
  // a1を起点にa2を連結していく
  // a1はまず1行目を設定して、以降は連結するたびa1に上書きする
  // for文は2行目から開始して、a2に設定する
  string a1,a2;
  a1 = orderAndGeoList.at(0).second;
  for(int i=1; i<orderAndGeoList.size(); i++)
  {
    a2 = orderAndGeoList.at(i).second;

    // ラインを形成するためにはジオメトリが2つ必要なため
    // それぞれ格納する
    vector<string> args;
    args.push_back(a1);
    args.push_back(a2);

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
    vector<bool> geomIsLineList;
    vector<string> geomNameList;
    geomNameList.push_back(stringUtil.split(geomStUpp1,"(").at(0));
    geomNameList.push_back(stringUtil.split(geomStUpp2,"(").at(0));
    for (int c = 0; c < geomNameList.size(); c++) {
      string geomName = geomNameList.at(c);
      geomName = stringUtil.split(geomName,"(").at(0);
      string::size_type left = geomName.find_first_not_of(" ");
      geomName = stringUtil.split(geomName.substr(left)," ").at(0);
      if ((geomName != "POINT") && (geomName != "LINESTRING")) {
         throw CastException("parse error - invalid geometry.");
      }
      // GeometryがLINESTRINGか判定してリストに保持
      if (geomName == "LINESTRING") {
        geomIsLineList.push_back(true);
      } else {
        geomIsLineList.push_back(false);
      }
    }

    // SRIDが同一か確認
    if (srId1 != srId2) {
       throw CastException("Operation on mixed SRID geometries.");
    }

    // WKT -> Geometry (型チェック)
    GEOSGeometry* g1;
    GEOSGeometry* g2;
    g1 = GEOSGeomFromWKT(a1.c_str());
    g2 = GEOSGeomFromWKT(a2.c_str());
    if ((g1 == NULL) || (g2 == NULL)) {
       throw CastException("GEOS ERROR. WKT to Geom failed.");
    }

    // GeometryのEMPTY判定
    // 両方がEMPTYの場合はLINESTRING EMPTY
    // 片方のみEMPTYの場合はEMPTYでない方をそのまま返却
    bool arg1Empty = false;
    bool arg2Empty = false;
    if (geomStUpp1.find("EMPTY") != std::string::npos) arg1Empty = true;
    if (geomStUpp2.find("EMPTY") != std::string::npos) arg2Empty = true;
    if ((arg1Empty == true) || (arg2Empty == true)) {
      if ((arg1Empty == true) && (arg2Empty == true)) {
        retString = "LINESTRING EMPTY";
      } else if (arg1Empty == true) {
        retString = a2;
      } else if (arg2Empty == true) {
        retString = a1;
      }
      
      a1 = retString;
      continue;
    }

    // 両方がLINESTRINGの場合は次元が同一か確認
    if ((geomIsLineList.at(0) == true) && (geomIsLineList.at(1) == true)) {
      if ( ( (GEOSHasZ(g1) == true) && (GEOSHasZ(g2) == false) ) ||
           ( (GEOSHasZ(g1) == false) && (GEOSHasZ(g2) == true) )
         ) {
         throw CastException("mixed dimensionality is not allowed.");
      }
    }

    // どちらかが二次元のLINESTRINGの場合はもう一方が三次元でないか確認
    if ((geomIsLineList.at(0) == true) && (GEOSHasZ(g1) == false) && (GEOSHasZ(g2) == true)) {
       throw CastException("mixed dimensionality is not allowed.");
    }
    if ((geomIsLineList.at(1) == true) && (GEOSHasZ(g2) == false) && (GEOSHasZ(g1) == true)) {
       throw CastException("mixed dimensionality is not allowed.");
    }

    // 2つのジオメトリの座標を1つのリストにまとめる
    vector<string> mPointList;
    unsigned int a1Size = 0;
    for (int j = 0; j < args.size(); j++)
    {
      string input = args.at(j);
      vector<string> value, pointList;
      stringUtil.getStringInBracket(input, value);
      stringUtil.split(value.at(0), ",", pointList);
      mPointList.insert(mPointList.end(), pointList.begin(), pointList.end());
      if (j == 0) {
        a1Size = static_cast<int>(pointList.size());
      }
      value.clear();
      pointList.clear();
    }

    // 2つのジオメトリの内、g1の末尾のポイントと
    // g2の先頭のポイントが2次元で同一だった場合、
    // postgress ではリストから省いていたため
    // 同じ動作となるようにチェック処理を行う
    // ただし、リストのサイズが3未満もしくは
    // g2のポイントが1つの場合は実施しない

    if (mPointList.size() >= 3 && mPointList.size() - a1Size >= 2) {
      // ポイントを格納したリストから対象のポイントを抜き出す
      string a1Str = mPointList.at(a1Size -1);
      string a2Str = mPointList.at(a1Size);
      vector<string> a1List, a2List;
      stringUtil.replace(a1Str, "  ", " ");
      stringUtil.split(a1Str, " ", a1List);

      stringUtil.replace(a2Str, "  ", " ");
      stringUtil.split(a2Str, " ", a2List);

      // それぞれのX座標とY座標を取得
      double a1X, a1Y, a2X, a2Y;
      a1X = std::stod(a1List.at(0));
      a1Y = std::stod(a1List.at(1));

      a2X = std::stod(a2List.at(0));
      a2Y = std::stod(a2List.at(1));

      // 2つのポイントのXとYの座標が一致したら
      // g2の先頭ポイントをリストから削除する
      if (fabs(a1X - a2X) < DBL_EPSILON &&
            fabs(a1Y - a2Y) < DBL_EPSILON) {
         mPointList.erase(mPointList.begin() + a1Size);
      }

      a1List.clear();
      a2List.clear();
    }

    // CoordSeqの作成
    GEOSCoordSeq seq = GEOSCoordSeq_create((int)mPointList.size(), 3);

    // CoordSeqに設定
    int j = 0;
    for (string pointStr : mPointList)
    {
      // シーケンスに座標情報を入れていく
      vector<string> valList;
      stringUtil.replace(pointStr, "  ", " ");
      stringUtil.split(pointStr, " ", valList);
      GEOSCoordSeq_setX(seq, j, std::stod(valList.at(0)));
      GEOSCoordSeq_setY(seq, j, std::stod(valList.at(1)));
      // Z軸は必須ではないため、サイズを見て設定を行う
      if (valList.size() >= 3)
      {
        GEOSCoordSeq_setZ(seq, j, std::stod(valList.at(2)));
      }
      valList.clear();
      j++;
    }

    // CoordSeqからLINESTRINGを生成
    GEOSGeometry *geosGm = GEOSGeom_createLineString(seq);
    string geomString = GEOSGeomToWKT(geosGm);

    // Geometry -> WKT
    unsigned int dimension = 2;
    if ((GEOSHasZ(g1)) || (GEOSHasZ(g2))) dimension = 3;
    GEOSWKTWriter *writer = GEOSWKTWriter_create();
    GEOSWKTWriter_setOutputDimension(writer,dimension);
    retString = GEOSWKTWriter_write(writer,geosGm);

    // Geometryの破棄
    GEOSGeom_destroy(geosGm);
    GEOSGeom_destroy(g1);
    GEOSGeom_destroy(g2);

    // 表示形式に整形
    retString = stringFormatGeosToDm2(retString);
    if (srId1 != 0) retString.insert(0,"SRID=" + to_string(srId1) + ";");

    // a1を上書き
    a1 = retString;
  }

  // a1を返却
  ret.push_back(a1);
  retList.push_back(ret);

  // 依存ライブラリの終了処理
  
  return retList;
}
