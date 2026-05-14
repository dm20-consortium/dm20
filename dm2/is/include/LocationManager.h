#ifndef LOCATIONMANAGER_H
#define LOCATIONMANAGER_H

// LocationManagerクラス

#include <mutex>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <log4cxx/logger.h>
#include <pqxx/pqxx>

#include "InformationSourceParser.h"
#include "is/StringUtil.h"
#include "is/DmUtil.h"
#include "Settings.h"
#include "TimerWorker.h"

using namespace std;
using namespace log4cxx;

namespace IS {

	
	/**
	* 位置管理情報
	*
	* @author	Nagoya University
	* @date	2019/12/11
	*/
	
	class PosMng
	{
	public:
		struct TranInfo {
			/*
			unsigned int edgeSID;
			unsigned int mngID;
			*/
			unsigned long long edgeSID;
			unsigned long long mngID;
		};
		struct MngInfo {
			//unsigned int requestedVehicleId;
			unsigned long long requestedVehicleId;
			unsigned int range;
			//unsigned int receptionEdgeId;
			unsigned long long receptionEdgeId;
			unsigned int receptionMngId;
			string tranOperatorTreeXML;
			vector<TranInfo> tranInfoList;
		};

		//int laneId = 0;
		unsigned long long laneId = 0;
		long updateTime = 0;
		unordered_map<string, MngInfo> Info;		// key:受付エッジSID_受付クエリ管理番号 value:管理情報

		PosMng(const string &tranOperatorTreeXML, 
			const unsigned int &range, 
			//const unsigned int requestedVehicleId,
			const unsigned long long requestedVehicleId, 
			const unsigned int receptionMngId,
			//const unsigned int receptionEdgeId);
			const unsigned long long receptionEdgeId);
		string getTranEdgeSID(const string &mngKey);

	};
	

	/**
	 * 位置情報を管理するクラス
	 *
	 * @author	Nagoya University
	 * @date	2019/12/11
	 */

	class LocationManager
	{
	private:
		LoggerPtr logger = Logger::getLogger("LocationManager");
		LocationManager() = default;
		~LocationManager() = default;
		IS::Settings &settings = IS::Settings::get_instance();
		IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		StringUtil stringUtil;
		TimerWorker* tw = NULL;
		/*
		unordered_map<unsigned int, IS::PosMng> p_mp;			// key:車両SID value:連携位置情報
		unordered_map<unsigned int, long> vPosUpdateMap;		// key:車両SID value:位置情報更新時間
		unordered_map<int, int> lid_controll_edge_sid_map;		//LaneID⇔管轄エッジ管理用非順序型MAPオブジェクト生成。key値：LaneID, value値：管轄エッジSID
		*/
		unordered_map<unsigned long long, IS::PosMng> p_mp;			// key:車両SID value:連携位置情報
		unordered_map<unsigned long long, long> vPosUpdateMap;		// key:車両SID value:位置情報更新時間
		unordered_map<unsigned long long, unsigned long long> lid_controll_edge_sid_map;		//LaneID⇔管轄エッジ管理用非順序型MAPオブジェクト生成。key値：LaneID, value値：管轄エッジSID
		map<string, string> sidSchemaMap;
		map<string, string> laneIdSchemaMap;

		int laneId = 0;
		vector<string> targetStreamList;
		pthread_mutex_t P_MAP_MTX;

		//unordered_map<int, int> loadLidCtrlCsv(const char* filename);
		unordered_map<unsigned long long, unsigned long long> loadLidCtrlCsv(const char* filename);
		//void checkAreaOfMonitoredVehicle(unsigned int vehicleSID);
		void checkAreaOfMonitoredVehicle(unsigned long long vehicleSID);

	public:
		LocationManager(const LocationManager&) = delete;
		LocationManager& operator=(const LocationManager&) = delete;
		LocationManager(LocationManager&&) = delete;
		LocationManager& operator=(LocationManager&&) = delete;

		static LocationManager& get_instance() {
			static LocationManager inst;
			return inst;
		}
		/*
		void setLaneId(int laneId);
		void addLocationManagementTask(unsigned int vehicleSID, PosMng posInfo);
		void addTranInfo(unsigned int vehicleSID, const string &mngKey, PosMng::TranInfo tranInfo);
		void updateTranMngId(const string &mngKey, unsigned int edgeSID, unsigned int mngId);
		void updateLaneId(unsigned int vehicleSID, int lane_id);
		*/
		void setLaneId(unsigned long long laneId);
		void addLocationManagementTask(unsigned long long vehicleSID, PosMng posInfo);
		void addTranInfo(unsigned long long vehicleSID, const string &mngKey, PosMng::TranInfo tranInfo);
		void updateTranMngId(const string &mngKey, unsigned long long edgeSID, unsigned int mngId);
		void updateLaneId(unsigned long long vehicleSID, unsigned long long lane_id);
		void deleteTask(const string &mngKey);
		//int  getLaneId();
		unsigned long long getLaneId();
		bool isHandlingLocation(const string &streamName, string &sidColName, string &laneIdColName);
		bool isExistTask(const string &mngKey, unsigned int &mngId);
		/*
		bool isExistTask(unsigned int vehicleSID, const string &mngKey, unsigned int &mngId);
		vector<unsigned int> getLocationMngVehicleID();
		unordered_map<unsigned int, unsigned int> getNearEdgeSID(unsigned int vehicleSID, const string &mngKey);
		*/
		bool isExistTask(unsigned long long vehicleSID, const string &mngKey, unsigned int &mngId);
		vector<unsigned long long> getLocationMngVehicleID();
		unordered_map<unsigned long long, unsigned long long> getNearEdgeSID(unsigned long long vehicleSID, const string &mngKey);
		void checkOutOfAreaProc();

		void init();       // impl
		void stop();       // impl

	};
}
#endif  // LOCATIONMANAGER_H