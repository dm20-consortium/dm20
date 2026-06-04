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
		// presetTimerのケース
		if (string(inet_ntoa(addr.sin_addr)) == "0.0.0.0") {
			logger->info("[" + this->type + "] ========== Response END ========== send-IP: 0.0.0.0");
			return true;
		}
		if (send_err_flag) {
			return true;
		}
#if MEASURE_MODE == 1
		long startTime = DmUtil::getTimeMicrosec();
		long procTime = startTime;
		int step = 1;
#endif
		IS::ProtobufParser &pp = IS::ProtobufParser::get_instance();
		
		// Selectionは1つのtuplesetを使用
		TupleSet& tupleset = ts.at(0);

		// 時刻情報を追加付与
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
		// DEBUG 与えられたタプル情報の出力
		printInputInfo(tupleset, this->argument);

		// QueryResultXMLを生成する
		string retXML = "";
		vector<string> retXMLList;

		int sepSize = IPv4_UDP_MAX_BYTE;
		if (ssl != NULL) sepSize = IPv4_DTLS_MAX_BYTE;
#if MEASURE_MODE == 1
		long now = DmUtil::getTimeMicrosec();
		double msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " parameter analytics processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif
		pp.init();
		if (code == ErrorCode::NO_ERR) {

			// 返却するTupleが存在しないかつ、待機状態からnotify_one()により起動された場合、かつUDP使用時(継続クエリ)は返却しない
			if (tupleset.size() == 0 && isTCP == false) {
				long completeTime = DmUtil::getTimeMicrosec();
				if (tupleset.info.recvTime != 0) {
					double totalProcTime = ((completeTime - tupleset.info.recvTime) / 1000.0);
					if (totalProcessNoResTimeSlowest < totalProcTime) totalProcessNoResTimeSlowest = totalProcTime;
					if (totalProcessNoResTimeEarliest == 0 || totalProcessNoResTimeEarliest > totalProcTime) totalProcessNoResTimeEarliest = totalProcTime;
					totalProcessNoResTimeAVG = (double)((totalProcessNoResTimeAVG * (processNum - notifiedNum)) + totalProcTime) / (double)(processNum + 1 - notifiedNum);
					logger->debug("[PERFORMANCE][" + getType() + "] totalProcTime(NoRes):" + to_string(totalProcTime) + "[ms] AVGTime(NoRes):" + to_string(totalProcessNoResTimeAVG) + "[ms] procNum:" + to_string(processNum + 1 - notifiedNum));
				}
				logger->debug("[" + this->type + "] ========== Response  END  (No Tuple) ========== ");
				return false;
			}

			if (isTCP) {
				// 返信種別を判定
				switch (currentResponseType)
				{
					// クエリ登録応答
					case responseType::RESPONSE_QUERY:
						// クエリ管理番号の返送
						retXML = pp.createQueryResponse(mngId, tcpPort);
						break;
					// クエリキャンセル応答
					case responseType::RESPONSE_CANCEL:
						// クエリキャンセルの応答
						retXML = pp.createCancelResponse(mngId);
						break;
					// 継続クエリ結果送信
					case responseType::QUERY_RESULT:
						// クエリ結果をprotobufに変換
						retXML = pp.createQueryResult(mngId, this->protobufMessageName, tupleset, this->isDynamicColumn);
						break;
				}
			} else {
				// 管理者カラム制御
				hideColumn(tupleset);
				// for (int idx = 0; idx < tupleset.size(); idx++) tupleset.getTuple(idx).dump();
				// クエリ結果をprotobufに変換 + 指定バイト長で分割
				retXMLList = pp.createQueryResult(mngId, this->protobufMessageName, tupleset, this->isDynamicColumn, IPv4_UDP_MAX_BYTE);
			}
		}
		else {
			// 返信種別を判定
			switch (currentResponseType)
			{
				// クエリキャンセル応答
				case responseType::RESPONSE_CANCEL:
					// エラー時のクエリキャンセルレスポンスを生成
					retXML = pp.createCancelErrorResponse(to_string(static_cast<int>(code)), msg);
					break;
				// 継続クエリ結果送信
				case responseType::QUERY_RESULT:
					// エラー時のクエリレスポンスを生成
					retXML = pp.createQueryErrorResponse(to_string(static_cast<int>(code)), msg);
					break;
			}
		}
		pp.finalize();

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " create xml processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif
		if (isTCP) {
			bool doSend = true;
			if (ssl != NULL) {
				if (isSslShutdown(ssl)) {
					doSend = false;
				}
			}
			if (doSend) {
				// TCPにてレスポンス返却
				TCPSend(retXML);
				if (tupleset.size() == 0 && (Operator::isDataReady() == false)) {
					// 継続クエリの管理番号返却時、キャンセル要求時はexecuteを通らないため終了可能
					exitReady = true;
				}
			}
		}
		else {
			struct sockaddr_in udpAddr;
			int udpSock;
			if (!replyForConQueryByTcp) {
				// UDPにてレスポンス返却
				if (ssl == NULL) {
					udpSock = socket(AF_INET, SOCK_DGRAM, 0);
					udpAddr.sin_family = AF_INET;
					udpAddr.sin_port = htons(this->udpPort);
					udpAddr.sin_addr.s_addr = inet_addr(inet_ntoa(addr.sin_addr));
				}
			}
			int ret = 0;
			unsigned int sendSumLen = 0;
			for (unsigned int i = 0; i < retXMLList.size(); i++) {
				//cout << retXMLList.at(i) << endl;
				if (ssl == NULL) {
					if (replyForConQueryByTcp) {
						Bytef outbuf[this->TcpSendSize];
						logger->debug("[ResponseOperator]send_sock:" + to_string(this->sock));
						if (stringUtil.compress(retXMLList.at(i), outbuf)) {
							try {
								ret = send(this->sock, outbuf, this->TcpSendSize, 0);
							} catch (...) {
								logger->warn(" ========== Send Exception Eroor  ========== sock: " + to_string(this->sock));
							}
						} else {
							ret = 0;
						}
					} else {
						char compressFlg = settings.getParameter("COMPRESS_FLG")[0];
						if (compressFlg == '1' || compressFlg == '2' ) {
							char outbuf[UNCOMPRESSED_BUF_SIZE];
							long key = DmUtil::getTimeMicrosec();
							int sendSize = stringUtil.setCompressedBufWithHeader(retXMLList.at(i), outbuf, compressFlg, key);
							if (sendSize > 0) {
								try {
									ret = sendto(udpSock, outbuf, sendSize, 0, (struct sockaddr *)&udpAddr, sizeof(udpAddr));
									//cout << "[" + this->type + "]ret:" << ret << ", sendSize:" << sendSize << endl;
								} catch (...) {
									logger->warn(" ========== Send Exception Eroor  ========== sock: " + to_string(this->sock));
								}
							} else {
								ret = 0;
								logger->warn("[" + this->type + "] CompressProc is Failed.");
								// 互換性のため、圧縮フラグは付与しない。
								//string s = "0" + retXMLList.at(i);
								string s = retXMLList.at(i);
								ret = sendto(udpSock, s.c_str(), s.length(), 0, (struct sockaddr *)&udpAddr, sizeof(udpAddr));
							}
						} else {
							// 互換性のため、圧縮フラグは付与しない。
							//string s = compressFlg + retXMLList.at(i);
							string s = retXMLList.at(i);
							ret = sendto(udpSock, s.c_str(), s.length(), 0, (struct sockaddr *)&udpAddr, sizeof(udpAddr));
						}
					}
				} else {
					if (isSslShutdown(ssl)) {
						break;
					}
					if (replyForConQueryByTcp) {
						Bytef outbuf[this->TcpSendSize];
						if (stringUtil.compress(retXMLList.at(i), outbuf)) {
							try {
								ret = SSL_write(ssl, outbuf, this->TcpSendSize);
							} catch (...) {
								logger->warn(" ========== Send Exception Eroor  ========== sock: " + to_string(this->sock));
							}
						} else {
							ret = 0;
						}
					} else {
						bool iscontinue = false;
						char compressFlg = settings.getParameter("COMPRESS_FLG")[0];
						if (compressFlg == '1' || compressFlg == '2' ) {
							char outbuf[UNCOMPRESSED_BUF_SIZE];
							long key = DmUtil::getTimeMicrosec();
							int sendSize = stringUtil.setCompressedBufWithHeader(retXMLList.at(i), outbuf, compressFlg, key);
							if (sendSize > 0) {
								try {
									ret = SSL_write(ssl, outbuf, sendSize);
									//cout << "[" + this->type + "]ret:" << ret << ", sendSize:" << sendSize << endl;
								} catch (...) {
									logger->warn(" ========== Send Exception Eroor  ========== sock: " + to_string(this->sock));
								}
							} else {
								ret = 0;
								logger->warn("[" + this->type + "] CompressProc is Failed.");
								// 互換性のため、圧縮フラグは付与しない。
								//string s = "0" + retXMLList.at(i);
								string s = retXMLList.at(i);
								ret = SSL_write(ssl, retXMLList.at(i).c_str(), retXMLList.at(i).length());
							}
						} else {
							ret = SSL_write(ssl, retXMLList.at(i).c_str(), retXMLList.at(i).length());
						}
						if (ret < 0) {
							switch (SSL_get_error(ssl, ret)) {
							case SSL_ERROR_NONE:
								cout << "ERROR_NONE" << endl;
								break;
							case SSL_ERROR_WANT_WRITE:
								cout << "ERROR_WANT_WRITE" << endl;
								break;
							case SSL_ERROR_SSL:
								cout << "ERROR_SSL" << endl;
								break;
							case SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER:
								cout << "SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER" << endl;
								break;
							case SSL_ERROR_SYSCALL:
								logger->warn("[ResponseOperator] ERROR_SYSCALL is catched");
								process_close();
								if(setDTLSsocket()) {
									logger->warn("[ResponseOperator] Reset DTLS Socket and Continue");
									iscontinue = true;
								} else {
									process_close();
								}
								break;
							default:
								logger->warn("[ResponseOperator] ERROR_ERROR_OTHER is catched");
								break;
							}
							if (iscontinue) continue;
						}
					}
					if (ret < 0) {
						//コネクションが切れた後に、再送しようとすると、プロセスが終了するため、キャンセルされるまでprocess関数内では何もしない。
						send_err_flag = true;
					}
				}

				if (ret > 0) {
					sendSumLen = sendSumLen + ret;
				}
				else {
					string errMsg = string(std::strerror(errno));
					if (replyForConQueryByTcp) {
						string errIp = inet_ntoa(addr.sin_addr);
						logger->error("[" + this->type + "] Failed to send TCP errmsg: " + errMsg + ",ip:" + errIp);
					} else {
						string errIp = inet_ntoa(udpAddr.sin_addr);
						logger->error("[" + this->type + "] Failed to sendto. retry.... split.length:" + std::to_string(retXMLList.at(i).length()));
						logger->error("[" + this->type + "] Failed to send UDP errmsg: " + errMsg + ",ip:" + errIp);
						if (ssl != NULL) {
							if (isSslShutdown(ssl)) {
								logger->error("[" + this->type + "] ssl shutdown");
							}
						}
					}
				}
			}
			ret = sendSumLen;

			if (ret > 0) {
				logger->debug("[" + this->type + "] Response by " + protocol + ". MNGID:" + std::to_string(mngId) + " sendto : " + string(inet_ntoa(addr.sin_addr)) + ":" + std::to_string(this->udpPort) + " Size:" + std::to_string(ret) + " byte");
			}
			else {
				//logger->debug("[" + this->type + "] MNGID:" + std::to_string(mngId) + " sendto error: " + string(inet_ntoa(addr.sin_addr)) + ":" + std::to_string(this->udpPort) + " XMLSize : " + std::to_string(retXML.length()) + " byte");
				//logger->debug("[" + this->type + "] Failed to send. errmsg: " + string(std::strerror(errno)));
			}
			if (!replyForConQueryByTcp) {
				close(udpSock);
			}
		}

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " sendto processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

		long completeTime = DmUtil::getTimeMicrosec();
		if (tupleset.info.recvTime != 0) {
			double totalProcTime = ((completeTime - tupleset.info.recvTime) / 1000.0);
			if (totalProcessTimeSlowest < totalProcTime) totalProcessTimeSlowest = totalProcTime;
			if (totalProcessTimeEarliest == 0 || totalProcessTimeEarliest > totalProcTime) totalProcessTimeEarliest = totalProcTime;
			totalProcessTimeAVG = (double)((totalProcessTimeAVG * (notifiedNum)) + totalProcTime) / (double)(notifiedNum + 1);
			logger->debug("[PERFORMANCE][" + getType() + "] totalProcTime:" + to_string(totalProcTime) + "[ms] AVGTime:" + to_string(totalProcessTimeAVG) + "[ms] procNum:" + to_string(processNum + 1) + " notifiedNum:" + to_string(++notifiedNum));
		}

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - startTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " total processing time: " + to_string(msec) + "[ms]");
#endif
		logger->debug(" ========== Response  END  ========== ");
		return true;
	}

	/**
	* TCPで指定した文字列を送信する
	*
	* @author	Nagoya University
	* @date	2018/10/29
	*
	* @param [in]	body	送信文字列
	*
	* @return	送信byte
	*/
	int ResponseOperator::TCPSend(const string &body)
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
