#include "UdpReceiveInterface.h"

namespace CS{
	/**
	 * @fn	UdpReceiveInterface::UdpReceiveInterface()
	 *
	 * @brief	コンストラクタ
	 * 			バッファの初期化
	 *
	 * @author	Nagoya University
	 * @date	2018/03/15
	 */
	UdpReceiveInterface::UdpReceiveInterface(){
		vectorBuf = {0}; 
		buf = {0};
	}
	/**
	 * @fn	void UdpReceiveInterface::run(const char* fd_name, std::function<void(send_message)>notify)
	 *
	 * @brief	APL用UDP受信処理
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @param	fd_name   	ファイルディスクリプタ名
	 * @param	notify		notify
	 */
	void UdpReceiveInterface::run(const char* fd_name, std::function<void(send_message_vector)>notify){
		run_common(fd_name, notify);
	}


	/**
	 * @fn	void UdpReceiveInterface::run_is(const char* fd_name, std::function<void(send_message_vector)>notify)
	 *
	 * @brief	IS用UDP受信処理
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @param	fd_name   	ファイルディスクリプタ名
	 * @param	notify		notify
	 */
	void UdpReceiveInterface::run_is(const std::string &fd_name, const std::string &nic, const std::string &port, std::function<void(send_message_vector)>notify){
		UdpServer udpserver;
		int res_init = udpserver.Init(fd_name.c_str(), nic, port);
		if(res_init < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "udpprocserver.Init fail:" << res_init << std::endl;
		}
		int header_size = sizeof(send_message_header);
		int combination_map_clear_time_ = 100;
		UnorderedMap<std::string, time_t> flagment_data_receive_time_map;
		UnorderedMap<std::string, std::vector<std::string>> flagment_data_combination_map;
		std::thread th0(SocketUtil::ClearUnorderedMap, 
				std::ref(combination_map_clear_time_),
				std::ref(flagment_data_combination_map),
				std::ref(flagment_data_receive_time_map));
		while(1)
		{
			if (udpserver.RecvPacket(buf, res_init) <= 0) continue;
			if (SocketUtil::combineFragment(buf, vectorBuf, flagment_data_receive_time_map, flagment_data_combination_map)) {
				notify(vectorBuf);
			}
		}
	}


	/**
	 * @fn	void UdpReceiveInterface::run_sec(const char* fd_name, std::function<void(send_message_vector)>notify)
	 *
	 * @brief	security用UDP受信処理
	 *
	 * @author	Ryosuke Takeuchi
	 * @date	2024/10/04
	 *
	 * @param	fd_name   	ファイルディスクリプタ名
	 * @param	notify		notify
	 */
	void UdpReceiveInterface::run_sec(const std::string &fd_name, std::function<void(send_message_vector)>notify){
		run_common(fd_name, notify);
	}

	/**
	 * @fn	void run_mng_ctl(const std::string &fd_name, std::function<void(send_message_vector)>notify)
	 *
	 * @brief	mng用UDP受信処理
	 *
	 * @date	2025/02/28
	 *
	 * @param	fd_name   	ファイルディスクリプタ名
	 * @param	notify		notify
	 */
	void UdpReceiveInterface::run_mng_ctl(const std::string &fd_name, std::function<void(send_message_vector)>notify){
		run_common(fd_name, notify);
	}
	/**
	 * @fn	void run_common(const std::string &fd_name, std::function<void(send_message_vector)>notify)
	 *
	 * @brief	mng用UDP受信処理
	 *
	 * @date	2025/02/28
	 *
	 * @param	fd_name   	ファイルディスクリプタ名
	 * @param	notify		notify
	 */
	void UdpReceiveInterface::run_common(const std::string &fd_name, std::function<void(send_message_vector)>notify){
		UdpProcServer udpserver;
		int res_init = udpserver.Init(fd_name.c_str(), "", "");
		if(res_init < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "udpprocserver.Init fail:" << res_init << std::endl;
		}
		int header_size = sizeof(send_message_header);
		int combination_map_clear_time_ = 100;
		UnorderedMap<std::string, time_t> flagment_data_receive_time_map;
		UnorderedMap<std::string, std::vector<std::string>> flagment_data_combination_map;
		std::thread th0(SocketUtil::ClearUnorderedMap, 
				std::ref(combination_map_clear_time_),
				std::ref(flagment_data_combination_map),
				std::ref(flagment_data_receive_time_map));
		while(1)
		{
			if (udpserver.RecvPacket(buf, res_init) <= 0) continue;
			if (SocketUtil::combineFragment(buf, vectorBuf, flagment_data_receive_time_map, flagment_data_combination_map)) {
				notify(vectorBuf);
			}
		}
	}

	/**
	 * @fn	void UdpReceiveInterface::notify(send_message buf_)
	 *
	 * @brief	UDP受信通知
	 *
	 * @author	Nagoya University
	 * @date	2018/03/15
	 *
	 * @param	buf_	送信メッセージバッファ
	 */
	void UdpReceiveInterface::notify(send_message_vector vectorBuf_){
		// ここにIS,APLでデータ受信時の処理を記載
	}
}