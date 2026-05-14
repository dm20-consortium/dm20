#ifndef __PROCRCV_H__
#define __PROCRCV_H__

#include "Cs.h"
#include <atomic>
#include <mutex>

using namespace std;

namespace CS {
	/**
	 * @class	ProcRcvSettings
	 * @brief	プロセス間通信受信 設定クラス
	 * @author	Nagoya University
	 * @date	2023/02/12
	 */
	class ProcRcvSettings
	{
	private:
		log4cxx::LoggerPtr logger;		// log
		std::atomic<bool> ipChangedFlag{false}; // IPアドレスの変更フラグ
		std::array<std::atomic<bool>, 10> threadFlags;  // ネットワークIFの数は最大10と想定
	public:
		ProcRcvSettings();		
		~ProcRcvSettings() {};

		void usage(const char *cmd);
		Util dm2util;					// utility

		bool load_dm2conf(const std::string &confDirPath);
		void make_dm2conf(std::string my_sid, std::string nic, std::string dest_sid, std::string dest_ip, std::string socket_type);
		//--- confファイル読み込み Start ---
		// int my_sid; 								// 自身のStation ID
		unsigned long long my_sid; 					// 自身のStation ID
		int network_num;							// ネットワーク数
		vector<string> interface_names;				// インタフェース名(複数)
		vector<string> udp_port_numbers;			// ポート番号
		vector<string> send_lists;					// cs->bin内、csvファイルのファイル名。SIDからIPに変換するための設定値を記載
		vector<int> socket_types;                   // 通信タイプ（0:UDP(エニーキャスト)、1:UDP(マルチキャスト)、DTLS）
		vector<int> cs_packet_sizes;                // CSパケットサイズ
		vector<unsigned long long> dtls_dest_sids;	// DTLSでの宛先SID
		vector<string> udp_etm_aes_keys;			// AESキー
		int dtls_idx = -1;							// DTLSの設定が格納されているインデックス
		int max_sendto_times;						// (UDP)再送回数上限値
		int retry_interval;							// 再送処理間隔(マイクロ秒)
		int retry_send_interval;					// 再送処理時のパケット送信の間隔(マイクロ秒)
		int retry_map_clear_time;					// 再送処理用のMAPの古いデータの削除処理間隔(秒) 
		int trace_on;								// トレースログのON/OFFフラグ
		//Queue<clientdata> rcv_q(MAX_QUEUE_SIZE);	// キューオブジェクト生成
		//--- confファイル読み込み End ---

		// 再送処理管理用非順序型MAPオブジェクト生成。key値：再送時間 value値：送信データ
		UnorderedMap<unsigned long long, struct send_message> retry_data_map;
		void setAllFlagsTrue();
		void setFlag(std::size_t index);
		void resetFlag(std::size_t index);
		bool isFlagTrue(std::size_t index);
		void set_dtls_conf_copy();
		
	};
}
#endif
