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
* 引数1～2から成るPointと引数3～4から成るPointの最小の距離（メートル）を返却する。
* @author    Shinichi Kusayama
* @date      2024/02/12
* @param     引数1=lon1(0.1マイクロ度), 引数2=lat1(0.1マイクロ度)
*            引数3=lon2(0.1マイクロ度), 引数4=lat2(0.1マイクロ度)
* @return    double
*/
extern "C" vector< vector<string> > UF_DISTANCE_4326(vector< vector<string> > argList)
{
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
      lon1 = stoi(a1) / (double)10000000;
      a2 = argList.at(i).at(1); // 引数2
      lat1 = stoi(a2) / (double)10000000;
      a3 = argList.at(i).at(2); // 引数3
      lon2 = stoi(a3) / (double)10000000;
      a4 = argList.at(i).at(3); // 引数4
      lat2 = stoi(a4) / (double)10000000;
    } catch (invalid_argument) {
      continue;
    } catch (out_of_range) {
      continue;
    }
    if (a1.length() <= 7 || a2.length() <= 7 || a3.length() <= 7 || a4.length() <= 7) {
      continue;
    }
    string geomStUpp1 = "POINT(" + to_string(lon1) + " " + to_string(lat1) + ")";
    string geomStUpp2 = "POINT(" + to_string(lon2) + " " + to_string(lat2) + ")";

    // WKT -> Geometry
    GEOSGeometry* g1;
    GEOSGeometry* g2;
    g1 = GEOSGeomFromWKT(geomStUpp1.c_str());
    g2 = GEOSGeomFromWKT(geomStUpp2.c_str());
    if ((g1 == NULL) || (g2 == NULL)) {
      throw CastException("GEOS ERROR. WKT to Geom failed.");
    }
    // GeometryにSRIDを設定
    GEOSSetSRID(g1,4326);
    GEOSSetSRID(g2,4326);
    int result = GEOSCovers(g1,g2);
    string retString;
    if (result == 1) {
      GEOSGeom_destroy(g1);
      GEOSGeom_destroy(g2);
      continue;
    } 
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

