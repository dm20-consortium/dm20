#ifndef PROTOBUF_DMI_CONTROLLER_H_
#define PROTOBUF_DMI_CONTROLLER_H_
#include <string>
#include <memory>

#include <log4cxx/logger.h>

#include "ProtobufDmiUploaderWrapper.h"

namespace IS
{

class TupleSet;

class ProtobufDmiController
{
public:
  using SharedPtr = std::shared_ptr<ProtobufDmiController>;

  ProtobufDmiController() = default;
  ~ProtobufDmiController() = default;

  void start();

  // nothing to do
  void init() {};
  void stop() {};
  void downloaderStart() {}
  void wrapSendTo(
      IS::TupleSet& tupleset,
      const std::string& tableName) {}

  ProtobufDMIUploaderWrapper uploader;

private:
  log4cxx::LoggerPtr logger_;
};

}  // namespace IS
#endif  // PROTOBUF_DMI_CONTROLLER_H_
