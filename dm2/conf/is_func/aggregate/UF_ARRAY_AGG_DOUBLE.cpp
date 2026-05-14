#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include <is/DmException.h>
/**
* 全ての行の入力値を収集して配列に格納する
* @author    Nagoya University
* @date      2023/02/21
* @param     引数1=double
* @return    vector<double>
*/
extern "C" vector< vector<string> > UF_ARRAY_AGG_DOUBLE(vector< vector<string> > argList)
{
  vector< vector<string> > retList;
  vector<string> ret;
  std::string str;

  if(argList.size() == 0){
    throw CastException("Data is none!");
  }
  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    double a1;
    if(argList.at(i).size() > 1){
      //列数が２つ以上の場合、エラーを返す 
      throw CastException("Column is over!");
    }
    try {
      a1 = std::stod(argList.at(i).at(0)); // 引数1
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }
    str += std::to_string(a1)+",";
  }
  str = "[" + str.substr(0,str.size()-1) + "]";
  ret.push_back(str);

  retList.push_back(ret);
  return retList;
}
