#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <ctime>
#include <vector>
#include <signal.h>
#include "unistd.h"
#include <is/DmManager.h>
#include <algorithm>

#include "Main.h"
#include "DMReceiver.h"

using namespace std;
DMSender *p_dms = NULL;
volatile sig_atomic_t eflag = 0;

int main(int argc, char *argv[])
{
	bool doRecv = false;
	bool isNoSpace = false;
	bool isSecureMode = false;
	bool isTcpMode = false;
	bool isOneShot = false;
	bool doCompress = false;
	bool allowDuplication = false;
	int addTimestamp = 0;
	string ip = "";
	string userid = "";
	string password = "";
	string query = "";
	string input_file = "";
	string window = "";
	string master_schema = "";
	int create_timer = 0;
	string columns = "";
	int window_rows = 1;
	string schema_name = "";
	string plus_schema_name = "";
	string paritition_keys = "";
	string where = "";
	string send_list = "";
	int delay = 0;
	int ch;
	long timeout = 0;
	double times = 1.0;
	unsigned long long destSID = 0;
	unsigned long long execSID = 0;
	unsigned int mngId = 0;
	bool doDestQuery = false;
	string minus_str = "-";
	// 引数処理(Todo: 長形式への対応｛getopt_long関数への移行｝)
	while ((ch = getopt(argc, argv, "0123456789A::B:c:C:d:D:E:f:F:i:I::m:Mno::O:p:P:r::R:sS:tT:u:w:W:zh")) != -1) {
		switch (ch) {
		case '0': minus_str += "0"; break;
		case '1': minus_str += "1"; break;
		case '2': minus_str += "2"; break;
		case '3': minus_str += "3"; break;
		case '4': minus_str += "4"; break;
		case '5': minus_str += "5"; break;
		case '6': minus_str += "6"; break;
		case '7': minus_str += "7"; break;
		case '8': minus_str += "8"; break;
		case '9': minus_str += "9"; break;
		case 'A':
			// dm2mes -r -A で記録した時のcreate_tsの位置をデフォルト値とする
			addTimestamp = -2;
			if (optind < argc) {
				string opt = argv[optind];
				if (opt.substr(0,1) != "-") {
					try {
						addTimestamp = stoi(opt);
					}
					catch (std::invalid_argument const&) {
						fprintf(stderr, "failed to parse times-value: %s\n", opt.c_str());
						usage(argv[0]);
						exit(1);
					}
					catch (std::out_of_range const&) {
						fprintf(stderr, "failed to parse times-value: %s\n", opt.c_str());
						usage(argv[0]);
						exit(1);
					}
				}
			}
			break;
		case 'B':
			plus_schema_name = optarg;
		    if (plus_schema_name == "" || plus_schema_name.substr(0,1) == "-") {
		        fprintf(stderr, "failed to parse schema_name: %s\n", optarg);
		    	usage(argv[0]);
		        exit(1);
		    }
			break;
		case 'c':
			columns = optarg;
			break;
		case 'C':
		    if (sscanf(optarg, "%d", &mngId) != 1) {
		        fprintf(stderr, "failed to parse management-ID-value: %s\n", optarg);
		    	usage(argv[0]);
		        exit(1);
		    }
			break;
		case 'd':
		    if (sscanf(optarg, "%d", &delay) != 1) {
		        fprintf(stderr, "failed to parse delay-value: %s\n", optarg);
		    	usage(argv[0]);
		        exit(1);
		    }
			break;
		case 'D':
		    if (sscanf(optarg, "%llu", &destSID) != 1) {
		        fprintf(stderr, "failed to parse destination-SID-value: %s\n", optarg);
		    	usage(argv[0]);
		        exit(1);
		    }
			doDestQuery = true;
			break;
		case 'E':
		    if (sscanf(optarg, "%llu", &execSID) != 1) {
		        fprintf(stderr, "failed to parse executer-SID-value: %s\n", optarg);
		    	usage(argv[0]);
		        exit(1);
		    }
			doDestQuery = true;
			break;
		case 'f':
			{
				string opt = optarg;
				if (opt.substr(0,1) != "-") {
					input_file = opt;
				}
			}
			if (input_file == "") {
		    	usage(argv[0]);
		        exit(1);
			}
			break;
		case 'F':
		    if (sscanf(optarg, "%lf", &times) != 1) {
		        fprintf(stderr, "failed to parse management-ID-value: %s\n", optarg);
		    	usage(argv[0]);
		        exit(1);
		    }
			break;
		case 'i':
			ip = optarg;
			break;
		case 'm':
			master_schema = optarg;
			if (master_schema == "" || master_schema.substr(0,1) == "-") {
		        fprintf(stderr, "failed to parse master_schema: %s\n", optarg);
		    	usage(argv[0]);
		        exit(1);
		    }
			break;
		case 'M':
			allowDuplication = true;
			break;
		case 'n':
			isNoSpace = true;
		    break;
		case 'o':
			if (optind < argc) {
				string opt = argv[optind];
				if (opt.substr(0,1) != "-") {
					query = opt;
				}
			}
			isOneShot = true;
			break;
		case 'O':
				isOneShot = true;
				if (sscanf(optarg, "%d", &create_timer) != 1) {
					fprintf(stderr, "failed to parse create_timer: %s\n", optarg);
					usage(argv[0]);
					exit(1);
				}
				if (create_timer <= 0) {
					cerr << "failed to set create_timer [" <<  create_timer << "] Set a value greater then zero." << endl;
					exit(1);
				}
			break;
		case 'p':
			password = optarg;
			break;
		case 'P':
			paritition_keys = optarg;
		    if (paritition_keys == "" || plus_schema_name.substr(0,1) == "-") {
		        fprintf(stderr, "failed to parse paritition_keys: %s\n", optarg);
		    	usage(argv[0]);
		        exit(1);
		    }
			break;
		case 'r':
			doRecv = true;
			if (optind < argc) {
				string opt = argv[optind];
				if (opt.substr(0,1) != "-") {
					deleteNl(opt);
					query = opt;
					transform(query.begin(), query.end(), query.begin(), [](unsigned char c) { return std::tolower(c); });
				    if (query.find("master ") == string::npos || query.find(" select ") == string::npos ||
				    		query.find(" from ") == string::npos || query.find("[") == string::npos || query.find("]") == string::npos) {
				        fprintf(stderr, "failed to parse query: %s.\n", query.c_str());
				    	usage(argv[0]);
				        exit(1);
				    }
					query = opt;
				}
			}
			break;
		case 'R':
			send_list = optarg;
		    if (send_list == "" || schema_name.substr(0,1) == "-") {
		        fprintf(stderr, "failed to parse send_list: %s\n", optarg);
		    	usage(argv[0]);
		        exit(1);
		    }
			break;
		case 's':
			isSecureMode = true;
			break;
		case 'S':
			schema_name = optarg;
		    if (schema_name == "" || schema_name.substr(0,1) == "-") {
		        fprintf(stderr, "failed to parse schema_name: %s\n", optarg);
		    	usage(argv[0]);
		        exit(1);
		    }
			break;
		case 't':
			isTcpMode = true;
			break;
		case 'T':
			if (optind <= argc) {
				string opt = argv[optind - 1];
				if (opt.substr(0,1) != "-") {
					try {
						transform(opt.begin(), opt.end(), opt.begin(), [](unsigned char c) { return std::tolower(c); });
						if (opt.find("ms") != string::npos) {
							opt = opt.replace(opt.find("ms"), 2, "");
							timeout = stoi(opt);
						} else {
							timeout = stoi(opt) * 1000;
						}
					} catch (...) {
						fprintf(stderr, "failed to parse timeout-value: %s\n", optarg);
						usage(argv[0]);
						exit(1);
					}
				}
			}
			break;
		case 'u':
			userid = optarg;
			break;
		case 'w':
			window = optarg;
			transform(window.begin(), window.end(), window.begin(), [](unsigned char c) { return std::tolower(c); });
		    if (window.find("msec") == string::npos) {
		    	int check;
			    if (sscanf(optarg, "%d", &check) != 1) {
			        fprintf(stderr, "failed to parse window-size: %s.\n", optarg);
			    	usage(argv[0]);
			        exit(1);
			    }
			    window_rows = stoi(window);
			    window = "rows " + window;
		    } else {
		    	window = "range " + window;
		    }
			break;
		case 'W':
			where = optarg;
		    if (where == "" || plus_schema_name.substr(0,1) == "-") {
		        fprintf(stderr, "failed to parse where: %s\n", optarg);
		    	usage(argv[0]);
		        exit(1);
		    }
			break;
		case 'z':
			doCompress = true;
			break;
		default:
			usage(argv[0]);
		    exit(1);
		}
	}
	if (!doDestQuery && execSID != 0) {
		fprintf(stderr, "If 'executer SID' is specified, 'destination SID' is required.\n");
		usage(argv[0]);
		exit(1);
	}
	if (send_list != "") {
		RegisterSendList(send_list, input_file);
		return 0;
	}
	if (addTimestamp != 0 && minus_str != "-") {
		addTimestamp = stoi(minus_str);
	}
	if (isSecureMode) {
		DmManager::initEncryptionSettings(DEFAULT_CA_CERT, DEFAULT_CA_KEY, DEFAULT_CA_PASS);
	}
	if (isOneShot) {
		if (query != "" || create_timer != 0) {
			if (DMOneshot(ip, userid, password, schema_name, isTcpMode, query, isSecureMode, create_timer) == false) return -1;
			return 0;
		} else if (input_file != "") {
			if (DMOneshotByFile(ip, userid, password, schema_name, isTcpMode, input_file, isSecureMode) == false) return -1;
			return 0;
		}
	}
	if (doRecv) {
		int ret;
		if (input_file != "") {
			query = getQuery(input_file);
			deleteNl(query);
		}
		if (doDestQuery) {
			ret = DMSetDestQuery(ip, userid, password, master_schema, schema_name, isTcpMode, query, window, addTimestamp, isSecureMode, destSID, execSID, columns, paritition_keys, where);
		} else {
			ret = DMRecv(ip, userid, password, master_schema, schema_name, isTcpMode, query, window, timeout, addTimestamp, delay, isSecureMode, columns, plus_schema_name, allowDuplication, paritition_keys, where);
		}
		return ret;
	}
	if (mngId != 0) {
		if (DMCancel(ip, userid, password, isTcpMode, isSecureMode, mngId)) {
			return 0;
		} else {
			return -1;
		}
	}

	DMSender dms;
	if (!isOneShot) {
		if (!dms.init(ip, userid, password, isTcpMode, isSecureMode)) return -1;
	}
	long start_ts = DmUtil::getTimeMillisec();
	std::ifstream ifscnf;
	if (input_file != "") {
		ifscnf.open(input_file.c_str());
		if (!ifscnf) {
			cerr << "Failed read file :" << input_file << endl;
			return 0;
		}
	}
	//cout << start_ts << endl;
	vector<std::string> v_line;
	while (true)
	{
		if (timeout != 0) {
			if (DmUtil::getTimeMillisec() - start_ts > timeout) break;
		}
		if (isOneShot) cout << "dm2mes=> ";
		std::string line;
		bool doGetLine = false;
		if (input_file == "") {
			doGetLine = static_cast<bool>(std::getline(std::cin, line));
		} else {
			doGetLine = static_cast<bool>(std::getline(ifscnf, line));
		}
		if (doGetLine)
		{
			if (isOneShot) {
				if (DMOneshot(ip, userid, password, schema_name, isTcpMode, line, isSecureMode, 0) == false) break;
			} else {
				bool waitFlg = true;
				if (addTimestamp == 0) {
					v_line.push_back(line);
					if ((int)v_line.size() < window_rows) continue;
				}
				while (waitFlg) {
					// 入力があった場合はDMへの送信処理を行う。
					if (dms.doSend(line, isNoSpace, addTimestamp, waitFlg, times) == false) {
						if (addTimestamp != 0) {
							v_line.push_back(line);
						}
					} else {
						if ((int)v_line.size() > 0) {
							if (dms.sendIs(schema_name, v_line, doCompress, addTimestamp, delay) == false) break;
							v_line.clear();
						}
						if (addTimestamp != 0 && waitFlg == false) {
							v_line.push_back(line);
						}
					}
				}
			}
		} else {
			break;
		}
	}
	// 同期ありの場合、時間外のまま残るケースあり
	if (addTimestamp != 0 && (int)v_line.size() > 0) {
		bool waitFlg = true;
		while (waitFlg) {
			if (dms.doSend(v_line[0], isNoSpace, addTimestamp, waitFlg, times) == false) {
				if (dms.sendIs(schema_name, v_line, doCompress, addTimestamp, delay) == false) break;
			}
		}
	}
	if (ifscnf) ifscnf.close();
	//cout << DmUtil::getTimeMillisec() << endl;

	return 0;
}
void RegisterSendList(string send_list, string input_file) {
	CS::Util util;
	CS::UdpSendInterface udpsendinterface;
	string confDirPath = "";
	string env_path = "";
	unsigned long long my_sid_ul;
	if (input_file == "") {
		// 環境変数を取得
		env_path = util.getAndCheckConfDirPath(false);
		if (env_path != "") confDirPath = env_path;
	} else {
		confDirPath = input_file;
	}
	if (confDirPath != "") {
		if (!util.chkDirPath(confDirPath)) {
			cout << "[WARN] config Directory does not exist. " << confDirPath << endl;
			return;
		}
	}
	string my_sid = util.ReadConfigPath("MY_STATION_ID", confDirPath + "/dm2.conf");
	if (my_sid == "read error") {
		cout << "[WARN] config Directory does not exist or MY_STATION_ID is missing. " << confDirPath << endl;
		return;
	}
    static std::string apl_fd = confDirPath + "/fd/CS_FD";
	
	DMSender dms;
	vector<string> v_send_list = dms.split(send_list, ',');
	CS::send_message_mng sm;
	memset(&sm, 0, sizeof (CS::send_message_mng));
	try {
		my_sid_ul = stoull(my_sid);
		sm.sid = stoull(v_send_list[0]);
		strncpy(sm.ip, v_send_list[1].c_str(), sizeof (sm.ip) - 1);
		if (v_send_list.size() > 2) {
			memcpy(sm.ctl_flag, v_send_list[2].c_str(), sizeof (sm.ctl_flag) - 1);
		}
	} catch (...) {
		cerr << "引数のパースエラー: " + send_list << endl;
		return;
	}
	udpsendinterface.MngConnectionInfoSendtoCs(apl_fd.c_str(), my_sid_ul, my_sid_ul, sm);

}
bool DMRecv(string ip, string userid, string password, string master_schema, string schema_name,
		bool isTransportMode, string query, string window, long timeout, int addTimestamp, int delay, bool isSecureMode, 
		string columns, string plus_schema_name, bool allowDuplication, string paritition_keys, string where) {
	if (signal(SIGINT, handler) == SIG_ERR) {
		cerr << "Signal Error" << endl;
		return -1;
	}
	DMReceiver dmr;
	if (!dmr.DMConnect(ip, userid, password, isTransportMode, isSecureMode)) return -1;
	
	if (!dmr.continuousQuery(master_schema, schema_name, query, window, addTimestamp, columns, plus_schema_name, allowDuplication, paritition_keys, where)) {
		dmr.DMDisconnect();
		return -1;
	}
	long start_ts = DmUtil::getTimeMillisec();
	while (true) {
		if (eflag == 1) break;
		if (timeout != 0) {
			if (DmUtil::getTimeMillisec() - start_ts > timeout) break;
		}
		usleep(1000 * delay);
	}
	try {
		dmr.DMReconnect();
		dmr.cancelQueryForStream();
		dmr.DMDisconnect();
	} catch (...) {
		cerr << "DMとの接続に失敗しました" << endl;
		return -1;
	}

	return 0;
}
bool DMSetDestQuery(string ip, string userid, string password, string master_schema, string schema_name,
		bool isTransportMode, string query, string window, int addTimestamp, bool isSecureMode, 
		unsigned long long destSID, unsigned long long execSID, string columns, string paritition_keys, string where) {
	DMReceiver dmr;
	if (!dmr.DMConnect(ip, userid, password, isTransportMode, isSecureMode)) return -1;

	if (!dmr.destQuery(master_schema, schema_name, query, window, destSID, execSID, columns, paritition_keys, where)) return -1;
	dmr.DMDisconnect();
	return 0;
}
bool DMOneshot(string ip, string userid, string password, string schema_name, bool isTransportMode, string query, bool isSecureMode, int create_timer) {
	string _query = query;
	string _userid = userid;
	if (create_timer != 0) {
		if (_userid == "") _userid = DEFAULT_DM_USER;
		_query = "CREATE STREAM " + _userid + "Timer" + to_string(create_timer) + "msec (time long) TIMER_FIX [" + to_string(create_timer) + "]";
		cout << _query << endl;
	}
	// 小文字化して、クエリチェック
	{
		string _qtmp = _query;
		transform(_qtmp.begin(), _qtmp.end(), _qtmp.begin(), [](unsigned char c) { return std::tolower(c); });
		if (_qtmp.find("create ") == string::npos && _qtmp.find("drop ") == string::npos
				&& _qtmp.find("insert ") == string::npos && _qtmp.find("update ") == string::npos
				&& _qtmp.find("delete ") == string::npos && _qtmp.find("select ") == string::npos) {
			if (_qtmp.find("quit") != string::npos || _qtmp.find("exit") != string::npos) {
				return false;
			}
			cerr << "Query is wrong." << endl;
			return true;
		}
	}
	DMReceiver dmr;
	if (!dmr.DMConnect(ip, userid, password, isTransportMode, isSecureMode)) return false;
	try {
		if (dmr.oneShotQueryExec(_query, true) == false) return false;
		dmr.DMDisconnect();
	} catch (...) {
		cerr << "DMとの接続に失敗しました" << endl;
		return false;
	}
	return true;
}
// 連続実行用
bool DMOneshotByFile(string ip, string userid, string password, string schema_name, bool isTransportMode, string query_file, bool isSecureMode) {
	DMReceiver dmr;
	DMSender dms;
	if (!dmr.DMConnect(ip, userid, password, isTransportMode, isSecureMode)) return false;
	string all_query = getQuery(query_file);
	deleteNl(all_query);
	vector<string> query_list = dms.split(all_query, ';');
	for (unsigned int i = 0; i < query_list.size(); i++) {
		string _query = query_list[i];
		transform(_query.begin(), _query.end(), _query.begin(), [](unsigned char c) { return std::tolower(c); });
		if (_query.find("create ") == string::npos && _query.find("drop ") == string::npos
				&& _query.find("insert ") == string::npos && _query.find("update ") == string::npos
				&& _query.find("delete ") == string::npos && _query.find("select ") == string::npos) {
			continue;
		}
		cerr << "[" << i << "] " << query_list[i] << endl; 
		if (dmr.oneShotQueryExec(_query, false) == false) return false;
	}
	try {
		dmr.DMDisconnect();
	} catch (...) {
		cerr << "DMとの接続に失敗しました" << endl;
		return false;
	}
	return true;
}
bool DMCancel(string ip, string userid, string password, bool isTransportMode, bool isSecureMode, unsigned int mngId) {
	DMReceiver dmr;
	if (!dmr.DMConnect(ip, userid, password, isTransportMode, isSecureMode)) return false;

	if (!dmr.cancelQuery(mngId)) return false;
	dmr.DMDisconnect();
	return true;
}

string getQuery(string fname) {
	ifstream ifs(fname.c_str());
	if (ifs.fail()) {
		cerr << "Query File Read Error:" << fname << endl;
		return "";
	}
	istreambuf_iterator<char> it(ifs);
	istreambuf_iterator<char> last;
	string query_str(it, last);
	return query_str;
}
void deleteNl(std::string &targetStr)
{
	const char CR = '\r';
	const char LF = '\n';
	std::string destStr;
	for (const auto c : targetStr) {
		if (c != CR && c != LF) {
			destStr += c;
		} else {
			destStr += ' ';
		}
	}
	targetStr = std::move(destStr);
}
void handler(int signum) {
  eflag = 1;
}

static void usage(const char *cmd)
{
	printf("Usage: %s [options] \n"
		"\n"
		"Options: [] => Required arg. {} => Any arg. \n"
		"  -A {timestamp-column-idx} [Recveive Mode] Add Create-Ts And Recv-Ts in Last 2-Columns.  \n"
		"                            [Send Mode]     Sync with Timestamp. \n"
		"                                            timestamp-column-idx: Starting From 1. Minus OK \n"
		"                                             ex.) column-name (id,time, ... ,create_ts,recv_ts) \n"
		"                                               1:id, 2:time, ... , -2:create_ts, -1:recv_ts \n"
		"                                               Default: -2:create_ts \n"
		"  -B <schema>               Both-mode (receive-mode and send-mode). \n"
		"                            ex.) dm2mes -r -S original_schema -w 100 -B copy_schema \n"
		"                              Similar to \"dm2mes -r -S original_schema -w 100 | dm2mes -S copy_schema\". \n"
		"                              But -P method is parallelized within the receive.\n"
		"  -c <columns>              Set columns-statement After SELECT-clause. ex.) id,time \n"
		"                            When you want to use a User-Function, escape the parentheses. ex.) UF_ABS\\(1\\). \n"
		"  -C <Manegement ID>        Cancel Query For Manegement ID. \n"
		"  -d <delay[msec]>          Set Delay for sorting. default: 0 msec \n"
		"  -D <destination SID>      For switch from -r {query} to -r {query with Destination} \n"
		"  -E <executer SID>         Specifies the SID from which the query will be executed. -D option must be specified.\n"
		"  -f <input_file>           [One-shot-query Mode]\n"
		"                                input_file example) \n"
		"                                  select * from table1;\n"
		"                                  select * from table2;\n"
		"                            [Continuous-Query Mode]\n"
		"                                input_file example) \n"
		"                                  master object_info select * from object_info [rows 1];\n"
		"                            [Send Mode]\n"
		"                                example) dm2mes -S object_info -w 50 -d 100 -f object_info.csv\n"
		"                            [Manager Mode]\n"
		"                                Set Config Directory Path.\n"
		"                                [Another Way] export DM2_CONF_DIR_PATH=/home/dm2/dm2/conf\n"
		"  -F <faster_times>         -A Mode Only. faster_times: ex) 2 => Time passes 2 times faster.\n"
		"  -i <ip>                   Set IP Address. default: %s \n"
		"  -m <event>                Set Receive Mode MASTER Schema. default: message_info \n"
		"  -M                        [Receive Mode] Set 'DUPLICATION' clause last of default query\n"
		"  -n                        Space is not Send. \n"
		"  -o {query}                One-shot-query Mode. If set query, executed once only. \n"
		"  -O {timer}                Set Timer By One-shot-query. \n"
		"                              ex.) dm2mes -O 100 => dm2mes -o \"CREATE STREAM %sTimer100msec (time long) TIMER_FIX [100]\" \n"
		"  -p <password>             Set Password. \n"
		"  -P <partition-keys>       Partition-By. \n"
		"                            ex.) dm2mes -r -S signal_info -P crp_id,id \n"
		"                              => dm2mes -r \"master signal_info select * from signal_info [partition by crp_id,id rows 1]\" \n"
		"  -r {query}                Receive Mode. default: Send Mode \n"
		"                            Free to set Full-Query.  \n"
		"                              default: MASTER message_info SELECT * \n"
		"                                       FROM message_info [rows 1] \n"
		"                            Note: If only change window-size, use '-w'.  \n"
		"                                  If only change schema only, use '-S'.  \n"
		"  -R {send_list}            SID,IP-Address(,delete) \n"
		"  -S <schema>               Set Schema Name. default: message_info \n"
		"  -s                        Secure Mode [DTLS/TLS] \n"
		"  -t                        Using TCP Session. \n"
		"  -T <Timeout>              Set Timeout. default: dm2mes -r -T 1   => 1 second after, exit\n"
		"                                              ms: dm2mes -r -T 1ms => 1 milli second after, exit\n"
		"  -u <userid>               Set User Id. default: %s \n"
		"  -w <window size>          [Receive Mode] \n"
		"                              default:          rows 1 \n"
		"                                ex1) 2        => rows 2 \n"
		"                                ex2) \"100 msec\" => range 100 msec \n"
		"                            [Send Mode] \n"
		"                              default: 1 \n"
		"                                ex) 3    => 3-Tuples in TupleSet is send \n"
		"  -z                        Zstd-compressed-Data is send. \n"
		"  -h                        Show help. \n"
		"\n"
		"Example 1: Hello, dm2 \n"
		"  $ dm2mes -r                         <== [Receive message_info] \n"
		"  $ echo hello,dm2 | dm2mes           <== [Send message_info] \n"
		"Example 2: object_info Save and Replay \n"
		"  $ dm2mes -r -S object_info -w 50       <== [Receive object_info {max 50 rows / 1 Received} ] \n"
		"  $ dm2mes -f object_info -w 50 -d 100   <== [Send object_info {max 50 rows / 1 Send} delay 100 msec ] \n"
		"\n",
		cmd, DEFAULT_DM_IP, DEFAULT_DM_USER, DEFAULT_DM_USER);
}
