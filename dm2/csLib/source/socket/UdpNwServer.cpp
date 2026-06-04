#include "Cs.h"
namespace CS{
	/**
	 * @fn	int UdpNwServer::Init(std::string port_no)
	 *
	 * @brief	初期化(受信IPアドレス指定なし)
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	port_no	ポート番号
	 *
	 * @return	ソケットID
	 */
	int UdpNwServer::Init(std::string port_no)
	{
		port_no_ = port_no;
		return InitServer(port_no);
	}

	/**
	 * @fn	int UdpNwServer::InitServer(std::string port_no)
	 *
	 * @brief	接続シーケンス(受信IPアドレス指定なし)
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	port_no	ポート番号
	 *
	 * @return	ソケットID
	 */
	int UdpNwServer::InitServer(std::string port_no){
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC; //IPv4/IPv6両方対応
		hints.ai_socktype = SOCK_DGRAM; //UDP送信
		hints.ai_flags = AI_PASSIVE;
		sock_res = getaddrinfo(NULL, port_no.c_str(), &hints, &res);
		if(sock_res != 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "getaddrinfo fail." << std::endl;
			perror("getaddrinfo");
			exit(EXIT_FAILURE);
		}
		sockd = CreateSocket(res->ai_family, res->ai_socktype);
		if(sockd < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "CreateSocket fail." << std::endl;
			exit(EXIT_FAILURE);
		}
		if(Bind(sockd, res->ai_addr) < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Bind fail." << std::endl;
			sockd = BIND_ERROR;
		}

		return sockd;
	}

	/**
	 * @fn	int UdpNwServer::Init(std::string receive_ip_addr, std::string port_no)
	 *
	 * @brief	初期化(受信IPアドレス指定)
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	receive_ip_addr	受信IPアドレス
	 * @param	port_no		   	ポート番号
	 *
	 * @return	ソケットID
	 */
	int UdpNwServer::Init(std::string receive_ip_addr, std::string port_no)
	{
		port_no_ = port_no;
		return InitServer(receive_ip_addr, port_no);
	}

	/**
	 * @fn	int UdpNwServer::InitServer(std::string receive_ip_addr, std::string port_no)
	 *
	 * @brief	接続シーケンス(受信IPアドレス指定)
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	receive_ip_addr	受信IPアドレス
	 * @param	port_no		   	ポート番号
	 *
	 * @return	ソケットID
	 */
	int UdpNwServer::InitServer(std::string receive_ip_addr, std::string port_no){
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC; //IPv4/IPv6両方対応
		hints.ai_socktype = SOCK_DGRAM; //UDP送信
		hints.ai_flags = AI_PASSIVE;
		sock_res = getaddrinfo(receive_ip_addr.c_str(), port_no.c_str(), &hints, &res);
		if(sock_res != 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "getaddrinfo fail. IP:" << receive_ip_addr  << std::endl;
			perror("getaddrinfo");
			exit(EXIT_FAILURE);
		}
		sockd = CreateSocket(res->ai_family, res->ai_socktype);
		if(sockd < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "CreateSocket fail. IP:" << receive_ip_addr << std::endl;
			exit(EXIT_FAILURE);
		}
		if(Bind(sockd, res->ai_addr, res->ai_addrlen) < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Bind fail. IP:" << receive_ip_addr << std::endl;
			exit(EXIT_FAILURE);
		}

		return sockd;
	}

	/**
	 * @fn	int UdpNwServer::Init(std::string interface_name, std::string port_no, std::string _anycast_address, std::string &rtn_address, const int _IP_version)
	 *
	 * @brief	初期化(受信IPアドレス指定なし、受信インタフェース名指定)
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	interface_name  	インタフェース名
	 * @param	port_no				ポート番号
	 * @param	_anycast_address	エニーキャストアドレス
	 * @param	rtn_address			サーバとして割り当てられるアドレス
	 * @param	_IP_version			IPバージョン
	 *
	 * @return	ソケットID
	 */
	int UdpNwServer::Init(std::string interface_name, std::string port_no, std::string _anycast_address, std::string &rtn_address, const int _IP_version, const std::string &confDirPath)
	{
		OpenSSL_add_all_algorithms();
		port_no_ = port_no;
		conf_dir_path_ = confDirPath;
		return InitServer(interface_name, port_no, _anycast_address, rtn_address, _IP_version);
	}
	/**
	 * @fn	int Init(const std::string& fd_name, std::string interface_name, std::string port_no)
	 *
	 * @brief	初期化(受信IPアドレス指定なし、受信インタフェース名指定)
	 *
	 * @author	Nagoya University
	 * @date	2026/6/4
	 *
	 * @param	fd_name  			FDファイル名指定
	 * @param	interface_name  	インタフェース名
	 * @param	port_no				ポート番号
	 *
	 * @return	ソケットID
	 */
	int UdpNwServer::Init(const std::string& fd_name, std::string interface_name, std::string port_no) {
		port_no_ = port_no;
		std::string rtn_address;
		return InitServer(interface_name, port_no, "", rtn_address, 4);
	}
	/**
	 * @fn	int UdpNwServer::Init_v6(std::string interface_name, std::string port_no, std::string _anycast_address)
	 *
	 * @brief	初期化(受信IPアドレス指定なし、受信インタフェース名指定)
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	interface_name  	インタフェース名
	 * @param	port_no				ポート番号
	 * @param	_anycast_address	エニーキャストアドレス
	 *
	 * @return	ソケットID
	 */
	int UdpNwServer::Init_v6(std::string interface_name, std::string port_no, std::string _anycast_address)
	{
		port_no_ = port_no;
		std::string rtn_address;
		return InitServer(interface_name, port_no, _anycast_address, rtn_address, 6);
	}

	/**
	 * @fn	int UdpNwServer::InitServer(std::string interface_name, std::string port_no, std::string _anycast_address, std::string &rtn_address, const int _IP_version)
	 *
	 * @brief	接続シーケンス(受信IPアドレス指定なし、受信インタフェース名指定)
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	interface_name  	インタフェース名
	 * @param	port_no				ポート番号
	 * @param	_anycast_address	エニーキャストアドレス
	 * @param	rtn_address			サーバとして割り当てられるアドレス
	 * @param	_IP_version			IPバージョン
	 *
	 * @return	ソケットID
	 */
	int UdpNwServer::InitServer(std::string interface_name, std::string port_no, std::string _anycast_address, std::string &rtn_address, const int _IP_version){
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC; //IPv4/IPv6両方対応
		hints.ai_socktype = SOCK_DGRAM; //UDP送信
		hints.ai_flags = AI_PASSIVE;
		if (interface_name.empty()) {
			sock_res = getaddrinfo(nullptr, port_no.c_str(), &hints, &res);
		} else {
			rtn_address = GetIPifaddrs(interface_name, _anycast_address, _IP_version);
			sock_res = getaddrinfo(rtn_address.c_str(), port_no.c_str(), &hints, &res);
		}
		if(sock_res != 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "getaddrinfo fail." << std::endl;
			perror("getaddrinfo");
			exit(EXIT_FAILURE);
		}
		sockd = CreateSocket(res->ai_family, res->ai_socktype);
		if(sockd < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "CreateSocket fail." << std::endl;
			exit(EXIT_FAILURE);
		}
		if(Bind(sockd, res->ai_addr, res->ai_addrlen) < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Bind fail." << std::endl;
			return -1;
		}

		return sockd;
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
	int UdpNwServer::RecvPacket(send_message &buf_, int socket_res_) {
		sockaddr_storage ss_;
		return Socket::Recvfrom(socket_res_, buf_, ss_);
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
	int UdpNwServer::RecvClientData(clientdata &buf_, int socket_res_) {
		sockaddr_storage ss_;
		return Socket::Recvfrom(socket_res_, buf_, ss_, sizeof(buf_));
	}
}
