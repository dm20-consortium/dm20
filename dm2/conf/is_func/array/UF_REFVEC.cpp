#include <iostream>
#include <vector>
#include <string>

#include <is/StringUtil.h>

using namespace std;
using namespace IS;

// 関数名をUF_XXX(XXXは任意)とし「extern "C"」と戻り値、引数(argList[row][col])は固定
extern "C" vector<vector<string>> UF_REFVEC(vector<vector<string>> argList)
{
  //printf("UF_REFVEC called!\n");
  vector<vector<string>> retList; // ISに返却する結果2次元配列(retList[row][col])
  StringUtil stringUtil;
  vector<string> delStr = { "[","]" };

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
	vector<double> area_propertyList;
	string value = argList.at(i).at(0);
	value = stringUtil.getDeleteStrings(value, delStr);
	try {
		for (string val : stringUtil.split(value, ",")) {
			//cout << "val:" << val << endl;
			area_propertyList.push_back(std::stod(val));
		}
	}
	catch (std::invalid_argument) {
		throw;
	}
    int		idx = std::stoi(argList.at(i).at(1)); //ユーザ関数引数に指定されたi行目の引数2番目を取得

    ret.push_back(std::to_string(area_propertyList.at(idx)));
    retList.push_back(ret);
  }
  //printf("UF_REFVEC end!\n");
  return retList;
}
