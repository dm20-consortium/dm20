#include "Cs.h"
namespace CS{
	/**
	 * @fn	int UdpProcServer::Init(const char* fd_name)
	 *
	 * @brief	初期化
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	fd_name	ファイルディスクリプタ名
	 *
	 * @return	ソケットID
	 */
    int UdpServer::Init(const std::string& fd_name, std::string interface_name, std::string port_no) {
        if (!interface_name.empty() && !port_no.empty()) {
            server_.reset(new UdpNwServer());
		} else {
            server_.reset(new UdpProcServer());
		}
        return server_->Init(fd_name, interface_name, port_no);
    }
	/**
	 * @fn	int UdpProcServer::Init(const std::string& fd_name)
	 *
	 * @brief	初期化
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	fd_name	ファイルディスクリプタ名
	 *
	 * @return	ソケットID
	 */
    int UdpServer::Init(const std::string& fd_name) {
        return Init(fd_name);
    }
	/**
	 * @fn	int RecvPacket(send_message &buf_, int socket_res_) 
	 *
	 * @brief	受信処理（send_message型）
	 *
	 * @author	Nagoya University
	 * @date	2026/6/4
	 *
	 * @param	buf_  			受信バッファ
	 * @param	socket_res_  	ソケット情報
	 *
	 * @return	受信サイズ
	 */
	int UdpServer::RecvPacket(send_message &buf_, int socket_res_) {
		return server_->RecvPacket(buf_, socket_res_);
	}
	/**
	 * @fn	int RecvClientData(clientdata &buf_, int socket_res_)
	 *
	 * @brief	受信処理（clientdata型）
	 *
	 * @author	Nagoya University
	 * @date	2026/6/4
	 *
	 * @param	buf_  			受信バッファ
	 * @param	socket_res_  	ソケット情報
	 *
	 * @return	受信サイズ
	 */
	int UdpServer::RecvClientData(clientdata &buf_, int socket_res_) {
		return server_->RecvClientData(buf_, socket_res_);
	}
}