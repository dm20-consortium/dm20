#ifndef UDPDMIDOWNLOADERWRAPPER_H
#define UDPDMIDOWNLOADERWRAPPER_H

#define SUB_FLG         1
#define PUB_TABLE_NUM   4

#include <iostream>
#include <thread>
#include <csignal>
#include <dlfcn.h>
#include <functional>
#include <map>
#include <unordered_map>
#include <vector>
#include <yaml-cpp/yaml.h>

#include "QueryManager.h"
#include "is/Tuple.h"
#include "is/TupleSet.h"

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
using namespace log4cxx;

namespace IS {
    class UDPDMIDownloaderWrapper {
    public:
        UDPDMIDownloaderWrapper();
        ~UDPDMIDownloaderWrapper();
        void init();
        void start();
        void stop();
        static UDPDMIDownloaderWrapper& get_instance() {
            static UDPDMIDownloaderWrapper inst;
            return inst;
        }
        void sendTo(IS::TupleSet &tupleset, string &tableName);

        const std::string SNDWRP_SO_PATH = "/usr/local/lib/libudp_dmi_sender_wrapper.so";
        using CreateSenderFuncPtr = intptr_t (*)(const std::string&, std::map<std::string, std::string>&);
        using SpinFuncPtr = void (*)(intptr_t);
        using SndOnDmEventFuncPtr = void (*)(intptr_t, IS::TupleSet&);
        using DestroyFuncPtr = void (*)(intptr_t);

        void* handle;
        std::string userYaml;
        std::string ipAddrYaml;
        CreateSenderFuncPtr create_sender;
        SpinFuncPtr spin_ptr;
        SndOnDmEventFuncPtr sndOnDmEvent;
        DestroyFuncPtr destroy;
        intptr_t sender;
        std::vector<std::string> senderList;
        std::map<std::string, std::thread> threadMap;
        std::map<std::string, intptr_t> instanceMap;

    private:
        LoggerPtr logger_ros = Logger::getLogger("UDPDMIDownloaderWrapper");
        bool _readErr = false;
		IS::Settings &settings = IS::Settings::get_instance();
        void spin(const std::string& streamName);
        vector<IS::Tuple> TupleSetToTuple(IS::TupleSet &tplst);
        void yamlToMap(YAML::Node& readConfig, std::map<std::string, std::map<std::string, std::string>>& configMap);
        void registerQuery(std::vector<RecvData> &dataVector);
        std::vector<RecvData> prepareQuery(std::vector<std::string>& query);
        std::vector<int> _mngIdVector;
    };
};

#endif
