#include <iostream>
#include <vector>
#include <string>
#include <sys/time.h>
#include <sstream>
#include <iomanip>

using namespace std;

#include <is/DmException.h>

/*1日の秒数(=24*60*60)*/
#define SECONDS_PER_DAY 86400

//閏秒の定義
#define URUBYOU_DAY1 20060101085959
#define URUBYOU_DAY2 20090101085959
#define URUBYOU_DAY3 20120701085959
#define URUBYOU_DAY4 20150701085959
#define URUBYOU_DAY5 20170101085959

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
        
	std::ostringstream sout;
	std::string inputTime;
	long uruDays[5]={URUBYOU_DAY1,URUBYOU_DAY2,URUBYOU_DAY3,URUBYOU_DAY4,URUBYOU_DAY5};

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

	//閏秒の計算
    sout << std::setfill('0') << std::setw(2) << month; //月
	sout << std::setfill('0') << std::setw(2) << date; //日
	sout << std::setfill('0') << std::setw(2) << hour; //時
	sout << std::setfill('0') << std::setw(2) << minute; //分
	sout << std::setfill('0') << std::setw(2) << second; //秒
    inputTime = std::to_string(year) + sout.str();
    for(int i=0; i<sizeof(uruDays)/sizeof(*uruDays); i++){
    	if(uruDays[i] < std::stod(inputTime) ){
    		second++;
    	}
    }
	/*秒を足す*/
	result+=second;

	/*結果を返す*/
	return result;
}

/**
* 現在時刻をITS時間にて取得
* @author    Nagoya University
* @date      2023/02/22
* @param     none
* @return    long
*/
extern "C" vector< vector<string> > UF_CURRENT_ITS_TIMESTAMP(vector< vector<string> > argList)
{
  
  vector< vector<string> > retList;
  long int itsStartTime,itsTime;
  vector<string> ret;
  
  string str;
  int a1, a2, a3, a4, a5, a6, a7;

  // ITS開始時間のUNIX時間を取得
  itsStartTime = date2ut(2004,1,1,0,0,0);
  itsStartTime *= 1000;

  // 現在時刻の取得を行う
  struct timeval tp;
  gettimeofday(&tp, NULL);
  struct tm *now = localtime(&tp.tv_sec);
  //std::cout << now->tm_year+1900 << now->tm_mon+1 << now->tm_mday << now->tm_hour << now->tm_min << now->tm_sec << tp.tv_usec << std::endl;
  itsTime = date2ut(now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
  itsTime *= 1000;
  itsTime += int(tp.tv_usec / 1000);


  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    try {
      string str = std::to_string( itsTime - itsStartTime);
      ret.push_back(str);
      retList.push_back(ret);

    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }
  }

  return retList;
}
