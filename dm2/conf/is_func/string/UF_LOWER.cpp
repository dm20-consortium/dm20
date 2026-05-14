#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

#include <is/DmException.h>
/**
* 引数1を大文字⇒小文字への変換を行う
* @author    Nagoya University
* @date      2020/03/02
* @param     引数1=double, 引数2=double
* @return    double
*/
extern "C" vector< vector<string> > UF_LOWER(vector< vector<string> > argList)
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

    // 文字列を小文字に変換
    std::transform(a1.cbegin(), a1.cend(), a1.begin(), ::tolower);

    ret.push_back(a1);
    retList.push_back(ret);
  }
  return retList;
}
