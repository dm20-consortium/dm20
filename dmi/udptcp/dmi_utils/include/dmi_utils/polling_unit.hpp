#ifndef DMI_UTILS_POLLING_CONVERTER_HPP_
#define DMI_UTILS_POLLING_CONVERTER_HPP_

#include "dmi_utils/dm_bridge.hpp"
#include "dmi_utils/converter.hpp"

#include "dmi_utils/queue.hpp"

namespace dmi_utils
{

template<class T, template<class> class D>
class PollingUnit
{
public:
  using SharedPtr = std::shared_ptr<PollingUnit>;

  PollingUnit(
      const uint32_t rate_msec,
      const typename Converter<T, D>::CallbackType& callback)
    : rate_msec_(rate_msec),
      active_(true),
      user_callback_(callback)
  {
    thread_.reset(new std::thread(std::bind(&PollingUnit<T, D>::Update, this)));
  }

  void Push(const std::vector<IS::Tuple>& in)
  {
    VLOG(2) << "Push";
    queue_.Push(in);
    VLOG(2) << "[done]Push";
  }

  void Stop()
  {
    std::unique_lock<std::mutex> lk(mtx_);
    latest_data_ = boost::none;
  }

  ~PollingUnit()
  {
    active_ = false;
    thread_->join();
  }

private:
  void Update()
  {
    VLOG(2) << "start thread(" << std::this_thread::get_id() << ")";
    uint32_t actual_rate_msec = rate_msec_;

    while (active_)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(actual_rate_msec));

      const auto st = std::chrono::system_clock::now();
      const auto st_nsec = st.time_since_epoch().count();
      VLOG(3) << "head of thread: " << st_nsec << "(nsec)";

      // キューが空で，最新データがあるなら送る
      if (queue_.Empty())
      {
        if (latest_data_ == boost::none) continue;

        user_callback_(latest_data_.get());
        continue;
      }

      // ポップに失敗しても，最新データがあるなら送る
      const boost::optional<std::vector<IS::Tuple>> data = queue_.Pop();
      if (data == boost::none)
      {
        if (latest_data_ == boost::none) continue;
        user_callback_(latest_data_.get());
        continue;
      }

      // データを更新して送る
      {
        std::unique_lock<std::mutex> lk(mtx_);
        latest_data_ = data;
        user_callback_(latest_data_.get());
      }

      // 正確にrate_msec周期で稼働するために，処理時間を考慮する
      // 設定されたスリープ時間に対して処理時間分を引いてあげる
      const auto et = std::chrono::system_clock::now();
      const auto et_nsec = et.time_since_epoch().count();
      VLOG(3) << "tail of thread: " << et_nsec << "(nsec)";
      const uint32_t dt_msec = std::round((et_nsec - st_nsec) / 1000000.0);
      VLOG(2) << "takt per cycle: " << dt_msec << "(msec)";
      actual_rate_msec = rate_msec_ - dt_msec;
    }
    VLOG(2) << "end thread(" << std::this_thread::get_id() << ")";
  }

  const uint32_t rate_msec_;
  std::shared_ptr<std::thread> thread_;
  bool active_;
  Queue<std::vector<IS::Tuple>> queue_;
  const typename Converter<T, D>::CallbackType user_callback_;
  boost::optional<std::vector<IS::Tuple>> latest_data_;
  std::mutex mtx_;
};

}  // namespace dmi_utils
#endif  // DMI_UTILS_POLLING_CONVERTER_HPP_
