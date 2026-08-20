#include "ProcReceiver.h"
#include "SidManagement.h"

using namespace std;
using namespace CS;
using namespace log4cxx;

/**
 * @fn		ProcReceiver::ProcReceiver()
 * @brief	ProcReceiverコンストラクタ
 * @author	Nagoya University
 * @date	2023/03/21
 * @return
 */
ProcReceiver::ProcReceiver(ProcRcvSettings &tmp_settings): settings(tmp_settings)
{
	// log4cxxの初期化
	logger = Logger::getLogger("ProcReceiver");
}

/**
 * @fn		ProcReceiver::ProcReceiver()
 * @brief	ProcReceiverデストラクタ
 * @author	Nagoya University
 * @date	2023/03/21
 * @return
 */
ProcReceiver::~ProcReceiver()
{
	delete pthread;
}


/**
 * @fn		ProcReceiver::Run()
 * @brief	起動処理
 * @author	Nagoya University
 * @date	2023/03/21
 * @return
 */
std::thread* ProcReceiver::Run(vector<Queue<send_message_vector>*>* pvec_queues)
{
	p_queues = pvec_queues;
	pthread = new thread(receiver, this, settings.dm2util.getConfigDirectory());

	return pthread;
}

/**
 * @fn		ProcReceiver::receiveDataToQueue
 * @brief	プロセス間通信待ち受け
 * @author	Nagoya University
 * @date	2026/6/4
 * @return
 */
void ProcReceiver::receiveDataToQueue(const ProcReceiver* me, struct send_message_vector &buf)
{
	struct timespec ts;
	if (buf.header.msg_type == DM2Type_MNG_CONN_REGIST) {
		// ペイロードの中身から、SIDの更新があるかチェックする。
		me->sid_update_check(me, buf.dm2_payload);
	} else {
		if (buf.header.transmission_flag == 0 || buf.header.duplication_check_id == 0) {
			// 送信元ID・重複チェックIDをセット（このルートに入らない場合は電文内の送信元ID・重複チェックIDが引き継がれる）
			buf.header.src_station_id = me->settings.my_sid;
			timespec_get(&ts, TIME_UTC);
			buf.header.duplication_check_id = ts.tv_sec * 1000000000 + ts.tv_nsec;
		}
		// 保有している全ネットワークに対し送信を行う
		for(uint index = 0; index < me->p_queues->size(); index++)
		{
			/// DTLS且つ、設定されたSIDが電文のSIDと相違ある場合は読み飛ばす
			if (me->settings.socket_types[index] == 2 && me->settings.dtls_dest_sids[index] != buf.header.dst_station_id) continue;
			// キューが溜まっている場合はクリアする。
			if (me->p_queues->at(index)->Size() > 10000) {
				while (!me->p_queues->at(index)->Empty()) {
					me->p_queues->at(index)->Pop();
				}
			}
			me->p_queues->at(index)->Push(buf);	// キューに格納し、送信を依頼する
		}
	}
}
/**
 * @fn		ProcReceiver::Receive()
 * @brief	プロセス間通信待ち受け
 * @author	Nagoya University
 * @date	2023/03/21
 * @return
 */
void ProcReceiver::receiver(const ProcReceiver* param, const string confDirPath)
{
	const ProcReceiver* me = param;	// 自身のメンバへのアクセスはmeを通じて行うこと

	LOG4CXX_DEBUG(me->logger, "receiver start");

	//------------------------------------------------------------
	// APLやISからデータを受信し、送信処理(NwSender)へ受け渡す
	//------------------------------------------------------------

	string log_str = "";			// ログ文字列用

	// IS/APLからの受信  [TODO] Init()に逃がせるかも... 失敗した際の対処を要検討
 	struct send_message buf;
	send_message_vector vectorBuf;
	UdpServer server_;
	int res_init = server_.Init(confDirPath + FD_IStoCS, me->settings.interface_by_is_cs, me->settings.cs_port_number);
	//UdpProcServer udpprocserver(me->udpprocserver);
	//int res_init = udpprocserver.Init(confDirPath + FD_IStoCS);
	if(res_init < 0){
		log_str = "udpprocserver.Init fail: " + to_string(res_init);
		LOG4CXX_WARN(me->logger, log_str);
		//exit(EXIT_FAILURE);
	}

	LOG4CXX_DEBUG(me->logger, "受信待ちへ移行");

	int combination_map_clear_time_ = 100;
	UnorderedMap<std::string, time_t> flagment_data_receive_time_map;
	UnorderedMap<std::string, std::vector<std::string>> flagment_data_combination_map;
	std::thread th0(SocketUtil::ClearUnorderedMap, 
			std::ref(combination_map_clear_time_),
			std::ref(flagment_data_combination_map),
			std::ref(flagment_data_receive_time_map));
	int header_size = sizeof(buf) - sizeof(buf.dm2_payload);
	while(1)
	{
		LOG4CXX_DEBUG(me->logger, "before UdpProcServer");
		if (server_.RecvPacket(buf, res_init) <= 0) continue;
		if (SocketUtil::combineFragment(buf, vectorBuf, flagment_data_receive_time_map, flagment_data_combination_map)) {
			receiveDataToQueue(me, vectorBuf);
		}
	}

}

/**
 * @fn		ProcReceiver::sid_update_check()
 * @brief	SID更新チェック
 * @author	Shinichi Kusayama
 * @date	2025/09/17
 * @return
 */
void ProcReceiver::sid_update_check(const ProcReceiver* me, const std::vector<char>& dm2_payload) const
{
	send_message_mng sm;
	if (dm2_payload.size() < sizeof(send_message_mng)) return;
	memcpy(&sm, dm2_payload.data(), sizeof(send_message_mng));
	std::string sm_ip = sm.ip;
	std::string sm_ctl_flag = sm.ctl_flag;
	if (sm.sid == 0 || sm_ip.empty()) {
		LOG4CXX_ERROR(me->logger, "Invalid SID or IP address received.");
		return;
	}
	LOG4CXX_INFO(me->logger, "[receive] SID: " + std::to_string(sm.sid) + ", IP: " + sm_ip + ", FLG: " + sm_ctl_flag);
	bool found_dtls_sid = false;
	bool update_dtls_sid = false;
	string dtlsFileName = "";        
	SidManagement sidManager; 
	for (int idx = 0; idx < me->settings.network_num; idx++) {
		bool sid_ip_update = false;
		bool delete_flg = false;
		string sid2ipFileName = me->settings.send_lists[idx];
		sidManager.init(sid2ipFileName);
		if (me->settings.socket_types[idx] == 2) {
			// DTLSのケース
			dtlsFileName = sid2ipFileName;
			if (me->settings.dtls_dest_sids[idx] == sm.sid) {
				found_dtls_sid = true;
				std::string current_ip = sidManager.sid2ip(sm.sid);
				if (current_ip != sm_ip) {
					// IPアドレスを変更
					sid_ip_update = true;
					update_dtls_sid = true;
					me->onStopSender(idx);
					me->settings.dtls_dest_sids[idx] = 0;
					LOG4CXX_INFO(me->logger, "[Mod] SID: " + std::to_string(sm.sid) + ", IP: " + current_ip + " => " + sm_ip);
				} else {
					if (sm_ctl_flag == "delete") {
						delete_flg = true;
						sid_ip_update = true;
						me->onStopSender(idx);
						me->settings.dtls_dest_sids[idx] = 0;
						LOG4CXX_INFO(me->logger, "[Del] SID: " + std::to_string(sm.sid) + ", IP: " + current_ip + " => " + sm_ip);
					}
				}
			}
		} else {
			sid_ip_update = true;
		}
		if (sid_ip_update) {
			LOG4CXX_INFO(me->logger, "[Add] SID: " + std::to_string(sm.sid) + ", IP: " + sm_ip);
			sidManager.updateSidIp(sm.sid, sm_ip, delete_flg);
			me->settings.setFlag(idx);
		}
	}
	if (dtlsFileName != "") {
		if (!found_dtls_sid) {
			sidManager.init(dtlsFileName);
			LOG4CXX_INFO(me->logger, "[Add] SID: " + std::to_string(sm.sid) + ", IP: " + sm_ip);
			sidManager.updateSidIp(sm.sid, sm_ip, false);
		}
		if (!found_dtls_sid || update_dtls_sid) {
			me->settings.set_dtls_conf_copy();
			me->settings.dtls_dest_sids.push_back(sm.sid);
			me->settings.network_num++;
			if (me->onNewSender) me->onNewSender(me->settings, me->settings.network_num - 1);
		}
	}
}


