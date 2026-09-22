
#include "ROS2DMIDownloaderWrapper.h"

using namespace std;

namespace IS {

    /**
     * コンストラクタ
     *
     * @author   Naoto Komatsu
     * @date     2023/12/28
     */
    ROS2DMIDownloaderWrapper::ROS2DMIDownloaderWrapper() {
    }

    /**
     * デストラクタ
     *
     * @author   Naoto Komatsu
     * @date     2023/12/28
     */
    ROS2DMIDownloaderWrapper::~ROS2DMIDownloaderWrapper() {
    }

    /**
     * 初期化処理
     *
     * @author   Naoto Komatsu
     * @date     2023/12/28
     */
    void ROS2DMIDownloaderWrapper::init() {
        /* 共有ライブラリのロード */
        handle = dlopen(PUBWRP_SO_PATH.c_str(), RTLD_NOW);
        if (!handle) {
            const char* const error_message = dlerror();
            logger_ros->warn("Failed to load the shared object: " + PUBWRP_SO_PATH + " [detail_message:" + std::string(error_message) + "]");
            _readErr = true;
            return;
        }
        logger_ros->info("found ros2dmi plugin:" + PUBWRP_SO_PATH);

        /* 設定ファイル読み込み */
        std::string yamlPath = settings.getConfigDirectory() + settings.CONF_YAML_DMI;
        if (!settings.IsFileExist(yamlPath)) {
            logger_ros->warn("open failed YamlFile: " + yamlPath);
            dlclose(handle);
            _readErr = true;
            return;
        }
        YAML::Node node = YAML::LoadFile(yamlPath);
        YAML::Node dmiConfigYaml = node["dmi"];
        YAML::Node ros2ConfigYaml = dmiConfigYaml["ros2"];
        YAML::Node publisherConfigYaml = ros2ConfigYaml["publisher"];
        YAML::Node commonConfigYaml = node["common"];
        this->userYaml = commonConfigYaml["user"].as<std::string>();
        this->ipAddrYaml = commonConfigYaml["ipAddr"].as<std::string>();

        create_publisher = reinterpret_cast<CreatePublisherFuncPtr>(dlsym(handle, "create"));
        pubOnDmEvent = reinterpret_cast<PubOnDmEventFuncPtr>(dlsym(handle, "pub_on_dm_event"));
        destroy = reinterpret_cast<DestroyFuncPtr>(dlsym(handle, "destroy"));

        if (!create_publisher || !pubOnDmEvent || !destroy) {
            std::cerr << "Failed to locate functions in the shared object: " << dlerror() << std::endl;
            return;
        }

        /* yamlファイルから読み出した情報をmapに変換 */
        std::map<std::string, std::map<std::string, std::string>> configMap;
        yamlToMap(publisherConfigYaml, configMap);

        /* mapからquery抽出 */
        std::vector<std::string> queryVector;
        for (auto& subMap : configMap) {
            queryVector.push_back(subMap.second["query"]);
        }
        /* 継続query登録処理 */
        registerQuery(queryVector);

        /* publisher */
        publisher = (*create_publisher)(configMap);

    }

    /**
     * デストラクタ
     *
     * @author   Naoto Komatsu
     * @date     2023/12/28
     */
    void ROS2DMIDownloaderWrapper::stop() {
        if (_readErr) return;
        /*FIXME: Implicit cast to unsigned int.*/
        IS::QueryManager &QM = IS::QueryManager::get_instance();
        QM.cancelQuery(this->_mngId);
        (*destroy)(publisher);
        dlclose(handle);
    }

    /**
     * publish処理を行う
     *
     * @author   Naoto Komatsu
     * @date     2023/12/28
     */
    void ROS2DMIDownloaderWrapper::publishTo(IS::TupleSet &tupleset, string &tableName) {
        vector<IS::Tuple> tuples;
        //tuples = TupleSetToTuple(tupleset);
        pubOnDmEvent(publisher, tableName, tupleset);
    }

    vector<IS::Tuple> ROS2DMIDownloaderWrapper::TupleSetToTuple(IS::TupleSet &tplst) {
        std::vector<IS::Tuple> tpl = tplst.getTuples();
        return tpl;
    }

    /**
     * Yamlから読み込んだ値をmapに格納
     *
     * @author   Naoto Komatsu
     * @date     2024/01/17
     */
    void ROS2DMIDownloaderWrapper::yamlToMap(YAML::Node& readConfig, std::map<std::string, std::map<std::string, std::string>>& configMap) {
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

    void ROS2DMIDownloaderWrapper::registerQuery(std::vector<std::string>& queryList)
	{
        const std::string dmiName = "ros";
        for (const std::string& query : queryList) {
            IS::QueryManager &QM = IS::QueryManager::get_instance();
            const int mngId = QM.getMngId();
            _mngId = mngId;
            QM.addQuery(mngId, this->userYaml, query, dmiName);
        }
	}
};
