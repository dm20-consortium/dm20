#include "UdpSendInterface.h"

namespace CS{
	/**
	 * @fn	UdpSendInterface::UdpSendInterface()
	 *
	 * @brief	コンストラクタ
	 * 			バッファの初期化
	 *
	 * @author	Nagoya University
	 * @date	2018/03/15
	 */
	UdpSendInterface::UdpSendInterface(){
		memset(&buf_, 0, sizeof (buf_));
	}

	/**
	 * @fn	void UdpSendInterface::AplUdpSendto(const char* fd_name_, int src_stid_, int dst_stid_, short retry_level_, int retry_lifetime_, std::vector<char> &&payload_)
	 *
	 * @brief	APL送信インタフェース
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @param 		  	fd_name_ 		ファイルディスクリプタ名
	 * @param 		  	src_stid_		送信元SID
	 * @param 		  	dst_stid_		宛先SID
	 * @param 		  	retry_level_	再送処理レベル
	 * @param 		  	retry_lifetime_	再送処理継続時間
	 * @param			payload  		送信データペイロード部
	 */
	void UdpSendInterface::AplUdpSendto(const char* send_fd_name_, const char* recv_fd_name_, unsigned long long src_stid_, unsigned long long dst_stid_, short retry_level_, int retry_lifetime_, std::vector<char> &&payload_)
	{
		Init(send_fd_name_);

		buf_vector_.header = {};
		buf_vector_.dm2_payload.clear();
		buf_vector_.header.src_station_id = src_stid_;
		buf_vector_.header.dst_station_id = dst_stid_;
		buf_vector_.header.retry_level = retry_level_;
		buf_vector_.header.retry_lifetime = retry_lifetime_;
		buf_vector_.header.msg_type = DM2TYPE_APL;
		buf_vector_.header.transmission_flag = 0;
		buf_vector_.header.priority_level = 0; //優先度非対応インタフェースなので0固定
		
		strcpy(buf_vector_.header.fd_name, recv_fd_name_);
		buf_vector_.dm2_payload = std::move(payload_);
		if(SendPacket(buf_vector_) < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Sendto fail." << std::endl;
		}
		CloseSocketFd();
	}
	/**
	 * @fn	void UdpSendInterface::IsStreamSendtoCs(unsigned long long lane_id_, unsigned long long src_stid_, unsigned long long dst_stid_, short retry_level_, int retry_lifetime_, std::vector<char> &&payload_)
	 *
	 * @brief	IS送信インタフェース (長さ指定)
	 *
	 * @author	Shinichi Kusayama
	 * @date	2022/10/04
	 *
	 * @param 		  	lane_id_		レーン
	 * @param 		  	src_stid_   	送信元SID
	 * @param 		  	dst_stid_   	宛先SID
	 * @param 		  	retry_level_	再送処理レベル
	 * @param 		  	retry_lifetime_	再送処理継続時間
	 * @param			payload			送信データペイロード部
	 */
	void UdpSendInterface::IsStreamSendtoCs(unsigned long long lane_id_, unsigned long long src_stid_, unsigned long long dst_stid_, short retry_level_, int retry_lifetime_, std::vector<char> &&payload_)
	{
		buf_vector_.header = {};
		buf_vector_.dm2_payload.clear();
		buf_vector_.header.src_station_id = src_stid_;
		buf_vector_.header.dst_station_id = dst_stid_;
		buf_vector_.header.lane_id = lane_id_;
		buf_vector_.header.retry_level = retry_level_;
		buf_vector_.header.retry_lifetime = retry_lifetime_;
		buf_vector_.header.msg_type = DM2TYPE_IS;
		buf_vector_.header.transmission_flag = 0;
		buf_vector_.header.priority_level = 0; //優先度非対応インタフェースなので0固定
		buf_vector_.header.fd_name[0] = '\0';
		buf_vector_.dm2_payload = std::move(payload_);
		if(SendPacket(buf_vector_) < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Sendto fail." << std::endl;
		}
	}
	/**
	 * @fn	void UdpSendInterface::SecSendtoCs(sockaddr_un server_addr_, int lane_id_, int src_stid_, int dst_stid_, short retry_level_, int retry_lifetime_, std::string payload_, int length_)
	 *
	 * @brief	Security送信インタフェース (長さ指定)
	 *
	 * @author	Ryosuke Takeuchi
	 * @date	2024/10/04
	 *
	 * @param 		  	server_addr_	sockaddr_un構造体（CSのファイルディスクリプタに対応するソケットアドレス)
	 * @param 		  	lane_id_		レーン
	 * @param 		  	src_stid_   	送信元SID
	 * @param 		  	dst_stid_   	宛先SID
	 * @param 		  	retry_level_	再送処理レベル
	 * @param 		  	retry_lifetime_	再送処理継続時間
	 * @param			payload			送信データペイロード部
	 * @param			length_			送信データサイズ
	 */
	void UdpSendInterface::SecSendtoCs(sockaddr_un server_addr_, unsigned long long src_stid_, unsigned long long dst_stid_, short retry_level_, int retry_lifetime_, char *payload_, int length_, const std::string &confDirPath)
	{
		std::vector<char> payload(payload_, payload_ + length_);
		buf_vector_.header = {};
		buf_vector_.dm2_payload.clear();
		buf_vector_.header.src_station_id = src_stid_;
		buf_vector_.header.dst_station_id = dst_stid_;
		buf_vector_.header.retry_level = retry_level_;
		buf_vector_.header.retry_lifetime = retry_lifetime_;
		buf_vector_.header.msg_type = DM2TYPE_SEC;
		buf_vector_.header.transmission_flag = 0;
		buf_vector_.header.priority_level = 0; //優先度非対応インタフェースなので0固定
		buf_vector_.header.fd_name[0] = '\0';
		buf_vector_.dm2_payload = std::move(payload);
		if(SendPacket(buf_vector_) < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Sendto fail." << std::endl;
			int error_number = errno; 
			// 文字列として取得する (strerror)
			std::cout << "Error Code: " << error_number << std::endl;
			std::cout << "Message: " << strerror(error_number) << std::endl;
		}
	}

	/**
	 * @fn	void MngConnectionInfoSendtoCs(const char* send_fd_name_, const char* recv_fd_name_, unsigned long long src_stid_, unsigned long long dst_stid_, short retry_level_, int retry_lifetime_, std::string payload, CS_CTL_MNG cs_ctl_mng)
	 *
	 * @brief	MNG送信インタフェース (長さ指定)
	 *
	 * @date	2025/02/28
	 *
	 * @param 		  	send_fd_name_	送信FD名
	 * @param 		  	recv_fd_name_	受信FD名
	 * @param 		  	src_stid_   	送信元SID
	 * @param 		  	dst_stid_   	宛先SID
	 * @param 		  	retry_level_	再送処理レベル
	 * @param 		  	retry_lifetime_	再送処理継続時間
	 * @param			send_message_mng	送信データペイロード部
	 */
	void UdpSendInterface::MngConnectionInfoSendtoCs(const char* send_fd_name_, unsigned long long src_stid_, unsigned long long dst_stid_, send_message_mng &sm)
	{
	    Init(send_fd_name_);

		buf_vector_.header = {};
		buf_vector_.dm2_payload.clear();
		buf_vector_.header.src_station_id = src_stid_;
		buf_vector_.header.dst_station_id = dst_stid_;
		buf_vector_.header.msg_type = DM2Type_MNG_CONN_REGIST;
		buf_vector_.dm2_payload.assign(reinterpret_cast<char*>(&sm), reinterpret_cast<char*>(&sm) + sizeof(send_message_mng));
	    if (SendPacket(buf_vector_) < 0) {
	        std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Sendto fail." << std::endl;
		}
	    CloseSocketFd();
	}
}
