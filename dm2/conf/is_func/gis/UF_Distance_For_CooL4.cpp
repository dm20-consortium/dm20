#include <iostream>
#include <vector>
#include <string>
#include <is/StringUtil.h>
#include <geos_c.h>
#include <algorithm>
#include <is/DmException.h>
#include <cmath>
#include <bits/stdc++.h>
//#include <iomanip>  $ for setprecision
using namespace std;

/**
* 引数1のGeometryと引数2～4から成るPointの最小の距離（メートル）を返却する。
* @author    Shinichi Kusayama
* @date      2023/01/08
* @param     引数1=geometry, 引数2=x(0.1マイクロ度), 引数3=y(0.1マイクロ度), 引数4=SRID
* @return    double
*/
extern "C" vector< vector<string> > UF_DISTANCE_FOR_COOL4(vector< vector<string> > argList)
{
  //printf("UF_DISTANCE_FOR_COOL4 called!\n");
  vector< vector<string> > retList; // ISに返却する結果2次元配列(retList[row][col])
  IS::StringUtil stringUtil;

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    string a1,a2,a3,a4;
    double lon1,lat1,lon2,lat2;
    try {
      a1 = argList.at(i).at(0); // 引数1
      a2 = argList.at(i).at(1); // 引数2
      lon2 = stoi(a2) / (double)10000000;
      a3 = argList.at(i).at(2); // 引数3
      lat2 = stoi(a3) / (double)10000000;
      a4 = argList.at(i).at(3); // 引数4
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }
    // WKTからSRIDを分離する
    int srId1 = stringUtil.getSRID(a1);
    if (srId1 != 0) a1 = stringUtil.split(a1,";").at(1);

    int srId2 = stoi(a4);
    // SRIDが同一か確認
    if (srId1 != srId2) {
      throw CastException("Operation on mixed SRID geometries.");
    }

    
    // Geometry文字列を大文字に変換
    string geomStUpp1 = a1;
    transform(geomStUpp1.begin(),geomStUpp1.end(),geomStUpp1.begin(),::toupper);
    
    if (a2.length() <= 7 || a3.length() <= 7) {
      throw CastException("Insufficient number of arguments!");
    }
    string ux = a2.substr(0, a2.length() - 7) + "." + a2.substr(a2.length() - 7);
    string uy = a3.substr(0, a3.length() - 7) + "." + a3.substr(a3.length() - 7);
    string geomStUpp2 = "POINT(" + to_string(lon2) + " " + to_string(lat2) + ")";

    // Geometryの型チェック
    vector<string> geomNameList;
    geomNameList.push_back(stringUtil.split(geomStUpp1,"(").at(0));
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
    g1 = GEOSGeomFromWKT(a1.c_str());
    g2 = GEOSGeomFromWKT(geomStUpp2.c_str());
    if ((g1 == NULL) || (g2 == NULL)) {
      throw CastException("GEOS ERROR. WKT to Geom failed.");
    }
    // GeometryにSRIDを設定
    GEOSSetSRID(g1,srId1);
    GEOSSetSRID(g2,srId2);
    int result = GEOSCovers(g1,g2);
    string retString;
    if (result == 1) {
    retString = "0";
    } else {
    // Geometry1上のGeometry2に最も近いポイントを取得
    GEOSCoordSequence* nearSeq = GEOSNearestPoints(g1,g2);

    GEOSCoordSeq_getX(nearSeq,0,&lon1);
    GEOSCoordSeq_getY(nearSeq,0,&lat1);
    //cout << std::setprecision(16) << lat2 << endl;  #need include <iomamip>
    // Google Mapで使用されている方法（地球を真球として扱う方法）で計算する。
    double clat1 = cos(lat1 * M_PI / (double)180);
    double clat2 = cos(lat2 * M_PI / (double)180);
    double lon1p = lon1 * M_PI / (double)180;
    double lon2p = lon2 * M_PI / (double)180;
    double clon = cos(lon2p - lon1p);
    double slat1 = sin(lat1 * M_PI / (double)180);
    double slat2 = sin(lat2 * M_PI / (double)180);
    double cs = clat1 * clat2 * clon + slat1 * slat2;
    double distance = 6371 * 1000 * acos(cs);
    //cout << "a1:" << a1 << ",a2:" << a2 << ",a3:" << a3 << "geom:" << geomStUpp2 << ",lon1 :" << lon1 << "," << "lat1 :" << lat1 << "," << "lon2 :" << lon2 << "," << "lat2 :" << lat2 << "," << "clat1:" << clat1 << "," << "clat2:" << clat2 << "," << "lon1p:" << lon1p << "," << "lon2p:" << lon2p << "," << "clon :" << clon  << "," << "slat1:" << slat1 << "," << "slat2:" << slat2 << "," << "cs   :" << cs << "distance:" << distance    << endl;
    retString = to_string(distance);
    }

    // Geometryの破棄
    GEOSGeom_destroy(g1);
    GEOSGeom_destroy(g2);

    // 表示形式に整形
    ret.push_back(retString);
    retList.push_back(ret);
  }

  //printf("UF_DISTANCE_FOR_COOL4 end!\n");
  return retList;
}

