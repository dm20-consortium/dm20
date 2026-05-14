#include <iostream>
#include <vector>
#include <string>
//#include <is/StringUtil.h>
//#include <algorithm>
//#include <is/DmException.h>
//#include <cmath>
//#include <bits/stdc++.h>
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
extern "C" vector< vector<string> > UF_UOT_TO_STATION(vector< vector<string> > argList)
{
  vector< vector<string> > retList; // ISに返却する結果2次元配列(retList[row][col])

  double ganken_lon =   139.939182;
  double kakeiken_lon = 139.942134;
  string ganken_id = "790529";
  string kakeiken_id = "798721";
  string wakashiba_id = "2";
  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    string a1;
    double lon1;
    string crp_id;
    try {
      a1 = argList.at(i).at(0); // 引数1
      lon1 = stoi(a1) / (double)10000000;
    } catch (invalid_argument) {
      continue;
    } catch (out_of_range) {
      continue;
    }
    if (a1.length() <= 7) {
      continue;
    }
    if (lon1 < ganken_lon) {
      crp_id = ganken_id;
    }
    else if (lon1 < kakeiken_lon) {
      crp_id = kakeiken_id;
    }
    else {
      crp_id = wakashiba_id;
    }
    // 表示形式に整形
    ret.push_back(crp_id);
    retList.push_back(ret);
  }

  //printf("UF_DISTANCE_FOR_COOL4 end!\n");
  return retList;
}

