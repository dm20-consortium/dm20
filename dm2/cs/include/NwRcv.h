#ifndef __NWRCV_H__
#define __NWRCV_H__

#include "Cs.h"
#include "DtlsNwServer.h"

using namespace std;

namespace CS {
	/**
	 * @class	NwRcvSettings
	 * @brief	プロセス間通信受信 設定クラス
	 * @author	Nagoya University
	 * @date	2023/02/12
	 */
	class NwRcvSettings
	{
	public:
		NwRcvSettings();		
		~NwRcvSettings() {};
		bool load_dm2conf(const std::string &confDirPath);
		void make_dm2conf(std::string my_sid, std::string nic, std::string socket_type);
		void usage(const char *cmd);
		Util dm2util;					// utility

		//--- confファイル読み込み Start ---
		//int my_sid; 								// 自身のStation ID
		unsigned long long my_sid; 					// 自身のStation ID
		int network_num;							// ネットワーク数
		string is_ip_address;						// ISのIPアドレス（送信用）
		string is_port_number;						// ISのポート番号（送信用）
		vector<string> interface_names;				// インタフェース名(複数)
		vector<int> interface_ip_vers;			// IPバージョン(複数)
		vector<string> udp_port_numbers;							// ポート番号
		vector<int> socket_types;                                   // 通信タイプ（0:UDP(エニーキャスト)、1:UDP(マルチキャスト)、DTLS）
		vector<int> cs_packet_sizes;                // CSパケットサイズ
		vector<string> udp_etm_aes_keys;			// AESキー
		int dual_map_clear_time;					// 重複データ管理マップの掃除までの間隔(秒)
		int max_sendto_times;						// (UDP)再送回数上限値
		int cs_duplicate_check;						// 複数通信時の重複チェックフラグ
		int delay_time;								// 通信遅延時間(dm20.confより読み込み)
		int retry_map_clear_time;					// 再送処理用のMAPの古いデータの削除処理間隔(秒) 
		int trace_on;								// トレースログのON/OFFフラグ
		//--- confファイル読み込み End ---

		// 再送処理フィルター機能用非順序型MAPオブジェクト生成。key値：車両SID＋retry_data_map, value値：受信時刻
		UnorderedMap<string, time_t> retry_data_filter_time_map;

		//--- vehicle & edge ---
		// 重複確認用非順序型MAPオブジェクト生成。key値：車両SID＋シーケンス番号, value値：受信時刻
		UnorderedMap<string, time_t> vehicle_sid_sq_id_received_time_map;

		//--- vehicle & cloud ---
		//Queue<clientdata> rcv_q(MAX_QUEUE_SIZE);	// キューオブジェクト生成

	private:
		log4cxx::LoggerPtr logger;		// log
	};
}

#endif
