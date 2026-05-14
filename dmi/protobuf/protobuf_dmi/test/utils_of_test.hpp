/**                                                                                                  
 * @file utils_of_test.hpp
 *
 * @brief テストプロジェクトで活用する関数・マクロ・ライブラリなどを定義している
 *
 */


#include <iostream>
#include <chrono>
#include <string>
#include <fstream>
#include <dirent.h>
#include <signal.h>
#include <thread>
#include <array>
#include <functional>
#include <is/DmManager.h>
#include <is/Tuple.h>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/asio.hpp>

#include "protobuf_dmi/protobuf_dmi.hpp"   

using namespace std;

/// 許容される受信時間
#ifndef ACCEPTABLE_RECEIVE_TIME_MSEC
#define ACCEPTABLE_RECEIVE_TIME_MSEC (5000)
#endif

/// 待機時間
#ifndef TEST_SPIN_MSEC
#define TEST_SPIN_MSEC 10000
#endif

/// 送信サービスの送信間隔
#ifndef SEND_INTERVAL_MSEC
#define SEND_INTERVAL_MSEC 100
#endif

/// 片付けるのため待ち時間
#ifndef WAITING_PERIOD_MSEC
#define WAITING_PERIOD_MSEC 5000
#endif

/// 生成したい物標情報の個数
#ifndef TEST_OBJ_NUM
#define TEST_OBJ_NUM 4
#endif

/// 生成したい感知情報配列の長さ
#ifndef TEST_MSG_LEN
#define TEST_MSG_LEN 5
#endif


/// 標準偏差を計算するウィンドウ
#ifndef CHECK_POINT
#define CHECK_POINT 20
#endif

/// スループットを計算する頻繁度
#ifndef THROUGHPUT_CALCULATE_TIME_MSEC
#define THROUGHPUT_CALCULATE_TIME_MSEC 1000
#endif

/**
 * @brief ResultSetからunsigned long long型のデータを取得
 *
 */
unsigned long long getULL(ResultSet rs, int idx)
{
  return stoull(rs.getString(idx));
}

/**
 * @brief ResultSetからunsigned long long型の配列を取得
 *
 */
vector<unsigned long long> getVectorULL(ResultSet rs, int idx)
{
  StringUtil su;
  string vals = rs.getString(idx);
  vector<string> delStr = {"[", "]"};
  vector<unsigned long long> ret;
  su.deleteStrings(vals, delStr);
  for(string val : su.split(vals, ","))
  {
    ret.push_back(stoull(val));
  }
  return ret;
}


/**
 * @brief 今の時間を取得し、送信サービスと同じタイムスタンプフォーマットに変換
 *
 */
uint64_t getTimeStamp()
{
  auto st = chrono::system_clock::now();
  auto st_nsec = st.time_since_epoch().count();
  return static_cast<uint64_t>(st_nsec / 1000000.0) - 1072915195000;
}

/**
 * @brief プロセス番号を指定して、渡したp_nameにプロセス名前を返す
 *
 */
bool readProcessName(int pid, string& p_name)
{
  ifstream commFile("/proc/" + to_string(pid) + "/comm");
  if(commFile.is_open())
  {
    getline(commFile, p_name);
    commFile.close();                                                                      
    return true;
  } else { 
    return false;
  }
}


/**
 * @brief 指定する名前をつけられた全部プロセスの番号を返す
 *
 */
vector<int> findPIDsByName(const string& p_name)
{
  vector<int> pids;
  DIR* dir = opendir("/proc");
  struct dirent* entry;
  while((entry = readdir(dir)) != nullptr)
  {
    if(entry->d_type == DT_DIR)
    {
      string dirName = entry->d_name;
      if(all_of(dirName.begin(), dirName.end(), ::isdigit))
      {
        int pid = stoi(dirName);
        string currentPName;
        if(readProcessName(pid, currentPName) && currentPName == p_name)
        {
          pids.push_back(pid);
        }
      }
    }
  }
  closedir(dir);
  return pids;
}


/**
 * @brief 指定名前のプロセスを全部中止する
 *
 * @note Linux kernelでプロセスの情報にcommに格納しているのはプロセス名前ですが、\n
 *      格納できる最大長さはTASK_COMM_LEN(16)ですので、\n
 *      ここで入力した文字列の前15 bytesのみを使い、検索を行います
 *
 */
bool killProcessesByName(const string p_name)
{
  string p_n_short = p_name;
  if(p_n_short.length() > 15)
  {
    p_n_short = p_n_short.substr(0,15);
  }
  
  bool success = true;
  vector<int> pids = findPIDsByName(p_n_short);
  while(pids.size())
  {
    for(int pid : pids)
    {
      if(kill(pid, SIGKILL) != 0)
      {
        cerr << "Unable to kill process with PID " << pid << endl;
        success = false;
      }
      else
      {
        // OK
      }
    }
    this_thread::sleep_for(chrono::milliseconds(WAITING_PERIOD_MSEC));
    pids = findPIDsByName(p_n_short);
  }

  return success;
}


