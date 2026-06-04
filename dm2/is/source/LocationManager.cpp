#include "LocationManager.h"
#include "QueueManager.h"
#include "QueryManager.h"

using namespace std;
using namespace pqxx;

namespace IS {

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2019/12/11
	 *
	 * @param 		  	tranOperatorTreeXML   	転送オペレーターツリーXML
	 * @param 		  	range 					データ取得要求範囲
	 * @param 		  	requestedVehicleId		要求車両ID
	 * @param 		  	receptionMngId			受付クエリ管理番号
	 * @param 		  	receptionEdgeId			受付エッジID
	 */

	//PosMng::PosMng(const string &tranOperatorTreeXML, const unsigned int &range, const unsigned int requestedVehicleId, const unsigned int receptionMngId, const unsigned int receptionEdgeId)
	PosMng::PosMng(const string &tranOperatorTreeXML, const unsigned int &range, const unsigned long long requestedVehicleId, const unsigned int receptionMngId, const unsigned long long receptionEdgeId)
	{
		Info.clear();
		PosMng::MngInfo mngInfo;
		mngInfo.requestedVehicleId = requestedVehicleId;
		mngInfo.range = range;
		mngInfo.receptionEdgeId = receptionEdgeId;
		mngInfo.receptionMngId = receptionMngId;
		mngInfo.tranOperatorTreeXML = tranOperatorTreeXML;
		this->Info.insert(std::pair<string, PosMng::MngInfo>(to_string(receptionEdgeId) + "_" + to_string(receptionMngId), mngInfo));
	}

	/**
	* 連携エッジSIDのカンマ区切り文字列を取得する
	*
	* @author	Nagoya University
	* @date	2020/01/06
	*
	* @return	EdgeSIDList文字列
	*/

	string PosMng::getTranEdgeSID(const string &mngKey)
	{
		string ret = "";
		for (TranInfo tranInfo : this->Info.at(mngKey).tranInfoList)
		{
			if (ret.length() != 0) ret.append(",");
			ret.append(to_string(tranInfo.edgeSID));
		}
		return ret;
	}
	/**
	 * 初期化処理
	 *
	 * @author	Nagoya University
	 * @date	2019/12/19
	 */

	void LocationManager::init()
	{
		p_mp.clear();
		pthread_mutex_init(&P_MAP_MTX, NULL);

		// TODO:暫定対応(CSが担当レーン情報をRDBに書き込む対応をした場合は別途対応
		//string path = settings.getConfigDirectory() + settings.CONF_FILE_CS_MAP;
		//lid_controll_edge_sid_map = loadLidCtrlCsv(path.c_str());

		// 自身の位置把握のためのストリームを抽出
		IS::QueueManager &QueuM = IS::QueueManager::get_instance();
		sidSchemaMap = QueuM.getSidSchemaMap();
		laneIdSchemaMap = QueuM.getLaneIdSchemaMap();
		for (auto sidItr = sidSchemaMap.begin(); sidItr != sidSchemaMap.end(); ++sidItr) {
			for (auto lidItr = laneIdSchemaMap.begin(); lidItr != laneIdSchemaMap.end(); ++lidItr) {
				if (sidItr->first == lidItr->first) {
					targetStreamList.push_back(sidItr->first);
				}
			}
		}
		if (settings.getSIDType() != Settings::SID_TYPE::CAR) {
			string intervalStr = settings.getParameter("MONITORING_OUT_OF_AREA_INTERVAL_TIME");
			if (intervalStr.length() == 0) intervalStr = "2000";
			// エリア外監視スレッドの起動
			this->tw = new TimerWorker(stoi(intervalStr), std::bind(&LocationManager::checkOutOfAreaProc, this));
		}
	}

	/**
	 * 処理の停止
	 *
	 * @author	Nagoya University
	 * @date	2019/12/19
	 */

	void LocationManager::stop()
	{
		p_mp.clear();
		pthread_mutex_destroy(&P_MAP_MTX);

		if (this->tw != NULL) {
			this->tw->abort_thread();
			delete tw;
			tw = NULL;
		}
	}

	/**
	* 担当レーン設定CSVをロードする
	*
	* @author	Nagoya University
	* @date	2019/12/19
	*
	* @param [in]	filename	ファイル名
	*
	*/
	//unordered_map<int, int> LocationManager::loadLidCtrlCsv(const char* filename)
	unordered_map<unsigned long long, unsigned long long> LocationManager::loadLidCtrlCsv(const char* filename)
	{
		//unordered_map<int, int> retMap;
		unordered_map<unsigned long long, unsigned long long> retMap;
		std::ifstream ifs(filename);
		if (!ifs) {
			logger->error("[loadLidCtrlCsv] Failed to read file :" + std::string(filename));
		}
		std::string str;
		std::string token;
		int clm = 0;
		//int laneId = 0;
		//int sid;
		unsigned long long laneId = 0;
		unsigned long long sid;
		while (getline(ifs, str)) {
			token.clear();
			std::istringstream stream(str);
			clm = 0;
			while (getline(stream, token, ',')) {
				if (clm == 0) {
					//laneId = stoi(token);
					laneId = stoull(token);
					clm++;
				}
				else {
					//sid = stoi(token);
					sid = stoull(token);
					retMap[laneId] = sid;
					clm--;
				}
			}
		}
		return retMap;
	}

	/**
	* 車両のデータ要求領域をチェックする
	*
	* @author	Nagoya University
	* @date	2019/12/19
	*
	* @param [in]	vehicleSID	車両ID
	*
	*/
	//void LocationManager::checkAreaOfMonitoredVehicle(unsigned int vehicleSID)
	void LocationManager::checkAreaOfMonitoredVehicle(unsigned long long vehicleSID)
	{
		logger->trace("[checkAreaOfMonitoredVehicle] IN");
		IS::QueryManager &QeryM = IS::QueryManager::get_instance();
		//vector<unsigned int> addRequestEdgeSID;
		//vector<unsigned int> cancelRequestEdgeSID;
		vector<unsigned long long> addRequestEdgeSID;
		vector<unsigned long long> cancelRequestEdgeSID;
		vector<string> mngKeyList;
		unsigned int myMngId = 0;

		pthread_mutex_lock(&P_MAP_MTX);
		for (auto i = p_mp.at(vehicleSID).Info.begin(); i != p_mp.at(vehicleSID).Info.end(); ++i) { mngKeyList.push_back(i->first); }
		pthread_mutex_unlock(&P_MAP_MTX);

		for (string mngKey : mngKeyList)
		{
			// データ要求範囲に含まれるエッジSIDを取得する
			//unordered_map<unsigned int, unsigned int> edgeMap = getNearEdgeSID(vehicleSID, mngKey);
			unordered_map<unsigned long long, unsigned long long> edgeMap = getNearEdgeSID(vehicleSID, mngKey);

			pthread_mutex_lock(&P_MAP_MTX);
			for (PosMng::TranInfo tranInfo : p_mp.at(vehicleSID).Info.at(mngKey).tranInfoList)
			{
				//if (tranInfo.edgeSID == static_cast<unsigned int>(stoi(settings.getParameter("MY_STATION_ID")))) {
				if (tranInfo.edgeSID == static_cast<unsigned long long>(stoull(settings.getParameter("MY_STATION_ID")))) {
					myMngId = tranInfo.mngID;
					break;
				}
			}
			// 変化の有無を判定する
			// 追加判定
			for (auto i = edgeMap.begin(); i != edgeMap.end(); ++i) {
				unsigned int idx = 0;
				for (; idx < p_mp.at(vehicleSID).Info.at(mngKey).tranInfoList.size(); idx++)
				{
					if (i->first == p_mp.at(vehicleSID).Info.at(mngKey).tranInfoList.at(idx).edgeSID) break;
				}
				if (idx == p_mp.at(vehicleSID).Info.at(mngKey).tranInfoList.size()) {
					// 見つからない場合は追加要求する
					addRequestEdgeSID.push_back(i->first);
					PosMng::TranInfo tranInfo;
					tranInfo.edgeSID = i->first;
					tranInfo.mngID = 0;
					p_mp.at(vehicleSID).Info.at(mngKey).tranInfoList.push_back(tranInfo);
					logger->info("[checkAreaOfMonitoredVehicle] New edge added to monitoring area. vehicleSID:" + to_string(vehicleSID) + " AddEdgeSID:" + to_string(i->first));
				}
				else if (p_mp.at(vehicleSID).Info.at(mngKey).tranInfoList.at(idx).mngID == 0) {
					// 見つかるが管理IDが"0"(未要求)の場合は追加要求する
					addRequestEdgeSID.push_back(i->first);
					logger->info("[checkAreaOfMonitoredVehicle] New edge updated to monitoring area. vehicleSID:" + to_string(vehicleSID) + " AddEdgeSID:" + to_string(i->first));
				}
			}
			// 削除判定
			for (auto it = p_mp.at(vehicleSID).Info.at(mngKey).tranInfoList.begin(); it != p_mp.at(vehicleSID).Info.at(mngKey).tranInfoList.end();) {
				PosMng::TranInfo tranInfo = *it;
				if (edgeMap.count(tranInfo.edgeSID) == 0) {
					// 見つからない場合は削除要求する
					cancelRequestEdgeSID.push_back(tranInfo.edgeSID);
					it = p_mp.at(vehicleSID).Info.at(mngKey).tranInfoList.erase(it);
					logger->info("[checkAreaOfMonitoredVehicle] Edge removed from monitoring area. vehicleSID:" + to_string(vehicleSID) + " removeEdgeSID:" + to_string(tranInfo.edgeSID));
				}
				else {
					++it;
				}
			}
			PosMng::MngInfo targetMngInfo = p_mp.at(vehicleSID).Info.at(mngKey);
			pthread_mutex_unlock(&P_MAP_MTX);

			//for (unsigned int edgeSID : addRequestEdgeSID) {
			for (unsigned long long edgeSID : addRequestEdgeSID) {
				QeryM.addRequest(myMngId, edgeSID, targetMngInfo, p_mp.at(vehicleSID).getTranEdgeSID(mngKey));
			}
			//for (unsigned int edgeSID : cancelRequestEdgeSID) {
			for (unsigned long long edgeSID : cancelRequestEdgeSID) {
				QeryM.cancelRequest(myMngId, edgeSID, targetMngInfo);
			}
		}
		logger->trace("[checkAreaOfMonitoredVehicle] OUT");
	}

	/**
	* レーンIDを設定する
	*
	* @author	Nagoya University
	* @date	2019/12/19
	*
	* @return	レーンID
	*/
	//void LocationManager::setLaneId(int laneId)
	void LocationManager::setLaneId(unsigned long long laneId)
	{
		this->laneId = laneId;
	}

	/**
	* 位置管理タスクを追加する
	*
	* @author	Nagoya University
	* @date	2019/12/19
	*
	* @param	vehicleSID	車両SID
	* @param	posMng		位置情報
	*/
	//void LocationManager::addLocationManagementTask(unsigned int vehicleSID, PosMng posMng)
	void LocationManager::addLocationManagementTask(unsigned long long vehicleSID, PosMng posMng)
	{
		logger->info("[addLocationManagementTask] Register Task vehicleID:" + to_string(vehicleSID));
		for (auto i = posMng.Info.begin(); i != posMng.Info.end(); ++i) {
			logger->info("[addLocationManagementTask]               Info[" + i->first + "] :");
			logger->info("[addLocationManagementTask]                 requestedVehicleId:" + to_string(i->second.requestedVehicleId));
			logger->info("[addLocationManagementTask]                 range:" + to_string(i->second.range));
			logger->info("[addLocationManagementTask]                 reception EdgeId:" + to_string(i->second.receptionEdgeId));
			logger->info("[addLocationManagementTask]                           MngId :" + to_string(i->second.receptionMngId));
			logger->info("[addLocationManagementTask]                 tranInfoList size:" + to_string(i->second.tranInfoList.size()));
			for (PosMng::TranInfo tranInfo : i->second.tranInfoList)
			{
				logger->info("[addLocationManagementTask]                      edgeSID:" + to_string(tranInfo.edgeSID) + " mngID:" + to_string(tranInfo.mngID));
			}
		}

		pthread_mutex_lock(&P_MAP_MTX);
		//p_mp.insert(std::pair<unsigned int, IS::PosMng>(vehicleSID, posMng));
		p_mp.insert(std::pair<unsigned long long, IS::PosMng>(vehicleSID, posMng));
		pthread_mutex_unlock(&P_MAP_MTX);
	}

	/**
	* 転送情報を更新する
	*
	* @author	Nagoya University
	* @date	2019/12/19
	*
	* @param	vehicleSID	車両SID
	* @param	mngKey		管理キー
	* @param	tranInfo	転送情報
	*/
	//void LocationManager::addTranInfo(unsigned int vehicleSID, const string &mngKey, PosMng::TranInfo tranInfo)
	void LocationManager::addTranInfo(unsigned long long vehicleSID, const string &mngKey, PosMng::TranInfo tranInfo)
	{
		pthread_mutex_lock(&P_MAP_MTX);
		if (p_mp.count(vehicleSID) != 0 && p_mp.at(vehicleSID).Info.count(mngKey) != 0) {
			p_mp.at(vehicleSID).Info.at(mngKey).tranInfoList.push_back(tranInfo);
		}
		pthread_mutex_unlock(&P_MAP_MTX);
	}

	/**
	* 受信したエッジ毎のクエリ管理番号を登録する
	*
	* @author	Nagoya University
	* @date	2020/01/06
	*
	* @param	mngKey	管理キー
	* @param	edgeSID	エッジSID
	* @param	mngId	クエリ管理番号
	*/
	//void LocationManager::updateTranMngId(const string &mngKey, unsigned int edgeSID, unsigned int mngId)
	void LocationManager::updateTranMngId(const string &mngKey, unsigned long long edgeSID, unsigned int mngId)
	{
		pthread_mutex_lock(&P_MAP_MTX);
		for (auto i = p_mp.begin(); i != p_mp.end(); ++i) {
			if (i->second.Info.count(mngKey) != 0) {
				for (unsigned int idx = 0; idx < i->second.Info.at(mngKey).tranInfoList.size(); idx++)
				{
					if (i->second.Info.at(mngKey).tranInfoList.at(idx).edgeSID == edgeSID)
					{
						logger->debug("[updateTranMngId] update mngId:" + to_string(i->second.Info.at(mngKey).tranInfoList.at(idx).mngID) + " -> " + to_string(mngId) + " VehicleID:" + to_string(i->first) + " mngKey:" + mngKey + " edgeSID:" + to_string(edgeSID));
						i->second.Info.at(mngKey).tranInfoList.at(idx).mngID = mngId;
					}
					
				}
			}
		}
		pthread_mutex_unlock(&P_MAP_MTX);
	}

	/**
	* レーンIDを更新する
	*
	* @author	Nagoya University
	* @date	2019/12/19
	*
	* @param	vehicleSID	車両SID
	* @param	lane_id		レーンID
	*/
	//void LocationManager::updateLaneId(unsigned int vehicleSID, int lane_id)
	void LocationManager::updateLaneId(unsigned long long vehicleSID, unsigned long long lane_id)
	{
		pthread_mutex_lock(&P_MAP_MTX);
		p_mp.at(vehicleSID).updateTime = DmUtil::getTimeMillisec();
		if (p_mp.at(vehicleSID).laneId != lane_id) {
			if (p_mp.at(vehicleSID).laneId == 0) logger->info("[updateLaneId] Vehicle(ID:" + to_string(vehicleSID) + ") has ENTERED the area of my Edge.");
			logger->info("[updateLaneId] update  VehicleID:" + to_string(vehicleSID) + " lane_id:" + to_string(p_mp.at(vehicleSID).laneId) + " -> " + to_string(lane_id) );
			p_mp.at(vehicleSID).laneId = lane_id;

			// 別スレッドに領域チェック/連携処理を移譲
			thread checkAreaThread(&LocationManager::checkAreaOfMonitoredVehicle, this, vehicleSID);
			checkAreaThread.detach();
		}
		pthread_mutex_unlock(&P_MAP_MTX);

	}

	/**
	* タスクを終了する
	*
	* @author	Nagoya University
	* @date	2019/12/24
	*
	* @param	mngKey	管理キー
	*/
	void LocationManager::deleteTask(const string &mngKey)
	{
		pthread_mutex_lock(&P_MAP_MTX);
		for (auto i = p_mp.begin(); i != p_mp.end();) {
			if (i->second.Info.count(mngKey) != 0) {
				i->second.Info.erase(mngKey);
				logger->info("[deleteTask] Deleted Task(mngKey). vehicleId:" + to_string(i->first) + " mngKey:" + mngKey);
				if (i->second.Info.size() == 0) {
					logger->info("[deleteTask] Deleted Task(vehicle). vehicleId:" + to_string(i->first));
					i = p_mp.erase(i);
				}
				break;
			}
			else {
				++i;
			}
		}
		pthread_mutex_unlock(&P_MAP_MTX);
	}

	/**
	* 現在のレーンIDを取得する
	*
	* @author	Nagoya University
	* @date	2018/08/24
	*
	* @return	レーンID
	*/
	//int LocationManager::getLaneId()
	unsigned long long LocationManager::getLaneId()
	{
		return this->laneId;
	}

	/**
	* 位置情報を扱うストリームかを判定する
	*
	* @author	Nagoya University
	* @date	2019/12/20
	*
	* @param	streamName	ストリーム名
	* @param	sidColName	SIDを取り扱うカラム名
	* @param	laneIdColName	レーンIDを取り扱うカラム名
	* @return	位置情報を扱うか否か
	*/
	bool LocationManager::isHandlingLocation(const string &streamName, string &sidColName, string &laneIdColName)
	{
		for (string stream : targetStreamList) {
			if (stream == streamName) {
				sidColName = sidSchemaMap[streamName];
				laneIdColName = laneIdSchemaMap[streamName];
				return true;
			}
		}
		return false;
	}

	/**
	* 既にタスクが存在するかを取得し、存在する場合は管理番号を返却する
	*
	* @author	Nagoya University
	* @date	2019/12/20
	*
	* @param	mngKey	管理キー
	* @param	mngId	クエリ管理番号
	* @return	成功可否
	*/
	bool LocationManager::isExistTask(const string &mngKey, unsigned int &mngId)
	{
		bool ret = false;
		pthread_mutex_lock(&P_MAP_MTX);
		for (auto i = p_mp.begin(); i != p_mp.end(); ++i) {
			if (i->second.Info.count(mngKey) != 0) {
				for (PosMng::TranInfo tranInfo : i->second.Info.at(mngKey).tranInfoList)
				{
					if (tranInfo.edgeSID == static_cast<unsigned int>(stoi(settings.getParameter("MY_STATION_ID")))) mngId = tranInfo.mngID;
				}
				ret = true;
			}
		}
		pthread_mutex_unlock(&P_MAP_MTX);
		return ret;
	}

	/**
	* 既にタスクが存在するかを取得し、存在する場合は管理番号を返却する
	*
	* @author	Nagoya University
	* @date	2019/12/20
	*
	* @param	vehicleSID	車両SID
	* @param	mngKey		管理キー
	* @param	mngId		クエリ管理番号
	* @return	成功可否
	*/
	//bool LocationManager::isExistTask(unsigned int vehicleSID, const string &mngKey, unsigned int &mngId)
	bool LocationManager::isExistTask(unsigned long long vehicleSID, const string &mngKey, unsigned int &mngId)
	{
		bool ret = false;
		pthread_mutex_lock(&P_MAP_MTX);
		if (p_mp.count(vehicleSID) != 0 && p_mp.at(vehicleSID).Info.count(mngKey) != 0) {
			for (PosMng::TranInfo tranInfo : p_mp.at(vehicleSID).Info.at(mngKey).tranInfoList)
			{
				//if (tranInfo.edgeSID == static_cast<unsigned int>(stoi(settings.getParameter("MY_STATION_ID")))) mngId = tranInfo.mngID;
				if (tranInfo.edgeSID == static_cast<unsigned long long>(stoull(settings.getParameter("MY_STATION_ID")))) mngId = tranInfo.mngID;
			}
			ret = true;
		}
		pthread_mutex_unlock(&P_MAP_MTX);
		return ret;
	}

	/**
	* 位置管理している車両SIDを返す
	*
	* @author	Nagoya University
	* @date	2019/12/20
	*
	* @return	位置管理中の車両SID
	*/
	//vector<unsigned int> LocationManager::getLocationMngVehicleID()
	vector<unsigned long long> LocationManager::getLocationMngVehicleID()
	{
		//vector<unsigned int> vehicleIDList;
		vector<unsigned long long> vehicleIDList;
		pthread_mutex_lock(&P_MAP_MTX);
		for (auto i = p_mp.begin(); i != p_mp.end(); ++i) {
			vehicleIDList.push_back(i->first);
		}
		pthread_mutex_unlock(&P_MAP_MTX);
		return vehicleIDList;
	}

	/**
	* 指定した車両と管理情報に従って、データ取得要求領域内に包含されるエッジSIDを返却する
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param	vehicleSID	車両SID
	* @param	mngKey	管理情報キー
	* @return	包含エッジSID
	*/

	//unordered_map<unsigned int, unsigned int> LocationManager::getNearEdgeSID(unsigned int vehicleSID, const string &mngKey)
	unordered_map<unsigned long long, unsigned long long> LocationManager::getNearEdgeSID(unsigned long long vehicleSID, const string &mngKey)
	{
		logger->trace("[getNearEdgeSID] IN");
		//unordered_map<unsigned int, unsigned int> edgeSIDmap;
		unordered_map<unsigned long long, unsigned long long> edgeSIDmap;

		if (targetStreamList.size() == 0) {
			logger->error("[getNearEdgeSID] LocationManager can't manage location information of each SID, because 'ADD_SID_SCHEMA' and 'ADD_LANE_ID_SCHEMA' couldn't be identified. Please check configuration.(is/conf/system.conf)");
			return edgeSIDmap;
		}

		// 走行しているレーンIDを抽出
		//int lane_id = 0;
		unsigned long long lane_id = 0;
		int range = 0;
		pthread_mutex_lock(&P_MAP_MTX);
		if (p_mp.count(vehicleSID) == 0) {
			logger->debug("[getNearEdgeSID] Vehicle(SID:" + to_string(vehicleSID) + " ) doesn't not exist in the managed list.");
			return edgeSIDmap;
		}
		else {
			lane_id = p_mp.at(vehicleSID).laneId;
			range = p_mp.at(vehicleSID).Info.at(mngKey).range;
			logger->debug("[getNearEdgeSID] Getting my lane_id from P_MAP. vehicleID:" + to_string(vehicleSID) + " lane_id:" + to_string(lane_id));
		}
		pthread_mutex_unlock(&P_MAP_MTX);
		
		if (lane_id == 0) {
			logger->debug("[getNearEdgeSID] Getting my lane_id from stream.");
			IS::QueueManager &QueuM = IS::QueueManager::get_instance();
			for (string targetStream : targetStreamList)
			{
				TupleSet ts;
				QueuM.getTupleSet(targetStream, 5000, 0, DmUtil::getTimeMillisec(), ts);
				int sidIndex = ts.getSchema().getAttributeIdx(sidSchemaMap[targetStream]);
				int laneIdIndex = ts.getSchema().getAttributeIdx(laneIdSchemaMap[targetStream]);
				any val;
				for (Tuple tuple : ts.getTuples()) {
					tuple.getValueByIdx(sidIndex, val);
					//unsigned int tupleSID = stoi(stringUtil.getAnyString(val));
					unsigned long long tupleSID = stoull(stringUtil.getAnyString(val));
					if (tupleSID == vehicleSID) {
						tuple.getValueByIdx(laneIdIndex, val);
						//lane_id = stoi(stringUtil.getAnyString(val));
						lane_id = stoull(stringUtil.getAnyString(val));
						logger->debug("[getNearEdgeSID] vehiclde_id:" + to_string(vehicleSID) + " lane_id:" + to_string(lane_id));
						break;
					}
				}
				if (lane_id != 0) break;
			}
			if (lane_id == 0) {
				string streams;
				for (string targetStream : targetStreamList) {
					if (streams.length() != 0) streams.append(",");
					streams.append(targetStream);
				}
				logger->error("[getNearEdgeSID] LocationManager can't manage location information of each SID. Can't extract location information from stream(" + streams + ")");
				return edgeSIDmap;
			}
		}

		// RDBへ問い合わせ
		// DB接続
		connection Conn(("dbname=" + settings.getParameter("DATABASE_NAME") + " user=" + settings.getParameter("USER_NAME") + " password=" + settings.getParameter("DB_PASS")  + " host=" + settings.getParameter("DATABASE_ADDR") + " port=" + settings.getParameter("DATABASE_PORT")));
		work T(Conn);

		string targetEdgeSID = "";
		string schemaName = settings.getParameter("QUERY_SCHEME_NAME");
		if (schemaName.length() > 0) schemaName.append(".");

		string query = "";
		query.append("SELECT face_id FROM ");
		query.append("  " + schemaName + "st_face ");
		query.append("WHERE ");
		query.append("  ST_CONTAINS( ");
		query.append("    (SELECT ST_BUFFER( (mbr), " + to_string(range) + ") ");
		query.append("     FROM " + schemaName + "st_face ");
		query.append("     WHERE face_id = " + to_string(lane_id) + " ");
		query.append("     ), ");
		query.append("     mbr ");
		query.append("  ); ");

		// SQLの実行
		result R1(T.exec(query));

		// データ部の取得
		for (result::const_iterator c = R1.begin(); c != R1.end(); ++c) {
			// CSVと照合
			if (lid_controll_edge_sid_map.count(c[0].as(int())) > 0 && edgeSIDmap.count(lid_controll_edge_sid_map[c[0].as(int())]) == 0) {
				//edgeSIDmap.insert(std::pair<unsigned int, unsigned int>(lid_controll_edge_sid_map[c[0].as(int())], lid_controll_edge_sid_map[c[0].as(int())]));
				// // ToDo: face_id <int> を取得しているため、as(int)で問題ないが、DB側がlong型のsidの場合、as(long)にする必要がある。
				edgeSIDmap.insert(std::pair<unsigned long long, unsigned long long>(lid_controll_edge_sid_map[c[0].as(int())], lid_controll_edge_sid_map[c[0].as(int())]));
				if (targetEdgeSID.length() != 0) targetEdgeSID.append(",");
				targetEdgeSID.append(to_string(lid_controll_edge_sid_map[c[0].as(int())]));
			}
		}
		if (edgeSIDmap.size() == 0) logger->debug("[getNearEdgeSID] Not found near lane_id. Search lane_id:" + to_string(lane_id));
		else logger->info("[getNearEdgeSID] vehicleID:" + to_string(vehicleSID) + " mngKey:" + mngKey + " laneId:" + to_string(lane_id) + " range:" + to_string(range) + " nearEdgeSID:[" + targetEdgeSID + "]");
		logger->trace("[getNearEdgeSID] OUT");
		return edgeSIDmap;

	}

	/**
	* エリア外へ移動したかを判定
	*
	* @author	Nagoya University
	* @date	2019/12/25
	*
	*/
	void LocationManager::checkOutOfAreaProc()
	{
		unsigned int myMngId = 0;
		//vector<unsigned int> removeEdgeSIDList;
		vector<unsigned long long> removeEdgeSIDList;
		pthread_mutex_lock(&P_MAP_MTX);
		for (auto i = p_mp.begin(); i != p_mp.end(); ++i) {
			if (i->second.laneId == 0) continue;
			if (vPosUpdateMap.count(i->first) == 0) {
				vPosUpdateMap[i->first] = i->second.updateTime;
				logger->info("[checkOutOfAreaProc] Add check target. VehicleID:" + to_string(i->first));
			}
			else {
				if (vPosUpdateMap[i->first] == i->second.updateTime) {
					logger->info("[checkOutOfAreaProc] Vehicle(ID:" + to_string(i->first) + ") is out of my area. So LaneId update to '0'");
					i->second.laneId = 0;
					
					for (auto itr = i->second.Info.begin(); itr != i->second.Info.end(); ++itr) {
						//logger->debug("[checkOutOfAreaProc] <before> tranInfoList.size:" + to_string(itr->second.tranInfoList.size()));
						for (auto tranItr = itr->second.tranInfoList.begin(); tranItr != itr->second.tranInfoList.end();) {
							PosMng::TranInfo tranInfo = *tranItr;
							//if (tranInfo.edgeSID != static_cast<unsigned int>(stoi(settings.getParameter("MY_STATION_ID")))) {
							if (tranInfo.edgeSID != static_cast<unsigned long long>(stoull(settings.getParameter("MY_STATION_ID")))) {
								tranItr = itr->second.tranInfoList.erase(tranItr);
								removeEdgeSIDList.push_back(tranInfo.edgeSID);
							}
							else {
								++tranItr;
								myMngId = tranInfo.mngID;
							}
						}
						//logger->debug("[checkOutOfAreaProc] <after > tranInfoList.size:" + to_string(itr->second.tranInfoList.size()));
					}
				}
				else {
					//logger->debug("[checkOutOfAreaProc] Vehicle(ID:" + to_string(i->first) + ") updateTime (" + to_string(vPosUpdateMap[i->first]) + " -> " + to_string(i->second.updateTime));
					vPosUpdateMap[i->first] = i->second.updateTime;
				}
			}
		}
		pthread_mutex_unlock(&P_MAP_MTX);

		// RequestOperatorも削除
		IS::QueryManager &QeryM = IS::QueryManager::get_instance();
		for (unsigned int edgeSID : removeEdgeSIDList) {
			QeryM.removeRequestOp(myMngId, edgeSID);
		}
		
	}
}
