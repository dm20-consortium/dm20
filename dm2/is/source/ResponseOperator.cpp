#include "ResponseOperator.h"
#include "InformationSourceParser.h"
#include "QueueManager.h"
#include "Settings.h"
using std::cout;		// cout
using std::endl;		// endl
using namespace std;

namespace IS {

	/**
	 * コンストラクタ（TCPによる応答)
	 * 主にOne-Shot Queryや継続クエリ登録時のクエリ管理番号返却時に使用
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	mngId	クエリ管理番号
	 * @param	data	受信データ
	 */

	ResponseOperator::ResponseOperator(unsigned int mngId, const RecvData &data)
	{
		this->type = MyName + "_MNGID:" + std::to_string(mngId);
		this->sock = data.sock;
		this->addr = data.client;
		this->ssl = data.ssl;
		if (this->ssl == NULL) {
			protocol = "TCP";
		}
		else {
			protocol = "SSL";
		}
		this->mngId = mngId;
		isTCP = true;
		expect_data_size = 1;
		this->argument.append("PROTOCOL:TCP ");
		this->compressFlg = settings.getParameter("COMPRESS_FLG")[0];
	}

	/**
	 * コンストラクタ（TCPによる応答)
	 * 主にOne-Shot Queryや継続クエリ登録時のクエリ管理番号返却時に使用
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	mngId	クエリ管理番号
	 * @param	data	受信データ
	 */

	ResponseOperator::ResponseOperator(unsigned int mngId, const RecvData &data, const int &tcpPort)
	{
		this->type = MyName + "_MNGID:" + std::to_string(mngId);
		this->sock = data.sock;
		this->addr = data.client;
		this->ssl = data.ssl;
		if (this->ssl == NULL) {
			protocol = "TCP";
		}
		else {
			protocol = "SSL";
		}
		this->mngId = mngId;
		this->code = code;
		this->msg = msg;
		isTCP = true;
		expect_data_size = 1;
		this->argument.append("PROTOCOL:" + protocol);
		this->argument.append(" ERROR_CODE:" + std::to_string((int)code));
		this->tcpPort = tcpPort;
		this->compressFlg = settings.getParameter("COMPRESS_FLG")[0];
	}

	/**
	 * コンストラクタ（TCPによるエラー情報の返却）
	 * クエリ構文エラーなどDBシステム内でエラーを検知した際にその情報をクライアントに返却する際に使用
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	mngId	クエリ管理番号
	 * @param	data	受信データ
	 * @param	code 	エラーコード
	 * @param	msg 	エラーメッセージ
	 */

	ResponseOperator::ResponseOperator(unsigned int mngId, const RecvData &data, IS::ErrorCode code, const string &msg)
	{
		this->type = MyName + "_MNGID:" + std::to_string(mngId);
		this->sock = data.sock;
		this->addr = data.client;
		this->ssl = data.ssl;
		if (this->ssl == NULL) {
			protocol = "TCP";
		}
		else {
			protocol = "SSL";
		}
		this->mngId = mngId;
		this->code = code;
		this->msg = msg;
		isTCP = true;
		expect_data_size = 1;
		this->argument.append("PROTOCOL:" + protocol);
		this->argument.append(" ERROR_CODE:" + std::to_string((int)code));
		this->compressFlg = settings.getParameter("COMPRESS_FLG")[0];
	}

	/**
	 * コンストラクタ（UDPによる通知）
	 * 主に継続クエリの結果通知に使用
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	user	ユーザ名
	 * @param	mngId	クエリ管理番号
	 * @param	data	受信データ
	 * @param	port 	通知先ポート
	 */

	ResponseOperator::ResponseOperator(const string &user, unsigned int mngId, const RecvData &data, int port, bool isDynamicMessage)
	{
		this->type = MyName + "_MNGID:" + std::to_string(mngId);
		this->user = user;
		this->addr = data.client;
		this->mngId = mngId;
		this->udpPort = port;
		this->isDynamicColumn = isDynamicMessage;
		isTCP = false;
		expect_data_size = 1;
		if (data.sock2 != 0) {
			replyForConQueryByTcp = true;
		}
		if (replyForConQueryByTcp) {
			this->sock = data.sock2;
			this->ssl = data.ssl2;
			this->sslForRegisterQuery = data.ssl;
			if (this->ssl != NULL) {
				protocol = "SSL";
			} else {
				protocol = "TCP";
			}
		} else {
			if (data.ssl != NULL) {
				setDTLSsocket();
			}
			else {
				protocol = "UDP";
			}
		}

		this->argument.append("PROTOCOL:" + protocol);
		this->compressFlg = settings.getParameter("COMPRESS_FLG")[0];
		
	}

	/**
	* コンストラクタ（TCPによる応答)
	* 電文を指定する場合に使用
	*
	* @author	Nagoya University
	* @date	2018/10/29
	*
	* @param	data	受信データ
	*/

	ResponseOperator::ResponseOperator(const RecvData &data)
	{
		this->type = MyName;
		this->sock = data.sock;
		this->addr = data.client;
		this->ssl = data.ssl;
		if (this->ssl == NULL) {
			protocol = "TCP";
		}
		else {
			protocol = "SSL";
		}
		isTCP = true;
		expect_data_size = 1;
		exitReady = true;	// executeを通らないため常に終了可能とする
		this->argument.append("PROTOCOL:" + protocol);
		this->compressFlg = settings.getParameter("COMPRESS_FLG")[0];
	}

	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	ResponseOperator::~ResponseOperator()
	{
		if (processNum != 0) {
			logger->debug("[PERFORMANCE_STAT][" + getType() + "] TotalProcAvgTime(NoRes) :" + to_string(totalProcessNoResTimeAVG) + "[ms] Fastest:" + to_string(totalProcessNoResTimeEarliest) + "[ms] Slowest:" + to_string(totalProcessNoResTimeSlowest) + "[ms] noResponseNum:" + to_string(processNum- notifiedNum));
			logger->debug("[PERFORMANCE_STAT][" + getType() + "] TotalProcAvgTime(Notify):" + to_string(totalProcessTimeAVG) + "[ms] Fastest:" + to_string(totalProcessTimeEarliest) + "[ms] Slowest:" + to_string(totalProcessTimeSlowest) + "[ms] notifiedNum:" + to_string(notifiedNum));
		}
	}
	bool ResponseOperator::setDTLSsocket()
	{
		struct sockaddr_in udpAddr;
		BIO *bio;
		struct timeval timeout;
		protocol = "DTLS";

		this->ctx = SSL_CTX_new(DTLSv1_2_client_method());
		this->ssl = SSL_new(ctx);

		sock = socket(AF_INET, SOCK_DGRAM, 0);
		udpAddr.sin_family = AF_INET;
		udpAddr.sin_port = htons(this->udpPort);
		udpAddr.sin_addr.s_addr = inet_addr(inet_ntoa(addr.sin_addr));

		bio = BIO_new_dgram(sock, BIO_CLOSE);
		connect(sock, (struct sockaddr *) &udpAddr, sizeof(udpAddr));
		BIO_ctrl(bio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &udpAddr);
		SSL_set_bio(ssl, bio, bio);

		if (SSL_connect(ssl) < 0) {
			return false;
		}

		// Set and activate timeouts
		timeout.tv_sec = stoi(settings.getParameter("DTLS_SOCK_TIMEOUT_SEC")) * stoi(settings.getParameter("DTLS_SOCK_TIMEOUT_CNT"));
		timeout.tv_usec = 0;
		BIO_ctrl(bio, BIO_CTRL_DGRAM_SET_RECV_TIMEOUT, 0, &timeout);
		return true;
	}
	void ResponseOperator::process_close()
	{
		if (this->ssl != NULL && replyForConQueryByTcp == false) {
			logger->warn("[ResponseOperator] SSL close");
			SSL_shutdown(ssl);
			close(sock);
			SSL_free(ssl);
			ERR_remove_state(0);
		}
	}
	/**
	* 処理前検査
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param [in]	tupleset	タプルセット
	*
	* @return	検査結果
	*/
	bool ResponseOperator::checkPreCondition(const TupleSet& tupleset)
	{
		// presetTimerのケース
		if (string(inet_ntoa(addr.sin_addr)) == "0.0.0.0") {
			logger->info("[" + this->type + "] ========== Response END ========== [Reason] send-IP: 0.0.0.0");
			return false;
		}
		if (send_err_flag) {
			logger->debug("[" + this->type + "] ========== Response END ========== [Reason] send_err_flag on");
			return false;
		}
		if (code == ErrorCode::NO_ERR) {

			// 返却するTupleが存在しないかつ、待機状態からnotify_one()により起動された場合、かつUDP使用時(継続クエリ)は返却しない
			if (tupleset.size() == 0 && isTCP == false) {
				logger->debug("[" + this->type + "] ========== Response END ========== [Reason] No tuple");
				return false;
			}
		}
		if (isTCP) {
			if (ssl != NULL) {
				if (isSslShutdown(ssl)) {
					logger->debug("[" + this->type + "] ========== Response END ========== [Reason] SSL is shutdown");
					return false;
				}
			}
		}
		return true;
	}
	/**
	* 時刻情報を追加付与
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param [in,out]	ts	タプルセット
	*
	*/
	void ResponseOperator::addTimestamp(TupleSet& tupleset) {
		if (!isTCP) {
			long recvTime = DmUtil::getTimeMicrosec();
			Schema schema = tupleset.getSchemaRef();
			string tablename = schema.getTableName();
			for (int idx = 0; idx < tupleset.size(); idx++) {
				any val;
				int tsGrpIdx = schema.getAttributeIdx(tablename + "." + Schema::COL_TS_GRP, false);
				if (tsGrpIdx != -1) {
					tupleset.getTuple(idx).getValueByIdx(tsGrpIdx, val);
					string strVal = stringUtil.getAnyString(val) + to_string(recvTime) + ",";
					tupleset.getTuple(idx).setValue(tsGrpIdx, strVal, 0, false);
				}
			}
		}
		return;
	}
	/**
	* 電文生成
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param [in]	tupleset	タプルセット
	* @param [out]	retProto		電文（TCP応答、エラー時）
	* @param [out]	retProtoList	電文（UDP応答）
	*
	* @return	正常に生成できた場合、true
	*/
	bool ResponseOperator::createResponse(TupleSet& tupleset, string& retProto, vector<string>& retProtoList)
	{
		IS::ProtobufParser &pp = IS::ProtobufParser::get_instance();
		//cout << "code:" << code << ", type:" << currentResponseType << endl;
		pp.init();
		if (code == ErrorCode::NO_ERR) {
			if (isTCP) {
				// 返信種別を判定
				switch (currentResponseType)
				{
					// クエリ登録応答
					case responseType::RESPONSE_QUERY:
						// クエリ管理番号の返送
						retProto = pp.createQueryResponse(mngId, tcpPort);
						break;
					// クエリキャンセル応答
					case responseType::RESPONSE_CANCEL:
						// クエリキャンセルの応答
						retProto = pp.createCancelResponse(mngId);
						break;
					// 継続クエリ結果送信
					case responseType::QUERY_RESULT:
						// クエリ結果をprotobufに変換
						retProto = pp.createQueryResult(mngId, this->protobufMessageName, tupleset, this->isDynamicColumn);
						break;
				}
			} else {
				// 管理者カラム制御
				hideColumn(tupleset);
				// for (int idx = 0; idx < tupleset.size(); idx++) tupleset.getTuple(idx).dump();
				// クエリ結果をprotobufに変換 + 指定バイト長で分割
				retProtoList = pp.createQueryResult(mngId, this->protobufMessageName, tupleset, this->isDynamicColumn, IPv4_UDP_MAX_BYTE);
			}
		} else {
			// 返信種別を判定
			switch (currentResponseType)
			{
				// クエリキャンセル応答
				case responseType::RESPONSE_CANCEL:
					// エラー時のクエリキャンセルレスポンスを生成
					retProto = pp.createCancelErrorResponse(to_string(static_cast<int>(code)), msg);
					break;
				// 継続クエリ結果送信
				case responseType::QUERY_RESULT:
					// エラー時のクエリレスポンスを生成
					retProto = pp.createQueryErrorResponse(to_string(static_cast<int>(code)), msg);
					break;
				case responseType::RESPONSE_QUERY:
					break;
			}
		}
		pp.finalize();
		return true;
	}
	/**
	* SSL_writeの結果チェック
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param [in]	ret_arg	SSL_writeの結果
	*
	* @return	正常に送信・あるいはリセット成功時、true
	*/
	bool ResponseOperator::checkSSLReturn(const int ret_arg)
	{
		bool rtn = false;
		string errMsg = "";
		switch (SSL_get_error(ssl, ret_arg)) {
		case SSL_ERROR_NONE:
			rtn = true;
			break;
		case SSL_ERROR_WANT_WRITE:
			errMsg = "ERROR_WANT_WRITE";
			break;
		case SSL_ERROR_SSL:
			errMsg = "ERROR_SSL";
			break;
		case SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER:
			errMsg = "SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER";
			break;
		case SSL_ERROR_SYSCALL:
			process_close();
			if(setDTLSsocket()) {
				errMsg = "Reset DTLS Socket and Continue";
				rtn = true;
			} else {
				errMsg = "ERROR_SYSCALL";
				process_close();
			}
			break;
		default:
			errMsg = "ERROR_OTHER";
			break;
		}
		if (isSslShutdown(ssl)) {
			errMsg = "SSL IS SHUTDOWN";
		}
		logger->warn("[ResponseOperator]" + errMsg);
		return rtn;
	}
	/**
	* ストリーム応答処理
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param [in]	tupleset	タプルセット
	* @param [out]	retProto		電文（TCP応答、エラー時）
	* @param [out]	retProtoList	電文（UDP応答）
	*
	* @return	正常に生成できた場合、true
	*/
	void ResponseOperator::sendStreamResponse(const vector<string>& retProtoList)
	{
		struct sockaddr_in udpAddr;
		int udpSock = -1;
		int ret = 0;
		unsigned int sendSumLen = 0;
		
		if (replyForConQueryByTcp) {
			// ストリームデータをTCPで返すケース
			for (unsigned int i = 0; i < retProtoList.size(); i++) {
				Bytef outbuf[this->TcpSendSize];
				logger->debug("[ResponseOperator]send_sock:" + to_string(this->sock));
				bool doCompress = stringUtil.compress(retProtoList.at(i), outbuf);
				if (doCompress) {
					if (ssl == NULL) {
						ret = send(this->sock, outbuf, this->TcpSendSize, 0);
					} else {
						if (isSslShutdown(ssl)) break;
						ret = SSL_write(ssl, outbuf, this->TcpSendSize);
					}
				} else {
					ret = -1;
				}
				if (ret > 0) {
					sendSumLen = sendSumLen + ret;
				} else {
					string errMsg = string(std::strerror(errno));
					string errIp = inet_ntoa(addr.sin_addr);
					logger->error("[" + this->type + "] Failed to send TCP errmsg: " + errMsg + ",ip:" + errIp);
					//コネクションが切れた後に、再送しようとすると、プロセスが終了するため、キャンセルされるまでprocess関数内では何もしない。
					send_err_flag = true;
					break;
				}
			}
		} else {
			// ストリームデータをUDPで返すケース
			if (ssl == NULL) {
				udpSock = socket(AF_INET, SOCK_DGRAM, 0);
				udpAddr.sin_family = AF_INET;
				udpAddr.sin_port = htons(this->udpPort);
				udpAddr.sin_addr.s_addr = inet_addr(inet_ntoa(addr.sin_addr));
			}
			for (unsigned int i = 0; i < retProtoList.size(); i++) {
				//cout << retProtoList.at(i) << endl;
				vector<char> sendBuf;
				const char* sendPtr;
				size_t sendSize;
				if (compressFlg == '1' || compressFlg == '2') {
					long key = DmUtil::getTimeMicrosec();
					sendBuf = stringUtil.setCompressedBufWithHeader(retProtoList.at(i), compressFlg, key);
					sendPtr = sendBuf.data();
					sendSize = sendBuf.size();
				}
				if (sendBuf.empty()) {
					if (compressFlg == '1' || compressFlg == '2') logger->warn("[" + this->type + "] Compress-Proc is Failed. Send to No-Compress-Data");
					const string& s = retProtoList.at(i);
					sendPtr = s.data();
					sendSize = s.size();
				}
				if (ssl == NULL) {
					ret = sendto(udpSock, sendPtr, sendSize, 0, (struct sockaddr *)&udpAddr, sizeof(udpAddr));
				} else {
					if (isSslShutdown(ssl)) break;
					ret = SSL_write(ssl, sendPtr, sendSize);
				}
				logger->debug("[ResponseOperator] sendSize:" + to_string(sendSize) + " ,compressFlg:" + compressFlg);
				if (ret > 0) {
					sendSumLen = sendSumLen + ret;
				} else {
					string errMsg = string(std::strerror(errno));
					string errIp = inet_ntoa(udpAddr.sin_addr);
					logger->error("[" + this->type + "] Failed to sendto. retry.... split.length:" + std::to_string(retProtoList.at(i).length()));
					logger->error("[" + this->type + "] Failed to send UDP errmsg: " + errMsg + ",ip:" + errIp);
					//コネクションが切れた後に、再送しようとすると、プロセスが終了するため、キャンセルされるまでprocess関数内では何もしない。
					send_err_flag = true;
					bool checkSSL = checkSSLReturn(ret);
					if (!checkSSL)  break;
				}
			}
		}
		ret = sendSumLen;

		if (ret > 0) {
			logger->debug("[" + this->type + "] Response by " + protocol + ". MNGID:" + std::to_string(mngId) + " sendto : " + string(inet_ntoa(addr.sin_addr)) + ":" + std::to_string(this->udpPort) + " Size:" + std::to_string(ret) + " byte");
		}
		else {
			//logger->debug("[" + this->type + "] MNGID:" + std::to_string(mngId) + " sendto error: " + string(inet_ntoa(addr.sin_addr)) + ":" + std::to_string(this->udpPort) + " XMLListSize : " + std::to_string(retProtoList.length()) + " byte");
			//logger->debug("[" + this->type + "] Failed to send. errmsg: " + string(std::strerror(errno)));
		}
		if (!replyForConQueryByTcp) {
			close(udpSock);
		}

	}
	/**
	* オペレータ処理
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param [in,out]	ts	タプルセット
	*
	* @return	正常にデータ処理を実施でき、次のオペレータに渡す際にtrue
	*/
	bool ResponseOperator::process(vector<IS::TupleSet>& ts)
	{
		logger->debug("[" + this->type + "] ========== Response START ========== sock:" + to_string(this->sock) + ",isTCP:" + to_string(isTCP) + ",tsize:" + to_string(ts.at(0).size()));

		// Selectionは1つのtuplesetを使用
		TupleSet& tupleset = ts.at(0);
		// 処理前検査
		if (!checkPreCondition(tupleset)) return true;
		// 時刻付与
		addTimestamp(tupleset);

		// DEBUG 与えられたタプル情報の出力
		printInputInfo(tupleset, this->argument);
		
		string retProto = "";
		vector<string> retProtoList;
		// 電文生成
		if (!createResponse(tupleset, retProto, retProtoList)) {
			return false;
		}
		if (isTCP) {
			// システム応答電文を送信
			sendSystemResponse(retProto);
			if (tupleset.size() == 0 && (Operator::isDataReady() == false)) {
				// 継続クエリの管理番号返却時、キャンセル要求時はexecuteを通らないため終了可能
				exitReady = true;
			}
		}
		else {
			// ストリーム応答電文を送信
			sendStreamResponse(retProtoList);
		}

		logger->debug(" ========== Response  END  ========== ");
		return true;
	}

	/**
	* システム応答電文をTCPで指定した文字列を送信する
	*
	* @author	Nagoya University
	* @date	2018/10/29
	*
	* @param [in]	body	送信文字列
	*
	* @return	送信byte
	*/
	int ResponseOperator::sendSystemResponse(const string &body)
	{
		logger->debug(body);
		// TCPにてレスポンス返却
		int ret;
		if (ssl == NULL) {
			ret = sendto(sock, body.c_str(), body.length(), 0, (struct sockaddr*)&addr, sizeof(addr));
		}
		else {
			ret = SSL_write(ssl, body.c_str(), body.length());
		}

		logger->debug("[" + this->type + "] Response by " + protocol + ". sendto : " + string(inet_ntoa(addr.sin_addr)) + " Size:" + std::to_string(ret) + " byte");
		return ret;
	}

	/**
	* 終了判定を実施する
	*
	* @author	Nagoya University
	* @date	2019/8/21
	*
	*/
	void ResponseOperator::checkTerminate() {
		// 各種オペレータを終了させる
		if (isTCP && (Operator::isDataReady() == true || code != ErrorCode::NO_ERR) ) {
			// 待機状態からnotify_one()により起動された場合にのみキャンセル処理を実施する(TCP)
			// (継続クエリ登録時の管理番号返却時はisDataReadyがfalseの状態）
			thread th(&ResponseOperator::terminate, this, mngId);
			th.detach();
		}
	}

	/**
	* クエリのキャンセル要求を実施する
	* 自身でCancelすると要求元がdeleteされるため別スレッドに移譲する
	*
	* @author	Nagoya University
	* @date	2019/12/04
	*
	*/
	void ResponseOperator::terminate(unsigned int mngId) {
		// presetTimer時はキャンセルしない
		if (string(inet_ntoa(addr.sin_addr)) == "0.0.0.0") return;
		IS::QueryManager &QM = IS::QueryManager::get_instance();
		QM.cancelQuery(mngId);
	}

	/**
	* 隠し属性のカラムをユーザに応じて削除する
	*
	* @author	Nagoya University
	* @date	2019/9/9
	*
	* @param [in,out]	tupleset	タプルセット
	*/
	void ResponseOperator::hideColumn(TupleSet &tupleset) {
		IS::QueueManager &QM = IS::QueueManager::get_instance();

		vector<string> deleteColNameList;
		// 参照権限の取得
		for (int i = 0; i < tupleset.getSchemaRef().getAttributeSize(); i++) {
			if (tupleset.getSchemaRef().getAttributeHidden(i) == true && QM.isRefAdminCol(tupleset.getSchemaRef().getAttributeOrgTblName(i), this->user) == false) {
				deleteColNameList.push_back(tupleset.getSchemaRef().getAttributeName(i));
			}
		}
		
		for (unsigned int i = 0; i < deleteColNameList.size(); i++) {
			tupleset.removeColumn(deleteColNameList.at(i));
			logger->debug("[hideColumn] delete column:" + deleteColNameList.at(i) + ". User '" + this->user + "' has no permission to read admin column(s).");
		}
	}

	/**
	* 総処理平均時間を取得する
	*
	* @author	Nagoya University
	* @date	2019/10/29
	*
	* @return 総処理平均時間
	*/
	double ResponseOperator::getTotalProcessTimeAVG()
	{
		return totalProcessTimeAVG;
	}
	/**
	* SSLがシャットダウン状態にあるか判定
	*
	* @author	Shinichi Kusayama
	* @date	2022/05/09
	*
	* @param	SSL	SSLセッション
	*
	* @return	シャットダウン状態にある場合はtrue
	*/
	bool ResponseOperator::isSslShutdown(SSL *ssl)
	{
		int sr = SSL_get_shutdown(ssl);
		if (sr == SSL_RECEIVED_SHUTDOWN || sr == SSL_SENT_SHUTDOWN) {
			return true;
		}
		return false;
	}
}
