#ifndef QUERYMANAGER_H
#define QUERYMANAGER_H

// QueryManagerクラス

#include <mutex>
#include <unordered_map>
#include <dirent.h>
#include <sys/stat.h>
#include <log4cxx/logger.h>

#include "InformationSource.h"
#include "InformationSourceParser.h"
#include "is/Schema.h"
#include "is/Tuple.h"
#include "is/TupleSet.h"
#include "Operator.h"
#include "ResponseOperator.h"
#include "QueryExecuter.h"
#include "Settings.h"
#include "is/StringUtil.h"
#include "ErrorCode.h"
#include "NetworkSource.h"
#include "LocationManager.h"

using namespace std;
using namespace log4cxx;

namespace IS {

	/**
	 * クエリの管理情報
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	class QueryInfo
	{
	public:
		unsigned int queryId;
		int sock;
		struct sockaddr_in addr;
		string query;
		QueryExecuter *QE;
		bool isContinuousQuery;
		QueryInfo(const unsigned int id, const int sock, const struct sockaddr_in addr, const string query, IS::QueryExecuter *qe, bool isContinuousQuery);
	};

	/**
	 * クエリを管理するクラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	class QueryManager
	{
	private:
		LoggerPtr logger = Logger::getLogger("QueryManager");
		QueryManager() = default;
		~QueryManager() = default;

		//IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		int sock;
		struct sockaddr_in addr;
		string query;
		std::unordered_map<unsigned int, IS::QueryInfo> mp;
		std::unordered_map<int, vector<unsigned int>> sockmng;
		std::unordered_map<unsigned int, string> presetQueryList;
		// 管理番号
		unsigned int MNGID = 0;
		bool exit_flag = false;

		pthread_mutex_t QImapMtx;
		pthread_mutex_t mngIdMtx;
		pthread_mutex_t sockMtx;
		pthread_mutex_t createMtx;

		StringUtil stringUtil;
		IS::Settings &settings = IS::Settings::get_instance();
		string pid;
		
		void run();
		void createQueryHistory(string action, int mngId, int sock, string payload, string memo, const string &user);
	public:
		QueryManager(const QueryManager&) = delete;
		QueryManager& operator=(const QueryManager&) = delete;
		QueryManager(QueryManager&&) = delete;
		QueryManager& operator=(QueryManager&&) = delete;

		static QueryManager& get_instance() {
			static QueryManager inst;
			return inst;
		}
		string parseQuery(const string &query);
		int getMngId();
		void addQuery(const string &user, const RecvData &data, const query_header &query_header_);
		void addQuery(const int &mngId, const string &user, const RecvData &data, const bool &returnMngId, const query_header &query_header_);
		void addQuery(const string &query);
		void addQuery(const int &mngId, const string &user, const string &query, const string &dmiName);
		void addOperatorTree(const string &user, const RecvData &data);
		//void addRequest(const unsigned int mngId, const unsigned int destSID, PosMng::MngInfo mngInfo, const string &edgeSIDListStr);
		void addRequest(const unsigned int mngId, const unsigned long long destSID, PosMng::MngInfo mngInfo, const string &edgeSIDListStr);
		void registerResult(const string &query);
		void returnError(const unsigned int mngId, const ErrorCode &code, const string &msg);
		void cancelQuery(const string &user, const RecvData &data, const unsigned int mngId);
		bool cancelQuery(const unsigned int mngId, const string &user);
		bool cancelQuery(const unsigned int mngId);
		//void cancelQuery(const unsigned int mngId, const int receptionSID, const int senderSID);
		void cancelQuery(const unsigned int mngId, const unsigned long long receptionSID, const unsigned long long senderSID);
		void cancelQuery(const RecvData &data);
		/*
		void cancelRequest(const unsigned int mngId, const unsigned int destSID, PosMng::MngInfo mngInfo);
		void removeRequestOp(const unsigned int mngId, const unsigned int destSID);
		*/
		void cancelRequest(const unsigned int mngId, const unsigned long long destSID, PosMng::MngInfo mngInfo);
		void removeRequestOp(const unsigned int mngId, const unsigned long long destSID);
		bool isOneShot(const unsigned int mngId);
		int  getRegisterQueryNum();
		vector<QueryStatus>  getQueryStatus();
		int registerPresetQueryUsingPath(const string &path, const bool &continuous);
		void registerPresetQueryUsingSchema(const string &schema, const string &window);
		void init();       // impl
		void start();      // impl
		void stop();       // impl

	};
}
#endif  // QUERYMANAGER_H
