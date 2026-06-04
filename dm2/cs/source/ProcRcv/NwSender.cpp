#include "NwSender.h"
//#include "CsModule.h"
#include "SidManagement.h"

using namespace std;
using namespace CS;
using namespace log4cxx;
Socket dm2socket;

/**
 * @fn		NwSender::NwSender()
 * @brief	NwSenderコンストラクタ
 * @author	Nagoya University
 * @date	2023/03/21
 * @return
 */
NwSender::NwSender(ProcRcvSettings &tmp_settings, uint count): settings(tmp_settings)
{
	memcpy(&setting_num, &count, sizeof(count));
	// log4cxxの初期化
	logger = Logger::getLogger("NwSender");

	switch(settings.socket_types[setting_num]) {
		case 0:
		case 10:
		case 11:
			client = new UdpNwClient();
			break;
		case 1:
			client = new UdpMcNwClient();
			break;
		case 2:
			client = new DtlsNwClient();
			break;
	}
}

/**
 * @fn		NwSender::~NwSender()
 * @brief	NwSenderデストラクタ
 * @author	Nagoya University
 * @date	2023/03/21
 * @return
 */
NwSender::~NwSender(void)
{
	delete pthread;
	delete client;
}

/**
 * @fn		NwSender::~NwSender()
 * @brief	実行処理
 * @author	Nagoya University
 * @date	2023/03/21
 * @return
 */
std::thread* NwSender::Run(CS::Queue<CS::clientdata>* queue)
{
	LOG4CXX_DEBUG(logger, settings.interface_names[setting_num] + " " + "start");
	p_queue = queue;

	// ネットワーク送信用スレッド生成
	pthread = new thread(sender, this, settings.dm2util.getConfigDirectory());
	LOG4CXX_DEBUG(logger, settings.interface_names[setting_num] + " " + "exit");

	return pthread;
}

/**
 * @brief 送信処理
 * 
 * @param param
 * @param se_cdata 送信データ構造体
 * @param res_addr_first_ Firstエッジ通信用NWドメインソケットのアドレス
 * @param udp_port_number UDPポート番号
 * 
 * @return void
 */
int NwSender::send_to_edge(const NwSender* param, clientdata& se_cdata, addrinfo& res_addr_first_, string udp_port_number)
{
	const NwSender* me = param;	// 自身のメンバへのアクセスはmeを通じて行うこと
	string interface_name = me->settings.interface_names[me->setting_num];
	int socket_type = me->settings.socket_types[me->setting_num];
	int cs_packet_size = me->settings.cs_packet_sizes[me->setting_num];
	string aesKey = me->settings.udp_etm_aes_keys[me->setting_num];

	//エッジに送信
	//unsigned long long now_time;
	string log_str = "";
	struct send_message buf_ = {};

	//(se_cdata.msg).src_station_id = me->settings.my_sid;
	//struct timespec ts;
	//timespec_get(&ts, TIME_UTC);
	//(se_cdata.msg).duplication_check_id = ts.tv_sec * 1000000000 + ts.tv_nsec;
	log_str = Util(me->dm2util).PrintSend_message(se_cdata.msg);
	LOG4CXX_DEBUG(me->logger, interface_name + " " + log_str);

	//LOG4CXX_DEBUG(me->logger, "se_cdata.msg.dm2_payload = " + std::string(se_cdata.msg.dm2_payload));

#if TRACELOG == 1
	// tracelogの取得と格納
	get_tracelog(trace_on, ref(se_cdata.msg), my_sid, VPRWT, rcv_q.Size());
#endif
	
	//clock_gettime(CLOCK_REALTIME, &ts);
	//now_time = ts.tv_sec * 1000000000 + ts.tv_nsec;
	
	int res_sendto = 0;
	// 送信
	buf_ = se_cdata.msg;
	switch(socket_type) {
		case 0:
			LOG4CXX_DEBUG(me->logger, "UDP（ユニキャスト, 暗号化なし）送信開始");
			res_sendto = ((UdpNwClient*)(me->client))->SendtoDivision(buf_, res_addr_first_, cs_packet_size, udp_port_number);
			break;
		case 10:
			LOG4CXX_DEBUG(me->logger, "UDP（ユニキャスト, 暗号化あり）送信開始");
			res_sendto = ((UdpNwClient*)(me->client))->SendtoDivision(buf_, res_addr_first_, cs_packet_size, udp_port_number, socket_type, aesKey);
			break;
		case 11:
			LOG4CXX_DEBUG(me->logger, "UDP（ユニキャスト, 暗号化・署名あり）送信開始");
			res_sendto = ((UdpNwClient*)(me->client))->SendtoDivision(buf_, res_addr_first_, cs_packet_size, udp_port_number, socket_type, aesKey);
			break;
		case 1:
			LOG4CXX_DEBUG(me->logger, "UDP（マルチキャスト）送信開始");
			res_sendto = ((UdpMcNwClient*)(me->client))->SendtoDivision(buf_, res_addr_first_, cs_packet_size, udp_port_number);
			break;
		case 2:
			LOG4CXX_DEBUG(me->logger, "DTLS 送信開始");
			res_sendto = ((DtlsNwClient*)(me->client))->SendtoDivision(buf_, cs_packet_size);
			break;
	}
	if(res_sendto < 0){
		log_str = "send Failed. res_sendto = " + to_string(res_sendto);
		LOG4CXX_WARN(me->logger, interface_name + " " + log_str);
	}else{
		log_str = "send Complete.";
		LOG4CXX_DEBUG(me->logger, interface_name + " " + log_str);
	}
	// クローズ処理
	switch(socket_type) {
		case 0:
		case 10:
		case 11:
			LOG4CXX_DEBUG(me->logger, "UDP Close START");
			((UdpNwClient*)(me->client))->CloseSocketFd();
			break;
		case 1:
			break;
		case 2:
			//LOG4CXX_DEBUG(me->logger, "DTLS Close START");
			//((DtlsNwClient*)(me->client))->CloseSocketFd();
			break;
	}
	LOG4CXX_DEBUG(me->logger, "Close END");
	return res_sendto;
}

/**
 * @brief 送信データ処理スレッド
 * @return void
 */
void NwSender::sender(const NwSender* param, const string confDirPath)
{
	bool first_dtls = true;
	NwSender* me = const_cast<NwSender*>(param);
	string interface_name = me->settings.interface_names[me->setting_num];
	string sid2ipFileName = me->settings.send_lists[me->setting_num];
	int socket_type = me->settings.socket_types[me->setting_num];

	LOG4CXX_DEBUG(me->logger, interface_name + " " + "start");
	string udp_port_number = me->settings.udp_port_numbers[me->setting_num];
	string ip_address = "";

 	addrinfo res_addr_first_;
	me->sidManagement.init(sid2ipFileName);
	//------------------------------------------------------------
	// キューからデータを取得し、外部向けの送信処理を行う
	//------------------------------------------------------------

	string log_str = "";
	struct clientdata w_cdata; //受信データ　＋　送信元IPアドレスの構造体
	
	bool errOutput = true;
	bool exit_flg = false;
	while(true)
	{
		// キューからデータを取得して送信
		while (!me->p_queue->Empty()) {
			if (me->exit_run_thread) {
				std::string s_ip_address = ip_address;
				if (socket_type == 2) {
					((DtlsNwClient*)(me->client))->CloseSocketFd();
				}
				LOG4CXX_INFO(me->logger, "Exit Thread No." + std::to_string(me->setting_num) + ", Socket Type:" + std::to_string(socket_type) + ", Dtls SID:" + std::to_string(me->settings.dtls_dest_sids[me->setting_num]) + ", IP:" + ip_address);
				return;
			}
			w_cdata = me->p_queue->Pop();
			string log_str = Util(me->dm2util).PrintClient_data(w_cdata);
			LOG4CXX_DEBUG(me->logger, log_str);
			if(me->settings.isFlagTrue(me->setting_num))
			{
				me->sidManagement.init(sid2ipFileName); // IPアドレスリストの更新
				me->settings.resetFlag(me->setting_num);
				LOG4CXX_INFO(me->logger, "IP LIST Changed NwSender Thread No." + std::to_string(me->setting_num) + ", Socket Type:" + std::to_string(socket_type) + ", Dtls SID:" + std::to_string(me->settings.dtls_dest_sids[me->setting_num]));
				errOutput = true;
			}
			try {
				// sidからIPに変換する
				ip_address = me->sidManagement.sid2ip(w_cdata.msg.dst_station_id);
				LOG4CXX_DEBUG(me->logger, "SID:" + std::to_string(w_cdata.msg.dst_station_id) + ", IP:" + ip_address);
			}
			catch (...)
			{
				if (errOutput) {
					string errmsg = to_string(w_cdata.msg.dst_station_id);
					LOG4CXX_ERROR(me->logger, "[sid-ip変換エラー]sid:" + errmsg + ",file:" + sid2ipFileName);
					errOutput = false;
				}
				continue;
			}

			int ip_version = 4;
			if (ip_address.find(":") != std::string::npos) ip_version = 6;

			// キューを受け取ったタイミングで送り先へ接続を行う
			switch(socket_type) {
				case 0:
					res_addr_first_ = ((UdpNwClient*)(me->client))->Init(udp_port_number, ip_address);
					break;
				case 10:
				case 11:
					res_addr_first_ = ((UdpNwClient*)(me->client))->Init_path(udp_port_number, ip_address, confDirPath);
					break;
				case 1:
					res_addr_first_ = ((UdpMcNwClient*)(me->client))->Init(udp_port_number, ip_address, interface_name, "", ip_version);
					break;
				case 2:
					LOG4CXX_DEBUG(me->logger, "first_dtls:" + std::to_string(first_dtls));
					if (first_dtls) {
						string src_ip = dm2socket.GetIPifaddrs(interface_name, "", ip_version);
						res_addr_first_ = ((DtlsNwClient*)(me->client))->Init(udp_port_number, ip_address, src_ip, confDirPath, exit_flg);
						if (exit_flg) {
							std::string s_ip_address = ip_address;
							LOG4CXX_INFO(me->logger, "Exit Thread No." + std::to_string(me->setting_num) + ", Socket Type:" + std::to_string(socket_type) + ", Dtls SID:" + std::to_string(me->settings.dtls_dest_sids[me->setting_num]) + ", IP:" + ip_address);
							return;
						}
						first_dtls = false;
					}
					break;
			}
			// 送信
			int send_rtn = me->send_to_edge(me, ref(w_cdata), ref(res_addr_first_), udp_port_number);
			if ((socket_type == 2) && send_rtn == -1) {
				if (socket_type == 2) {
					LOG4CXX_INFO(me->logger, "[After send_to_edge]CloseSocketFd");
					((DtlsNwClient*)(me->client))->CloseSocketFd();
					first_dtls = true;
				}
			}
		}
	}
}

/**
 * @fn		NwSender::getSocket();
 * @brief	ソケット取得
 * @author	Shinichi Kusayama
 * @date	2025/9/17
 * @return
 */
Socket* NwSender::getSocket()
{
	return client;
}

/**
 * @fn		NwSender::setRunExitThreadFlg();
 * @brief	Runスレッド終了フラグセット
 * @author	Shinichi Kusayama
 * @date	2025/9/17
 * @return
 */
void NwSender::setRunExitThreadFlg()
{
	exit_run_thread.store(true);
}