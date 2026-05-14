#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

#include <is/DmException.h>
/**
* 引数1の自然対数を求める
* @author    Nagoya University
* @date      2020/03/13
* @param     引数1=double
* @return    double
*/
extern "C" vector< vector<string> > UF_LN(vector< vector<string> > argList)
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

    // 入力チェック
    if (a1 == 0.0) throw CastException("cannot take logarithm of zero!");
    if (a1 < 0) throw CastException("cannot take logarithm of a negative number!");

    // 自然対数を求める
    double retDouble = log(a1);

    // overflow underflow チェック
    if ( ( !isinf(a1) ) && (isinf(retDouble) ) ) throw CastException("value out of range: overflow!");
    if ( ( a1 != 1.0 ) && (retDouble == 0.0 ) ) throw CastException("value out of range: underflow!");

    ret.push_back(std::to_string(retDouble));
    retList.push_back(ret);
  }
  return retList;
}
