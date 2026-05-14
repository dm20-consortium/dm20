#include <iostream>
#include <vector>
#include <string>
#include <regex>

using namespace std;

#include <is/DmException.h>


bool getMatch(string a1, string a2) {
	std::regex re(a2);
	std::smatch m;

	if (std::regex_match(a1, m, re)) {
		return true;
	}else{
		return false;
	}
}
/**
* パターンマッチング演算子の代替
* @author    Nagoya University
* @date      2023/02/23
* @param     引数1=string
* @param     引数2=string
* @return    bool
*/
extern "C" vector< vector<string> > UF_LIKE(vector< vector<string> > argList)
{
  vector< vector<string> > retList;

  // 入力された各行に対して操作を行う
	for(int i=0; i<argList.size(); i++)
	{
		vector<string> ret;
		string a1, a2;
		int pos1=0;
		int pos2=0;
		try {
			//引数を取得
			a1 = argList.at(i).at(0); //引数1
			a2 = argList.at(i).at(1); //引数2
		} catch (invalid_argument) {
			throw CastException("Data type is not match!");
		} catch (out_of_range) {
			throw CastException("Insufficient number of arguments!");
		}
		while( pos1 != string::npos || pos2 != string::npos){
			pos1 = a2.find("_");  // 検索文字列が見つかった位置
			if (pos1 != std::string::npos) {
				a2.replace(pos1, 1, "."); // 任意の１文字
			}
			pos2 = a2.find("%");  // 検索文字列が見つかった位置
			if (pos2 != std::string::npos) {
				a2.replace(pos2, 1, ".*"); // ワイルドカード
			}
		}
		bool retBool = getMatch(a1, a2);

		ret.push_back(to_string(retBool));
		retList.push_back(ret);
	}
  
  return retList;
}
