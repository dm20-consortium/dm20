#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

#include <is/DmException.h>
/**
* 引数1の符号を求める
* @author    Nagoya University
* @date      2020/03/13
* @param     引数1=double
* @return    long
*/
extern "C" vector< vector<string> > UF_SIGN(vector< vector<string> > argList)
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

    // 符号を求める
    // 引数1が0より大きければ 最初の判定が1となるため 1 - 0 で 1となる
    // 引数1が0より小さければ 2番目の判定が1となるため 0 - 1 で -1となる
    // 引数1が0であれば、どちらの判定も0となるため 0 となる
    long retDouble = (a1 > 0) - (a1 < 0);

    ret.push_back(std::to_string(retDouble));
    retList.push_back(ret);
  }
  return retList;
}
