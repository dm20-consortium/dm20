#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include <is/DmException.h>
/**
* 引数1の長さを求める
* @author    Nagoya University
* @date      2020/03/13
* @param     引数1=string
* @return    long
*/
extern "C" vector< vector<string> > UF_LENGTH(vector< vector<string> > argList)
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

    // 文字列の長さを求める
    long retLong = a1.length();

    ret.push_back(std::to_string(retLong));
    retList.push_back(ret);
  }
  return retList;
}
