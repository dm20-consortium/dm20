#include <iostream>
#include <vector>
#include <string>
#include <math.h>

using namespace std;

/**
* 円周率の値を取得する
* @author    Nagoya University
* @date      2020/03/13
* @param     none
* @return    double
*/
extern "C" vector< vector<string> > UF_PI(vector< vector<string> > argList)
{
  vector< vector<string> > retList;

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;

    // 円周率を取得する
    double retDouble = M_PI;

    ret.push_back(std::to_string(retDouble));
    retList.push_back(ret);
  }
  return retList;
}
