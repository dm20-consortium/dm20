#ifndef PROTOBUF_DMI_UPLOADER_WRAPPER_H_
#define PROTOBUF_DMI_UPLOADER_WRAPPER_H_
#include <dlfcn.h>

#include <functional>
#include <vector>
#include <string>
#include <thread>
#include <map>

#include <log4cxx/logger.h>
#include <is/Tuple.h>

namespace IS
{

class ProtobufDMIUploaderWrapper
{
public:
  using CallbackType = std::function<void (std::vector<IS::Tuple>&)>;

  ProtobufDMIUploaderWrapper();
  ~ProtobufDMIUploaderWrapper() = default;

  void init();
  void start();
  void stop();

private:
  void spin(const std::string& msg_type);
  void eventCallback(std::vector<Tuple>& tuples);

  using CreateReceiverFuncPtr = intptr_t (*)(
      std::string&,
      int,
      int,
      int,
      void (ProtobufDMIUploaderWrapper::*)(std::vector<Tuple>&));
  using SpinFuncPtr = void (*)(intptr_t);
  using DestroyFuncPtr = void (*)(intptr_t);

  log4cxx::LoggerPtr logger_;
  bool read_error_;
  // スレッドセーフにするための措置
  static std::string ipaddr_;
  static std::string user_;
  SpinFuncPtr spin_;

  using ThreadsByMsgType = std::map<std::string, std::vector<std::thread>>;
  using InstancesByMsgType = std::map<std::string, std::vector<intptr_t>>;
  ThreadsByMsgType threads_by_msg_;
  InstancesByMsgType instances_by_msg_;
};

}  // namespace IS

#endif  // PROTOBUF_DMI_UPLOADER_WRAPPER_H_
