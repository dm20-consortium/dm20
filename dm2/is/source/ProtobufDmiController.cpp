#include "ProtobufDmiController.h"

namespace IS
{

void ProtobufDmiController::start()
{
  uploader.init();
  uploader.start();
}

}  // namespace IS
