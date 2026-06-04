#ifndef DM_MANAGER_H
#define DM_MANAGER_H

#include "is/Connection.h"
#include "is/ResultSet.h"
#include "is/DatagramSocket.h"
#include "is/DmUtil.h"
#include "is/DmException.h"
#include "is/StringUtil.h"
#include "is/InformationSourceParser.h"

#include <string>
#include <map>
#include <openssl/md5.h>
#include <sys/socket.h>
#include <netdb.h>

using namespace std;

/**
 * DMライブラリの管理クラス
 *
 * @author	Nagoya University
 * @date	2018/03/14
 */

class DmManager
{
private:
	static map<string, string> sessionMap;
	static string certFilePath;
	static string privateKeyFilePath;
	static string pemPass;
	static bool isSSL;
	static int tcpSslTimeoutSec;
	static int dtlsTimeoutSec;

	static Connection* getConnection(const string &ip, const int port, const string &username, const string &pass);
	static Connection* getConnection(const string &ip, const int port, const string &username, const string &pass, bool isTransportMode);
	static Connection* getSSLConnection(const string &ip, const int port, const string &username, const string &pass);
	static Connection* getSSLConnection(const string &ip, const int port, const string &username, const string &pass, bool isTransportMode);

	static bool createSession(int &sock, const string &ip, const string &username, const string &pass);
	static bool createSession(SSL* ssl,  const string &ip, const string &username, const string &pass);
	static bool createSession(int &sock, SSL* ssl,  const string &ip, const string &username, const string &pass);
	static string getMD5(const string &pass);
public:
	DmManager();
	~DmManager();

	static int connectSock(const string &ip, const int port);
	static SSL* connectSSLSock(int &sock, const string &ip, const int port, SSL_CTX *ctx);
	static void initEncryptionSettings(const string &inCertFilePath, const string &inPrivateKeyFilePath, const string &inPemPass);
	static void initTcpSslTimeoutSetting(const int TcpSslTimeoutSec);
	static void initDtlsTimeoutSetting(const int DtlsTimeoutSec);
	static Connection* getDBConnection(const string &username, const string &pass);
	static Connection* getDBConnection(const string &ip, const int port, const string &username, const string &pass);
	static Connection* getDBConnection(const string &ip, const int port, const string &username, const string &pass, bool isTransportMode);
	static DatagramSocket* getDatagramSocket(const string &username, const string &pass);
	static DatagramSocket* getDatagramSocket(const string &ip, const int port, const string &username, const string &pass);
	static DatagramSocket* getDatagramSocket(const string &ip, const int port, const string &username, const string &pass, bool isTransportMode);
	static void clearSession();
};
#endif  // DM_MANAGER_H
