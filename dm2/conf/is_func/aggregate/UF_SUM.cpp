#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include <is/DmException.h>
/**
* 全ての行の入力値の平均値を求める
* @author    Nagoya University
* @date      2023/02/22
* @param     引数1=double
* @return    double
*/
extern "C" vector< vector<string> > UF_SUM(vector< vector<string> > argList)
{
  vector< vector<string> > retList;
  double retDouble;
  vector<string> ret;
  
  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    if(argList.at(i).size() > 1){
      //列数が２つ以上の場合、エラーを返す 
      throw CastException("Column is over!");
    }
    try {
      retDouble = retDouble + std::stod(argList.at(i).at(0));
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }
  }

  ret.push_back(std::to_string(retDouble));
  retList.push_back(ret);
  
  return retList;
}
