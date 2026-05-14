#ifndef SOCKET_H
#define SOCKET_H

#include "Cs.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/hmac.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

namespace CS{
	//epol用
	#define NEVENTS 16
	#define EPOLL_TIMEOUT -1 //-1を設定すると無制限の待ち時間になる

	// ファイルディスクリプタ
	const std::string FD_IStoCS = "fd/CS_FD";			// IS  => CS
	const std::string FD_CStoIS = "fd/IS_FD";			// CS  => IS
	const std::string FD_CStoCS = "fd/CS_FD";			// CS  => CS
	const std::string FD_APLtoCS = "fd/CS_FD";			// APL => CS
	const std::string FD_CSMNG = "fd/CSMNG_FD";			// => CS (MNG)
	const std::string FD_DMMNG = "fd/DMMNG_FD";			// => DM (MNG)
	const std::string FD_SECtoCS = "fd/CS_FD";			// SEC => CS
	const std::string FD_CStoSEC = "fd/SEC_FD";			// CS  => SEC

	const std::string SHM_POLICING = "fd/policing.dat";

	/**
	 * @class	Socket
	 *
	 * @brief	DM2.0UDPネットワーク通信ソケットクラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/15
	 */
	class Socket
	{
	private:
		/** @brief	socketファイルディスクリプタ */
		int socket_res_;

		/** @brief	送信メッセージバッファ */
		struct send_message buf_;

		/** @brief	sockaddr_un構造体 */
		struct sockaddr_un addr_;

		/** @brief	各関数の戻り値 */
		// int res_;

		/** @brief	sockaddr_storage構造体 */
		struct sockaddr_storage ss_;

		/** @brief ss_構造体のサイズ */
		socklen_t sslen_ = sizeof(ss_);
	public:
		std::string conf_dir_path_;

		/**
		 * @fn	Socket::Socket()
		 *
		 * @brief	コンストラクタ
		 *
		 * @author	Nagoya University
		 * @date	2018/03/12
		 */
		Socket(){
		};

		/**
		 * @fn	virtual Socket::~Socket()
		 *
		 * @brief	ソケットファイルディスクリプタの開放を実施。
		 *
		 * @author	Nagoya University
		 * @date	2018/03/12
		 */

		virtual ~Socket(){
			close(socket_res_);
		}

		int CreateSocket(int sock_family_, int sock_type_);

		int Bind(int socket_, sockaddr_un addr_);

		int Bind(int socket_, sockaddr* addr_);

		int Bind(int socket_, sockaddr* addr_, socklen_t addr_len_);

		int Recv(send_message &buf_);
		int Recv(struct clientdata &buf_);

		int Recvfrom(int socket_res_, send_message &buf_, sockaddr_storage &ss_);
		int Recvfrom(int socket_res_, send_message &buf_, sockaddr_storage &ss_, int recv_size_);
		int RecvfromEtM(int socket_res_, send_message &buf_, sockaddr_storage &ss_, int recv_size_, const std::string &aesKey);
		int RecvfromEtMonPki(int socket_res_, send_message &buf_, sockaddr_storage &ss_, int recv_size_, const std::string &aesKey);

		int Sendto(send_message &buf_, sockaddr_un &addr_);
		int Sendto(struct clientdata &buf_, sockaddr_un &addr_);
		int Sendto(send_message &buf_, addrinfo &addr_);
		int Sendto(send_message &buf_, addrinfo &addr_, int send_size_, std::string udp_port_number_);
		int SendtoEtM(send_message &buf_, addrinfo &addr_, int send_size_, std::string udp_port_number_, const std::string &aesKey);
		int SendtoEtMonPki(send_message &buf_, addrinfo &addr_, int send_size_, std::string udp_port_number_, const std::string &aesKey);
		int Sendto(char *buf_, addrinfo &addr_, int send_size_, std::string udp_port_number_);

		int SendtoDivision(send_message &buf_, addrinfo &addr_, int send_size_, std::string udp_port_number_, int socket_type, const std::string &aesKey);
		int SendtoDivision(send_message &buf_, addrinfo &addr_, int send_size_, std::string udp_port_number_);

		int Getnameinfo(sockaddr_storage &ss_, char src_ip_[NI_MAXHOST]);

		std::string GetIPifaddrs_v6(std::string _ifname);
		std::string GetIPifaddrs_v6(std::string _ifname, std::string _anycast_address);
		
		std::string GetIPifaddrs(std::string _ifname, const int _IP_version);
		std::string GetIPifaddrs(std::string _ifname, std::string _anycast_address, const int _IP_version);
		std::string GetIPifaddrs(std::string _ifname, std::string _anycast_address, const int _IP_version, bool getMulticast);
		std::string GetMcIPifaddrs(std::string _ifname, const int _IP_version);

		void CloseSocket();

		//std::string addHMACToMessage(const std::string& message, const std::string& key);
		//std::string encryptMessage(const std::string& message, std::string iv_, const std::string& aesKey);
		//std::string decryptMessage(const unsigned char *encryptedMessage, std::string iv_, const std::string& aesKey, int res_size);
		//std::string verifyHMAC(const std::string& decryptedMessageWithHMAC, const std::string& hMacKey);
	};
}
#endif
