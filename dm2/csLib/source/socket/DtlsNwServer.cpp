#include "Cs.h"
#include "DtlsNwServer.h"

#include <string>
namespace CS{

	/**
	 * @fn	int DtlsNwServer::Init(std::string interface_name, std::string port_no, std::string _anycast_address)
	 *
	 * @brief	初期化(受信IPアドレス指定なし、受信インタフェース名指定)
	 *
	 * @author	Nagoya University
	 * @date	2023/01/23
	 *
	 * @param	interface_name  	インタフェース名
	 * @param	port_no				ポート番号
	 * @param	_anycast_address	エニーキャストアドレス
	 *
	 * @return	ソケットID
	 */
	int DtlsNwServer::Init(std::string interface_name, int ip_ver, std::string port_no, std::string _anycast_address, const std::string &confDirPath)
	{
		return InitServer(interface_name, ip_ver, port_no, _anycast_address, confDirPath);
	}

	/**
	 * @fn	int DtlsNwServer::InitServer(std::string interface_name, std::string port_no, std::string _anycast_address)
	 *
	 * @brief	接続シーケンス(受信IPアドレス指定なし、受信インタフェース名指定)
	 *
	 * @author	Nagoya University
	 * @date	2023/01/23
	 *
	 * @param	interface_name  	インタフェース名
	 * @param	port_no				ポート番号
	 * @param	_anycast_address	エニーキャストアドレス
	 *
	 * @return	ソケットID
	 */
	int DtlsNwServer::InitServer(std::string interface_name, int ip_ver, std::string port_no, std::string _anycast_address, const std::string &confDirPath){
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC; //IPv4/IPv6両方対応
		hints.ai_socktype = SOCK_DGRAM; //UDP送信
		hints.ai_flags = AI_PASSIVE;
		std::string local_address;
		while (true) {
			local_address = GetIPifaddrs(interface_name, "", ip_ver);
			if (local_address == "") {
				sleep(10);
			}else {
				break;
			}
		}
		sock_res = getaddrinfo(local_address.c_str(), port_no.c_str(), &hints, &res);

		if(sock_res != 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "getaddrinfo fail." << std::endl;
			perror("getaddrinfo");
			exit(EXIT_FAILURE);
		}
		
		//DTLSサーバー開始
		sockd = StartDtlsServer(stoi(port_no), (char *)local_address.c_str(), confDirPath);

		return sockd;
	}
}
