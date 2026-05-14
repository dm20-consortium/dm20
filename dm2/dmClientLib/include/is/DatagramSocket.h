#ifndef DATAGRAM_SOCKET_H
#define DATAGRAM_SOCKET_H

#include "is/InformationSourceParser.h"
#include "is/Tuple.h"
#include "is/StringUtil.h"
#include "is/DmException.h"

#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <zlib.h>

#include <openssl/ssl.h>

using namespace std;
using namespace IS;

/**
 * UDP送信処理を扱うクラス
 *
 * @author	Nagoya University
 * @date	2018/03/14
 */

#define IPv4_UDP_MAX_BYTE 65000
// SSL_read()関数の最大サイズ: 16384
#define IPv4_DTLS_MAX_BYTE (1<<14)

// 1: zlib, 2: Zstandard
#define COMPRESS_FLG_DEFAULT '2'

class DatagramSocket
{
private:
	string ip;
	int port;
	int sock = -1;
	struct sockaddr_in addr;
	string key;
	SSL *ssl = NULL;
	SSL_CTX *ctx = NULL;

public:
	DatagramSocket(const string &ip, const int port, const int sock, const struct sockaddr_in addr, const string &key);
	DatagramSocket(const string &ip, const int port, const int sock, const struct sockaddr_in addr, const string &key, SSL *ssl, SSL_CTX *ctx);
	~DatagramSocket();

	bool sendStreamData(const string &streamName, const vector<Tuple> &tuples);
	bool sendStreamData(const string &streamName, const vector<Tuple> &tuples, const bool doCompress);
	void closeSocket();
};
#endif  // DATAGRAM_SOCKET_H
