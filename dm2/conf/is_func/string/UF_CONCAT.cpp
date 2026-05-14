#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include <is/DmException.h>
/**
* 引数1と引数2の文字列を結合する
* @author    Nagoya University
* @date      2020/03/13
* @param     引数1=string, 引数2=string
* @return    string
*/
extern "C" vector< vector<string> > UF_CONCAT(vector< vector<string> > argList)
{
  vector< vector<string> > retList;

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    string a1, a2;
    try {
      a1 = argList.at(i).at(0); // 引数1
      a2 = argList.at(i).at(1); // 引数2
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }

    // 文字列の結合を行う
    string retStr = a1 + a2;

    ret.push_back(retStr);
    retList.push_back(ret);
  }
  return retList;
}
