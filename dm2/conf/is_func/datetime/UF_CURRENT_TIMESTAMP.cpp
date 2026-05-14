#include <iostream>
#include <vector>
#include <string>
#include <sys/time.h>

using namespace std;

/**
* 現在時刻をunix時間にて取得
* @author    Nagoya University
* @date      2020/03/10
* @param     none
* @return    long
*/
extern "C" vector< vector<string> > UF_CURRENT_TIMESTAMP(vector< vector<string> > argList)
{
  
  vector< vector<string> > retList;

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;

    // 現在時刻の取得を行う
    struct timeval tp;
    gettimeofday(&tp, NULL);
    // 現在時刻の秒に1000をかけて桁を13桁にし、
    // マイクロ秒を1000で割り、ミリ秒にして加算することで
    // 現在時刻のミリ秒を求める
    long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    string str = std::to_string(ms);

    ret.push_back(str);
    retList.push_back(ret);
  }

  return retList;
}
