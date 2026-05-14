#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

using namespace std;

#include <is/DmException.h>
/**
* 引数から変換した文字列を取得
* @author    Nagoya University
* @date      2020/03/10
* @param     引数1=double
* @return    string
*/
extern "C" vector< vector<string> > UF_TO_CHAR(vector< vector<string> > argList)
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
    // 文字列へと変換
    string a1str = std::to_string(a1);

    ret.push_back(a1str);
    retList.push_back(ret);
  }
  return retList;
}
