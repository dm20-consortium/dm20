#ifndef NETWORKSORCE_H
#define NETWORKSORCE_H

// NetworkSourceクラス
// 通信を実施するクラス

#include "InformationSource.h"
#include "InformationSourceParser.h"
#include "is/Schema.h"
#include "is/TupleSet.h"

#include <thread>
#include <log4cxx/logger.h>
#include <pqxx/pqxx>

// for UDP
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <unordered_map>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/conf.h>
#include <openssl/engine.h>

#define COOKIE_SECRET_LENGTH 16

using namespace std;
using namespace log4cxx;
using namespace pqxx;

namespace IS {
	struct RecvData
	{
		int sock;
		int sock2;
		bool isClose;
		struct sockaddr_in client;
		string payload;
		SSL *ssl = NULL;
		SSL *ssl2 = NULL;
		string schema_name;
		string cs_ip_address;
	};
	struct pass_info {
		union {
			struct sockaddr_storage ss;
			struct sockaddr_in6 s6;
			struct sockaddr_in s4;
		} server_addr, client_addr;
		SSL *ssl;
	};
	/**
	 * ネットワークを介する情報源に共通する処理クラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	class NetworkSource : public InformationSource
	{
	private:
		LoggerPtr logger = Logger::getLogger("NetworkSource");
		IS::Settings &settings = IS::Settings::get_instance();
		StringUtil stringUtil;
		string sourcename;
		int port;
		int port2;
		int TCPSockFD = 0;
		int TCPSockFD2 = 0;
		static unsigned char cookie_secret[COOKIE_SECRET_LENGTH];
		static int cookie_initialized;
		pthread_mutex_t createMtx;

		int setTCPconfig(const int port, const unsigned long ipaddr);
		void sockProcess(int sock, int sock2, struct sockaddr_in client, struct sockaddr_in client2);
		void sockSSLProcess(int sock, int sock2, struct sockaddr_in client, struct sockaddr_in client2, SSL *ssl, SSL *ssl2);

		int setUDPconfig(const int port, const unsigned long ipaddr);
		void sockDTLSProcess(void *info);
		int handle_socket_error();
		void dataIntegration(string &buf, std::unordered_map<string, vector<string>> &recvDataMap, string &result);
		static int password_cb(char *buf, int size, int rwflag, void *u);
		static int dtls_verify_cb(int ok, X509_STORE_CTX *ctx);
		static int generate_cookie_cb(SSL *ssl, unsigned char *cookie, unsigned int *cookie_len);
		static int verify_cookie_cb(SSL *ssl, const unsigned char *cookie, unsigned int cookie_len);
		void createRecvHistory(string proto, struct sockaddr_in client, struct sockaddr_in client2, int sock1, int sock2, string act, int res, int res2, string data, string memo);
		int recvUsingHeader(int sock, char *outBuf, SSL *ssl, bool isTcp, bool isSSL);
	protected:
		bool exit_flag = false;
		void receiveUDPdata();
		void receiveTCPdata();
		SSL_CTX* setupSSL();
		SSL_CTX* setupDTLS();
		void receiveSSLdata(SSL_CTX *ctx);
		void receiveDTLSdata(SSL_CTX *ctx);
		virtual void receiveProcess(RecvData &data) = 0;
	public:
		NetworkSource(string name, int port);
		virtual ~NetworkSource();
		int getTCPSocket(int port);
		void checkTCPSocket(int sock);
		int getSSLSocket(int port, SSL_CTX *ctx, SSL *ssl);
		void checkSSLSocket(int sock, SSL *ssl);

		virtual void init() = 0;       // impl
		virtual void start() = 0;      // impl
		virtual void start(bool isSSL) = 0;      // impl
		virtual void stop() = 0;       // impl

		static void SSL_init();
		static void SSL_finalize();
	};
}
#endif  // NETWORKSORCE_H
