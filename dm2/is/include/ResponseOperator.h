#ifndef RESPONCEOPERATOR_H
#define RESPONCEOPERATOR_H

#include "Operator.h"
#include "is/StringUtil.h"
#include "QueryManager.h"
#include "ErrorCode.h"
#include "NetworkSource.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <log4cxx/logger.h>

using namespace log4cxx;

namespace IS {

	/**
	 * クライアントからの要求に対しての応答処理を扱うオペレータ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	class ResponseOperator : public Operator
	{
	private:
		const string MyName = "Response";
		LoggerPtr logger = Logger::getLogger("ResponseOperator");
		StringUtil stringUtil;
		//IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		IS::Settings &settings = IS::Settings::get_instance();
		int sock = 0;
		struct sockaddr_in addr;
		SSL *ssl = NULL;
		SSL *sslForRegisterQuery = NULL;
		SSL_CTX *ctx = NULL;
		string user;
		int udpPort = 0;
		int tcpPort = 0;
		string protocol = "";
		bool isTCP = false;
		bool exit_flag = false;
		ErrorCode code = IS::ErrorCode::NO_ERR;
		string msg;
		// レスポンスなし時間
		double totalProcessNoResTimeAVG = 0;
		double totalProcessNoResTimeSlowest = 0;
		double totalProcessNoResTimeEarliest = 0;
		// レスポンスあり時間
		unsigned int notifiedNum = 0;
		double totalProcessTimeAVG = 0;
		double totalProcessTimeSlowest = 0;
		double totalProcessTimeEarliest = 0;

		std::queue<TupleSet> buffer;
		std::condition_variable cond;
		std::mutex mtx;
		bool replyForConQueryByTcp = false;
		int TcpSendSize = 1280;
		bool send_err_flag = false;

		void hideColumn(TupleSet &tupleset);
		void terminate(unsigned int mngId);
		bool isSslShutdown(SSL *ssl);
	public:
		// UDP用コンストラクタ
		ResponseOperator(const string &user, unsigned int mngId, const RecvData &data, int port);
		// TCP用コンストラクタ
		ResponseOperator(const RecvData &data);
		// TCP用コンストラクタ(管理番号あり)
		ResponseOperator(unsigned int mngId, const RecvData &data);
		// TCP用コンストラクタ(管理番号あり)
		ResponseOperator(unsigned int mngId, const RecvData &data, const int &port);
		// TCP用コンストラクタ(エラー返却)
		ResponseOperator(unsigned int mngId, const RecvData &data, IS::ErrorCode code, const string &msg);
		virtual ~ResponseOperator();

		double getTotalProcessTimeAVG();
		bool setDTLSsocket();
		// 処理
		virtual bool process(vector<IS::TupleSet>& ts);
		virtual void process_close();

		int TCPSend(const string &body);
		void checkTerminate();
	};
}
#endif  // RESPONCEOPERATOR_H
