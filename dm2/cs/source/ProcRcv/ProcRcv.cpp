#include <list>
#include <string>
#include <memory>
#include "ProcRcv.h"
#include "ProcReceiver.h"
#include "NwSender.h"
#include "SidManagement.h"
#include "DtlsContext.h"
#include "I2vSignatureProcessor.h"

using namespace std;
using namespace CS;
using namespace log4cxx;

// グローバルに共有（NwSender の起動に関わるパラメータ）
vector<INwSender*> senders;			// インスタンス格納リスト
vector<Queue<clientdata>*> queues;	// キュー
vector<std::thread*> threads;		// スレッド
std::mutex sender_mutex;

/**
 * @fn	AddNewSender
 * @brief	NwSender追加用関数
 * @author	Shinichi Kusayama
 * @date	2025/9/11
 */
void AddNewSender(ProcRcvSettings& settings, uint count) {
    std::lock_guard<std::mutex> lock(sender_mutex);

    CS::INwSender* p_sender = new NwSender(settings, count);
    Queue<clientdata>* p_queue = new Queue<clientdata>(MAX_QUEUE_SIZE);

    senders.push_back(p_sender);
    queues.push_back(p_queue);

    std::thread* p_nwsender_thread = p_sender->Run(p_queue);

    threads.push_back(p_nwsender_thread);
}

/**
 * @fn	StopNewSender
 * @brief	NwSenderスレッド停止用関数
 * @author	Shinichi Kusayama
 * @date	2025/9/11
 */
void StopNewSender(uint idx) {
	INwSender* p_sender = senders[idx];
	if (auto* client = dynamic_cast<DtlsNwClient*>(p_sender->getSocket())) {
 		client->CloseSocketOnly();
		p_sender->setRunExitThreadFlg();
	} else {
    	std::cerr << "DtlsNwClient cast failed.\n";
	}
	return;
}
/**
 * @fn	int main(int argc, char *argv[])
 *
 * @brief	manage処理
 *
 * @author	Nagoya University
 * @date	2021/03/21
 *
 * @return
 */

int main(int argc, char *argv[])
{
	ProcRcvSettings settings;
	int ch;
	std::string confDirPath = "";
	std::string opt = "";
	std::string sid_id_map_ip = "";
	std::string sid_id_map_sid = "";
	std::string nic = "";
	std::string my_sid = "";
	std::string socket_type = "udp";
	bool test_mode = false;
	while ((ch = getopt(argc, argv, "d:i:n:r:s:t:h")) != -1) {
		switch (ch) {
		case 'd':
			opt = optarg;
			if (opt.substr(0,1) != "-") confDirPath = opt;
			break;
		case 'i':
			test_mode = true;
			opt = optarg;
			if (opt.substr(0,1) != "-") sid_id_map_ip = opt;
			break;
		case 'n':
			test_mode = true;
			opt = optarg;
			if (opt.substr(0,1) != "-") nic = opt;
			break;
		case 'r':
			test_mode = true;
			opt = optarg;
			if (opt.substr(0,1) != "-") {
				try {
					sid_id_map_sid = opt;
					stoull(opt);
				} catch (...) {
					fprintf(stderr, "failed to parse station-id-value: %s\n", opt.c_str());
					settings.usage(argv[0]);
					exit(1);
				}
			}
			break;
		case 's':
			test_mode = true;
			opt = optarg;
			if (opt.substr(0,1) != "-") {
				try {
					my_sid = opt;
					stoull(opt);
				} catch (...) {
					fprintf(stderr, "failed to parse station-id-value: %s\n", opt.c_str());
					settings.usage(argv[0]);
					exit(1);
				}
			}
			break;
		case 't':
			opt = optarg;
			if (opt.substr(0,1) != "-") {
				if (opt == "udp" || opt == "dtls") {
					socket_type = opt;
				} else {
					fprintf(stderr, "failed to parse socket-type-value [udp/dtls]: %s\n", opt.c_str());
					settings.usage(argv[0]);
					exit(1);
				}
			}
			break;
		default:
			settings.usage(argv[0]);
		    exit(1);
		}
	}
	if (test_mode) {
		if (sid_id_map_ip == "" || nic == "") {
			fprintf(stderr, "2 options are required -i<IP>: %s -n<NIC>: %s [Option] -r<Receiver's SID>: %s[Default:90000000] -s<my SID>: %s[Default:201]\n", sid_id_map_ip.c_str(), nic.c_str(), sid_id_map_sid.c_str(), my_sid.c_str());
			exit(1);
		}
	}
	if (confDirPath == "") {
		// 環境変数を取得
		string env_path = settings.dm2util.getAndCheckConfDirPath();
		if (env_path != "") confDirPath = env_path;
	}
	if (confDirPath != "") settings.dm2util.setConfigDirectory(confDirPath);

	// // log4cxxの初期化
	LoggerPtr logger = Logger::getLogger("ProcRcv_main");
	PropertyConfigurator::configure(settings.dm2util.getConfigDirectory() + CONF_FILE_PROCRCV_LOG);
	setlocale(LC_CTYPE, "");

	ProcReceiver* proc_receiver;
	if (test_mode) {
		settings.make_dm2conf(my_sid, nic, sid_id_map_sid, sid_id_map_ip, socket_type);
	} else {
		// 初期設定ファイル読み込み
		if (!settings.load_dm2conf(settings.dm2util.getConfigDirectory()))
		{
			return EXIT_FAILURE;
		}
	}

	// クラス初期化
	LOG4CXX_DEBUG(logger, "ProcReceiver, NwSender生成");
	proc_receiver = new ProcReceiver(settings);

	// 初期設定ファイルに定義されたネットワーク情報の個数分Nwsenderを生成する
	for(uint count = 0; count < (uint)settings.network_num; count++) {
		AddNewSender(settings, count);
	}
	
    // ProcReceiver にNwSender追加用コールバックを登録
    proc_receiver->SetOnNewSender(AddNewSender);
    proc_receiver->SetOnStopSender(StopNewSender);

	// データ送信依頼待ち開始
	LOG4CXX_DEBUG(logger, "ProcReceiver開始");
	std::thread* proc_receiver_thread = proc_receiver->Run(&queues);

	LOG4CXX_DEBUG(logger, "全スレッド起動処理完了");

	// [TODO]コンソール待ち(設定表示, 設定変更)

	
	// 全スレッドの待ち
	for(uint index = 0; index < threads.size(); index++) {
		threads[index]->join();
	}
	proc_receiver_thread->join();

	LOG4CXX_INFO(logger, "終了");

	// receiver破棄
	delete proc_receiver;

	// sendersの破棄
	for(uint index = 0; index < senders.size(); index++) {
		INwSender* p_sender = senders[index];
		delete p_sender;
	}
	senders.clear();
	threads.clear();

	// queuesの破棄
	for(uint index = 0; index < queues.size(); index++) {
		Queue<clientdata>* p_queue = queues[index];
		delete p_queue;
	}
	queues.clear();
	
	return EXIT_SUCCESS;
}

/**
 * @fn	ProcRcvSettings::ProcRcvSettings()
 * @brief	ProcRcvSettingsコンストラクタ
 * @author	Nagoya University
 * @date	2021/03/21
 */
ProcRcvSettings::ProcRcvSettings()
{
	logger = Logger::getLogger("ProcRcvSettings");

	my_sid = 0;								// disable
	interface_names.clear();
	udp_port_numbers.clear();
	send_lists.clear();
	socket_types.clear();
	retry_interval = -1;					// disable
	retry_send_interval = -1;				// disable
	retry_map_clear_time = -1;				// disable
	max_sendto_times = -1;					// disable
	trace_on = 0;							// OFF
	for (auto& flag : threadFlags)
	{
    	flag.store(false, std::memory_order_relaxed); //false
	}
}

/**
 * @fn	ProcRcvSettings::set_dtls_conf_copy()
 * @brief	DTLSの設定をコピーする
 * @author	Shinichi Kusayama
 * @date	2025/09/17
 */
void ProcRcvSettings::set_dtls_conf_copy()
{
	interface_names.push_back(interface_names[dtls_idx]);
	udp_port_numbers.push_back(udp_port_numbers[dtls_idx]);
	send_lists.push_back(send_lists[dtls_idx]);
	socket_types.push_back(socket_types[dtls_idx]);
	cs_packet_sizes.push_back(cs_packet_sizes[dtls_idx]);
}

/**
 * @fn	ProcRcvSettings::load_dm2conf()
 * @brief	dm2conf読込処理
 * @author	Nagoya University
 * @date	2021/03/21
 * @return  true/false
 */
bool ProcRcvSettings::load_dm2conf(const std::string &confDirPath)
{
	if (!dm2util.chk_dm2conf_unsigned_long_long_item("MY_STATION_ID", my_sid, true)) return false;

	if (!dm2util.chk_dm2conf_int_item("SEND_NETWORK_NUM", network_num, true)) return false;

	if (!dm2util.chk_dm2conf_str_item("CS_PORT_NUMBER", cs_port_number, false)) cs_port_number = "";
	
	if (!dm2util.chk_dm2conf_str_item("INTERFACE_BY_IS_CS", interface_by_is_cs, false)) interface_by_is_cs = "";
	
	for (int count = 1; count < network_num + 1; count++) {
		string item_str;
		int item_int;
		if (!dm2util.chk_dm2conf_str_item("INTERFACE_NAME_" + to_string(count), item_str, true)) return false;
		interface_names.push_back(item_str);

		if (!dm2util.chk_dm2conf_str_item("UDP_PORT_NUMBER_" + to_string(count), item_str, true)) return false;
		udp_port_numbers.push_back(item_str);
		
		if (dm2util.chk_dm2conf_str_item("SEND_LIST_" + to_string(count), item_str, true)) {
			send_lists.push_back(dm2util.getConfigDirectory() + item_str);
		} else {
			if (count == 1) {
				return false;
			} else {
				send_lists.push_back(send_lists[0]);
			}
		}

		if (!dm2util.chk_dm2conf_int_item("SOCKET_TYPE_" + to_string(count), item_int, true)) return false;
		socket_types.push_back(item_int);
		if (item_int == 2) {
			SEC::DtlsContext Context = SEC::DtlsContext();
			if (!Context.CheckClientContext(confDirPath)) {
				return false;
			}
		} else if (item_int == 11) {
			if (!SEC::I2vSignatureProcessor::checkClientSignature(confDirPath, my_sid)) {
				return false;
			}
		}
		if (!dm2util.chk_dm2conf_str_item("UDP_ETM_AES_KEY_" + to_string(count), item_str, false)) item_str = "";
		if (item_str == "") {
			if (item_int == 10 || item_int == 11) {
				cerr << "[dm2.conf] UDP_ETM_AES_KEY_" + to_string(count) + "に有効な値が入っていません" << endl;
				return false;
			}
		}
		udp_etm_aes_keys.push_back(item_str);
		if (!dm2util.chk_dm2conf_int_item("CS_PACKET_SIZE_" + to_string(count), item_int, true)) return false;
		cs_packet_sizes.push_back(item_int);

	}
	// DTLSの場合は、送信者の数（SEND_LISTの行数分）だけsettingを追加
	int dtls_add_count = 0;
	for (int idx = 0; idx < network_num; idx++) {
		if (socket_types[idx] != 2) {
			// DTLS以外でもインデックスを合わせるために、dtls_dest_sids配列に追加しておく。
			dtls_dest_sids.push_back(0);
			continue;
		}
		dtls_idx = idx;
		SidManagement *sidManagement = new SidManagement();
		sidManagement->init(send_lists[idx]);
		sidManagement->show();
		//vector<int> keys = sidManagement->getKeys();
		vector<unsigned long long> keys = sidManagement->getKeys();
		delete sidManagement;
		int keys_size = keys.size();
		for (int key_idx = 0; key_idx < keys_size; key_idx++) {
			dtls_dest_sids.push_back(keys[key_idx]);
			if (key_idx != 0) {
				set_dtls_conf_copy();
			}
		}
		if (keys_size > 1) dtls_add_count += (keys_size - 1);
	}
	network_num += dtls_add_count;

	if (!dm2util.chk_dm2conf_int_item("MAX_SENDTO_TIMES", max_sendto_times, false)) return false;

	if (!dm2util.chk_dm2conf_int_item("TRACE_ON", trace_on, false)) return false;

	return true;
}
/**
 * @fn	ProcRcvSettings::make_dm2conf()
 * @brief	テストモード時の設定項目生成処理
 * @author	Nagoya University
 * @date	2021/03/21
 * @return  true/false
 */
void ProcRcvSettings::make_dm2conf(std::string my_sid_str, std::string nic, std::string dest_sid_, std::string dest_ip, std::string socket_type_str)
{
	int socket_type = 0;
	std::string tmp_dm2_conf_fname = "/tmp/dm2cs_send.conf";
	std::string tmp_dm2_conf_text;
	std::string tmp_send_list_fname = "/tmp/dm2cs_send_list.csv";
	my_sid = 201;
	std::string dest_sid = "90000000";		// Group Cast SID
	network_num = 1;
	int tmp_cs_packet_size = 1442;
	
	if (my_sid_str != "") my_sid = stoull(my_sid_str);
	if (dest_sid_ != "") dest_sid = dest_sid_;
	interface_names.push_back(nic);
	udp_port_numbers.push_back("55555");
	ofstream send_list_file(tmp_send_list_fname);
	std::string tmp_send_list_text = dest_sid + "," + dest_ip;
	send_list_file<<tmp_send_list_text;
	send_list_file.close();
	send_lists.push_back(tmp_send_list_fname);
	dtls_dest_sids.push_back(stoull(dest_sid));
	if (socket_type_str == "dtls") {
		socket_type = 2;
	}
	socket_types.push_back(socket_type);
	cs_packet_sizes.push_back(tmp_cs_packet_size);
	max_sendto_times = 10;
	trace_on = 0;

	ofstream dm2conf_file(tmp_dm2_conf_fname);
	tmp_dm2_conf_text = "MY_STATION_ID = " + to_string(my_sid) + "\nSEND_NETWORK_NUM = 1\nRECEIVE_NETWORK_NUM = 1\nINTERFACE_NAME_1 = " + nic 
					+ "\nINTERFACE_IP_VER_1 = 4\nUDP_PORT_NUMBER_1 = 55555\nSOCKET_TYPE_1 = " + to_string(socket_type) + "\nSEND_LIST_1 = " + tmp_dm2_conf_fname
					+ "\nCS_PACKET_SIZE_1 = " + to_string(tmp_cs_packet_size) + "\nCOMBINATION_MAP_CLEAR_TIME = 100\nDUAL_MAP_CLEAR_TIME = 100\nMAX_SENDTO_TIMES = 10\nCS_DUPLICATE_CHECK = 1\nTRACE_ON = 0";
	dm2conf_file<<tmp_dm2_conf_text;
	dm2conf_file.close();
	std::cout << "[" << tmp_dm2_conf_fname << "]" << std::endl;
	std::cout << tmp_dm2_conf_text << std::endl;
	std::cout << "[" << tmp_send_list_fname << "]" << std::endl;
	std::cout << tmp_send_list_text << std::endl;
}
/**
 * @fn	ProcRcvSettings::setAllFlagsTrue()
 * @brief	フラグセット
 * @author	Nagoya University
 * @date	2025/2/28
 */
void ProcRcvSettings::setAllFlagsTrue()
{
	for (auto& flag : threadFlags)
	{
    	flag.store(true, std::memory_order_relaxed);
	}
}
/**
 * @fn	ProcRcvSettings::isFlagTrue()
 * @brief	フラグ判定
 * @author	Nagoya University
 * @date	2025/2/28
 */
bool ProcRcvSettings::isFlagTrue(std::size_t index)
{
	return threadFlags[index].load(std::memory_order_relaxed);
}
/**
 * @fn	ProcRcvSettings::setFlag()
 * @brief	フラグセット
 * @author	Shinichi Kusayama
 * @date	2025/9/17
 */
void ProcRcvSettings::setFlag(std::size_t index)
{
	threadFlags[index].store(true, std::memory_order_relaxed);
}
/**
 * @fn	ProcRcvSettings::resetFlag()
 * @brief	フラグリセット
 * @author	Nagoya University
 * @date	2025/2/28
 */
void ProcRcvSettings::resetFlag(std::size_t index)
{
	threadFlags[index].store(false, std::memory_order_relaxed);
}
/**
 * @fn	ProcRcvSettings::usage()
 * @brief	引数説明
 * @author	Shinichi Kusayama
 * @date	2024/4/17
 */
void ProcRcvSettings::usage(const char *cmd)
{
	printf("Usage: %s [options] \n"
		"\n"
		"Options: [] => Required arg. {} => Any arg. \n"
		"  -d <directory path>      Set Config Directory Path. \n"
		"                            [Another Way] export DM2_CONF_DIR_PATH=/home/dm2/dm2/conf\n"
		"  -h                       Show help. \n"
		"  ------------------------------------------ \n"
		"  [Test Mode Not Using dm2.conf]            \n"
		"   -d And more Set the 2 Options]           \n"
		"   dm2cs_send ---------------> dm2cs_recv   \n"
		"     [-n:NIC]                 [-n:NIC]      \n"
		"          [-i:recv's IP]                    \n"
		"  ------------------------------------------ \n"
		"  ex.) \n"
		"    [Recevier] $ dm2cs_recv -d /home/dm2/dm2/conf -n eth0 \n"
		"    [Sender  ] $ dm2cs_send -d /home/dm2/dm2/conf -n eth0 -i 192.168.0.2 \n"
		"                                              \n"
		"  [Options] (*) Default                       \n"
		"     -t <Socket Type>    udp(*), dtls         \n"
		"     -s <My SID>         201(*)               \n"
		"     -r <recv's SID>     90000000(*)          \n"
		"                         Group Cast SID       \n"
		"  ------------------------------------------  \n"
		"\n",
		cmd);
}
