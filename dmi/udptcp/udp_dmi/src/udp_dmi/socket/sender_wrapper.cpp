
#include <string>
#include <map>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <is/Tuple.h>
#include <is/TupleSet.h>

#include "udp_dmi/sender.hpp"
#include <cool4_api_dataclass/object_info.hpp>
#include <cool4_api_dataclass/freespace_info.hpp>
#include <cool4_api_dataclass/signal_info.hpp>
#include <cool4_api_dataclass/sensor_info.hpp>
#include <cool4_api_dataclass/config.h>
#include <dmi_utils/filesystem.hpp>
#include <dmi_utils/downloader.hpp>

using DataTypeObject = cool4_api_dataclass::ObjectInfo;
using DataTypeFreeSpace = cool4_api_dataclass::FreespaceInfo;
using DataTypeSignal = cool4_api_dataclass::SignalInfo;
using DataTypeSensor = cool4_api_dataclass::SensorInfo;
template<class T>
using DataPackage = cool4_api_dataclass::DataClassesWithHeader<T>;

class SenderWrapper {
public:
    SenderWrapper(const std::string& streamName, std::map<std::string, std::string>& configMap) {
        _streamName = streamName;
        int32_t targetPort = boost::lexical_cast<int32_t>(configMap["targetPort"]);
        uint32_t frameRate = boost::lexical_cast<uint32_t>(configMap["frameRateMaxMilsec"]);
        uint32_t dataNumPerFrame = boost::lexical_cast<uint32_t>(configMap["dataNumPerFrame"]);
        if (_streamName == "object_info_0_8_1") {
            udp_dmi::system::Sender<DataTypeObject>::SharedPtr system(
                new udp_dmi::system::Sender<DataTypeObject>(
                {
                configMap["targetIpAddr"],
                targetPort,
                frameRate,
                static_cast<uint32_t>(100 / frameRate)
                }, {
                cool4_api_dataclass_VERSION_MAJOR,
                cool4_api_dataclass_VERSION_MINOR,
                cool4_api_dataclass_VERSION_PATCH,
                dataNumPerFrame
                }));
            _systemObject = system;
        } else if (_streamName == "freespace_info_0_8_1") {
            udp_dmi::system::Sender<DataTypeFreeSpace>::SharedPtr system(
                new udp_dmi::system::Sender<DataTypeFreeSpace>(
                {
                configMap["targetIpAddr"],
                targetPort,
                frameRate,
                static_cast<uint32_t>(100 / frameRate)
                }, {
                cool4_api_dataclass_VERSION_MAJOR,
                cool4_api_dataclass_VERSION_MINOR,
                cool4_api_dataclass_VERSION_PATCH,
                dataNumPerFrame
                }));
            _systemFreeSpace = system;
        } else if (_streamName == "signal_info") {
            udp_dmi::system::Sender<DataTypeSignal>::SharedPtr system(
                new udp_dmi::system::Sender<DataTypeSignal>(
                {
                configMap["targetIpAddr"],
                targetPort,
                frameRate,
                static_cast<uint32_t>(100 / frameRate)
                }, {
                cool4_api_dataclass_VERSION_MAJOR,
                cool4_api_dataclass_VERSION_MINOR,
                cool4_api_dataclass_VERSION_PATCH,
                dataNumPerFrame
                }));
            _systemSignal = system;
        } else if (_streamName == "sensor_info_0_8_1") {
            udp_dmi::system::Sender<DataTypeSensor>::SharedPtr system(
                new udp_dmi::system::Sender<DataTypeSensor>(
                {
                configMap["targetIpAddr"],
                targetPort,
                frameRate,
                static_cast<uint32_t>(100 / frameRate)
                }, {
                cool4_api_dataclass_VERSION_MAJOR,
                cool4_api_dataclass_VERSION_MINOR,
                cool4_api_dataclass_VERSION_PATCH,
                dataNumPerFrame
                }));
            _systemSensor = system;
        } else {
          std::cerr << "unsupported stream name: " << _streamName << std::endl;
        }
    }

    ~SenderWrapper() {}

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
            /*nothing to do.*/
        }
    }

    void sendTo(IS::TupleSet& tupleset) {
        if (_streamName == "object_info_0_8_1") {
            _systemObject->Send(tupleset);
        } else if (_streamName == "freespace_info_0_8_1") {
            _systemFreeSpace->Send(tupleset);
        } else if (_streamName == "signal_info") {
            _systemSignal->Send(tupleset);
        } else if (_streamName == "sensor_info_0_8_1") {
            _systemSensor->Send(tupleset);
        } else {
          std::cerr << "unsupported stream: " << _streamName << std::endl;
        }
    }

private:
    std::string _streamName;
    udp_dmi::system::Sender<DataTypeObject>::SharedPtr _systemObject;
    udp_dmi::system::Sender<DataTypeFreeSpace>::SharedPtr _systemFreeSpace;
    udp_dmi::system::Sender<DataTypeSignal>::SharedPtr _systemSignal;
    udp_dmi::system::Sender<DataTypeSensor>::SharedPtr _systemSensor;
};

extern "C" {

    /**
     * @brief SenderWrapperクラスのインスタンスを生成する。
     * @brief エントリーポイント
     *
     * @param callback
     * 
     * @return instance
     */
    intptr_t create(const std::string& stream, std::map<std::string, std::string>& configMap) {
        return reinterpret_cast<intptr_t>(new SenderWrapper(stream, configMap));
    }

    /**
     * @brief SenderWrapperクラスのspinメソッドをコールする。
     * @brief エントリーポイント
     *
     * @param ptr
     * 
     */
    void spin(intptr_t ptr) {
        reinterpret_cast<SenderWrapper*>(ptr)->spin();
    }

    /**
     * @brief SenderWrapperクラスのspinメソッドをコールする。
     * @brief エントリーポイント
     *
     * @param ptr
     * 
     */
    void sendTo(intptr_t ptr, IS::TupleSet& ts) {
        reinterpret_cast<SenderWrapper*>(ptr)->sendTo(ts);
    }

    /**
     * @brief SenderWrapperクラスのインスタンスを破棄する。
     * @brief エントリーポイント
     *
     * @param ptr
     * 
     */
    void destroy(intptr_t ptr) {
        delete reinterpret_cast<SenderWrapper*>(ptr);
    }
}
