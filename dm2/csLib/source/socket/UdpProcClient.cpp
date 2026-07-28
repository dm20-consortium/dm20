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
	* @fn	void UdpProcClient::Init(const std::string& fd_name, const std::string& port, const std::string& ip) 
	*
	* @brief	Init 処理（切り替え用）
	*
	* @author	Shinichi Kusayama
	* @date     2026/6/4
	*
	* @param [in,out]	fd_name FDファイル名
	* @param [in,out]	port	宛先ポート番号
	* @param [in,out]	ip	宛先IPアドレス
	*
	*/
	void UdpProcClient::Init(const std::string& fd_name, const std::string& port, const std::string& ip) 
	{
		server_addr_ = Init(fd_name); // 既存呼び出し
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

	/**
	* @fn	void UdpProcClient::CloseSocketFd()
	*
	* @brief	ソケットクローズ
	*
 	 * @author	Nagoya University
     * @date	2018/03/14
	*
	*/
	void UdpProcClient::CloseSocketFd(){
		CloseSocket();
	}
	/**
	* @fn	int UdpProcClient::SendClientData(struct clientdata &buf_)
	*
	* @brief	sendto送信(clientdata型)
	*
	* @author	Shinichi Kusayama
	* @date	2026/6/4
	*
	* @param [in,out]	buf_ 	送信メッセージバッファ
	*
	* @return	int sendtoの戻り値
	*/
	int UdpProcClient::SendClientData(struct clientdata &buf_) {
		return Socket::Sendto(buf_, server_addr_);
	}
	/**
	* @fn	int UdpProcClient::SendPacket(struct send_message &buf_)
	*
	* @brief	sendto送信(send_message型)
	*
	* @author	Shinichi Kusayama
	* @date	2026/6/4
	*
	* @param [in,out]	buf_ 	送信メッセージバッファ
	*
	* @return	int sendtoの戻り値
	*/
	int UdpProcClient::SendPacket(struct send_message &buf_) {
		return Socket::Sendto(buf_, server_addr_);
	}
}


