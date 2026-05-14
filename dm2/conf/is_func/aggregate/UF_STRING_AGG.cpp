#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include <is/DmException.h>
/**
* 全ての行の入力値 xが、指定したデリミタdで区切られた文字列に連携
* @author    Nagoya University
* @date      2023/02/22
* @param     引数1=string
* @param     引数2=string
* @return    string
*/
extern "C" vector< vector<string> > UF_STRING_AGG(vector< vector<string> > argList)
{
  vector< vector<string> > retList;
  vector<string> ret;
  string retString,delimiter;

  // 入力された各行に対して操作を行う
  try {

    //引数1とデリミタを取得
    retString.append(argList.at(0).at(0));//引数1
    delimiter = argList.at(0).at(1);      //引数2

    for(int i=1; i<argList.size(); i++){
      //文字列結合
      retString.append(delimiter);
      retString.append(argList.at(i).at(0));
    }
  } catch (invalid_argument) {
    throw CastException("Data type is not match!");
  } catch (out_of_range) {
    throw CastException("Insufficient number of arguments!");
  }

  ret.push_back(retString);
  retList.push_back(ret);
  
  return retList;
}
