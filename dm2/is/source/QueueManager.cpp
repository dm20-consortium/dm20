#include "QueueManager.h"

#include <thread>
#include <pqxx/pqxx>
using namespace std;
using namespace pqxx;

namespace IS {

	/**
	 * 初期化処理
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */
	void QueueManager::init()
	{
		prioritySensor = settings.getParameter("ADD_LANE_SEARCH_PRIORITY_SENSOR");
		mp.clear();
		tableSidNameMap.clear();
		tableSidIdxMap.clear();
		tableLaneIdNameMap.clear();
		tableLaneIdIdxMap.clear();
		pathPlanVidNameMap.clear();
		pathPlanVidIdxMap.clear();
		pathPlanLaneNameMap.clear();
		pathPlanLaneIdxMap.clear();
		vIdPathMap.clear();
		vIdCuurentLaneMap.clear();
		addPosSetMap.clear();
		addVIdSetMap.clear();
		// SIDマップの作成
		string addSidListStr = settings.getParameter("ADD_SID_SCHEMA");
		if (addSidListStr != "") {
			vector<string> addSidList = stringUtil.split(addSidListStr, ",");
			for (string s : addSidList) {
				vector<string> sList = stringUtil.split(s, ".");
				if (sList.size() == 2) {
					tableSidNameMap[stringUtil.split(s, ".")[0]] = stringUtil.split(s, ".")[1];
				}
				else {
					logger->warn("[init] 'ADD_SID_SCHEMA' parameter is invalied. Expect:'schemaName.columnName' Please confirm system.conf.");
				}
			}
		}
		// レーンIDマップの作成
		string addLaneListStr = settings.getParameter("ADD_LANE_ID_SCHEMA");
		if (addLaneListStr != "") {
			vector<string> addLaneList = stringUtil.split(addLaneListStr, ",");
			for (string s : addLaneList) {
				vector<string> sList = stringUtil.split(s, ".");
				if (sList.size() == 2) {
					tableLaneIdNameMap[stringUtil.split(s, ".")[0]] = stringUtil.split(s, ".")[1];
				}
				else {
					logger->warn("[init] 'ADD_LANE_ID_SCHEMA' parameter is invalied. Expect:'schemaName.columnName' Please confirm system.conf.");
				}
			}
		}
		string addPlanVidListStr = settings.getParameter("ADD_LANE_SEARCH_PRIORITY_PLAN_VID");
		if (addPlanVidListStr != "") {
			vector<string> addPlanVidList = stringUtil.split(addPlanVidListStr, ",");
			for (string s : addPlanVidList) {
				vector<string> sList = stringUtil.split(s, ".");
				if (sList.size() == 2) {
					pathPlanVidNameMap[stringUtil.split(s, ".")[0]] = stringUtil.split(s, ".")[1];
				}
				else {
					logger->warn("[init] 'ADD_LANE_SEARCH_PRIORITY_PLAN_VID' parameter is invalied. Expect:'schemaName.columnName' Please confirm system.conf.");
				}
			}
		}
		string addPlanLaneListStr = settings.getParameter("ADD_LANE_SEARCH_PRIORITY_PLAN_LANE");
		if (addPlanLaneListStr != "") {
			vector<string> addPlanLaneList = stringUtil.split(addPlanLaneListStr, ",");
			for (string s : addPlanLaneList) {
				vector<string> sList = stringUtil.split(s, ".");
				if (sList.size() == 2) {
					pathPlanLaneNameMap[stringUtil.split(s, ".")[0]] = stringUtil.split(s, ".")[1];
				}
				else {
					logger->warn("[init] 'ADD_LANE_SEARCH_PRIORITY_PLAN_LANE' parameter is invalied. Expect:'schemaName.columnName' Please confirm system.conf.");
				}
			}
		}
		if (tableLaneIdNameMap.size() > 0) {
			// DB接続
			connection Conn(("dbname=" + settings.getParameter("DATABASE_NAME") + " user=" + settings.getParameter("USER_NAME") + " password=" + settings.getParameter("DB_PASS")  + " host=" + settings.getParameter("DATABASE_ADDR") + " port=" + settings.getParameter("DATABASE_PORT")));
			work T(Conn);
			string schemeName = settings.getParameter("QUERY_SCHEME_NAME");
			string srid = settings.getParameter("PLANE_NUMBER_SRID");
			string query = "";
			query.append("SELECT tablename FROM pg_tables ");
			query.append("WHERE schemaname = '" + schemeName + "' AND tablename IN ('st_edge','linestring') ORDER BY tablename ");
			// SQLの実行
			result R1(T.exec(query));
			if (R1.begin() != R1.end()) {
				if (schemeName.length() > 0) schemeName.append(".");
				result::const_iterator c = R1.begin();
				string lineType = c[0].as(string());
				query = "";
				logger->debug("[init]lineType:" + lineType);
				if (lineType == "linestring") {
					query.append("SELECT lanelet_id, ST_AsEwkt(geometry)");
					query.append("FROM " + schemeName + "Lanelet ");
				} else {
					query.append("SELECT pcfp.comp_feature_id, ST_AsEwkt(ST_Transform(sf.mbr, " + srid + ")) FROM ");
					query.append("  " + schemeName + "pf_comp_feat_part pcfp, ");
					query.append("  " + schemeName + "pf_area_feature paf, ");
					query.append("  " + schemeName + "pf_area_topo_prim patp, ");
					query.append("  " + schemeName + "st_face sf ");
					query.append("WHERE ");
					query.append("  sf.face_id = patp.face_id ");
					query.append("  AND patp.feature_id = paf.feature_id ");
					query.append("  AND paf.feature_class_code in('8120', '8170')");	// Lane Area(8120) or Intersection Area Shape(8170)
					query.append("  AND paf.feature_id = pcfp.feature_id ");
					query.append("  AND pcfp.feature_category_num = 3 ");				// Area(3)
				}
				logger->debug("[init]query:" + query);
				// SQLの実行
				result R2(T.exec(query));
				// データ部の取得
				for (result::const_iterator c = R2.begin(); c != R2.end(); ++c) {
					unsigned long long laneId = c[0].as(long());
					string a1 = c[1].as(string());
					// WKTからSRIDを分離する
					int srId1 = stringUtil.getSRID(a1);
					if (srId1 != 0) a1 = stringUtil.split(a1,";").at(1);
					// Geometry文字列を大文字に変換
					string geomStUpp1 = a1;
					transform(geomStUpp1.begin(),geomStUpp1.end(),geomStUpp1.begin(),::toupper);
					// Geometryの型チェック
					string geomName = geomStUpp1;
					geomName = stringUtil.split(geomName,"(").at(0);
					string::size_type left = geomName.find_first_not_of(" ");
					geomName = stringUtil.split(geomName.substr(left)," ").at(0);
					if (geomName != "POLYGON") {
						continue;
					}
					// cout << laneId << "," << geomStUpp1 << endl;
					// WKT -> Geometry
					laneIdGeometryMap[laneId] = GEOSGeomFromWKT(a1.c_str());
				}
			}
			T.commit();
			//Conn.disconnect();
		}
	}

	/**
	* Queueの初期化処理
	*
	* @author	Nagoya University
	* @date	2019/12/24
	*/

	void QueueManager::initQueue()
	{
		// 定義されたStreamのキューを作成、保持
		vector<string> tableNameList = ism.getStreamSchemaNames();
		for (string tableName : tableNameList)
		{
			Schema schema;
			ism.getStreamSchema(tableName, schema);
			createQueue(tableName, schema);
			
			// SIDのカラム名からインデックスを導出し、マップ作成
			if (tableSidNameMap.find(tableName) == tableSidNameMap.end()) {
				tableSidIdxMap[tableName] = -1;
			} else {
				tableSidIdxMap[tableName] = schema.getAttributeIdx(tableSidNameMap[tableName]);
			}
			// レーンIDのカラム名からインデックスを導出し、マップ作成
			if (tableLaneIdNameMap.find(tableName) == tableLaneIdNameMap.end()) {
				tableLaneIdIdxMap[tableName] = -1;
			} else {
				tableLaneIdIdxMap[tableName] = schema.getAttributeIdx(tableLaneIdNameMap[tableName]);
			}
			// レーンIDのカラム名からインデックスを導出し、マップ作成
			if (pathPlanVidNameMap.find(tableName) == pathPlanVidNameMap.end()) {
				pathPlanVidIdxMap[tableName] = -1;
			} else {
				pathPlanVidIdxMap[tableName] = schema.getAttributeIdx(pathPlanVidNameMap[tableName]);
			}
			// レーンIDのカラム名からインデックスを導出し、マップ作成
			if (pathPlanLaneNameMap.find(tableName) == pathPlanLaneNameMap.end()) {
				pathPlanLaneIdxMap[tableName] = -1;
			} else {
				pathPlanLaneIdxMap[tableName] = schema.getAttributeIdx(pathPlanLaneNameMap[tableName]);
			}
		}
	}

	/**
	 * 実行処理
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void QueueManager::run()
	{
	}

	/**
	 * 実行処理の開始
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void QueueManager::start()
	{
		thread queuingThread(&QueueManager::run, this);
		queuingThread.detach();
	}

	/**
	 * 処理の停止
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void QueueManager::stop()
	{
		for (auto itr = mp.begin(); itr != mp.end();) {
			itr->second->finalize();
			delete itr->second;
			itr = mp.erase(itr);
		}
		for (auto itr = laneIdGeometryMap.begin(); itr != laneIdGeometryMap.end(); ++itr) {
			GEOSGeom_destroy(itr->second);
		}
		laneIdGeometryMap.clear();
	}

	/**
	* ストリームキューの作成
	*
	* @author	Nagoya University
	* @date	2018/08/23
	* @param[in]	tableName	テーブル名
	* @param[in]	schema		スキーマ設定名
	*/

	Queue* QueueManager::createQueue(const string &tableName, const Schema &schema)
	{
		if (mp.find(tableName) != mp.end()) {
			logger->error("[createQueue] target stream already exists. tableName:" + tableName);
			return NULL;
		}
		Queue *queue = new Queue(tableName, schema);
		mp.insert(std::pair<string, IS::Queue*>(tableName, queue));
		logger->debug("[createQueue] Name:" + tableName);
		return queue;
	}
	void QueueManager::printQueue() 
	{
		for(auto itr = mp.begin(); itr != mp.end(); ++itr) {
			cout << "key = " << itr->first << endl;
		}
	}
	/**
	* 一時ストリームキューの作成
	*
	* @author	Nagoya University
	* @date	2018/08/23
	* @param[in]	tableName	テーブル名
	* @param[in]	schemaName	スキーマ設定名
	*/

	void QueueManager::createTempQueue(const string &tableName, const string &schemaName)
	{
		Schema schema;

		// TODO : 自DBに指定のスキーマ設定が存在しない場合は別DBからスキーマ設定を取得する必要あり
		ism.getStreamSchema(schemaName, schema);
		createQueue(tableName, schema);

	}

	/**
	* ストリームキューの削除
	*
	* @author	Nagoya University
	* @date	2019/04/17
	* @param[in]	tableName	テーブル名
	* @return	成功可否
	*/
	bool QueueManager::removeQueue(const string &tableName)
	{
		// Queueオブジェクトの取得
		try {
			Queue* queue = mp.at(tableName);

			// Listenerの追加
			if (queue->getListenerSize() == 0) {
				queue->finalize();
				mp.erase(tableName);
				
				logger->debug("[remove queue] Name:" + tableName);
				return true;
			}
		}
		catch (out_of_range const &) {
			logger->error("[removeQueueListener] Failed push queue because Not defined table. Name:" + tableName);
		}
		return false;
	}

	/**
	* ハッシュ値取得処理 (タプルから生成)
	*
	* @author	Shinichi Kusayama
	* @date		2024/05/02
	*
	* @param	tuples	受信データ
	*/
	vector<string> QueueManager::getTupleHash(const vector<Tuple> &tuples)
	{
		vector<string> rtnStrList;
		for (unsigned int i = 0; i < tuples.size(); i++) {
			string tuplesStr = "";
			for (int j = 0; j < tuples.at(i).size(); j++) {
				any val;
				tuples.at(i).getValueByIdx(j, val);
				tuplesStr += stringUtil.getAnyString(val);
				if (j == 0) {
					long ts;
					tuples.at(i).getTimestampByIdx(j, ts);
					tuplesStr += to_string(ts);
				}
			}
			std::hash<std::string> hasher;
			size_t hash1 = hasher(tuplesStr);
			rtnStrList.push_back(to_string(hash1));
		}
		return rtnStrList;
	}
	/**
	* ハッシュ値取得処理 (電文から生成)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/06
	*
	* @param	xmlTuples	受信データ
	*/
	vector<string> QueueManager::getTupleHash(const string &xmlTuples)
	{
		vector<string> rtnStrList;
		string tuplesStr;
		try {
			string::size_type begin = xmlTuples.find("  <tuple>");
			string::size_type size = xmlTuples.rfind("</tuple>") - begin;
			tuplesStr = xmlTuples.substr(begin, size);
		} catch (...) {
			try {
				string::size_type begin = xmlTuples.find("<tuple>");
				string::size_type size = xmlTuples.rfind("</tuple>") - begin;
				tuplesStr = xmlTuples.substr(begin, size);
			} catch (...) {
				logger->error("<tuple></tuple> not found");
				return rtnStrList;
			}
		}
		vector<string> tupleStrList = stringUtil.split(tuplesStr, "</tuple>");
		for (int i = 0; i < (int)tupleStrList.size(); i++) {
			string s = tupleStrList[i];
			stringUtil.deleteNl(s);
			std::hash<std::string> hasher;
			size_t hash1 = hasher(s);
			rtnStrList.push_back(to_string(hash1));
		}
		return rtnStrList;
	}
	/**
	* CSからの電文をキューに追加する
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param	data	受信データ
	*/

	void QueueManager::addQueueProc(const RecvData &data)
	{
		//REL_COMMENT logger->trace("[addQueueProc] IN");

#if MEASURE_MODE == 1
		long startTime = DmUtil::getTimeMicrosec();
		long procTime = startTime;
		int step = 1;
#endif
		string tableName = "";
		vector<Tuple> tuples;
		Schema schema;
		// 電文からタプルへ変換
		recvDataToTuples(data, tuples, schema, tableName);
		// データが存在しない場合は処理終了
		if (tuples.size() == 0) return;

		int userIdx = schema.getAttributeIdx(Schema::COL_USERID);
		int ipIdx   = schema.getAttributeIdx(Schema::COL_IPADDR);
		int hashIdx = schema.getAttributeIdx(Schema::COL_HASH);
		bool setHash = false;
		vector<string> tupleHashes;

		for (unsigned int i = 0; i < tuples.size(); i++) {
			// 情報の付与（IPアドレス）
			tuples.at(i).setValue(ipIdx, data.cs_ip_address, 0, false);
			// 情報の付与（ハッチュ値）※(null)の場合に、付与する。
			// 　・予測・フュージョン関数によって生成され、CS連携されたタプルは、ハッシュ値(null)となる。
			// 　・ToDo: EvalOperator側で付与できれば、ここでの処理は不要になる。
			if (i == 0) {
				any hashval;
				tuples.at(i).getValueByIdx(schema.getAttributeIdx(Schema::COL_HASH), hashval);
				string hashStr = stringUtil.getAnyString(hashval);
				if (hashStr == "(null)") {
					tupleHashes = getTupleHash(tuples);
					//tupleHashes = getTupleHash(data.payload);
					if (tuples.size() == tupleHashes.size()) setHash = true;
				}
			}
			if (setHash) tuples.at(i).setValue(hashIdx, tupleHashes[i], 0, false);

			// 書き込み権限のチェック
			//　・ユーザIDが未付与、あるいは書き込み権限がない場合はpublicユーザとして登録する
			any val;
			if (tuples.at(i).getValueByIdx(userIdx, val)) {
				string user = stringUtil.getAnyString(val);
				if (!schema.isAppend(user)) tuples.at(i).updateValueByIdx(userIdx, "public");
			} else {
				tuples.at(i).setValue(userIdx, "public", 0, false);
			}
		}
		//REL_COMMENT logger->trace("[addQueueProc] lane proc after");
		// Queueオブジェクトの取得
		try {
			Queue* queue = mp.at(tableName);
			queue->pushTuple(tuples);
		}
		catch (out_of_range const &) {
			logger->error("[addQueueProc] Failed push queue because Not defined table. Name:" + tableName);
		}
		//REL_COMMENT logger->trace("[addQueueProc] OUT");

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - startTime) / 1000.0;
		logger->info("[addQueueProc] STAT_STEP" + to_string(step++) + " total processing time: " + to_string(msec) + "[ms]");
#endif

		tuples.clear();
	}

	/**
	* アプリからの電文をキューに追加する
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param	data	受信データ
	* @param	user	ユーザ名
	*/
	void QueueManager::addQueueProc(const RecvData &data, const string &user)
	{
		//REL_COMMENT logger->trace("[addQueueProc] IN");

#if MEASURE_MODE == 1
		long startTime = DmUtil::getTimeMicrosec();
		long procTime = startTime;
		int step = 1;
#endif
		// Tupleとtablenameの取得
		string tableName = "";
		vector<Tuple> tuples;
		Schema schema;
		// 電文からタプルへ変換
		recvDataToTuples(data, tuples, schema, tableName);
		// データが存在しない場合は処理終了
		if (tuples.size() == 0) return;

		vector<string> tupleHashes = getTupleHash(tuples);
		string ip_addr = inet_ntoa(data.client.sin_addr);
		// タプルに管理者属性を追加、必要ならSID等の補完を行う。
		complementTuples(tableName, schema, tuples, user, ip_addr, tupleHashes, false);
		
		// Queueオブジェクトの取得
		try {
			Queue* queue = mp.at(tableName);
			queue->pushTuple(tuples);
		}
		catch (out_of_range const &) {
			logger->error("[addQueueProc] Failed push queue because Not defined table. Name:" + tableName);
		}

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - startTime) / 1000.0;
		logger->info("[addQueueProc] STAT_STEP" + to_string(step++) + " total processing time: " + to_string(msec) + "[ms]");
#endif

		tuples.clear();
	}
	/**
	* DMIから受け取ったタプルをキューに追加する
	*
	* @author	Shinichi Kusayama
	* @date		2024/05/02
	*
	* @param	data	受信データ
	* @param	user	ユーザ名
	*/
	void QueueManager::addQueueProc(vector<Tuple> tuples, string &ip_addr, const string &user)
	{
		if (tuples.size() == 0) return;
		string tableName = tuples.at(0).getTableName();
		Schema schema;
		if (tableName == "") {
			logger->warn(
			"[addQueueProc] The table name in the head of tuples is empry. "
			"We ignore this tuples");
			return;
		}
		getSchema(tableName, schema);
		vector<string> tupleHashes = getTupleHash(tuples);
		// タプルに管理者属性を追加、必要ならSID等の補完を行う。
		complementTuples(tableName, schema, tuples, user, ip_addr, tupleHashes, true);
		
		// Queueオブジェクトの取得
		try {
			Queue* queue = mp.at(tableName);
			queue->pushTuple(tuples);
		}
		catch (out_of_range const &) {
			logger->error("[addQueueProc] Failed push queue because Not defined table. Name:" + tableName);
		}

	}
	/**
	* タプルに管理者属性を追加、必要ならSID等の補完を行う。
	*
	* @author	Shinichi Kusayama
	* @date		2024/05/02
	*
	* @param	schema		スキーマ
	* @param	tuples		タプル（in, out）
	* @param	user		ユーザ名
	* @param	ip_addr		IPアドレス
	* @param	tupleHashes	タプルのハッシュ
	* @param	callDmi		DMIからのコール
	*/
	void QueueManager::complementTuples(const string &tableName, const Schema &schema, vector<Tuple> &tuples, const string &user, const string &ip_addr, const vector<string> &tupleHashes, const bool callDmi)
	{
		int userIdx = schema.getAttributeIdx(Schema::COL_USERID);
		int ipIdx   = schema.getAttributeIdx(Schema::COL_IPADDR);
		int hashIdx = schema.getAttributeIdx(Schema::COL_HASH);
		int sidIdx = tableSidIdxMap[tableName];
		int laneIdIdx = tableLaneIdIdxMap[tableName];
		int pathVidIdx = pathPlanVidIdxMap[tableName];
		bool setHash = false;
		if (tuples.size() == tupleHashes.size()) setHash = true;
		for (unsigned int i = 0; i < tuples.size(); i++) {
			if (callDmi) tuples.at(i).resize(schema);
			// 情報の付与（ユーザID、IPアドレス、ハッシュ）
			tuples.at(i).setValue(userIdx, user, 0, false);
			tuples.at(i).setValue(ipIdx, ip_addr, 0, false);
			if (setHash) tuples.at(i).setValue(hashIdx, tupleHashes[i], 0, false);

			// SID補完
			if (sidIdx != -1) tuples.at(i).updateValueIf(sidIdx, 0, stoull(settings.getParameter("MY_STATION_ID")));
			// レーンID補完のための経路作成
			if (pathVidIdx != -1) makeVidPath(tuples.at(i), i, pathVidIdx, pathPlanLaneIdxMap[tableName]);
			// レーンID補完
			else if (laneIdIdx != -1) complementLaneId(schema, tuples.at(i), laneIdIdx);
		}
	}
	/**
	* 電文をタプルに変換する
	*
	* @author	Shinichi Kusayama
	* @date	2024/05/02
	*
	* @param	data		電文
	* @param	tuples		タプル
	* @param	schema		スキーマ
	* @param	tableName	テーブル名
	*/

	void QueueManager::recvDataToTuples(const RecvData &data, vector<Tuple> &tuples, Schema &schema, string &tableName)
	{
		string schema_name, payload;
		IS::ProtobufParser &pp = IS::ProtobufParser::get_instance();
		struct ProtobufHeaderInfo headerInfo;
		pp.getProtobufHeaderInfo(data.payload, headerInfo);
		// Protobufヘッダあり
		if (headerInfo.headerSize != 0)
		{
			schema_name = headerInfo.header.table_name;
			payload = string(headerInfo.payload_p, headerInfo.header.payload_size);
		}
		else {
			schema_name = data.schema_name;
			payload = data.payload;
		}
		recvDataToTuples(schema_name, payload, tuples, schema, tableName, true);
		// for (int i = 0; i < tuples.size(); i++) tuples[i].dump();
	}
	/**
	* 電文をタプルに変換する
	*
	* @author	Shinichi Kusayama
	* @date	2024/05/02
	*
	* @param	schema_name		スキーマ名
	* @param	payload		電文（本体）
	* @param	tuples		タプル
	* @param	schema		スキーマ
	* @param	tableName	テーブル名
	* @param	payloadCheck	電文チェックフラグ
	*/

	void QueueManager::recvDataToTuples(const string &schema_name, const string &payload, vector<Tuple> &tuples, Schema &schema, string &tableName, const bool &payloadCheck)
	{
		if (schema_name == "") {
			// XMLパーサへ電文解析を依頼し、タプルへ変換
			IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
			isp.init();
			isp.getTuplesBySAX(payload, tableName, tuples);
			isp.finalize();
			getSchema(tableName, schema);
			//REL_COMMENT logger->trace("[addQueueProc] getTuplesBySAX after");
			// end
		} else {
			// ProtoBufパーサに電文解析を依頼し、タプルへ変換
			tableName = schema_name;
			getSchema(tableName, schema);
			// protobufを使ったメッセージ作成
			IS::ProtobufParser &pp = IS::ProtobufParser::get_instance();
			try {
				if (tableName == "object_info" || tableName == "object_info_processed") {
					pp.objectInfoDeserializeToTuple_0_6_0(payload, tuples, schema);
				} else if (tableName == "object_info_0_8_0" || tableName == "object_info_0_8_0_processed") {
					pp.objectInfoDeserializeToTuple_0_8_0(payload, tuples, schema);
				} else if (tableName == "object_info_0_8_1" || tableName == "object_info_0_8_1_processed") {
					pp.objectInfoDeserializeToTuple_0_8_1(payload, tuples, schema);
				} else if (tableName == "freespace_info") {
					pp.freespaceInfoDeserializeToTuple_0_6_0(payload, tuples, schema);
				} else if (tableName == "freespace_info_0_8_0") {
					pp.freespaceInfoDeserializeToTuple_0_8_0(payload, tuples, schema);
				} else if (tableName == "freespace_info_0_8_1") {
					pp.freespaceInfoDeserializeToTuple_0_8_1(payload, tuples, schema);
				} else if (tableName == "signal_info") {
					pp.signalInfoDeserializeToTuple_0_6_0(payload, tuples, schema);
				} else if (tableName == "sensor_info") {
					pp.sensorInfoDeserializeToTuple_0_8_0(payload, tuples, schema);
				} else if (tableName == "sensor_info_0_8_1") {
					pp.sensorInfoDeserializeToTuple_0_8_1(payload, tuples, schema);
				} else {
					pp.DeserializeToTupleDynamically(payload, tuples, schema, payloadCheck);
				}
			} catch (const std::runtime_error& e) {
				string errMsg = e.what();
				logger->warn(errMsg);
			}
		}
	}
	/**
	 * タプルの追加処理
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	schema	スキーマ情報
	 * @param	tuples 	タプルリスト
	 */

	void QueueManager::addTuple(const Schema &schema, const vector<Tuple> &tuples)
	{
		// Queueオブジェクトの取得
		try {
			Queue* queue = mp.at(schema.getTableName());

			// Queueをプッシュ (呼び出し先にて別スレッド実行)
			queue->pushTuple("unknown", tuples);
		}
		catch (out_of_range const &) {
			logger->error("[addTuple] Failed push queue because Not defined table. Name:" + schema.getTableName());
		}
	}

	/**
	 * キューに対してlistenerを設定する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param 		  	streamName	ストリーム名
	 * @param [in,out]	ope		  	オペレータオブジェクト
	 */

	void QueueManager::addQueueListener(const string &streamName, Operator* ope)
	{
		// Queueオブジェクトの取得
		try {
			Queue* queue = mp.at(streamName);

			// Listenerの追加
			queue->addListener(ope);
		}
		catch (out_of_range const &) {
			logger->error("[addQueueListener] Failed addListener because Not defined table. Name:" + streamName);
		}
	}

	/**
	 * キューに対してlistenerの削除を行う
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param 		  	streamName	ストリーム名
	 * @param [in,out]	ope		  	オペレータオブジェクト
	 */

	void QueueManager::removeQueueListener(const string &streamName, Operator* ope)
	{
		// Queueオブジェクトの取得
		try {
			Queue* queue = mp.at(streamName);

			// Listenerの追加
			queue->removeListener(ope);
		}
		catch (out_of_range const &) {
			logger->error("[removeQueueListener] Failed push queue because Not defined table. Name:" + streamName);
		}
	}


	/**
	* キューに対してreferrerを設定する
	*
	* @author	Nagoya University
	* @date	2019/08/05
	*
	* @param 		  	streamName	ストリーム名
	* @param [in,out]	ope		  	オペレータオブジェクト
	*/

	void QueueManager::addQueueReferrer(const string &streamName, Operator* ope)
	{
		// Queueオブジェクトの取得
		try {
			Queue* queue = mp.at(streamName);

			// Listenerの追加
			queue->addReferrer(ope);
		}
		catch (out_of_range const &) {
			logger->error("[addQueueReferrer] Failed addReferrer because Not defined table. Name:" + streamName);
		}
	}

	/**
	* キューに対してreferrerの削除を行う
	*
	* @author	Nagoya University
	* @date	2019/08/05
	*
	* @param 		  	streamName	ストリーム名
	* @param [in,out]	ope		  	オペレータオブジェクト
	*/

	void QueueManager::removeQueueReferrer(const string &streamName, Operator* ope)
	{
		// Queueオブジェクトの取得
		try {
			Queue* queue = mp.at(streamName);

			// Listenerの追加
			queue->removeReferrer(ope);
		}
		catch (out_of_range const &) {
			logger->error("[removeQueueReferrer] Failed push queue because Not defined table. Name:" + streamName);
		}
	}

	/**
	 * 全件タプルセットへと格納する
	 *
	 * @author	Shinichi Kusayama
	 * @date	2023/3/29
	 *
	 * @param	streamName	ストリーム名
	 * @param [in,out]	tupleset	The tuple set.
	 */

	void QueueManager::getTupleSet(const string &streamName, const long lastRecvTime, TupleSet &tupleset)
	{
		//REL_COMMENT logger->trace("[getTupleSet num] IN");
		// Queueオブジェクトの取得
		try {
			Queue* queue = mp.at(streamName);

			// TupleSetの取得
			queue->getTupleSetMtxLock(lastRecvTime, tupleset);
		}
		catch (out_of_range const &) {
			logger->error("[getTupleSet(num)] Not found Queue. StreamName:" + streamName);
		}
		//REL_COMMENT logger->trace("[getTupleSet num] OUT");
	}

	/**
	 * 全件タプルセットへと格納する
	 *
	 * @author	Shinichi Kusayama
	 * @date	2025/08/12
	 *
	 * @param	streamName	ストリーム名
	 * @param [in,out]	tupleset	The tuple set.
	 * @param	partitionList		パーティションリスト
	 */

	void QueueManager::getTupleSetPartition(const string &streamName, TupleSet &tupleset, const int num, vector<int> &partitionList)
	{
		//REL_COMMENT logger->trace("[getTupleSet num] IN");
		// Queueオブジェクトの取得
		try {
			Queue* queue = mp.at(streamName);

			// TupleSetの取得
			queue->getTupleSetPartitionMtxLock(tupleset, num, partitionList);
		}
		catch (out_of_range const &) {
			logger->error("[getTupleSet(num)] Not found Queue. StreamName:" + streamName);
		}
		//REL_COMMENT logger->trace("[getTupleSet num] OUT");
	}

	/**
	 * 個数指定でタプルを取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	streamName	ストリーム名
	 * @param	num		  	取得個数
	 * @param	referenceTime		  	取得するための基準時間
	 * @param [in,out]	tupleset	The tuple set.
	 */

	void QueueManager::getTupleSet(const string &streamName, const int num, const long referenceTime, TupleSet &tupleset)
	{
		//REL_COMMENT logger->trace("[getTupleSet num] IN");
		// Queueオブジェクトの取得
		try {
			Queue* queue = mp.at(streamName);

			// TupleSetの取得
			queue->getTupleSetMtxLock(num, referenceTime, tupleset);
		}
		catch (out_of_range const &) {
			logger->error("[getTupleSet(num)] Not found Queue. StreamName:" + streamName);
		}
		//REL_COMMENT logger->trace("[getTupleSet num] OUT");
	}

	/**
	 * 時間指定でタプルを取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	streamName	ストリーム名
	 * @param	rangeMsec	  	取得時間
	 * @param	notifiedTime	  	通知済み時間
	 * @param	referenceTime		  	取得するための基準時間
	 * @param [in,out]	tupleset	The tuple set.
	 */

	void QueueManager::getTupleSet(const string &streamName, const long rangeMsec, const long notifiedTime, const long referenceTime, TupleSet &tupleset)
	{
		//REL_COMMENT logger->trace("[getTupleSet msec] IN");
		// Queueオブジェクトの取得
		try {
			Queue* queue = mp.at(streamName);

			// TupleSetの取得
			queue->getTupleSetMtxLock(rangeMsec, notifiedTime, referenceTime, tupleset);
		}
		catch (out_of_range const &) {
			logger->error("[getTupleSet(msec)] Not found Queue. StreamName:" + streamName);
		}
		//REL_COMMENT logger->trace("[getTupleSet msec] OUT");
	}

	/**
	* ストリーム名を指定してスキーマ情報を取得する
	*
	* @author	Nagoya University
	* @date	2018/04/25
	*
	* @param	streamName	ストリーム名
	* @param [in,out]	schemaInfo	スキーマ情報
	*/
	void QueueManager::getSchema(const string &streamName, Schema &schemaInfo)
	{
		// Queueオブジェクトの取得
		try {
			Queue* queue = mp.at(streamName);

			// TupleSetの取得
			queue->setSchemaInfo(schemaInfo);
		}
		catch (out_of_range const &) {
			logger->error("[getSchema] Not found Queue. StreamName:" + streamName);
		}
	}

	/**
	* ストリーム名を指定してレーンIDを扱うカラム名を取得する
	*
	* @author	Nagoya University
	* @date	2019/12/05
	*
	* @param	streamName	ストリーム名
	* @return	ret		レーンIDを扱うカラム名
	*/
	string QueueManager::getLaneIdSchema(const string &streamName)
	{
		string ret = "";
		if (tableLaneIdNameMap.count(streamName) > 0) {
			ret = tableLaneIdNameMap[streamName];
		}
		return ret;
	}

	/**
	* ストリーム名の存在可否を取得する
	*
	* @author	Shinichi Kusayama
	* @date	2024/10/24
	*
	* @param [in]	streamName	ストリーム名
	* @return	存在可否
	*/
	bool QueueManager::isFound(const string &streamName)
	{
		if (mp.find(streamName) == mp.end()) return false;
		return true;
	}

	/**
	* ストリーム名とユーザ名を指定して読出し許可可否を取得する
	*
	* @author	Nagoya University
	* @date	2019/09/04
	*
	* @param [in]	streamName	ストリーム名
	* @param [in]	user	ユーザ名
	* @return	読出し許可可否
	*/
	bool QueueManager::isSelect(const string &streamName, const string &user)
	{
		// Queueオブジェクトの取得
		try {
			Queue* queue = mp.at(streamName);
			// 読出し許可可否の取得
			return queue->isSelect(user);
		}
		catch (out_of_range const &) {
			logger->error("[isSelect] Not found Queue. StreamName:" + streamName);
			return false;
		}
	}

	/**
	* ストリーム名とユーザ名を指定して管理者情報参照可否を取得する
	*
	* @author	Nagoya University
	* @date	2019/09/04
	*
	* @param [in]	streamName	ストリーム名
	* @param [in]	user	ユーザ名
	* @return	読出し許可可否
	*/
	bool QueueManager::isRefAdminCol(const string &streamName, const string &user)
	{
		// Queueオブジェクトの取得
		try {
			Queue* queue = mp.at(streamName);

			// 読出し許可可否の取得
			return queue->isRefAdminCol(user);
		}
		catch (out_of_range const &) {
			logger->error("[isRefAdminCol] Not found Queue. StreamName:" + streamName);
			return false;
		}
	}

	/**
	* 経路情報を作成
	*
	* @author	Shinichi Kusayama
	* @date		2024/05/02
	*
	* @param	tuple		タプル
	* @param	vidIdx		VIDのインデックス値
	* @param	laneIdx		レーンIDのインデックス値
	*
	*/
	void QueueManager::makeVidPath(const Tuple &tuple, const int tupleIdx, const int vidIdx, const int pathListIdx) {
		if (tableLaneIdNameMap.size() <= 0 || pathListIdx == -1) return;
		any vidVal, pathListVal;
		tuple.getValueByIdx(vidIdx, vidVal);
		tuple.getValueByIdx(pathListIdx, pathListVal);
		string vid = stringUtil.getAnyString(vidVal);;
		string pathList = stringUtil.getAnyString(pathListVal);
		pathList = pathList.substr(1, pathList.length() - 2);	// []を除去

		logger->debug("makeVidPath vid:" + vid + ",pathList:" + pathList);
		
		// VID-PATHマップをクリアする
		if (tupleIdx == 0)	vIdPathMap.clear();
		
		int pathIdx = 0;
		for (string path : stringUtil.split(pathList, ",")) {
			pathIdx++;
			string key = vid + "," + path;
			// 暫定対応：既に存在するレーン（周回）はスキップ。通過後に更新される想定
			if (vIdPathMap.find(key) == vIdPathMap.end()) {
				vIdPathMap[vid + "," + path] = pathIdx;
			}
			vIdCuurentLaneMap[vid] = 0;
		}
	}
	/**
	* 対象スキーマの不足している情報を補完する
	*
	* @author	Nagoya University
	* @date	2018/07/09
	*
	* @param	schema		スキーマ
	* @param	tuple		タプル [in, out]
	* @param	laneIdx		レーンIDのインデックス値
	*
	*/
	void QueueManager::complementLaneId(const Schema &schema, Tuple &tuple, const int laneIdIdx) {
		//REL_COMMENT logger->trace("[complementLaneId] IN");
		any val;
		try {
			//IS::LocationManager &LM = IS::LocationManager::get_instance();
			tuple.getValueByIdx(laneIdIdx, val);
			if (schema.getTableName() == "object_info" || schema.getTableName() == "freespace_info") {
				//if (any_cast<string>(val) == "0000000000000000") {
					unsigned long long laneID;
					logger->debug("[complementLaneId]prioritySensor:" + prioritySensor);
					if (prioritySensor == "LIDAR") {
						laneID = estimatingLaneIdByUsingCache(schema, tuple);
					} else {
						laneID = estimatingLaneId(schema, tuple);
					}
					logger->debug("[complementLaneId]laneID:" + to_string(laneID));
					string strLaneID = "0000000000000000";
					if (laneID != 0) {
						strLaneID = to_string(laneID);
						// [Memo]laneIDのlong化対応
						//LM.setLaneId(laneID);
					}
					if (!tuple.updateValueByIdx(laneIdIdx, strLaneID)) {
						logger->error("[complementLaneId] set error " + tableLaneIdNameMap[schema.getTableName()]);
					}
				//}
				//else {
				//}
			} else {
				if (any_cast<long>(val) == 0) {
					unsigned long long laneID;
					if (prioritySensor == "LIDAR") {
						laneID = estimatingLaneIdByUsingCache(schema, tuple);
					} else {
						laneID = estimatingLaneId(schema, tuple);
					}
					if (laneID == 0) return;
					// [Memo]laneIDのlong化対応
					//LM.setLaneId(laneID);
					if (!tuple.updateValueByIdx(laneIdIdx, laneID)) {
						logger->error("[complementLaneId] set error " + tableLaneIdNameMap[schema.getTableName()]);
					}
				}
				else {
					// [Memo]laneIDのlong化対応
					// 既に付与されている場合はこちらを真とする
					//LM.setLaneId(any_cast<long>(val));
				}
			}
		}
		catch (std::exception& e) {
			string what(e.what());
			logger->error("what:" + what);
			string valType = Demangle(val.type()).realname;
			logger->error("[complementLaneId] any_cast error. column:" + tableLaneIdNameMap[schema.getTableName()] + " valType:" + valType);
		}
		//REL_COMMENT logger->trace("[complementLaneId] OUT");
		return;
	}

	/**
	* レーンIDの取得
	*
	* @author	Nagoya University
	* @date	2018/08/02
	*
	* @param	schema	スキーマ情報
	* @param [in]	tuple	タプル
	*
	* @return	レーンID
	*/
	//long QueueManager::estimatingLaneIdByUsingCache(Schema &schema, Tuple tuple) {
	unsigned long long QueueManager::estimatingLaneIdByUsingCache(const Schema &schema, Tuple tuple) {

		any val;
		//long laneId = 0;
		unsigned long long laneId = 0;
		string srid = settings.getParameter("PLANE_NUMBER_SRID");
		// スキーマ情報からX,Y,Zの座標情報を取得
		string posSetKey = "POSSET_" + schema.getTableName();
		vector<string> posList;
		if (addPosSetMap.find(posSetKey) == addPosSetMap.end()) {
			posList = stringUtil.split(settings.readConfigFile(posSetKey.c_str(), settings.CONF_FILE_SYS), ",");
			if (posList.size() == 0) {
				logger->warn("Not defined parameter:" + posSetKey + " So Not processing estimatingLaneId.");
				return laneId;
			}
			addPosSetMap[posSetKey] = posList;
		} else {
			posList = addPosSetMap[posSetKey];
		}
		// スキーマ情報からVehicleId情報を取得
		string vidSetKey = "VIDSET_" + schema.getTableName();
		string vidAttr = "";
		string vid = "";
		if (addVIdSetMap.find(vidSetKey) == addVIdSetMap.end()) {
			vidAttr = settings.readConfigFile(vidSetKey.c_str(), settings.CONF_FILE_SYS);
			if (posList.size() != 0) {
				addVIdSetMap[vidSetKey] = vidAttr;
			}
		} else {
			vidAttr = addVIdSetMap[vidSetKey];
		}
		if (vidAttr != "") {
			int vid_idx = schema.getAttributeIdx(vidAttr);
			if (vid_idx >= 0) {
				tuple.getValueByIdx(vid_idx, val);
				vid = stringUtil.getAnyString(val);
			}
		}
		//cout << "[DEBUG]srid:" << srid << ",vidAttr:" << vidAttr ",vid:" << vid << endl;
		string x, y, z = "";
		int x_idx = schema.getAttributeIdx(posList.at(0));
		int y_idx = schema.getAttributeIdx(posList.at(1));
		int z_idx = -1;
		if (posList.size() > 2) {
			z_idx = schema.getAttributeIdx(posList.at(2));
		}
		if (x_idx >= 0 && y_idx >= 0) {
			tuple.getValueByIdx(x_idx, val);
			x = stringUtil.getAnyString(val);
			tuple.getValueByIdx(y_idx, val);
			y = stringUtil.getAnyString(val);
		}
		else {
			logger->error("Failed to read column. Not found column:" + posList.at(0) + "," + posList.at(1) + " So Not processing estimatingLaneId.");
			return laneId;
		}
		if (x.length() <= 7 || y.length() <= 7) return laneId;
		string ux = x.substr(0, x.length() - 7) + "." + x.substr(x.length() - 7);
		string uy = y.substr(0, y.length() - 7) + "." + y.substr(y.length() - 7);
		string pointObj = "POINT(" + ux + " " + uy + ")";
		if (z_idx >= 0) {
			tuple.getValueByIdx(z_idx, val);
			z = stringUtil.getAnyString(val);
			string uz = z.substr(0, z.length() - 4) + "." + z.substr(z.length() - 4);
			pointObj = "POINT(" + ux + " " + uy + " " + uz + ")";
		}
		//cout << "[DEBUG]pointObj:" << pointObj << endl;
		// WKT -> Geometry
		GEOSGeometry* g2;
		g2 = GEOSGeomFromWKT(pointObj.c_str());
		if (g2 == NULL) {
			return laneId;
		}
		// GeometryにSRIDを設定
		GEOSSetSRID(g2,stoi(srid));
		string setKey = "";
		for (auto itr = laneIdGeometryMap.begin(); itr != laneIdGeometryMap.end(); ++itr) {
			if (itr->second == NULL) continue;
			int result = GEOSCovers(itr->second, g2);
			int min_path_diff = std::numeric_limits<int>::max();
			if (result == 1) {
				//物標の位置がレーン内である場合
				if (vid != "" && vIdPathMap.size() > 0) {
					//車両の経路計画情報を受信済みの場合
					string key = vid + "," + to_string(itr->first);
					if (vIdPathMap.find(key) != vIdPathMap.end()) {
						// 物標の位置が経路計画上のレーンの場合、見つかったレーンの計画No.と、前回のNo.の差分を取る。
						int path_diff = abs(vIdPathMap[key] - vIdCuurentLaneMap[vid]);
						//cout << itr->first << "[Diff:" << path_diff << "],";
						if (path_diff <= 1) {
							// 差分が1パス以内の場合（計画通りに進んでいる場合）
							laneId = itr->first;
							setKey = key;
							break;
						} else if (path_diff < min_path_diff) {
							// 計画上にはあるが、直近のレーンでない場合、ループ継続。次点の候補として、最小の差分のレーンとする
							laneId = itr->first;
							setKey = key;
							min_path_diff = path_diff;
						}
					} else {
						// 計画外のレーンの場合、ループ継続。
						if (min_path_diff == std::numeric_limits<int>::max()) {
							// 全て計画外だった場合に備えて、候補のレーンをセットするが、計画内のレーンを受信した場合は候補は上書き。
							laneId = itr->first;
						}
					}
				} else {
					// 経路計画情報を受信していない場合は最初に見つかったレーンで確定
					laneId = itr->first;
					break;
				}
			}
		}
		if (setKey != "") {
			vIdCuurentLaneMap[vid] = vIdPathMap[setKey];
		}
		//cout << "Path:" << ",laneid:"  << laneId << ",Point:" << pointObj << ",vid:" << vid << endl;
		GEOSGeom_destroy(g2);
		return laneId;
	}

	/**
	* レーンIDの取得
	*
	* @author	Nagoya University
	* @date	2018/08/02
	*
	* @param	schema	スキーマ情報
	* @param [in]	tuple	タプル
	*
	* @return	レーンID
	*/
	//long QueueManager::estimatingLaneId(Schema &schema, Tuple tuple) {
	unsigned long long QueueManager::estimatingLaneId(const Schema &schema, Tuple tuple) {
		//REL_COMMENT logger->trace("[estimatingLaneId] IN");
		//long laneId = 0;
		unsigned long long laneId = 0;

		// スキーマ情報からX,Y,Zの座標情報を取得
		string key = "POSSET_" + schema.getTableName();
		vector<string> posList = stringUtil.split(settings.readConfigFile(key.c_str(), settings.CONF_FILE_SYS), ",");
		if (posList.size() == 0) {
			logger->warn("Not defined parameter:" + key + " So Not processing estimatingLaneId.");
			return laneId;
		}
		string schemeName = settings.getParameter("QUERY_SCHEME_NAME");
		string srid = settings.getParameter("PLANE_NUMBER_SRID");

		if (schemeName.length() > 0) schemeName.append(".");

		// SQL問い合わせ
		// RDBへの接続
		try {
			string x, y, z = "";
			any val;
			int x_idx = schema.getAttributeIdx(posList.at(0));
			int y_idx = schema.getAttributeIdx(posList.at(1));
			if (x_idx >= 0 && y_idx >= 0) {
				tuple.getValueByIdx(x_idx, val);
				x = stringUtil.getAnyString(val);
				tuple.getValueByIdx(y_idx, val);
				y = " " + stringUtil.getAnyString(val);
			}
			else {
				logger->error("Failed to read column. Not found column:" + posList.at(0) + "," + posList.at(1) + " So Not processing estimatingLaneId.");
				return laneId;
			}
			
			if (posList.size() > 2) {
				int z_idx = schema.getAttributeIdx(posList.at(2));
				if (z_idx >= 0) {
					tuple.getValueByIdx(z_idx, val);
					z = " " + stringUtil.getAnyString(val);
				}
			}
			string query = "";
			// 包含関係からの検索
			//query.append("SELECT pcfp.comp_feature_id FROM ");
			//query.append("  " + schemeName + "pf_comp_feat_part pcfp, ");
			//query.append("  " + schemeName + "pf_area_feature paf, ");
			//query.append("  " + schemeName + "pf_area_topo_prim patp, ");
			//query.append("  " + schemeName + "st_face sf ");
			//query.append("WHERE ");
			//query.append("  patp.face_id IN(");
			//query.append("    SELECT face_id FROM " + schemeName + "st_face ");
			//query.append("      WHERE ST_Intersects(ST_Transform(mbr," + srid + "),ST_GeomFromEWKT('SRID=" + srid + ";POINT(" + x + y + z + ")') )");	// transform(COIFormat's SRID -> settingParameter SRID)
			//query.append("  ) ");
			//query.append("  AND sf.face_id = patp.face_id ");
			//query.append("  AND patp.feature_id = paf.feature_id ");
			//query.append("  AND paf.feature_class_code in('8120', '8170')");	// Lane Area(8120) or Intersection Area Shape(8170)
			//query.append("  AND paf.feature_id = pcfp.feature_id ");
			//query.append("  AND pcfp.feature_category_num = 3; ");				// Area(3)
			////REL_COMMENT logger->trace(query);

			// DB接続
			connection Conn(("dbname=" + settings.getParameter("DATABASE_NAME") + " user=" + settings.getParameter("USER_NAME") + " password=" + settings.getParameter("DB_PASS")  + " host=" + settings.getParameter("DATABASE_ADDR") + " port=" + settings.getParameter("DATABASE_PORT")));
			work T(Conn);

			//// 包含関係からの検索：SQLの実行
			//result R2(T.exec(query));

			int rowsNum = 0;
			string ret = "";
			std::map<long, double> distList;

			//// 包含関係からの検索：データ部の取得
			//for (result::const_iterator c = R2.begin(); c != R2.end(); ++c) {
			//	laneId = c[0].as(int());
			//	if (rowsNum != 0) ret.append(",");
			//	ret.append(std::to_string(laneId));
			//	rowsNum++;
			//}
			// 何も包含関係にない場合は近いレーンを探索
			if (rowsNum == 0) {
				//logger->warn("[estimatingLaneId] Not found LaneId, So find using ST_Distance. INPUT X:" + x + " Y:" + y + " Z:" + z + " SRID:" + srid);
				string limitNum = settings.getParameter("ADD_LANE_SEARCH_LIMIT");
				if (limitNum.length() == 0) limitNum = "4";
				query = "";
				if (schema.getTableName() == "object_info") {
					// 包含関係をDBで検索する場合
					// query.append("SELECT lanelet_id, 0.0, ST_AsEwkt(geometry)");
					// query.append("FROM " + schemeName + "Lanelet ");
					// query.append("WHERE ST_Covers(geometry, ST_SetSRID(ST_MakePoint(cast(" + x + " as float)/10000000,cast(" + y + " as float)/10000000)," + srid + ") ) = TRUE  ");
					// query.append("LIMIT 1");
					query.append("SELECT lanelet_id, ");
					query.append("ST_Distance(geometry, ST_SetSRID(ST_MakePoint(cast(" + x + " as float)/10000000,cast(" + y + " as float)/10000000)," + srid + ") )  ");
					query.append("AS dist FROM " + schemeName + "Lanelet ORDER BY dist DESC LIMIT " + limitNum);
				} else {
					query.append("SELECT pcfp.comp_feature_id, distList.dist FROM ");
					query.append("  " + schemeName + "pf_comp_feat_part pcfp, ");
					query.append("  " + schemeName + "pf_area_feature paf, ");
					query.append("  " + schemeName + "pf_area_topo_prim patp, ");
					query.append("  " + schemeName + "st_face sf, ");
					query.append("  (SELECT face_id, ST_Distance(ST_Transform(mbr," + srid + "),ST_GeomFromEWKT('SRID=" + srid + ";POINT(" + x + y + z + ")') ) ");	// transform(COIFormat's SRID -> settingParameter SRID)
					query.append("        AS dist FROM " + schemeName + "st_face order by dist limit " + limitNum + ") as distList ");
					query.append("WHERE ");
					query.append("  patp.face_id = distList.face_id ");
					query.append("  AND sf.face_id = patp.face_id ");
					query.append("  AND patp.feature_id = paf.feature_id ");
					query.append("  AND paf.feature_class_code in('8120', '8170')");	// Lane Area(8120) or Intersection Area Shape(8170)
					query.append("  AND paf.feature_id = pcfp.feature_id ");
					query.append("  AND pcfp.feature_category_num = 3 ");				// Area(3)
					query.append("  ORDER BY distList.dist desc; ");
				}
				//REL_COMMENT logger->trace(query);

				// SQLの実行
				result R2(T.exec(query));
				// データ部の取得
				for (result::const_iterator c = R2.begin(); c != R2.end(); ++c) {
					laneId = c[0].as(long());
					if (rowsNum != 0) ret.append(",");
					ret.append(std::to_string(laneId));
					distList[laneId] = c[1].as(double());
					rowsNum++;
					logger->debug("laneId:" + to_string(laneId) + " distance:" + to_string(distList[laneId]));
				}
			}
			logger->debug("[estimatingLaneId] Multiple rows matched. ans:" + ret + " INPUT X:" + x + " Y:" + y + " Z:" + z + " SRID:" + srid);

			double roll = 0, pitch = 0;
			if (schema.isExistAttribute("roll")) {
				tuple.getValueByIdx(schema.getAttributeIdx("roll"), val);
				roll = any_cast<double>(val);
			}
			if (schema.isExistAttribute("pitch")) {
				tuple.getValueByIdx(schema.getAttributeIdx("pitch"), val);
				pitch = any_cast<double>(val);
			}

			int thresholdYaw = stoi(settings.getParameter("ADD_LANE_SEARCH_YAW_THRESHOLD"));
			// 候補が複数存在し、かつ車両の向きが正常に取得出来ている場合
			//if (rowsNum > 1 && roll != 0 && pitch != 0) {
			if (rowsNum > 1 && thresholdYaw != 0) {

				double yaw = 0;
				// 車両の向きによる絞込みを実施
				if (schema.isExistAttribute("yaw")) {
					tuple.getValueByIdx(schema.getAttributeIdx("yaw"), val);
					yaw = any_cast<double>(val);
				}
				else {
					logger->debug("[estimatingLaneId]  Multiple rows matched but not defined schema:yaw. So Select the closest laneId : " + std::to_string(laneId) + " Candidate : " + ret + " Dist : " + to_string(distList[laneId]) + " thresholdYaw : " + to_string(thresholdYaw));
					return laneId;
				}
				double orgYaw = yaw;

				if (0 > yaw) {
					yaw = M_PI + (M_PI - fabs(yaw));	// 範囲を変更(-M_PI～M_PI => 0～M_PI*2)
				}
				yaw = fabs(yaw - M_PI * 2);		// 反時計周りの扱いを時計回りへ
				yaw = fabs(yaw + M_PI / 2);		// 基軸を東から北へ
				if (yaw > M_PI * 2) {
					yaw = yaw - M_PI * 2;
				}
				logger->debug("[estimatingLaneId] Estimating by yaw. roll:" + to_string(roll) + " pitch:" + to_string(pitch) + " yaw:" + to_string(orgYaw) + " convert_yaw:" + to_string(yaw));

				query = "";
				if (schema.getTableName() == "object_info") {
					query.append("SELECT ll.lanelet_id, ST_Azimuth(p1.geometry, p2.geometry) ");
					query.append("FROM " + schemeName + "lanelet AS ll, " + schemeName + "linestring AS ls, " + schemeName + "point AS p1, " + schemeName + "point AS p2 ");
					query.append("WHERE ll.lanelet_id in (" + ret + ") ");
					query.append("  AND ll.ceneterline_id = ls.linestring_id ");
					query.append("  AND ls.point_ids[1] = p1.PointId ");
					query.append("  AND ls.point_ids[array_length(point_ids, 1)] = p2.point_id ");
				} else {
					query.append("SELECT edge_id, ST_Azimuth( ");
					query.append("   ST_StartPoint(geometry), ST_EndPoint(geometry) ");
					query.append(") FROM " + schemeName + "st_edge ");
					query.append("WHERE ");
					query.append("  edge_id IN(");
					query.append("    SELECT se.edge_id FROM ");
					query.append("      " + schemeName + "pf_comp_feat_part pcfp,");
					query.append("      " + schemeName + "pf_line_feature plf,");
					query.append("      " + schemeName + "pf_line_topo_prim pltp,");
					query.append("      " + schemeName + "st_edge se");
					query.append("    WHERE ");
					query.append("      pcfp.comp_feature_id in (" + ret + ") AND ");
					query.append("      pcfp.feature_category_num = 2 AND ");				// Line(2)
					query.append("      pcfp.feature_id = plf.feature_id AND ");
					query.append("      plf.feature_id = pltp.feature_id AND ");
					query.append("      pltp.edge_id = se.edge_id ");
					query.append(");");
				}
				//REL_COMMENT logger->trace(query);

				// SQLの実行
				result R2(T.exec(query));

				laneId = 0;
				double diff = M_PI / (180 / thresholdYaw);
				double distance = stod(settings.getParameter("ADD_LANE_SEARCH_DIST_THRESHOLD"));

				// データ部の取得
				for (result::const_iterator c = R2.begin(); c != R2.end(); ++c) {
					// 時計回りからの差分
					double yawDiff = fabs(c[1].as(double()) - yaw);
					//差分がM_PIを超える場合は反時計回りの差分を取得
					if (yawDiff > M_PI) yawDiff = M_PI * 2 - yawDiff; 

					if (yawDiff < diff && distList.count(c[0].as(int())) > 0 && distance > distList[c[0].as(int())]) {
						laneId = c[0].as(long());
						if (prioritySensor == "IMU") {
							diff = yawDiff;
						}
						else {
							distance = distList[c[0].as(int())];
						}
						//REL_COMMENT logger->trace(" Right laneId:" + to_string(laneId) + " diff:" + to_string(fabs(c[1].as(double()) - yaw)) + " distance:" + to_string(distance) );
					}
					logger->debug("laneId:" + to_string(c[0].as(int())) + " distance:" + to_string(distList[c[0].as(int())]) + " lane_yaw:" + to_string(c[1].as(double())) + "(" + to_string(c[1].as(double())/(2*M_PI) * 360) + ") yawDiff:" + to_string(yawDiff));
				}
				logger->debug("[estimatingLaneId] Select area from the candidates area. laneId:" + std::to_string(laneId) + " car_yaw:" + to_string(yaw) + "(" + to_string(yaw / (2 * M_PI) * 360) + ") dist:" + to_string(distance) + " thresholdYawDiff:" + to_string(diff));
			}
			else {
				if (rowsNum == 1) {
					logger->debug("[estimatingLaneId] Area matched. laneId:" + std::to_string(laneId) + " INPUT X:" + x + " Y:" + y + " Z:" + z + " SRID:" + srid);
				}
				else {
					logger->debug("[estimatingLaneId]  Multiple rows matched but not defined parameter:ADD_LANE_SEARCH_YAW_THRESHOLD = 0. So Select the closest laneId:" + std::to_string(laneId) + " Candidate:" + ret + " Dist:" + to_string(distList[laneId]) + " thresholdYaw:" + to_string(thresholdYaw));
				}
			}
			T.commit();
			//Conn.disconnect();
		}
		catch (const exception &e) {
			string what(e.what());
			logger->error("[estimatingLaneId] ERROR. cause:" + what);
			return false;
		}

		//REL_COMMENT logger->trace("[estimatingLaneId] OUT laneId:" + std::to_string(laneId));
		return laneId;

	}

	/**
	* ストリーム受信数を取得する
	*
	* @author	Nagoya University
	* @date	2018/10/29
	*
	* @return	ストリーム受信数
	*/
	int QueueManager::getStreamRcvInfo()
	{
		//REL_COMMENT logger->trace("[getStreamRcvInfo] IN mp.size():" + std::to_string(mp.size()));
		int sum = 0;
		try {
			for (auto itr = mp.begin(); itr != mp.end(); ++itr) {
				sum = sum + itr->second->getStreamRcvNum();
			}
		}
		catch (exception& e) {
			string what(e.what());
			logger->error("[getStreamRcvInfo]: " + what);
		}
		//REL_COMMENT logger->trace("[getStreamRcvInfo] OUT");
		return sum;
	}

	/**
	* ストリーム受信数を取得する
	*
	* @author	Nagoya University
	* @date	2018/10/29
	*
	* @param	streamName	ストリーム名
	* @return	ストリーム受信数
	*/
	int QueueManager::getStreamRcvInfo(const string &streamName)
	{
		//REL_COMMENT logger->trace("[getStreamRcvInfo] IN mp.size():" + std::to_string(mp.size()));
		int sum = 0;
		try {
			for (auto itr = mp.begin(); itr != mp.end(); ++itr) {
				if (itr->first == streamName) {
					sum = itr->second->getStreamRcvNum();
					break;
				}
			}
		}
		catch (exception& e) {
			string what(e.what());
			logger->error("[getStreamRcvInfo]: " + what);
		}
		//REL_COMMENT logger->trace("[getStreamRcvInfo] OUT");
		return sum;
	}

	/**
	* SIDを扱うスキーマ情報を取得する
	*
	* @author	Nagoya University
	* @date	2019/12/19
	*
	* @return	SIDスキーマ設定
	*/
	std::map<string, string> QueueManager::getSidSchemaMap()
	{
		return this->tableSidNameMap;
	}

	/**
	* レーンIDを扱うスキーマ情報を取得する
	*
	* @author	Nagoya University
	* @date	2019/12/19
	*
	* @return	レーンIDスキーマ設定
	*/
	std::map<string, string> QueueManager::getLaneIdSchemaMap()
	{
		return this->tableLaneIdNameMap;
	}
}
