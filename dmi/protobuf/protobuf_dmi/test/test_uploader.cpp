/**                                                                                                  
 * @file test_uploader.cpp
 *
 * @brief uploader.hpp(cpp)の単体テスト
 *
 */

#include<gtest/gtest.h>
#include "utils_of_test.hpp"                                                                              
/**
 *  @brief パラメータのテストフィクスチャ
 */
class UploaderTest : public ::testing::Test
{
  protected:
  virtual void SetUp() override {
    conf_ = new protobuf_dmi::Uploader::Config(
    "127.0.0.1",
    9002,
    "dm2sampleuser",
    "dm2samplepassword",
    9); 
  }
  UploaderTest(){}

  protobuf_dmi::Uploader::Config* conf_;

};

/**
 * @brief Uploaderの設定パラメータが正しく設定されているかを検証する
 *
 *
 */
TEST_F(UploaderTest, ConfigTest)
{
  // set up

  // action
  protobuf_dmi::Uploader::Config c(
    "127.0.0.1",
    9002,
    "dm2sampleuser",
    "dm2samplepassword",
    9);

  // verification
  EXPECT_EQ(conf_->ipaddr, c.ipaddr);
  EXPECT_EQ(conf_->port, c.port);
  EXPECT_EQ(conf_->user, c.user);
  EXPECT_EQ(conf_->pass, c.pass);
  EXPECT_EQ(conf_->connection_try_count, c.connection_try_count);

  // teardown
}

/**
 * @brief DMが起動されていない場合、Uploaderが錯誤を報告できるかどうかを検証する
 *
 *
 */ 
TEST_F(UploaderTest, NoDMErrorShouldThrowTest)
{
  // set up
  conf_->connection_try_count = 1;
  // action
  protobuf_dmi::Uploader::SharedPtr p_up;

  // verification
  EXPECT_ANY_THROW(p_up.reset(new protobuf_dmi::Uploader(*conf_)));

  // teardown

}



/**
 * @brief DMを起動して、正しいコンフィグを渡して、DMに繋がれるがどうかを検証する
 *
 *
 */
TEST_F(UploaderTest, ConnectDMSuccessTest)
{
  // set up
  system("dm2is -d /home/dm2/dm2/conf &");
  std::this_thread::sleep_for(std::chrono::milliseconds(WAITING_PERIOD_MSEC));

  // action
  protobuf_dmi::Uploader::SharedPtr p_up;

  // verification
  EXPECT_NO_THROW(p_up.reset(new protobuf_dmi::Uploader(*conf_)));

  // teardown
  killProcessesByName("dm2is");
}

/**
 * @brief DMを起動して、間違いコンフィグを渡して、DMに繋がれないことを検証する
 *
 *
 */
TEST_F(UploaderTest, ConnectDMFailedTest)
{
  // set up
  system("dm2is -d /home/dm2/dmi_test/dm20/conf &");
  std::this_thread::sleep_for(std::chrono::milliseconds(WAITING_PERIOD_MSEC));
  
  conf_->connection_try_count = 1;
  conf_->pass = "errortry";
  conf_->ipaddr = "127.0.0.127";
  // action
  protobuf_dmi::Uploader::SharedPtr p_up;

  // verification
  EXPECT_ANY_THROW(p_up.reset(new protobuf_dmi::Uploader(*conf_)));


  // teardown
  killProcessesByName("dm2is");
}
