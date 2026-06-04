#ifndef ROS2DMIDOWNLOADERWRAPPER_H
#define ROS2DMIDOWNLOADERWRAPPER_H

#define SUB_FLG         1
#define PUB_TABLE_NUM   4

#include <iostream>
#include <thread>
#include <csignal>
#include <dlfcn.h>
#include <functional>
#include <map>
#include <yaml-cpp/yaml.h>

#include "QueryManager.h"
#include "is/Tuple.h"
#include "is/TupleSet.h"

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
using namespace log4cxx;

namespace IS {
    class ROS2DMIDownloaderWrapper {
    public:
        ROS2DMIDownloaderWrapper();
        ~ROS2DMIDownloaderWrapper();
        void init();
        void stop();
        static ROS2DMIDownloaderWrapper& get_instance() {
            static ROS2DMIDownloaderWrapper inst;
            return inst;
        }
        void publishTo(IS::TupleSet &tupleset, string &tableName);

#if defined(ROS2_WORKSPACE_DIR)
        const std::string ros2ws_path = ROS2_WORKSPACE_DIR;
#else
        const std::string ros2ws_path = "";
#endif
        const std::string libpath = ros2ws_path + std::string("/build/ros2_dmi/libros2_dmi_publisher_wrapper.so");
        const std::string PUBWRP_SO_PATH = libpath;
        using CreatePublisherFuncPtr = intptr_t (*)(std::map<std::string, std::map<std::string, std::string>>&);
        using PubOnDmEventFuncPtr = void (*)(intptr_t, std::string&, IS::TupleSet&);
        using DestroyFuncPtr = void (*)(intptr_t);

        void* handle;
        std::string userYaml;
        std::string ipAddrYaml;
        CreatePublisherFuncPtr create_publisher;
        PubOnDmEventFuncPtr pubOnDmEvent;
        DestroyFuncPtr destroy;
        intptr_t publisher;
        std::string table_list[PUB_TABLE_NUM] = {
            "object_info",
            "freespace_info",
            "signal_info",
            "pole_info"
        };

    private:
        LoggerPtr logger_ros = Logger::getLogger("ROS2DMIDownloaderWrapper");
        bool _readErr = false;
		IS::Settings &settings = IS::Settings::get_instance();
        vector<IS::Tuple> TupleSetToTuple(IS::TupleSet &tplst);
        void yamlToMap(YAML::Node& readConfig, std::map<std::string, std::map<std::string, std::string>>& configMap);
        void registerQuery(std::vector<std::string>& query);
        int _mngId;
    };
};

#endif
