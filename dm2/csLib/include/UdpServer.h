#ifndef UDPSERVER_H
#define UDPSERVER_H
#include <iostream>
#include <memory>
#include "IUdpServer.h" 

namespace CS{
	/**
	 * @class	UdpProcServer
	 *
	 * @brief	DM2.0UDPプロセス通信の受信側(Server)処理クラス
	 *
	 * @author	Shinichi Kusayama
	 * @date		2026/6/4
	 */
	class UdpServer{
	private:
    	std::unique_ptr<IUdpServer> server_;
			
	public:
		UdpServer(){
		}
		int Init(const std::string& fd_name, std::string interface_name, std::string port_no);
		int Init(const std::string& fd_name);
		int RecvPacket(send_message &buf_, int socket_res_);
		int RecvClientData(clientdata &buf_, int socket_res_);
		virtual ~UdpServer(){
		}
	};
}
#endif