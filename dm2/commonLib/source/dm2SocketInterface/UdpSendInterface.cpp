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
	 * @fn	void UdpSendInterface::AplUdpSendto(const char* fd_name_, int src_stid_, int dst_stid_, short retry_level_, int retry_lifetime_, std::string payload)
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
	//void UdpSendInterface::AplUdpSendto(const char* fd_name_, int src_stid_, int dst_stid_, short retry_level_, int retry_lifetime_, std::string payload)
	void UdpSendInterface::AplUdpSendto(const char* send_fd_name_, const char* recv_fd_name_, unsigned long long src_stid_, unsigned long long dst_stid_, short retry_level_, int retry_lifetime_, std::string payload)
	{
		Init(send_fd_name_);

		buf_.src_station_id = src_stid_;
		buf_.dst_station_id = dst_stid_;
		buf_.retry_level = retry_level_;
		buf_.retry_lifetime = retry_lifetime_;
		buf_.transmission_flag = 0;
		buf_.lane_id = 0;
		buf_.msg_type = DM2TYPE_APL;
		buf_.priority_level = 0; //優先度非対応インタフェースなので0固定
		strcpy(buf_.fd_name, recv_fd_name_);
		//分割不要のサイズであればそのまま送信
		if(payload.length() <= MSGSIZE){
			payload.copy(buf_.dm2_payload, MSGSIZE);
			buf_.flagment_sum = 1;
			if(SendPacket(buf_) < 0){
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Sendto fail." << std::endl;
			}
		}else{
			std::cout << "送信データ廃棄" << std::endl;
		}
		CloseSocketFd();
	}
	/**
	 * @fn	void UdpSendInterface::AplUdpSendtoCsInShortSize(const char* fd_name_, int src_stid_, int dst_stid_, std::string payload)
	 *
	 * @brief	APL送信インタフェース
	 *
	 * @author	Shinichi Kusayama
	 * @date	2023/02/03
	 *
	 * @param 		  	fd_name_ 		ファイルディスクリプタ名
	 * @param 		  	src_stid_		送信元SID
	 * @param 		  	dst_stid_		宛先SID
	 * @param			payload  		送信データペイロード部
	 */
	//void UdpSendInterface::AplUdpSendtoCsInShortSize(const char* fd_name_, int src_stid_, int dst_stid_, std::string payload)
	void UdpSendInterface::AplUdpSendtoCsInShortSize(const char* send_fd_name_, const char* recv_fd_name_, unsigned long long  src_stid_, unsigned long long  dst_stid_, std::string payload)
	{
		Init(send_fd_name_);
		buf_.src_station_id = src_stid_;
		buf_.dst_station_id = dst_stid_;
		buf_.retry_level = 0;
		buf_.retry_lifetime = 0;
		buf_.transmission_flag = 0;
		buf_.lane_id = 0;
		buf_.msg_type = -1 * (DM2TYPE_APL + 10);
		buf_.priority_level = 0; //優先度非対応インタフェースなので0固定
		strcpy(buf_.fd_name, recv_fd_name_);
		payload.copy(buf_.dm2_payload, MSGSIZE);
		buf_.flagment_sum = 1;

		if(SendPacket(buf_) < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Sendto fail." << std::endl;
		}
		CloseSocketFd();
	}
	

	/**
	 * @brief 優先度付きAPL送信インタフェース 
	 * 
	 * @param 		  	fd_name_ 		ファイルディスクリプタ名
	 * @param 		  	src_stid_		送信元SID
	 * @param 		  	dst_stid_		宛先SID
	 * @param 		  	retry_level_	再送処理レベル
	 * @param 		  	retry_lifetime_	再送処理継続時間
	 * @param 			priority_level_	優先度
	 * @param			payload  		送信データペイロード部
	 * 
	 * @return void
	 */
	//void UdpSendInterface::AplUdpSendtoPriority(const char* fd_name_, int src_stid_, int dst_stid_, short retry_level_, int retry_lifetime_, int priority_level_, std::string payload)
	void UdpSendInterface::AplUdpSendtoPriority(const char* send_fd_name_, const char* recv_fd_name_, unsigned long long src_stid_, unsigned long long dst_stid_, short retry_level_, int retry_lifetime_, int priority_level_, std::string payload)
	{
		Init(FD_APLtoCS);

		buf_.src_station_id = src_stid_;
		buf_.dst_station_id = dst_stid_;
		buf_.retry_level = retry_level_;
		buf_.retry_lifetime = retry_lifetime_;
		buf_.transmission_flag = 0;
		buf_.lane_id = 0;
		buf_.msg_type = DM2TYPE_APL;
		buf_.priority_level = priority_level_;
		strcpy(buf_.fd_name, recv_fd_name_);

		//分割不要のサイズであればそのまま送信
		if(payload.length() <= MSGSIZE){
			payload.copy(buf_.dm2_payload, MSGSIZE);
			buf_.flagment_sum = 1;
			if(SendPacket(buf_) < 0){
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Sendto fail." << std::endl;
			}
		}else{
			std::cout << "送信データ廃棄" << std::endl;
		}
		CloseSocketFd();
	}
	
	/**
	 * @fn	void UdpSendInterface::IsStreamSendtoCs(int lane_id_, int src_stid_, int dst_stid_, short retry_level_, int retry_lifetime_, std::string payload_, int length_)
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
	 * @param			length_			送信データサイズ
	 */
	void UdpSendInterface::IsStreamSendtoCs(unsigned long long lane_id_, unsigned long long src_stid_, unsigned long long dst_stid_, short retry_level_, int retry_lifetime_, char *payload_, int length_, const std::string &confDirPath, bool doLog)
	{
		buf_.src_station_id = src_stid_;
		buf_.dst_station_id = dst_stid_;
		buf_.retry_level = retry_level_;
		buf_.retry_lifetime = retry_lifetime_;
		buf_.transmission_flag = 0;
		buf_.lane_id = lane_id_;
		buf_.msg_type = DM2TYPE_IS;
		buf_.priority_level = 0; //優先度非対応インタフェースなので0固定
		buf_.fd_name[0] = '\0';
		buf_.payload_size = length_;
		
		#if LID_PRIORITY == 1
		Util dm2util;
		// VehicleProcRcvで作成済みの共有メモリをプロセスにアタッチする
		char* const policing_flag = reinterpret_cast<char*>(shmat(dm2util.GetSharedId(confDirPath + SHM_POLICING, 50, 0), 0, 0));
		// char* const policing_flag = reinterpret_cast<char*>(shmat(seg_id, 0, 0));
		//std::cout << "policing_flag = " << policing_flag << std::endl;
		std::string policing_flag_str = std::string(policing_flag);
		if(policing_flag_str == "0"){
			//分割不要のサイズであればそのまま送信
			if(length_ <= MSGSIZE){
				memset(buf_.dm2_payload, 0, MSGSIZE);
				memcpy(buf_.dm2_payload, payload_, length_);
				buf_.flagment_sum = 1;
				if(SendPacket(buf_) < 0){
					if (doLog) std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Sendto fail." << std::endl;
				}
			}else{
				if (doLog) std::cout << "送信データ廃棄" << std::endl;
			}
		}else{
			if (doLog) std::cout << "送信データ廃棄" << std::endl;
		}

		// 共有メモリをプロセスから切り離す
		shmdt(policing_flag);

		#else //LID_PRIORITY
		//分割不要のサイズであればそのまま送信
		if(length_ <= MSGSIZE){
			memset(buf_.dm2_payload, 0, MSGSIZE);
			memcpy(buf_.dm2_payload, payload_, length_);
			buf_.flagment_sum = 1;
			if(SendPacket(buf_) < 0){
				if (doLog) std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Sendto fail." << std::endl;
			}
		}else{
			if (doLog) std::cout << "送信データ廃棄" << std::endl;
		}
		#endif //LID_PRIORITY
	}
	/**
	 * @fn	void UdpSendInterface::IsStreamSendtoCs(int lane_id_, int src_stid_, int dst_stid_, short retry_level_, int retry_lifetime_, std::string payload_, int length_)
	 *
	 * @brief	IS送信インタフェース (長さ指定、ログ出力なし)
	 *
	 * @author	Shinichi Kusayama
	 * @date	2025/6/11
	 *
	 * @param 		  	lane_id_		レーン
	 * @param 		  	src_stid_   	送信元SID
	 * @param 		  	dst_stid_   	宛先SID
	 * @param 		  	retry_level_	再送処理レベル
	 * @param 		  	retry_lifetime_	再送処理継続時間
	 * @param			payload			送信データペイロード部
	 * @param			length_			送信データサイズ
	 */
	void UdpSendInterface::IsStreamSendtoCs(unsigned long long lane_id_, unsigned long long src_stid_, unsigned long long dst_stid_, short retry_level_, int retry_lifetime_, char *payload_, int length_, const std::string &confDirPath)
	{
		IsStreamSendtoCs(lane_id_, src_stid_, dst_stid_, retry_level_, retry_lifetime_, payload_, length_, confDirPath, false);
	}
	/**
	 * @fn	void UdpSendInterface::IsStreamSendtoCs(int lane_id_, int src_stid_, int dst_stid_, short retry_level_, int retry_lifetime_, std::string payload_)
	 *
	 * @brief	IS送信インタフェース
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @param 		  	lane_id_		レーン
	 * @param 		  	src_stid_   	送信元SID
	 * @param 		  	dst_stid_   	宛先SID
	 * @param 		  	retry_level_	再送処理レベル
	 * @param 		  	retry_lifetime_	再送処理継続時間
	 * @param			payload			送信データペイロード部
	 */
	void UdpSendInterface::IsStreamSendtoCs(unsigned long long lane_id_, unsigned long long src_stid_, unsigned long long dst_stid_, short retry_level_, int retry_lifetime_, const std::string &payload_, const std::string &confDirPath)
	{
		IsStreamSendtoCs(lane_id_, src_stid_, dst_stid_, retry_level_, retry_lifetime_, (char *)payload_.c_str(), payload_.length(), confDirPath);
	}
	/**
	 * @fn	void UdpSendInterface::IsStreamSendtoCsInShortSize(int lane_id_, int src_stid_, int dst_stid_, std::string payload_, int length_)
	 *
	 * @brief	IS送信インタフェース (ITSデータ用)
	 *
	 * @author	Shinichi Kusayama
	 * @date	2023/02/02
	 *
	 * @param 		  	lane_id_		レーン
	 * @param 		  	src_stid_   	送信元SID
	 * @param 		  	dst_stid_   	宛先SID
	 * @param			payload			送信データペイロード部
	 * @param			length_			送信データサイズ
	 */
	//void UdpSendInterface::IsStreamSendtoCsInShortSize(sockaddr_un server_addr_, int lane_id_, int src_stid_, int dst_stid_, char *payload_, int length_)
	void UdpSendInterface::IsStreamSendtoCsInShortSize(unsigned long long lane_id_, unsigned long long src_stid_, unsigned long long dst_stid_, char *payload_, int length_)
	{
		buf_.src_station_id = src_stid_;
		buf_.dst_station_id = dst_stid_;
		buf_.retry_level = 0;
		buf_.retry_lifetime = 0;
		buf_.transmission_flag = 0;
		buf_.lane_id = lane_id_;
		buf_.msg_type = -1 * (DM2TYPE_IS + 10);
		buf_.priority_level = 0; //優先度非対応インタフェースなので0固定
		buf_.fd_name[0] = '\0';
		memset(buf_.dm2_payload, 0, MSGSIZE);
		memcpy(buf_.dm2_payload, payload_, length_);
		buf_.flagment_sum = 1;
		if(SendPacket(buf_) < 0){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Sendto fail." << std::endl;
		}
	}


	/**
	 * @brief 優先度付きIS送信インタフェース
	 * 
	 * @param 		  	lane_id_		レーン
	 * @param 		  	src_stid_   	送信元SID
	 * @param 		  	dst_stid_   	宛先SID
	 * @param 		  	retry_level_	再送処理レベル
	 * @param 		  	retry_lifetime_	再送処理継続時間
	 * @param 			priority_level_	優先度
	 * @param			payload			送信データペイロード部
	 * 
	 * @return int 送信結果 -1:sendto fail. 0:成功
	 */
	int UdpSendInterface::IsStreamSendtoCsPriority(unsigned long long lane_id_, unsigned long long src_stid_, unsigned long long dst_stid_, short retry_level_, int retry_lifetime_, int priority_level_, const std::string &payload_, const std::string &confDirPath)
	{
		int res = 0;
		buf_.src_station_id = src_stid_;
		buf_.dst_station_id = dst_stid_;
		buf_.retry_level = retry_level_;
		buf_.retry_lifetime = retry_lifetime_;
		buf_.transmission_flag = 0;
		buf_.lane_id = lane_id_;
		buf_.msg_type = DM2TYPE_IS;
		buf_.priority_level = priority_level_;
		buf_.fd_name[0] = '\0';


		#if LID_PRIORITY == 1
		Util dm2util;
	    // 作成済みの共有メモリのIDを取得する
		// VehicleProcRcvで作成済みの共有メモリをプロセスにアタッチする
		char* const policing_flag = reinterpret_cast<char*>(shmat(dm2util.GetSharedId(confDirPath + SHM_POLICING, 50, 0), 0, 0));
		//std::cout << "IsStreamSendtoCsPriority policing_flag = " << policing_flag << std::endl;
		std::string policing_flag_str = std::string(policing_flag);
		if(policing_flag_str == "0"){
			//分割不要のサイズであればそのまま送信
			if(payload_.length() <= MSGSIZE){
				payload_.copy(buf_.dm2_payload, MSGSIZE);
				buf_.flagment_sum = 1;

				if(SendPacket(buf_) < 0){
					std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Sendto fail." << std::endl;
					res = -1;
				}
			}else{
				std::cout << "送信データ廃棄" << std::endl;
			}
		}else{
			std::cout << "送信データ廃棄" << std::endl;
		}

		// 共有メモリをプロセスから切り離す
		shmdt(policing_flag);

		#else //LID_PRIORITY
		//分割不要のサイズであればそのまま送信
		if(payload_.length() <= MSGSIZE){
			payload_.copy(buf_.dm2_payload, MSGSIZE);
			buf_.flagment_sum = 1;
			if(SendPacket(buf_) < 0){
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Sendto fail." << std::endl;
				res = -1;
			}
		}else{
			std::cout << "送信データ廃棄" << std::endl;
		}

		#endif //LID_PRIORITY
		return res;
	}


	void UdpSendInterface::DmMngUdpSendtoCsMng(sockaddr_un server_addr_, std::string payload_)
	{
		buf_.msg_type = DM2TYPE_DMMNG;
		buf_.priority_level = 0; //優先度非対応インタフェースなので0固定
		//分割不要のサイズであればそのまま送信
		if(payload_.length() <= MSGSIZE){
			payload_.copy(buf_.dm2_payload, MSGSIZE);
			buf_.flagment_sum = 1;
			if(SendPacket(buf_) < 0){
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Sendto fail." << std::endl;
			}
		}else{
			std::cout << "送信データ廃棄" << std::endl;
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
		buf_.src_station_id = src_stid_;
		buf_.dst_station_id = dst_stid_;
		buf_.retry_level = retry_level_;
		buf_.retry_lifetime = retry_lifetime_;
		buf_.transmission_flag = 0;
		buf_.lane_id = 0;
		buf_.msg_type = DM2TYPE_SEC;
		buf_.priority_level = 0; //優先度非対応インタフェースなので0固定
		buf_.fd_name[0] = '\0';
		buf_.payload_size = length_;
		
		#if LID_PRIORITY == 1
		Util dm2util;
		// VehicleProcRcvで作成済みの共有メモリをプロセスにアタッチする
		char* const policing_flag = reinterpret_cast<char*>(shmat(dm2util.GetSharedId(confDirPath + SHM_POLICING, 50, 0), 0, 0));
		// char* const policing_flag = reinterpret_cast<char*>(shmat(seg_id, 0, 0));
		//std::cout << "policing_flag = " << policing_flag << std::endl;
		std::string policing_flag_str = std::string(policing_flag);
		if(policing_flag_str == "0"){
			//分割不要のサイズであればそのまま送信
			if(length_ <= MSGSIZE){
				memset(buf_.dm2_payload, 0, MSGSIZE);
				memcpy(buf_.dm2_payload, payload_, length_);
				buf_.flagment_sum = 1;
				if(SendPacket(buf_) < 0){
					std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Sendto fail." << std::endl;
				}
			}else{
				std::cout << "送信データ廃棄" << std::endl;
			}
		}else{
			std::cout << "送信データ廃棄" << std::endl;
		}

		// 共有メモリをプロセスから切り離す
		shmdt(policing_flag);

		#else //LID_PRIORITY
		//分割不要のサイズであればそのまま送信
		if(length_ <= MSGSIZE){
			memset(buf_.dm2_payload, 0, MSGSIZE);
			memcpy(buf_.dm2_payload, payload_, length_);
			buf_.flagment_sum = 1;
			if(SendPacket(buf_) < 0){
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Sendto fail." << std::endl;
				int error_number = errno; 
				// 文字列として取得する (strerror)
				std::cout << "Error Code: " << error_number << std::endl;
				std::cout << "Message: " << strerror(error_number) << std::endl;
			}
		}else{
			std::cout << "送信データ廃棄" << std::endl;
		}
		#endif //LID_PRIORITY
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
		buf_.msg_type = DM2Type_MNG_CONN_REGIST;
		memcpy(buf_.dm2_payload, &sm, sizeof (send_message_mng));

		// 下記は未使用だが将来の拡張のためにセットしておく
	    buf_.src_station_id = src_stid_;
	    buf_.dst_station_id = dst_stid_;
	    buf_.retry_level = 0;
	    buf_.retry_lifetime = 0;
	    buf_.transmission_flag = 0;
	    buf_.lane_id = 0;
	    buf_.flagment_sum = 1;
	    buf_.priority_level = 0;
		
	    if (SendPacket(buf_) < 0) {
	        std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "Sendto fail." << std::endl;
		}
	    CloseSocketFd();
	}
}
