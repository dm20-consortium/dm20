#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

#include <is/DmException.h>
/**
* 引数1を小数点以下の引数2の位で四捨五入した値を求める
* @author    Nagoya University
* @date      2020/03/13
* @param     引数1=double, 引数2=int
* @return    double
*/
extern "C" vector< vector<string> > UF_ROUND(vector< vector<string> > argList)
{
  vector< vector<string> > retList;

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    double a1;
    string a2;
    int a2Int;
    try {
      a1 = std::stod(argList.at(i).at(0)); // 引数1
      // 引数2は省略可能なため、サイズを見て付与されていれば取得する
      if (argList.at(i).size() >= 2) {
        a2 = argList.at(i).at(1); // 引数2
        if (a2.find(".") == std::string::npos) {
          a2Int = std::stoi(a2);
        } else {
          throw CastException("Decimal digits not allowed!");
        }
      }
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }

    // 四捨五入を行う
    double retDouble;
    // 引数1が整数か
    if (std::floor(a1) == a1) {
      // 引数2が正の値の場合、もしくは引数2が設定されていない場合
      // 引数１をそのまま返す
      // 負の値の場合、10の引数2の値 乗を掛けて
      // 四捨五入を行う
      if (a2.length() == 0 ||  a2Int > 0) {
        retDouble = a1;
      } else {
        retDouble = a1 * pow(10, a2Int);
        retDouble = round(retDouble);
        retDouble /= pow(10, a2Int);
      }
    } else {
      // 引数2がない、もしくは0が指定された場合は
      // そのまま四捨五入を行う
      if (a2.length() == 0 || a2Int == 0) {
        retDouble = round(a1);
      } else {
        // 引数2が0ではない場合、10 の 引数2の値 乗を掛け
        // 小数点をずらしてから四捨五入を行う
        retDouble = a1 * pow(10, a2Int);
        retDouble = round(retDouble);
        retDouble /= pow(10, a2Int);
      }
    }

    ret.push_back(std::to_string(retDouble));
    retList.push_back(ret);
  }
  return retList;
}
