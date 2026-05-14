#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cerrno>

using namespace std;

#include <is/DmException.h>
/**
* 引数1の余接関数の値を求める
* @author    Nagoya University
* @date      2020/03/13
* @param     引数1=double
* @return    double
*/
extern "C" vector< vector<string> > UF_COT(vector< vector<string> > argList)
{
  vector< vector<string> > retList;

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    double a1;
    try {
      a1 = std::stod(argList.at(i).at(0)); // 引数1
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }

    // 正接関数の値を求める
    errno = 0;
    double a1tan = std::tan(a1);

    // エラー判定
    if ( (errno != 0) || (isinf(a1)) ) throw CastException("input is out of range!");

    // 1/tan で余接関数の値を求める
    double retDouble = 1.0 / a1tan;

    ret.push_back(std::to_string(retDouble));
    retList.push_back(ret);
  }
  return retList;
}
