#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <sys/time.h>

using namespace std;

#include <is/DmException.h>
/**
* UNIX時間から日付を取得
* @author    Nagoya University
* @date      2020/03/10
* @param     引数1=long
* @return    string
*/
extern "C" vector< vector<string> > UF_GET_DATE(vector< vector<string> > argList)
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
    long int ms = std::stol(a1);
    long int milli = ms % 1000;
    milli = milli * 1000;
    tp.tv_sec = ms / 1000;
    tp.tv_usec = milli;
    // timevalに設定した秒までの時間からtmを生成
    localtime_r(&tp.tv_sec, &t);

    // tmとtimevalから値を取得して、年月の文字列を生成
    string str;
    str = std::to_string(t.tm_year + 1900);
    str += "/" + std::to_string(t.tm_mon + 1);
    str += "/" + std::to_string(t.tm_mday);

    ret.push_back(str);
    retList.push_back(ret);
  }

  return retList;
}
