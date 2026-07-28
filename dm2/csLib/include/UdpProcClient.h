#ifndef UDPPROCCLIENT_H
#define UDPPROCCLIENT_H

#include "Cs.h"
#include "UdpClient.h"
namespace CS{
	/**
	 * @class	UdpProcClient
	 *
	 * @brief	DM2.0UDPプロセス通信の送信側(Client)処理クラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */
	class UdpProcClient:public SocketUdp, public IUdpClient {
	private:
		int server_d_; // ソケットディスクリプタ(クライアント)
		struct sockaddr_un server_addr_; // ソケットのアドレス(クライアント)

	public:
		UdpProcClient():SocketUdp(){
			server_d_ = 0;
			server_addr_ = {0};
		}
		
		virtual ~UdpProcClient(){
			close(server_d_);
		}

		void CloseSocketFd();

		sockaddr_un Init(const std::string &fd);

		sockaddr_un InitClient(const char* fd_);

		void Init(const std::string& fd_name, const std::string& port, const std::string& ip) override;
		int SendClientData(struct clientdata &buf_) override;
		int SendPacket(struct send_message &buf_) override;
	};
}
#endif