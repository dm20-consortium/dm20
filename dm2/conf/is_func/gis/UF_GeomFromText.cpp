#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <is/DmUtil.h>
#include <is/StringUtil.h>
#include <is/DmException.h>

using namespace std;

/**
* Well-Known Text表現(WKT)から指定したST_Geometryを返す。
* @author    Shinichi Kusayama
* @date      2020/03/25
* @param     引数1=string
* @return    geometry
*/
extern "C" vector< vector<string> > UF_GEOMFROMTEXT(vector< vector<string> > argList)
{
  //printf("UF_GeomFromText called!\n");
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
    ret.push_back(a1);
    retList.push_back(ret);
  }
  //printf("UF_GeomFromText end!\n");
  return retList;
}
