#ifndef ROS2DMIUPLOADERWRAPPER_H
#define ROS2DMIUPLOADERWRAPPER_H


#include <dlfcn.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <map>
#include <yaml-cpp/yaml.h>
#include "QueueManager.h"

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
using namespace log4cxx;

namespace IS {
    class ROS2DMIUploaderWrapper {
    public:
        ROS2DMIUploaderWrapper();
        ~ROS2DMIUploaderWrapper();
        void init();
        void start();
        void stop();

    private:
        LoggerPtr logger_ros = Logger::getLogger("ROS2DMIUploaderWrapper");
        bool _readErr = false;
        YAML::Node subscriberConfigYaml;
        
		IS::Settings &settings = IS::Settings::get_instance();
        volatile std::sig_atomic_t ext_flg_;
        std::string SUBWRP_SO_PATH;

        void uploaderCall();
        void subscribeCallback(std::vector<Tuple>& tuples);
        void yamlToMap(YAML::Node& readConfig, std::map<std::string, std::map<std::string, std::string>>& configMap);
    };
}
#endif
