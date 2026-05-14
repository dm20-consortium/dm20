#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

// 関数名をUF_XXX(XXXは任意)とし「extern "C"」と戻り値、引数(argList[row][col])は固定
extern "C" vector<vector<string>> UF_XY_DISTANCE(vector<vector<string>> argList)
{
  //printf("UF_XY_DISTANCE called!\n");
  vector<vector<string>> retList; // ISに返却する結果2次元配列(retList[row][col])

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    double x1, y1, x2, y2;
    x1 = std::stod(argList.at(i).at(0)); //ユーザ関数引数に指定されたi行目の引数1番目を取得
    y1 = std::stod(argList.at(i).at(1)); //ユーザ関数引数に指定されたi行目の引数2番目を取得
    x2 = std::stod(argList.at(i).at(2)); //ユーザ関数引数に指定されたi行目の引数3番目を取得
    y2 = std::stod(argList.at(i).at(3)); //ユーザ関数引数に指定されたi行目の引数4番目を取得
    double retDouble = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    // std::cout << x1 << "," << y1 << "," << x2 << "," << y2 << "," << retDouble << std::endl;

    ret.push_back(std::to_string(retDouble));
    retList.push_back(ret);
  }
  //printf("UF_XY_DISTANCE end!\n");
  return retList;
}
