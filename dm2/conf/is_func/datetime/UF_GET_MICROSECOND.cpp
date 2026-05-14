#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include <is/DmException.h>
/**
* UNIX時間からミリ秒を取得
* @author    Nagoya University
* @date      2020/03/10
* @param     引数1=long
* @return    int
*/
extern "C" vector< vector<string> > UF_GET_MICROSECOND(vector< vector<string> > argList)
{
  vector< vector<string> > retList;

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    string a1;
    try {
      a1 = argList.at(i).at(0); // 引数1
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }

    // ミリ秒は入力された値の下3桁なので
    // 他クラスを使わず、そのまま計算で算出する  
    long int ms;
    std::size_t stolSize;
    try {
      ms = std::stol(a1,&stolSize);
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    }
    if (stolSize != a1.size()) throw CastException("Data type is not match!");

    long int milli = ms % 1000;

    string str = std::to_string(std::abs(milli));

    ret.push_back(str);
    retList.push_back(ret);
  }

  return retList;
}
