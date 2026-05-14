#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include <is/DmException.h>
/**
* 引数1の末尾から引数2で指定したサイズまでの文字列を抽出
* @author    Nagoya University
* @date      2020/03/02
* @param     引数1=double, 引数2=double
* @return    double
*/
extern "C" vector< vector<string> > UF_RIGHT(vector< vector<string> > argList)
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

    // 文字列サイズと引数2の値を比較
    int a1Size = (int)a1.length();
    if (a1Size < a2) {
      // 文字列サイズより大きい場合、同じ値にして
      // 先頭からすべての文字列を返す
      a2 = a1Size;
    }

    // 文字列を抽出
    string retStr;
    if (a2 >= 0) {
      retStr = a1.substr(a1Size - a2);
    } else {
      int cnt = abs(a2);
      if (cnt < (int)a1.length()) {
        retStr = a1.substr(cnt);
      } else {
        retStr = "";
      }
    }

    ret.push_back(retStr);
    retList.push_back(ret);
  }
  return retList;
}
