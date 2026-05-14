#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

#include <is/DmException.h>
/**
* 全ての行の入力値の最大値を求める
* @author    Nagoya University
* @date      2023/02/21
* @param     引数1=double
* @return    double
*/
extern "C" vector< vector<string> > UF_MAX(vector< vector<string> > argList)
{
  vector< vector<string> > retList;
  vector<double> a1;
  vector<string> ret;
  
  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    if(argList.at(i).size() > 1){
      //列数が２つ以上の場合、エラーを返す 
      throw CastException("Column is over!");
    }
    try {
      a1.push_back(std::stod(argList.at(i).at(0))); //引数1
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }
  }
    // 最大値を求める
  double retDouble = *max_element(begin(a1), end(a1));

  ret.push_back(std::to_string(retDouble));
  retList.push_back(ret);
  
  return retList;
}
