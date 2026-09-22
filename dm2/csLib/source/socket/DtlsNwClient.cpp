#include "Cs.h"
#include "DtlsNwClient.h"
namespace CS{
	/**
	* @fn	addrinfo DtlsNwClient::Init(std::string port_no, std::string dst_ip);
	*
	* @brief	DTLS通信接続シーケンス(IPv4)
	*
	* @author	Nagoya University
	* @date	2023/01/23
	*
	* @param	port_no	ポート番号
	* @param	dst_ip 	宛先IPアドレス
	*
	* @return	addrinfo構造体
	*/
	addrinfo DtlsNwClient::Init(std::string port_no, std::string dst_ip)
	{
		return InitServer(port_no, dst_ip);
	}

	/**
	* @fn	addrinfo DtlsNwClient::Init(std::string port_no, std::string dst_ip, std::string src_ip);
	*
	* @brief	DTLS通信接続シーケンス(IPv6)
	*
	* @author	Nagoya University
	* @date	2023/01/23
	*
	* @param	port_no	ポート番号
	* @param	dst_ip 	宛先IPアドレス
	* @param	src_ip 	送信元IPアドレス
	*
	* @return	addrinfo構造体
	*/
	addrinfo DtlsNwClient::Init(std::string port_no, std::string dst_ip, std::string src_ip, const std::string &confDirPath, bool &exit_flg_)
	{
		return InitServer(port_no, dst_ip, src_ip, confDirPath, exit_flg_);
	}

	/**
	* @fn	addrinfo DtlsNwClient::InitServer(std::string port_no, std::string dst_ip);
	*
	* @brief	DTLS接続シーケンス 送信インタフェースが一つの場合
	*
	* @author	Nagoya University
	* @date	2023/01/23
	*
	* @param	port_no	ポート番号
	* @param	dst_ip 	宛先IPアドレス
	*
	* @return	addrinfo構造体
	*/
	addrinfo DtlsNwClient::InitServer(std::string port_no, std::string dst_ip){
		memset(&hints, 0, sizeof(hints));
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
	* @fn	addrinfo DtlsNwClient::InitServer(std::string port_no, std::string dst_ip, std::string src_ip);
	*
	* @brief	DTLS接続シーケンス 送信インタフェースが複数で選択が必要な場合
	*
	* @author	Nagoya University
	* @date	2023/02/02
	*
	* @param	port_no	ポート番号
	* @param	dst_ip 	宛先IPアドレス
	* @param	src_ip 	送信元IPアドレス
	*
	* @return	addrinfo構造体
	*/
	addrinfo DtlsNwClient::InitServer(std::string port_no, std::string dst_ip, std::string src_ip, const std::string &confDirPath, bool &exit_flg_){
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC; //IPv4/IPv6両方対応
		hints.ai_socktype = SOCK_DGRAM; //UDP送信

		sock_res = getaddrinfo(dst_ip.c_str(), port_no.c_str(), &hints, &res);
		if(sock_res != 0){
			freeaddrinfo(res);
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "getaddrinfo fail." << std::endl;
			perror("getaddrinfo");
			exit(EXIT_FAILURE);
		}
		//DTLSクライアント開始
		while (true) {
			int retval = StartDtlsClient((char*)dst_ip.c_str(), (char*)src_ip.c_str(), stoi(port_no), confDirPath, exit_flg_);
			if (retval <= 0){
				std::cout << "DTLS client connect failed" << std::endl;
				perror("StartDtlsClient");
				if (exit_flg_) break;
				continue;
			}
			else{
				std::cout << "DTLS client connect success" << std::endl;
				break;
			}
		}		
		return *res;
	}

	void DtlsNwClient::CloseSocketFd(){
		CloseDtls();
	}
}
