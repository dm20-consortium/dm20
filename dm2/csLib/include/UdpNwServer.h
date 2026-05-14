#ifndef UDPNWSERVER_H
#define UDPNWSERVER_H

#include "Cs.h"
#include <openssl/ssl.h>
namespace CS{
	/**
	 * @class	UdpNwServer
	 *
	 * @brief	DM2.0UDPネットワーク通信の受信側(Server)処理クラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */
	class UdpNwServer:public SocketUdp{
	private:
		int sockd; //socketディスクリプタ
		struct addrinfo hints;
		struct addrinfo *res;
		int sock_res; //Socket関連関数の戻り値
		
		std::string port_no_;
	public:
		UdpNwServer():SocketUdp(){
			sockd = 0;
			hints = {0};
			res = {0};
			sock_res = 0;
		}
		
		virtual ~UdpNwServer(){
			close(sockd);
			freeaddrinfo(res);
		}
		
		int Init(std::string port_no);

		int InitServer(std::string port_no);
		
		int Init(std::string receive_ip_addr, std::string port_no);

		int InitServer(std::string receive_ip_addr, std::string port_no);
		
		int Init_v6(std::string interface_name, std::string port_no, std::string ipv6_anycast_address);

		int Init(std::string interface_name, std::string port_no, std::string ipv6_anycast_address, std::string &rtn_address, const int _IP_version, const std::string &confDirPath);

		int InitServer(std::string interface_name, std::string port_no, std::string ipv6_anycast_address, std::string &rtn_address, const int _IP_version);

	};
}
#endif