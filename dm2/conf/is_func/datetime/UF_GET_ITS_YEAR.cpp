#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <sys/time.h>

using namespace std;

/*1日の秒数(=24*60*60)*/
#define SECONDS_PER_DAY 86400

#include <is/DmException.h>

/*1年からyear年まで（両端を含む）の日数を求める*/
static long long getDays(int year) {
        long long result;
        if(year<0)return -getDays(-year)-366+365;
        else if(year==0)return 0;
        result=year*365;/*1年は基本365日*/
        result+=year/4;/*4で割り切れたら閏年*/
        result-=year/100;/*100で割り切れたら閏年ではない*/
        result+=year/400;/*400で割り切れたら閏年*/
        return result;
}

/*1970年1月1日　00:00:00からの経過秒数を返す*/
long long date2ut(int year,int month,int date,
                int hour,int minute,int second) {
        const int monthDays[13]={
                        0,31,59,90,120,151,181,212,243,273,304,334,365
        };/*その月までの日数の和（累積和）*/
        long long result;
        /*アクセス違反になる不正入力なら0を返す*/
        if(month<1 || month>12)return 0;
        /* year年1月1日 00:00:00までの日数を求める*/
        result=getDays(year-1)-getDays(1969);
        /*秒に変換*/
        result*=SECONDS_PER_DAY;
        /*月の日数を秒に変換して足す*/
        result+=monthDays[month-1]*SECONDS_PER_DAY;
        /*閏年かつ3月以降なら1日足す*/
        if(year%400==0 || (year%4==0 && year%100!=0)) {
                if(month>=3)result+=SECONDS_PER_DAY;
        }
        /*その月の日数を秒に変換して足す*/
        result+=(date-1)*SECONDS_PER_DAY;
        /*時間を足す*/
        result+=hour*60*60;
        /*分を足す*/
        result+=minute*60;
        /*秒を足す*/
        result+=second;
        /*結果を返す*/
        return result;
}

int getUruTime( long int itsTime ){
        long int uruDays[5]={date2ut(2006,1,1,8,59,59),date2ut(2009,1,1,8,59,59),date2ut(2012,7,1,8,59,59),date2ut(2015,7,1,8,59,59),date2ut(2017,1,1,8,59,59)};
        int uruTime=0;

        for(int i=0; i<sizeof(uruDays)/sizeof(*uruDays); i++){
        if( (uruDays[i]*1000) < itsTime ){
          uruTime += 1000;
        }
      }
      return uruTime;
}

/**
* UNIX時間から年を取得
* @author    Nagoya University
* @date      2020/03/10
* @param     引数1=long
* @return    int
*/
extern "C" vector< vector<string> > UF_GET_ITS_YEAR(vector< vector<string> > argList)
{
  vector< vector<string> > retList;
  long int itsStartTime,itsTime;

  // ITS開始時間のUNIX時間を取得
  itsStartTime = date2ut(2004,1,1,0,0,0);
  itsStartTime *= 1000;

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    string a1;
    try {
      itsTime = std::stod(argList.at(i).at(0));
      a1 = std::to_string(itsTime + itsStartTime - getUruTime(itsTime + itsStartTime));

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
