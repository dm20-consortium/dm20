#include "UDPDMIUploaderWrapper.h"

namespace IS {
    const std::string UDPDMIUploaderWrapper::RCVWRP_SO_PATH = "/usr/local/lib/libudp_dmi_receiver_wrapper.so";
    std::string userName;
    std::string ipAddr;
    /**
     * コンストラクタ
     *
     * @author   Naoto Komatsu
     * @date     2024/02/14
     */
    UDPDMIUploaderWrapper::UDPDMIUploaderWrapper() {
    }

    /**
     * デストラクタ
     *
     * @author   Naoto Komatsu
     * @date     2024/02/14
     */
    UDPDMIUploaderWrapper::~UDPDMIUploaderWrapper() {
    }

    /**
     * 初期化処理
     *
     * @author   Naoto Komatsu
     * @date     2024/02/14
     */
    void UDPDMIUploaderWrapper::init() {
        void* handle = dlopen(RCVWRP_SO_PATH.c_str(), RTLD_NOW);
        if (!handle) {
            const char* const error_message = dlerror();
            logger_ros->warn("Failed to load the shared object: " + RCVWRP_SO_PATH + " [detail_message:" + std::string(error_message) + "]");
            _readErr = true;
            return;
        }
        
        CreateReceiverFuncPtr create_receiver = reinterpret_cast<CreateReceiverFuncPtr>(dlsym(handle, "create"));
        SpinFuncPtr spin_func = reinterpret_cast<SpinFuncPtr>(dlsym(handle, "spin"));
        this->spin_ = spin_func;
        DestroyFuncPtr destroy = reinterpret_cast<DestroyFuncPtr>(dlsym(handle, "destroy"));

        if (!create_receiver || !spin_func || !destroy) {
            std::cerr << "Failed to locate functions in the shared object: " << dlerror() << std::endl;
            _readErr = true;
            return;
        }

        /* 設定ファイル読み込み */
        std::string yamlPath = settings.getConfigDirectory() + settings.CONF_YAML_DMI;
        if (!settings.IsFileExist(yamlPath)) {
            logger_ros->warn("open failed YamlFile: " + yamlPath);
            _readErr = true;
            dlclose(handle);
            return;
        }
        YAML::Node node = YAML::LoadFile(yamlPath);
        YAML::Node dmiConfigYaml = node["dmi"];
        YAML::Node udpConfigYaml = dmiConfigYaml["udp"];
        YAML::Node receiverConfigYaml = udpConfigYaml["receiver"];
        YAML::Node commonConfigYaml = node["common"];
        std::string ip_addr = commonConfigYaml["ipAddr"].as<std::string>();
        std::string user_name = commonConfigYaml["user"].as<std::string>();
        ipAddr = ip_addr;
        userName = user_name;

        std::map<std::string, std::map<std::string, std::string>> configMap;
        yamlToMap(receiverConfigYaml, configMap);

        for (auto& subMap : configMap) {
            int portStr = stoi(subMap.second["receptionPort"]);
            intptr_t receiver = (*create_receiver)(portStr, subMap.second["targetTable"], &UDPDMIUploaderWrapper::eventCallback);
            tableList.push_back(subMap.second["targetTable"]);
            instanceMap.insert(std::make_pair(subMap.second["targetTable"], receiver));
        }
        logger_ros->warn("# Started. Path[" + RCVWRP_SO_PATH + "]");

        dlclose(handle);
    }

    /**
     * UDP-DMIからのデータ受信スレッド起動
     *
     * @author   Naoto Komatsu
     * @date     2024/02/14
     */
    void UDPDMIUploaderWrapper::start() {
        if (_readErr) return;
        for (auto& table : tableList) {
            threadMap.insert(std::make_pair(table, std::thread(&UDPDMIUploaderWrapper::spin, this, table)));
            threadMap[table].detach();
        }
        //cout << "UDP-DMI_Uploader has started." << endl;
    }

    /**
     * 終了処理
     *
     * @author   Naoto Komatsu
     * @date     2024/02/14
     */
    void UDPDMIUploaderWrapper::stop() {
    }

    void UDPDMIUploaderWrapper::spin(const std::string& streamName) {
        (*spin_)(instanceMap[streamName]);
    }

    /**
     * キューの追加処理(コールバック処理)
     *
     * @author   Naoto Komatsu
     * @date     2024/02/14
     */
    void UDPDMIUploaderWrapper::eventCallback(std::vector<Tuple> &tuples) {
        IS::QueueManager &QM = IS::QueueManager::get_instance();
        QM.addQueueProc(tuples, ipAddr, userName);
    }

    /**
     * Yamlから読み込んだ値をmapに格納
     *
     * @author   Naoto Komatsu
     * @date     2024/01/17
     */
    void UDPDMIUploaderWrapper::yamlToMap(YAML::Node& readConfig, std::map<std::string, std::map<std::string, std::string>>& configMap) {
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
