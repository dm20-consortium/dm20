#ifndef UDPDMIUPLOADERWRAPPER_H
#define UDPDMIUPLOADERWRAPPER_H


#include <dlfcn.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <map>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include "QueueManager.h"

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
using namespace log4cxx;

namespace IS {
    class UDPDMIUploaderWrapper {
    public:
        UDPDMIUploaderWrapper();
        ~UDPDMIUploaderWrapper();
        void init();
        void start();
        void stop();

    private:
        LoggerPtr logger_ros = Logger::getLogger("UDPDMIUploaderWrapper");
        bool _readErr = false;
		IS::Settings &settings = IS::Settings::get_instance();
        static const std::string RCVWRP_SO_PATH;
        using CallbackType = std::function<void (std::vector<IS::Tuple>&)>;
        //using CreateReceiverFuncPtr = intptr_t (*)(int&, CallbackType*);
        using CreateReceiverFuncPtr = intptr_t (*)(int&, std::string&, void (UDPDMIUploaderWrapper::*)(std::vector<Tuple>&));
        using SpinFuncPtr = void (*)(intptr_t);
        using DestroyFuncPtr = void (*)(intptr_t);
        SpinFuncPtr spin_;
        std::unordered_map<std::string, std::thread> threadMap;
        std::unordered_map<std::string, intptr_t> instanceMap;
        std::vector<std::string> tableList;
        void spin(const std::string& streamName);
        void eventCallback(std::vector<Tuple>& tuples);
        void yamlToMap(YAML::Node& readConfig, std::map<std::string, std::map<std::string, std::string>>& configMap);
    };
}
#endif
