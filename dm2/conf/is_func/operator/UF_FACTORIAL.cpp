#include <iostream>
#include <vector>
#include <string>
#include <limits.h>

using namespace std;

#include <is/DmException.h>
/**
* 階乗を計算する
* @author    Nagoya University
* @date      2020/03/13
* @param     引数1=long
* @return    long
*/
extern "C" vector< vector<string> > UF_FACTORIAL(vector< vector<string> > argList)
{
  vector< vector<string> > retList;

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    string a1;
    long a1Long;
    size_t strSize;
    try {
      a1 = argList.at(i).at(0); // 引数1
      a1Long = stol(a1, &strSize);
      if (strSize != a1.length()) {
        throw CastException("Data type is not match!");
      }
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }

    // 階乗を計算
    int fact = 1;
    for(int j=1; j<=a1Long; ++j) {
      if (fact > INT_MAX / j) {
        throw CastException("value overflows numeric format");
        break;
      } else {
        fact = fact * j;
      }
    }

    ret.push_back(std::to_string(fact));
    retList.push_back(ret);
  }
  return retList;
}
