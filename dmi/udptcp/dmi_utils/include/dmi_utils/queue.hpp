#ifndef DMI_UTILS_QUEUE_HPP_
#define DMI_UTILS_QUEUE_HPP_
#include <memory>
#include <queue>
#include <mutex>

#include <boost/optional.hpp>

namespace dmi_utils {

template<class T>
class Queue {
 public:
  using SharedPtr = std::shared_ptr<Queue>;

  Queue() = default;
  ~Queue() = default;

  void Push(const T& in) {
    std::unique_lock<std::mutex> lk(mtx_);
    queue_.push(in);
  }

  size_t Size() {
    std::unique_lock<std::mutex> lk(mtx_);
    return queue_.size();
  }

  bool Empty() {
    std::unique_lock<std::mutex> lk(mtx_);
    return queue_.empty();
  }

  boost::optional<T> Pop() {
    std::unique_lock<std::mutex> lk(mtx_);
    if (queue_.empty()) {
      return boost::none;
    }
    const T ret = queue_.front();
    queue_.pop();
    if(queue_.empty()){
      std::queue<T>().swap(queue_);
    }
    return ret;
  }

 private:
  std::queue<T> queue_;
  std::mutex mtx_;
};

}

#endif
