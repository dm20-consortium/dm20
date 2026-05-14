#include <iostream>
#include <vector>
#include <string>

using namespace std;

std::string replaceString(std::string str, std::string findStr, std::string replaceStr); 
// 関数名をUF_XXX(XXXは任意)とし「extern "C"」と戻り値、引数(argList[row][col])は固定
extern "C" vector<vector<string>> UF_IN(vector<vector<string>> argList)
{
  //printf("UF_IN called!\n");
  vector<vector<string>> retList; // ISに返却する結果2次元配列(retList[row][col])

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    string val, lst;
    val = argList.at(i).at(0); //ユーザ関数引数に指定されたi行目の引数1番目を取得
    lst = argList.at(i).at(1); //ユーザ関数引数に指定されたi行目の引数2番目を取得
    lst = replaceString(lst, "[", ",");
    lst = replaceString(lst, "]", ",");
    bool retBool = false;
    if (lst.find("," + val + ",") != string::npos) {
      retBool = true;
    }
   
    ret.push_back(std::to_string(retBool));
    retList.push_back(ret);
  }
  //printf("UF_IN end!\n");
  return retList;
}
std::string replaceString(std::string str, std::string findStr, std::string replaceStr) {
  std::string::size_type  Pos( str.find( findStr ) );
  while (Pos != string::npos) {
    str.replace(Pos, findStr.length(), replaceStr);
    Pos = str.find(findStr, Pos + replaceStr.length());
  }
  return str;
}
