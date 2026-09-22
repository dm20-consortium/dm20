#include "Cs.h"
namespace CS{
	/**
	* @fn	addrinfo UdpMcNwClient::Init(std::string port_no, std::string dst_ip, std::string interface_name, const int _IP_version);
	*
	* @brief	初期化(エニキャスト引数なし)
	*
	* @author	Nagoya University
	* @date	2018/03/14
	*
	* @param	port_no		  	ポート番号
	* @param	dst_ip		  	宛先IPアドレス
	* @param	interface_name	インタフェース名
	* @param	anycast_address	エニーキャストアドレス
	* @param	_IP_version	    IPバージョン
	*
	* @return	addrinfo構造体
	*/
	addrinfo UdpMcNwClient::Init(std::string port_no, std::string dst_ip, std::string interface_name, std::string anycast_address, const int _IP_version)
	{
		OpenSSL_add_all_algorithms();
		port_no_ = port_no;
		return InitClient(port_no, dst_ip, interface_name, GetIPifaddrs(interface_name, anycast_address, _IP_version), _IP_version);
	}

	/**
	* @fn	addrinfo UdpMcNwClient::InitClient(std::string port_no, std::string dst_ip, std::string interface_name, std::string src_ip, const int _IP_version);
	*
	* @brief	UDPクライアント側接続シーケンス(共通))
	*
	* @author	Nagoya University
	* @date	2018/03/14
	*
	* @param	port_no		  	ポート番号
	* @param	dst_ip		  	宛先IPアドレス
	* @param	interface_name	インタフェース名
	* @param	src_ip		  	送信元IPアドレス
	* @param	_IP_version	    IPバージョン
	*
	* @return	addrinfo構造体
	*/
	addrinfo UdpMcNwClient::InitClient(std::string port_no, std::string dst_ip, std::string interface_name, std::string src_ip, const int _IP_version){
		int hops=3;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC; //IPv4/IPv6両方対応
		hints.ai_socktype = SOCK_DGRAM; //UDP送信

		sock_res = getaddrinfo(dst_ip.c_str(), port_no.c_str(), &hints, &res);
		if(sock_res != 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "getaddrinfo fail." << std::endl;
			// perror("getaddrinfo");
			exit(EXIT_FAILURE);
		}

		sockd = CreateSocket(res->ai_family, res->ai_socktype);
		if(sockd < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "CreateSocket fail." << std::endl;
			// perror("CreateSocket");
			exit(EXIT_FAILURE);
		}
		
		// bindに渡す送信用IPアドレスを取得
		sock_res = getaddrinfo(src_ip.c_str(), NULL, &hints, &res_src);
		if(sock_res != 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "getaddrinfo fail." << std::endl;
			// perror("getaddrinfo");
			exit(EXIT_FAILURE);
		}

		// 送信用IPアドレスをbind
		if(Bind(sockd, res_src->ai_addr, res_src->ai_addrlen) < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Bind fail." << std::endl;
			exit(EXIT_FAILURE);
		}

		gr_ = {0};
		gr_.gr_interface = if_nametoindex(interface_name.c_str());
		memcpy(&gr_.gr_group, res->ai_addr, res->ai_addrlen);

		if (_IP_version == 4) {
			if(setsockopt(sockd, IPPROTO_IP, MCAST_JOIN_GROUP, (char*) &gr_, sizeof(gr_))<0){
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "setsockopt set MCAST_JOIN_GROUP fail." << std::endl;
			}
			if(setsockopt(sockd, IPPROTO_IP, IP_MULTICAST_TTL, (char*) &lTTL, sizeof(lTTL))<0){
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "setsockopt set IP_MULTICAST_TTL fail." << std::endl;
			}
		} else {
			if(setsockopt(sockd, IPPROTO_IPV6, MCAST_JOIN_GROUP, (char*) &gr_, sizeof(gr_))<0){
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "setsockopt set MCAST_JOIN_GROUP fail." << std::endl;
			}
			if(setsockopt(sockd, IPPROTO_IPV6, IPV6_MULTICAST_IF, (char*) &gr_, sizeof(gr_))<0){
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "setsockopt set IPV6_MULTICAST_IF fail." << std::endl;
			}
			if(setsockopt(sockd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, (char*) &hops, sizeof(hops))<0){
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "setsockopt set IPV6_MULTICAST_HOPS fail." << std::endl;
				exit(EXIT_FAILURE);
			}
		}

		return *res;
	}
	/**
	* @fn	int UdpMcNwClient::Sendto(send_message &buf_, addrinfo &addr_)
	*
	* @brief	sendto送信(バッファ渡し、addrinfo用)
	*
	* @author	Shinichi Kusayama
	* @date	2022/11/22
	*
	* @param [in,out]	buf_ 	送信メッセージバッファ
	* @param [in,out]	addr_	宛先IPアドレス
	*
	* @return	int sendtoの戻り値
	*/
	int UdpMcNwClient::Sendto(send_message &buf_, addrinfo &addr_, int send_size_){
		return sendto(sockd, &buf_, send_size_, 0, addr_.ai_addr, addr_.ai_addrlen);
	}
	/**
	* @fn	int UdpMcNwClient::Sendto(send_message &buf_, addrinfo &addr_)
	*
	* @brief	sendto送信(バッファ渡し、addrinfo用)
	*
	* @author	Shinichi Kusayama
	* @date	2022/11/22
	*
	* @param [in,out]	buf_ 	送信メッセージバッファ
	* @param [in,out]	addr_	宛先IPアドレス
	*
	* @return	int sendtoの戻り値
	*/
	int UdpMcNwClient::Sendto(send_message &buf_, addrinfo &addr_){
		int ret = Sendto(buf_, addr_, sizeof(buf_));
		return ret;
	}
}
