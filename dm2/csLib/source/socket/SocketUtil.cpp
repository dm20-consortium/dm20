#include "SocketUtil.h"

namespace SocketUtil{
	/**
	* @fn	std::string SocketUtil::combineFragment(send_message& buf_, UnorderedMap<std::string, time_t>& flagment_data_receive_time_map_, UnorderedMap<std::string, std::vector<std::string>>& flagment_data_combination_map_, int division_size_)
	*
	* @brief	電文結合処理
	*
	* @author	Shinichi Kusayama
	* @date		2026/08/11
	*
	* @param [in]		buf_	   	分割元
	* @param [out]	vectorBuf_		結合先
	* @param [in,out]	flagment_data_receive_time_map_		   	受信時刻マップ
	* @param [in,out]	flagment_data_combination_map_  結合データマップ
	*
	* @return   bool 結合結果 
	*/
	bool combineFragment(CS::send_message& buf_, CS::send_message_vector& vectorBuf, CS::UnorderedMap<std::string, time_t>& flagment_data_receive_time_map_, CS::UnorderedMap<std::string, std::vector<std::string>>& flagment_data_combination_map_)
	{
		//print_send_message_header(buf_.header);
		if (buf_.header.flagment_sum == 1) {
			vectorBuf.header = buf_.header;
			vectorBuf.dm2_payload.assign(buf_.dm2_payload, buf_.dm2_payload + buf_.header.payload_size);
			return true;
		}
		const int flagment = buf_.header.flagment_offset;
		const int flagment_max = buf_.header.flagment_sum;
		const std::string key = std::to_string(buf_.header.src_station_id) + std::to_string(buf_.header.flagment_duplication_check_id);

		// 受信時刻を更新
		try {
			flagment_data_receive_time_map_.UnorderedMapUpdate(key, time(NULL));
		} catch (std::out_of_range&) {
			flagment_data_receive_time_map_.UnorderedMapInsert(key, time(NULL));
		}

		// 受信途中のデータが存在するか
		if (flagment_data_combination_map_.UnorderedMapKeyExistVector(key)) {
			try {
				flagment_data_combination_map_.UnorderedMapUpdateVectorValueThenDecrease(key, flagment,
						std::string(buf_.dm2_payload, buf_.header.payload_size));
			} catch (std::out_of_range&) {
				std::cout << "FILE:" << __FILE__ << ", LINE:" << __LINE__ << " out_of_range" << std::endl;
				return false;
			}
			// 全フラグメントが揃ったか確認
			std::string combined_payload = flagment_data_combination_map_.UnorderedMapVectorCombineAndDeletePlusSize(key, flagment_max);
			if (!combined_payload.empty()) {
				//std::cout << "[combineFragment] flagment:" << flagment << ",size:" << combined_payload.length() << std::endl;
				// ヘッダコピー
				vectorBuf.header = buf_.header;
				vectorBuf.header.flagment_offset = 0;
				vectorBuf.header.flagment_sum = 1;
				// ペイロードコピー
				vectorBuf.dm2_payload.assign(combined_payload.begin(), combined_payload.end());
				flagment_data_receive_time_map_.UnorderedMapErase(key);
				return true;
			}
			return false;
		}
		// 初回フラグメント
		try {
			flagment_data_combination_map_.UnorderedMapInsertVectorPlusSize(key, flagment_max, flagment,
				std::string(buf_.dm2_payload, buf_.header.payload_size));
		}
		catch (std::out_of_range&) {
			std::cout << "FILE:" << __FILE__ << ", LINE:" << __LINE__ << " out_of_range" << std::endl;
		}

		return false;
	}
	/**
	* @fn	void SocketUtil::ClearUnorderedMap(int& combination_map_clear_time_th, UnorderedMap<std::string, std::vector<std::string>>& flagment_data_combination_map_th, UnorderedMap<std::string, time_t>& flagment_data_receive_time_map_th
	*
	* @brief	電文結合マップクリア処理
	*
	* @author	Shinichi Kusayama
	* @date		2026/08/11
	*
	* @param [in]		combination_map_clear_time_th	   	クリア間隔
	* @param [in,out]	flagment_data_combination_map_  結合データマップ
	* @param [in,out]	flagment_data_receive_time_map_		   	受信時刻マップ
	*/
	void ClearUnorderedMap(int& combination_map_clear_time_th, CS::UnorderedMap<std::string, std::vector<std::string>>& flagment_data_combination_map_th, CS::UnorderedMap<std::string, time_t>& flagment_data_receive_time_map_th)
	{
		while(1){
			std::string key_ = "0";
			while(1){
				//◆結合MAP管理用MAPにて現在時刻より指定時間以上古いデータの車両SID＋フラグメントシーケンス番号を取得
				key_ = flagment_data_receive_time_map_th.UnorderedMapOldKeyString(combination_map_clear_time_th);
				if(key_ != "0"){
					//◆取得した車両SIDをkey値として、結合MAPからエントリを削除。
					flagment_data_combination_map_th.UnorderedMapErase(key_);
					//◆取得した車両SIDをkey値として、結合MAP管理用MAPからエントリを削除。
					flagment_data_receive_time_map_th.UnorderedMapErase(key_);
				}else{
					break;
				}
			}
			sleep(combination_map_clear_time_th);
		}
	}
	
	/**
	* @fn	void print_send_message_header(const CS::send_message_header &header)
	*
	* @brief	電文ヘッダプリント処理
	*
	* @author	Shinichi Kusayama
	* @date		2026/08/11
	*
	* @param [in]		header	   	ヘッダ
	*/
	void print_send_message_header(const CS::send_message_header &header){
		std::string res_str = "";
		res_str = "src_station_id:" + std::to_string(header.src_station_id) 
		        + ", dst_station_id:" + std::to_string(header.dst_station_id)
				+ ", src_station_type:" + std::to_string(header.src_station_type) 
				+ ", dst_station_type:" + std::to_string(header.dst_station_type)
				+ ", payload_size:" + std::to_string(header.payload_size)
				+ ", transmission_flag:" + std::to_string(header.transmission_flag) 
				+ ", duplication_check_id:" + std::to_string(header.duplication_check_id) 
				+ ", lane_id:" + std::to_string(header.lane_id) 
				+ ", retry_level:" + std::to_string(header.retry_level) 
				+ ", retry_data_id:" + std::to_string(header.retry_data_id) 
				+ ", retry_lifetime:" + std::to_string(header.retry_lifetime) 
				+ ", msg_type:" + std::to_string(header.msg_type) 
				+ ", cs_message_detail:" + std::to_string(header.cs_message_detail) 
				+ ", fd_name:" + std::string(header.fd_name, sizeof(header.fd_name)) 
				+ ", flagment_duplication_check_id:" + std::to_string(header.flagment_duplication_check_id) 
				+ ", flagment_sum" + std::to_string(header.flagment_sum) 
				+ ", flagment_offset:" + std::to_string(header.flagment_offset) 
				+ ", priority_level:" + std::to_string(header.priority_level);
		std::cout << res_str << std::endl;
	}
}