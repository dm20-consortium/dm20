#include "Cs.h"
#include "I2vSignatureProcessor.h"
#include "I2vCryptoProcessor.h"
namespace CS{
	/**
	* @fn	int Socket::CreateSocket(int sock_family_, int sock_type_)
	*
	* @brief	socketの作成
	*
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @param	sock_family_	ソケットファミリ
	* @param	sock_type_  	ソケットタイプ
	*
	* @return	int				ソケットID
	*/
	int Socket::CreateSocket(int sock_family_, int sock_type_){
		socket_res_ = socket(sock_family_, sock_type_, 0);
		return socket_res_;		
	}

	/**
	 * @fn	void Socket::Bind(int socket_, sockaddr_un addr_)
	 *
	 * @brief	socketのbind(sockaddr_un用)
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	socket_	The socket.
	 * @param	addr_  	The address.
	 * @return	int		bindの実行結果
	 */
	int Socket::Bind(int socket_, sockaddr_un addr_){
		int res = 0;
		res = bind(socket_, (struct sockaddr *)&addr_, sizeof(addr_));
		return res;
	}


	/**
	* @fn	void Socket::Bind(int socket_, sockaddr* addr_)
	*
	* @brief	socketのbind(sockaddr用)
	*
	* @author	Nagoya University
	* @date	2018/03/12
	*
	 * @param socket_ ソケットID
	 * @param addr_  ソケットにBINDするIPアドレス
	 * @return int  bindの実行結果
	 */
	int Socket::Bind(int socket_, sockaddr* addr_){
		int res = 0;
		res = bind(socket_, addr_, sizeof(*addr_));
		return res;
	}


	/**
	* @fn	void Socket::Bind(int socket_, sockaddr* addr_, socklen_t addr_len_)
	*
	* @brief	socketのbind(sockaddr用、且つ、アドレス長)
	*
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @param 		  	socket_  	ソケットID
	* @param [in,out]	addr_	 	ソケットにBINDするIPアドレス
	* @param 		  	addr_len_	BINDするIPアドレスの長さ
	* @return int  bindの実行結果
	*/
	int Socket::Bind(int socket_, sockaddr* addr_, socklen_t addr_len_){
		int res = 0;
		res = bind(socket_, addr_, addr_len_);
		return res;
	}


	/**
	* @fn	int Socket::Recv(send_message &buf_)
	*
	* @brief	recv受信処理
	*
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @param 	[in,out] buf_	受信メッセージバッファ
	* @return   int				recvの実行結果 
	*
	* @return	標準関数recvのリターン値
	*/
	int Socket::Recv(send_message &buf_){
		return recv(socket_res_, &buf_, sizeof(buf_), 0);
	}

	/**
	* @fn	int Socket::Recv((struct clientdata &buf_)
	*
	* @brief	recv受信処理
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/28
	*
	* @param 	[in,out] buf_	受信メッセージバッファ
	* @return   int				recvの実行結果 
	*
	* @return	標準関数recvのリターン値
	*/
	int Socket::Recv(struct clientdata &buf_){
		return recv(socket_res_, &buf_, sizeof(buf_), 0);
	}

	/**
	* @fn	void Socket::Recvfrom(int socket_res_, send_message &buf_, sockaddr_storage &ss_)
	*
	* @brief	recvfrom受信処理
	*
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @param 		  	socket_res_	ソケットID
	* @param [in,out]	buf_	   	受信メッセージバッファ
	* @param [in,out]	ss_		   	送信元IPアドレス
	* @return   int				recvfromの実行結果 
	*/
	int Socket::Recvfrom(int socket_res_, send_message &buf_, sockaddr_storage &ss_){
		return Recvfrom(socket_res_, buf_, ss_, sizeof(buf_));
	}
	/**
	* @fn	void Socket::Recvfrom(int socket_res_, send_message &buf_, sockaddr_storage &ss_, int recv_size_)
	*
	* @brief	recvfrom受信処理
	*
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @param 		  	socket_res_	ソケットID
	* @param [in,out]	buf_	   	受信メッセージバッファ
	* @param [in,out]	ss_		   	送信元IPアドレス
	* @param [in]		recv_size_  受信サイズ
	* @return   int				recvfromの実行結果 
	*/
    int Socket::Recvfrom(int socket_res_, send_message &buf_, sockaddr_storage &ss_, int recv_size_){
		int res = 0;
		socklen_t sslen = sizeof(ss_);
		res = recvfrom(socket_res_, &buf_, recv_size_, 0, (struct sockaddr *)&ss_, &sslen);
		return res;
    }
	/**
	* @fn	RecvfromEtM(int socket_res_, send_message &buf_, sockaddr_storage &ss_, int recv_size_, const std::string &aesKey)
	*
	* @brief	recvfrom受信処理
	*
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @param 		  	socket_res_	ソケットID
	* @param [in,out]	buf_	   	受信メッセージバッファ
	* @param [in,out]	ss_		   	送信元IPアドレス
	* @param [in,out]	recv_size_	受信サイズ
	* @param [in]		aesKey		AESキー
	* @return   int				recvfromの実行結果 
	*/
    int Socket::RecvfromEtM(int socket_res_, send_message &buf_, sockaddr_storage &ss_, int recv_size_, const std::string &aesKey) {
		int res_size = 0;
		socklen_t sslen = sizeof(ss_);
			
		memset(&buf_, 0, sizeof(buf_));
		unsigned char encryptedMessage[recv_size_];
		res_size = recvfrom(socket_res_, encryptedMessage, sizeof(encryptedMessage), 0, (struct sockaddr *)&ss_, &sslen);
		std::string encryptedStr(reinterpret_cast<char*>(encryptedMessage), res_size);
		std::string decryptedMessage = "";
		try {
			decryptedMessage = SEC::I2vCryptoProcessor:: decryptEtMMessage(encryptedStr, aesKey);
		} catch (const std::exception &e) {
			std::cerr << e.what() << std::endl;
			return -1;
		}

		memcpy(&buf_, decryptedMessage.c_str(), decryptedMessage.length());
		return res_size;
    }
	/**
	 * @fn int Socket::RecvfromEtMonPki(int socket_res_, send_message &buf_, sockaddr_storage &ss_, int recv_size_, const std::string &aesKey)
	 */
    int Socket::RecvfromEtMonPki(int socket_res_, send_message &buf_, sockaddr_storage &ss_, int recv_size_, const std::string &aesKey){
		int res_size = 0;
		socklen_t sslen = sizeof(ss_);
			
		memset(&buf_, 0, sizeof(buf_));
		unsigned char encryptedMessage[recv_size_];
		res_size = recvfrom(socket_res_, encryptedMessage, sizeof(encryptedMessage), 0, (struct sockaddr *)&ss_, &sslen);
		std::string encryptedStr(reinterpret_cast<char*>(encryptedMessage), res_size);
		std::string decryptedMessage = SEC::I2vCryptoProcessor::decryptEtMMessage(encryptedStr, aesKey);
		authenticated_encryption_message ae_message;
		memset(&ae_message, 0, sizeof(ae_message));
		memcpy(&ae_message, decryptedMessage.data(), decryptedMessage.length());
		if (SEC::I2vSignatureProcessor::verifySignature(ae_message.signer_station_id, ae_message.sign_size, (unsigned char*)ae_message.sign, ae_message.message_size, (unsigned char*)ae_message.message, conf_dir_path_)){
			memcpy(&buf_, ae_message.message,  ae_message.message_size);
			return ae_message.message_size;
		}
		return -1;
    }

	/**
	* @fn	void Socket::Recvfrom(int socket_res_, clientdata &buf_, sockaddr_storage &ss_, int recv_size_)
	*
	* @brief	recvfrom受信処理 (CSからISへ電文を受信する場合は、clientdata型)
	*
	* @author	Shinichi Kusayama
	* @date	2026/04/08
	*
	* @param 		  	socket_res_	ソケットID
	* @param [in,out]	buf_	   	受信メッセージバッファ
	* @param [in,out]	ss_		   	送信元IPアドレス
	* @param [in]		recv_size_  受信サイズ
	*/
    int Socket::Recvfrom(int socket_res_, clientdata &buf_, sockaddr_storage &ss_, int recv_size_){
		int res = 0;
		socklen_t sslen = sizeof(ss_);
		res = recvfrom(socket_res_, &buf_, recv_size_, 0, (struct sockaddr *)&ss_, &sslen);
		return res;
    }

	/**
	* @fn	int Socket::Sendto(send_message &buf_, sockaddr_un &addr_)
	*
	* @brief	sendto送信処理(バッファ渡し、sockaddr_un用、UdpProcClientから呼び出される)
	*
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @param [in,out]	buf_ 	送信メッセージバッファ
	* @param [in,out]	addr_	宛先IPアドレス
	*
	* @return	int 標準関数sendtoのリターン値
	*/
	int Socket::Sendto(send_message &buf_, sockaddr_un &addr_){
		//std::cout << "socket_res_:"  << socket_res_ << std::endl;
		return sendto(socket_res_, &buf_, sizeof(buf_), 0, (struct sockaddr *)&addr_, sizeof(addr_));
	}

	/**
	* @fn	int Socket::Sendto(struct clientdata &buf_, sockaddr_un &addr_)
	*
	* @brief	sendto送信処理(バッファ渡し、sockaddr_un用、UdpProcClientから呼び出される)
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/28
	*
	* @param [in,out]	buf_ 	送信メッセージバッファ
	* @param [in,out]	addr_	宛先IPアドレス
	*
	* @return	int 標準関数sendtoのリターン値
	*/
	int Socket::Sendto(struct clientdata &buf_, sockaddr_un &addr_){
		return sendto(socket_res_, &buf_, sizeof(buf_), 0, (struct sockaddr *)&addr_, sizeof(addr_));
	}
	/**
	* @fn	int Socket::SendClientData(struct clientdata &buf_, addrinfo &addr_)
	*
	* @brief	sendto送信処理(clientdata型)
	*
	* @author	Shinichi Kusayama
	* @date	2026/6/4
	*
	* @param [in,out]	buf_ 	送信メッセージバッファ
	* @param [in,out]	addr_	宛先IPアドレス
	*
	* @return	int 標準関数sendtoのリターン値
	*/
	int Socket::SendClientData(struct clientdata &buf_, addrinfo &addr_){
		return sendto(socket_res_, &buf_, sizeof(buf_), 0, addr_.ai_addr, addr_.ai_addrlen);
	}
	/**
	* @fn	int Socket::Sendto(char *buf_, addrinfo &addr_, int send_size_, std::string udp_port_number_)
	*
	* @brief	sendto送信(バッファ渡し、addrinfo用、UdpNwClient/UdpMcNwClientから呼び出される)
	*
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @param [in,out]	buf_ 	送信メッセージバッファ
	* @param [in,out]	addr_	宛先IPアドレス
	* @param [in]	send_size_	送信サイズ
	* @param [in]	udp_port_number_	UDPポート番号
	*
	* @return	int sendtoの戻り値
	*/
	int Socket::Sendto(send_message &buf_, addrinfo &addr_, int send_size_, std::string udp_port_number_){
		return Sendto(reinterpret_cast<char*>(&buf_), addr_, send_size_, udp_port_number_);
	}
	/**
	* @fn	int Socket::SendtoEtM(char *buf_, addrinfo &addr_, int send_size_, std::string udp_port_number_, const std::string &aesKey)
	*
	* @brief	暗号化処理付きsendto送信(バッファ渡し、addrinfo用、UdpNwClient/UdpMcNwClientから呼び出される)
	*
	* @author	Shinichi Kusayama
	* @date	2024/01/29
	*
	* @param [in,out]	buf_ 	送信メッセージバッファ
	* @param [in,out]	addr_	宛先IPアドレス
	* @param [in]	send_size_	送信サイズ
	* @param [in]	udp_port_number_	UDPポート番号
	* @param [in]	aesKey	AESキー
	*
	* @return	int sendtoの戻り値
	*/
	int Socket::SendtoEtM(send_message &buf_, addrinfo &addr_, int send_size_, std::string udp_port_number_, const std::string &aesKey){
		char *buf_char = reinterpret_cast<char*>(&buf_);
		std::string buf_str(buf_char, send_size_);

		std::string encryptedMessage = SEC::I2vCryptoProcessor::encryptThenMACMessage(buf_str, aesKey);
		return Sendto((char *)encryptedMessage.c_str(), addr_, encryptedMessage.length(), udp_port_number_);
	}
	/**
	* @fn	int Socket::SendtoEtMonPki(char *buf_, addrinfo &addr_, int send_size_, std::string udp_port_number_)
	*
	* @brief	暗号化処理付きsendto送信(バッファ渡し、addrinfo用、UdpNwClient/UdpMcNwClientから呼び出される)
	*
	* @author	Shinichi Kusayama
	* @date	2024/01/29
	*
	* @param [in,out]	buf_ 	送信メッセージバッファ
	* @param [in,out]	addr_	宛先IPアドレス
	* @param [in]	send_size_	送信サイズ
	* @param [in]	udp_port_number_	UDPポート番号
	*
	* @return	int sendtoの戻り値
	*/
	int Socket::SendtoEtMonPki(send_message &buf_, addrinfo &addr_, int send_size_, std::string udp_port_number_, const std::string &aesKey){
		char *buf_char = reinterpret_cast<char*>(&buf_);
		std::string buf_str(buf_char, send_size_);

		authenticated_encryption_message ae_message;
		size_t slen = SEC::I2vSignatureProcessor::makeSignature(buf_str, (unsigned char*)ae_message.sign, conf_dir_path_, buf_.src_station_id);
		ae_message.signer_station_id = buf_.src_station_id;
		ae_message.sign_size = slen; //short_size 2
		ae_message.message_size = buf_str.length(); //short_size 2
		memcpy(ae_message.message, buf_str.c_str(), buf_str.length());

		// メッセージの長さを取得
		int messageLength = 12 + ae_message.sign_size + ae_message.message_size;
		char *message_char = reinterpret_cast<char*>(&ae_message);
		std::string message(message_char, messageLength);
		
		std::string encryptedMessage = SEC::I2vCryptoProcessor::encryptThenMACMessage(message, aesKey);

		return Sendto((char *)encryptedMessage.c_str(), addr_, encryptedMessage.length(), udp_port_number_);
	
	}
	/**
	* @fn	int Socket::Sendto(char *buf_, addrinfo &addr_, int send_size_, std::string udp_port_number_)
	*
	* @brief	sendto送信(バッファ渡し、addrinfo用、UdpNwClient/UdpMcNwClientから呼び出される)
	*
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @param [in,out]	buf_ 	送信メッセージバッファ
	* @param [in,out]	addr_	宛先IPアドレス
	* @param [in]	send_size_	送信サイズ
	* @param [in]	udp_port_number_	UDPポート番号
	*
	* @return	int sendtoの戻り値
	*/
	int Socket::Sendto(char *buf_, addrinfo &addr_, int send_size_, std::string udp_port_number_){
		int ret;
		ret = sendto(socket_res_, buf_, send_size_, 0, addr_.ai_addr, addr_.ai_addrlen);
		return ret;
	}
	/**
	* @fn	int Socket::Sendto(send_message &buf_, addrinfo &addr_)
	*
	* @brief	sendto送信(バッファ渡し、addrinfo用、UdpNwClient/UdpMcNwClientから呼び出される)
	*
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @param [in,out]	buf_ 	送信メッセージバッファ
	* @param [in,out]	addr_	宛先IPアドレス
	*
	* @return	int sendtoの戻り値
	*/
	int Socket::Sendto(send_message &buf_, addrinfo &addr_){
		return Sendto(buf_, addr_, sizeof(buf_), 0);
	}

	/**
	* @fn	int Socket::SendtoDivision(send_message &buf_, addrinfo &addr_, int send_size_, std::string udp_port_number_)
	*
	* @brief	sendto送信処理(汎用型CS間通信で使用)
	*
	* @author	Nagoya University
	* @date		2019/03/26
	*
	* @param buf_		送信データ構造体 
	* @param addr_		プロセス間ソケットアドレス構造体
	* @param [in]	send_size_	送信サイズ
	* @param [in]	udp_port_number_	UDPポート番号
	* @param [in]	iv_	初期化ベクトル
	*
	* @return	int sendtoの戻り値
	*/
	int Socket::SendtoDivision(send_message &buf_, addrinfo &addr_, int send_size_, std::string udp_port_number_){
		int header_size = sizeof (buf_) - MSGSIZE;						// ヘッダのサイズ
		int division_size = send_size_ - (sizeof(buf_) - MSGSIZE);		// 分割サイズ
		if (buf_.payload_size <= division_size) {
			return Sendto(buf_, addr_, header_size + buf_.payload_size, udp_port_number_);
		}
		int flagment_num = 0;
		struct timespec ts;
		char payload_[MSGSIZE];
		memcpy(payload_, buf_.dm2_payload, MSGSIZE);
		
		memset(buf_.dm2_payload, '\0', division_size);
		
		//送信データのflagment_duplication_check_idに現在時刻を設定
		timespec_get(&ts, TIME_UTC);
		buf_.flagment_duplication_check_id = ts.tv_sec * 1000000000 + ts.tv_nsec;
		
		//フラグメント数を計算
		flagment_num = buf_.payload_size / division_size;
		buf_.flagment_sum = flagment_num;
		
		//フラグメント時の端数の有無を求める
		if(buf_.payload_size % division_size != 0){
			flagment_num++;
		}
		buf_.flagment_sum = flagment_num;
		int len = 0;
		//フラグメントごとにデータを送信
		for(int i= 0; i < flagment_num - 1; i++){
			buf_.flagment_offset = i;
			memset(buf_.dm2_payload, '\0', MSGSIZE);
			memcpy(buf_.dm2_payload, payload_+ division_size * i, division_size);
			//std::cout << "[flagment data]No." << i << ":" << buf_.dm2_payload << std::endl;
			len = Sendto(buf_, addr_, send_size_, udp_port_number_);
		}
		if (len >= 0) {
			// 最後の送信サイズ = ヘッダサイズ + ペイロードサイズ - 分割して送信したペイロードサイズ
			int last_send_size_ = header_size + buf_.payload_size - division_size * (flagment_num - 1);
			buf_.flagment_offset = flagment_num - 1;
			memset(buf_.dm2_payload, '\0', MSGSIZE);
			memcpy(buf_.dm2_payload, payload_+ division_size * (flagment_num - 1), division_size);
			//std::cout << "[flagment data]No." << flagment_num - 1 << ":" << buf_.dm2_payload << std::endl;
			len = Sendto(buf_, addr_, last_send_size_, udp_port_number_);
		}
		return len;
	}
	/**
	* @fn	int Socket::SendtoDivision(send_message &buf_, addrinfo &addr_, int send_size_, std::string udp_port_number_, int socket_type)
	*
	* @brief	sendto送信処理(汎用型CS間通信で使用)
	*
	* @author	Nagoya University
	* @date		2019/03/26
	*
	* @param buf_		送信データ構造体 
	* @param addr_		プロセス間ソケットアドレス構造体
	* @param [in]	send_size_	送信サイズ
	* @param [in]	udp_port_number_	UDPポート番号
	* @param [in]	socket_type	ソケットタイプ
	* @param [in]	aesKey	AESキー
	*
	* @return	int sendtoの戻り値
	*/
	int Socket::SendtoDivision(send_message &buf_, addrinfo &addr_, int send_size_, std::string udp_port_number_, int socket_type, const std::string &aesKey){
		int header_size = sizeof (buf_) - MSGSIZE;						// ヘッダのサイズ
		int division_size = send_size_ - (sizeof(buf_) - MSGSIZE);		// 分割サイズ
		if (buf_.payload_size <= division_size) {
			if (socket_type == 10) {
				return SendtoEtM(buf_, addr_, header_size + buf_.payload_size, udp_port_number_, aesKey);
			} else {
				return SendtoEtMonPki(buf_, addr_, header_size + buf_.payload_size, udp_port_number_, aesKey);
			}
		}
		int flagment_num = 0;
		struct timespec ts;
		char payload_[MSGSIZE];
		memcpy(payload_, buf_.dm2_payload, MSGSIZE);
		
		memset(buf_.dm2_payload, '\0', division_size);
		
		//送信データのflagment_duplication_check_idに現在時刻を設定
		timespec_get(&ts, TIME_UTC);
		buf_.flagment_duplication_check_id = ts.tv_sec * 1000000000 + ts.tv_nsec;
		
		//フラグメント数を計算
		flagment_num = buf_.payload_size / division_size;
		buf_.flagment_sum = flagment_num;
		
		//フラグメント時の端数の有無を求める
		if(buf_.payload_size % division_size != 0){
			flagment_num++;
		}
		buf_.flagment_sum = flagment_num;
		int len = 0;
		//フラグメントごとにデータを送信
		for(int i= 0; i < flagment_num - 1; i++){
			buf_.flagment_offset = i;
			memset(buf_.dm2_payload, '\0', MSGSIZE);
			memcpy(buf_.dm2_payload, payload_+ division_size * i, division_size);
			//std::cout << "[flagment data]No." << i << ":" << buf_.dm2_payload << std::endl;
			if (socket_type == 10) {
				len = SendtoEtM(buf_, addr_, send_size_, udp_port_number_, aesKey);
			} else {
				len = SendtoEtMonPki(buf_, addr_, send_size_, udp_port_number_, aesKey);
			}
		}
		if (len >= 0) {
			// 最後の送信サイズ = ヘッダサイズ + ペイロードサイズ - 分割して送信したペイロードサイズ
			int last_send_size_ = header_size + buf_.payload_size - division_size * (flagment_num - 1);
			buf_.flagment_offset = flagment_num - 1;
			memset(buf_.dm2_payload, '\0', MSGSIZE);
			memcpy(buf_.dm2_payload, payload_+ division_size * (flagment_num - 1), division_size);
			//std::cout << "[flagment data]No." << flagment_num - 1 << ":" << buf_.dm2_payload << std::endl;
			if (socket_type == 10) {
				len = SendtoEtM(buf_, addr_, last_send_size_, udp_port_number_, aesKey);
			} else {
				len = SendtoEtMonPki(buf_, addr_, last_send_size_, udp_port_number_, aesKey);
			}
		}
		return len;
	}

	/**
	* @fn	void Socket::Getnameinfo(sockaddr_storage &ss_, char src_ip_[NI_MAXHOST])
	*
	* @brief	ソケットアドレスからIPアドレスを取得
	*
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @param [in,out]	ss_	   	ソケットアドレス
	* @param 		  	src_ip_	IPアドレス
	* @return	int getnameinfoの戻り値
	*/
	int Socket::Getnameinfo(sockaddr_storage &ss_, char src_ip_[NI_MAXHOST]){
		return getnameinfo((struct sockaddr *)&ss_, sslen_, src_ip_, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
	}

	/**
	* @fn	std::string Socket::GetIPifaddrs(std::string _ifname, std::string _anycast_address, int IP_version, bool getMulticast)
	*
	* @brief	指定インタフェース名のIPアドレスを抽出。
	* 			IPv4の場合、127.x.x.xのループバックアドレスを除く、
	* 			IPv6の場合、：：1のループバックアドレス、：：の未指定アドレス、fe80：：/16のリンクローカル、及びdm2.confで指定したエニーキャストアドレスを除く、
	* 			１つのIPアドレスを抽出。
	*
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @param	_ifname				インタフェース名
	* @param	_anycast_address	エニーキャストアドレス
	* @param	_IP_version			IP Version
	* @param	getMulticast		マルチキャストアドレスのみ抽出
	*
	* @return	std::string IPアドレス。インタフェースに条件にあったIPアドレスが存在しない場合には空文字を返す。
	*/
	std::string Socket::GetIPifaddrs(std::string _ifname, std::string _anycast_address, const int _IP_version, bool getMulticast) {
		struct ifaddrs *ifa_list;
		ifa_list = nullptr;
		struct ifaddrs *ifa;
		ifa = nullptr;
		std::string rtn_str = "";
		std::cout << "[GetIPifaddrs] 指定されたインターフェース名" << _ifname << "のIPアドレスを抽出します." << std::endl;
		if (_anycast_address != "") {
			std::cout << "[GetIPifaddrs] ただし、エニーキャストアドレス" << _anycast_address << "は除外されます." << std::endl;
			if (_IP_version == 6 && _anycast_address != "") {
				transform(_anycast_address.begin(), _anycast_address.end(), _anycast_address.begin(), ::tolower);
			}
		}
		// 全てのNICの情報を取得
		int ifcount = getifaddrs(&ifa_list);
		if(ifcount != 0){
			freeifaddrs(ifa_list);
			ifa_list = nullptr;
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "GetIPifaddrs1 fail." << std::endl;
			perror("GetIPifaddrs1");
			ifcount = getifaddrs(&ifa_list);
			if(ifcount !=0){
				freeifaddrs(ifa_list);
				ifa_list = nullptr;
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "GetIPifaddrs1_2 fail." << std::endl;
				perror("GetIPifaddrs2");
			}
		}
		std::string addrstr_str;
		char addrstr[256] = {0};
		std::vector<std::string> ip_list;
		// 引数で指定されたNICのリストを作成する
		for(ifa = ifa_list; ifa != NULL; ifa = ifa->ifa_next){
			// VPNソフトウェアによってはifa_addrがNULLとなるケースがあるため、検索対象から除外（スキップ）
			if (ifa->ifa_addr == NULL) continue;
			if (_IP_version == 4) {
				if(ifa->ifa_addr->sa_family == AF_INET){
					inet_ntop(AF_INET, &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr, addrstr, sizeof(addrstr));
					addrstr_str = addrstr;
					if(strcmp(ifa->ifa_name, _ifname.c_str()) == 0)	ip_list.push_back(addrstr_str);
				}
			} else {
				if(ifa->ifa_addr->sa_family == AF_INET6){
					inet_ntop(AF_INET6, &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr,	addrstr, sizeof(addrstr));
					addrstr_str = addrstr;
					if(strcmp(ifa->ifa_name, _ifname.c_str()) == 0)	ip_list.push_back(addrstr_str);
				}
			}
		}
		for(auto itr = ip_list.begin(); itr != ip_list.end(); ++itr) {
			std::cout << "[GetIPifaddrs] 候補：" << *itr << std::endl;
			// デフォルトで除外するもの。::の未指定アドレス、fe80::/16のリンクローカルアドレス
			if (itr->substr(0,2) == "::") continue;
			if (itr->substr(0,4) == "fe80") continue;
			// エニーキャストアドレスと一致する場合は除外
			if (_anycast_address != "") {
				if (_anycast_address == *itr) continue;
			}
			// マルチキャストアドレスならアドレスの範囲に含むものを抽出、それ以外は見つかったものを抽出
			if (getMulticast) {
				if (_IP_version == 4) {
					if (itr->length() < 3) continue;
					const char *p_addr = itr->c_str();
					//IPv4の場合、先頭が224～239
					if (p_addr[0] == '2' && (p_addr[1] >= '2' || p_addr[1] <= '3') && (p_addr[2] >= '4' || p_addr[2] <= '9')) {
						rtn_str = *itr;
						break;
					}
				} else {
					//IPv6の場合、ffから始まる
					if (itr->substr(0,2) == "ff") {
						rtn_str = *itr;
						break;
					}
				}
			} else {
				rtn_str = *itr;
				break;
			}
		}
		freeifaddrs(ifa_list);
		if (rtn_str == "") {
			std::cout << "[GetIPifaddrs] 抽出失敗 - NIC:" << _ifname << ", anycast:" << _anycast_address << ", IPVer:" << _IP_version << ", getMulticast:" << getMulticast << std::endl;
		} else {
			std::cout << "[GetIPifaddrs] 抽出成功：" << rtn_str << std::endl;
		}
		return rtn_str;
	}
	/**
	* @fn	std::string Socket::GetIPifaddrs(std::string _ifname, std::string , int IP_version)
	* 
	* @brief	指定インタフェース名およびIPバージョンのIPアドレスを抽出。ただし、anycast_addressを除く。
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @param	_ifname				インタフェース名
	* @param	_anycast_address	エニーキャストアドレス
	* @param	_IP_version	IP Version
	*
	* @return	std::string IPアドレス。インタフェースに条件にあったIPアドレスが存在しない場合には空文字を返す
	*/
	std::string Socket::GetIPifaddrs(std::string _ifname, std::string _anycast_address, const int _IP_version) {
		return GetIPifaddrs(_ifname, _anycast_address, _IP_version, false);
	}
	/**
	* @fn	std::string Socket::GetIPifaddrs_v6(std::string _ifname, std::string _anycast_address)
	*
	* @brief	指定インタフェース名のIPアドレスを抽出。ただし、anycast_addressを除く。
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @param	_ifname				インタフェース名
	* @param	_anycast_address	エニーキャストアドレス
	*
	* @return	std::string IPアドレス。インタフェースに条件にあったIPアドレスが存在しない場合には空文字を返す
	*/
	std::string Socket::GetIPifaddrs_v6(std::string _ifname, std::string _anycast_address){
		return GetIPifaddrs(_ifname, _anycast_address, 6, false);
	}
	/**
	* @fn	std::string Socket::GetIPifaddrs(std::string _ifname, const int _IP_version)
	*
	* @brief	指定インタフェース名およびIPバージョンのIPアドレスを抽出。
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @param	_ifname				インタフェース名
	* @param	_IP_version	IP Version
	*
	* @return	std::string IPアドレス。インタフェースに条件にあったIPアドレスが存在しない場合には空文字を返す
	*/
	std::string Socket::GetIPifaddrs(std::string _ifname, const int _IP_version){
		return GetIPifaddrs(_ifname, "", _IP_version, false);
	}
	/**
	* @fn	std::string Socket::GetIPifaddrs_v6(std::string _ifname)
	*
	* @brief	指定インタフェース名のIPアドレスを抽出。
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @param	_ifname				インタフェース名
	*
	* @return	std::string IPアドレス。インタフェースに条件にあったIPアドレスが存在しない場合には空文字を返す
	*/
	std::string Socket::GetIPifaddrs_v6(std::string _ifname){
		return GetIPifaddrs(_ifname, "", 6, false);
	}
	/**
	* @fn	std::string Socket::GetMcIPifaddrs(std::string _ifname, const int _IP_version)
	*
	* @brief	指定インタフェース名およびIPバージョンのマルチキャストIPアドレスを抽出。
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @param	_ifname				インタフェース名
	* @param	_IP_version	IP Version
	*
	* @return	std::string IPアドレス。インタフェースに条件にあったIPアドレスが存在しない場合には空文字を返す
	*/
	std::string Socket::GetMcIPifaddrs(std::string _ifname, const int _IP_version){
		//ITS無線機による通信で使用しているため、第3引数（マルチキャストIPアドレスのチェック）はfalseにする
		//return GetIPifaddrs(_ifname, "", _IP_version, true);
		return GetIPifaddrs(_ifname, "", _IP_version, false);
	}
	/**
	* @fn	std::string Socket::CloseSocket()
	*
	* @brief	close処理
	* @author	Nagoya University
	* @date	2018/03/12
	*
	*/
	void Socket::CloseSocket(){
		close(socket_res_);
	}


}
