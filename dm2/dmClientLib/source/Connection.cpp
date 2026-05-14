#include "is/Connection.h"
#include "is/StringUtil.h"
#include "is/DmUtil.h"
#include "is/DatagramSocket.h"
#include "is/DmManager.h"

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <zlib.h>

const int BUF_MAX = 4096;
unsigned char Connection::cookie_secret[COOKIE_SECRET_LENGTH] = {};
int Connection::cookie_initialized = 0;
string Connection::pemPass = "";

/**
 * コンストラクタ
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	ip  	IPアドレス
 * @param	port	ポート番号
 * @param	sock	ソケット
 * @param	addr	ソケットアドレス情報
 * @param	key		セッションキー
 * @param	tcpSslTimeoutSec TCP/SSLタイムアウト秒
 */

Connection::Connection(const string &ip, const int port, const int sock, const struct sockaddr_in addr, const string &key, const int tcpSslTimeoutSec, const bool isTransportMode)
{
	this->ip = ip;
	this->port = port;
	this->sock = sock;
	this->isTransportMode = isTransportMode;
	this->addr = addr;
	this->key = key;
	this->tcpSslTimeoutSec = tcpSslTimeoutSec;
	callBackMp.clear();
	callBackClassMp.clear();
}

/**
* コンストラクタ(SSL)
*
* @author	Nagoya University
* @date	2019/07/31
*
* @param	ip  	IPアドレス
* @param	port	ポート番号
* @param	sock	ソケット
* @param	addr	ソケットアドレス情報
* @param	key		セッションキー
* @param	ssl		SSLオブジェクト
* @param	ctx		SSLコンテキスト
* @param	certFilePath		サーバ証明書ファイルの絶対パス指定
* @param	privateKeyFilePath	プライベートキーファイルの絶対パス指定
* @param	inPemPass			証明書のパスフレーズ
* @param	tcpSslTimeoutSec	TCP/SSLタイムアウト秒
* @param	dtlsTimeoutSec		DTLSタイムアウト秒
*/

Connection::Connection(const string &ip, const int port, const int sock,  const struct sockaddr_in addr, const string &key,
	SSL *ssl, SSL_CTX *ctx, const string &certFilePath, const string &privateKeyFilePath, const string &inPemPass,
	const int tcpSslTimeoutSec, const int dtlsTimeoutSec, const bool isTransportMode)
{
	this->ip = ip;
	this->port = port;
	this->sock = sock;
	this->addr = addr;
	this->key = key;
	this->ssl = ssl;
	this->isTransportMode = isTransportMode;
	this->ctx = ctx;
	this->certFilePath = certFilePath;
	this->privateKeyFilePath = privateKeyFilePath;
	pemPass = inPemPass;
	this->tcpSslTimeoutSec = tcpSslTimeoutSec;
	this->dtlsTimeoutSec = dtlsTimeoutSec;
	callBackMp.clear();
	callBackClassMp.clear();
	isSSL = true;
}

/**
 * デストラクタ
 *
 * @author	Nagoya University
 * @date	2018/03/15
 */

Connection::~Connection()
{
	if (ssl != NULL) {
		if (SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN) {
			SSL_set_shutdown(ssl, SSL_SENT_SHUTDOWN);
			SSL_shutdown(ssl);
		} else {
			SSL_clear(ssl);
		}
		SSL_free(ssl);
		ssl = NULL;
	}
	/* socketの終了 */
	close(sock);

	if (isTransportMode) {
		if (ssl2 != NULL) {
			if (SSL_get_shutdown(ssl2) & SSL_RECEIVED_SHUTDOWN) {
				SSL_set_shutdown(ssl2, SSL_SENT_SHUTDOWN);
				SSL_shutdown(ssl2);
			} else {
				SSL_clear(ssl2);
			}
			SSL_free(ssl2);
			ssl2 = NULL;
		}
		close(sock2);
	}
	if (ctx != NULL) {
		SSL_CTX_free(ctx);
		ctx = NULL;
	}
}

ResultSet Connection::changeid(const string &query)
{
	char buffer[BUF_MAX];
	int data = 0;
	int sumLen = 0;
	int bufSize = 0;
	
	string receiveData = "";

	if (query.length() <= 6) {
		cout << "[execute] ERROR occured, throw SQLException. cause: Illigal SQL statement" << endl;
		throw SQLException("[execute] ERROR occured, throw SQLException. cause: Illigal SQL statement");
	}
	// クエリをQueryXMLに変換
	string queryXML;
	IS::InformationSourceParser isp;
	isp.createChangeidXML(query, 0, this->key, queryXML, false);

	// サーバに送信
	int retLen;
	if(!isSSL){
		retLen = send(sock, queryXML.c_str(), queryXML.length(), 0);
		cout << "send cahngeID" << endl;
	}
	else{
		retLen = SSL_write(ssl, queryXML.c_str(), queryXML.length());
	}
		
	cout << to_string(retLen) << endl;

	if (retLen < 0) {
		perror("[changeID] send/SSL_write error");
		cerr << "[changeID] send/SSL_write TCP Connection Timeout. errno:" << errno << endl;
		throw ConnectionTimeoutException();
	}

	// ResultSetに変換
	ResultSet ret;

	return ret;

}

/**
 * One-Shot Query(ワンショットクエリ)の実行
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @exception	ConnectionTimeoutException	DBシステムとの接続でタイムアウト発生
 * @exception	SQLException			  	クエリの構文エラー
 *
 * @param	query	クエリ文字列
 *
 * @return	クエリ結果
 */

ResultSet Connection::execute(const string &query)
{
	char buffer[BUF_MAX];
	int data = 0;
	int sumLen = 0;
	int bufSize = 0;
	
	string receiveData = "";

	if (query.length() <= 6) {
		cout << "[execute] ERROR occured, throw SQLException. cause: Illigal SQL statement" << endl;
		throw SQLException("[execute] ERROR occured, throw SQLException. cause: Illigal SQL statement");
	}
#if MEASURE_MODE == 1
	long startTime = DmUtil::getTimeMicrosec();
	long procTime = startTime;
#endif
	// クエリをQueryXMLに変換
	string queryXML;
	IS::InformationSourceParser isp;
	isp.createQueryXML(query, 0, this->key, queryXML, false);
#if MEASURE_MODE == 1
	long now = DmUtil::getTimeMicrosec();
	double msec = (now - procTime) / 1000.0;
	cout << "[STAT] createQueryXML processTime(msec):" << msec << endl;
#endif
#if DEBUG == 1
	cout << "[execute] ---------------- REQUEST XML ----------------" << endl;
	cout << "[execute] " << query << endl;
#endif
#if MEASURE_MODE == 1
	procTime = DmUtil::getTimeMicrosec();
#endif
	// サーバに送信
	int retLen;
	if(!isSSL)
		retLen = send(sock, queryXML.c_str(), queryXML.length(), 0);
	else
		retLen = SSL_write(ssl, queryXML.c_str(), queryXML.length());
#if DEBUG == 1
	cout << "[execute] dataSize: " << queryXML.length() << " retSize : " << retLen << endl;
#endif
	if (retLen < 0) {
		perror("[execute] send/SSL_write error");
		cerr << "[execute] send/SSL_write TCP Connection Timeout. errno:" << errno << endl;
		throw ConnectionTimeoutException();
	}
#if MEASURE_MODE == 1
	now = DmUtil::getTimeMicrosec();
	msec = (now - procTime) / 1000.0;
	cout << "[STAT] send/SSL_write processTime(msec):" << msec << endl;

	procTime = DmUtil::getTimeMillisec();
#endif
	string bufStr;

	/* サーバからデータを受信 */
	while (1) {
		memset(buffer, 0, sizeof(buffer));
		if(!isSSL)
			data = read(sock, buffer, sizeof(buffer));
		else
			data = SSL_read(ssl, buffer, sizeof(buffer));

		if (data < 0) {
			perror("[execute] read/SSL_read error");
			cerr << "[execute] read/SSL_read TCP Connection Timeout. errno:" << errno << endl;
			throw ConnectionTimeoutException();
		}
		else if (data == 0) {
			continue;
		}
		bufStr = string(buffer, data);
		if (receiveData.length() == 0) {
			bufSize = stringUtil.getXMLSize(bufStr);
		}
		receiveData.append(bufStr);
		sumLen = sumLen + data;
		if (sumLen == bufSize) break;
	}
#if MEASURE_MODE == 1
	now = DmUtil::getTimeMicrosec();
	msec = (now - procTime) / 1000.0;
	cout << "[STAT] read/SSL_read processTime(msec):" << msec << endl;
#endif
	
#if DEBUG == 1
	/*受信したデータを表示*/
	cout << "[execute] ---------------- RESULT RECV ----------------" << endl;
	cout << "[execute] " << receiveData << endl;
	printf("[execute] size : %d\n", sumLen);
#endif
#if MEASURE_MODE == 1
	procTime = DmUtil::getTimeMicrosec();
#endif
	// SQLエラーのハンドリング
	if (isp.getErrorCode(buffer) != 0) {
		cerr << "[execute] ERROR occured, throw SQLException. code:" << isp.getErrorCode(buffer) << endl;
		throw SQLException("[execute] ErrorCode:" + to_string(isp.getErrorCode(buffer)) + " Msg:" + isp.getErrorMessage(buffer));
	}
#if MEASURE_MODE == 1
	now = DmUtil::getTimeMicrosec();
	msec = (now - procTime) / 1000.0 ;
	cout << "[STAT] ErrorCheck processTime(msec):" << msec << endl;

	procTime = DmUtil::getTimeMicrosec();
#endif
	// ResultSetに変換
	ResultSet ret;
	isp.getResultSetBySAX(receiveData, ret);
#if MEASURE_MODE == 1
	now = DmUtil::getTimeMicrosec();
	msec = (now - procTime) / 1000.0;
	cout << "[STAT] ResultSet processTime(msec):" << msec << endl;
	msec = (now - startTime) / 1000.0;
	cout << "[STAT] execute processTime(msec):" << msec << endl;
#endif
	return ret;

}

/**
 * Continuous Query(継続クエリ)の登録
 *
 * @author	Shinichi Kusayama
 * @date	2023/03/06
 *
 * @exception	ConnectionTimeoutException	DBシステムとの接続でタイムアウト発生
 * @exception	SQLException			  	クエリの構文エラー
 *
 * @param	query	クエリ文字列
 * @param	func_c 	コールバック関数
 * @param	func_c_plus 	コールバック関数
 *
 * @return	クエリ管理番号
 */

unsigned int Connection::registerQuery(const string &query, FUNC_CALLBACK func_c, std::function<void(ResultSet)> func_c_plus, const string &reference)
{
	char buffer[BUF_MAX];
	int data;

	if (query.length() <= 6) {
		cerr << "[registerQuery] ERROR occured, throw SQLException. cause: Illigal SQL statement" << endl;
		throw SQLException("[registerQuery] ERROR occured, throw SQLException. cause: Illigal SQL statement");
	}
#if MEASURE_MODE == 1
	long startTime = DmUtil::getTimeMicrosec();
	long procTime = startTime;
#endif
	// ポート確認
	struct sockaddr_in confirm_addr;

	if (isTransportMode == false) {
		int Sock = socket(AF_INET, SOCK_DGRAM, 0);
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;

		bool bindUdp = false;
		if (func_c != NULL) {
			if (callBackMp.size() == 0) bindUdp = true;
		} else {
			if (callBackClassMp.size() == 0) bindUdp = true;
		}
		if (bindUdp) {
			udpReceptionPort = 9010;
			for (int i = 0; i < 30; i++)
			{
				udpReceptionPort = udpReceptionPort + i;
				addr.sin_port = htons(udpReceptionPort);
				int ret = bind(Sock, (struct sockaddr *)&addr, sizeof(addr));
				if (ret == 0) {
					close(Sock);
					break;
				}
			}
		}
	}

#if MEASURE_MODE == 1
	long now = DmUtil::getTimeMicrosec();
	double msec = (now - procTime) / 1000.0;
	cout << "[STAT] confirmPort processTime(msec):" << msec << endl;

	procTime = DmUtil::getTimeMicrosec();
#endif
	// クエリをQueryXMLに変換
	string queryXML;
	IS::InformationSourceParser isp;
	isp.createQueryXML(query, udpReceptionPort, this->key, queryXML, isTransportMode);
#if MEASURE_MODE == 1
	now = DmUtil::getTimeMicrosec();
	msec = (now - procTime) / 1000.0;
	cout << "[STAT] createQueryXML processTime(msec):" << msec << endl;
#endif
#if DEBUG == 1
	cout << "[registerQuery] ---------------- REQUEST XML ----------------" << endl;
	cout << "[registerQuery] " << query << endl;
#endif
#if MEASURE_MODE == 1
	procTime = DmUtil::getTimeMicrosec();
#endif
	// サーバに送信
	int ret;
	if (!isSSL)
		ret = send(sock, queryXML.c_str(), queryXML.length(), 0);
	else
		ret = SSL_write(ssl, queryXML.c_str(), queryXML.length());

	if (ret < 0) {
		perror("[registerQuery] send/SSL_write error");
		cerr << "[registerQuery] send/SSL_write TCP Connection Timeout. errno:" << errno << endl;
		throw ConnectionTimeoutException();
	}
#if MEASURE_MODE == 1
	now = DmUtil::getTimeMicrosec();
	msec = (now - procTime) / 1000.0;
	cout << "[STAT] send/SSL_write processTime(msec):" << msec << endl;

	procTime = DmUtil::getTimeMicrosec();
#endif
#if DEBUG == 1
	cout << "[registerQuery] dataSize: " << queryXML.length() << " retSize : " << ret << endl;
#endif
	/* サーバからデータを受信 */
	memset(buffer, 0, sizeof(buffer));

	if (!isSSL)
		data = read(sock, buffer, sizeof(buffer));
	else
		data = SSL_read(ssl, buffer, sizeof(buffer));

	if (data < 0) {
		perror("[registerQuery] read/SSL_read error");
		cerr << "[registerQuery] recv TCP Connection Timeout. errno:" << errno << endl;
		throw ConnectionTimeoutException();
	}
	else if (data == 0) {
		throw SQLException("[registerQuery] After read/SSL_read, Response size is 0");
	}
#if MEASURE_MODE == 1
	now = DmUtil::getTimeMicrosec();
	msec = (now - procTime) / 1000.0;
	cout << "[STAT] read/SSL_read processTime(msec):" << msec << endl;
#endif
#if DEBUG == 1
	/*受信したデータを表示*/
	cout << "[registerQuery] ---------------- RESULT RECV ----------------" << endl;
	printf("[registerQuery] %s\n", buffer);
	printf("[registerQuery] size : %d\n", data);
#endif
	// SQLエラーのハンドリング
	if (isp.getErrorCode(buffer) != 0) {
		cerr << "[registerQuery] ERROR occured, throw SQLException. code:" << isp.getErrorCode(buffer) << endl;
		throw SQLException("[registerQuery] ErrorCode:" + to_string(isp.getErrorCode(buffer)) + " Msg : " + isp.getErrorMessage(buffer));
	}
#if MEASURE_MODE == 1
	procTime = DmUtil::getTimeMicrosec();
#endif
	int tcpPort = 0;
	// レスポンスからクエリ番号を取得
	unsigned int mngId = isp.getMngId(buffer, tcpPort);
#if DEBUG == 1
	printf("Set mngId:%d\n", mngId);
#endif
	if (func_c != NULL) {
		// クエリ管理番号を保持
		callBackMp.insert(std::pair<unsigned int, FUNC_CALLBACK>(mngId, func_c));
	} else {
		// クエリ管理番号を保持
		callBackClassMp.insert(std::pair<unsigned int, std::function<void(ResultSet)>>(mngId, func_c_plus));
	}
	if (reference == "") {
		referenceMp[mngId] = query;
	} else {
		referenceMp[mngId] = reference;
	}
#if MEASURE_MODE == 1
	now = DmUtil::getTimeMicrosec();
	msec = (now - procTime) / 1000.0;
	cout << "[STAT] getMngId&save processTime(msec):" << msec << endl;

	procTime = DmUtil::getTimeMicrosec();
#endif
	if (isTransportMode) {
		usleep(5000);
		DmManager dm;
		struct sockaddr_in addr;
		int dataSock;
		SSL *dataSsl = NULL;
		isTransportMode = true;
		if (!isSSL) {
			dataSock = dm.connectSock(this->ip, tcpPort, addr);
		} else {
			dataSsl = dm.connectSSLSock(dataSock, this->ip, tcpPort, addr, this->ctx);
			this->ssl2 = dataSsl;
		}
		this->sock2 = dataSock;
	}
	bool startReceive = false;
	if (func_c != NULL) {
		if (callBackMp.size() == 1) startReceive = true;
	} else {
		if (callBackClassMp.size() == 1) startReceive = true;
	}
	if (startReceive) {
		//cout << "[RegisterQuery]: udpReceptionPort " << udpReceptionPort << endl;
		int recvPort;
		if (isTransportMode) {
			recvPort = this->port;
		} else {
			recvPort = udpReceptionPort;
		}
		// 受信スレッドの開始
		if (!isSSL) {
			thread receiveThread(&Connection::receiveData, this, recvPort);
			receiveThread.detach();
		}
		else {
			if (isTransportMode) {
				void *info;
				thread receiveThread(&Connection::sockDTLSProcess, this, info);
				receiveThread.detach();
			} else {
				thread receiveThread(&Connection::receiveDTLSData, this, recvPort);
				receiveThread.detach();
			}
		}
	}
#if MEASURE_MODE == 1
	now = DmUtil::getTimeMicrosec();
	msec = (now - procTime) / 1000.0;
	cout << "[STAT] receiveThread start processTime(msec):" << msec << endl;
	msec = (now - startTime) / 1000.0;
	cout << "[STAT] registerQuery processTime(msec):" << msec << endl;
#endif

	return mngId;
}


/**
 * Continuous Query(継続クエリ)の登録(C言語ライクなAPI)
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @exception	ConnectionTimeoutException	DBシステムとの接続でタイムアウト発生
 * @exception	SQLException			  	クエリの構文エラー
 *
 * @param	query	クエリ文字列
 * @param	func 	コールバック関数
 *
 * @return	クエリ管理番号
 */

unsigned int Connection::registerQuery(const string &query, FUNC_CALLBACK func)
{
	return registerQuery(query, func, NULL, "");
}

/**
 * Continuous Query(継続クエリ)の登録(C言語ライクなAPI)
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @exception	ConnectionTimeoutException	DBシステムとの接続でタイムアウト発生
 * @exception	SQLException			  	クエリの構文エラー
 *
 * @param	query	クエリ文字列
 * @param	func 	コールバック関数
 *
 * @return	クエリ管理番号
 */

unsigned int Connection::registerQuery(const string &query, FUNC_CALLBACK func, const string &reference)
{
	return registerQuery(query, func, NULL, reference);
}

/**
* Continuous Query(継続クエリ)の登録(C++言語の関数型指定API)
*
* @author	Nagoya University
* @date	2019/09/12
*
* @exception	ConnectionTimeoutException	DBシステムとの接続でタイムアウト発生
* @exception	SQLException			  	クエリの構文エラー
*
* @param	query	クエリ文字列
* @param	func 	コールバック関数
*
* @return	クエリ管理番号
*/

unsigned int Connection::registerQuery(const string &query, std::function<void(ResultSet)> func)
{
	return registerQuery(query, NULL, func, "");
}

/**
* Continuous Query(継続クエリ)の登録(C++言語の関数型指定API)
*
* @author	Nagoya University
* @date	2019/09/12
*
* @exception	ConnectionTimeoutException	DBシステムとの接続でタイムアウト発生
* @exception	SQLException			  	クエリの構文エラー
*
* @param	query	クエリ文字列
* @param	func 	コールバック関数
*
* @return	クエリ管理番号
*/

unsigned int Connection::registerQuery(const string &query, std::function<void(ResultSet)> func, const string &reference)
{
	return registerQuery(query, NULL, func, reference);
}

/**
* Continuous Query(継続クエリ)の登録(C++言語(オブジェクト指向)ライクなAPI)
*
* @author	Nagoya University
* @date	2019/09/20
*
* @exception	ConnectionTimeoutException	DBシステムとの接続でタイムアウト発生
* @exception	SQLException			  	クエリの構文エラー
*
* @param	query	クエリ文字列
* @param	cbl 	CallBackListener継承クラス
*
* @return	クエリ管理番号
*/
unsigned int Connection::registerQuery(const string &query, CallBackListener* cbl)
{
	return registerQuery(query, NULL, std::bind(&CallBackListener::callBack, std::ref(*cbl), std::placeholders::_1), "");
}

/**
* Continuous Query(継続クエリ)の登録(C++言語(オブジェクト指向)ライクなAPI)
*
* @author	Nagoya University
* @date	2019/09/20
*
* @exception	ConnectionTimeoutException	DBシステムとの接続でタイムアウト発生
* @exception	SQLException			  	クエリの構文エラー
*
* @param	query	クエリ文字列
* @param	cbl 	CallBackListener継承クラス
*
* @return	クエリ管理番号
*/
unsigned int Connection::registerQuery(const string &query, CallBackListener* cbl, const string &reference)
{
	return registerQuery(query, NULL, std::bind(&CallBackListener::callBack, std::ref(*cbl), std::placeholders::_1), reference);
}

/**
* 宛先付き Queryの登録
*
* @author	Shinichi Kusayama
* @date	2023/08/14
*
* @exception	ConnectionTimeoutException	DBシステムとの接続でタイムアウト発生
* @exception	SQLException			  	クエリの構文エラー
*
* @param	query	クエリ文字列
* @param	cbl 	CallBackListener継承クラス
*
* @return	クエリ管理番号
*/
unsigned int Connection::registerQuery(const string &query, const unsigned long long destSID) 
{
	return registerQuery(query, destSID, 0);
}
/**
* 宛先付き Queryの登録(実行元SID指定)
*
* @author	Nagoya University
* @date	2025/01/20
*
* @exception	ConnectionTimeoutException	DBシステムとの接続でタイムアウト発生
* @exception	SQLException			  	クエリの構文エラー
*
* @param	query	クエリ文字列
* @param	destSID 宛先SID
* @param    executerSID 実行元SID
*
*/
unsigned int Connection::registerQuery(const string &query, const unsigned long long destSID, const unsigned long long executerSID) 
{
	char buffer[BUF_MAX];
	int data;

	if (query.length() <= 6) {
		cerr << "[registerQuery] ERROR occured, throw SQLException. cause: Illigal SQL statement" << endl;
		throw SQLException("[registerQuery] ERROR occured, throw SQLException. cause: Illigal SQL statement");
	}
	// クエリをQueryXMLに変換
	string queryXML;
	IS::InformationSourceParser isp;
	isp.createQueryXML(query, this->key, queryXML, destSID, executerSID);
	// サーバに送信
	int ret;
	if (!isSSL)
		ret = send(sock, queryXML.c_str(), queryXML.length(), 0);
	else
		ret = SSL_write(ssl, queryXML.c_str(), queryXML.length());

	if (ret < 0) {
		perror("[registerQuery] send/SSL_write error");
		cerr << "[registerQuery] send/SSL_write TCP Connection Timeout. errno:" << errno << endl;
		throw ConnectionTimeoutException();
	}
	/* サーバからデータを受信 */
	memset(buffer, 0, sizeof(buffer));

	if (!isSSL)
		data = read(sock, buffer, sizeof(buffer));
	else
		data = SSL_read(ssl, buffer, sizeof(buffer));

	if (data < 0) {
		perror("[registerQuery] read/SSL_read error");
		cerr << "[registerQuery] recv TCP Connection Timeout. errno:" << errno << endl;
		throw ConnectionTimeoutException();
	}
	// SQLエラーのハンドリング
	if (isp.getErrorCode(buffer) != 0) {
		cerr << "[registerQuery] ERROR occured, throw SQLException. code:" << isp.getErrorCode(buffer) << endl;
		throw SQLException("[registerQuery] ErrorCode:" + to_string(isp.getErrorCode(buffer)) + " Msg : " + isp.getErrorMessage(buffer));
	}
	int tcpPort = 0;
	// レスポンスからクエリ番号を取得
	unsigned int mngId = isp.getMngId(buffer, tcpPort);
	return mngId;
}
/**
 * 継続クエリのキャンセル要求
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @exception	ConnectionTimeoutException	DBシステムとの接続でタイムアウト発生
 *
 * @param	mngId	クエリ管理番号
 */

void Connection::cancelQuery(const unsigned int mngId)
{
	char buffer[BUF_MAX];
	int data;

	// DBシステムに対して継続クエリのキャンセル要求を投げる
	string cancelQuery;
	IS::InformationSourceParser isp;
	isp.createCancelXML(mngId, this->key, cancelQuery);

#if DEBUG == 1
	cout << "[cancelQuery] ---------------- REQUEST XML ----------------" << endl;
	cout << "[cancelQuery] " << cancelQuery << endl;
#endif
#if MEASURE_MODE == 1
	long procTime = DmUtil::getTimeMicrosec();
#endif
	int ret;
	if(!isSSL)
		ret = send(sock, cancelQuery.c_str(), cancelQuery.length(), 0);
	else
		ret = SSL_write(ssl, cancelQuery.c_str(), cancelQuery.length());

#if DEBUG == 1
	cout << "[cancelQuery] dataSize: " << cancelQuery.length() << " retSize : " << ret << endl;
#endif
	if (ret < 0) {
		perror("[cancelQuery] send/SSL_write error");
		cerr << "[cancelQuery] TCP Connection Timeout. errno:" << errno << endl;
		throw ConnectionTimeoutException();
	}
#if MEASURE_MODE == 1
	long now = DmUtil::getTimeMicrosec();
	long msec = (now - procTime) / 1000.0;
	cout << "[STAT] send/SSL_write processTime(msec):" << msec << endl;
#endif
	/* サーバからデータを受信 */
	memset(buffer, 0, sizeof(buffer));
#if MEASURE_MODE == 1
	procTime = DmUtil::getTimeMicrosec();
#endif
	if(!isSSL)
		data = read(sock, buffer, sizeof(buffer));
	else
		data = SSL_read(ssl, buffer, sizeof(buffer));

	if (data < 0) {
		perror("[cancelQuery] read/SSL_read error");
		cerr << "[cancelQuery] recv TCP Connection Timeout. errno:" << errno << endl;
		throw ConnectionTimeoutException();
	}
#if MEASURE_MODE == 1
	now = DmUtil::getTimeMicrosec();
	msec = (now - procTime) / 1000.0;
	cout << "[STAT] read/SSL_read processTime(msec):" << msec << endl;
#endif
#if DEBUG == 1
	/*受信したデータを表示*/
	cout << "[cancelQuery] ---------------- RESULT RECV ----------------" << endl;
	printf("[cancelQuery] %s\n", buffer);
	printf("[cancelQuery] size : %d\n", data);
#endif
	// SQLエラーのハンドリング
	if (isp.getErrorCode(buffer) != 0) {
		cerr << "[cancelQuery] ERROR occured, throw SQLException. code:" << isp.getErrorCode(buffer) << endl;
		throw InvalidArgumentException("ErrCd:" + to_string(isp.getErrorCode(buffer)) + ", ErrMsg: " + isp.getErrorMessage(buffer));
	}

	try {
		if (callBackMp.find(mngId) != callBackMp.end()) {
			callBackMp.erase(mngId);
		}
		else if(callBackClassMp.find(mngId) != callBackClassMp.end()){
			callBackClassMp.erase(mngId);
		}
		else {
			//プリセットクエリをキャンセルする際は、Mapに登録していないため、コメントアウト
			//cerr << "[cancelQuery] Not Found CallBack. MngId : " << mngId << endl;
		}
		
#if DEBUG == 1
		cout << "[cancelQuery] CallBack map Cancel MNGID : " << mngId << endl;
#endif
	}
	catch (std::out_of_range) {
		cerr << "[cancelQuery] Not Found CallBack. MngId : " << mngId << endl;
	}
}

/**
 * 接続断要求
 *
 * @author	Nagoya University
 * @date	2018/03/15
 */

void Connection::disconnect()
{
	//if (isTransportMode) {
		if (callBackMp.size() > 0 || callBackClassMp.size() > 0) {
			return;
		}
	//}
#if DEBUG == 1
	cout << "[disconnect] disconnect connection." << endl;
#endif
	if (ssl != NULL) {
		if (SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN) {
			SSL_set_shutdown(ssl, SSL_SENT_SHUTDOWN);
			SSL_shutdown(ssl);
		} else {
			SSL_clear(ssl);
		}
		SSL_free(ssl);
		ssl = NULL;
	}
	/* socketの終了 */
	if (sock != -1) {
		close(sock);
		sock = -1;
	}

	if (isTransportMode) {
		if (ssl2 != NULL) {
			if (SSL_get_shutdown(ssl2) & SSL_RECEIVED_SHUTDOWN) {
				SSL_set_shutdown(ssl2, SSL_SENT_SHUTDOWN);
				SSL_shutdown(ssl2);
			} else {
				SSL_clear(ssl2);
			}
			SSL_free(ssl2);
			ssl2 = NULL;
		}
		/* socketの終了 */
		if (sock2 != -1) {
			close(sock2);
			sock2 = -1;
		}
	}
	if (ctx != NULL) {
		SSL_CTX_free(ctx);
		ctx = NULL;
	}
}

/**
 * 接続断要求
 *
 * @author	Shinichi Kusayama
 * @date	2022/03/14
 */

void Connection::disconnect(bool doClear)
{
	if (doClear) {
		callBackMp.clear();
		callBackClassMp.clear();
	}
	disconnect();
}
/**
 * 再接続要求
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @exception	ConnectionFailedException	DBシステム接続失敗
 */

void Connection::reconnect()
{
	struct sockaddr_in addr;
	int sock;

	//if (isTransportMode) {
		if (callBackMp.size() > 0 || callBackClassMp.size() > 0) {
			return;
		}
	//}
	// 接続中の場合は一旦Close
	disconnect();

	/* ソケットの作成 */
	sock = socket(AF_INET, SOCK_STREAM, 0);

	/* 接続先指定用構造体の準備 */
	addr.sin_family = AF_INET;
	addr.sin_port = htons(this->port);
	addr.sin_addr.s_addr = inet_addr(this->ip.c_str());

	//タイムアウト時間設定
	struct timeval timeout;
	timeout.tv_sec = tcpSslTimeoutSec;
	timeout.tv_usec = 0;
	// TCPのタイムアウト設定
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		perror("[reconnect] setsockopt Error");
	}

	/* サーバに接続 */
#if DEBUG == 1
	printf("[reconnect] Server connect START\n");
	printf("[reconnect] Connect to %s:%d\n", this->ip.c_str(), this->port);
#endif
	if (connect(sock, (struct sockaddr*)&addr, sizeof(addr))) {
		printf("Faild Connect to %s:%d\n", this->ip.c_str(), this->port);
		throw ConnectionFailedException("[recconect] Faild Connect to " + this->ip + ":" + to_string(this->port));
	}
#if DEBUG == 1
	printf("[reconnect] Server Connect SUCCESS\n");
#endif
	

	if (isTransportMode) {
		int sock2;
		/* ソケットの作成 */
		sock2 = socket(AF_INET, SOCK_STREAM, 0);

		/* 接続先指定用構造体の準備 */
		addr.sin_family = AF_INET;
		addr.sin_port = htons(this->port2);
		addr.sin_addr.s_addr = inet_addr(this->ip.c_str());

		// TCPのタイムアウト設定
		if (setsockopt(sock2, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
			perror("[reconnect] setsockopt Error");
		}

		/* サーバに接続 */
		if (connect(sock2, (struct sockaddr*)&addr, sizeof(addr))) {
			printf("Faild Connect to %s:%d\n", this->ip.c_str(), this->port2);
			throw ConnectionFailedException("[recconect] Faild Connect to " + this->ip + ":" + to_string(this->port2));
		}
	}
	if (isSSL) {
		OpenSSL_add_all_algorithms();
		ERR_load_BIO_strings();
		ERR_load_crypto_strings();
		SSL_load_error_strings();
		if (SSL_library_init() < 0) {
			cerr << "[reconnect] Could not initialize the OpenSSL library !" << endl;
		}
		ctx = SSL_CTX_new(SSLv23_client_method());
		SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);

		ssl = SSL_new(ctx);
		SSL_set_fd(ssl, sock);
		SSL_connect(ssl);
		if (isTransportMode) {
			ssl2 = SSL_new(ctx);
			SSL_set_fd(ssl2, sock2);
			SSL_connect(ssl2);
		}
	}
#if DEBUG == 1
	printf("[reconnect] Server Connect SUCCESS\n");
#endif

	this->sock = sock;
	this->sock2 = sock2;
	this->addr = addr;

	return;
}

/**
 * 継続クエリの受信処理(UDP)
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	port	ポート番号
 */

void Connection::receiveData(const int port)
{
#if DEBUG == 1
	cout << "[receiveData] START. port : " << port << endl;
#endif
	struct timeval timeout;
	char buf[IPv4_UDP_MAX_BYTE * 10];
	string receiveData = "";
	int bufSize = 0;
	bool isExit = false;
	//タイムアウト時間設定
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	int Sock;
	if (isTransportMode) {
		Sock = this->sock2;
	} else {
		// UDPのタイムアウト設定
		Sock = socket(AF_INET, SOCK_DGRAM, 0);

		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = INADDR_ANY;

		if (setsockopt(Sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
			perror("[receiveData] setsockopt Error");
		}
		if (bind(Sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			perror("[receiveData] bind Error");
		}
	}
	std::unordered_map<string, vector<string>> recvDataMap;
	while(true)
	{
		receiveData = "";
		while (true) {
			if (callBackMp.size() == 0 && callBackClassMp.size() == 0)
			{
				// クラス内で管理している管理IDが消えていないか確認、消えていたらスレッド終了
#if DEBUG == 1
				cout << "[receiveData] All continuous query cleared. Receive Thread End" << endl;
#endif
				isExit = true;
				break;
			}

			memset(buf, 0, sizeof(buf));
			int len = 0;
			if (isTransportMode) {
				len = recvUsingHeader(Sock, buf, NULL, true, isSSL);
			} else {
				len = recvUsingHeader(Sock, buf, NULL, false, isSSL);
			}
			if (len < 0)
			{
				// timeout. so continue
				//cout << "timeout..." << endl;
				continue;
			}
			string bufStr = string(buf, len);
			//cout << "[receiveData] bufStr = [" << bufStr << "]" << << endl;
#if DEBUG == 1
			cout << "[receiveData] recvfrom : " << bufStr.length() << " bytes" << endl;
#endif
			if (isTransportMode) {
				if (bufStr == "") {
					continue;
				}
			}
			// データを統合
			dataIntegration(bufStr, recvDataMap, receiveData);
#if DEBUG == 1
				cout << "[receiveData] recvfrom after (addrlen == 0)" << endl;
#endif
			if (receiveData.length() != 0) break;
		}
		if (isExit) break;
#if DEBUG == 1
		//cout << "[receiveData] recvData(Integrated) : " << receiveData.length() << " bytes" << endl;
#endif
		ResultSet ret;
		unsigned int mngId = 0;
		try {
			IS::InformationSourceParser isp;
			isp.getResultSetBySAX(receiveData, ret);
			mngId = ret.getResultSetMetaData().getManagementId();
			ret.setReference(referenceMp[mngId]);
		}
		catch (...) {
			cerr << "[receiveData] Unexpected Exception. Can't parse QueryResult. So continue.";
			continue;
		}
		if (callBackMp.end() != callBackMp.find(mngId)) {

			// データが来た場合は結果をコールバック関数に非同期で返却
			thread receiveThread(callBackMp[mngId], ret);
			receiveThread.detach();
		} else if (callBackClassMp.end() != callBackClassMp.find(mngId)) {

			// データが来た場合は結果をコールバック関数に非同期で返却
			thread receiveThread(callBackClassMp[mngId], ret);
			receiveThread.detach();
		}

	}
	if (isTransportMode == false) close(Sock);
}

/**
* DTLS通信のセットアップを行う
*
* @author	Nagoya University
* @date	2019/07/31
*
* @return SSLコンテキスト
*/
SSL_CTX* Connection::setupDTLS()
{
	SSL_CTX *ctx;
	ctx = SSL_CTX_new(DTLSv1_2_server_method());
	/* We accept all ciphers, including NULL.
	* Not recommended beyond testing and debugging
	*/
	SSL_CTX_set_cipher_list(ctx, "ALL:NULL:eNULL:aNULL");
	SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_OFF);
	SSL_CTX_set_default_passwd_cb(ctx, this->password_cb);

	if (!SSL_CTX_use_certificate_file(ctx, this->certFilePath.c_str(), SSL_FILETYPE_PEM)) {
		cerr << "[setupDTLS] Failed loading file: " << this->certFilePath << " Please check if the file exists and the settings are correct." << endl;
	}
	if (!SSL_CTX_use_PrivateKey_file(ctx, this->privateKeyFilePath.c_str(), SSL_FILETYPE_PEM)) {
		cerr << "[setupDTLS] Failed loading file: " << this->privateKeyFilePath << " Please check if the file exists and the settings are correct." << endl;
	}
	if (!SSL_CTX_check_private_key(ctx))
		cerr << "[setupDTLS] Failed check file: " << this->privateKeyFilePath << endl;

	/* Client has to authenticate */
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, this->dtls_verify_cb);

	SSL_CTX_set_read_ahead(ctx, 1);
	SSL_CTX_set_cookie_generate_cb(ctx, this->generate_cookie_cb);
	SSL_CTX_set_cookie_verify_cb(ctx, this->verify_cookie_cb);
	return ctx;
}


/**
* エラーハンドリング関数
*
* @author	Nagoya University
* @date	2019/07/31
*
* @return	int
*/
int Connection::handle_socket_error() {
	switch (errno) {
	case EINTR:
		/* Interrupted system call.
		* Just ignore.
		*/
		cerr << "[handle_socket_error] Interrupted system call!" << endl;
		return 1;
	case EBADF:
		/* Invalid socket.
		* Must close connection.
		*/
		cerr << "[handle_socket_error] Invalid socket!" << endl;
		return 0;
		break;
#ifdef EHOSTDOWN
	case EHOSTDOWN:
		/* Host is down.
		* Just ignore, might be an attacker
		* sending fake ICMP messages.
		*/
		cerr << "[handle_socket_error] Host is down!" << endl;
		return 1;
#endif
#ifdef ECONNRESET
	case ECONNRESET:
		/* Connection reset by peer.
		* Just ignore, might be an attacker
		* sending fake ICMP messages.
		*/
		cerr << "[handle_socket_error] Connection reset by peer!" << endl;
		return 1;
#endif
	case ENOMEM:
		/* Out of memory.
		* Must close connection.
		*/
		cerr << "[handle_socket_error] Out of memory!" << endl;
		return 0;
		break;
	case EACCES:
		/* Permission denied.
		* Just ignore, we might be blocked
		* by some firewall policy. Try again
		* and hope for the best.
		*/
		cerr << "[handle_socket_error] Permission denied!" << endl;
		return 1;
		break;
	default:
		/* Something unexpected happened */
		cerr << "[handle_socket_error] Unexpected error! (errno =" + to_string(errno) + ")" << endl;
		return 0;
		break;
	}
	return 0;
}

/**
* DTLSの認証時コールバック関数
* コールバックにてパスコードを登録する際に利用
*
* @author	Nagoya University
* @date	2019/07/31
*
* @param	buf	バッファ
* @param	size	サイズ
* @param	rwflag	読み書きフラグ
* @param	u	ユーザデータ
*
* @return	パスワード長
*/
int Connection::password_cb(char *buf, int size, int rwflag, void *u) {

	const char *tmp = pemPass.c_str();
	if (tmp == NULL) /* An error occurred */
		return -1;

	size_t len = strlen(tmp);

	if (len > size)
		len = size;
	memcpy(buf, tmp, len);
	return len;
}

/**
* DTLSの認証時コールバック関数
*
* @author	Nagoya University
* @date	2019/07/31
*
* @param ok	OK
* @param ctx	X509_STORE_CTX
*
* @return int
*/
int Connection::dtls_verify_cb(int ok, X509_STORE_CTX *ctx) {
	/* This function should ask the user
	* if he trusts the received certificate.
	* Here we always trust.
	*/
	return 1;
}


/**
* Cookie生成時コールバック関数
*
* @author	Nagoya University
* @date	2019/07/31
*
* @param	ssl	SSLオブジェクト
* @param	cookie	クッキー
* @param	cookie_len	クッキー長
*
* @return	int
*/
int Connection::generate_cookie_cb(SSL *ssl, unsigned char *cookie, unsigned int *cookie_len) {

	unsigned char *buffer, result[EVP_MAX_MD_SIZE];
	unsigned int length = 0, resultlength;
	union {
		struct sockaddr_storage ss;
		struct sockaddr_in6 s6;
		struct sockaddr_in s4;
	} peer;

	if (!cookie_initialized)
	{
		if (!RAND_bytes(cookie_secret, COOKIE_SECRET_LENGTH))
		{
			cerr << "[generate_cookie_cb] error setting random cookie secret" << endl;
			return 0;
		}
		cookie_initialized = 1;
	}

	(void)BIO_dgram_get_peer(SSL_get_rbio(ssl), &peer);

	length = 0;
	switch (peer.ss.ss_family) {
	case AF_INET:
		length += sizeof(struct in_addr);
		break;
	case AF_INET6:
		length += sizeof(struct in6_addr);
		break;
	default:
		OPENSSL_assert(0);
		break;
	}
	length += sizeof(in_port_t);
	buffer = (unsigned char*)OPENSSL_malloc(length);

	if (buffer == NULL)
	{
		cerr << "[generate_cookie_cb] out of memory" << endl;
		return 0;
	}

	switch (peer.ss.ss_family) {
	case AF_INET:
		memcpy(buffer,
			&peer.s4.sin_port,
			sizeof(in_port_t));
		memcpy(buffer + sizeof(peer.s4.sin_port),
			&peer.s4.sin_addr,
			sizeof(struct in_addr));
		break;
	case AF_INET6:
		memcpy(buffer,
			&peer.s6.sin6_port,
			sizeof(in_port_t));
		memcpy(buffer + sizeof(in_port_t),
			&peer.s6.sin6_addr,
			sizeof(struct in6_addr));
		break;
	default:
		OPENSSL_assert(0);
		break;
	}

	HMAC(EVP_sha1(), (const void*)cookie_secret, COOKIE_SECRET_LENGTH,
		(const unsigned char*)buffer, length, result, &resultlength);
	OPENSSL_free(buffer);

	memcpy(cookie, result, resultlength);
	*cookie_len = resultlength;

	return 1;
}

/**
* Cookie確認時コールバック関数
*
* @author	Nagoya University
* @date	2019/07/31
*
* @param	ssl	SSLオブジェクト
* @param	cookie	クッキー
* @param	cookie_len	クッキー長
*
* @return	int
*/
int Connection::verify_cookie_cb(SSL *ssl, const unsigned char *cookie, unsigned int cookie_len) {
	unsigned char *buffer, result[EVP_MAX_MD_SIZE];
	unsigned int length = 0, resultlength;
	union {
		struct sockaddr_storage ss;
		struct sockaddr_in6 s6;
		struct sockaddr_in s4;
	} peer;

	if (!cookie_initialized)
		return 0;

	(void)BIO_dgram_get_peer(SSL_get_rbio(ssl), &peer);

	length = 0;
	switch (peer.ss.ss_family) {
	case AF_INET:
		length += sizeof(struct in_addr);
		break;
	case AF_INET6:
		length += sizeof(struct in6_addr);
		break;
	default:
		OPENSSL_assert(0);
		break;
	}
	length += sizeof(in_port_t);
	buffer = (unsigned char*)OPENSSL_malloc(length);

	if (buffer == NULL)
	{
		cerr << "[verify_cookie_cb] out of memory" << endl;
		return 0;
	}

	switch (peer.ss.ss_family) {
	case AF_INET:
		memcpy(buffer,
			&peer.s4.sin_port,
			sizeof(in_port_t));
		memcpy(buffer + sizeof(in_port_t),
			&peer.s4.sin_addr,
			sizeof(struct in_addr));
		break;
	case AF_INET6:
		memcpy(buffer,
			&peer.s6.sin6_port,
			sizeof(in_port_t));
		memcpy(buffer + sizeof(in_port_t),
			&peer.s6.sin6_addr,
			sizeof(struct in6_addr));
		break;
	default:
		OPENSSL_assert(0);
		break;
	}

	HMAC(EVP_sha1(), (const void*)cookie_secret, COOKIE_SECRET_LENGTH,
		(const unsigned char*)buffer, length, result, &resultlength);
	OPENSSL_free(buffer);

	if (cookie_len == resultlength && memcmp(result, cookie, resultlength) == 0)
		return 1;

	return 0;
}

/**
* 継続クエリの受信処理(DTLS)
*
* @author	Nagoya University
* @date	2019/08/06
*
* @param	port	ポート番号
*/

void Connection::receiveDTLSData(const int port)
{
#if DEBUG == 1
	cout << "[receiveDTLSData] START. port : " << port << endl;
#endif
	int fd;
	union {
		struct sockaddr_storage ss;
		struct sockaddr_in s4;
		struct sockaddr_in6 s6;
	} server_addr, client_addr;

	SSL *ssl;
	BIO *bio;
	struct timeval timeout;
	struct pass_info *info;
	const int on = 1, off = 0;

	SSL_CTX *ctx = setupDTLS();

	memset(&server_addr, 0, sizeof(struct sockaddr_storage));

	server_addr.s4.sin_family = AF_INET;
#ifdef HAVE_SIN_LEN
	server_addr.s4.sin_len = sizeof(struct sockaddr_in);
#endif
	server_addr.s4.sin_port = htons(port);

	fd = socket(server_addr.ss.ss_family, SOCK_DGRAM, 0);
	if (fd < 0) {
		perror("socket");
		exit(-1);
	}

	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&on, (socklen_t) sizeof(on));
#ifdef SO_REUSEPORT
	setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (const void*)&on, (socklen_t) sizeof(on));
#endif

	if (server_addr.ss.ss_family == AF_INET) {
		bind(fd, (const struct sockaddr *) &server_addr, sizeof(struct sockaddr_in));
	}
	else {
		setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&off, sizeof(off));
		bind(fd, (const struct sockaddr *) &server_addr, sizeof(struct sockaddr_in6));
	}

#if DEBUG == 1
	cout << "[receiveDTLSData]  START. port : " << std::to_string(port) << endl;
#endif

	while (1) {
		if (callBackMp.size() == 0 && callBackClassMp.size() == 0)
		{
			// クラス内で管理している管理IDが消えていないか確認、消えていたらスレッド終了
#if DEBUG == 1
			cout << "[receiveDTLSData] All continuous query cleared. Receive Thread End" << endl;
#endif
			break;
		}
		memset(&client_addr, 0, sizeof(struct sockaddr_storage));

		/* Create BIO */
		bio = BIO_new_dgram(fd, BIO_NOCLOSE);

		/* Set and activate timeouts */
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		BIO_ctrl(bio, BIO_CTRL_DGRAM_SET_RECV_TIMEOUT, 0, &timeout);

		ssl = SSL_new(ctx);
		SSL_set_bio(ssl, bio, bio);
		SSL_set_options(ssl, SSL_OP_COOKIE_EXCHANGE);

		while (DTLSv1_listen(ssl, (BIO_ADDR*)&client_addr) <= 0);

		info = (struct pass_info*) malloc(sizeof(struct pass_info));
		memcpy(&info->server_addr, &server_addr, sizeof(struct sockaddr_storage));
		memcpy(&info->client_addr, &client_addr, sizeof(struct sockaddr_storage));
		info->ssl = ssl;

		// ソケット処理は別スレッドに委譲
		thread queuingThread(&Connection::sockDTLSProcess, this, info);
		queuingThread.detach();

	}
}

/**
* DTLSにてacceptされた後の処理
*
* @author	Nagoya University
* @date	2019/07/31
*/
void Connection::sockDTLSProcess(void *info) {
	struct pass_info *pinfo = (struct pass_info*) info;
	ssize_t len;
	char buf[IPv4_UDP_MAX_BYTE * 10];
	int num_timeouts = 0, max_timeouts = dtlsTimeoutSec;
	string receiveData = "", bufStr = "";
	unordered_map<string, vector<string>> recvDataMap;

	SSL *ssl;
	int fd;
	if (isTransportMode) {
		ssl = this->ssl2;
	} else {
		ssl = pinfo->ssl;
		fd = acceptDTLS(pinfo);
		if (fd < 0) {
			goto cleanup;
		}
	}
	while (!(SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN) && num_timeouts < max_timeouts) {

		if (callBackMp.size() == 0 && callBackClassMp.size() == 0)
		{
			// クラス内で管理している管理IDが消えていないか確認、消えていたらスレッド終了
#if DEBUG == 1
			cout << "[sockDTLSProcess] All continuous query cleared. Receive Thread End" << endl;
#endif
			break;
		}
		receiveData = "";
		int sumLen = 0;
		// UDPの受信
		// 可変長でデータを受信する
		memset(buf, 0, sizeof(buf));
		if (isTransportMode) {
			len = recvUsingHeader(0, buf, ssl, true, isSSL);
		} else {
			len = recvUsingHeader(0, buf, ssl, false, isSSL);
		}
		switch (SSL_get_error(ssl, len)) {
		case SSL_ERROR_NONE:
			num_timeouts = 0;
#if DEBUG == 1
			cout << "[sockDTLSProcess] read " + to_string(len) + " bytes" << endl;
#endif
			break;
		case SSL_ERROR_WANT_READ:
			// Handle socket timeouts
			if (BIO_ctrl(SSL_get_rbio(ssl), BIO_CTRL_DGRAM_GET_RECV_TIMER_EXP, 0, NULL)) {
				num_timeouts++;
			}
			// Just try again
			break;
		case SSL_ERROR_ZERO_RETURN:
			break;
		case SSL_ERROR_SYSCALL:
			cerr << "[sockDTLSProcess] Socket read error" << endl;
			if (!handle_socket_error()) goto cleanup;
			break;
		case SSL_ERROR_SSL:
			cerr << "[sockDTLSProcess] SSL read error" << endl;
			goto cleanup;
			break;
		default:
			cerr << "[sockDTLSProcess] Unexpected error while reading!" << endl;
			goto cleanup;
			break;
		}

		if (len < 0)
		{
			continue;
		}
		bufStr = string(buf, len);

		// データを統合
		dataIntegration(bufStr, recvDataMap, receiveData);
		if (receiveData.length() == 0) continue;

#if MEASURE_MODE == 1
		cout << "[sockDTLSProcess] receive complete NowMicroSec:" << DmUtil::getTimeMicrosec() << endl;
#endif
#if DEBUG == 1
		cout << "[sockDTLSProcess] recvData(Integrated) : " << receiveData.length() << " bytes" << endl;
#endif
		ResultSet ret;
		unsigned int mngId = 0;
		try {
			IS::InformationSourceParser isp;
			isp.getResultSetBySAX(receiveData, ret);
			mngId = ret.getResultSetMetaData().getManagementId();
			ret.setReference(referenceMp[mngId]);
		}
		catch (...) {
			cerr << "[sockDTLSProcess] Unexpected Exception. Can't parse QueryResult. So continue.";
			continue;
		}
		if (callBackMp.end() != callBackMp.find(mngId)) {

			// データが来た場合は結果をコールバック関数に非同期で返却
			thread receiveThread(callBackMp[mngId], ret);
			receiveThread.detach();
		} else if (callBackClassMp.end() != callBackClassMp.find(mngId)) {

			// データが来た場合は結果をコールバック関数に非同期で返却
			thread receiveThread(callBackClassMp[mngId], ret);
			receiveThread.detach();
		}
	}
	SSL_shutdown(ssl);
cleanup:
	if (isTransportMode == false) {
		close(fd);

		SSL_free(ssl);
		ERR_remove_state(0);
	}
	free(info);

#if DEBUG == 1
	cout << "[sockDTLSProcess] Thread done, connection closed." << endl;
#endif

	pthread_exit((void *)NULL);

}

/**
* 受信データを統合する
*
* @author	Nagoya University
* @date	2018/03/15
*
* @param [in,out]	buf	受信文字列
* @param [in,out]	recvDataMap	受信途中のデータマップ
* @param [in,out]	result	統合後の受信データ(全て受信していない場合は空文字を返却）
*
*/

inline void Connection::dataIntegration(string &buf, std::unordered_map<string, vector<string>> &recvDataMap, string &result)
{
	int flagment = 0, flagmentMax = 0;
	string key;

	// ヘッダ情報を取得する
	stringUtil.getHeaderInfo(buf, key, flagment, flagmentMax);
#if DEBUG == 1
	cout << "[dataIntegration] UDP recv headerInfo  key:" << key << " flagment:" << flagment << " max:" << flagmentMax << endl;
#endif
	// resultMapに受信途中がないかチェック
	auto itr = recvDataMap.find(key);
	if (itr != recvDataMap.end()) {
		// 受信途中が存在する
//		cout << "[dataIntegration] flagment:" << flagment << endl;
		recvDataMap[key].at(flagment) = buf;
	}
	else {
		// 新規に受信
		vector<string> data(flagmentMax);
//		cout << "[dataIntegration] flagment:" << flagment << endl;
		data.at(flagment) = buf;
		recvDataMap[key] = data;
	}

	bool recvFinish = true;
	for (string str : recvDataMap[key]) {
//		cout << "[dataIntegration] str:" << str << endl;
		// 用意した配列に全てデータが入っていない場合は未受信データあり
		if (str.length() == 0) recvFinish = false;
	}
//	cout << "[dataIntegration] " << recvFinish << endl;

	if (recvFinish) {
		// 全てのデータを受信出来たらデータを繋ぎ合わせてMapは削除
		for (string str : recvDataMap[key]) {
			result.append(str);
		}
		recvDataMap.erase(key);
	}
}

/**
* 展開処理
*
* @author	Shinichi Kusayama
* @date	2022/05/10
*
* @param	inBuf	ソケット
* @param	outBuf	受信データ
* @param	ssl	sslコネクション
*
* @return	展開に失敗した場合は-1を、成功した場合はサイズを返す
*/
int Connection::recvUsingHeader(int sock, char *outBuf, SSL *ssl, bool isTcp, bool isSSL)
{
	char bufComp[IPv4_UDP_MAX_BYTE * 10];
	int len = 0;
	int pre_len = 0;
	memset(bufComp, 0, sizeof(bufComp));
	char bufTmp[IPv4_UDP_MAX_BYTE * 10];
	// for UDP(recvfrom)
	struct sockaddr_in from;
	socklen_t addrlen;
	addrlen = sizeof(struct sockaddr_in);

	int maxCompSize = SEND_BUF_SIZE;

	bool isFirst = true;
	struct IsHeaderInfo headerInfo;
	while (true) {
		memset(bufTmp, 0, sizeof(bufTmp));
		int now_len;
		char *bufTmp_p = bufTmp;
		if (isTcp) {
			if (isSSL) {
				now_len = SSL_read(ssl, bufTmp, sizeof(bufTmp));
			} else {
				now_len = recv(sock, bufTmp, maxCompSize, 0);
			}
		} else {
			if (isSSL) {
				now_len = SSL_read(ssl, bufTmp, sizeof(bufTmp));
			} else {
				now_len = recvfrom(sock, bufTmp, sizeof(bufTmp), 0, (struct sockaddr *)&from, &addrlen);
			}
		}
		if (addrlen == 0 || now_len <= 0) {
			return -1;		// undefined address
		}
		if (isFirst && isTcp == false) {
			// ヘッダ＋圧縮データとなっているペイロードに対して、ヘッダの箇所のみ抽出して各変数へ格納
			stringUtil.getIsHeader(bufTmp_p, headerInfo);
			now_len -= headerInfo.headerSize;
			bufTmp_p = headerInfo.payload_p;
			if (headerInfo.header.compressFlg != '1' && headerInfo.header.compressFlg != '2') {
				if (headerInfo.header.compressFlg == '0') {
					memcpy(outBuf, bufTmp_p, now_len);
				} else {
					memcpy(outBuf, bufTmp, now_len);
				}
				return now_len;
			}
			if (headerInfo.header.compressFlg == '1') {
				maxCompSize -= headerInfo.headerSize;
			} else if (headerInfo.header.compressFlg == '2') {
				maxCompSize = headerInfo.header.length;
			}
#if DEBUG == 1
			cout << "[recvUsingHeader] CompressFlg:" << headerInfo.header.compressFlg << endl;
			cout << "[recvUsingHeader] length:" << headerInfo.header.length << endl;
			cout << "[recvUsingHeader] key:" << headerInfo.header.key << endl;
			cout << "[recvUsingHeader] seq:" << headerInfo.header.seq << endl;
			cout << "[recvUsingHeader] now_len:" << now_len << endl;
			cout << "[recvUsingHeader] maxCompSize:" << maxCompSize << endl;
#endif
			isFirst = false;
		}
		//cout << "[recvUsingHeader] now_len:" << now_len << ", pre_len:" << pre_len << endl;
		if (now_len <= 0 || now_len > maxCompSize) {
			len = now_len;
			break;
		}
		if (now_len == maxCompSize) {
			// 送信されたサイズを分割されずに全て受信できた場合
			memcpy(bufComp, bufTmp_p, now_len);
			len = now_len;
			break;
		} else if (pre_len > 0) {
			// 分割送信の２回目以降
			if (pre_len + now_len == maxCompSize) {
				// 分割されたペイロードが全て到着
				memcpy(&bufComp[pre_len], bufTmp_p, now_len);
				len = maxCompSize;
				break;
			} else if (pre_len + now_len < maxCompSize) {
				// 分割されたペイロードの一部が到着
				memcpy(&bufComp[pre_len], bufTmp_p, now_len);
				pre_len += now_len;
			} else {
				// 分割されたペイロードの総数が想定サイズ以上となった異常ケース
				break;
			}
		} else {
			// 分割送信の１回目
			pre_len = now_len;
			memcpy(bufComp, bufTmp_p, now_len);
		}
	}
	if (len != maxCompSize) {
#if DEBUG == 1
		cerr << "BufferSize Not " << SEND_BUF_SIZE << " length:" + to_string(len) << endl;
#endif
		return -1;
	}
	if (headerInfo.header.compressFlg == '1' || isTcp) {
		len = stringUtil.decompress(bufComp, outBuf);
	} else if (headerInfo.header.compressFlg == '2') {
		int outSize;
		if (stringUtil.decompressUsingZstd(bufComp, outBuf, maxCompSize, &outSize)) {
			len = outSize;
		} else {
			return -1;
		}
	}

	return len;
}
/**
* accept処理 (DTLS用)
*
* @author	Shinichi Kusayama
* @date	2022/05/10
*
* @param	info	引き渡し用構造体
*
* @return	生成したファイルディスクリプタを返す. 失敗した場合は負数を返す
*/
int Connection::acceptDTLS(struct pass_info* info) {
	struct pass_info *pinfo = (struct pass_info*) info;
	int fd, ret;
	const int on = 1, off = 0;
	struct timeval timeout;

	OPENSSL_assert(pinfo->client_addr.ss.ss_family == pinfo->server_addr.ss.ss_family);
	fd = socket(pinfo->client_addr.ss.ss_family, SOCK_DGRAM, 0);
	if (fd < 0) {
		perror("[sockDTLSProcess] socket error");
		return fd;
	}

	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&on, (socklen_t) sizeof(on));
#ifdef SO_REUSEPORT
	setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (const void*)&on, (socklen_t) sizeof(on));
#endif
	switch (pinfo->client_addr.ss.ss_family) {
	case AF_INET:
		bind(fd, (const struct sockaddr *) &pinfo->server_addr, sizeof(struct sockaddr_in));
		connect(fd, (struct sockaddr *) &pinfo->client_addr, sizeof(struct sockaddr_in));
		break;
	case AF_INET6:
		setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&off, sizeof(off));
		bind(fd, (const struct sockaddr *) &pinfo->server_addr, sizeof(struct sockaddr_in6));
		connect(fd, (struct sockaddr *) &pinfo->client_addr, sizeof(struct sockaddr_in6));
		break;
	default:
		OPENSSL_assert(0);
		break;
	}

	/* Set new fd and set BIO to connected */
	BIO_set_fd(SSL_get_rbio(pinfo->ssl), fd, BIO_NOCLOSE);
	BIO_ctrl(SSL_get_rbio(pinfo->ssl), BIO_CTRL_DGRAM_SET_CONNECTED, 0, &pinfo->client_addr.ss);

	/* Finish handshake */
	do { ret = SSL_accept(pinfo->ssl); } while (ret == 0);
	if (ret < 0) {
		perror("[sockDTLSProcess] SSL_accept error");
		return ret;
	}

	/* Set and activate timeouts */
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	BIO_ctrl(SSL_get_rbio(pinfo->ssl), BIO_CTRL_DGRAM_SET_RECV_TIMEOUT, 0, &timeout);
	return fd;
}
