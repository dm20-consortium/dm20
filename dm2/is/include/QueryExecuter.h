#ifndef QUERYEXECUTER_H
#define QUERYEXECUTER_H

// QueryExecuterクラス

#include <mutex>
#include <log4cxx/logger.h>

#include "InformationSource.h"
#include "InformationSourceParser.h"
#include "InformationSourceManager.h"
#include "is/Schema.h"
#include "is/TupleSet.h"
#include "Operator.h"
#include "Parser.h"
#include "ErrorCode.h"
#include "NetworkSource.h"
#include "LocationManager.h"

using namespace std;
using namespace log4cxx;

namespace IS {

	/**
	* クエリの稼動状況情報
	*
	* @author	Nagoya University
	* @date	2019/10/29
	*/
	struct QueryStatus {
		unsigned int mngId;			// クエリID
		string query;				// クエリ文字列
		string user;				// クエリ登録ユーザ
		string operatorTreeXML;		// オペレーターツリーXML
		int operatorNum;			// 稼動オペレーター数
		double notifiedIntervalAVG;	// 処理開始間隔平均時間
		double totalProcessTimeAVG; // 総処理時間平均時間
		int unprocessedDataNum;		// 未処理データ数
		string latestOperatorName;	// 最遅オペレータ識別名
		double latestOpeProcAVG;	// 最遅オペレータ平均処理時間
		double inputStreamTupleNumPerSec;	// 秒間INPUTストリームタプル数
		double inputRelationTupleNumPerSec; // 秒間INPUTリレーションタプル数
		double outputTupleNumPerSec; // 秒間OUTPUTタプル数
	};

	/**
	 * クエリ実行処理
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	class QueryExecuter
	{
	private:
		LoggerPtr logger = Logger::getLogger("QueryExecuter");
		//IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		IS::Settings &settings = IS::Settings::get_instance();
		IS::InformationSourceManager &ism = IS::InformationSourceManager::get_instance();
		StringUtil stringUtil;

		string user;
		RecvData recvInfo;
		query_header query_header_;

		unsigned int mngId;
		unsigned int requestedMngId;
		bool isOneShot = false;
		vector<IS::Operator*> opeList;
		bool isFromCS = false;
		bool isExistsJoin = false;
		bool isExistsAggregateFunc = false;
		bool isExistsRequest = false;
		map<string, string> returnNodeIdMap;	// keyにRecursiceOperatorのNodeId, valueに戻り先のNodeId
		ErrorCode retErrCode = IS::ErrorCode::QUERY_PARSE_ERR;;
		string retErrMsg = "";
		string query = "";
		string parseResult = "";
		string mngKey = "";
		pthread_mutex_t opListMtx;
		string rdbXmlPre = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\"\n  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n  xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns \n    http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">\n<key id=\"nid\" attr.name=\"nid\" attr.type=\"string\" for=\"node\" />\n<key id=\"operator_type\" attr.name=\"operator_type\" attr.type=\"string\" for=\"node\" />\n<key id=\"parameter\" attr.name=\"parameter\" attr.type=\"string\" for=\"node\" />\n<graph id=\"g0\" edgedefault=\"undirected\" >\n<node id=\"n0\" ><data key=\"master\"></data>\n<data key=\"nid\">n0</data>\n<data key=\"operator_type\">out</data>\n<data key=\"parameter\"></data>\n</node>\n<node id=\"n1\" ><data key=\"master\"></data>\n<data key=\"nid\">n1</data>\n<data key=\"operator_type\">RDB</data>\n<data key=\"parameter\">";

		string rdbXmlSur = "</data>\n</node>\n<edge id=\"e0\" source=\"n1\" target=\"n0\" ></edge>\n</graph>\n</graphml>";
		bool doReturn = false;
		void run();
		void relationingOperator(IS::Operator* from, IS::Operator* to);
		void startOperator();
		void returnMngID(unsigned int mngId);
		void outputXmlLog(DOMDocument* doc);
		double getNotifiedIntervalAVG();
		double getTotalProcessTimeAVG();
		void addOperator(Operator* ope);
		bool runForNearBy(int range, unsigned long long senderId, unsigned long long destId, unsigned long long requestVehicleId, unsigned long long receptionEdgeId, unsigned int receptionMngId, string edgeSIDsStr);
		IS::Operator* parseOperatorTree(const string &operatorTreeXml);
		Operator* getOperator(DOMNodeList* dataList, DOMNodeList* graphList, const string &nodeId, bool isDynamicColumn);

	public:
		QueryExecuter(const string &user, const unsigned int mngId, const RecvData &data, const bool &returnMngId, const query_header &_query_header);
		QueryExecuter(const string &user, const unsigned int mngId, const RecvData &data);
		QueryExecuter(const string &user, const unsigned int mngId, const string query);

		bool isOperatorTreeXML = false;
		bool isExitRun = false;

		virtual ~QueryExecuter();
		void returnError(const ErrorCode &code, const string &msg);
		void exitExecuter();
		void registerCancelId(const string &query);
		/*
		void settingCancellationRange(const int senderId);
		void addRequest(const unsigned int destSID, PosMng::MngInfo mngInfo, const string &edgeSIDListStr);
		void cancelRequest(const unsigned int destSID, PosMng::MngInfo mngInfo);
		void removeRequestOp(const unsigned int destSID);
		*/
		void settingCancellationRange(const unsigned long long senderId);
		void addRequest(const unsigned long long destSID, PosMng::MngInfo mngInfo, const string &edgeSIDListStr);
		void cancelRequest(const unsigned long long destSID, PosMng::MngInfo mngInfo);
		void removeRequestOp(const unsigned long long destSID);
		bool isOneShotQuery();
		bool checkUser(const string &user);
		
		QueryStatus getQueryStatus();

		void init();       // impl
		void start();      // impl
		void stop();       // impl

	};
}
#endif  // QUERYEXECUTER_H
