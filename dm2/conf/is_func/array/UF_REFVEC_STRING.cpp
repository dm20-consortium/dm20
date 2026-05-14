#include <iostream>
#include <vector>
#include <string>

#include <is/StringUtil.h>

using namespace std;
using namespace IS;

// 関数名をUF_XXX(XXXは任意)とし「extern "C"」と戻り値、引数(argList[row][col])は固定
extern "C" vector<vector<string>> UF_REFVEC_STRING(vector<vector<string>> argList)
{
  //printf("UF_REFVEC called!\n");
  vector<vector<string>> retList; // ISに返却する結果2次元配列(retList[row][col])
  StringUtil stringUtil;
  vector<string> delStr = { "[","]" };

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    string value = argList.at(i).at(0);
    int	findIdx = std::stoi(argList.at(i).at(1)); 
    value = stringUtil.getDeleteStrings(value, delStr);

    int idx = 0;
    for (string val : stringUtil.split(value, ",")) {
      idx++;
      if (idx == findIdx) {
        ret.push_back(val);
        break;
      }
    }
    retList.push_back(ret);
  }
  //printf("UF_REFVEC end!\n");
  return retList;
}
