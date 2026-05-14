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
		IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		isp.init();
		// XMLの種別を取得
		string rootTagName, key, user;
		if (!isp.getRootTagNameAndKey(data.payload, rootTagName, key)) {
			isp.finalize();
			return;
		}
#if MEASURE_MODE == 1
		long now = DmUtil::getTimeMicrosec();
		double msec = (now - procTime) / 1000.0;
		logger->info("[receiveProcess] getAttrValueFromRootTag processTime(msec):" + to_string(msec));
#endif
		if ("create_session" == rootTagName) {
			// セッションの生成
			string hash, pid;
			isp.getUserAuthInfo(data.payload, user, hash, pid);
			SM.createSession(data, user, hash, pid);
		}
		else {
			// セッションの確認
			if (!SM.checkSession(key, user)) {
				isp.finalize();
				return;
			}

			if ("info" == rootTagName) {
				// 統計情報の取得要求
				string funcName = isp.getAttrValueStrFromRootTag("func", data.payload);
				ProcessingExecuter *PE = new ProcessingExecuter(data, funcName);
				PE->start();
			}
			else if ("query" == rootTagName) {
				string tcp_session = isp.getStringByTagName(data.payload, "tcp_session");
				logger->debug("tcp_session:" + tcp_session);

				if (tcp_session == "1" || tcp_session == "true") {
					string query;
					isp.getQueryString(data.payload, query);
					string errMsg = QM.parseQuery(query);
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
						QM.addQuery(mngId, user, data, false);
						if (data.ssl == NULL) {
							thread checkThread(&NetworkSource::checkTCPSocket, this, dataSock);
							checkThread.detach();
						} else {
							thread checkThread(&NetworkSource::checkSSLSocket, this, dataSock, dataSsl);
							checkThread.detach();
						}
					}
				} else {
					unsigned long long dstSID = isp.getDestinationSID(data.payload);
					unsigned long long execSID = isp.getSenderSID(data.payload);
					string mySid = settings.getParameter("MY_STATION_ID");
					if (execSID != 0 && execSID != stoull(mySid)) {
						string query;
						isp.getQueryString(data.payload, query);
						transferQuery(execSID, data, query);
					} else {
						if (dstSID == 0) {
							// クエリの登録
							int mngId = QM.getMngId();
							QM.addQuery(mngId, user, data, true);
						} else {
							string ip = inet_ntoa(data.client.sin_addr);
							if (ip == "127.0.0.1") {
								QM.addQuery("public", data);
							} else {
								QM.addQuery(user, data);
							}
						}
					}
				}
			}
			else if ("cancel" == rootTagName) {
				// 継続クエリのキャンセル要求
				QM.cancelQuery(user, data, isp.getAttrValueFromRootTag("id", data.payload));
			}
			else if ("operatorTree" == rootTagName) {
				// オペレーターツリーの登録
				QM.addOperatorTree(user, data);
			}
			else if ("change_id" == rootTagName) {
				string message;
				isp.getQueryString(data.payload, message);
				QM.changeid(message);
			}
			else
			{
				logger->error("[ERROR] Not Define RootTagName. : " + rootTagName);
			}
		}
		isp.finalize();
	}
}
