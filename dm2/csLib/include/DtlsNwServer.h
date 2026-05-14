#ifndef DTLSNWSERVER_H
#define DTLSNWSERVER_H

#include "Cs.h"
#include "SocketDTLS.h"

namespace CS{
	/**
	 * @class	DtlsNwServer
	 *
	 * @brief	DM2.0UDPネットワーク通信の受信側(Server)処理クラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */
	class DtlsNwServer:public SocketDTLS{
	private:
		int sockd; //socketディスクリプタ
		struct addrinfo hints;
		struct addrinfo *res;
		int sock_res; //Socket関連関数の戻り値
		
	public:
		DtlsNwServer():SocketDTLS(){
			sockd = 0;
			hints = {0};
			res = {0};
			sock_res = 0;
		}
		
		virtual ~DtlsNwServer(){
			close(sockd);
			freeaddrinfo(res);
		}
		
		int Init(std::string interface_name, int ip_ver, std::string port_no, std::string ipv6_anycast_address, const std::string &confDirPath);

		int InitServer(std::string interface_name, int ip_ver, std::string port_no, std::string ipv6_anycast_address, const std::string &confDirPath);


	};
}
#endif