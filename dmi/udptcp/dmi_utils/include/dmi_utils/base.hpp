#ifndef DMI_UTILS_BASE_HPP_
#define DMI_UTILS_BASE_HPP_
#include <memory>
#include <thread>

namespace IS
{
class TupleSet;
}

namespace dmi_utils {

class Base {
 public:
  using SharedPtr = std::shared_ptr<Base>;

  Base() = default;
  virtual ~Base() {
    if (thread_ == nullptr) return;

    thread_->join();
  }

  virtual void Spin(const uint32_t rate_msec = 10);

 private:
  std::shared_ptr<std::thread> thread_;
};

class SendInterface
{
public:
  virtual void Send(IS::TupleSet& ts) = 0;
};

class BaseWithSendInterface : public Base, public SendInterface
{
public:
  using SharedPtr = std::shared_ptr<BaseWithSendInterface>;
};

}
#endif
