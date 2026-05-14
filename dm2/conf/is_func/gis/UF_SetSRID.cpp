#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <is/DmUtil.h>
#include <is/StringUtil.h>
#include <is/DmException.h>

using namespace std;

/**
* ジオメトリのSRIDを特定の整数値に設定する。
* @author    Shinichi Kusayama
* @date      2020/03/25
* @param     引数1=geometry, 引数2=srid
* @return    geometry
*/
extern "C" vector< vector<string> > UF_SETSRID(vector< vector<string> > argList)
{
  //printf("UF_SetSRID called!\n");
  vector< vector<string> > retList; // ISに返却する結果2次元配列(retList[row][col])

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
    string srid = "SRID=" + a2 + ";";
    string retString = srid + a1;
    ret.push_back(retString);
    retList.push_back(ret);
  }
  //printf("UF_SetSRID end!\n");
  return retList;
}
