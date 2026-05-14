#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include <is/DmException.h>
/**
* 引数1の先頭から引数2で指定した場所までの文字列を抽出する
* @author    Nagoya University
* @date      2020/03/13
* @param     引数1=string, 引数2=int
* @return    string
*/
extern "C" vector< vector<string> > UF_LEFT(vector< vector<string> > argList)
{
  vector< vector<string> > retList;

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    string a1;
    int a2;
    try {
      a1 = argList.at(i).at(0); // 引数1
      a2 = std::stoi(argList.at(i).at(1)); // 引数2
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }

    // 文字列を抽出する
    string retStr;
    if (a2 >= 0) {
      retStr = a1.substr(0, a2);
    } else {
      int cnt = abs(a2);
      if (cnt < a1.length()) {
        retStr = a1.substr(0, a1.length()-cnt);
      } else {
        retStr = "";
      }
    }

    ret.push_back(retStr);
    retList.push_back(ret);
  }
  return retList;
}
