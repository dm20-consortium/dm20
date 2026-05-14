#include <iostream>
#include <vector>
#include <string>

using namespace std;

// 関数名をUF_XXX(XXXは任意)とし「extern "C"」と戻り値、引数(argList[row][col])は固定
extern "C" vector<vector<string>> UF_TO_STRING(vector<vector<string>> argList)
{
  //printf("UF_TO_STRING called!\n");
  vector<vector<string>> retList; // ISに返却する結果2次元配列(retList[row][col])
  
  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    string retString;
    retString = argList.at(i).at(0); //ユーザ関数引数に指定されたi行目の引数1番目を取得
    ret.push_back(retString);
    retList.push_back(ret);
  }
  //printf("UF_TO_STRING end!\n");
  return retList;
}
