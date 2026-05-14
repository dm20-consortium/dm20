#include "dmi_utils/base.hpp"

#include <chrono>


namespace dmi_utils {

void Base::Spin(const uint32_t rate_msec) {
  thread_.reset(new std::thread([rate_msec] {
    while (true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(rate_msec));
    }
  }));
  thread_->join();
}

}
