#include <iostream>
#include <vector>
#include <string>

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

/**
* UNIX時間からミリ秒を取得
* @author    Nagoya University
* @date      2023/02/22
* @param     引数1=long
* @return    int
*/
extern "C" vector< vector<string> > UF_GET_ITS_MICROSECOND(vector< vector<string> > argList)
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
      a1 = std::to_string(itsTime + itsStartTime);
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }

    // ミリ秒は入力された値の下3桁なので
    // 他クラスを使わず、そのまま計算で算出する  
    long int ms;
    std::size_t stolSize;
    try {
      ms = std::stol(a1,&stolSize);
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    }
    if (stolSize != a1.size()) throw CastException("Data type is not match!");

    long int milli = ms % 1000;

    string str = std::to_string(std::abs(milli));

    ret.push_back(str);
    retList.push_back(ret);
  }

  return retList;
}
