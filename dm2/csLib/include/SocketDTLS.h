#ifndef SOCKETDTLS_H
#define SOCKETDTLS_H

#include "Cs.h"
#include "UnorderedMap.h"
#include "Socket.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/conf.h>
#include <openssl/engine.h>
#include "Queue.h"

#include "DtlsContext.h"

namespace CS{
	//#define BUFFER_SIZE          (1<<16)
	#define BUFFER_SIZE          (1<<8)

	/**
	 * @class	SocketDTLS
	 *
	 * @brief	DM2.0DTLSネットワーク通信ソケットクラス
	 *
	 * @author	Nagoya University
	 * @date	2023/01/23
	 */
	class SocketDTLS : public Socket
	{
	private:

		/** @brief ssl管理ブロック */
		SSL *ssl_ = NULL;		// Client用
		SSL *ssl  = NULL;		// Server用
		bool first_connect = false;						// 初回接続の判定フラグ
		const int sleep_time_for_reconnect = 10;		// 切断時から再接続するまでの待機時間
		std::string server_addr_str;
		bool exit_flg = false;
		/** @brief	socketファイルディスクリプタ */
		int socket_res_;

		struct pass_info {
			union {
				struct sockaddr_storage ss;
				struct sockaddr_in6 s6;
				struct sockaddr_in s4;
			} server_addr, client_addr;
			SSL *ssl;
		};

		//static struct pass_info *info;
		static pthread_mutex_t* mutex_buf;
		struct pass_info *info;
		SSL_CTX *ctx;

		static unsigned long id_function(void);
		static int THREAD_setup(void);
		static int THREAD_cleanup(void);
		static int handle_socket_error(void);
		static void* connection_handle(void *info);
		//void socketDTLSProcess(void *info ,send_message &buf_, Queue<clientdata> * get_rcv_q, int recv_size_, int my_sid_);
		void socketDTLSProcess(void *info, Queue<clientdata> * get_rcv_q, int recv_size_, unsigned long long my_sid_, const std::string &fd_cs_to_cs);
		static double calcTime(void);
		static void ClearUnorderedMap(int& combination_map_clear_time_, UnorderedMap<std::string, std::vector<std::string>>& flagment_data_combination_map_, 
		              UnorderedMap<std::string, time_t>& flagment_data_receive_time_map_);
		
	public:
		/**
		 * @fn	SocketDTLS::SocketDTLS()
		 *
		 * @brief	コンストラクタ
		 *
		 * @author	Nagoya University
		 * @date	2023/01/23
		 */
		SocketDTLS():Socket(){
		}

		/**
		 * @fn	virtual SocketDTLS::~SocketDTLS()
		 *
		 * @brief	デストラクタ
		 *
		 * @author	Nagoya University
		 * @date	2023/01/23
		 */
		virtual ~SocketDTLS(){}

		int StartDtlsServer(int port, char *local_address, const std::string &confDirPath);
		int StartDtlsClient(char *remote_address, char *local_address, int port, const std::string &confDirPath, bool &exit_flg_);

		//int Recvfrom(int socket_res_, send_message &buf_, sockaddr_storage &ss_,Queue<clientdata> * get_rcv_q, int recv_size_, int my_sid_);
		int Recvfrom(int socket_res_, sockaddr_storage &ss_,Queue<clientdata> * get_rcv_q, int recv_size_, unsigned long long my_sid_, const std::string &fd_cs_to_cs);

		int Sendto(send_message &buf_, int send_size_);
		int SendtoDivision(send_message &buf_, int send_size_);

		void CloseDtls();
		void CloseSocketOnly();
	};
}
#endif
