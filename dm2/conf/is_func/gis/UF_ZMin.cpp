#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <is/StringUtil.h>
#include <is/DmException.h>
#include "GeoAccessor.h"

using namespace std;

/**
* GeometryのZの最小値を返却する。
* @author    Nagoya University
* @date      2020/03/10
* @param     引数1=geometry
* @return    double
*/
extern "C" vector< vector<string> > UF_ZMIN(vector< vector<string> > argList)
{
  //printf("UF_ZMIN called!\n");
  vector< vector<string> > retList; // ISに返却する結果2次元配列(retList[row][col])

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
    if ((geomName != "POINT") && (geomName != "LINESTRING") && (geomName != "POLYGON") &&
        (geomName != "MULTIPOINT") && (geomName != "MULTILINESTRING") && (geomName != "MULTIPOLYGON")) {
      throw CastException("parse error - invalid geometry.");
    }

    // GeometryのEMPTY判定
    // EMPTYは空データを返却
    if (geomStUpp1.find("EMPTY") != std::string::npos) {
      ret.push_back("");
      retList.push_back(ret);
      continue;
    }

    // WKTから指定次元のリストを取得
    vector<double> getNumList = GeoAccessor(a1,3);

    // Zがない場合は0を返却
    if ( getNumList.empty() ) {
      ret.push_back("0");
      retList.push_back(ret);
      continue;
    }

    // 最小値を取得
    double min = *min_element(getNumList.begin(), getNumList.end());

    // 表示形式に整形
    string retString = to_string(min);

    ret.push_back(retString);
    retList.push_back(ret);
  }

  //printf("UF_ZMIN end!\n");
  return retList;
}
