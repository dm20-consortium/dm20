#include "Cs.h"
#include "SocketDTLS.h"
#include <pthread.h>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
//#include <fcntl.h>		#ノンブロッキング用（現在未使用）

using namespace log4cxx;

namespace CS{
	pthread_mutex_t* SocketDTLS::mutex_buf = NULL;
	LoggerPtr logger = Logger::getLogger("SocketDTLS");
	volatile sig_atomic_t stop_flag = 0;
	
	unsigned long SocketDTLS::id_function(void) {
		return (unsigned long) pthread_self();
	}

	int SocketDTLS::THREAD_setup(void) {
		int i;

		mutex_buf = (pthread_mutex_t*) malloc(CRYPTO_num_locks() * sizeof(pthread_mutex_t));

		if (!mutex_buf)
			return 0;
		for (i = 0; i < CRYPTO_num_locks(); i++)
			pthread_mutex_init(&mutex_buf[i], NULL);

		CRYPTO_set_id_callback(id_function);
		CRYPTO_set_locking_callback(locking_function);
		return 1;
	}

	int SocketDTLS::THREAD_cleanup(void){
		int i;

		if (!mutex_buf)
			return 0;

		CRYPTO_set_id_callback(NULL);
		CRYPTO_set_locking_callback(NULL);
		for (i = 0; i < CRYPTO_num_locks(); i++)
		pthread_mutex_destroy(&mutex_buf[i]);
		free(mutex_buf);
		mutex_buf = NULL;
		return 1;
	}

	int SocketDTLS::handle_socket_error(void) {
		switch (errno) {
			case EINTR:
				/* Interrupted system call.
				 * Just ignore.
				 */
				printf("Interrupted system call!\n");
				return 1;
			case EBADF:
				/* Invalid socket.
				 * Must close connection.
				 */
				printf("Invalid socket!\n");
				return 0;
				break;
	#ifdef EHOSTDOWN
			case EHOSTDOWN:
				/* Host is down.
				 * Just ignore, might be an attacker
				 * sending fake ICMP messages.
				 */
				printf("Host is down!\n");
				return 1;
#endif
#ifdef ECONNRESET
			case ECONNRESET:
				/* Connection reset by peer.
				 * Just ignore, might be an attacker
				 * sending fake ICMP messages.
				 */
				printf("Connection reset by peer!\n");
				return 1;
#endif
			case ENOMEM:
				/* Out of memory.
				 * Must close connection.
				 */
				printf("Out of memory!\n");
				return 0;
				break;
			case EACCES:
				/* Permission denied.
				 * Just ignore, we might be blocked
				 * by some firewall policy. Try again
				 * and hope for the best.
				 */
				printf("Permission denied!\n");
				return 1;
				break;
			default:
				/* Something unexpected happened */
				printf("Unexpected error! (errno = %d)\n", errno);
				return 0;
				break;
		}
		return 0;
	}
	/**
	 * @fn	int SocketDTLS::StartDtlsServer(int port, char *local_address)
	 *
	 * @brief	DTLSサーバー開始
	 *
	 * @author	Nagoya University
	 * @date	2023/01/27
	 *
	 * @param	int
	 * @param	char*
	 * @return	int		ソケットID
	 */
	int SocketDTLS::StartDtlsServer(int port, char *local_address, const std::string &confDirPath){
		union {
			struct sockaddr_storage ss;
			struct sockaddr_in s4;
			struct sockaddr_in6 s6;
		} server_addr;
		server_addr_str = local_address;

		double start_time = 0;
		double end_time = 0;

		const int on = 1, off = 0;

		memset(&server_addr, 0, sizeof(struct sockaddr_storage));
		size_t bind_size = sizeof(struct sockaddr_in6);
		if (strlen(local_address) == 0) {
			server_addr.s6.sin6_family = AF_INET6;
#ifdef HAVE_SIN6_LEN
			server_addr.s6.sin6_len = sizeof(struct sockaddr_in6);
#endif
			server_addr.s6.sin6_addr = in6addr_any;
			server_addr.s6.sin6_port = htons(port);
		}else{
			if (inet_pton(AF_INET, local_address, &server_addr.s4.sin_addr) == 1){
				bind_size = sizeof(struct sockaddr_in);
				server_addr.s4.sin_family = AF_INET;
#ifdef HAVE_SIN_LEN
				server_addr.s4.sin_len = sizeof(struct sockaddr_in);
#endif
				server_addr.s4.sin_port = htons(port);
			}else if (inet_pton(AF_INET6, local_address, &server_addr.s6.sin6_addr) == 1){
				server_addr.s6.sin6_family = AF_INET6;
#ifdef HAVE_SIN6_LEN
				server_addr.s6.sin6_len = sizeof(struct sockaddr_in6);
#endif
				server_addr.s6.sin6_port = htons(port);
			}else{
				socket_res_ = -1;
				exit(EXIT_FAILURE);
			}
		}

		THREAD_setup();
		OpenSSL_add_ssl_algorithms();
		ERR_load_BIO_strings();
		ERR_load_crypto_strings();
		SSL_load_error_strings();
		if (SSL_library_init() < 0) {
			std::cerr << "[reconnect] Could not initialize the OpenSSL library !" << std::endl;
		}
		SEC::DtlsContext Context = SEC::DtlsContext();
		ctx = Context.CreateServerContext(confDirPath);

		start_time = calcTime();
		LOG4CXX_DEBUG(logger, "Socketの生成 START");

		socket_res_ = socket(server_addr.ss.ss_family, SOCK_DGRAM, 0);
		if (socket_res_ < 0) {
			perror("socket");
			socket_res_ = -1;
			exit(EXIT_FAILURE);
		}

		setsockopt(socket_res_, SOL_SOCKET, SO_REUSEADDR, (const void*) &on, (socklen_t) sizeof(on));

		if (bind(socket_res_, (const struct sockaddr *) &server_addr, bind_size)){
			std::string msg = "local_address=" + std::string(local_address) + ",port=" + std::to_string(port);
			LOG4CXX_ERROR(logger, "bind error:" + msg);
			perror("bind");
			socket_res_ = -1;
			exit(EXIT_FAILURE);
		}
		info = (struct pass_info*) malloc (sizeof(struct pass_info));
		memcpy(&info->server_addr, &server_addr, sizeof(struct sockaddr_storage));
		//ssl = SSL_new(ctx);

		end_time = calcTime();
		LOG4CXX_DEBUG(logger, "Socketの生成 END 経過時間[ms]：" + std::to_string(end_time - start_time));
		return socket_res_;
	}

	/**
	 * @fn	int SocketDTLS::StartDtlsClient(char *remote_address, char *local_address, int port)
	 *
	 * @brief	DTLSクライアント開始
	 *
	 * @author	Nagoya University
	 * @date	2023/02/02
	 *
	 * @param	char*
	 * @param	char*
	 * @param	int
	 * @return	int		接続結果リターン値
	 */
	int SocketDTLS::StartDtlsClient(char *remote_address, char *local_address, int port, const std::string &confDirPath, bool &exit_flg_) {
		union {
			struct sockaddr_storage ss;
			struct sockaddr_in s4;
			struct sockaddr_in6 s6;
		} remote_addr, local_addr;

		SSL_CTX *ctx;
		int retval = 0;
		int error_retval = -1;
		std::string msg = "remote_ip=" + std::string(remote_address);

		memset((void *) &remote_addr, 0, sizeof(struct sockaddr_storage));
		memset((void *) &local_addr, 0, sizeof(struct sockaddr_storage));

		if (inet_pton(AF_INET, remote_address, &remote_addr.s4.sin_addr) == 1){
			remote_addr.s4.sin_family = AF_INET;
#ifdef HAVE_SIN_LEN
			remote_addr.s4.sin_len = sizeof(struct sockaddr_in);
#endif
			remote_addr.s4.sin_port = htons(port);
			LOG4CXX_DEBUG(logger, "v4");
		} else if (inet_pton(AF_INET6, remote_address, &remote_addr.s6.sin6_addr) == 1){
			remote_addr.s6.sin6_family = AF_INET6;
#ifdef HAVE_SIN6_LEN
			remote_addr.s6.sin6_len = sizeof(struct sockaddr_in6);
#endif
			remote_addr.s6.sin6_port = htons(port);
			LOG4CXX_DEBUG(logger, "v6");
		} else {
			return error_retval;
		}

		double start_socket = calcTime();
		LOG4CXX_DEBUG(logger, "Socket生成 START");

		socket_res_ = socket(remote_addr.ss.ss_family, SOCK_DGRAM, 0);
		LOG4CXX_DEBUG(logger, "Socket No." + std::to_string(socket_res_));
		if (socket_res_ < 0){
			perror("socket");
			return error_retval;
		}

		if (strlen(local_address) > 0){
			if (inet_pton(AF_INET, local_address, &local_addr.s4.sin_addr) == 1){
				local_addr.s4.sin_family = AF_INET;
#ifdef HAVE_SIN_LEN
				local_addr.s4.sin_len = sizeof(struct sockaddr_in);
#endif
				local_addr.s4.sin_port = htons(0);
			} else if (inet_pton(AF_INET6, local_address, &local_addr.s6.sin6_addr) == 1){
				local_addr.s6.sin6_family = AF_INET6;
#ifdef HAVE_SIN6_LEN
				local_addr.s6.sin6_len = sizeof(struct sockaddr_in6);
#endif
				local_addr.s6.sin6_port = htons(0);
			} else {
				return error_retval;
			}
			//OPENSSL_assert(remote_addr.ss.ss_family == local_addr.ss.ss_family);
			if (local_addr.ss.ss_family == AF_INET) {
				if (bind(socket_res_, (const struct sockaddr *) &local_addr, sizeof(struct sockaddr_in))){
					LOG4CXX_ERROR(logger, "bind v4 error:" + msg);
					perror("bind");
					return error_retval;
				}
			} else {
				if (bind(socket_res_, (const struct sockaddr *) &local_addr, sizeof(struct sockaddr_in6))){
					LOG4CXX_ERROR(logger, "bind v6 error:" + msg);
					perror("bind");
					return error_retval;
				}
			}
		}

		OpenSSL_add_ssl_algorithms();
		SSL_load_error_strings();

		if (SSL_library_init() < 0) {
			std::cerr << "[reconnect] Could not initialize the OpenSSL library !" << std::endl;
			return error_retval;
		}

		SEC::DtlsContext Context = SEC::DtlsContext();
		ctx = Context.CreateClientContext(confDirPath);
		
		ssl_ = SSL_new(ctx);
		BIO *bio = BIO_new_dgram(socket_res_, BIO_CLOSE);
		
		int sleep_time_cnt = 0;
		int sleep_time_cnt_max = 6;
		while (true) {
			if (sleep_time_cnt > sleep_time_cnt_max) break;
			size_t sock_size = 0;
			if (remote_addr.ss.ss_family == AF_INET){
				sock_size = sizeof(struct sockaddr_in);
			} else {
				sock_size = sizeof(struct sockaddr_in6);
			}
			// ソケットをノンブロッキングモードに設定（現在は未使用）
			/*
			int flags = fcntl(socket_res_, F_GETFL, 0);
			fcntl(socket_res_, F_SETFL, flags | O_NONBLOCK);
			int connect_rtn = connect(socket_res_, (struct sockaddr *) &remote_addr, sock_size);
			//std::cout << "[connect after]" << connect_rtn << ",sock:" << socket_res_ << "," << msg << std::endl;
			if (connect_rtn){
				perror("connect");
				sleep_time_cnt++;
				sleep(sleep_time_for_reconnect);
				continue;
			}
			*/
			if (connect(socket_res_, (struct sockaddr *) &remote_addr, sock_size)){
				perror("connect");
				sleep_time_cnt++;
				sleep(sleep_time_for_reconnect);
				continue;
			}
			BIO_ctrl(bio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &remote_addr.ss);
		
			SSL_set_bio(ssl_, bio, bio);
			double end_socket = calcTime();
			LOG4CXX_DEBUG(logger, "Socket生成 END 経過時間[ms]：" + std::to_string(end_socket - start_socket));

			double start_time = calcTime();
			retval = SSL_connect(ssl_);
			if (retval <= 0){
				if (exit_flg) {
					exit_flg_ = true;
					return retval;
				}
				std::string errmsg;
				switch (SSL_get_error(ssl_, retval)){
					case SSL_ERROR_ZERO_RETURN:
						errmsg = "SSL_connect failed with SSL_ERROR_ZERO_RETURN";
						break;
					case SSL_ERROR_WANT_READ:
						errmsg = "SSL_connect failed with SSL_ERROR_WANT_READ";
						break;
					case SSL_ERROR_WANT_WRITE:
						errmsg = "SSL_connect failed with SSL_ERROR_WANT_WRITE";
						break;
					case SSL_ERROR_WANT_CONNECT:
						errmsg = "SSL_connect failed with SSL_ERROR_WANT_CONNECT";
						break;
					case SSL_ERROR_WANT_ACCEPT:
						errmsg = "SSL_connect failed with SSL_ERROR_WANT_ACCEPT";
						break;
					case SSL_ERROR_WANT_X509_LOOKUP:
						errmsg = "SSL_connect failed with SSL_ERROR_WANT_X509_LOOKUP";
						break;
					case SSL_ERROR_SYSCALL:
						errmsg = "SSL_connect failed with SSL_ERROR_SYSCALL";
						break;
					case SSL_ERROR_SSL:
						errmsg = "SSL_connect failed with SSL_ERROR_SSL";
						break;
					default:
						errmsg = "SSL_connect failed with unknown error";
						break;
				}
				LOG4CXX_WARN(logger, errmsg + "," + msg + ",sleep_time_cnt:" + std::to_string(sleep_time_cnt));
			}
			if (retval > 0) {
				double end_time = calcTime();
				LOG4CXX_DEBUG(logger, "HandShake END 経過時間[ms]：" + std::to_string(end_time - start_time));
				break;
			}
			sleep_time_cnt++;
			sleep(sleep_time_for_reconnect);
		}
		if (retval <= 0) {
			LOG4CXX_WARN(logger, "close");
			close(socket_res_);
			LOG4CXX_WARN(logger, "SSL_free");
			SSL_free(ssl_);
		}

		return retval;
	}


	/**
	 * @fn	int SocketDTLS::Recvfrom(int fd, sockaddr_storage &ss_, Queue<clientdata> * get_rcv_q, int recv_size_, int my_sid_)
	 *
	 * @brief	DTLS受信受付処理
	 *
	 * @author	Nagoya University
	 * @date	2023/01/23
	 *
	 * @param	int		ソケットディスクリプタ
	 * @param	int		受信メッセージバッファ
	 * @return	int		受信サイズ
	 */
	//int SocketDTLS::Recvfrom(int fd, send_message &buf_, sockaddr_storage &ss_, Queue<clientdata> * get_rcv_q, int recv_size_, int my_sid_){
	int SocketDTLS::Recvfrom(int fd, sockaddr_storage &ss_, Queue<clientdata> * get_rcv_q, int recv_size_, unsigned long long my_sid_, const std::string &fd_cs_to_cs){
		BIO *bio;
		struct timeval timeout;

		union {
			struct sockaddr_storage ss;
			struct sockaddr_in s4;
			struct sockaddr_in6 s6;
		} client_addr;

		while (true) {
			memset(&client_addr, 0, sizeof(struct sockaddr_storage));

			// Create BIO
			bio = BIO_new_dgram(fd, BIO_NOCLOSE);
	
			timeout.tv_sec = 0;
			timeout.tv_usec = 0;
			BIO_ctrl(bio, BIO_CTRL_DGRAM_SET_RECV_TIMEOUT, 0, &timeout);
			
			ssl = SSL_new(ctx);
			SSL_set_bio(ssl, bio, bio);
			SSL_set_options(ssl, SSL_OP_COOKIE_EXCHANGE);

			while (1){
				int ret = DTLSv1_listen(ssl, (BIO_ADDR *) &client_addr) ;
				if(ret>=1){
					break;
				} else if (ret < 0) {
					perror("[Recvfrom] DTLSv1_listen fatal error");
					LOG4CXX_ERROR(logger, "DTLSv1_listen fatal error.");
					std::cout << "client_addr=" << &client_addr.s4.sin_addr.s_addr << std::endl;
					break;
				} else {
					LOG4CXX_INFO(logger, "DTLSv1_listen return zero.");
					//std::cout << "client_addr=" << &client_addr.s4.sin_addr.s_addr << std::endl;
				}
			};

			memcpy(&info->client_addr, &client_addr, sizeof(struct sockaddr_storage));
			info->ssl = ssl;
			// ソケット処理は別スレッドに委譲
			std::thread queuingThread(&SocketDTLS::socketDTLSProcess,this, info, get_rcv_q, recv_size_, my_sid_, fd_cs_to_cs);
			queuingThread.detach();

			sleep(1);
		}
	}

	/**
	 * @fn	int SocketDTLS::socketDTLSProcess(void *info, Queue<clientdata> * get_rcv_q, int recv_size_, int my_sid_)
	 *
	 * @brief	DTLS受信処理
	 *
	 * @author	Nagoya University
	 * @date	2023/01/23
	 *
	 * @param	void				アドレス情報格納構造体へのポインタ
	 * @param	send_message		電文格納用構造体
	 * @return	Queue<clientdata>	キュー連携
	 */
	//void SocketDTLS::socketDTLSProcess(void *info, send_message &buf_, Queue<clientdata> * get_rcv_q, int recv_size_, int my_sid_)
	void SocketDTLS::socketDTLSProcess(void *info, Queue<clientdata> * get_rcv_q, int recv_size_, unsigned long long my_sid_, const std::string &fd_cs_to_cs)
	{
		send_message buf_;
		// 分割ペイロードサイズ = 分割全体サイズ - ヘッダのサイズ (send_message構造体からペイロードサイズを引いたもの)
		int division_size = recv_size_ - (sizeof(buf_) - MSGSIZE);
		ssize_t len = 0;
		struct pass_info *pinfo = (struct pass_info *)info;
		SSL *ssl = pinfo->ssl;
		int ret;
		const int on = 1, off = 0;
		struct timeval timeout;
		int num_timeouts = 0, max_timeouts = 5;
		int fd;
		bool doFirst = false;

		double start_time = 0;
		double end_time = 0;
			
		sockaddr_un own_cs_addr;
		UdpProcClient owncsudpprocclient;
		//自クラウドのCS(ProcRcv)向け送信用socketFD作成
		own_cs_addr = owncsudpprocclient.Init(fd_cs_to_cs);

		int combination_map_clear_time_ = 100;
		UnorderedMap<std::string, time_t> flagment_data_receive_time_map;
		UnorderedMap<std::string, std::vector<std::string>> flagment_data_combination_map;
		std::thread th0(ClearUnorderedMap, 
				std::ref(combination_map_clear_time_),
				std::ref(flagment_data_combination_map),
				std::ref(flagment_data_receive_time_map));

		int flagment = 0, flagment_max = 0;
		std::string key = "";
		std::string combined_payload = "";
		//OPENSSL_assert(pinfo->client_addr.ss.ss_family == pinfo->server_addr.ss.ss_family);

		start_time = calcTime();

		fd = socket(pinfo->client_addr.ss.ss_family, SOCK_DGRAM, 0);

		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&on, (socklen_t)sizeof(on));
		//setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (const void*)&on, (socklen_t) sizeof(on));

		switch (pinfo->client_addr.ss.ss_family)
		{
		case AF_INET:
			pinfo->server_addr.ss.ss_family=AF_INET;
			if (bind(fd, (const struct sockaddr *) &pinfo->server_addr, sizeof(struct sockaddr_in))) {
				perror("bind");
				goto cleanup;
			}
			if (connect(fd, (struct sockaddr *)&pinfo->client_addr, sizeof(struct sockaddr_in)))
			{
					perror("connect");
					goto cleanup;
			}
			break;
		case AF_INET6:
			pinfo->server_addr.ss.ss_family=AF_INET6;
			setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&off, sizeof(off));
			if (bind(fd, (const struct sockaddr *) &pinfo->server_addr, sizeof(struct sockaddr_in6))) {
				perror("bind");
				goto cleanup;
			}
			if (connect(fd, (struct sockaddr *)&pinfo->client_addr, sizeof(struct sockaddr_in6)))
			{
					perror("connect");
					goto cleanup;
			}
			break;
		default:
			OPENSSL_assert(0);
			break;
		}

		/* Set new fd and set BIO to connected */
		BIO_set_fd(SSL_get_rbio(ssl), fd, BIO_NOCLOSE);
		BIO_ctrl(SSL_get_rbio(ssl), BIO_CTRL_DGRAM_SET_CONNECTED, 0, &pinfo->client_addr.ss);

		end_time = calcTime();
		LOG4CXX_DEBUG(logger, "Socketの生成 END 経過時間[ms]：" + std::to_string(end_time - start_time));

		start_time = calcTime();
		/* Finish handshake */
		// SSL通信の開始
		do
		{
			ret = SSL_accept(ssl);
		} while (ret == 0);
		if (ret < 0)
		{
			perror("SSL_accept");
			printf("%s\n", ERR_error_string(ERR_get_error(), buf_.dm2_payload));
			goto cleanup;
		}

		end_time = calcTime();
		LOG4CXX_DEBUG(logger, "HandShake END 経過時間[ms]：" + std::to_string(end_time - start_time));

		/* Set and activate timeouts */
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		BIO_ctrl(SSL_get_rbio(ssl), BIO_CTRL_DGRAM_SET_RECV_TIMEOUT, 0, &timeout);

		while (!(SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN) && num_timeouts < max_timeouts)
		{
			start_time = calcTime();
			memset(&buf_, 0, sizeof(buf_));
			len = SSL_read(ssl, &buf_, recv_size_);
			LOG4CXX_DEBUG(logger, "SSL_read length:" + std::to_string(len));
			if (len <= 0) {
				switch (SSL_get_error(ssl, len))
				{
					case SSL_ERROR_NONE:
						LOG4CXX_WARN(logger, "[socketDTLSProcess]SSL_ERROR_NONE");
						break;
					case SSL_ERROR_WANT_READ:
						/* Handle socket timeouts */
						if (BIO_ctrl(SSL_get_rbio(ssl), BIO_CTRL_DGRAM_GET_RECV_TIMER_EXP, 0, NULL))
						{
							LOG4CXX_INFO(logger, "[socketDTLSProcess]SSL_ERROR_WANT_READ");
						}
						/* Just try again */
						break;
					case SSL_ERROR_ZERO_RETURN:
						LOG4CXX_WARN(logger, "[socketDTLSProcess]SSL_ERROR_ZERO_RETURN");
						break;
					case SSL_ERROR_SYSCALL:
						LOG4CXX_WARN(logger, "[socketDTLSProcess]SSL_ERROR_SYSCALL");
						if (!handle_socket_error())
							goto cleanup;
						break;
					case SSL_ERROR_SSL:
						LOG4CXX_WARN(logger, "[socketDTLSProcess]SSL_ERROR_SSL");
						printf("[SocketDTLS::socketDTLSProcess]SSL read error: ");
						printf("%s (%d)\n", ERR_error_string(ERR_get_error(), buf_.dm2_payload), SSL_get_error(ssl, len));
						goto cleanup;
						break;
					default:
						LOG4CXX_WARN(logger, "[socketDTLSProcess]Unexpected error");
						printf("Unexpected error while reading!\n");
						goto cleanup;
						break;
				}
			} else {
				bool doSend = false;
				clientdata m_cdata;
				strcpy(m_cdata.from_ip, server_addr_str.c_str());
				//LOG4CXX_DEBUG(logger, "SSL_read payload:" + std::string(buf_.dm2_payload));
				if(buf_.flagment_sum == 1){
					m_cdata.msg = buf_;
					//std::cout << buf_.dm2_payload << std::endl;
					doSend = true;
				}else if(buf_.flagment_sum > 1){
					//std::cout << std::string(buf_.dm2_payload, division_size) << std::endl;
					//分割されたデータの結合処理
					flagment = buf_.flagment_offset;
					flagment_max = buf_.flagment_sum;
					key = std::to_string(buf_.src_station_id).append(std::to_string(buf_.flagment_duplication_check_id));

					//flagment_data_receive_time_mapを更新
					try{
						flagment_data_receive_time_map.UnorderedMapUpdate(key, time(NULL));
					}catch(std::out_of_range& e){
						if(flagment_data_receive_time_map.UnorderedMapInsert(key, time(NULL)) < 0){
							//std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "keyが存在するのでInsert中止" << std::endl;
						}
					}

					// flagment_data_combination_mapに受信途中がないかチェック
					if(flagment_data_combination_map.UnorderedMapKeyExistVector(key) == true){
						// 受信途中が存在する場合
						try{
							flagment_data_combination_map.UnorderedMapUpdateVectorValueThenDecrease(key, flagment, std::string(buf_.dm2_payload, division_size));
						}catch(std::out_of_range& oor){
							std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "out_of_range" << std::endl;
						}
						//keyのvectorの要素数がflagment_maxと同値なら全てデータが揃ったと判断し、結合及び削除してからnotifyする。
						combined_payload = flagment_data_combination_map.UnorderedMapVectorCombineAndDeletePlusSize(key, flagment_max);

						if(combined_payload.length() > 0){
							combined_payload.copy(buf_.dm2_payload, MSGSIZE);
							buf_.flagment_offset = 0;
							buf_.flagment_sum = 1;
							m_cdata.msg = buf_;
							//std::cout << buf_.dm2_payload << std::endl;
							doSend = true;
							flagment_data_receive_time_map.UnorderedMapErase(key);
						}
					}else{
						// 受信途中が存在しない場合
						try{
							flagment_data_combination_map.UnorderedMapInsertVectorPlusSize(key, flagment_max, flagment, std::string(buf_.dm2_payload, division_size));
						}catch(std::out_of_range& oor){
							std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "out_of_range" << std::endl;
						}
					}

				}else{
					std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "受信データのflagment_sum値が想定外:" << buf_.flagment_sum << std::endl;
				}
				if (doSend) {
					// 宛先SIDが自分(車両)宛てである場合
					if((m_cdata.msg).dst_station_id == my_sid_ || (m_cdata.msg).dst_station_id == 90000000) {
						get_rcv_q->Push(m_cdata);
					//宛先SIDが0の場合(想定外)
					} else if((m_cdata.msg).dst_station_id == 0){
						LOG4CXX_WARN(logger, "宛先SIDが0の場合(想定外)");
					//宛先SIDが自分(車両)宛て以外の場合
					} else {
						if (!doFirst) {
							LOG4CXX_WARN(logger, "[宛先SIDが自分(車両)宛て以外の場合] SID:" + std::to_string((m_cdata.msg).dst_station_id));
							doFirst = true;
						}
						if ((m_cdata.msg).transmission_flag <= 5) { 
							(m_cdata.msg).transmission_flag++; //転送フラグをプラス１
							owncsudpprocclient.Sendto(m_cdata.msg, own_cs_addr);
						}
					}
				}
			}
			end_time = calcTime();
			LOG4CXX_DEBUG(logger, "データ受信 END 経過時間[ms]：" + std::to_string(end_time - start_time));
		}
		// SSLの終了
		SSL_shutdown(ssl);
		stop_flag=1;
		th0.join();
		owncsudpprocclient.CloseSocketFd();

	cleanup:
	    start_time = calcTime();
		LOG4CXX_DEBUG(logger, "Close START");

		stop_flag=1;
		th0.join();
		owncsudpprocclient.CloseSocketFd();
		close(fd);
		SSL_free(ssl);

	    end_time = calcTime();
		LOG4CXX_DEBUG(logger, "Close END 経過時間[ms]：" + std::to_string(end_time - start_time));
	}

	/**
	 * @fn	int SocketDTLS::Sendto(send_message &buf_, sint send_size)
	 *
	 * @brief	DTLS送信処理
	 *
	 * @author	Nagoya University
	 * @date	2023/01/23
	 *
	 * @param	send_message	送信メッセージバッファ
	 * @param	send_size		送信サイズ
	 * @return	int				送信結果リターン値
	 */
	int SocketDTLS::Sendto(send_message &buf_, int send_size_){
		if (ssl_ == NULL) {
			std::cout << "SSL NULL ERROR!" << std::endl;
			return 0;
		}
		int len = SSL_write(ssl_, &buf_, send_size_);
		
		LOG4CXX_DEBUG(logger, "SSL_write length:" + std::to_string(len));
		if (len <= 0) {
			std::string errmsg;
			switch (SSL_get_error(ssl_, len)){
				case SSL_ERROR_NONE:
					errmsg = "SSL_write success with SSL_ERROR_NONE";
					break;
				case SSL_ERROR_WANT_WRITE:
					errmsg = "SSL_write failed with SSL_ERROR_WANT_WRITE";
					break;
				case SSL_ERROR_WANT_READ:
					errmsg = "SSL_write failed with SSL_ERROR_WANT_READ";
					break;
				case SSL_ERROR_SYSCALL:
					errmsg = "SSL_write failed with SSL_ERROR_SYSCALL";
					break;
				case SSL_ERROR_SSL:
					errmsg = "SSL_write failed with SSL_ERROR_SSL";
					break;
				default:
					errmsg = "SSL_write failed with unknown error";
					break;
			}
			LOG4CXX_WARN(logger, errmsg);
		}
		return len;
	}
	/**
	* @fn	void SocketDTLS::SendtoDivision(send_message &buf_, int send_size_)
	*
	* @brief	DTLS送信処理(sockaddr_un用)
	*
	* @author	Nagoya University
	* @date		2023/01/27
	*
	* @param buf_		送信データ構造体 
	* @param send_size_	分割サイズ
	*/
	int SocketDTLS::SendtoDivision(send_message &buf_, int send_size_){
		int header_size = sizeof (buf_) - MSGSIZE;						// ヘッダのサイズ
		int division_size = send_size_ - (sizeof(buf_) - MSGSIZE);		// 分割サイズ
		if (buf_.payload_size <= division_size) {
			return Sendto(buf_, header_size + buf_.payload_size);
		}
		int flagment_num = 0;
		struct timespec ts;
		char payload_[MSGSIZE];
		memcpy(payload_, buf_.dm2_payload, MSGSIZE);
		memset(buf_.dm2_payload, '\0', division_size);
		
		//送信データのflagment_duplication_check_idに現在時刻を設定
		timespec_get(&ts, TIME_UTC);
		buf_.flagment_duplication_check_id = ts.tv_sec * 1000000000 + ts.tv_nsec;
		
		//フラグメント数を計算
		flagment_num = buf_.payload_size / division_size;
		buf_.flagment_sum = flagment_num;
		
		//フラグメント時の端数の有無を求める
		if(buf_.payload_size % division_size != 0){
			flagment_num++;
		}
		buf_.flagment_sum = flagment_num;
		int len = 0;
		//フラグメントごとにデータを送信
		for(int i= 0; i < flagment_num - 1; i++){
			buf_.flagment_offset = i;
			memset(buf_.dm2_payload, '\0', MSGSIZE);
			memcpy(buf_.dm2_payload, payload_+ division_size * i, division_size);
			len = Sendto(buf_, send_size_);
		}
		if (len >= 0) {
			// 最後の送信サイズ = ヘッダサイズ + ペイロードサイズ - 分割して送信したペイロードサイズ
			int last_send_size_ = header_size + buf_.payload_size - division_size * (flagment_num - 1);
			buf_.flagment_offset = flagment_num - 1;
			memset(buf_.dm2_payload, '\0', MSGSIZE);
			memcpy(buf_.dm2_payload, payload_+ division_size * (flagment_num - 1), division_size);
			//std::cout << "[flagment data]No." << flagment_num - 1 << ":" << buf_.dm2_payload << std::endl;
			len = Sendto(buf_, last_send_size_);
		}
		return len;
	}

	/**
	 * @fn	void SocketDTLS::CloseDtls(void)
	 *
	 * @brief	DTLS切断
	 *
	 * @author	Nagoya University
	 * @date	2023/01/23
	 *
	 * @param	int		ソケットディスクリプタ
	 * @return	void
	 */
	void SocketDTLS::CloseDtls(){
		double start_time = calcTime();
		LOG4CXX_DEBUG(logger, "Close START");

		SSL_shutdown(ssl_);
		close(socket_res_);
		//DTLS2台送信している場合、1台が再接続に向けてSSL_freeを行うとコアダンプする。
		//ToDo: Init処理で2台共通で呼び出している箇所を見直す必要あり。
		//SSL_free(ssl_);

		double end_time = calcTime();
		LOG4CXX_DEBUG(logger, "Close END 経過時間[ms]：" + std::to_string(end_time - start_time));
	}

	/**
	 * @fn	void SocketDTLS::CloseSocketOnly(void)
	 *
	 * @brief	DTLS切断
	 *
	 * @author	Shinichi Kusayama
	 * @date	2025/9/17
	 *
	 * @return	void
	 */
	void SocketDTLS::CloseSocketOnly(){
		LOG4CXX_DEBUG(logger, "CloseSocketOnly START Socket No." + std::to_string(socket_res_));
		exit_flg = true;
		//SSL_shutdown(ssl_);
		close(socket_res_);
	}
	double SocketDTLS::calcTime()
	{
		/* 処理測定用につき、通常は未使用とする。
    	struct::timespec getTime;
    	clock_gettime(CLOCK_MONOTONIC, &getTime);
		std::cout << getTime.tv_sec << "." << getTime.tv_nsec << std::endl;
    	return (getTime.tv_sec + getTime.tv_nsec*1e-9) *1000;
		*/
		return 0.0;
	}
	void SocketDTLS::ClearUnorderedMap(
				int& combination_map_clear_time_th,
				UnorderedMap<std::string, std::vector<std::string>>& flagment_data_combination_map_th,
				UnorderedMap<std::string, time_t>& flagment_data_receive_time_map_th
				)
	{
		while(!stop_flag){
			std::string key_ = "0";
			while(1){
				//◆結合MAP管理用MAPにて現在時刻より指定時間以上古いデータの車両SID＋フラグメントシーケンス番号を取得
				key_ = flagment_data_receive_time_map_th.UnorderedMapOldKeyString(combination_map_clear_time_th);
				if(key_ != "0"){
					//◆取得した車両SIDをkey値として、結合MAPからエントリを削除。
					flagment_data_combination_map_th.UnorderedMapErase(key_);
					//◆取得した車両SIDをkey値として、結合MAP管理用MAPからエントリを削除。
					flagment_data_receive_time_map_th.UnorderedMapErase(key_);
				}else{
					break;
				}
			}
			//sleep(combination_map_clear_time_th);
			sleep(1);
		}
	}
}
