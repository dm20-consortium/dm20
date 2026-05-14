#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <sys/time.h>

using namespace std;

#include <is/DmException.h>
/**
* UNIX時間から年を取得
* @author    Nagoya University
* @date      2020/03/10
* @param     引数1=long
* @return    int
*/
extern "C" vector< vector<string> > UF_GET_YEAR(vector< vector<string> > argList)
{
  vector< vector<string> > retList;

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    string a1;
    try {
      a1 = argList.at(i).at(0); // 引数1
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }
  
    // timevalとtmを利用して時間を計算する
    struct timeval tp;
    struct tm t;
    // 一度現在時刻でtimevalを生成してから
    // 引数の時間を設定
    gettimeofday(&tp, NULL);

    long int ms;
    std::size_t stolSize;
    try {
      ms = std::stol(a1,&stolSize);
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    }
    if (stolSize != a1.size()) throw CastException("Data type is not match!");

    long int milli = ms % 1000;
    milli = milli * 1000;
    tp.tv_sec = ms / 1000;
    tp.tv_usec = milli;
    // timevalに設定した秒までの時間からtmを生成
    localtime_r(&tp.tv_sec, &t);

    // tmより年を取得して、1900を足すことで
    // 現在の年を求める
    int year = t.tm_year + 1900;

    string str = to_string(year);

    ret.push_back(str);
    retList.push_back(ret);
  }

  return retList;
}
