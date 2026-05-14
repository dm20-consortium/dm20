#ifndef DMI_UTILS_CONVERTER_HPP__
#define DMI_UTILS_CONVERTER_HPP__
#include <vector>
#include <functional>

#include <is/Tuple.h>

namespace dmi_utils {

template<class ElementType, template<class> class DataPackage>
class Converter {
 public:
  using CallbackType = std::function<void (std::vector<IS::Tuple>&)>;
  using SharedPtr = std::shared_ptr<Converter>;

  Converter(const CallbackType& user_callback) : user_callback_(user_callback) {}

  void FromDataclass(const DataPackage<ElementType>& dataclasses) {
    std::vector<IS::Tuple> tuples;
    for(const auto& e : dataclasses.dataclasses) {
      const long stamp = DmUtil::getTimeMillisec();
      IS::Tuple tuple(ElementType::ElementNum);
      uint32_t idx = 0;
      DmBridge<ElementType>::FromDataclass(e, stamp, idx, tuple);
      VLOG(2) << "index of tuple: " << idx;
      tuples.push_back(tuple);
    }

    if (!user_callback_) return;

    user_callback_(tuples);
  }

 private:
  CallbackType user_callback_;
};
}
#endif
