#ifndef CONNECTION_H
#define CONNECTION_H

#include "is/ResultSet.h"
#include "is/InformationSourceParser.h"
#include "is/StringUtil.h"
#include "is/DmException.h"
#include "is/CallBackListener.h"

#include <algorithm>
#include <thread>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <functional>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <zstd.h>

#define COOKIE_SECRET_LENGTH 16

using namespace std;

typedef void(*FUNC_CALLBACK)(ResultSet);

/**
 * DBシステム接続状態時の処理を提供するクラス
 *
 * @author	Nagoya University
 * @date	2018/03/14
 */

class Connection
{
private:
	string ip;
	int port;
	int sock = -1;
	int port2;
	int sock2 = -1;
	bool isTransportMode = false;
	struct sockaddr_in addr;
	string key;
	SSL *ssl = NULL;
	SSL *ssl2 = NULL;
	SSL_CTX *ctx = NULL;
	string certFilePath;
	string privateKeyFilePath;
	static string pemPass;
	bool isSSL = false;
	unsigned int mngId;
	int tcpSslTimeoutSec;
	int dtlsTimeoutSec;
	std::unordered_map<unsigned int, FUNC_CALLBACK> callBackMp;
	std::unordered_map<unsigned int, std::function<void(ResultSet)>> callBackClassMp;
	std::map<unsigned int, string> referenceMp;

	IS::StringUtil stringUtil;

	struct pass_info {
		union {
			struct sockaddr_storage ss;
			struct sockaddr_in6 s6;
			struct sockaddr_in s4;
		} server_addr, client_addr;
		SSL *ssl;
	};

	// 9010から10ポート(9010～9019)番を動的に割り当てる
	int udpReceptionPort = 9010;
	void receiveData(const int port);

	static unsigned char cookie_secret[COOKIE_SECRET_LENGTH];
	static int cookie_initialized;

	SSL_CTX* setupDTLS();
	int handle_socket_error();
	static int password_cb(char *buf, int size, int rwflag, void *u);
	static int dtls_verify_cb(int ok, X509_STORE_CTX *ctx);
	static int generate_cookie_cb(SSL *ssl, unsigned char *cookie, unsigned int *cookie_len);
	static int verify_cookie_cb(SSL *ssl, const unsigned char *cookie, unsigned int cookie_len);

	void receiveDTLSData(const int port);
	void sockDTLSProcess(void *info);
	void dataIntegration(string &buf, std::unordered_map<string, vector<string>> &recvDataMap, string &result);
	int recvUsingHeader(int sock, char *outBuf, SSL *ssl, bool isTcp, bool isSSL);
	int acceptDTLS(struct pass_info* info);
	unsigned int registerQuery(const string &query, FUNC_CALLBACK func_c, std::function<void(ResultSet)> func_c_plus, const string &reference);
public:
	Connection(const string &ip, const int port, const int sock, const struct sockaddr_in addr, const string &key, const int tcpSslTimeoutSec, const bool isTransportMode);
	Connection(const string &ip, const int port, const int sock, const struct sockaddr_in addr, const string &key,
		SSL *ssl, SSL_CTX *ctx, const string &certFilePath, const string &privateKeyFilePath, const string &inPemPass,
		const int tcpSslTimeoutSec, const int dtlsTimeoutSec, const bool isTransportMode);
	~Connection();

	ResultSet changeid(const string &query);
	ResultSet execute(const string &query);
	void disconnect();
	void disconnect(bool doClear);
	unsigned int registerQuery(const string &query, FUNC_CALLBACK func);
	unsigned int registerQuery(const string &query, FUNC_CALLBACK func, const string &reference);
	unsigned int registerQuery(const string &query, std::function<void(ResultSet)> func);
	unsigned int registerQuery(const string &query, std::function<void(ResultSet)> func, const string &reference);
	unsigned int registerQuery(const string &query, CallBackListener* cbl);
	unsigned int registerQuery(const string &query, CallBackListener* cbl, const string &reference);
	unsigned int registerQuery(const string &query, const unsigned long long destSID);
	unsigned int registerQuery(const string &query, const unsigned long long destSID, const unsigned long long executerSID);
	void cancelQuery(const unsigned int mngId);
	void reconnect();
};
#endif  // CONNECTION_H

