#ifndef UDPMCNWSERVER_H
#define UDPMCNWSERVER_H

#include "Cs.h"
#include <openssl/ssl.h>

namespace CS{

	/**
	 * @class	UdpMcNwServer
	 *
	 * @brief	DM2.0UDPネットワーク通信の受信側(Server)処理クラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */
	class UdpMcNwServer:public SocketUdp{
	private:
		/** @brief	socketディスクリプタ */
		int sockd;
		/** @brief	hints */
		struct addrinfo hints;
		/** @brief	*res */
		struct addrinfo *res;
		/** @brief	ss_ */
		struct sockaddr_storage ss_;
		/** @brief	gr_ */
		struct group_req gr_;
		/** @brief	*dst_res */
		struct addrinfo *dst_res = (struct addrinfo *)&ss_;
		/** @brief	Socket関連関数の戻り値 */
		int sock_res;
		
		std::string dst_multicast_ip_;
		std::string port_no_;
	public:

		/**
		 * @fn	UdpMcNwServer()
		 *
		 * @brief	コンストラクタ
		 * 			パラメータの初期化
		 *
		 * @author	Nagoya University
		 * @date	2018/03/14
		 */

		UdpMcNwServer():SocketUdp(){
			sockd = 0;
			hints = {0};
			res = {0};
			ss_ = {0};
			gr_ = {0};
			dst_res = {0};
			sock_res = 0;
		}

		/**
		 * @fn	virtual ~UdpMcNwServer()
		 *
		 * @brief	デストラクタ
		 * 			ソケットのクローズ処理
		 * 			addrinfo ストレージの解放
		 *
		 * @author	Nagoya University
		 * @date	2018/03/14
		 */

		virtual ~UdpMcNwServer(){
			close(sockd);
			freeaddrinfo(res);
			freeaddrinfo(dst_res);
		}
		int Init(std::string port_no, std::string rcv_interface_name, int ip_ver);

		int Init(std::string port_no, std::string rcv_interface_name, std::string dst_multicast_ip, const int _IP_version);

		int Init(std::string port_no, std::string rcv_interface_name_1, std::string rcv_interface_name_2, std::string dst_multicast_ip, const int _IP_version);

		int InitServer(std::string port_no, std::string rcv_interface_name, std::string dst_multicast_ip, const int _IP_version);

		int InitServer(std::string port_no, std::string rcv_interface_name_1, std::string rcv_interface_name_2, std::string dst_multicast_ip, const int _IP_version);

	};
}
#endif
