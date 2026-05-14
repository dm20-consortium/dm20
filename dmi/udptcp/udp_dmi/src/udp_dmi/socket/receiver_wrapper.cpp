#include <string>
#include <map>
#include <vector>

#include <is/Tuple.h>

#include <cool4_api_dataclass/object_info.hpp>
#include <cool4_api_dataclass/freespace_info.hpp>
#include <cool4_api_dataclass/signal_info.hpp>
#include <cool4_api_dataclass/sensor_info.hpp>
#include <cool4_api_dataclass/dataclasses_with_header.hpp>
#include <dmi_utils/filesystem.hpp>
#include "udp_dmi/receiver.hpp"

using DataTypeObject = cool4_api_dataclass::ObjectInfo;
using DataTypeFreeSpace = cool4_api_dataclass::FreespaceInfo;
using DataTypeSignal = cool4_api_dataclass::SignalInfo;
using DataTypeSensor = cool4_api_dataclass::SensorInfo;
template<class T>
using DataPackage = cool4_api_dataclass::DataClassesWithHeader<T>;
using CallbackType = std::function<void (std::vector<IS::Tuple>&)>;

class ReceiverWrapper {
public:
    ReceiverWrapper(const int& receptionPort, const std::string& streamName, void (*user_callback)(std::vector<IS::Tuple>&)) {
        _streamName = streamName;
        _callbackFunc = user_callback;
        if (_streamName == "object_info_0_8_1") {
            udp_dmi::system::Receiver<DataTypeObject, DataPackage>::SharedPtr system(
                new udp_dmi::system::Receiver<DataTypeObject, DataPackage>(
                  receptionPort,
                  [this](const std::vector<IS::Tuple>& tuples) {
                    std::vector<IS::Tuple> mutable_tuples = tuples;
                    for (auto& e : mutable_tuples) e.setTableName(this->_streamName);
                    this->_callbackFunc(mutable_tuples);
                  }));
            _systemObject = system;
        } else if (_streamName == "freespace_info_0_8_1") {
            udp_dmi::system::Receiver<DataTypeFreeSpace, DataPackage>::SharedPtr system(
                new udp_dmi::system::Receiver<DataTypeFreeSpace, DataPackage>(
                  receptionPort,
                  [this](const std::vector<IS::Tuple>& tuples) {
                    std::vector<IS::Tuple> mutable_tuples = tuples;
                    for (auto& e : mutable_tuples) e.setTableName(this->_streamName);
                    this->_callbackFunc(mutable_tuples);
                  }));
            _systemFreeSpace = system;
        } else if (_streamName == "signal_info") {
            udp_dmi::system::Receiver<DataTypeSignal, DataPackage>::SharedPtr system(
                new udp_dmi::system::Receiver<DataTypeSignal, DataPackage>(
                  receptionPort,
                  [this](const std::vector<IS::Tuple>& tuples) {
                    std::vector<IS::Tuple> mutable_tuples = tuples;
                    for (auto& e : mutable_tuples) e.setTableName(this->_streamName);
                    this->_callbackFunc(mutable_tuples);
                  }));
            _systemSignal = system;
        } else if (_streamName == "sensor_info_0_8_1") {
            udp_dmi::system::Receiver<DataTypeSensor, DataPackage>::SharedPtr system(
                new udp_dmi::system::Receiver<DataTypeSensor, DataPackage>(
                  receptionPort,
                  [this](const std::vector<IS::Tuple>& tuples) {
                    std::vector<IS::Tuple> mutable_tuples = tuples;
                    for (auto& e : mutable_tuples) e.setTableName(this->_streamName);
                    this->_callbackFunc(mutable_tuples);
                  }));
            _systemSensor = system;
        } else {
          std::cerr << "unsupported stream: " << _streamName << std::endl;
        }
    }

    ~ReceiverWrapper() {}

    void spin() {
      if (_streamName == "object_info_0_8_1") {
        _systemObject->Spin();
      } else if (_streamName == "freespace_info_0_8_1") {
        _systemFreeSpace->Spin();
      } else if (_streamName == "signal_info") {
        _systemSignal->Spin();
      } else if (_streamName == "sensor_info_0_8_1") {
        _systemSensor->Spin();
      } else {
        std::cerr << "unsupported stream: " << _streamName << std::endl;
      }
    }

private:
    std::string _streamName;
    udp_dmi::system::Receiver<DataTypeObject, DataPackage>::SharedPtr _systemObject;
    udp_dmi::system::Receiver<DataTypeFreeSpace, DataPackage>::SharedPtr _systemFreeSpace;
    udp_dmi::system::Receiver<DataTypeSignal, DataPackage>::SharedPtr _systemSignal;
    udp_dmi::system::Receiver<DataTypeSensor, DataPackage>::SharedPtr _systemSensor;
    CallbackType _callbackFunc;
};

extern "C" {

    /**
     * @brief ReceiverWrapperクラスのインスタンスを生成する。
     * @brief エントリーポイント
     *
     * @param callback
     * 
     * @return instance
     */
    intptr_t create(const int& port, const std::string& stream, void (*callback)(std::vector<IS::Tuple>&)) {
        return reinterpret_cast<intptr_t>(new ReceiverWrapper(port, stream, callback));
    }

    /**
     * @brief ReceiverWrapperクラスのspinメソッドをコールする。
     * @brief エントリーポイント
     *
     * @param ptr
     * 
     */
    void spin(intptr_t ptr) {
        reinterpret_cast<ReceiverWrapper*>(ptr)->spin();
    }

    /**
     * @brief ReceiverWrapperクラスのインスタンスを破棄する。
     * @brief エントリーポイント
     *
     * @param ptr
     * 
     */
    void destroy(intptr_t ptr) {
        delete reinterpret_cast<ReceiverWrapper*>(ptr);
    }
}
