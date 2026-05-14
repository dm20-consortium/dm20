#include "Cs.h"
namespace CS{
	/**
	 * @fn	sockaddr_un UdpProcClient::Init(const char* fd)
	 *
	 * @brief	初期化
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	fd	ファイルディスクリプタ
	 *
	 * @return	sockaddr_un構造体
	 */
	sockaddr_un UdpProcClient::Init(const std::string &fd){
		return InitClient(fd.c_str());
	}

	/**
	 * @fn	sockaddr_un UdpProcClient::InitClient(const char* fd_)
	 *
	 * @brief	接続シーケンス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	fd_	ファイルディスクリプタ
	 *
	 * @return	sockaddr_un構造体
	 */
	sockaddr_un UdpProcClient::InitClient(const char* fd_){
		//アドレスの初期化
		server_addr_.sun_family = PF_UNIX;
		strcpy(server_addr_.sun_path,fd_);
		
		// //ソケット作成
		server_d_ = CreateSocket(PF_UNIX, SOCK_DGRAM);
		if(server_d_ < 0){
			perror("CreateSocket in UdpProcClient::InitClient");
		}

		return server_addr_;
	}

	void UdpProcClient::CloseSocketFd(){
		CloseSocket();
	}
}


