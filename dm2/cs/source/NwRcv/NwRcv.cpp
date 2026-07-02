#include "Cs.h"

#include <list>
#include <string>

#include "NwRcv.h"
#include "NwReceiver.h"
#include "ProcSender.h"
#include "I2vSignatureProcessor.h"

using namespace std;
using namespace CS;
using namespace log4cxx;

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
	NwRcvSettings settings;
	int ch;
	std::string confDirPath = "";
	std::string opt = "";
	std::string nic = "";
	std::string my_sid = "";
	std::string socket_type = "udp";
	bool test_mode = false;
	while ((ch = getopt(argc, argv, "d:n:s:t:h")) != -1) {
		switch (ch) {
		case 'd':
			opt = optarg;
			if (opt.substr(0,1) != "-") confDirPath = opt;
			break;
		case 'n':
			test_mode = true;
			opt = optarg;
			if (opt.substr(0,1) != "-") nic = opt;
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
		if (nic == "") {
			fprintf(stderr, "1 options are required -n<NIC>: %s [Option] -s<my SID>: %s[Default:202]\n", nic.c_str(), my_sid.c_str());
			exit(1);
		}
	}
	if (confDirPath == "") {
		// 環境変数を取得
		string env_path = settings.dm2util.getAndCheckConfDirPath();
		if (env_path != "") confDirPath = env_path;
	}
	if (confDirPath != "") settings.dm2util.setConfigDirectory(confDirPath);
    // log4cxxの初期化
	LoggerPtr logger = Logger::getLogger("NwRcv_main");
    PropertyConfigurator::configure(settings.dm2util.getConfigDirectory() + CONF_FILE_NWRCV_LOG);
    setlocale(LC_CTYPE, "");

	if (test_mode) {
		settings.make_dm2conf(my_sid, nic, socket_type);
	} else {
		// 初期設定ファイル読み込み
		if (!settings.load_dm2conf(settings.dm2util.getConfigDirectory()))
		{
			return EXIT_FAILURE;
		}
	}

    // クラス初期化
    LOG4CXX_INFO(logger, "ProcReceiver, NwSender生成");
    ProcSender *proc_sender = new ProcSender(settings);
    Queue<clientdata>* p_queue = new Queue<clientdata>(MAX_QUEUE_SIZE);

	// 初期設定ファイルに定義されたネットワーク情報の個数分NwReceiverを生成する
	vector<INwReceiver*> receivers;
	vector<std::thread*> threads;

	for(uint count = 0; count < (uint)settings.network_num; count++) {
		INwReceiver* p_receiver = new NwReceiver(settings, count);

		receivers.push_back(p_receiver);

		// ネットワーク受信待ち開始
		LOG4CXX_INFO(logger, "NwReceiver開始");
		std::thread* p_nwreceiver_thread = p_receiver->Run(p_queue);

		threads.push_back(p_nwreceiver_thread);
	}

    // データ送信依頼待ち開始
    LOG4CXX_INFO(logger, "ProcSender開始");
    std::thread* proc_sender_thread = proc_sender->Run(p_queue);

    LOG4CXX_DEBUG(logger, "全スレッド起動処理完了");

    // [TODO]コンソール待ち(設定表示, 設定変更)


	// 全スレッドの待ち
	for(uint index = 0; index < threads.size(); index++) {
		threads[index]->join();
	}
	proc_sender_thread->join();

    LOG4CXX_INFO(logger, "終了");
	

	// sender破棄
	delete proc_sender;

	// receivers破棄
	for(uint index = 0; index < receivers.size(); index++) {
		INwReceiver* p_receiver = receivers[index];
		delete p_receiver;
	}
	receivers.clear();
	threads.clear();

	// queue破棄
	delete p_queue;

    return EXIT_SUCCESS;
}


/**
 * @fn	NwRcvSettings::NwRcvSettings()
 * @brief	NwRcvSettings
 * @author	Nagoya University
 * @date	2021/03/21
 */
NwRcvSettings::NwRcvSettings()
{
	logger = Logger::getLogger("NwRcvSettings");

	my_sid = 0;							// disable
	interface_names.clear();
	udp_port_numbers.clear();
	interface_ip_vers.clear();
	socket_types.clear();
	max_sendto_times = -1;					// disable
	trace_on = 0;							// OFF
}

/**
 * @fn	NwRcvSettings::load_dm2conf()
 * @brief	dm2conf読込処理
 * @author	Nagoya University
 * @date	2021/03/21
 * @return  true/false
 */
bool NwRcvSettings::load_dm2conf(const std::string &confDirPath)
{
	if (!dm2util.chk_dm2conf_unsigned_long_long_item("MY_STATION_ID", my_sid, true)) return false;

	if (!dm2util.chk_dm2conf_int_item("RECEIVE_NETWORK_NUM", network_num, true)) return false;

	if (!dm2util.chk_dm2conf_str_item("IS_IP_ADDRESS", is_ip_address, false)) is_ip_address = "";

	if (!dm2util.chk_dm2conf_str_item("IS_PORT_NUMBER", is_port_number, false)) is_port_number = "";

	for (int count = 1; count < network_num + 1; count++) {
		string item_str;
		int item_int;
		if (!dm2util.chk_dm2conf_str_item("INTERFACE_NAME_" + to_string(count), item_str, false)) item_str = "";
		interface_names.push_back(item_str);

		if (!dm2util.chk_dm2conf_int_item("INTERFACE_IP_VER_" + to_string(count), item_int, true)) return false;
		interface_ip_vers.push_back(item_int);

		if (!dm2util.chk_dm2conf_str_item("UDP_PORT_NUMBER_" + to_string(count), item_str, true)) return false;
		udp_port_numbers.push_back(item_str);
		
		if (!dm2util.chk_dm2conf_int_item("SOCKET_TYPE_" + to_string(count), item_int, true)) return false;
		socket_types.push_back(item_int);
		if (item_int == 11) {
			if (!SEC::I2vSignatureProcessor::checkServerSignature(confDirPath)) {
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

	if (!dm2util.chk_dm2conf_int_item("DUAL_MAP_CLEAR_TIME", dual_map_clear_time, false)) return false;

	if (!dm2util.chk_dm2conf_int_item("MAX_SENDTO_TIMES", max_sendto_times, false)) return false;

	if (!dm2util.chk_dm2conf_int_item("CS_DUPLICATE_CHECK", cs_duplicate_check, false)) return false;
	
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
void NwRcvSettings::make_dm2conf(std::string my_sid_str, std::string nic, std::string socket_type_str)
{
	int socket_type = 0;
	std::string tmp_dm2_conf_fname = "/tmp/dm2cs_recv.conf";
	std::string tmp_dm2_conf_text;
	my_sid = 202;
	network_num = 1;
	int tmp_cs_packet_size = 1442;
	
	if (my_sid_str != "") my_sid = stoull(my_sid_str);
	interface_names.push_back(nic);
	interface_ip_vers.push_back(4);
	udp_port_numbers.push_back("55555");
	if (socket_type_str == "dtls") {
		socket_type = 2;
	}
	socket_types.push_back(socket_type);
	cs_packet_sizes.push_back(tmp_cs_packet_size);
	dual_map_clear_time = 100;
	max_sendto_times = 10;
	cs_duplicate_check = 1;
	trace_on = 0;

	ofstream dm2conf_file(tmp_dm2_conf_fname);
	tmp_dm2_conf_text = "MY_STATION_ID = " + to_string(my_sid) + "\nSEND_NETWORK_NUM = 1\nRECEIVE_NETWORK_NUM = 1\nINTERFACE_NAME_1 = " + nic 
					+ "\nINTERFACE_IP_VER_1 = 4\nUDP_PORT_NUMBER_1 = 55555\nSOCKET_TYPE_1 = " + to_string(socket_type) + "\nSEND_LIST_1 = "
					+ "\nCS_PACKET_SIZE_1 " + to_string(tmp_cs_packet_size) + "\nCOMBINATION_MAP_CLEAR_TIME = 100\nDUAL_MAP_CLEAR_TIME = 100\nMAX_SENDTO_TIMES = 10\nCS_DUPLICATE_CHECK = 1\nTRACE_ON = 0";
	dm2conf_file<<tmp_dm2_conf_text;
	dm2conf_file.close();
	std::cout << "[" << tmp_dm2_conf_fname << "]" << std::endl;
	std::cout << tmp_dm2_conf_text << std::endl;
}
/**
 * @fn	ProcRcvSettings::usage()
 * @brief	引数説明
 * @author	Shinichi Kusayama
 * @date	2024/4/17
 */
void NwRcvSettings::usage(const char *cmd)
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
		"     -s <My SID>         202(*)               \n"
		"  ------------------------------------------  \n"
		"\n",
		cmd);
}
