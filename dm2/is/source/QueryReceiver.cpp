#include "QueryReceiver.h"

using namespace std;

namespace IS {

	static const int BUF_MAX = 4096;

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	name	ネットワーク名称
	 * @param	port	ポート番号
	 */

	QueryReceiver::QueryReceiver(string name, int port)
		: NetworkSource(name, port)
	{
		// QueryManagerの起動
		QM.init();
		pthread_mutex_init(&tcpPortMtx, NULL);
	}

	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	QueryReceiver::~QueryReceiver()
	{
	}

	/**
	 * 初期化処理
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void QueryReceiver::init()
	{
	}

	/**
	* 受信スレッドの開始
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*/

	void QueryReceiver::start()
	{
		start(false);
	}

	/**
	 * 受信スレッドの開始(暗号化可否指定)
	 *
	 * @author	Nagoya University
	 * @date	2019/07/31
	 *
	 * @param	isSSL	暗号化実施可否
	 */

	void QueryReceiver::start(bool isSSL)
	{
		if (!isSSL) {
			thread queuingThread(&QueryReceiver::receiveTCPdata, this);
			queuingThread.detach();
		}
		else {
			CTX = setupSSL();
			thread queuingThread(&QueryReceiver::receiveSSLdata, this, CTX);
			queuingThread.detach();
		}
	}

	/**
	 * 受信処理の終了
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void QueryReceiver::stop()
	{
		logger->debug("[QueryReceiver] exit_flag ON");
		pthread_mutex_destroy(&tcpPortMtx);
		exit_flag = true;
	}

	/**
	 * クエリ管理番号の返却処理
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	mngId	クエリ管理番号
	 */

	inline void QueryReceiver::returnMngID(unsigned int mngId, const RecvData &data, const int &tcpPort)
	{
		TupleSet tupleset;
		vector<TupleSet> ts;
		ts.push_back(tupleset);

		logger->debug("[returnMngID] IN Use ResponseOperator. MyMngId:" + std::to_string(mngId));
		IS::ResponseOperator		*opX = new IS::ResponseOperator(mngId, data, tcpPort);
		opX->process(ts);
		opX->exit();
		delete opX;
	}
	
	/**
	 * クエリ転送処理
	 *
	 * @author	Shinichi Kusayama
	 * @date	2025/9/22
	 *
	 * @param	execSID	実行元SID
	 * @param	payload	クエリ
	 */
	void QueryReceiver::transferQuery(const unsigned long long execSID, RecvData &data, string &query)
	{
		string resMsg;
		ErrorCode resCode = ErrorCode::NO_ERR;
		string errMsg = QM.parseQuery(query);
		if (errMsg != "") {
			resMsg = errMsg;
			resCode = ErrorCode::QUERY_PARSE_ERR;
		} else {
			// クエリ転送処理
			IS::TransferOperator *opX = new IS::TransferOperator(execSID, 0, 0);
			opX->process(data.payload);
			opX->exit();
			delete opX;
		}
		// 処理結果を応答
		TupleSet tupleset;
		vector<TupleSet> ts;
		ts.push_back(tupleset);
		IS::ResponseOperator *opX = new IS::ResponseOperator(0, data, resCode, resMsg);
		opX->process(ts);
		opX->exit();
		delete opX;

	}
	/**
	* 受信処理
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param	data  	受信データ
	*/

	void QueryReceiver::receiveProcess(RecvData &data)
	{
#if MEASURE_MODE == 1
		long procTime = DmUtil::getTimeMicrosec();
#endif
		//cout << "[" << data.payload << "]" << endl;
		if (data.isClose) {
			// 継続クエリのキャンセル要求
			QM.cancelQuery(data);
			return;
		}
		if (data.payload == "") return;
		string user, key;
		IS::ProtobufParser &pp = IS::ProtobufParser::get_instance();
		pp.init();

		// Protobufの接続要求、クエリ登録、キャンセル
		dm2_proto::System_request request = pp.systemRequestDeserialize(data.payload);
		query_header query_header_;
		query_header_.dstSID = request.destination();
		query_header_.port = request.port();
		query_header_.continuous = request.continuous();
		query_header_.dmiName = "";
		// 接続要求
		if ("session" == request.type()) {
			user = request.body();
			key = request.key();
			string pid = to_string(request.pid());
			SM.createSession(data, user, key, pid);
		}
		else if ("query" == request.type() || "cancel" == request.type()) {
			key = request.key();
			// セッションの確認
			if (!SM.checkSession(key, user)) {
				pp.finalize();
				return;
			}

			if ("query" == request.type()) {
				// クエリ文で上書き
				data.payload = request.body();
				if (request.tcp_session())
				{
					string errMsg = QM.parseQuery(data.payload);
					if (errMsg != "") {
						TupleSet tupleset;
						vector<TupleSet> ts;
						ts.push_back(tupleset);
						IS::ResponseOperator *opX = new IS::ResponseOperator(0, data, ErrorCode::QUERY_PARSE_ERR, errMsg);
						opX->process(ts);
						opX->exit();
						delete opX;
					} else {
						// クエリの登録
						int mngId = QM.getMngId();
						// 管理番号を発行
						pthread_mutex_lock(&tcpPortMtx);
						if (data.ssl == NULL) {
							if (TCPPORT == 0 || TCPPORT == INIT_TCPPORT + 999) TCPPORT = INIT_TCPPORT;
						} else {
							if (TCPPORT == 0 || TCPPORT == INIT_SSLPORT + 999) TCPPORT = INIT_SSLPORT;
						}
						int port = ++TCPPORT;
						pthread_mutex_unlock(&tcpPortMtx);
						logger->debug("tcp_session port:" + to_string(port));
						returnMngID(mngId, data, port);
						int dataSock;
						SSL *dataSsl;
						if (data.ssl == NULL) {
							dataSock = getTCPSocket(port);
						} else {
							dataSsl = SSL_new(this->CTX);
							dataSock = getSSLSocket(port, this->CTX, dataSsl);
							data.ssl2 = dataSsl;
						}
						logger->debug("tcp_session dataSock:" + to_string(dataSock));
						data.sock2 = dataSock;
						QM.addQuery(mngId, user, data, false, query_header_);
						if (data.ssl == NULL) {
							thread checkThread(&NetworkSource::checkTCPSocket, this, dataSock);
							checkThread.detach();
						} else {
							thread checkThread(&NetworkSource::checkSSLSocket, this, dataSock, dataSsl);
							checkThread.detach();
						}
					}
				} else {
					if (query_header_.dstSID == 0) {
						// クエリの登録
						int mngId = QM.getMngId();
						QM.addQuery(mngId, user, data, true, query_header_);
					} else {
						string ip = inet_ntoa(data.client.sin_addr);
						if (ip == "127.0.0.1") {
							QM.addQuery("public", data, query_header_);
						} else {
							QM.addQuery(user, data, query_header_);
						}
					}
				}
			} else if ("cancel" == request.type()) {
				unsigned int mngId = stoi(request.body());
				// 継続クエリのキャンセル要求
				QM.cancelQuery(user, data, mngId);
			}
		} else {
			string rootTagName;
			IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
			isp.init();
			// XMLの種別を取得
			if (!isp.getRootTagNameAndKey(data.payload, rootTagName, key)) {
				isp.finalize();
				pp.finalize();
				return;
			}

			if ("info" == rootTagName) {
				// 統計情報の取得要求
				string funcName = isp.getAttrValueStrFromRootTag("func", data.payload);
				ProcessingExecuter *PE = new ProcessingExecuter(data, funcName);
				PE->start();
			}
			else if ("operatorTree" == rootTagName) {
				// オペレーターツリーの登録
				QM.addOperatorTree(user, data);
			}
			else
			{
				logger->error("[ERROR] Not Define RootTagName. : " + rootTagName);
			}
			isp.finalize();
		}
		pp.finalize();
	}
}
