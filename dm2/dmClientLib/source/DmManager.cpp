#include "is/DmManager.h"

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

using namespace std;

const string IP = "127.0.0.1";
const int TCP_PORT = 9001;
const int UDP_PORT = 9002;
const int SSL_PORT = 11001;
const int DTLS_PORT= 11002;
const int BUF_MAX = 4096;

map<string, string> DmManager::sessionMap;
string DmManager::certFilePath = "";
string DmManager::privateKeyFilePath = "";
string DmManager::pemPass = "";
bool DmManager::isSSL = false;
int DmManager::tcpSslTimeoutSec = 60;
int DmManager::dtlsTimeoutSec = 60 * 60;

/**
 * コンストラクタ
 *
 * @author	Nagoya University
 * @date	2018/03/15
 */

DmManager::DmManager()
{
	sessionMap.clear();
}

/**
 * デストラクタ
 *
 * @author	Nagoya University
 * @date	2018/03/15
 */

DmManager::~DmManager()
{
}

/**
* DTLS通信のサーバ側処理(継続クエリの受信)に必要な値の設定とSSL通信の前処理を実施
*
* @author	Nagoya University
* @date	2019/08/08
*
* @param	inCertFilePath	  	サーバ証明書ファイルの絶対パス指定
* @param	inPrivateKeyFilePath	プライベートキーファイルの絶対パス指定
* @param	inPemPass				証明書のパスフレーズ
*/
void DmManager::initEncryptionSettings(const string &inCertFilePath, const string &inPrivateKeyFilePath, const string &inPemPass)
{
	if (inCertFilePath.length() == 0 || inPrivateKeyFilePath.length() == 0 || inPemPass.length() == 0) {
		cerr << "[initEncryptionSettings] Argument is invalid." << endl;
		return;
	}
	isSSL = true;
	certFilePath = inCertFilePath;
	privateKeyFilePath = inPrivateKeyFilePath;
	pemPass = inPemPass;

	OpenSSL_add_all_algorithms();
	ERR_load_BIO_strings();
	ERR_load_crypto_strings();
	SSL_load_error_strings();

	if (SSL_library_init() < 0) {
		cerr << "[initEncryptionSettings] Could not initialize the OpenSSL library !" << endl;
	}

}

/**
* TCP/SSLソケット生成時のタイムアウト値を設定する(本APIをコールしない場合はデフォルト値を設定)
*
* @author	Nagoya University
* @date	2019/09/03
*
* @param	TcpSslTimeoutSec	TCP/SSLタイムアウト秒
*/
void DmManager::initTcpSslTimeoutSetting(const int TcpSslTimeoutSec)
{
	tcpSslTimeoutSec = TcpSslTimeoutSec;
}

/**
* DTLS認証タイムアウト値を設定する(本APIをコールしない場合はデフォルト値を設定)
*
* @author	Nagoya University
* @date	2019/09/03
*
* @param	DtlsTimeoutSec	DTLSタイムアウト秒
*/
void DmManager::initDtlsTimeoutSetting(const int DtlsTimeoutSec)
{
	dtlsTimeoutSec = DtlsTimeoutSec;
}

/**
 * DBシステムとのコネクションを取得
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @exception	ConnectionFailedException	DBシステム接続失敗
 *
 * @param	ip  	IPアドレス
 * @param	port	ポート番号
 * @param	username	ユーザ名
 * @param	pass	パスワード
 *
 * @return	コネクションオブジェクト
 */

Connection* DmManager::getConnection(const string &ip, const int port, const string &username, const string &pass)
{
	return getConnection(ip, port, username, pass, false);
}
/**
 * DBシステムとのコネクションを取得
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @exception	ConnectionFailedException	DBシステム接続失敗
 *
 * @param	ip  	IPアドレス
 * @param	port	ポート番号
 * @param	username	ユーザ名
 * @param	pass	パスワード
 * @param	isTransportMode	TCP/UDP
 *
 * @return	コネクションオブジェクト
 */

Connection* DmManager::getConnection(const string &ip, const int port, const string &username, const string &pass, bool isTransportMode)
{
	struct sockaddr_in addr;
	int sock = connectSock(ip, port, addr);
	// セッションキーの確認
	if (sessionMap.count(username + "_" + ip) == 0) {
		try {
			createSession(sock, ip, username, pass);
		}
		catch (AuthorityException &e) {
			close(sock);
			throw;
		}
		catch (ConnectionTimeoutException &e) {
			close(sock);
			throw;
		}
		catch (ConnectionFailedException &e) {
			close(sock);
			throw;
		}
	}
#if MEASURE_MODE == 1
	long startTime = DmUtil::getTimeMicrosec();
#endif
	
	Connection *con = new Connection(ip, port, sock, addr, sessionMap[username + "_" + ip], tcpSslTimeoutSec, isTransportMode);
#if MEASURE_MODE == 1
	long now = DmUtil::getTimeMicrosec();
	double msec = (now - startTime) / 1000.0;
	cout << "[STAT] getConnection processTime(msec):" << msec << endl;
#endif
	return con;
}

/**
* DBシステムとのSSLコネクションを取得
*
* @author	Nagoya University
* @date	2019/07/30
*
* @exception	ConnectionFailedException	DBシステム接続失敗
*
* @param	ip  	IPアドレス
* @param	port	ポート番号
* @param	username	ユーザ名
* @param	pass	パスワード
*
* @return	コネクションオブジェクト
*/

Connection* DmManager::getSSLConnection(const string &ip, const int port, const string &username, const string &pass)
{
	return getSSLConnection(ip, port, username, pass, false);
}

/**
* DBシステムとのSSLコネクションを取得
*
* @author	Nagoya University
* @date	2019/07/30
*
* @exception	ConnectionFailedException	DBシステム接続失敗
*
* @param	ip  	IPアドレス
* @param	port	ポート番号
* @param	username	ユーザ名
* @param	pass	パスワード
* @param	isTransportMode	TCP/UDP
*
* @return	コネクションオブジェクト
*/

Connection* DmManager::getSSLConnection(const string &ip, const int port, const string &username, const string &pass, bool isTransportMode)
{
	int sock;
	SSL *ssl = NULL;
	SSL_CTX *ctx = NULL;
	struct sockaddr_in addr;

	// セッションキーの確認
	if (sessionMap.count(username + "_" + ip) == 0) {
		ssl = connectSSLSock(sock, ip, port, addr, ctx);
		try {
			createSession(ssl, ip, username, pass);
		}
		catch (AuthorityException &e) {
			SSL_free(ssl);
			close(sock);
			SSL_CTX_free(ctx);;
			throw;
		}
		catch (ConnectionTimeoutException &e) {
			SSL_free(ssl);
			close(sock);
			SSL_CTX_free(ctx);
			throw;
		}
		catch (ConnectionFailedException &e) {
			SSL_free(ssl);
			close(sock);
			SSL_CTX_free(ctx);
			throw;
		}
	}
#if MEASURE_MODE == 1
	long startTime = DmUtil::getTimeMicrosec();
#endif

	Connection *con = new Connection(ip, port, sock, addr, sessionMap[username + "_" + ip], ssl, ctx, certFilePath, privateKeyFilePath, pemPass, tcpSslTimeoutSec, dtlsTimeoutSec, isTransportMode);
#if MEASURE_MODE == 1
	long now = DmUtil::getTimeMicrosec();
	double msec = (now - startTime) / 1000.0;
	cout << "[STAT] getConnection processTime(msec):" << msec << endl;
#endif
	return con;
}

/**
 * DBシステムとのコネクションを取得(IP、ポート指定なし)
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	username	ユーザ名
 * @param	pass	パスワード
 *
 * @return	コネクションオブジェクト
 */

Connection* DmManager::getDBConnection(const string &username, const string &pass)
{
	return getConnection(IP, TCP_PORT, username, pass);
}

/**
 * DBシステムとのコネクションを取得(IP、ポート指定)
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	ip  	IPアドレス
 * @param	port	ポート番号
 * @param	username	ユーザ名
 * @param	pass	パスワード
 *
 * @return	コネクションオブジェクト
 */

Connection* DmManager::getDBConnection(const string &ip, const int port, const string &username, const string &pass)
{
	if(!isSSL)
		return getConnection(ip, port, username, pass);
	else
		return getSSLConnection(ip, port, username, pass);
}

/**
 * DBシステムとのコネクションを取得(IP、ポート指定)
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	ip  	IPアドレス
 * @param	port	ポート番号
 * @param	username	ユーザ名
 * @param	pass	パスワード
 * @param	isTransportMode	TCP/UDP
 *
 * @return	コネクションオブジェクト
 */

Connection* DmManager::getDBConnection(const string &ip, const int port, const string &username, const string &pass, bool isTransportMode)
{
	if(!isSSL)
		return getConnection(ip, port, username, pass, isTransportMode);
	else
		return getSSLConnection(ip, port, username, pass, isTransportMode);
}
/**
 * データグラムソケットの取得（IP、ポート指定なし）
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	username	ユーザ名
 * @param	pass	パスワード
 *
 * @return	データグラムソケットオブジェクト
 */

DatagramSocket* DmManager::getDatagramSocket(const string &username, const string &pass)
{	
	return getDatagramSocket(IP, UDP_PORT, username, pass);
}

/**
 * データグラムソケットの取得（IP、ポート指定）
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	ip  	IPアドレス
 * @param	port	ポート番号
 * @param	username	ユーザ名
 * @param	pass	パスワード
 *
 * @return	データグラムソケットオブジェクト
 */

DatagramSocket* DmManager::getDatagramSocket(const string &ip, const int port, const string &username, const string &pass)
{
	return getDatagramSocket(ip, port, username, pass, false);
}
/**
 * データグラムソケットの取得（IP、ポート指定）
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	ip  	IPアドレス
 * @param	port	ポート番号
 * @param	username	ユーザ名
 * @param	pass	パスワード
 * @param	isTransportMode	TCP/UDP
 *
 * @return	データグラムソケットオブジェクト
 */

DatagramSocket* DmManager::getDatagramSocket(const string &ip, const int port, const string &username, const string &pass, const bool isTransportMode)
{
	DatagramSocket *ds = NULL;
	struct sockaddr_in addr;

	// セッションキーの確認
	if (sessionMap.count(username + "_" + ip) == 0) {

		if (!isSSL) {
			int sock = connectSock(ip, TCP_PORT, addr);
			try {
				createSession(sock, ip, username, pass);
			}
			catch (AuthorityException &e) {
				close(sock);
				throw;
			}
			catch (ConnectionTimeoutException &e) {
				close(sock);
				throw;
			}
			catch (ConnectionFailedException &e) {
				close(sock);
				throw;
			}
			close(sock);
		}
		else {
			int sock;
			int sock2 = 0;
			SSL *ssl = NULL;
			SSL_CTX *ctx = NULL;
			ssl = connectSSLSock(sock, ip, SSL_PORT, addr, ctx);
			try {
				createSession(ssl, ip, username, pass);
			}
			catch (AuthorityException &e) {
				if (ssl != NULL) SSL_free(ssl);
				close(sock);
				if (ctx != NULL) SSL_CTX_free(ctx);
				throw;
			}
			catch (ConnectionTimeoutException &e) {
				if (ssl != NULL) SSL_free(ssl);
				close(sock);
				if (ctx != NULL) SSL_CTX_free(ctx);
				throw;
			}
			catch (ConnectionFailedException &e) {
				if (ssl != NULL) SSL_free(ssl);
				close(sock);
				if (ctx != NULL) SSL_CTX_free(ctx);
				throw;
			}
			if (ssl != NULL) SSL_shutdown(ssl);
			if (ssl != NULL) SSL_free(ssl);
			close(sock);
			if (ctx != NULL) SSL_CTX_free(ctx);
		}
	}
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip.c_str());

	// DatagramSocketオブジェクトの生成
	if (!isSSL) {
		ds = new DatagramSocket(ip, port, sock, addr, sessionMap[username + "_" + ip]);
	}
	else {
		// DTLSの手続き処理
		SSL *ssl;
		SSL_CTX *ctx;
		BIO *bio;
		struct timeval timeout;
		
		ctx = SSL_CTX_new(DTLSv1_2_client_method());
		ssl = SSL_new(ctx);
		bio = BIO_new_dgram(sock, BIO_CLOSE);
		if (connect(sock, (struct sockaddr *) &addr, sizeof(addr))) {
			printf("[getDatagramSocket] Faild Connect to %s:%d\n", ip.c_str(), port);
			throw ConnectionFailedException("[getDatagramSocket] Faild Connect to " + ip + ":" + to_string(port));
		}
		BIO_ctrl(bio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &addr);
		SSL_set_bio(ssl, bio, bio);

		if (SSL_connect(ssl) < 0) {
			char buf[BUF_MAX];
			perror("[getDatagramSocket] SSL_connect");
			cerr << ERR_error_string(ERR_get_error(), buf) << endl;
			cerr << ERR_reason_error_string(ERR_get_error()) << endl;
			exit(-1);
		}
		/* Set and activate timeouts */
		timeout.tv_sec = dtlsTimeoutSec;
		timeout.tv_usec = 0;
		BIO_ctrl(bio, BIO_CTRL_DGRAM_SET_RECV_TIMEOUT, 0, &timeout);

		ds = new DatagramSocket(ip, port, sock, addr, sessionMap[username + "_" + ip], ssl, ctx);
	}
	return ds;
}

/**
* 接続確立
*
* @author	Nagoya University
* @date	2019/07/11
*
* @param	ip  	IPアドレス
* @param	port	ポート番号
* @param	addr	アドレス情報
*
* @return	ソケット
*/
int DmManager::connectSock(const string &ip, const int port, struct sockaddr_in &addr) {
	int sock;

	/* ソケットの作成 */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	/* 接続先指定用構造体の準備 */
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip.c_str());

	//タイムアウト時間設定
	struct timeval timeout;
	timeout.tv_sec = tcpSslTimeoutSec;
	timeout.tv_usec = 0;
	// TCPのタイムアウト設定
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		perror("[connectSock] setsockopt Error");
	}

	/* サーバに接続 */
#if DEBUG == 1
	printf("[connectSock] Server connect START\n");
	printf("[connectSock] Connect to %s:%d\n", ip.c_str(), port);
#endif
	if (connect(sock, (struct sockaddr*)&addr, sizeof(addr))) {
		printf("[connectSock] Faild Connect to %s:%d\n", ip.c_str(), port);
		throw ConnectionFailedException("[connectSock] Faild Connect to " + ip + ":" + to_string(port));
	}
#if DEBUG == 1
	printf("[connectSock] Server Connect SUCCESS sock %d\n", sock);
#endif
	return sock;
}

/**
* 接続確立
*
* @author	Nagoya University
* @date	2019/07/31
*
* @param	ip  	IPアドレス
* @param	port	ポート番号
* @param	addr	アドレス情報
*
* @return	ソケット
*/
SSL* DmManager::connectSSLSock(int &sock, const string &ip, const int port, struct sockaddr_in &addr, SSL_CTX *ctx) {
	SSL *ssl;
	if (ctx == NULL) {
		ctx = SSL_CTX_new(SSLv23_client_method());
		SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
	}

	/* ソケットの作成 */
	sock = socket(AF_INET, SOCK_STREAM, 0);

	/* 接続先指定用構造体の準備 */
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip.c_str());

	//タイムアウト時間設定
	struct timeval timeout;
	timeout.tv_sec = tcpSslTimeoutSec;
	timeout.tv_usec = 0;
	// TCPのタイムアウト設定
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		perror("[connectSSLSock] setsockopt Error");
	}

	/* サーバに接続 */
#if DEBUG == 1
	printf("[connectSSLSock] Server connect START\n");
	printf("[connectSSLSock] Connect to %s:%d\n", ip.c_str(), port);
#endif
	if (connect(sock, (struct sockaddr*)&addr, sizeof(addr))) {
		printf("[connectSSLSock] Faild Connect to %s:%d\n", ip.c_str(), port);
		throw ConnectionFailedException("[connectSSLSock] Faild Connect to " + ip + ":" + to_string(port));
	}
	ssl = SSL_new(ctx);
	SSL_set_fd(ssl, sock);
	if (SSL_connect(ssl) < 0) {
		perror("[connectSSLSock] SSL_connect");
		exit(-1);
	}
#if DEBUG == 1
	printf("[connectSSLSock] Server Connect SUCCESS\n");
#endif
	return ssl;
}

/**
* セッションキーを取得する
*
* @author	Nagoya University
* @date	2019/07/11
*
* @param	sock  	ソケット
* @param	ip		IPアドレス
* @param	username  	ユーザ名
* @param	pass	パスワード
*
* @return	セッションキー取得成功可否
*/
bool DmManager::createSession(int &sock, const string &ip, const string &username, const string &pass) {
	char buffer[BUF_MAX];
	int data;
	IS::InformationSourceParser isp;
	IS::StringUtil stringUtil;

	// DBシステムに対して継続クエリのキャンセル要求を投げる
	string createSession;
	isp.createSessionXML(username, getMD5(pass), DmUtil::getPID(), createSession);
#if DEBUG == 1
	cout << "[createSession] ---------------- REQUEST XML ----------------" << endl;
	cout << "[createSession] " << createSession << endl;
#endif
	int ret = send(sock, createSession.c_str(), createSession.length(), 0);
#if DEBUG == 1
	cout << "[createSession] dataSize: " << createSession.length() << " retSize : " << ret << endl;
#endif
	if (ret < 0) {
		string errMsg = "[createSession] TCP Connection Timeout";
		cout << errMsg << endl;
		throw ConnectionTimeoutException(errMsg);
	}

	/* サーバからデータを受信 */
	memset(buffer, 0, sizeof(buffer));

	data = read(sock, buffer, sizeof(buffer));
	if (data < 0) {
		string errMsg = "[createSession] recv TCP Connection Timeout";
		cout << errMsg << endl;
		throw ConnectionTimeoutException(errMsg);
	} else if (data == 0) {
		string errMsg = "[createSession] Connection is Closed";
		cout << errMsg << endl;
		throw ConnectionFailedException(errMsg);
    }
#if DEBUG == 1
	/*受信したデータを表示*/
	cout << "[createSession] ---------------- RESULT RECV ----------------" << endl;
	printf("[createSession] %s\n", buffer);
	printf("[createSession] size : %d\n", data);
#endif
	// エラーのハンドリング
	if (isp.getErrorCode(buffer) != 0) {
		cerr << "[createSession] Authentication error, Please confirm. user:" << username << " pass:" << pass << " msg:" << isp.getErrorMessage(buffer) << endl;
		string errMsg = isp.getErrorMessage(buffer);
		throw AuthorityException(errMsg);
		return false;
	}

	// sessionKey取得
	string sessionKey = isp.getSessionKey(buffer);
	if (sessionKey.length() == 0) {
		cerr << "[createSession] Authentication error, Please confirm. user:" << username << " pass:" << pass << endl;
		string errMsg = isp.getErrorMessage(buffer);
		throw AuthorityException(errMsg);
		return false;
	}
	// Mapに保持
	sessionMap[username + "_" + ip] = sessionKey;

	return true;
}

/**
* セッションキーを取得する
*
* @author	Nagoya University
* @date	2019/07/11
*
* @param	sock  	ソケット
* @param	ip		IPアドレス
* @param	username  	ユーザ名
* @param	pass	パスワード
*
* @return	セッションキー取得成功可否
*/
bool DmManager::createSession(SSL* ssl, const string &ip, const string &username, const string &pass) {
	char buffer[BUF_MAX];
	int data;
	IS::InformationSourceParser isp;
	IS::StringUtil stringUtil;

	// DBシステムに対して継続クエリのキャンセル要求を投げる
	string createSession;
	isp.createSessionXML(username, getMD5(pass), DmUtil::getPID(), createSession);
#if DEBUG == 1
	cout << "[createSession(SSL)] ---------------- REQUEST XML ---------------- " << endl;
	cout << "[createSession(SSL)] " << createSession << endl;
#endif
	int ret = SSL_write(ssl, createSession.c_str(), createSession.length());
#if DEBUG == 1
	cout << "[createSession(SSL)] dataSize: " << createSession.length() << " retSize : " << ret << endl;
#endif
	if (ret < 0) {
		string errMsg = "[createSession(SSL)] TCP Connection Timeout";
		cout << errMsg << endl;
		throw ConnectionTimeoutException(errMsg);
	}

	/* サーバからデータを受信 */
	memset(buffer, 0, sizeof(buffer));

	data = SSL_read(ssl, buffer, sizeof(buffer));
	if (data < 0) {
		string errMsg = "[createSession(SSL)]  recv TCP Connection Timeout";
		cout << errMsg << endl;
		throw ConnectionTimeoutException(errMsg);
	} else if (data == 0) {
		string errMsg = "[createSession] Connection is Closed";
		cout << errMsg << endl;
		throw ConnectionFailedException(errMsg);
    }
#if DEBUG == 1
	/*受信したデータを表示*/
	cout << "[createSession(SSL)] ---------------- RESULT RECV ----------------" << endl;
	printf("[createSession(SSL)] %s\n", buffer);
	printf("[createSession(SSL)] size : %d\n", data);
#endif
	// エラーのハンドリング
	if (isp.getErrorCode(buffer) != 0) {
		cerr << "[createSession(SSL)] Authentication error, Please confirm. user:" << username << " pass:" << pass << endl;
		string errMsg = isp.getErrorMessage(buffer);
		throw AuthorityException(errMsg);
		return false;
	}

	// sessionKey取得
	string sessionKey = isp.getSessionKey(buffer);
	if (sessionKey.length() == 0) {
		cerr << "[createSession(SSL)] Authentication error, Please confirm. user:" << username << " pass:" << pass << endl;
		string errMsg = isp.getErrorMessage(buffer);
		throw AuthorityException(errMsg);
		return false;
	}
	// Mapに保持
	sessionMap[username + "_" + ip] = sessionKey;

	return true;
}

/**
* セッションキーを削除する
*
* @author	Shinichi Kusayama
* @date	2022/03/14
*
*/
void DmManager::clearSession() {
	sessionMap.clear();
}

/**
* PASS文字列からMD5ハッシュを取得する
*
* @author	Nagoya University
* @date	2019/07/19
*
* @param	pass	pass文字列
* @return	hash
*
*/

string DmManager::getMD5(const string &pass)
{
	MD5_CTX c;
	unsigned char md[MD5_DIGEST_LENGTH];
	char mdString[33];
	int r;

	r = MD5_Init(&c);
	if (r != 1) {
		cout << "[getMD5] init error" << endl;
	}

	r = MD5_Update(&c, pass.c_str(), strlen(pass.c_str()));
	if (r != 1) {
		cout << "[getMD5] update error" << endl;
	}

	r = MD5_Final(md, &c);
	if (r != 1) {
		cout << "[getMD5] final error" << endl;
	}

	for (int i = 0; i < 16; i++)
		sprintf(&mdString[i * 2], "%02x", (unsigned int)md[i]);
#if DEBUG == 1
	printf("md5 digest: %s\n", mdString);
#endif
	
	string md5 = mdString;
	return md5;
}
