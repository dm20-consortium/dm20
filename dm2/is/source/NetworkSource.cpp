#include "NetworkSource.h"
#include "is/DmUtil.h"
#include "Settings.h"


using namespace std;

namespace IS {

	static const int BUF_MAX = 4096;
	unsigned char NetworkSource::cookie_secret[COOKIE_SECRET_LENGTH] = { };
	int NetworkSource::cookie_initialized = 0;

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	name	ネットワーク名称
	 * @param	port	ポート番号
	 */

	NetworkSource::NetworkSource(string name, int port)
		:InformationSource(name)
	{
		this->sourcename = name;
		this->port = port;
		this->port2 = 0;
	}

	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	NetworkSource::~NetworkSource()
	{
		if (this->TCPSockFD != 0) close(this->TCPSockFD);
	}

	/**
	* SSLライブラリの初期化
	* SSLの処理を動作させる場合は必ず1度だけ事前にコールする
	*
	* @author	Nagoya University
	* @date	2019/07/31
	*/

	void NetworkSource::SSL_init()
	{
		if (SSL_library_init() < 0) {
			cerr << "[SSL_init] Could not initialize the OpenSSL library !" << endl;
		}
		OpenSSL_add_all_algorithms();
		OpenSSL_add_ssl_algorithms();
		SSL_load_error_strings();
		ERR_load_crypto_strings();
		ERR_load_BIO_strings();
	}

	/**
	* SSLライブラリの終了処理
	* SSLの処理を終了させる場合は必ず1度だけコールする
	*
	* @author	Nagoya University
	* @date	2019/08/08
	*/

	void NetworkSource::SSL_finalize()
	{
		ENGINE_cleanup();
		CONF_modules_unload(1);
		ERR_free_strings();
		CRYPTO_cleanup_all_ex_data();
		EVP_cleanup();
		ERR_remove_state(0);
		sk_SSL_COMP_free(SSL_COMP_get_compression_methods());
	}

	/**
	 * UDPデータを受信する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void NetworkSource::receiveUDPdata()
	{
		char buf[IPv4_UDP_MAX_BYTE * 10] = { 0 };
		struct sockaddr_in from;
		socklen_t addrlen;
		addrlen = sizeof(struct sockaddr_in);
		StringUtil stringUtil;
		string receiveData = "";
		string bufStr;
		unordered_map<string, vector<string>> recvDataMap;

		// UDPソケットの生成
		int sockfd = setUDPconfig(this->port, 0/*ANY*/);
		if (sockfd < 0)
		{
			return;
		}

		// キューイング開始
		logger->debug("[receiveUDPdata] " + sourcename + " Receive START   port:" + to_string(this->port));

		while (1)
		{
			receiveData = "";

			// UDPの受信
			memset(buf, 0, sizeof (buf));

			int len = recvUsingHeader(sockfd, buf, NULL, false, false);
			//int len = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&from, &addrlen);

			if (len < 0)
			{
				// timeout. so continue
				if (exit_flag) {
					break;
				}
				continue;
			}
			else if (addrlen == 0)
			{
				continue;    //unidentified address
			}
			bufStr = string(buf, len);
			// データを統合
			dataIntegration(bufStr, recvDataMap, receiveData);
			if (receiveData.length() == 0) continue;
			std::string ip = inet_ntoa(from.sin_addr);
			logger->debug("[UDP] " + sourcename + " received data from " + ip + ":" + std::to_string(ntohs(from.sin_port)) + " Size : " + std::to_string(receiveData.size()) + " byte");
			logger->debug("[UDP] " + receiveData);

			RecvData data;
			data.sock = sockfd;
			data.client = from;
			data.payload = receiveData;
			data.isClose = false;
			receiveProcess(data);
		}
	}

	/**
	 * UDP通信設定を実施する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	port  	The port.
	 * @param	ipaddr	The ipaddr.
	 *
	 * @return	1	成功
	 * 			-1	失敗
	 */

	int NetworkSource::setUDPconfig(const int port, const unsigned long ipaddr)
	{
		int sockfd;
		struct sockaddr_in server;
		struct timeval timeout;

		//タイムアウト時間設定
		timeout.tv_sec = stoi(settings.getParameter("UDP_SOCK_TIMEOUT_SEC"));
		timeout.tv_usec = 0;

		// socket生成
		sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (sockfd < 0) {
			logger->error("[setUDPconfig] socket error");
			exit(-1);
		}

		// sockaddrに必要な情報を設定
		memset(&server, 0, sizeof(struct sockaddr_in));
		server.sin_family = AF_INET;
		if (ipaddr == 0)
		{
			server.sin_addr.s_addr = htonl(INADDR_ANY);
		}
		else
		{
			server.sin_addr.s_addr = htonl(ipaddr);
		}
		server.sin_port = htons(port);

		// タイムアウト設定
		if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
			logger->error("[setUDPconfig] setsockopt Error");
		}

		// socketへのバインド
		if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
		{
			logger->error("[setUDPconfig] bind error");
			close(sockfd);
			exit(-1);
		}
		return sockfd;
	}

	/**
	* SSL通信のセットアップを行う
	*
	* @author	Nagoya University
	* @date	2019/07/31
	*
	* @return SSLコンテキスト
	*/
	SSL_CTX* NetworkSource::setupSSL()
	{
		SSL_CTX *ctx;
		ctx = SSL_CTX_new(SSLv23_server_method());
		if (!ctx) {
			perror("[setupSSL] Unable to create SSL context");
			logger->error("[setupSSL] Unable to create SSL context");
			exit(-1);
		}
		SSL_CTX_set_ecdh_auto(ctx, 1);
		SSL_CTX_set_default_passwd_cb(ctx, this->password_cb);

		/* Set the key and cert */
		if (SSL_CTX_use_certificate_file(ctx, settings.getParameter("SSL_CERTIFICATE_FILE_ABS_PATH").c_str(), SSL_FILETYPE_PEM) <= 0) {
			logger->error("[setupSSL] Failed loading file: " + settings.getParameter("SSL_CERTIFICATE_FILE_ABS_PATH") + " Please check if the file exists and the settings are correct.");
			exit(-1);
		}
		if (SSL_CTX_use_PrivateKey_file(ctx, settings.getParameter("SSL_PRIVATE_KEY_FILE_ABS_PATH").c_str(), SSL_FILETYPE_PEM) <= 0) {
			logger->error("[setupSSL] Failed loading file: " + settings.getParameter("SSL_PRIVATE_KEY_FILE_ABS_PATH") + " Please check if the file exists and the settings are correct.");
			exit(-1);
		}
		return ctx;
	}

	/**
	* DTLS通信のセットアップを行う
	*
	* @author	Nagoya University
	* @date	2019/07/31
	*
	* @return SSLコンテキスト
	*/
	SSL_CTX* NetworkSource::setupDTLS()
	{
		SSL_CTX *ctx;
		ctx = SSL_CTX_new(DTLSv1_2_server_method());
		/* We accept all ciphers, including NULL.
		* Not recommended beyond testing and debugging
		*/
		SSL_CTX_set_cipher_list(ctx, "ALL:NULL:eNULL:aNULL");
		SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_OFF);
		SSL_CTX_set_default_passwd_cb(ctx, this->password_cb);

		if (!SSL_CTX_use_certificate_file(ctx, settings.getParameter("SSL_CERTIFICATE_FILE_ABS_PATH").c_str() , SSL_FILETYPE_PEM))
			logger->error("[setupDTLS] Failed loading file: " + settings.getParameter("SSL_CERTIFICATE_FILE_ABS_PATH") + " Please check if the file exists and the settings are correct.");

		if (!SSL_CTX_use_PrivateKey_file(ctx, settings.getParameter("SSL_PRIVATE_KEY_FILE_ABS_PATH").c_str(), SSL_FILETYPE_PEM))
			logger->error("[setupDTLS] Failed loading file: " + settings.getParameter("SSL_PRIVATE_KEY_FILE_ABS_PATH") + " Please check if the file exists and the settings are correct.");

		if (!SSL_CTX_check_private_key(ctx))
			logger->error("[setupDTLS] Failed check file: " + settings.getParameter("SSL_PRIVATE_KEY_FILE_ABS_PATH") + " Please check if the file exists and the settings are correct.");

		/* Client has to authenticate */
		SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, this->dtls_verify_cb);

		SSL_CTX_set_read_ahead(ctx, 1);
		SSL_CTX_set_cookie_generate_cb(ctx, this->generate_cookie_cb);
		SSL_CTX_set_cookie_verify_cb(ctx, this->verify_cookie_cb);
		return ctx;
	}

	/**
	* DTLSデータを受信する
	*
	* @author	Nagoya University
	* @date	2019/07/31
	*
	* @param	ctx	SSLコンテキスト
	*/

	void NetworkSource::receiveDTLSdata(SSL_CTX *ctx)
	{
		int fd;
		union {
			struct sockaddr_storage ss;
			struct sockaddr_in s4;
			struct sockaddr_in6 s6;
		} server_addr, client_addr;

		SSL *ssl;
		BIO *bio;
		struct pass_info *info;
		const int on = 1, off = 0;

		memset(&server_addr, 0, sizeof(struct sockaddr_storage));

		server_addr.s4.sin_family = AF_INET;
#ifdef HAVE_SIN_LEN
		server_addr.s4.sin_len = sizeof(struct sockaddr_in);
#endif
		server_addr.s4.sin_port = htons(port);

		fd = socket(server_addr.ss.ss_family, SOCK_DGRAM, 0);
		if (fd < 0) {
			perror("[receiveDTLSdata] socket error");
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

		logger->debug("[receiveDTLSdata] " + sourcename + " Receive START   port:" + to_string(this->port));

		while (1) {
			memset(&client_addr, 0, sizeof(struct sockaddr_storage));

			/* Create BIO */
			bio = BIO_new_dgram(fd, BIO_NOCLOSE);

			ssl = SSL_new(ctx);
			SSL_set_bio(ssl, bio, bio);
			SSL_set_options(ssl, SSL_OP_COOKIE_EXCHANGE);

			while (1) {
				int rtn = DTLSv1_listen(ssl, (BIO_ADDR*)&client_addr);
				if (rtn >= 1) {
					break;
				} else if (rtn < 0) {
					perror("[receiveDTLSdata] DTLSv1_listen fatal error");
					logger->error("[receiveDTLSdata] DTLSv1_listen fatal error ");
					break;
				}
			}

			info = (struct pass_info*) malloc(sizeof(struct pass_info));
			memcpy(&info->server_addr, &server_addr, sizeof(struct sockaddr_storage));
			memcpy(&info->client_addr, &client_addr, sizeof(struct sockaddr_storage));
			info->ssl = ssl;

			// ソケット処理は別スレッドに委譲
			thread queuingThread(&NetworkSource::sockDTLSProcess, this, info);
			queuingThread.detach();

		}
	}

	/**
	* DTLSにてacceptされた後の処理
	*
	* @author	Nagoya University
	* @date	2019/07/31
	*
	* @param	info	ソケット情報
	*/
	void NetworkSource::sockDTLSProcess(void *info) {
		ssize_t len;
		char buf[IPv4_UDP_MAX_BYTE * 10];
		struct pass_info *pinfo = (struct pass_info*) info;
		SSL *ssl = pinfo->ssl;
		int fd,  ret;
		const int on = 1, off = 0;
		struct timeval timeout;
		int num_timeouts = 0, max_timeouts = stoi(settings.getParameter("DTLS_SOCK_TIMEOUT_CNT"));
		string receiveData = "", bufStr = "";
		unordered_map<string, vector<string>> recvDataMap;
		string ip = inet_ntoa(pinfo->client_addr.s4.sin_addr);

		OPENSSL_assert(pinfo->client_addr.ss.ss_family == pinfo->server_addr.ss.ss_family);
		fd = socket(pinfo->client_addr.ss.ss_family, SOCK_DGRAM, 0);
		if (fd < 0) {
			perror("[sockDTLSProcess] socket error");
			logger->error("[sockDTLSProcess] socket error ");
			goto cleanup;
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
		BIO_set_fd(SSL_get_rbio(ssl), fd, BIO_NOCLOSE);
		BIO_ctrl(SSL_get_rbio(ssl), BIO_CTRL_DGRAM_SET_CONNECTED, 0, &pinfo->client_addr.ss);

		/* Finish handshake */
		do { ret = SSL_accept(ssl); } while (ret == 0);
		if (ret < 0) {
			//perror("[sockDTLSProcess] SSL_accept error");
			//logger->error("[sockDTLSProcess] SSL_accept error :" + string(ERR_error_string(ERR_get_error(), buf)));
			switch (SSL_get_error(ssl, ret)) {
			case SSL_ERROR_WANT_READ:
			case SSL_ERROR_WANT_WRITE:
				logger->error("[sockDTLSProcess] SSL_accept error SSL_ERROR_WANT_READ/WRITE");
				break;
			case SSL_ERROR_SYSCALL:
				logger->error("[sockDTLSProcess] SSL_accept error SSL_ERROR_SYSCALL");
				break;
			case SSL_ERROR_SSL:
				logger->error("[sockDTLSProcess] SSL_accept error SSL_ERROR_SSL");
				break;
			default:
				logger->error("[sockDTLSProcess] SSL_accept error SSL_ERROR_OTHER");
				break;
			}
			goto cleanup;
		}
		/* Set and activate timeouts */
		timeout.tv_sec = stoi(settings.getParameter("DTLS_SOCK_TIMEOUT_SEC"));
		timeout.tv_usec = 0;
		BIO_ctrl(SSL_get_rbio(ssl), BIO_CTRL_DGRAM_SET_RECV_TIMEOUT, 0, &timeout);

		while (!(SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN) && num_timeouts < max_timeouts) {
			receiveData = "";
			// UDPの受信
			// 可変長でデータを受信する
			memset(buf, 0, sizeof(buf));

			//int len = recvUsingHeader(fd, buf, ssl, false, true);
			len = SSL_read(ssl, buf, sizeof(buf));

			switch (SSL_get_error(ssl, len)) {
			case SSL_ERROR_NONE:
				num_timeouts = 0;
				logger->debug("[sockDTLSProcess] read " + to_string(len) + " bytes");
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
				logger->error("[sockDTLSProcess] Socket read error");
				if (!handle_socket_error()) goto cleanup;
				break;
			case SSL_ERROR_SSL:
				logger->error("[sockDTLSProcess] SSL read error");
				logger->error("[sockDTLSProcess] " + string(ERR_error_string(ERR_get_error(), buf)) + "(" + to_string(SSL_get_error(ssl, len)) + ")");
				goto cleanup;
				break;
			default:
				logger->error("[sockDTLSProcess] Unexpected error while reading!");
				goto cleanup;
				break;
			}

			if (len < 0)
			{
				// timeout. so continue
				if (exit_flag) {
					break;
				}
				continue;
			}
			bufStr = string(buf, len);

			// データを統合
			dataIntegration(bufStr, recvDataMap, receiveData);
			if (receiveData.length() == 0) continue;

#if MEASURE_MODE == 1
			logger->info("[sockDTLSProcess] receive complete NowMicroSec:" + to_string(DmUtil::getTimeMicrosec()));
#endif
			logger->debug("[DTLS] " + sourcename + " received data from " + ip + ":" + to_string(ntohs(pinfo->client_addr.s4.sin_port)) + " Size : " + std::to_string(receiveData.length()) + " byte(No split header)");
			logger->debug("[DTLS] " + receiveData);

			RecvData data;
			data.sock = -1;
			data.client = pinfo->client_addr.s4;
			data.payload = receiveData;
			data.ssl = pinfo->ssl;
			data.isClose = false;
			receiveProcess(data);
		}
		if (SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN) {
			SSL_set_shutdown(ssl, SSL_SENT_SHUTDOWN);
			SSL_shutdown(ssl);
		} else {
			SSL_clear(ssl);
		}
	cleanup:
		close(fd);

		free(info);
		SSL_free(ssl);
		ERR_remove_state(0);
		logger->debug("[sockDTLSProcess] Thread done, connection closed from " + ip + ":" + std::to_string(ntohs(pinfo->client_addr.s4.sin_port)));

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

	inline void NetworkSource::dataIntegration(string &buf, std::unordered_map<string, vector<string>> &recvDataMap, string &result)
	{
		int flagment = 0, flagmentMax = 0;
		string key;

		// ヘッダ情報を取得する
		stringUtil.getHeaderInfo(buf, key, flagment, flagmentMax);
		if (flagmentMax == 0) {
			logger->warn("[dataIntegration] Received invalied header, So continue...");
			return;
		}
#if DEBUG == 1
		cout << "UDP recv headerInfo  key:" << key << " flagment:" << flagment << " max:" << flagmentMax << endl;
#endif
		// resultMapに受信途中がないかチェック
		auto itr = recvDataMap.find(key);
		if (itr != recvDataMap.end()) {
			// 受信途中が存在する
			recvDataMap[key].at(flagment) = buf;
		}
		else {
			// 新規に受信
			vector<string> data(flagmentMax);
			data.at(flagment) = buf;
			recvDataMap[key] = data;
		}

		bool recvFinish = true;
		for (string str : recvDataMap[key]) {
			// 用意した配列に全てデータが入っていない場合は未受信データあり
			if (str.length() == 0) recvFinish = false;
		}

		if (recvFinish) {
			// 全てのデータを受信出来たらデータを繋ぎ合わせてMapは削除
			for (string str : recvDataMap[key]) {
				result.append(str);
			}
			recvDataMap.erase(key);
		}
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
	int NetworkSource::password_cb(char *buf, int size, int rwflag, void *u) {
		IS::Settings &settings = IS::Settings::get_instance();
		const char *tmp = settings.getParameter("SSL_PEM_PASS_PHRASE").c_str();
		if (tmp == NULL) /* An error occurred */
			return -1;

		size_t len = strlen(tmp);

		if (len > (unsigned int)size)
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
	int NetworkSource::dtls_verify_cb(int ok, X509_STORE_CTX *ctx) {
		/* This function should ask the user
		* if he trusts the received certificate.
		* Here we always trust.
		*/
		return 1;
	}

	/**
	* エラーハンドリング関数
	*
	* @author	Nagoya University
	* @date	2019/07/31
	*
	* @return	int
	*/
	int NetworkSource::handle_socket_error() {
		switch (errno) {
		case EINTR:
			/* Interrupted system call.
			* Just ignore.
			*/
			logger->error("[handle_socket_error] Interrupted system call!");
			return 1;
		case EBADF:
			/* Invalid socket.
			* Must close connection.
			*/
			logger->error("[handle_socket_error] Invalid socket!");
			return 0;
			break;
#ifdef EHOSTDOWN
		case EHOSTDOWN:
			/* Host is down.
			* Just ignore, might be an attacker
			* sending fake ICMP messages.
			*/
			logger->error("[handle_socket_error] Host is down!");
			return 1;
#endif
#ifdef ECONNRESET
		case ECONNRESET:
			/* Connection reset by peer.
			* Just ignore, might be an attacker
			* sending fake ICMP messages.
			*/
			logger->error("[handle_socket_error] Connection reset by peer!");
			return 1;
#endif
		case ENOMEM:
			/* Out of memory.
			* Must close connection.
			*/
			logger->error("[handle_socket_error] Out of memory!");
			return 0;
			break;
		case EACCES:
			/* Permission denied.
			* Just ignore, we might be blocked
			* by some firewall policy. Try again
			* and hope for the best.
			*/
			logger->error("[handle_socket_error] Permission denied!");
			return 1;
			break;
		default:
			/* Something unexpected happened */
			logger->error("[handle_socket_error] Unexpected error! (errno =" + to_string(errno) + ")");
			return 0;
			break;
		}
		return 0;
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
	int NetworkSource::generate_cookie_cb(SSL *ssl, unsigned char *cookie, unsigned int *cookie_len) {
		
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
			cout << "[generate_cookie_cb] out of memory" << endl;
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
	int NetworkSource::verify_cookie_cb(SSL *ssl, const unsigned char *cookie, unsigned int cookie_len) {
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
	 * TCPデータを受信する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	int NetworkSource::getTCPSocket(int port)
	{
		struct sockaddr_in client;
		int len;
		int sock;

		// TCPソケットの生成
		int TCPSockFD = setTCPconfig(port, 0/*ANY*/);
		//logger->warn("[TCP] socket fd:" + to_string(this->TCPSockFD) + " port:" + to_string(this->port));
		if (TCPSockFD < 0)
		{
			logger->error("[TCP] socket open error. socket fd:" + to_string(TCPSockFD) + " port:" + to_string(port));
			exit(-1);
		}
		// キューイング開始

		/*TCPクライアントからの接続要求を持てる状態にする*/
		listen(TCPSockFD, 5);
		logger->debug("[getTCPSocket] Receive START port:" + to_string(port));
		while (1)
		{
			len = sizeof(client);
			sock = accept(TCPSockFD, (struct sockaddr*)&client, (socklen_t*)&len);
			if (sock < 0) {
				logger->debug("[getTCPSocket] timeout:" + to_string(port) + ", sock:" + to_string(sock));
				// timeout. so continue
				continue;
			}
			logger->debug("[getTCPSocket] accept OK:" + to_string(port));
			break;
		}
		close(TCPSockFD);
		return sock;
	}
	/**
	 * TCPデータを受信する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void NetworkSource::checkTCPSocket(int sock)
	{
		struct timeval timeout;
		char buf[BUF_MAX] = { 0 };
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
			logger->error("[sockProcess] setsockopt error.");
		}
		while (true) {
			int result = recv(sock, buf, BUF_MAX, 0);
			if (result <= 0) {
				break;
			}
		}
		close(sock);
		return;
	}
	/**
	 * TCPデータを受信する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void NetworkSource::receiveTCPdata()
	{
		struct sockaddr_in client, client2;
		int len;
		int sock;
		int sock2 = 0;

		// TCPソケットの生成
		this->TCPSockFD = setTCPconfig(this->port, 0/*ANY*/);
		//logger->warn("[TCP] socket fd:" + to_string(this->TCPSockFD) + " port:" + to_string(this->port));
		if (this->TCPSockFD < 0)
		{
			logger->error("[TCP] socket open error. socket fd:" + to_string(this->TCPSockFD) + " port:" + to_string(this->port));
			exit(-1);
		}

		// キューイング開始
		/*TCPクライアントからの接続要求を持てる状態にする*/
		listen(this->TCPSockFD, 5);
		logger->debug("[receiveTCPdata] " + sourcename + " Receive START   port:" + to_string(this->port));
		while (1)
		{
			
			len = sizeof(client);
			sock = accept(this->TCPSockFD, (struct sockaddr*)&client, (socklen_t*)&len);
			if (exit_flag) {
				break;
			}
			if (sock > 0) {
				string ip = inet_ntoa(client.sin_addr);
				logger->debug("[receiveTCPdata] sock:" + to_string(sock) + " sock2:" + to_string(sock2) + " ip:" + ip);
				if (stringUtil.contain(settings.getParameter("HISTORY_RECORD_CLASS"), stringUtil.getClassName(typeid(*this)))) {
					thread createThread(&NetworkSource::createRecvHistory, this, "TCP", client, client2, sock, sock2, "accept", 0, 0, "", "");
					createThread.detach();
				}
			}
			if (exit_flag) {
				break;
			}
			if (sock < 0 || sock2 < 0) {
				// timeout. so continue
				continue;
			}
			// ソケット処理は別スレッドに委譲
			thread queuingThread(&NetworkSource::sockProcess, this, sock, sock2, client, client2);
			queuingThread.detach();

		}
		close(this->TCPSockFD);
	}
	/**
	 * ソケット処理を実施する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	sock  	ソケット
	 * @param	client	クライアント
	 */

	void NetworkSource::sockProcess(int sock, int sock2, struct sockaddr_in client, struct sockaddr_in client2)
	{
		char buf[BUF_MAX] = { 0 };
		struct timeval timeout;
		bool isExit = false;
		string query = "";
		int sumLen = 0;
		unsigned int bufSize = 0;
		string bufStr;
		StringUtil stringUtil;
		string ip = inet_ntoa(client.sin_addr);
		//タイムアウト時間設定
		timeout.tv_sec = std::stoi(settings.getParameter("TCP_TIMEOUT_SEC"));
		timeout.tv_usec = 0;
		
		if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
			logger->error("[sockProcess] setsockopt error.");
		}
		
		while (1)
		{
			logger->debug("[sockProcess] recv START...");
			query = "";
			sumLen = 0;
			bool isClose;

			while (1) {
				bool isBreak = false;
				isClose = false;
				string mes = "";
				string state = "recv";
				memset(buf, 0, sizeof(buf));
				int result = recv(sock, buf, BUF_MAX, 0);
				int result2 = 0;
				if (exit_flag) {
					close(sock);
					isExit = true;
					break;
				}
				if (result < 0) {
					state = "close";
					mes = "[sockProcess] Receive timeout. So sockProcess is break;";
					logger->warn(mes);
					result = close(sock);
					isExit = true;
					isBreak = true;
					isClose = true;
				}
				else if (result == 0) {
					// 接続がcloseされたためbreak;
					state = "close";
					mes = "[sockProcess] Connection close from " + ip + ":" + std::to_string(ntohs(client.sin_port));
					isExit = true;
					result = close(sock);
					logger->debug(mes);
					isBreak = true;
					isClose = true;
				}
				else {
					bufStr = string(buf, result);
					if (query.length() == 0) {
						bufSize = stringUtil.getXMLSize(bufStr);
						mes = "[sockProcess] recv from sock: " + std::to_string(sock) + ", bufSize: " + std::to_string(bufSize);
					}
					query.append(bufStr);
					sumLen = sumLen + result;
					bufStr.clear();
					if (query.length() >= bufSize || bufSize == 0) {
						isBreak = true;
					}
				}
				if (isBreak) {
					if (stringUtil.contain(settings.getParameter("HISTORY_RECORD_CLASS"), stringUtil.getClassName(typeid(*this)))) {
						thread createThread(&NetworkSource::createRecvHistory, this, "TCP", client, client2, sock, sock2, state, result, result2, query, mes);
						createThread.detach();
					}
					break;
				}
			}
			if (isExit) {
				if (isClose) {
					// Cancel処理を行う
					RecvData data;
					data.sock = sock;
					data.sock2 = sock2;
					data.isClose = isClose;
					data.client = client;
					data.payload = "";
					receiveProcess(data);
				}
				break;
			}
#if MEASURE_MODE == 1
			logger->info("[sockProcess] receive complete NowMicroSec:" + to_string(DmUtil::getTimeMicrosec()));
#endif
			logger->debug("[TCP] " + sourcename + " received data from " + ip + ":" + std::to_string(ntohs(client.sin_port)) + " Size : " + std::to_string(sumLen) + " byte");
			logger->debug("[TCP] " + query);

			RecvData data;
			data.sock = sock;
			data.sock2 = sock2;
			data.isClose = isClose;
			data.client = client;
			data.payload = query;
			receiveProcess(data);

		}
	}

	/**
	 * TCP通信設定を実施する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	port  	The port.
	 * @param	ipaddr	The ipaddr.
	 *
	 * @return	1	成功
	 * 			-1	失敗
	 */

	int NetworkSource::setTCPconfig(const int port, const unsigned long ipaddr)
	{
		int sockfd;
		struct sockaddr_in server;
		struct timeval timeout;
		int yes = 1;

		//タイムアウト時間設定
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		// socket生成
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) {
			logger->error("[setTCPconfig] socket error");
			return -1;
		}

		// sockaddrに必要な情報を設定
		memset(&server, 0, sizeof(struct sockaddr_in));
		server.sin_family = AF_INET;
		if (ipaddr == 0)
		{
			server.sin_addr.s_addr = htonl(INADDR_ANY);
		}
		else
		{
			server.sin_addr.s_addr = htonl(ipaddr);
		}
		server.sin_port = htons(port);

		// タイムアウト設定
		if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
			logger->error("[setTCPconfig] setsockopt Error.(Timeout setting)");
		}

		// TIME_WAIT対策設定
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes)) < 0) {
			logger->error("[setTCPconfig] setsockopt Error.(Time wait counterplain setting)");
		}

		// socketへのバインド
		if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
		{
			logger->error("[setTCPconfig] bind error");
			close(sockfd);
			return -1;
		}
		return sockfd;
	}

	/**
	* SSLデータを受信する
	*
	* @author	Nagoya University
	* @date	2019/07/31
	*
	* @param	ctx	SSLコンテキスト
	*/

	int NetworkSource::getSSLSocket(int port, SSL_CTX *ctx, SSL *ssl)
	{
		struct sockaddr_in client;
		int sock, len;
		char buf[BUF_MAX];

		int sockfd = setTCPconfig(port, 0);
		if (sockfd < 0)
		{
			logger->error("[getSSLSocket] socket open error");
			exit(-1);
		}
		// TCPクライアントからの接続要求を持てる状態にする
		listen(sockfd, 5);
		logger->debug("[getSSLSocket] " + sourcename + " Receive START   port:" + to_string(port));
		while (1)
		{
			len = sizeof(client);

			sock = accept(sockfd, (struct sockaddr*)&client, (socklen_t*)&len);
			// timeout. so continue
			if (exit_flag) {
				break;
			}
			if (sock < 0) {
				// timeout. so continue
				continue;
			}
			//ssl = SSL_new(ctx);
			SSL_set_fd(ssl, sock);
			int accept_rtn = SSL_accept(ssl);
			int accept_errno = errno;
			if (accept_rtn > 0) {
				logger->debug("[getSSLSocket] SSL_accept1 success. sock:" + to_string(sock));
				break;
			} else {
				logger->error("[getSSLSocket] SSL_accept error:" +
						string(ERR_error_string(ERR_get_error(), buf)) + ",SSL_get_error:" +
						to_string(SSL_get_error(ssl, accept_rtn)));
				perror("perror   ");
			    printf("%d\n", accept_errno);
			}
		}
		close(sockfd);
		//SSL_CTX_free(ctx);
		return sock;
	}

	void NetworkSource::checkSSLSocket(int sock, SSL *ssl)
	{
		struct timeval timeout;
		char buf[BUF_MAX] = { 0 };
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
			logger->error("[sockProcess] setsockopt error.");
		}
		while (true) {
			int result = SSL_read(ssl, buf, sizeof(buf));
			if (result <= 0) {
				sleep(1);
				if (SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN) {
					SSL_set_shutdown(ssl, SSL_SENT_SHUTDOWN);
					SSL_shutdown(ssl);
				} else {
					SSL_clear(ssl);
				}
				SSL_free(ssl);
				result = close(sock);
				break;
			}
		}
	}
	/**
	* SSLデータを受信する
	*
	* @author	Nagoya University
	* @date	2019/07/31
	*
	* @param	ctx	SSLコンテキスト
	*/

	void NetworkSource::receiveSSLdata(SSL_CTX *ctx)
	{
		while (1)
		{
			struct sockaddr_in client, client2;
			int sock, len;
			int sock2 = 0;

			int sockfd = setTCPconfig(this->port, 0);
			if (sockfd < 0)
			{
				logger->error("[receiveSSLdata] socket open error");
				exit(-1);
			}

			// TCPクライアントからの接続要求を持てる状態にする
			listen(sockfd, 5);
			logger->debug("[receiveSSLdata] " + sourcename + " Receive START   port:" + to_string(this->port));
			while (1)
			{
				len = sizeof(client);
				SSL *ssl;
				SSL *ssl2 = NULL;

				sock = accept(sockfd, (struct sockaddr*)&client, (socklen_t*)&len);
				// timeout. so continue
				if (exit_flag) {
					break;
				}
				if (sock < 0) {
					// timeout. so continue
					continue;
				}
				ssl = SSL_new(ctx);
				SSL_set_fd(ssl, sock);
				bool isAccept = false;
				int accept_rtn = SSL_accept(ssl);
				if (accept_rtn > 0) {
					logger->debug("[receiveSSLdata] SSL_accept1 success. sock:" + to_string(sock));
					isAccept = true;
				} else {
					//char buf[BUF_MAX];
					//int accept_errno = errno;
					//logger->error("[receiveSSLdata] SSL_accept error:" +
							//string(ERR_error_string(ERR_get_error(), buf)) + ",SSL_get_error:" +
							//to_string(SSL_get_error(ssl, accept_rtn)));
					//perror("perror   ");
					//printf("%d\n", accept_errno);
					switch (SSL_get_error(ssl, accept_rtn)) {
					case SSL_ERROR_WANT_READ:
					case SSL_ERROR_WANT_WRITE:
						logger->error("[receiveSSLdata] SSL_accept error SSL_ERROR_WANT_READ/WRITE");
						break;
					case SSL_ERROR_SYSCALL:
						logger->error("[receiveSSLdata] SSL_accept error SSL_ERROR_SYSCALL");
						break;
					case SSL_ERROR_SSL:
						logger->error("[receiveSSLdata] SSL_accept error SSL_ERROR_SSL");
						break;
					default:
						logger->error("[receiveSSLdata] SSL_accept error SSL_ERROR_OTHER");
						break;
					}
				}
				if (isAccept) {
					if (stringUtil.contain(settings.getParameter("HISTORY_RECORD_CLASS"), stringUtil.getClassName(typeid(*this)))) {
						thread createThread(&NetworkSource::createRecvHistory, this, "SSL", client, client2, sock, sock2, "accept", 0, 0, "", "");
						createThread.detach();
					}
					// ソケット処理は別スレッドに委譲
					thread queuingThread(&NetworkSource::sockSSLProcess, this, sock, sock2, client, client2, ssl, ssl2);
					queuingThread.detach();
				} else {
					logger->warn("[receiveSSLdata] SSL_accept error => reset SSL");
				}

			}
			close(sockfd);
		}
		SSL_CTX_free(ctx);
	}

	/**
	* ソケット処理を実施する
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param	sock  	ソケット
	* @param	client	クライアント
	*/

	void NetworkSource::sockSSLProcess(int sock, int sock2, struct sockaddr_in client, struct sockaddr_in client2, SSL *ssl, SSL *ssl2)
	{
		IS::Settings &settings = IS::Settings::get_instance();
		char buf[BUF_MAX] = { 0 };
		struct timeval timeout;
		bool isExit = false;
		string query = "";
		int sumLen = 0;
		unsigned int bufSize = 0;
		string bufStr;
		StringUtil stringUtil;
		string ip = inet_ntoa(client.sin_addr);
		//タイムアウト時間設定
		timeout.tv_sec = std::stoi(settings.getParameter("TCP_TIMEOUT_SEC"));
		timeout.tv_usec = 0;

		if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
			logger->error("[sockSSLProcess] setsockopt error.");
		}

		while (1)
		{
			logger->debug("[sockSSLProcess] recv START...");
			query = "";
			sumLen = 0;

			bool isClose;
			while (1) {
				bool isBreak = false;
				isClose = false;
				string mes = "";
				string state = "recv";
				memset(buf, 0, sizeof(buf));
				int result = SSL_read(ssl, buf, sizeof(buf));
				int result2 = 0;

				if (result < 0) {
					state = "close";
					mes = "[sockSSLProcess] Receive timeout. So sockProcess is break;";
					logger->warn(mes);
					sleep(1);
					if (SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN) {
						SSL_set_shutdown(ssl, SSL_SENT_SHUTDOWN);
						SSL_shutdown(ssl);
					} else {
						SSL_clear(ssl);
					}
					SSL_free(ssl);
					result = close(sock);
					isExit = true;
					isBreak = true;
					isClose = true;
				}
				else if (result == 0) {
					// 接続がcloseされた
					state = "close";
					mes = "[sockSSLProcess] Connection close from " + ip + ":" + std::to_string(ntohs(client.sin_port));
					isExit = true;
					sleep(1);
					if (SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN) {
						SSL_set_shutdown(ssl, SSL_SENT_SHUTDOWN);
						SSL_shutdown(ssl);
					} else {
						SSL_clear(ssl);
					}
					SSL_free(ssl);
					result = close(sock);
					logger->debug(mes);
					isBreak = true;
					isClose = true;
				}
				else {
					bufStr = string(buf, result);
					if (query.length() == 0) {
						bufSize = stringUtil.getXMLSize(bufStr);
						mes = "[sockProcess] recv from sock: " + std::to_string(sock) + ", bufSize: " + std::to_string(bufSize);
					}
					query.append(bufStr);
					sumLen = sumLen + result;
					bufStr.clear();
					if (query.length() >= bufSize || bufSize == 0) {
						isBreak = true;
					}
				}
				if (isBreak) {
					if (stringUtil.contain(settings.getParameter("HISTORY_RECORD_CLASS"), stringUtil.getClassName(typeid(*this)))) {
						thread createThread(&NetworkSource::createRecvHistory, this, "SSL", client, client2, sock, sock2, state, result, result2, query, mes);
						createThread.detach();
					}
					break;
				}
			}
			if (isExit) {
				if (isClose) {
					// Cancel処理を行う
					RecvData data;
					data.sock = sock;
					data.sock2 = sock2;
					data.isClose = isClose;
					data.client = client;
					data.payload = "";
					data.ssl = ssl;
					receiveProcess(data);
				}
				break;
			}
#if MEASURE_MODE == 1
			logger->info("[sockSSLProcess] receive complete NowMicroSec:" + to_string(DmUtil::getTimeMicrosec()));
#endif
			logger->debug("[SSL] " + sourcename + " received data from " + ip + ":" + std::to_string(ntohs(client.sin_port)) + " Size : " + std::to_string(sumLen) + " byte");
			logger->debug("[SSL] " + query);

			RecvData data;
			data.sock = sock;
			data.sock2 = sock2;
			data.isClose = isClose;
			data.client = client;
			data.payload = query;
			data.ssl = ssl;
			data.ssl2 = ssl2;
			receiveProcess(data);

		}
	}
	/**
	 * 受信履歴をDBに登録する
	 *
	 * @author	Shinichi Kusayama
	 * @date	2022/02/11
	 */

	void NetworkSource::createRecvHistory(string proto, struct sockaddr_in client, struct sockaddr_in client2, int sock1, int sock2, string act, int res, int res2, string recvData, string memo)
	{
		long ts = DmUtil::getTimeMillisec();
		string local_ip = "localhost";
		int local_port1 = this->port;
		string foreign_ip = inet_ntoa(client.sin_addr);
		int foreign_port1 = int(ntohs(client.sin_port));
		int foreign_port2 = 0;
		string recvDataOneLine = recvData;
		stringUtil.deleteNl(recvDataOneLine);
		pthread_mutex_init(&createMtx, NULL);
		pthread_mutex_lock(&createMtx);
		try {
			// DB接続
			connection Conn(("dbname=" + settings.getParameter("DATABASE_IS_NAME") + " user=" + settings.getParameter("USER_NAME") + " password=" + settings.getParameter("DB_PASS") + " hostaddr=" + settings.getParameter("DATABASE_ADDR") + " port=" + settings.getParameter("DATABASE_PORT")));
			// 受信履歴を登録
			work T(Conn);
			Conn.prepare("insert_recv_history", "INSERT INTO recv_history VALUES($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13,$14);");
			T.exec_prepared("insert_recv_history", ts, proto, local_ip, local_port1, foreign_ip, foreign_port1, foreign_port2, sock1, sock2, act, res, res2, recvDataOneLine, memo);
			T.commit();
			//Conn.disconnect();
		}
		catch (std::exception& e) {
			string what(e.what());
			string funcname = __func__;
			logger->warn(funcname + ", PostgreSQL Error: " + what);
		}
		pthread_mutex_unlock(&createMtx);

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
	int NetworkSource::recvUsingHeader(int sock, char *outBuf, SSL *ssl, bool isTcp, bool isSSL)
	{
		char bufComp[IPv4_UDP_MAX_BYTE * 10];
		int len = 0;
		int pre_len = 0;
		memset(bufComp, 0, sizeof(bufComp));
		char bufTmp[IPv4_UDP_MAX_BYTE];
		// for UDP(recvfrom)
		struct sockaddr_in from;
		socklen_t addrlen;
		addrlen = sizeof(struct sockaddr_in);

		int maxCompSize;
		if (isTcp) {
			maxCompSize = 1300;
		} else {
			maxCompSize = SEND_BUF_SIZE;
		}
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
					now_len = recv(sock, bufTmp, SEND_BUF_SIZE, 0);
				}
			} else {
				if (isSSL) {
					now_len = SSL_read(ssl, bufTmp, sizeof(bufTmp));
				} else {
					now_len = recvfrom(sock, bufTmp, sizeof(bufTmp), 0, (struct sockaddr *)&from, &addrlen);
					if (addrlen == 0 || now_len <= 0) {
						return -1;		// undefined address
					}
					if (isFirst) {
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
						isFirst = false;
					}
				}
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
}
