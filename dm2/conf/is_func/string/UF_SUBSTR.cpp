#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include <is/DmException.h>
/**
* 引数1の引数2で指定した位置から引数3の値まで文字列を抽出する
* @author    Nagoya University
* @date      2020/03/13
* @param     引数1=string, 引数2=int, 引数3=int
* @return    string
*/
extern "C" vector< vector<string> > UF_SUBSTR(vector< vector<string> > argList)
{
  vector< vector<string> > retList;

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    string a1, a2, a3;
    int  a2Int, a3Int;
    try {
      a1 = argList.at(i).at(0); // 引数1
      a2 = argList.at(i).at(1); // 引数2
      a2Int = std::stoi(a2);
      if (argList.at(i).size() >= 3) {
        a3 = argList.at(i).at(2); // 引数3
        a3Int = std::stoi(a3);
      }
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }

    // 文字列を抽出する
    // 第3パラメータが省略された場合は最後まで表示を行う
    // 表示開始位置により文字列長は変化する
    string retStr;
    // 第3パラメータが付与されていたか
    if (a3.length() > 0) {
      // 第3パラメータと第2パラメータがどちらも0より大きい場合
      if (a3Int > 0 && a2Int > 0 ) {
        // 第1パラメータの長さが第2パラメータ以上の場合は
        // 第2パラメータ - 1の地点から 第3パラメータ分の長さの
        // 文字列を設定する
        // 第1パラメータの長さが第2パラメータよりも小さい場合は
        // 空文字列を設定する
        if ((int)a1.length() >= a2Int) {
          retStr = a1.substr(a2Int - 1, a3Int);
        } else {
          retStr = "";
        }
      // 第3パラメータが0より大きいが、第2パラメータが0以下の場合
      } else if (a3Int > 0 && 0 >= a2Int) {
        // 一度第2パラメータの絶対値を求めて、第3パラメータと比較する
        a2Int = abs(a2Int);
        // 第3パラメータの方が第2パラメータに1加えた値より大きい場合
        // 先頭から第3パラメータの値から第2パラメータに1加えた値を
        // 引いたサイズで文字列を切り出す
        if (a3Int > a2Int + 1) {
          retStr = a1.substr(0, a3Int-(a2Int+1));
        } else {
          retStr = "";
        }
      // 第3パラメータに負の値が設定されていた場合は例外を返す 
      } else if (0 > a3Int) {
        throw CastException("negative substring length not allowed");
      } else {
        retStr = "";
      }
    // 第3パラメータが付与されていない場合
    } else {
      // 第1パラメータの長さと第2パラメータが正の値かつ
      // 値を比較して第1パラメータの長さが大きい場合は文字列を切り出す
      if ((int)a1.length() >= a2Int && a2Int > 0) {
        retStr = a1.substr(a2Int-1);
      // 第2パラメータが負の値の場合は、入力された文字列を
      // そのまま返す
      } else if (0 > a2Int) {
        retStr = a1;
      } else {
        retStr = "";
      }
    }

    ret.push_back(retStr);
    retList.push_back(ret);
  }
  return retList;
}
