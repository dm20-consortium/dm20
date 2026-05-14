#include "ROS2DMIUploaderWrapper.h"

using CreateSubscriberFuncPtr = intptr_t (*)(std::map<std::string, std::map<std::string, std::string>>&, void (ROS2DMIUploaderWrapper::*)(std::vector<Tuple>&));
using SpinSomeFuncPtr = void (*)(intptr_t);
using DestroyFuncPtr = void (*)(intptr_t);

string _ipAddr;
string _userName;
namespace IS {
    /**
     * コンストラクタ
     *
     * @author   Naoto Komatsu
     * @date     2023/12/06
     */
    ROS2DMIUploaderWrapper::ROS2DMIUploaderWrapper() : ext_flg_(true) {
    }

    /**
     * デストラクタ
     *
     * @author   Naoto Komatsu
     * @date     2023/12/06
     */
    ROS2DMIUploaderWrapper::~ROS2DMIUploaderWrapper() {
    }

    /**
     * 初期化処理
     *
     * @author   Naoto Komatsu
     * @date     2023/12/06
     */
    void ROS2DMIUploaderWrapper::init() {
        std::string ros2ws_path = "";
#if defined(ROS2_WORKSPACE_DIR)
        ros2ws_path = ROS2_WORKSPACE_DIR;
#endif
        const std::string libpath = ros2ws_path + std::string("/build/ros2_dmi/libros2_dmi_subscriber_wrapper.so");
        if (!settings.IsFileExist(libpath)) {
            logger_ros->warn("open failed libpath: " + libpath);
            _readErr = true;
            return;
        }
        SUBWRP_SO_PATH = libpath;

        /* 設定ファイル読み込み */
        std::string yamlPath = settings.getConfigDirectory() + settings.CONF_YAML_DMI;
        if (!settings.IsFileExist(yamlPath)) {
            logger_ros->warn("open failed YamlFile: " + yamlPath);
            _readErr = true;
            return;
        }
        YAML::Node node = YAML::LoadFile(yamlPath);
        YAML::Node dmiConfigYaml = node["dmi"];
        YAML::Node ros2ConfigYaml = dmiConfigYaml["ros2"];
        subscriberConfigYaml = ros2ConfigYaml["subscriber"];
        YAML::Node commonConfigYaml = node["common"];
        _ipAddr = commonConfigYaml["ipAddr"].as<std::string>();
        _userName = commonConfigYaml["user"].as<std::string>();
    }

    /**
     * ROS2DMIからのデータ受信スレッド起動
     *
     * @author   Naoto Komatsu
     * @date     2023/12/06
     */
    void ROS2DMIUploaderWrapper::start() {
        if (_readErr) return;
        //signal(SIGINT, &ROS2DMIUploaderWrapper::handler);
        thread uploaderCallThread(&ROS2DMIUploaderWrapper::uploaderCall, this);
        uploaderCallThread.detach();
        //cout << "ROS2-DMI_Uploader has started." << endl;
    }

    /**
     * 終了処理
     *
     * @author   Naoto Komatsu
     * @date     2023/12/06
     */
    void ROS2DMIUploaderWrapper::stop() {
        if (_readErr) return;
        ext_flg_ = false;
    }
    /**
     * アップローダー呼び出し処理
     *
     * @author   Naoto Komatsu
     * @date     2023/12/06
     */
    void ROS2DMIUploaderWrapper::uploaderCall() {
        void* handle = dlopen(SUBWRP_SO_PATH.c_str(), RTLD_NOW);
        if (!handle) {
            const char* const error_message = dlerror();
            logger_ros->warn("Failed to load the shared object: " + SUBWRP_SO_PATH + " [detail_message:" + std::string(error_message) + "]");
            return;
        }
        logger_ros->info("found ros2dmi plugin:" + SUBWRP_SO_PATH);
        CreateSubscriberFuncPtr create_subscriber = reinterpret_cast<CreateSubscriberFuncPtr>(dlsym(handle, "create"));
        SpinSomeFuncPtr spin_some = reinterpret_cast<SpinSomeFuncPtr>(dlsym(handle, "spin"));
        DestroyFuncPtr destroy = reinterpret_cast<DestroyFuncPtr>(dlsym(handle, "destroy"));

        if (!create_subscriber || !spin_some || !destroy) {
            const char* const error_message = dlerror();
            logger_ros->warn("dlerror_message:" + std::string(error_message));
            return;
        }

        std::map<std::string, std::map<std::string, std::string>> configMap;
        yamlToMap(subscriberConfigYaml, configMap);
        intptr_t subscriber = (*create_subscriber)(configMap, &ROS2DMIUploaderWrapper::subscribeCallback);
        while (ext_flg_) {
            (*spin_some)(subscriber);
        }
        (*destroy)(subscriber);
        dlclose(handle);
    }

    /**
     * キューの追加処理(Subscribe時のコールバック処理)
     *
     * @author   Naoto Komatsu
     * @date     2023/12/06
     */
    void ROS2DMIUploaderWrapper::subscribeCallback(std::vector<Tuple> &tuples) {
        IS::QueueManager &QM = IS::QueueManager::get_instance();
        QM.addQueueProc(tuples, _ipAddr, _userName);
    }

    /**
     * Yamlから読み込んだ値をmapに格納
     *
     * @author   Naoto Komatsu
     * @date     2024/01/17
     */
    void ROS2DMIUploaderWrapper::yamlToMap(YAML::Node& readConfig, std::map<std::string, std::map<std::string, std::string>>& configMap) {
        for (const auto& firstKey : readConfig) {
            const std::string& table = firstKey.first.as<std::string>();
            const YAML::Node& subConfigYaml = firstKey.second;

            std::map<std::string, std::string> subConfigMap;
            for (const auto& secondKey : subConfigYaml) {
                subConfigMap[secondKey.first.as<std::string>()] = subConfigYaml[secondKey.first.as<std::string>()].as<std::string>();
            }

            configMap[table] = subConfigMap;
        }
    }
}
