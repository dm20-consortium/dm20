#ifndef DTLSNWCLIENT_H
#define DTLSNWCLIENT_H

#include "Cs.h"
#include "SocketDTLS.h"

namespace CS{

	/**
	 * @class	DtlsNwClient
	 *
	 * @brief	DM2.0UDPネットワーク通信の送信側(Client)処理クラス
	 *
	 * @author	Nagoya University
	 * @date	2023/01/23
	 */
	class DtlsNwClient:public SocketDTLS{
	private:
		/** @brief	socketディスクリプタ */
		int sockd;
		/** @brief	hints */
		struct addrinfo hints;
		/** @brief	*res */
		struct addrinfo *res;
		/** @brief	*res_src */
		struct addrinfo *res_src;
		/** @brief	Socket関連関数の戻り値 */
		int sock_res;
	
	public:
		/**
		 * @fn	DtlsNwClient()
		 *
		 * @brief	コンストラクタ
		 * 			パラメータの初期化
		 *
		 * @author	Nagoya University
		 * @date	2023/01/23
		 */
		DtlsNwClient():SocketDTLS(){
			sockd = 0;
			sock_res = 0;
			hints = {0};
			res = {0};
			res_src = {0};
		}

		/**
		 * @fn	virtual ~DtlsNwClient()
		 *
		 * @brief	デストラクタ
		 * 			ソケットのクローズ処理
		 * 			addrinfoストレージの解放
		 *
		 * @author	Nagoya University
		 * @date	2023/01/23
		 */
		virtual ~DtlsNwClient(){
			close(sockd);
			freeaddrinfo(res);
			freeaddrinfo(res_src);
		}

		addrinfo Init(std::string port_no, std::string dst_ip);

		addrinfo Init(std::string port_no, std::string dst_ip, std::string src_ip, const std::string &confDirPath, bool &exit_flg_);

		addrinfo InitServer(std::string port_no, std::string dst_ip);

		addrinfo InitServer(std::string port_no, std::string dst_ip, std::string src_ip, const std::string &confDirPath, bool &exit_flg_);

		void CloseSocketFd();
	};
}
#endif
