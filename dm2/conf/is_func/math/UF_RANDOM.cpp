#include <iostream>
#include <vector>
#include <string>
#include <random>

using namespace std;

/**
* 0.0から1.0の範囲で乱数を求める
* @author    Nagoya University
* @date      2020/03/13
* @param     none
* @return    double
*/
extern "C" vector< vector<string> > UF_RANDOM(vector< vector<string> > argList)
{
  vector< vector<string> > retList;

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;

    // 乱数を求める
    std::random_device seed_gen;
    std::mt19937 engine(seed_gen());
    // 範囲を0.0から1.0の間に設定
    std::uniform_real_distribution<> dist1(0.0, 1.0);
    double retDouble = dist1(engine);

    ret.push_back(std::to_string(retDouble));
    retList.push_back(ret);
  }
  return retList;
}
