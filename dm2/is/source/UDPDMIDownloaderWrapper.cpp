
#include "UDPDMIDownloaderWrapper.h"

using namespace std;

namespace IS {

    /**
     * コンストラクタ
     *
     * @author   Naoto Komatsu
     * @date     2023/12/28
     */
    UDPDMIDownloaderWrapper::UDPDMIDownloaderWrapper() {
    }

    /**
     * デストラクタ
     *
     * @author   Naoto Komatsu
     * @date     2023/12/28
     */
    UDPDMIDownloaderWrapper::~UDPDMIDownloaderWrapper() {
    }

    /**
     * 初期化処理
     *
     * @author   Naoto Komatsu
     * @date     2023/12/28
     */
    void UDPDMIDownloaderWrapper::init() {
        /* 共有ライブラリのロード */
        handle = dlopen(SNDWRP_SO_PATH.c_str(), RTLD_NOW);
        if (!handle) {
            const char* const error_message = dlerror();
            logger_ros->warn("Failed to load the shared object: " + SNDWRP_SO_PATH + " [detail_message:" + std::string(error_message) + "]");
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
        YAML::Node senderConfigYaml = udpConfigYaml["sender"];
        YAML::Node commonConfigYaml = node["common"];
        this->userYaml = commonConfigYaml["user"].as<std::string>();
        this->ipAddrYaml = commonConfigYaml["ipAddr"].as<std::string>();
        create_sender = reinterpret_cast<CreateSenderFuncPtr>(dlsym(handle, "create"));
        spin_ptr = reinterpret_cast<SpinFuncPtr>(dlsym(handle, "spin"));
        sndOnDmEvent = reinterpret_cast<SndOnDmEventFuncPtr>(dlsym(handle, "sendTo"));
        destroy = reinterpret_cast<DestroyFuncPtr>(dlsym(handle, "destroy"));

        if (!create_sender || !sndOnDmEvent || !spin_ptr || !destroy) {
            std::cerr << "Failed to locate functions in the shared object: " << dlerror() << std::endl;
            return;
        }

        /* yamlファイルから読み出した情報をmapに変換 */
        std::map<std::string, std::map<std::string, std::string>> configMap;
        yamlToMap(senderConfigYaml, configMap);

        /* mapからquery抽出 & sender生成 */
        std::vector<std::string> queryVector;
        std::vector<RecvData> queryRcvVector;
        for (auto& subMap : configMap) {
            queryVector.push_back(subMap.second["query"]);
        }
        /* query→XML変換 */
        queryRcvVector = prepareQuery(queryVector);
        /* 継続query登録処理 */
        registerQuery(queryRcvVector);

        for (auto& subMap : configMap) {
            /* sender生成 */
            sender = (*create_sender)(subMap.first, subMap.second);
            instanceMap.insert(std::make_pair(subMap.first, sender));
            senderList.push_back(subMap.first);
        }
        logger_ros->warn("# Started. Path[" + SNDWRP_SO_PATH + "]");

    }

    /**
     * UDP-DMIからのデータ受信スレッド起動
     *
     * @author   Naoto Komatsu
     * @date     2024/02/14
     */
    void UDPDMIDownloaderWrapper::start() {
        if (_readErr) return;
        for (auto& table : senderList) {
            threadMap.insert(std::make_pair(table, std::thread(&UDPDMIDownloaderWrapper::spin, this, table)));
            threadMap[table].detach();
        }
        //cout << "UDP-DMI_Downloader has started." << endl;
    }

    void UDPDMIDownloaderWrapper::spin(const std::string& streamName) {
        (*spin_ptr)(instanceMap[streamName]);
    }

    /**
     * デストラクタ
     *
     * @author   Naoto Komatsu
     * @date     2023/12/28
     */
    void UDPDMIDownloaderWrapper::stop() {
        if (_readErr) return;
        /*FIXME: Implicit cast to unsigned int.*/
        IS::QueryManager &QM = IS::QueryManager::get_instance();
        for (auto& table : senderList) {
            //(*destroy)(instanceMap[table]);
        }
        for (auto& mngId : this->_mngIdVector) {
            QM.cancelQuery(mngId);
        }
        dlclose(handle);

        //cout << "UDP-DMI_Downloader has finished." << endl;
    }

    /**
     * publish処理を行う
     *
     * @author   Naoto Komatsu
     * @date     2023/12/28
     */
    void UDPDMIDownloaderWrapper::sendTo(IS::TupleSet &tupleset, string &tableName) {
        sndOnDmEvent(instanceMap[tableName], tupleset);
    }

    vector<IS::Tuple> UDPDMIDownloaderWrapper::TupleSetToTuple(IS::TupleSet &tplst) {
        std::vector<IS::Tuple> tpl = tplst.getTuples();
        return tpl;
    }

    /**
     * Yamlから読み込んだ値をmapに格納
     *
     * @author   Naoto Komatsu
     * @date     2024/01/17
     */
    void UDPDMIDownloaderWrapper::yamlToMap(YAML::Node& readConfig, std::map<std::string, std::map<std::string, std::string>>& configMap) {
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

    std::vector<RecvData> UDPDMIDownloaderWrapper::prepareQuery(std::vector<std::string>& queryList)
    {
        RecvData info;
        std::vector<RecvData> infoVector;
        struct sockaddr_in addr;
        std::string queryXML = "";
        std::string dmiName = "udp";

        IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		isp.init();
        
        for (const std::string& query : queryList) {
            /* query→XML変換 */
            isp.createQueryXML(query, -1, "0", dmiName, queryXML);
            info.sock = -1;
            info.client = addr;
            info.payload = queryXML;
            info.isClose = false;
            infoVector.push_back(info);
        }
        isp.finalize();

        return infoVector;
    }

    void UDPDMIDownloaderWrapper::registerQuery(std::vector<RecvData> &dataVector)
	{
        for (const RecvData& data : dataVector) { 
            IS::QueryManager &QM = IS::QueryManager::get_instance();
            if (data.isClose) {
                // 継続クエリのキャンセル要求
                QM.cancelQuery(data);
                return;
            }
            IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
            isp.init();
            // XMLの種別を取得
            std::string rootTagName;
            if (!isp.getRootTagName(data.payload, rootTagName)) {
                isp.finalize();
                return;
            }

            if ("query" == rootTagName) {
                int mngId = QM.getMngId();
                this->_mngIdVector.push_back(mngId);
                QM.addQuery(mngId, this->userYaml, data, true);
            }
            else if ("cancel" == rootTagName) {
                // 継続クエリのキャンセル要求
                QM.cancelQuery(this->userYaml, data, isp.getAttrValueFromRootTag("id", data.payload));
            }
            else
            {
                //logger->error("[ERROR] Not Define RootTagName. : " + rootTagName);
            }
            isp.finalize();
        }
	}
};
