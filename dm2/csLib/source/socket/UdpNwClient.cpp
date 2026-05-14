#include "Cs.h"
namespace CS{
	/**
	* @fn	addrinfo UdpNwClient::Init(std::string port_no, std::string dst_ip);
	*
	* @brief	UPD通信接続シーケンス(IPv4)
	*
	* @author	Nagoya University
	* @date	2018/03/14
	*
	* @param	port_no	ポート番号
	* @param	dst_ip 	宛先IPアドレス
	*
	* @return	addrinfo構造体
	*/
	addrinfo UdpNwClient::Init(std::string port_no, std::string dst_ip)
	{
		OpenSSL_add_all_algorithms();
		port_no_ = port_no;
		return InitClient(port_no, dst_ip);
	}

	/**
	* @fn	addrinfo UdpNwClient::Init(std::string port_no, std::string dst_ip, std::string conf_dir_path);
	*
	* @brief	UPD通信接続シーケンス(IPv6)
	*
	* @author	Nagoya University
	* @date	2018/03/14
	*
	* @param	port_no	ポート番号
	* @param	dst_ip 	宛先IPアドレス
	* @param	conf_dir_path 	設定ディレクトリ名
	*
	* @return	addrinfo構造体
	*/
	addrinfo UdpNwClient::Init(std::string port_no, std::string dst_ip, std::string conf_dir_path)
	{
		port_no_ = port_no;
		conf_dir_path_ = conf_dir_path;
		return InitClient(port_no, dst_ip);
	}

	//インタフェース名を引数とする	
	addrinfo UdpNwClient::Init_v6(std::string port_no, std::string dst_ip, std::string if_name)
	{
		port_no_ = port_no;
		return InitClient_v6(port_no, dst_ip, if_name);
	}


	/**
	* @fn	addrinfo UdpNwClient::InitClient(std::string port_no, std::string dst_ip);
	*
	* @brief	UDP接続シーケンス 送信インタフェースが一つの場合
	*
	* @author	Nagoya University
	* @date	2018/03/14
	*
	* @param	port_no	ポート番号
	* @param	dst_ip 	宛先IPアドレス
	*
	* @return	addrinfo構造体
	*/
	addrinfo UdpNwClient::InitClient(std::string port_no, std::string dst_ip){
		hints = {0};
		// ToDo: IPv6で動作確認
		//hints.ai_family = AF_UNSPEC; //IPv4/IPv6両方対応
		hints.ai_socktype = SOCK_DGRAM; //UDP送信
		sock_res = getaddrinfo(dst_ip.c_str(), port_no.c_str(), &hints, &res);
			if(sock_res != 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "getaddrinfo fail." << std::endl;
			perror("getaddrinfo");
			exit(EXIT_FAILURE);
		}
		sockd = CreateSocket(res->ai_family, res->ai_socktype);
		if(sockd < 0){
			close(sockd);
			freeaddrinfo(res);
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "CreateSocket fail." << std::endl;
			exit(EXIT_FAILURE);
		}
		return *res;
	}

	/**
	 * @fn addrinfo UdpNwClient::InitClient_v6(std::string port_no, std::string dst_ip, std::string if_name)
	 * @brief UDP接続シーケンス 送信インタフェースを指定して送信する場合
	 * 
 	 * @author	Nagoya University
     * @date	2018/03/14
	 * @param port_no 
	 * @param dst_ip 
	 * @param if_name 
	 * @return addrinfo 
	 */
	addrinfo UdpNwClient::InitClient_v6(std::string port_no, std::string dst_ip, std::string if_name){
		std::string src_ip;
		Socket dm2socket;
		src_ip = dm2socket.GetIPifaddrs_v6(if_name);
		if(src_ip.find(":") != std::string::npos){
			//std::cout  << "LINE:" << __LINE__ << " in UdpNwClient.cpp " << "送信元IPアドレスを一意に設定完了。" << std::endl;
		}else{
			std::cout  << "LINE:" << __LINE__ << " in UdpNwClient.cpp " << "送信元IPアドレスが不定。dm2.confのインタフェース名の指定を確認してください。" << std::endl;
		}
		int sockopt_res=0;		
		hints = {0};
		hints.ai_family = AF_UNSPEC; //IPv4/IPv6両方対応
		hints.ai_socktype = SOCK_DGRAM; //UDP送信

		struct in6_pktinfo ipi;
		int iface_id = 0;
		iface_id = if_nametoindex(if_name.c_str());
		memset(&ipi, 0, sizeof(ipi));
		ipi.ipi6_ifindex = iface_id;

		sock_res = getaddrinfo(dst_ip.c_str(), port_no.c_str(), &hints, &res);
		if(sock_res != 0){
			freeaddrinfo(res);
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "getaddrinfo fail." << std::endl;
			perror("getaddrinfo");
			exit(EXIT_FAILURE);
		}

		sockd = CreateSocket(res->ai_family, res->ai_socktype);
		if(sockd < 0){
			close(sockd);
			freeaddrinfo(res);
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "CreateSocket fail." << std::endl;
			perror("CreateSocket");
			exit(EXIT_FAILURE);
		}
		
		// in6_pktinfo構造体へのデータセット
		memcpy(&ipi.ipi6_addr, src_ip.c_str(), sizeof(struct in6_addr));
		
		sockopt_res = setsockopt(sockd, IPPROTO_IPV6, IPV6_PKTINFO, &ipi, sizeof(ipi));
		if(sockopt_res != 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "setsockopt fail." << std::endl;
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}

		return *res;
	}

	void UdpNwClient::CloseSocketFd(){
		CloseSocket();
	}
	/**
	* @fn	int UdpNwClient::Sendto(send_message &buf_, addrinfo &addr_)
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
	int UdpNwClient::Sendto(send_message &buf_, addrinfo &addr_, int send_size_){
		return sendto(sockd, &buf_, send_size_, 0, addr_.ai_addr, addr_.ai_addrlen);
	}
	/**
	* @fn	int UdpNwClient::Sendto(send_message &buf_, addrinfo &addr_)
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
	int UdpNwClient::Sendto(send_message &buf_, addrinfo &addr_){
		int ret = Sendto(buf_, addr_, sizeof(buf_));
		return ret;
	}
}
