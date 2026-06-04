#ifndef UDPNWCLIENT_H
#define UDPNWCLIENT_H

#include "Cs.h"
#include "UdpClient.h"
#include <openssl/ssl.h>

namespace CS{

	/**
	 * @class	UdpNwClient
	 *
	 * @brief	DM2.0UDPネットワーク通信の送信側(Client)処理クラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */
	class UdpNwClient:public SocketUdp, public IUdpClient {
	private:
		/** @brief	socketディスクリプタ */
		int sockd;
		/** @brief	hints */
		struct addrinfo hints;
		/** @brief	*res */
		struct addrinfo *res;
		/** @brief	*res_src */
		struct addrinfo *res_src;
		/** @brief	Socket関連関数の戻り値 */
		int sock_res;
	
		std::string port_no_;
	public:
		/**
		 * @fn	UdpNwClient()
		 *
		 * @brief	コンストラクタ
		 * 			パラメータの初期化
		 *
		 * @author	Nagoya University
		 * @date	2018/03/14
		 */
		UdpNwClient():SocketUdp(){
			sockd = 0;
			sock_res = 0;
			hints = {0};
			res = {0};
			res_src = {0};
		}

		/**
		 * @fn	virtual ~UdpNwClient()
		 *
		 * @brief	デストラクタ
		 * 			ソケットのクローズ処理
		 * 			addrinfoストレージの解放
		 *
		 * @author	Nagoya University
		 * @date	2018/03/14
		 */
		virtual ~UdpNwClient(){
			close(sockd);
			freeaddrinfo(res);
			freeaddrinfo(res_src);
		}

		addrinfo Init(std::string port_no, std::string dst_ip);

		addrinfo Init_path(std::string port_no, std::string dst_ip, std::string conf_dir_path);

		void Init(const std::string& fd_name, const std::string& port, const std::string& ip) override;
		int SendClientData(struct clientdata &buf_) override;
		int SendPacket(struct send_message &buf_) override;
		
		addrinfo Init_v6(std::string port_no, std::string dst_ip, std::string if_name);

		addrinfo InitClient(std::string port_no, std::string dst_ip);

		addrinfo InitClient_v6(std::string port_no, std::string dst_ip, std::string if_name);

		void CloseSocketFd();

		int Sendto(send_message &buf_, addrinfo &addr_);
		int Sendto(send_message &buf_, addrinfo &addr_, int send_size_);
	};
}
#endif
