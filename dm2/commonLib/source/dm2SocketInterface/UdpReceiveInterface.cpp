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
		upper_buf_ = {0}; 
		buf_ = {0};
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
	void UdpReceiveInterface::run(const char* fd_name, std::function<void(send_message_upper)>notify){
		UdpServer udpserver;
		int res_init = udpserver.Init(fd_name);
		if(res_init < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "udpprocserver.Init fail:" << res_init << std::endl;
		}
		while(1)
		{
			if(udpserver.RecvClientData(buf_, res_init) > 0){
				upper_buf_.src_station_id = buf_.msg.src_station_id;
				upper_buf_.dst_station_id = buf_.msg.dst_station_id;
				upper_buf_.lane_id = buf_.msg.lane_id;
				upper_buf_.dm2_payload = std::string(buf_.msg.dm2_payload, MSGSIZE);
				notify(upper_buf_);
			}
		}
	}


	/**
	 * @fn	void UdpReceiveInterface::run_is(const char* fd_name, std::function<void(send_message_upper)>notify)
	 *
	 * @brief	IS用UDP受信処理
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @param	fd_name   	ファイルディスクリプタ名
	 * @param	notify		notify
	 */
	void UdpReceiveInterface::run_is(const std::string &fd_name, const std::string &nic, const std::string &port, std::function<void(send_message_upper)>notify){
		UdpServer udpserver;
		int res_init = udpserver.Init(fd_name.c_str(), nic, port);
		if(res_init < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "udpprocserver.Init fail:" << res_init << std::endl;
		}
		while(1)
		{
			if(udpserver.RecvClientData(buf_, res_init) > 0){
				strcpy(upper_buf_.from_ip, buf_.from_ip);
				upper_buf_.src_station_id = buf_.msg.src_station_id;
				upper_buf_.dst_station_id = buf_.msg.dst_station_id;
				upper_buf_.lane_id = buf_.msg.lane_id;
				upper_buf_.dm2_payload = std::string(buf_.msg.dm2_payload, MSGSIZE);
				notify(upper_buf_);
			}
		}
	}


	/**
	 * @fn	void UdpReceiveInterface::run_sec(const char* fd_name, std::function<void(send_message_upper)>notify)
	 *
	 * @brief	security用UDP受信処理
	 *
	 * @author	Ryosuke Takeuchi
	 * @date	2024/10/04
	 *
	 * @param	fd_name   	ファイルディスクリプタ名
	 * @param	notify		notify
	 */
	void UdpReceiveInterface::run_sec(const std::string &fd_name, std::function<void(send_message_upper)>notify){
		UdpProcServer udpprocserver;
		int res_init = udpprocserver.Init(fd_name.c_str());
		if(res_init < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "udpprocserver.Init fail:" << res_init << std::endl;
		}
		while(1)
		{
			if(udpprocserver.Recv(buf_) > 0){
				strcpy(upper_buf_.from_ip, buf_.from_ip);
				upper_buf_.src_station_id = buf_.msg.src_station_id;
				upper_buf_.dst_station_id = buf_.msg.dst_station_id;
				upper_buf_.lane_id = buf_.msg.lane_id;
				upper_buf_.dm2_payload = std::string(buf_.msg.dm2_payload, MSGSIZE);
				notify(upper_buf_);
				return;
			}
		}
	}

	/**
	 * @fn	void run_mng_ctl(const std::string &fd_name, std::function<void(send_message_upper)>notify)
	 *
	 * @brief	mng用UDP受信処理
	 *
	 * @date	2025/02/28
	 *
	 * @param	fd_name   	ファイルディスクリプタ名
	 * @param	notify		notify
	 */
	void UdpReceiveInterface::run_mng_ctl(const std::string &fd_name, std::function<void(send_message_upper)>notify){
		UdpProcServer udpprocserver;
		int res_init = udpprocserver.Init(fd_name.c_str());
		if(res_init < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "udpprocserver.Init fail:" << res_init << std::endl;
		}
		clientdata buf_ = {0};
		send_message_upper upper_buf_ = {0};
		while(1)
		{
			if(udpprocserver.Recv(buf_) > 0){
				upper_buf_.src_station_id = buf_.msg.src_station_id;
				upper_buf_.dst_station_id = buf_.msg.dst_station_id;
				upper_buf_.lane_id = buf_.msg.lane_id;
				upper_buf_.dm2_payload = std::string(buf_.msg.dm2_payload, MSGSIZE);
				
				// ipアドレスを通知する場合は以下のように記載していく想定
				// upper_buf_.executer_sid = buf_.msg.executer_sid;
				// strncpy(upper_buf_.ip_address, buf_.msg.ip_address, NI_MAXHOST);
				notify(upper_buf_);
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
	void UdpReceiveInterface::notify(send_message_upper upper_buf_){
		// ここにIS,APLでデータ受信時の処理を記載
	}
}