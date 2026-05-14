#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <sys/time.h>

using namespace std;

#include <is/DmException.h>
/**
* 入力された年、月、日、時間、分、秒 、ミリ秒からUNIXタイムスタンプを作成する
* @author    Nagoya University
* @date      2020/03/10
* @param     引数1=int 引数2=int 引数3=int 引数4=int 引数5=int 引数6=int 引数7=int
* @return    long
*/
extern "C" vector< vector<string> > UF_MAKE_TIMESTAMP(vector< vector<string> > argList)
{
  vector< vector<string> > retList;

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    string a1, a2, a3, a4, a5, a6, a7;
    try { 
      a1 = argList.at(i).at(0);  // 引数1
      a2 = argList.at(i).at(1);  // 引数2
      a3 = argList.at(i).at(2);  // 引数3
      a4 = argList.at(i).at(3);  // 引数4
      a5 = argList.at(i).at(4);  // 引数5
      a6 = argList.at(i).at(5);  // 引数6
      a7 = argList.at(i).at(6);  // 引数7
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }
 
    // 時間計算用のオブジェクト生成 
    struct timeval tp;
    struct tm t;
    // 一旦現在時刻の情報を取得
    gettimeofday(&tp, NULL);
    try {
      // ミリ秒はtmでは扱えないので個別でtimevalに格納
      long int milli = std::stol(a7);
      milli = milli * 1000;
      tp.tv_usec = milli;
      // 現在時刻でtmを生成
      localtime_r(&tp.tv_sec, &t);

      // 生成したtmに年、月、日、時間、分、秒を設定してUNIX時間(秒)を取得
      int year, month, day, hour, minit, second;
    
      year = std::stoi(a1);
      month = std::stoi(a2);
      day = std::stoi(a3);
      hour = std::stoi(a4);
      minit = std::stoi(a5);
      second = std::stoi(a6);

      t.tm_year = year - 1900;
      t.tm_mon = month - 1;
      t.tm_mday = day;
      t.tm_hour = hour;
      t.tm_min = minit;
      t.tm_sec = second;

      long int unix = mktime(&t);

      // timevalに秒を設定して13桁のUNIX時間を生成
      tp.tv_sec = unix;
      long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;

      string str = std::to_string(ms);
      ret.push_back(str);
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    }

    retList.push_back(ret);
  }

  return retList;

}
