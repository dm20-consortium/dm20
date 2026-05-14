#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include <is/DmException.h>

/**
* 引配列の末尾に要素を追加する
* @author    Nagoya University
* @date      20230/02/22
* @param     引数1=vector(string) 引数2=string
* @return    vector(string)
*/
extern "C" vector< vector<string> > UF_STR_ARRAY_APPEND(vector< vector<string> > argList)
{
  vector< vector<string> > retList;

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    std::string retStr;
    std::string a1, a2;

    try {
      a1 = argList.at(i).at(0);  // 引数1
      a2 = argList.at(i).at(1);  // 引数2
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }

    // 文字列から配列定義の文字列を削除する
    size_t point;
    while((point = a1.find("ARRAY", 0, 5)) != string::npos){
      a1.erase(point,5);
    }
    while((point = a1.find("array", 0, 5)) != string::npos){
      a1.erase(point,5);
    }

    // '['の始まりを探して文字列を切り出す
    a1 = a1.substr(a1.find('[')+1);
    a1 = a1.substr(0, a1.length() - 1);

    if( a1.size() <= 0 ){
      throw CastException("Cannot determine type of empty array!");
    }
    if( a2.find("array[") != -1 || a2.find("ARRAY[") != -1 ){
      throw CastException("Data type is not match!");
    }
    a2 += "]";
    retStr = "[" + a1 + "," + a2;

    ret.push_back(retStr);
    retList.push_back(ret);
  }
  return retList;
}
