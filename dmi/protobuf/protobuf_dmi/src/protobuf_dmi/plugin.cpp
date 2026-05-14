#include <memory>
#include <thread>
#include <chrono>
#include <vector>

#include <is/Tuple.h>

#include "protobuf_dmi/protobuf_dmi.hpp"

using namespace protobuf_dmi;

class PluginBase
{
};

template<class T, class D>
class Plugin : public PluginBase
{
public:
  Plugin(
      const int port,
      const uint32_t rsu_id,
      const uint32_t sensor_id,
      void (*user_callback)(std::vector<IS::Tuple>&))
    : callback_(user_callback)
  {
    utils::Config conf;
    conf.rsu_id = rsu_id;
    conf.sensor_id = sensor_id;

    receiver_.reset(new Receiver<T>(
          port,
          {
            std::make_shared<D>(D(conf))
          },
          [this](const std::vector<IS::Tuple>& tuples)
          {
            std::vector<IS::Tuple> mutable_tuples = tuples;
            this->callback_(mutable_tuples);
          }));
  }

  virtual ~Plugin() = default;

private:
  std::function<void(std::vector<IS::Tuple>&)> callback_;
  typename Receiver<T>::SharedPtr receiver_;
};

class SensingMessagePlugin : public Plugin<SensingMessage, SensingMessageDmBridge>
{
public:
  SensingMessagePlugin(
      const int port,
      const uint32_t rsu_id,
      const uint32_t sensor_id,
      void (*user_callback)(std::vector<IS::Tuple>&))
    : Plugin<SensingMessage, SensingMessageDmBridge>(
        port, rsu_id, sensor_id, user_callback)
  {
  }

  virtual ~SensingMessagePlugin() = default;
};

class ProtobufDmiPlugin final
{
public:
  ProtobufDmiPlugin(
      const std::string& message_type,
      const int port,
      const uint32_t rsu_id,
      const uint32_t sensor_id,
      void (*user_callback)(std::vector<IS::Tuple>&))
  {
    if (message_type == "SensingMessage")
    {
      plugin_.reset(new SensingMessagePlugin(
            port,
            rsu_id,
            sensor_id,
            user_callback));
    }
    else
    {
      throw std::runtime_error("unsupported message type: " + message_type);
    }
  }

  ~ProtobufDmiPlugin() = default;

  void Spin()
  {
    std::thread wait_loop(
      []
      {
        while (true)
        {
          std::this_thread::sleep_for(
              std::chrono::milliseconds(10));
        }
      });
    wait_loop.join();
  }

private:
  std::shared_ptr<PluginBase> plugin_;
};


extern "C"
{

intptr_t create(
      const std::string& message_type,
      const int port,
      const int rsu_id,
      const int sensor_id,
      void (*user_callback)(std::vector<IS::Tuple>&))
{
  return reinterpret_cast<intptr_t>(new ProtobufDmiPlugin(
        message_type,
        port,
        static_cast<uint32_t>(rsu_id),
        static_cast<uint32_t>(sensor_id),
        user_callback));
}

void spin(intptr_t ptr)
{
  reinterpret_cast<ProtobufDmiPlugin*>(ptr)->Spin();
}

void destroy(intptr_t ptr)
{
  delete reinterpret_cast<ProtobufDmiPlugin*>(ptr);
}

}
