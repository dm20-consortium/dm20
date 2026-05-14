#ifndef TCP_DMI_SERVER_HPP_
#define TCP_DMI_SERVER_HPP_
#include <dmi_utils/base.hpp>
#include <dmi_utils/dm_bridge.hpp>
#include <dmi_utils/converter.hpp>

#include "tcp_dmi/simple_server.hpp"
#include <dmi_utils/polling_uploader.hpp>
#include <dmi_utils/polling_unit.hpp>

namespace tcp_dmi
{

template<class T, template<class> class D>
class Server : public dmi_utils::Base {
 public:
  using SharedPtr = std::shared_ptr<Server>;

  Server(
      const typename dmi_utils::Uploader<T, D>::Config& dm2_conf,
      const int rate_msec,
      const int port)
    : uploader_(
        new dmi_utils::PollingUploader<T, D>(dm2_conf, rate_msec)),
      converter_(
        new dmi_utils::Converter<T, D>(
          std::bind(
            &dmi_utils::PollingUploader<T, D>::Push,
            uploader_.get(),
            std::placeholders::_1))),
      server_(
        new SimpleServer<T, D>(
          port,
          std::bind(
            &dmi_utils::Converter<T, D>::FromDataclass,
            converter_.get(),
            std::placeholders::_1),
          std::bind(
            &dmi_utils::PollingUploader<T, D>::Stop,
            uploader_.get()))) {}

  Server(
      const int port,
      const typename dmi_utils::Converter<T, D>::CallbackType& callback)
    : polling_unit_(
        new dmi_utils::PollingUnit<T, D>(
          100,
          callback)),
      converter_(
        new dmi_utils::Converter<T, D>(
          std::bind(
            &dmi_utils::PollingUnit<T, D>::Push,
            polling_unit_.get(),
            std::placeholders::_1))),
      server_(
        new SimpleServer<T, D>(
          port,
          std::bind(
            &dmi_utils::Converter<T, D>::FromDataclass,
            converter_.get(),
            std::placeholders::_1),
          std::bind(
            &dmi_utils::PollingUnit<T, D>::Stop,
            polling_unit_.get()))) {}

  ~Server() = default;

  void Spin(const uint32_t rate_msec = 10) override {
    server_->Run();
  }

 private:
  typename dmi_utils::PollingUploader<T, D>::SharedPtr uploader_;
  typename dmi_utils::PollingUnit<T, D>::SharedPtr polling_unit_;
  typename dmi_utils::Converter<T, D>::SharedPtr converter_;
  typename SimpleServer<T, D>::SharedPtr server_;
};

}  // tcp_dmi
#endif  // TCP_DMI_SERVER_HPP_
