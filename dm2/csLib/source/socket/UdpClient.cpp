#include "UdpClient.h"
#include "UdpProcClient.h"
using namespace CS;
namespace CS{
	/**
	 * @fn	void UdpClient::Init(const std::string &fd_name, const std::string &port, const std::string &ip) 
	 *
	 * @brief	初期化
	 *
	* @author	Shinichi Kusayama
	* @date		2026/6/4
	 *
	 * @param	fd_name	ファイルディスクリプタ
	 * @param	port	ポート番号
	 * @param	ip		IPアドレス
	 *
	 */
    void UdpClient::Init(const std::string &fd_name, const std::string &port, const std::string &ip) {
        if (!port.empty() && !ip.empty()) {
            client_.reset(new UdpNwClient());
		} else {
            client_.reset(new UdpProcClient());
		}
        client_->Init(fd_name, port, ip);
    }
	/**
	 * @fn	void UdpClient::Init(const std::string &fd_name)
	 *
	 * @brief	初期化
	 *
	* @author	Shinichi Kusayama
	* @date		2026/6/4
	 *
	 * @param	fd_name	ファイルディスクリプタ
	 *
	 */
    void UdpClient::Init(const std::string &fd_name) {
		Init(fd_name, "", "");
    }
	/**
	* @fn	void UdpClient::CloseSocketFd()
	*
	* @brief	ソケットクローズ
	*
	* @author	Shinichi Kusayama
	* @date		2026/6/4
	*
	*/
	void UdpClient::CloseSocketFd() {
		client_->CloseSocketFd();
	}
	/**
	* @fn	int UdpClient::SendClientData(struct clientdata &buf_)
	*
	* @brief	sendto送信(clientdata型)
	*
	* @author	Shinichi Kusayama
	* @date		2026/6/4
	*
	* @param [in,out]	buf_ 	送信メッセージバッファ
	*
	* @return	int sendtoの戻り値
	*/
	int UdpClient::SendClientData(struct clientdata &buf_) {
		return client_->SendClientData(buf_);
	}
	/**
	* @fn	int UdpClient::SendPacket(struct send_message &buf_)
	*
	* @brief	sendto送信(send_message型)
	*
	* @author	Shinichi Kusayama
	* @date		2026/6/4
	*
	* @param [in,out]	buf_ 	送信メッセージバッファ
	*
	* @return	int sendtoの戻り値
	*/
	int UdpClient::SendPacket(struct send_message &buf_) {
		return client_->SendPacket(buf_);
	}
}


