#ifndef UDPCLIENT_H
#define UDPCLIENT_H
#include <iostream>
#include <memory>
#include "IUdpClient.h" 
namespace CS{
	/**
	 * @class	UdpProcClient
	 *
	 * @brief	DM2.0UDPプロセス通信の送信側(Client)処理クラス
	 *
	 * @author	Shinichi Kusayama
	 * @date		2026/6/4
	 */
	class UdpClient{
	private:
    	std::unique_ptr<IUdpClient> client_;
	public:
		UdpClient() {
		}
		void Init(const std::string &fd_name, const std::string &is_port_number, const std::string &ip_address);
		void Init(const std::string &fd_name);
		int SendClientData(struct clientdata &buf_);
		int SendPacket(struct send_message &buf_);
		void CloseSocketFd();
	};
}
#endif