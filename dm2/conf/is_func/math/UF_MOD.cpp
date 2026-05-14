#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

#include <is/DmException.h>
/**
* 引数1を引数2で割った剰余を求める
* @author    Nagoya University
* @date      2020/03/02
* @param     引数1=double, 引数2=double
* @return    double
*/
extern "C" vector< vector<string> > UF_MOD(vector< vector<string> > argList)
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

    // 0除算はエラーとする
    if (a2 == 0.0) {
       throw CastException("division by zero");
    }

    // 剰余を求める
    double retDouble = std::fmod(a1, a2);

    ret.push_back(std::to_string(retDouble));
    retList.push_back(ret);
  }
  return retList;
}
