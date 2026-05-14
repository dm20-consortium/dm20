#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

#include <is/DmException.h>
/**
* 引数1を引数2で割った値の逆正接関数の値を求める
* @author    Nagoya University
* @date      2020/03/13
* @param     引数1=double, 引数2=double
* @return    double
*/
extern "C" vector< vector<string> > UF_ATAN2(vector< vector<string> > argList)
{
  vector< vector<string> > retList;

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    double a1, a2;
    try {
      a1 = std::stod(argList.at(i).at(0)); // 引数1
      a2 = std::stod(argList.at(i).at(1)); // 引数2
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }

    // 逆正接関数の値を求める
    double retDouble = std::atan2(a1, a2);

    // overflow underflow チェック
    if ( isinf(retDouble) ) throw CastException("value out of range: overflow!");

    ret.push_back(std::to_string(retDouble));
    retList.push_back(ret);
  }
  return retList;
}
