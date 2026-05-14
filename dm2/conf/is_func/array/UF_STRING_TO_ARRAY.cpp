#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include <is/DmException.h>
/**
* デリミタを使用して、文字列を配列の要素に分割
* @author    Nagoya University
* @date      2023/02/22
* @param     引数1=string
* @param     引数2=string
* @return    svector<string>
*/
extern "C" vector< vector<string> > UF_STRING_TO_ARRAY(vector< vector<string> > argList)
{
  vector< vector<string> > retList;
  string a1;
  string delimiter;

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    try {

      //引数とデリミタを取得
      a1 = argList.at(i).at(0);       //引数1
      delimiter = argList.at(i).at(1); //引数2

      int first = 0;
      int last = a1.find_first_of(delimiter);
      vector<string> ret, ret2;
 
      while (first < a1.size()) {
        std::string subStr(a1, first, last - first);
        ret.push_back(subStr);
		if (a1.size() == subStr.size()) break;
 
        first = last + 1;
        last = a1.find_first_of(delimiter, first);
 
        if (last == std::string::npos) {
          last = a1.size();
        }
      }
      string retStr = "[";
      for (auto itr = ret.begin(); itr != ret.end(); ++itr) {
    	  if (itr != ret.begin()) retStr += ",";
    	  retStr += *itr;
      }
      retStr += "]";
      ret2.push_back(retStr);
      retList.push_back(ret2);

    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }

  }

  return retList;
}
