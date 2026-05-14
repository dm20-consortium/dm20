/**
 * @file test_combine.cpp
 * @brief 結合テスト
 *
 */


#include <gtest/gtest.h>
#include "utils_of_test.hpp"


/**
 * @brief DM起動していない場合、protobuf_dmiが接続失敗を送信するか検証する
 *
 *
 */
TEST(CombineTest, ConnectionFailTest)
{
  // set up
  protobuf_dmi::Uploader::SharedPtr uploader;

  // action
  // verification
  EXPECT_ANY_THROW(uploader.reset(new protobuf_dmi::Uploader(
    {
      "127.0.0.1",
      9001,
      "dm2sampleuser",
      "dm2samplepassword",
      1
    })));
  // teardown
}

namespace fs = boost::filesystem;
/// パフォーマンスタイプの羅列

const vector<string> perf_types = {"average", "standard_deviation", "throughput"};
/**
 * @brief 指定するフォルダーに格納しているファイルの個数を返す
 *
 */
int getFolderFilesNum(string f_name)
{
  int files_num = 0;
  for( const auto& e : boost::make_iterator_range(fs::directory_iterator(f_name), { }))
  {
    if (! fs::is_directory(e))
    {
      files_num++;
    }
  }
  return files_num;
}

/**
 * @brief パフォーマンスデータファイルを削除すべきかを判定する。\n
 *       削除すべきの条件は、3つのパフォーマンスタイプ名前をつけているフォルダーで、
 *       ファイル個数が(パフォーマンス個数 * データタイプ数 * 起動方法数 * SUPPORT_TYPES個数)に超える
 * 
 */
bool checkIfClear()
{
  unsigned int total_files_num = 0;
  string f_name = "/tmp/dmi_perf_data/";

  if(fs::exists("/tmp/dmi_perf_data/"))
  {
    for(auto t : perf_types)
    {
      string t_s = f_name + t;
      if(fs::exists(t_s))
      {
        total_files_num += getFolderFilesNum(t_s);
      } else {
        cout << "error when access " << t_s << ", Not EXISTS!!!" << endl;
      }
    }
  }
   //                  パフォーマンス個数 * データタイプ数 * 起動方法数 * SUPPORT_TYPES個数
  return (total_files_num >= (perf_types.size()*(3)*(2)*(2)));
}

/**
 * @brief 前回のパーフォーマンスデータを処理する 
 *
 * @note 追加書き込みでログファイルを開くので、今回のデータを影響されないため、
 *      前回のデータを廃棄にします
 */
void clearPerfHistory()
{
  string f_name = "/tmp/dmi_perf_data/";

  if(fs::exists("/tmp/dmi_perf_data/"))
  {
    for(auto t : perf_types)
    {
      string t_s = f_name + t;
      if(fs::exists(t_s))
      {
        for( const auto& e : boost::make_iterator_range(fs::directory_iterator(t_s), { }))
        {
          if (! fs::is_directory(e))
          {
            try{
              fs::remove(e.path());
            }
            catch (fs::filesystem_error& ex)
            {
              cout << ex.what() << endl;
              throw;
            }
          }
        }
      }
    }
  }
}


/// 受信した回数のカウンター
static int diff_n;
/// 受信時間の標準偏差を計算するため、一時的にCHECK_POINT個受信時間を格納する配列
static vector<long> v_diff;
/// 平均受信時間のキャッシュ、CHECK_POINT回後にファイルに書き込まれます
static vector<pair<long, float>> avg_cache;
/// 前回の平均受信時間を格納する
float avg;
/// 今回のテストデータタイプを指示する文字列
static string test_name;

/**
 * @brief 標準偏差を計算・記録する
 *
 */
void calStandardDeviation(vector<long> v)
{
  float sum_ = 0.0;
  long sum = 0;
  float _avg = 0.0;
  
  for(auto i : v)
  {
    sum_ += i;
  }

  _avg = sum_ / v.size();

  for(auto i : v)
  {
    sum += (i - _avg) * (i - _avg);
  }
  float sd = 0.0;
  sd = sqrt(static_cast<float>(sum) / v_diff.size());
  
  // write into files 
  uint64_t now = getTimeStamp();
  string file_name = "/tmp/dmi_perf_data/standard_deviation/" + test_name + ".csv";
  ofstream sd_data_log(file_name.c_str(), ofstream::app);
  sd_data_log << now << "," << sd << endl;
}

/**
 * @brief 平均受信時間を計算・記録する。
 * @note CHECK_POINT回計算すると、標準偏差を計算する関数を呼ぶ。
 *
 */
void calAverageReceiveTime(long new_diff)
{
  
  avg = (avg * diff_n + new_diff ) / (diff_n + 1);

  avg_cache.push_back({diff_n++, avg});
  
  // check point: will write avg_cache into files, 
  //              then calculate and save the standard deviation
  if(v_diff.size() < CHECK_POINT)
  {
    v_diff.push_back(new_diff);
  }else
  {
    // to avoid frequent file access periodically write
    string file_name = "/tmp/dmi_perf_data/average/" + test_name + ".csv" ;
    ofstream avg_data_log(file_name.c_str(), ofstream::app);
    for(auto p : avg_cache)
    {
      // times,avg
      avg_data_log << p.first << "," << p.second << endl;       
    }
    avg_cache.clear();

    // standard deviation
    calStandardDeviation(v_diff);
    v_diff.clear();
  }
}

/// 前回スループットを計算された時刻を記録
static uint64_t check_ts;
/// 前回まで受信した回数を記録
static int last_diff_n;

/**
 * @brief スループットを計算・記録する
 *
 */
void calThroughputs(long now)
{
  float t;
  if((now - check_ts) > THROUGHPUT_CALCULATE_TIME_MSEC)
  {
    t = static_cast<float>(diff_n - last_diff_n) / ((now - check_ts)/1000.0);
    
    last_diff_n = diff_n;
    check_ts = now;
    
    string file_name = "/tmp/dmi_perf_data/throughput/" + test_name + ".csv" ;
    ofstream throughput_data_log(file_name.c_str(), ofstream::app);
    throughput_data_log << now << "," << t << endl; 
  }
}


/**
 * @brief コールバック関数をせめて一回呼ばれることをチェックする
 * @note 各コールバック関数の終了時にtrueに設定されます
 *
 */
static bool callback_check;


/**
 * @brief 渡した結果集合に格納したセンサー情報が認識システムの模擬データと一致しているかを検証する 
 *      
 *
 */
void verifySensorInfo(ResultSet rs)
{
  const uint64_t _cb_time = getTimeStamp();
  long data_ts;
  ResultSetMetaData rsmd = rs.getResultSetMetaData();
  EXPECT_EQ(71, rsmd.getColumnSize());
  int sensor_idx = 0;
  while(rs.next())
  {
    int idx = 0;
    int v = 0;
    // conf.rsu_id
    EXPECT_EQ(305419896, getULL(rs, idx));
    // conf.sensor_id
    EXPECT_EQ(1, rs.getInt(++idx));
    // type
    EXPECT_EQ(0, rs.getInt(++idx));
    // SensorLocation
    // -> SystemCode
    EXPECT_EQ(6668, rs.getInt(++idx));
    // -> latitude
    EXPECT_EQ(v++, rs.getInt(++idx));
    // -> longitude
    EXPECT_EQ(v++, rs.getInt(++idx));
    // -> altitude
    EXPECT_EQ(v++, rs.getInt(++idx));
    // -> projective srid
    EXPECT_EQ(0, rs.getInt(++idx));
    // -> prc_x
    EXPECT_EQ(-132768, rs.getInt(++idx));
    // -> prc_y
    EXPECT_EQ(-132768, rs.getInt(++idx));
    // -> prc_z
    EXPECT_EQ(20001, rs.getInt(++idx));
    // -> crp_id
    EXPECT_EQ(0, rs.getInt(++idx));
    // -> dx_crp
    EXPECT_EQ(-132768, rs.getInt(++idx));
    // -> dy_crp
    EXPECT_EQ(-132768, rs.getInt(++idx));
    // -> dz_crp
    EXPECT_EQ(20001, rs.getInt(++idx));
    // -> lane_count
    EXPECT_EQ(15, rs.getInt(++idx));
    // -> lane_position
    EXPECT_EQ(17, rs.getInt(++idx));
    // -> lane_lateral_position
    EXPECT_EQ(101,rs.getInt(++idx));
    // -> crp_id_begin
    EXPECT_EQ(0, rs.getInt(++idx));
    // -> crp_id_end
    EXPECT_EQ(0, rs.getInt(++idx));
    // -> lane_vertical_position
    EXPECT_EQ(10001, rs.getInt(++idx));
    // -> lane_id
    EXPECT_EQ(0, getULL(rs, ++idx));
    // -> dx_lane dy_lane dh_lane
    for(int i = 0; i < 3; i++)
    {
      EXPECT_EQ(-132768, rs.getInt(++idx));
    }
    // -> semi_axis_length_major
    EXPECT_EQ(4095, rs.getInt(++idx));
    // -> semi_axis_length_minor
    EXPECT_EQ(4095, rs.getInt(++idx));
    // -> ellipse_orientation
    EXPECT_EQ(28800, rs.getInt(++idx));
    // -> altitude_accuracy
    EXPECT_EQ(7201, rs.getInt(++idx));

    // base time 
    // verificate that the time slot between send time and callback time is lower than ACCEPTABLE_RECEIVE_TIME_MSEC
    data_ts = rs.getLong(++idx);
    EXPECT_NEAR(data_ts, _cb_time, ACCEPTABLE_RECEIVE_TIME_MSEC);
   
    for(int i = 0; i < 8; i++)
    {
      // detectable_classes
      EXPECT_EQ(v++, rs.getInt(++idx));
      // poly_points x,y
      vector<int> tx, ty;
      tx = rs.getVectorInt(++idx);
      EXPECT_EQ(4, tx.size());
      ty = rs.getVectorInt(++idx);
      EXPECT_EQ(4, ty.size());
      for (size_t j = 0; j < tx.size(); ++j)
      {
        EXPECT_EQ(v++, tx[j]);
        EXPECT_EQ(v++, ty[j]);
      }
      // confidence
      EXPECT_EQ(v++, rs.getInt(++idx));
      // detectable_size
      EXPECT_EQ(v++, rs.getInt(++idx));
    }

    // sensor_status
    EXPECT_EQ(v++, rs.getInt(++idx));
    sensor_idx++;
                                                         
    break;
  }
  calAverageReceiveTime(_cb_time - data_ts);
  calThroughputs(_cb_time);
  callback_check = true;
}

/**
 * @brief 渡した結果集合に格納した物標情報が認識システムの模擬データと一致しているか検証する
 *      
 *
 */
void verifyObjectInfo(ResultSet rs)
{
  const uint64_t _cb_time = getTimeStamp();
  long data_ts;
  ResultSetMetaData rsmd = rs.getResultSetMetaData();
  EXPECT_EQ(88, rsmd.getColumnSize());
  while(rs.next())
  {
    uint32_t v = 0;
    int idx = 0;
    // id
    unsigned long long id = getULL(rs, idx);
    //EXPECT_EQ(v++, id);
    ++v;
    // time
    data_ts = rs.getLong(++idx);
    EXPECT_NEAR(data_ts, _cb_time, ACCEPTABLE_RECEIVE_TIME_MSEC);
    // revision
    EXPECT_EQ(0, rs.getInt(++idx));

    // object_classes
    // class_type
    EXPECT_EQ(2, rs.getInt(++idx));
    // class_confidence  
    EXPECT_EQ(v++, rs.getInt(++idx));
    // subclass_type
    EXPECT_EQ(0, rs.getInt(++idx));
    // subclass_type_confidence
    EXPECT_EQ(v++, rs.getInt(++idx));
    // empty_obj_class 3*4
    for(int i = 0; i < 3 * 4; i ++)
    {
       EXPECT_EQ(0, rs.getInt(++idx));
    }
    // confidence
    EXPECT_EQ(v++, rs.getInt(++idx));
    // Position
    // -> kGeodeticSystemCodeJgd2011
    EXPECT_EQ(6668, rs.getInt(++idx));
    // -> p.latitude
    EXPECT_EQ(v++, rs.getInt(++idx));
    // -> p.longitude
    EXPECT_EQ(v++, rs.getInt(++idx));
    // -> p.altitude
    EXPECT_EQ(v++, rs.getInt(++idx));
    // -> p.projective srid
    EXPECT_EQ(0, rs.getInt(++idx));
    // -> prc_x y z
    for(int i = 0; i < 3; i++)
    {
      EXPECT_EQ(-132768, rs.getInt(++idx));
    }
    // -> crp_id
    EXPECT_EQ(0, rs.getInt(++idx));
    // -> crp x y z
    for(int i = 0; i < 3; i++)
    {
      EXPECT_EQ(-132768, rs.getInt(++idx));
    }
    // -> lane_count
    EXPECT_EQ(15, rs.getInt(++idx));
    // -> lane_position
    EXPECT_EQ(17, rs.getInt(++idx));
    // -> lane_lateral_position
    EXPECT_EQ(101, rs.getInt(++idx));
    // -> crp_id begin end
    for(int i = 0; i < 2; i++)
    {
      EXPECT_EQ(0, rs.getInt(++idx));
    }
    // -> lane_vertical_position
    EXPECT_EQ(10001, rs.getInt(++idx));
    // -> lane_id
    EXPECT_EQ(0, getULL(rs, ++idx));
    // -> lane x y z
    for(int i = 0; i < 3; i++)
    {
      EXPECT_EQ(-132768, rs.getInt(++idx));
    }
    //  semi_major_axis_length semi_minor_axis_length semi_major_orientation altitude_accuracy
    for(int i = 0; i < 4; i++)
    {
      EXPECT_EQ(v++, rs.getInt(++idx));
    }
    // Position End
    
    // ref_point
    EXPECT_EQ(0, rs.getInt(++idx));

    // has_heading
    EXPECT_EQ(7200, rs.getInt(++idx));
    // ...
    for(int i = 0; i < 7; i++)
    {
      EXPECT_EQ(0, rs.getInt(++idx));
    }
    
    // orientation
    EXPECT_EQ(7200, rs.getInt(++idx));
    // orientation accuracy
    EXPECT_EQ(0, rs.getInt(++idx));
    // length
    EXPECT_EQ(300, rs.getInt(++idx));
    // length accuracy
    EXPECT_EQ(0, rs.getInt(++idx));
    // width
    EXPECT_EQ(200, rs.getInt(++idx));
    // width accuracy
    EXPECT_EQ(0, rs.getInt(++idx));
    // height
    EXPECT_EQ(100, rs.getInt(++idx));
    // height accuracy
    EXPECT_EQ(0, rs.getInt(++idx));
    // static status
    EXPECT_EQ(v++, rs.getInt(++idx));
    
    // shift_position                                   
    EXPECT_EQ(7, rs.getInt(++idx));
    // steering_angle_front                                   
    EXPECT_EQ(91, rs.getInt(++idx));
    // steering_angle_rear                                   
    EXPECT_EQ(91, rs.getInt(++idx));
    // brake_state                                   
    EXPECT_EQ(0, rs.getInt(++idx));
    // auxiliary_brake_state                                   
    EXPECT_EQ(0, rs.getInt(++idx));
    // throttle_position                                   
    EXPECT_EQ(255, rs.getInt(++idx));
    // exterior_lights adaptive_cruise_control_system c-accs pre_crash_safety_system
    // abs tcs escs lkas ldws vehicle_size_type
    for(int i = 0; i < 10; i++)
    {
      EXPECT_EQ(0, rs.getInt(++idx));
    }
    // vehicle_role                                   
    EXPECT_EQ(15, rs.getInt(++idx));
    // vehicle_extended_info                                   
    EXPECT_EQ(255, rs.getInt(++idx));
    // towing vehicle                                   
    EXPECT_EQ(0, getULL(rs, ++idx));
    // tracking_status o.detection_count lost_count object_age                             
    for(int i = 0; i < 4; i++)
    {
      EXPECT_EQ(v++, rs.getInt(++idx));
    }                                                                                                                                                
    // conf_.rsu_id
    vector<unsigned long long> uid = getVectorULL(rs, ++idx);
    EXPECT_EQ(0x12345678, uid[0]);
    break;
  }
  calAverageReceiveTime(_cb_time - data_ts);
  calThroughputs(_cb_time);
  callback_check = true;
}


/**
 * @brief 渡した結果集合に格納した走行可能領域情報が認識システムの模擬データと一致しているか検証する 
 *      
 *
 */
void verifyFreeSpaceInfo(ResultSet rs)
{
  const uint64_t _cb_time = getTimeStamp();
  long data_ts;
  ResultSetMetaData rsmd = rs.getResultSetMetaData();
  EXPECT_EQ(92, rsmd.getColumnSize());

  while(rs.next())
  {
    int v = 0;
    int idx = 0;
    // id
    EXPECT_EQ(0, getULL(rs, idx));
    // time
    data_ts = rs.getLong(++idx);
    EXPECT_NEAR(data_ts, _cb_time, ACCEPTABLE_RECEIVE_TIME_MSEC);
    // revision
    EXPECT_EQ(0, rs.getInt(++idx));
    // kFreespaceDetectionMethod
    EXPECT_EQ(1, rs.getInt(++idx));
    // detectable_classes
    EXPECT_EQ(0, rs.getInt(++idx));
    
    // Position
    // -> kGeodeticSystemCodeJgd2011
    EXPECT_EQ(6668, rs.getInt(++idx));
    // -> p.latitude
    EXPECT_EQ(v++, rs.getInt(++idx));
    // -> p.longitude
    EXPECT_EQ(v++, rs.getInt(++idx));
    // -> p.altitude
    EXPECT_EQ(v++, rs.getInt(++idx));
    // -> p.projective srid
    EXPECT_EQ(0, rs.getInt(++idx));
    // -> prc_x y z
    for(int i = 0; i < 3; i++)
    {
      EXPECT_EQ(-132768, rs.getInt(++idx));
    }
    // -> crp_id
    EXPECT_EQ(0, rs.getInt(++idx));
    // -> crp x y z
    for(int i = 0; i < 3; i++)
    {
      EXPECT_EQ(-132768, rs.getInt(++idx));
    }
    // -> lane_count
    EXPECT_EQ(15, rs.getInt(++idx));
    // -> lane_position
    EXPECT_EQ(17, rs.getInt(++idx));
    // -> lane_lateral_position
    EXPECT_EQ(101, rs.getInt(++idx));
    // -> crp_id begin end
    for(int i = 0; i < 2; i++)
    {
      EXPECT_EQ(0, rs.getInt(++idx));
    }
    // -> lane_vertical_position
    EXPECT_EQ(10001, rs.getInt(++idx));
    // -> lane_id
    EXPECT_EQ(0, getULL(rs, ++idx));
    // -> lane x y z
    for(int i = 0; i < 3; i++)
    {
      EXPECT_EQ(-132768, rs.getInt(++idx));
    }
    //  semi_major_axis_length semi_minor_axis_length semi_major_orientation altitude_accuracy
    for(int i = 0; i < 4; i++)
    {
      EXPECT_EQ(v++, rs.getInt(++idx));
    }
    // Position End
    
    // poly_points x poly_points y
    vector<int> v_ppts_x = rs.getVectorInt(++idx);
    vector<int> v_ppts_y = rs.getVectorInt(++idx);
    EXPECT_EQ(12, v_ppts_x.size());
    EXPECT_EQ(12, v_ppts_y.size());
    for(int pt_i = 0; pt_i < 12; pt_i++)
    {
      EXPECT_EQ(v++, v_ppts_x[pt_i]);
      EXPECT_EQ(v++, v_ppts_y[pt_i]);    
    }
    // Begin&End Position
    for(int pos_i = 0; pos_i < 2; pos_i++)
    {
      // -> kGeodeticSystemCodeJgd2011
      EXPECT_EQ(6668, rs.getInt(++idx));
      // -> p.latitude
      EXPECT_EQ(0, rs.getInt(++idx));
      // -> p.longitude
      EXPECT_EQ(0, rs.getInt(++idx));
      // -> p.altitude
      EXPECT_EQ(0, rs.getInt(++idx));
      // -> p.projective srid
      EXPECT_EQ(0, rs.getInt(++idx));
      // -> prc_x y z
      for(int i = 0; i < 3; i++)
      {
        EXPECT_EQ(-132768, rs.getInt(++idx));
      }
      // -> crp_id
      EXPECT_EQ(0, rs.getInt(++idx));
      // -> crp x y z
      for(int i = 0; i < 3; i++)
      {
        EXPECT_EQ(-132768, rs.getInt(++idx));
      }
      // -> lane_count
      EXPECT_EQ(15, rs.getInt(++idx));
      // -> lane_position
      EXPECT_EQ(17, rs.getInt(++idx));
      // -> lane_lateral_position
      EXPECT_EQ(101, rs.getInt(++idx));
      // -> crp_id begin end
      for(int i = 0; i < 2; i++)
      {
        EXPECT_EQ(0, rs.getInt(++idx));
      }
      // -> lane_vertical_position
      EXPECT_EQ(10001, rs.getInt(++idx));
      // -> lane_id
      EXPECT_EQ(0, getULL(rs, ++idx));
      // -> lane x y z
      for(int i = 0; i < 3; i++)
      {
        EXPECT_EQ(-132768, rs.getInt(++idx));
      }        
      //  semi_major_axis_length semi_minor_axis_length  
      for(int i = 0; i < 2; i++)
      {
        EXPECT_EQ(4095, rs.getInt(++idx));
      }
      // -> semi_major_orientation
      EXPECT_EQ(28800, rs.getInt(++idx));
      // -> altitude_accuracy
      EXPECT_EQ(20001, rs.getInt(++idx));
    }
    // Begin&End Position End

    // lenth
    EXPECT_EQ(-132768, rs.getInt(++idx));
    // lenth accurancy
    EXPECT_EQ(0, rs.getInt(++idx));
    // id_begin id_end
    for(int i = 0; i < 2; i++)
    {
      EXPECT_EQ(0, getULL(rs, ++idx));
    }
    
    // confidence 
    EXPECT_EQ(v++, rs.getInt(++idx));
    // detectable_size 
    EXPECT_EQ(v++, rs.getInt(++idx));
    
    // rsu_id
    vector<unsigned long long> uid = getVectorULL(rs, ++idx);
    EXPECT_EQ(0x12345678, uid[0]);
    break;
  } 
  calAverageReceiveTime(_cb_time - data_ts);                                         
  calThroughputs(_cb_time);
  callback_check = true;
}

/**
 * @brief パラメータ化テスト用の構造体
 *
 */
struct TestParamStruct
{
  string type;                        ///< 情報タイプ
  function<void (ResultSet)> fc;      ///< コールバック関数
  int num;                            ///< データ数
};

/**
 * @brief 情報タイプ・コールバック関数・データ数を指定
 * @note 実行の順番は配列の順番の通りです
 *
 */
vector<TestParamStruct> v_s = {
  {"sensor_info_0_8_1", bind(verifySensorInfo, placeholders::_1), 1},
  {"object_info_0_8_1", bind(verifyObjectInfo, placeholders::_1), 1},
  {"freespace_info_0_8_1", bind(verifyFreeSpaceInfo, placeholders::_1), 1}};

/**
 * @brief クライアント式で実行するprotobuf_dmiの結合テストのフィクスチャ
 *
 * @note active flow:   \n
 * DM ->| sendmsg     -> | *TesterReceiver \n 
 *      | protobuf_dmi-> |
 */
class ClientCombineTest : public ::testing::Test
{ 
protected:
  /**
　 * @brief テスト起動する前、用意すべき環境や変数を設定する
   *
   */
  void SetUp() override {
      // active DM
      system("dm2is -d /home/dm2/dm2/conf &");
      this_thread::sleep_for(chrono::milliseconds(WAITING_PERIOD_MSEC));
      
      
      callback_check = false;
      
      // performance data init
      diff_n = 0;
      avg = 0.0;
      v_diff.clear();
      check_ts = getTimeStamp();
      last_diff_n = 0;
      protobuf_dmi::utils::CreateLogDirIfNotExists("/tmp/dmi_perf_data/average/");    
      protobuf_dmi::utils::CreateLogDirIfNotExists("/tmp/dmi_perf_data/standard_deviation");    
      protobuf_dmi::utils::CreateLogDirIfNotExists("/tmp/dmi_perf_data/throughput/");    
      if(checkIfClear())
      {
        clearPerfHistory();
      }
   }
  /**
   * @brief テスト実行後、環境を片付ける
   *
   */
  void TearDown() override {
      killProcessesByName("protobuf_dmi_upload_sensing_message*");
      killProcessesByName("ccam_cool4_sensor_io_sample_send_msg*");
      killProcessesByName("dm2is");
      // write remaining data in avg_cache into files
      if(avg_cache.size())
      {
        string file_name = "/tmp/dmi_perf_data/average/" + test_name + ".csv" ;
        ofstream avg_data_log(file_name.c_str(), ofstream::app);
        for(auto p : avg_cache)
        {
          // times,avg
          avg_data_log << p.first << "," << p.second << endl;       
        }
        avg_cache.clear();
      }
    }
  string send_cmd;
};

/// 値をパラメータ化のためにテストクラスの親クラスを宣言する
class ParamClientCombineTest : public ClientCombineTest, public ::testing::WithParamInterface<TestParamStruct> {};

/**
 * @brief クライアント式で、DMに感知情報を要求して、送信したデータと一致かどうかを検証する。
 * @note 検証データとその配置をパラメータ化されたものが、v_sに定義されている。\n
 *       他の情報が検証したい時も利用できる、ただし、データ検証や実行検証のコールバック関数を用意する必要がある。
 *
 */
TEST_P(ParamClientCombineTest, CheckTypedMsgDataTest)
{
  // set up
      
  // active protobuf_dmi client
  system("protobuf_dmi_upload_sensing_message --dm_pass dm2samplepassword --receive_port 31877 &");
  this_thread::sleep_for(chrono::milliseconds(1000));

  // active sending service
  send_cmd = "ccam_cool4_sensor_io_sample_send_msg --port 31877 --rate_msec " + to_string(SEND_INTERVAL_MSEC) + " --object_info_num " + to_string(1) + " &";
  system(send_cmd.c_str());

  auto p = GetParam();

  string query = "MASTER " + p.type + " SELECT * FROM " + p.type + " [rows 1]";
  
  
  Connection* _conn = DmManager::getDBConnection("127.0.0.1", 9001, "dm2sampleuser", "dm2samplepassword", false);
 
  unsigned int mngId = 0;

  test_name = "client_" + p.type;
  
  // action
  
  mngId = _conn->registerQuery(query, p.fc);

  // verification

  this_thread::sleep_for(chrono::milliseconds(500));
  // in case that query doesn't return data, here will report if the callback function has not been called.
  EXPECT_TRUE(callback_check);
  // teardown
  
  _conn->reconnect();
  EXPECT_NO_THROW(_conn->cancelQuery(mngId));
  _conn->disconnect();

}

/**
 * @note ここからテスト起動させる
 *
 */
INSTANTIATE_TEST_CASE_P(callbackBindedClientCombineTest, ParamClientCombineTest, ::testing::ValuesIn(v_s));

#ifdef ENABLE_PLUGIN_TEST

/**
 * @brief プラグイン式で実行するprotobuf_dmiの結合テストのフィクスチャ
 *
 * @note active flow: \n
 * DM ->| sendmsg2plugin -> | *TesterReceiver 
 *    
 */
class PluginCombineTest : public ::testing::Test
{ 
protected:
  /**
　 * @brief テスト起動する前、用意すべき環境や変数を設定する
   *
   */
  void SetUp() override {
      
      callback_check = false;

      // performance data init
      diff_n = 0;
      avg = 0.0;
      v_diff.clear();
      check_ts = getTimeStamp();
      last_diff_n = 0;
      protobuf_dmi::utils::CreateLogDirIfNotExists("/tmp/dmi_perf_data/average/");    
      protobuf_dmi::utils::CreateLogDirIfNotExists("/tmp/dmi_perf_data/standard_deviation");    
      protobuf_dmi::utils::CreateLogDirIfNotExists("/tmp/dmi_perf_data/throughput/");    
      if(checkIfClear())
      {
        clearPerfHistory();
      }
    }

  /**
   * @brief テスト実行後、環境を片付ける
   *
   */
  void TearDown() override {
      killProcessesByName("ccam_cool4_sensor_io_sample_send_msg*");
      killProcessesByName("dm2is");
      if(avg_cache.size())
      {
        string file_name = "/tmp/dmi_perf_data/average/" + test_name + ".csv" ;
        ofstream avg_data_log(file_name.c_str(), ofstream::app);
        for(auto p : avg_cache)
        {
          // times,avg
          avg_data_log << p.first << "," << p.second << endl;       
        }
        avg_cache.clear();
      }
    }
  string send_cmd;
};

/// 値をパラメータ化のためにテストクラスの親クラスを宣言する
class ParamPluginCombineTest : public PluginCombineTest, public ::testing::WithParamInterface<TestParamStruct> {};

/**
 * @brief プラグイン式で、DMに感知情報を要求して、送信したデータと一致かどうかを検証する
 * @note 検証データとその配置をパラメータ化された値が、v_pに定義されている
 *
 */
TEST_P(ParamPluginCombineTest, CheckTypedMsgDataTest)
{
  // set up
  // active dm
  system("dm2is -d /home/dm2/dm2/conf &");
  this_thread::sleep_for(chrono::milliseconds(WAITING_PERIOD_MSEC));
  // active sending service
  send_cmd = "ccam_cool4_sensor_io_sample_send_msg --rate_msec " + to_string(SEND_INTERVAL_MSEC) + " --object_info_num " + to_string(1) + " &";
  system(send_cmd.c_str());

  auto p = GetParam();

  string query = "MASTER " + p.type + " SELECT * FROM " + p.type + " [rows " + to_string(p.num) + "]";
  
  
  Connection* _conn = DmManager::getDBConnection("127.0.0.1", 9001, "dm2sampleuser", "dm2samplepassword", false);
 
  unsigned int mngId = 0;
  
  test_name = "plugin_" + p.type;
  // action
  
  mngId = _conn->registerQuery(query, p.fc);

  // verification

  this_thread::sleep_for(chrono::milliseconds(TEST_SPIN_MSEC));
  // in case that query doesn't return data, here will report if the callback function has not been called.
  EXPECT_TRUE(callback_check);
  
  // teardown
  
  _conn->reconnect();
  EXPECT_NO_THROW(_conn->cancelQuery(mngId));
  _conn->disconnect();

}


/**
 * @note ここからテスト起動させる
 *
 */
INSTANTIATE_TEST_CASE_P(callbackBindedPluginCombineTest, ParamPluginCombineTest, ::testing::ValuesIn(v_s));

#endif  // ENABLE_PLUGIN_TEST

