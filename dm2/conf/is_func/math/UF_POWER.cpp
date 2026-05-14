#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cerrno>

using namespace std;

#include <is/DmException.h>
/**
* 引数1を引数2で冪乗した値を求める
* @author    Nagoya University
* @date      2020/03/13
* @param     引数1=double, 引数2=double
* @return    double
*/
extern "C" vector< vector<string> > UF_POWER(vector< vector<string> > argList)
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
    if (a1 == 0 && a2 < 0) throw CastException("zero raised to a negative power is undefined!");
    if (a1 < 0 && floor(a2) != a2) throw CastException("a negative number raised to a non-integer power yields a complex result!");
    // 冪乗を求める
    errno = 0;
    double retDouble = std::pow(a1, a2);

    // エラーの判定
    if (errno == EDOM && isnan(retDouble) ) {
      if ((fabs(a1) > 1 && a2 >= 0) || (fabs(a1) < 1 && a2 < 0)) {
        if ( !isinf(a1) ) throw CastException("value out of range: overflow!");
      } else if (fabs(a1) != 1) {
        retDouble = 0;
      } else {
        retDouble = 1;
      }
    } else if (errno == ERANGE && retDouble != 0 && !isinf(retDouble)) {
        if ( !isinf(a1) ) throw CastException("value out of range: overflow!");
    }
    if ( ( !( isinf(a1) || isinf(a2) ) ) && (isinf(retDouble) ) ) throw CastException("value out of range: overflow!");
    if ( ( a1 != 0.0 ) && (retDouble == 0.0 ) ) throw CastException("value out of range: underflow!");

    ret.push_back(std::to_string(retDouble));
    retList.push_back(ret);
  }
  return retList;
}
