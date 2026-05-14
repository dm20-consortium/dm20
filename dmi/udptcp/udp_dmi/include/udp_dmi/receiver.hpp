#ifndef UDP_DMI_RECEIVER_HPP_
#define UDP_DMI_RECEIVER_HPP_
#include <memory>

#include <dmi_utils/base.hpp>
#include "udp_dmi/socket_receiver.hpp"
#include <dmi_utils/uploader.hpp>
#include <dmi_utils/converter.hpp>

namespace udp_dmi {
namespace system {

template<class T, template<class> class D>
class Receiver : public dmi_utils::Base {
 public:
  using SharedPtr = std::shared_ptr<Receiver>;

  Receiver(
      const typename dmi_utils::Uploader<T, D>::Config& dm2_conf,
      const int port)
    : uploader_(
        new dmi_utils::Uploader<T, D>(dm2_conf)),
      converter_(
        new dmi_utils::Converter<T, D>(
          std::bind(
            &dmi_utils::Uploader<T, D>::Upload,
            uploader_.get(),
            std::placeholders::_1))),
      socket_(
        new SocketReceiver<T, D>(
          port,
          std::bind(
            &dmi_utils::Converter<T, D>::FromDataclass,
            converter_.get(),
            std::placeholders::_1))) {}

  // つかわないメンバ変数がいるので，あとで修正する
  Receiver(
      const int port,
      const typename dmi_utils::Converter<T, D>::CallbackType& user_callback)
    : converter_(
        new dmi_utils::Converter<T, D>(user_callback)),
      socket_(
        new SocketReceiver<T, D>(
          port,
          std::bind(
            &dmi_utils::Converter<T, D>::FromDataclass,
            converter_.get(),
            std::placeholders::_1))) {}

  ~Receiver() = default;

 private:
  typename dmi_utils::Uploader<T, D>::SharedPtr uploader_;
  typename dmi_utils::Converter<T, D>::SharedPtr converter_;
  typename SocketReceiver<T, D>::SharedPtr socket_;
};

}
}

#endif
