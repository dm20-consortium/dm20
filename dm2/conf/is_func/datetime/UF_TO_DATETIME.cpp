#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <sys/time.h>

using namespace std;

#include <is/DmException.h>
/**
* UNIX時間から日付時間の文字列を取得
* @author    Nagoya University
* @date      2020/03/10
* @param     引数1=long
* @return    string
*/
extern "C" vector< vector<string> > UF_TO_DATETIME(vector< vector<string> > argList)
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

    // tmでは秒までしか扱えないため、ミリ秒を扱うために
    // timevalも使用する
    struct timeval tp;
    struct tm t;
    // 現在時刻でtimevalを生成し、引数から秒とミリ秒を求めて設定を行う
    gettimeofday(&tp, NULL);
    long int ms = std::stol(a1);
    long int milli = ms % 1000;
    milli = milli * 1000;
    tp.tv_sec = ms / 1000;
    tp.tv_usec = milli;

    // 設定されたtimevalの秒からtmを生成
    localtime_r(&tp.tv_sec, &t);

    // tmとtimevalから値を取得して、日付時間の文字列を生成
    string str;
    str = std::to_string(t.tm_year + 1900);
    str += "/" + std::to_string(t.tm_mon + 1);
    str += "/" + std::to_string(t.tm_mday);
    str += " " + std::to_string(t.tm_hour);
    str += ":" + std::to_string(t.tm_min);
    str += ":" + std::to_string(t.tm_sec);

    // ミリ秒だけは3桁表示になるよう0埋めを行う
    string msstr = std::to_string(std::abs(tp.tv_usec) / 1000);
    if (msstr.length() == 1)
    {
      str += ".00" + msstr;
    }
    else if (msstr.length() == 2)
    {
      str += ".0" + msstr;
    } else
    {
      str += "." + msstr;
    }

    ret.push_back(str);
    retList.push_back(ret);
  }

  return retList;
}
