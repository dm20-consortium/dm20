#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include <is/DmException.h>

/**
* 引配列の末尾に要素を追加する
* @author    Nagoya University
* @date      2020/03/17
* @param     引数1=vector(double) 引数2=double
* @return    vector(double)
*/
extern "C" vector< vector<string> > UF_ARRAY_APPEND(vector< vector<string> > argList)
{
  vector< vector<string> > retList;

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    std::string retStr;
    std::string a1;
    double a2;

    try {
      a1 = argList.at(i).at(0);  // 引数1
      a2 = std::stod(argList.at(i).at(1)); //引数2
      
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

    int first = 0;
    int last = a1.find_first_of(",");
    vector<double> retDouble;

    //文字列をdoubleに変換
    while (first < a1.size()) {
      std::string subStr(a1, first, last - first);
      retDouble.push_back(std::stod(subStr));

      if(a1.size() == subStr.size()){
        break;
      }

      first = last + 1;
      last = a1.find_first_of(",", first);

      if (last == std::string::npos) {
        last = a1.size();
      }
    }

    //doubleをstringに変換
    a1 = "";
    for(int i=0; i<retDouble.size(); i++){
      a1 = a1 + std::to_string(retDouble[i]) + ",";
    }
    a1.pop_back();

    retStr = "[" + a1 + "," + std::to_string(a2) + "]";
    ret.push_back(retStr);
    retList.push_back(ret);
  }
  return retList;
}
