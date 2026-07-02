#include "is/DatagramSocket.h"
#include "is/DmUtil.h"

using namespace IS;
/**
 * コンストラクタ
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	ip  	IPアドレス
 * @param	port	ポート番号
 * @param	sock	ソケット
 * @param	key	セッションキー
 */

DatagramSocket::DatagramSocket(const string &ip, const int port, const int sock, const string &key)
{
	this->ip = ip;
	this->port = port;
	this->sock = sock;
	this->key = key;
}

/**
* コンストラクタ
*
* @author	Nagoya University
* @date	2019/07/31
*
* @param	ip  	IPアドレス
* @param	port	ポート番号
* @param	sock	ソケット
* @param	key	セッションキー
* @param	ssl  	SSLオブジェクト
* @param	ctx		SSLコンテキスト
*/

DatagramSocket::DatagramSocket(const string &ip, const int port, const int sock, const string &key, SSL *ssl, SSL_CTX *ctx)
{
	this->ip = ip;
	this->port = port;
	this->sock = sock;
	this->key = key;
	this->ssl = ssl;
	this->ctx = ctx;
}

/**
 * デストラクタ
 *
 * @author	Nagoya University
 * @date	2018/03/15
 */

DatagramSocket::~DatagramSocket()
{
	if (ssl != NULL) {
		SSL_free(ssl);
		ssl = NULL;
	}

	/* socketの終了 */
	if (sock = !- 1) {
		close(sock);
		sock = -1;
	}

	if (ctx != NULL) {
		SSL_CTX_free(ctx);
		ctx = NULL;
	}
}

/**
 * ストリームデータ送信処理
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @exception	ConnectionFailedException	DBシステム接続失敗
 *
 * @param	streamName	ストリーム名
 * @param	tuples	  	タプルリスト
 *
 * @return	True 
 */

bool DatagramSocket::sendStreamData(const string &streamName, const vector<Tuple> &tuples)
{
	bool doCompress = false;
#if COMPRESS_MODE == 1
	doCompress = true;
#endif
	return sendStreamData(streamName, tuples, doCompress);
}
/**
 * ストリームデータ送信処理
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @exception	ConnectionFailedException	DBシステム接続失敗
 *
 * @param	streamName	ストリーム名
 * @param	tuples	  	タプルリスト
 *
 * @return	True
 */

bool DatagramSocket::sendStreamData(const string &streamName, const vector<Tuple> &tuples, const bool doCompress)
{
#if MEASURE_MODE == 1
	long startTime = DmUtil::getTimeMicrosec();
	long procTime = DmUtil::getTimeMicrosec();
#endif

	// UDP送信
	// TODO : CSからsenderIdを取得
	string senderId = "";

	// ストリームデータXMLに変換
	int len;
	vector<string> sendDataList;
#if MEASURE_MODE == 1
	long now = DmUtil::getTimeMicrosec();
	double msec = (now - procTime) / 1000.0;
	cout << "[STAT] sock create processTime(msec):" << msec << endl;

	procTime = DmUtil::getTimeMicrosec();
#endif

	IS::ProtobufParser &pp = IS::ProtobufParser::get_instance();
	sendDataList = pp.createStreamList(streamName, tuples, this->key, IPv4_UDP_MAX_BYTE);
	if (sendDataList.empty()) {
		cerr << "[sendStreamData] Serialization failure using protobuf" << endl;
	}
#if MEASURE_MODE == 1
	now = DmUtil::getTimeMicrosec();
	msec = (now - procTime) / 1000.0;
	cout << "[STAT] createStreamXML processTime(msec):" << msec << endl;

	procTime = DmUtil::getTimeMicrosec();
#endif
	//cout << "[sendStreamData REQUEST] All length:" << len << " part:" << sendDataList.size() <<endl;

	IS::StringUtil stringUtil;
	char compressFlg = '0';
	if (doCompress) {
		compressFlg = COMPRESS_FLG_DEFAULT;
	}
	unsigned int sendSumLen = 0;
	for (string sendData : sendDataList) {
		char *sendPointer = (char *)sendData.c_str();
		int sendSize = sendData.length();
		if (compressFlg == '1' || compressFlg == '2') {
			char outbuf[IPv4_UDP_MAX_BYTE];
			long key = DmUtil::getTimeMicrosec();
			int compressedSize = stringUtil.setCompressedBufWithHeader(sendData, outbuf, compressFlg, key);
			if (compressedSize > 0) {
				sendPointer = outbuf;
				sendSize = compressedSize;
			}
		}
		if (ssl == NULL) {
			len = send(sock, sendPointer, sendSize, 0);
		}
		else {
			len = SSL_write(ssl, sendPointer, sendSize);
		}

		if (len > 0) {
#if DEBUG == 1
			cout << "[sendStreamData] send length:" << len << endl;
#endif
			sendSumLen = sendSumLen + len;
		}
		else {
			cout << "[sendStreamData] Failed to sendto. retry.... split.length:" << sendSize << endl;;
			cout << "[sendStreamData] Failed to send UDP errmsg: " << std::strerror(errno) << endl;;
			for (int retry = 1; retry <= 3; retry++) {
				if (ssl == NULL) {
					len = send(sock, sendPointer, sendSize, 0);
				} else {
					len = SSL_write(ssl, sendPointer, sendSize);
				}
				if (len > 0) {
					sendSumLen = sendSumLen + len;
					break;
				}
				else {
					if (retry >= 3) {
						cerr << "Failed to retry[3] send UDP. So break" << endl;
						break;
					}
					usleep(10 * 1000);
				}
			}
		}
	}
	len = sendSumLen;

#if MEASURE_MODE == 1
	now = DmUtil::getTimeMicrosec();
	msec = (now - procTime) / 1000.0;
	cout << "[STAT] sendto/SSL_write processTime(msec):" << msec << endl;

	procTime = DmUtil::getTimeMicrosec();
#endif

	if (len < 0) {
		throw ConnectionFailedException("[sendStreamData] Faild Connect to " + this->ip + ":" + to_string(this->port));
	}
#if MEASURE_MODE == 1
	now = DmUtil::getTimeMicrosec();
	msec = (now - procTime) / 1000.0;
	cout << "[STAT] close processTime(msec):" << msec << endl;
	msec = (now - startTime) / 1000.0;
	cout << "[STAT] sendStreamData processTime(msec):" << msec << endl;
#endif
	return true;
}


/**
* クローズ信処理
*
* @author	Nagoya University
* @date	2019/09/12
*
*/

void DatagramSocket::closeSocket()
{
	if (ssl != NULL) {
		SSL_free(ssl);
		ssl = NULL;
	}

	/* socketの終了 */
	if (sock = !- 1) {
		close(sock);
		sock = -1;
	}

	if (ctx != NULL) {
		SSL_CTX_free(ctx);
		ctx = NULL;
	}
}

