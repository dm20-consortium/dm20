#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

#include <is/DmException.h>
/**
* 引数1を底値とする引数2の対数を求める
* @author    Nagoya University
* @date      2020/03/02
* @param     引数1=double, 引数2=double
* @return    double
*/
extern "C" vector< vector<string> > UF_LOG(vector< vector<string> > argList)
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

    // 入力チェック
    if (a1 == 0.0) throw CastException("cannot take logarithm of zero!");
    if (a1 < 0) throw CastException("cannot take logarithm of a negative number!");
    if (a2 == 0.0) throw CastException("cannot take logarithm of zero!");
    if (a2 < 0) throw CastException("cannot take logarithm of a negative number!");

    // 自然対数を求める
    double retDouble;
    double a1log = log(a1);
    double a2log = log(a2);

    // overflow underflow チェック
    if ( ( !isinf(a1) ) && (isinf(a1log) ) ) throw CastException("value out of range: overflow!");
    if ( ( a1 != 1.0 ) && (a1log == 0.0 ) ) throw CastException("value out of range: underflow!");
    if ( ( !isinf(a2) ) && (isinf(a2log) ) ) throw CastException("value out of range: overflow!");
    if ( ( a2 != 1.0 ) && (a2log == 0.0 ) ) throw CastException("value out of range: underflow!");

    // 0除算はエラーとする
    if (a1log == 0.0) {
      throw CastException("division by zero");
    }

    // 引数1を底値として、引数2の対数を求める
    retDouble = a2log / a1log;

    ret.push_back(std::to_string(retDouble));
    retList.push_back(ret);
  }
  return retList;
}
