#ifndef QUEUEMANAGER_H
#define QUEUEMANAGER_H

// QueueManagerクラス

#include <unordered_map>
#include <log4cxx/logger.h>
#include <geos_c.h>

#include "InformationSource.h"
#include "is/Schema.h"
#include "is/Tuple.h"
#include "is/TupleSet.h"
#include "Operator.h"
#include "Queue.h"
#include "QueryExecuter.h"
#include "InformationSourceManager.h"
#include "NetworkSource.h"
#include "Settings.h"
#include "ProtobufParser.h"

using namespace std;
using namespace log4cxx;

namespace IS {

	/**
	 * キューを管理するクラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	class QueueManager
	{
	private:
		LoggerPtr logger = Logger::getLogger("QueueManager");
		QueueManager() = default;
		~QueueManager() = default;

		int sock;
		struct sockaddr_in addr;
		string query;
		std::unordered_map<string, IS::Queue*> mp;
		//int laneId = 0;
		unsigned long long laneId = 0;
		string prioritySensor;

		IS::Settings &settings = IS::Settings::get_instance();
		//IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		IS::InformationSourceManager &ism = IS::InformationSourceManager::get_instance();
		StringUtil stringUtil;
		vector<string> schemaList;
		// 【初期処理】設定ファイル読込み用マップ
		std::map<string, string> tableSidNameMap;				// ADD_SID_SCHEMAのテーブル名、SIDのカラム名
		std::map<string, int> tableSidIdxMap;					// （上記を元に生成）テーブル名、SIDのインデックス値
		std::map<string, string> tableLaneIdNameMap;			// ADD_LANE_ID_SCHEMAのテーブル名、レーンID
		std::map<string, int> tableLaneIdIdxMap;				// （上記を元に生成）テーブル名、レーンIDのインデックス値
		std::map<string, string> pathPlanVidNameMap;			// ADD_LANE_SEARCH_PRIORITY_PLAN_VIDのテーブル名、VIDのカラム名
		std::map<string, int> pathPlanVidIdxMap;				// （上記を元に生成）テーブル名、VIDのインデックス値
		std::map<string, string> pathPlanLaneNameMap;			// ADD_LANE_SEARCH_PRIORITY_PLAN_LANEのテーブル名、レーンID
		std::map<string, int> pathPlanLaneIdxMap;				// （上記を元に生成）テーブル名、レーンIDのインデックス値
		std::map<string, vector<string>> addPosSetMap;			// POSSET_XX
		std::map<string, string> addVIdSetMap;					// VISSET_XX

		// 内部マップ
		std::map<string, int> vIdPathMap;						// tableLaneIdIdxMapをトリガーにして動的情報から生成
		std::map<string, int> vIdCuurentLaneMap;				// tableLaneIdIdxMapをトリガーにして動的情報から生成


		std::map<unsigned long long, GEOSGeometry *> laneIdGeometryMap;
		void run();
		void makeVidPath(const Tuple &tuple, const int tupleIdx, const int vidIdx, const int laneIdx);
		void complementLaneId(const Schema &schema, Tuple &tuple, const int laneIdIdx);
		unsigned long long estimatingLaneId(const Schema &schema, Tuple tuple);
		unsigned long long estimatingLaneIdByUsingCache(const Schema &schema, Tuple tuple);
		vector<string> getTupleHash(const string &xmlTuples);
		vector<string> getTupleHash(const vector<Tuple> &tuples);
		void complementTuples(const string &tableName, const Schema &schema, vector<Tuple> &tuples, const string &user, const string &ip_addr, const vector<string> &tupleHashes, const bool callDmi);
		void recvDataToTuples(const RecvData &data, vector<Tuple> &tuples, Schema &schema, string &tableName);
	public:
		QueueManager(const QueueManager&) = delete;
		QueueManager& operator=(const QueueManager&) = delete;
		QueueManager(QueueManager&&) = delete;
		QueueManager& operator=(QueueManager&&) = delete;

		static QueueManager& get_instance() {
			static QueueManager inst;
			return inst;
		}
		Queue* createQueue(const string &tableName, const Schema &schema);
		void createTempQueue(const string &tableName, const string &schemaName);
		bool removeQueue(const string &tableName);
		void addQueueProc(const RecvData &data, const string &user);
		void addQueueProc(const RecvData &data);
		void addQueueProc(vector<Tuple> tuples, string &ip_addr, const string &user);
		void addTuple(const Schema &schema, const vector<Tuple> &tuples);
		void addQueueListener(const string &streamName, Operator* ope);
		void removeQueueListener(const string &streamName, Operator* ope);
		void addQueueReferrer(const string &streamName, Operator* ope);
		void removeQueueReferrer(const string &streamName, Operator* ope);
		void getTupleSetPartition(const string &streamName, TupleSet &tupleset, const int num, vector<int> &partitionList);
		void getTupleSet(const string &streamName, const long lastRecvTime, TupleSet &tupleset);
		void getTupleSet(const string &streamName, const int num, const long referenceTime, TupleSet &tupleset);
		void getTupleSet(const string &streamName, const long rangeMsec, const long notifiedTime, const long referenceTime, TupleSet &tupleset);
		void getSchema(const string &streamName, Schema &schemaInfo);
		string getLaneIdSchema(const string &streamName);
		bool isFound(const string &streamName);
		bool isSelect(const string &streamName, const string &user);
		bool isRefAdminCol(const string &streamName, const string &user);
		int  getStreamRcvInfo();
		int  getStreamRcvInfo(const string &streamName);
		std::map<string, string> getSidSchemaMap();
		std::map<string, string> getLaneIdSchemaMap();
		void printQueue();

		void init();       // impl
		void initQueue();
		void start();      // impl
		void stop();       // impl
	};
}
#endif  // QUEUEMANAGER_H
