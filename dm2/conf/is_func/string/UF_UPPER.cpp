#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

#include <is/DmException.h>
/**
* 引数1を小文字⇒大文字への変換を行う
* @author    Nagoya University
* @date      2020/03/13
* @param     引数1=string
* @return    string
*/
extern "C" vector< vector<string> > UF_UPPER(vector< vector<string> > argList)
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

    // 大文字への変換を行う
    std::transform(a1.cbegin(), a1.cend(), a1.begin(), ::toupper);

    ret.push_back(a1);
    retList.push_back(ret);
  }
  return retList;
}
