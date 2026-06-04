#include "Cs.h"
namespace CS{
	/**
	* @fn	int UdpMcNwServer::Init(std::string port_no, std::string rcv_interface_name, int ip_ver);
	*
	* @brief	初期化
	*
	* @author	Shinichi Kusayama
	* @date	2023/08/23
	*
	* @param	port_no			  	ポート番号
	* @param	rcv_interface_name	インタフェース名
	* @param	ip_ver  			IP Version
	*
	* @return	int 				ソケットID
	*/
	int UdpMcNwServer::Init(std::string port_no, std::string rcv_interface_name, int ip_ver)
	{
		OpenSSL_add_all_algorithms();
		port_no_ = port_no;
		dst_multicast_ip_ = GetMcIPifaddrs(rcv_interface_name, ip_ver);
		if (dst_multicast_ip_ == "") {
			exit(EXIT_FAILURE);
		}
		return InitServer(port_no, rcv_interface_name, dst_multicast_ip_, ip_ver);
	}
	/**
	* @fn	int UdpMcNwServer::Init(std::string port_no, std::string rcv_interface_name, std::string dst_multicast_ip, const int _IP_version);
	*
	* @brief	初期化
	*
	* @author	Nagoya University
	* @date	2018/03/14
	*
	* @param	port_no			  	ポート番号
	* @param	rcv_interface_name	インタフェース名
	* @param	dst_multicast_ip  	宛先マルチキャストアドレス
	* @param	_IP_version 		IP Version
	*
	* @return	int 				ソケットID
	*/
	int UdpMcNwServer::Init(std::string port_no, std::string rcv_interface_name, std::string dst_multicast_ip, const int _IP_version)
	{
		dst_multicast_ip_ = dst_multicast_ip;
		port_no_ = port_no;
		return InitServer(port_no, rcv_interface_name, dst_multicast_ip, _IP_version);
	}

	/**
	 * @fn	int UdpMcNwServer::Init(std::string port_no, std::string rcv_interface_name_1, std::string rcv_interface_name_2, std::string dst_multicast_ip, const int _IP_version)
	 * @brief  初期化(２つのインタフェース対応版)
	 * 
	 * @param port_no 				ポート番号
	 * @param rcv_interface_name_1 	インタフェース1名
	 * @param rcv_interface_name_2 	インタフェース2名
	 * @param dst_multicast_ip 		宛先マルチキャストアドレス
	 * @return int 					ソケットID
	 * @param _IP_version  			IP Version
	 */
	int UdpMcNwServer::Init(std::string port_no, std::string rcv_interface_name_1, std::string rcv_interface_name_2, std::string dst_multicast_ip, const int _IP_version)
	{
		dst_multicast_ip_ = dst_multicast_ip;
		port_no_ = port_no;
		return InitServer(port_no, rcv_interface_name_1, rcv_interface_name_2, dst_multicast_ip, _IP_version);
	}

	/**
	* @fn	int UdpMcNwServer::InitServer(std::string port_no, std::string rcv_interface_name, std::string dst_multicast_ip, const int _IP_version);
	*
	* @brief	UDPサーバ側接続シーケンス
	*
	* @author	Nagoya University
	* @date	2018/03/14
	*
	* @param	port_no			  	ポート番号
	* @param	rcv_interface_name	インタフェース名
	* @param	dst_multicast_ip  	宛先マルチキャストアドレス
	* @param	_IP_version	    IPバージョン
	*
	* @return	int ソケットID
	*/
	int UdpMcNwServer::InitServer(std::string port_no, std::string rcv_interface_name, std::string dst_multicast_ip, const int _IP_version){
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC; //IPv4/IPv6両方対応
		hints.ai_socktype = SOCK_DGRAM; //UDP送信
		hints.ai_flags = AI_PASSIVE;
		if(getaddrinfo(dst_multicast_ip.c_str(), port_no.c_str(), &hints, &res)<0){
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
			sockd = BIND_ERROR;
		}

		// IPv4/IPv6共通仕様
		gr_ = {0};
		gr_.gr_interface = if_nametoindex(rcv_interface_name.c_str());
		memcpy(&gr_.gr_group, res->ai_addr, res->ai_addrlen);
		if (_IP_version == 4) {
			if(setsockopt(sockd, IPPROTO_IP, MCAST_JOIN_GROUP, (char*) &gr_, sizeof(gr_))<0){
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "setsockopt set Membership fail." << std::endl;
				perror("ERROR setsockopt set Membership");
			}
		} else {
			if(setsockopt(sockd, IPPROTO_IPV6, MCAST_JOIN_GROUP, (char*) &gr_, sizeof(gr_))<0){
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "setsockopt set Membership fail." << std::endl;
				perror("ERROR setsockopt set Membership");
			}
		}

		freeaddrinfo(res);

		return sockd;
	}


	/**
	 * @fn int UdpMcNwServer::InitServer(std::string port_no, std::string rcv_interface_name_1, std::string rcv_interface_name_2, std::string dst_multicast_ip, const int _IP_version)
	 * @brief UDPサーバ側接続シーケンス(２つのインタフェース対応版)
	 * 
	 * @param port_no 				ポート番号
	 * @param rcv_interface_name_1 	インタフェース1名
	 * @param rcv_interface_name_2 	インタフェース2名
	 * @param dst_multicast_ip 		宛先マルチキャストアドレス
	 * @param	_IP_version	    IPバージョン
	 * @return int ソケットID
	 */
	int UdpMcNwServer::InitServer(std::string port_no, std::string rcv_interface_name_1, std::string rcv_interface_name_2, std::string dst_multicast_ip, const int _IP_version){
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC; //IPv4/IPv6両方対応
		hints.ai_socktype = SOCK_DGRAM; //UDP送信
		hints.ai_flags = AI_PASSIVE;
		if(getaddrinfo(dst_multicast_ip.c_str(), port_no.c_str(), &hints, &res)<0){
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
			sockd = BIND_ERROR;
		}

		// IPv4/IPv6共通仕様
		//1つ目のインタフェース設定
		gr_ = {0};
		gr_.gr_interface = if_nametoindex(rcv_interface_name_1.c_str());
		memcpy(&gr_.gr_group, res->ai_addr, res->ai_addrlen);
		if (_IP_version == 4) {
			if(setsockopt(sockd, IPPROTO_IP, MCAST_JOIN_GROUP, (char*) &gr_, sizeof(gr_))<0){
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "setsockopt set Membership fail." << std::endl;
				perror("setsockopt set Membership");
			}
		} else {
			if(setsockopt(sockd, IPPROTO_IPV6, MCAST_JOIN_GROUP, (char*) &gr_, sizeof(gr_))<0){
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "setsockopt set Membership fail." << std::endl;
				perror("setsockopt set Membership");
			}
		}

		//２つ目のインタフェース設定
		gr_ = {0};
		gr_.gr_interface = if_nametoindex(rcv_interface_name_2.c_str());
		memcpy(&gr_.gr_group, res->ai_addr, res->ai_addrlen);
		if (_IP_version == 4) {
			if(setsockopt(sockd, IPPROTO_IP, MCAST_JOIN_GROUP, (char*) &gr_, sizeof(gr_))<0){
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "setsockopt set Membership fail." << std::endl;
				perror("setsockopt set Membership");
			}
		} else {
			if(setsockopt(sockd, IPPROTO_IPV6, MCAST_JOIN_GROUP, (char*) &gr_, sizeof(gr_))<0){
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "setsockopt set Membership fail." << std::endl;
				perror("setsockopt set Membership");
			}
		}

		freeaddrinfo(res);

		return sockd;
	}
}
