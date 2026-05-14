/**
 * @file test_utils.cpp
 *
 * @brief utils.hpp(cpp)の単体テスト
 *
 */

#include<gtest/gtest.h>
#include "utils_of_test.hpp"



namespace fs = boost::filesystem;
namespace sys = boost::system;

/**
 * @brief 既に存在しているログフォルダーを検出するかを確認する
 *
 */
TEST(CreateLogDirIfNotExistsTest, ExistFileTest)
{
  // set up
  const std::string test_path = "./exist";
  sys::error_code err_code;

  fs::path p = fs::path(test_path);


  try
  {
    fs::create_directories(p, err_code);
  }catch(fs::filesystem_error& ex){
    std::cout << ex.what() << std::endl;
    throw;
  }

  // action
  bool flag = protobuf_dmi::utils::CreateLogDirIfNotExists(test_path);
  // verification
  EXPECT_EQ(true, flag);

  // tear down
  try{
    fs::remove(test_path);
  }catch(fs::filesystem_error& ex){
    std::cout << ex.what() << std::endl;
    throw;
  }
}

/**
 * @brief ログファイルが存在しなければ新しいファイルを作成するか確認する
 *
 */
TEST(CreateLogDirIfNotExistsTest, NotExistFileTest)
{
  //set up
  const std::string t_path = "./notyetexisted";
  fs::path p = fs::path(t_path);

  sys::error_code err_code;
  

  //action
  bool flag = protobuf_dmi::utils::CreateLogDirIfNotExists(t_path);

  //verification
  EXPECT_EQ(true, flag);
  EXPECT_EQ(true, fs::exists(t_path, err_code));

  //tear down
  try{
      fs::remove(t_path);
    }catch(fs::filesystem_error& ex){
      std::cout << ex.what() << std::endl;
      throw;
   }    
}


/**
 * @brief 入力値が最大値より大きい場合、循環回数を正しく計算されかどうかを検証する
 *
 *
 */
TEST(CalcLoopNumberTest, GTMaxLoopNumTest)
{
  // set up
  std::pair<int, int> out1;
  // action
  out1 = protobuf_dmi::utils::CalcLoopNumber(10, 100);
  // verification
  EXPECT_EQ(10, out1.first);
  EXPECT_EQ(0, out1.second);
  // tear down
}

/**
 * @brief 入力値が最大値より小さい場合、循環回数を正しく計算されかどうかを検証する
 *
 *
 */
TEST(CalcLoopNumberTest, LTMaxResultLoopNumTest)
{
  // set up
  std::pair<int, int> out2;
  // action
  out2 = protobuf_dmi::utils::CalcLoopNumber(10, 1);
  // verification
  EXPECT_EQ(1, out2.first);
  EXPECT_EQ(9, out2.second);
  // tear down
}




