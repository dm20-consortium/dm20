#ifndef QUERYRECEIVER_H
#define QUERYRECEIVER_H

#include <mutex>
#include <thread>
#include <log4cxx/logger.h>

#include "NetworkSource.h"
#include "InformationSourceParser.h"
#include "is/Schema.h"
#include "is/TupleSet.h"
#include "QueryManager.h"
#include "ProcessingExecuter.h"
#include "SessionManager.h"
#include "TransferOperator.h"


using namespace std;
using namespace log4cxx;

#define INIT_TCPPORT 51150
#define INIT_SSLPORT 52150

namespace IS {

	/**
	 * クエリの受信処理を扱うクラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	class QueryReceiver : public NetworkSource
	{
	private:
		LoggerPtr logger = Logger::getLogger("QueryReceiver");
		//IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		IS::QueryManager &QM = IS::QueryManager::get_instance();
		IS::SessionManager &SM = IS::SessionManager::get_instance();
		IS::Settings &settings = IS::Settings::get_instance();
		deque<Tuple> queue;
		int windowsize;

		std::condition_variable cond_queue;
		std::mutex mtx;
		bool tuple_ready = false;
		void returnMngID(unsigned int mngId, const RecvData &data, const int &tcpPort);
		void transferQuery(const unsigned long long execSID, RecvData &data, string &query);

		// 管理番号
		int TCPPORT = 0;
		pthread_mutex_t tcpPortMtx;
		SSL_CTX *CTX;
	public:
		QueryReceiver(string name, int port);
		virtual ~QueryReceiver();

		void init();       // impl
		void start();      // impl
		void start(bool isSSL);      // impl
		void stop();       // impl
		virtual void receiveProcess(RecvData &data); // impl
	};
}
#endif  // QUERYRECEIVER_H
