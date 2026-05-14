#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cerrno>

using namespace std;

#include <is/DmException.h>
/**
* 引数1の指数を求める
* @author    Nagoya University
* @date      2020/03/13
* @param     引数1=double
* @return    double
*/
extern "C" vector< vector<string> > UF_EXP(vector< vector<string> > argList)
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

    // 指数を求める
    errno = 0;
    double retDouble = exp(a1);
    // overflow underflow チェック
    if (errno == ERANGE) throw CastException("value out of range: overflow!");
    if ( ( !isinf(a1) ) && (isinf(retDouble) ) ) throw CastException("value out of range: overflow!");
    if ( retDouble == 0.0 ) throw CastException("value out of range: underflow!");

    ret.push_back(std::to_string(retDouble));
    retList.push_back(ret);
  }
  return retList;
}
