#ifndef UDPMCNWCLIENT_H
#define UDPMCNWCLIENT_H

#include "Cs.h"
#include <openssl/ssl.h>

namespace CS{

	/**
	 * @class	UdpMcNwClient
	 *
	 * @brief	DM2.0UDPネットワーク通信の送信側(Client)処理クラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */
	class UdpMcNwClient:public SocketUdp{
	private:
		/** @brief	socketディスクリプタ */
		int sockd;
		struct addrinfo hints;
		struct addrinfo *res;
		struct addrinfo *res_src;
		struct sockaddr_storage ss_;
		struct group_req gr_;
		struct addrinfo_in *res_in = (struct addrinfo_in *)&ss_;
		/** @brief	Socket関連関数の戻り値 */
		int sock_res;
		/** @brief	lTTL */
		u_long lTTL = 2;

		std::string port_no_;
			
	public:

		/**
		 * @fn	UdpMcNwClient()
		 *
		 * @brief	コンストラクタ
		 * 			パラメータの初期化
		 *
		 * @author	Nagoya University
		 * @date	2018/03/14
		 */
		UdpMcNwClient():SocketUdp(){
			sockd = 0;
			hints = {0};
			res = {0};
			res_src = {0};
			ss_ = {0};
			gr_ = {0};
			res_in = {0};
			sock_res = 0;
		}

		/**
		 * @fn	virtual ~UdpMcNwClient()
		 *
		 * @brief	デストラクタ
		 * 			ソケットのクローズ処理
		 * 			addrinfoストレージの解放
		 *
		 * @author	Nagoya University
		 * @date	2018/03/14
		 */
		virtual ~UdpMcNwClient(){
			close(sockd);
			freeaddrinfo(res);
		}

		addrinfo Init(std::string port_no, std::string dst_ip, std::string interface_name, std::string anycast_address, const int _IP_version);

		addrinfo InitClient(std::string port_no, std::string dst_ip, std::string interface_name, std::string src_ip, const int _IP_version);

		int Sendto(send_message &buf_, addrinfo &addr_);
		int Sendto(send_message &buf_, addrinfo &addr_, int send_size_);
	};
}
#endif
