#ifndef UDPPROCSERVER_H
#define UDPPROCSERVER_H

#include "Cs.h"

namespace CS{
	/**
	 * @class	UdpProcServer
	 *
	 * @brief	DM2.0UDPプロセス通信の受信側(Server)処理クラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */
	class UdpProcServer:public SocketUdp{
	private:
		int server_d_; // ソケットディスクリプタ(サーバ)
		struct sockaddr_un server_addr_; // ソケットのアドレス(サーバ)
			
	public:
		UdpProcServer():SocketUdp(){
			server_d_ = 0;
			server_addr_ = {0};
		}
		
		virtual ~UdpProcServer(){
			close(server_d_);
		}
		
		int Init(const std::string &fd_name);

		int InitServer(const char* fd_name);

		void DeleteFd(const char* fd_name);
	};
}
#endif