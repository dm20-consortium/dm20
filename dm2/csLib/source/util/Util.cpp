#include "Cs.h"
#include <sys/types.h>
#include <sys/stat.h>

namespace CS{
	std::string Util::PrintSend_message(send_message &sm){
		std::string res_str = "";
		res_str = "src_station_id:" + std::to_string(sm.src_station_id) 
		        + ", dst_station_id:" + std::to_string(sm.dst_station_id)
				+ ", src_station_type:" + std::to_string(sm.src_station_type) 
				+ ", dst_station_type:" + std::to_string(sm.dst_station_type)
				+ ", transmission_flag:" + std::to_string(sm.transmission_flag) 
				+ ", duplication_check_id:" + std::to_string(sm.duplication_check_id) 
				+ ", lane_id:" + std::to_string(sm.lane_id) 
				+ ", retry_level:" + std::to_string(sm.retry_level) 
				+ ", retry_data_id:" + std::to_string(sm.retry_data_id) 
				+ ", retry_lifetime:" + std::to_string(sm.retry_lifetime) 
				+ ", msg_type:" + std::to_string(sm.msg_type) 
				+ ", cs_message_detail:" + std::to_string(sm.cs_message_detail) 
				+ ", fd_name:" + std::string(sm.fd_name, 40) 
				+ ", flagment_duplication_check_id:" + std::to_string(sm.flagment_duplication_check_id) 
				+ ", flagment_sum" + std::to_string(sm.flagment_sum) 
				+ ", flagment_offset:" + std::to_string(sm.flagment_offset) 
				+ ", priority_level:" + std::to_string(sm.priority_level);
		#if LID_PRIORITY == 1
		res_str = res_str + ", policing_time:" + std::to_string(sm.policing_time); 
		#endif
		res_str = res_str + ", dm2_payload:" + std::string(sm.dm2_payload, MSGSIZE); 
		return res_str;
	}
	std::string Util::PrintClient_data(clientdata &cd){
		std::string res_str = "";
		res_str = PrintSend_message(cd.msg);
		std::string from_ip = cd.from_ip;
		res_str += ",w_cdata.from_ip:" + from_ip;
		return res_str;
	}

	/**
	 * @fn	std::string Util::ReadConfigPath(const char* key, const std::string &pathname_str)
	 *
	 * @brief	コンフィグファイルからパラーメータ値を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	key			検索キー
	 * @param	filename	configファイル名
	 *
	 * @return	検索キーに対する値
	 */
	std::string Util::ReadConfigPath(const char* key, const std::string &pathname_str){
		std::ifstream ifscnf(pathname_str.c_str());
		if(!ifscnf){
			std::cout << pathname_str << "ファイル読み込みエラー" << std::endl;
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "ReadConfigPath fail." << std::endl;
			perror("ReadConfigPath");
			return "read error";
		}
		std::string strcnf;
		while(getline(ifscnf, strcnf)){
			//読み込んだ行のスペースを全て削除
			strcnf.erase(remove(strcnf.begin(), strcnf.end(),' '), strcnf.end());
			//先頭に#がある場合は読み飛ばす
			if(strcnf.substr(0,0) == "#"){
				continue;
			}
			if(strcnf.find(key) == 0){
				//keyにヒットする文字列がある行の途中に#がある場合はそれ以降の文字列を全て削除
				if(strcnf.find("#") != std::string::npos){
					strcnf.erase(strcnf.find("#"));
				}
				//Trim処理をして=以降の文字列をリターン
				return Util::Trim(strcnf.substr(strcnf.find('=')+1));
			}
		}
		return "ReadConfigFile ERROR";
	}
	/**
	 * @fn	std::string Util::ReadConfigFile(const char* key, const std::string &filename_str)
	 *
	 * @brief	コンフィグファイルからパラーメータ値を取得する（CS・IS用。引数のパスに予めセットされたパスと繋げて使う）
	 *
	 * @author	Shinichi Kusayama
	 * @date	2024/4/19
	 *
	 * @param	key			検索キー
	 * @param	filename	configファイル名
	 *
	 * @return	検索キーに対する値
	 */
	std::string Util::ReadConfigFile(const char* key, const std::string &filename_str){
		const char *filename = filename_str.c_str();
		std::string pathname = this->_dirname + std::string(filename);
		return ReadConfigPath(key, pathname);
	}
	/**
	 * @fn	std::string Util::ReadAplConfigFile(const char* key, const std::string &filename_str)
	 *
	 * @brief	コンフィグファイルからパラーメータ値を取得する（アプリ用。引数のパスをそのまま使う）
	 *
	 * @author	Shinichi Kusayama
	 * @date	2024/4/19
	 *
	 * @param	key			検索キー
	 * @param	filename	configファイル名
	 *
	 * @return	検索キーに対する値
	 */
	std::string Util::ReadAplConfigFile(const char* key, const std::string &filename_str){
		return ReadConfigPath(key, filename_str);
	}
	/**
	 * @fn	bool Util::chk_dm2conf_int_item(std::string item, int &value, bool doWarnLog)
	 *
	 * @brief	dm2.confファイルからInt項目を読み取る。
	 *
	 * @author	Shinichi Kusayama
	 * @date	2023/08/29
	 *
	 * @param	item		検索キー
	 * @param	value		読み取った値
	 * @param	doWarnLog	WARNログ出力するか
	 *
	 * @return	読み取り成功/失敗
	 */
	bool Util::chk_dm2conf_int_item(std::string item, int &value, bool doWarnLog)
	{
		std::string str_value;
		if (!chk_dm2conf_str_item(item, str_value, doWarnLog)) return false;
		try
		{
			value = stoi(str_value);
		}
		catch(...) {
			std::cerr << item << ": 数値変換エラー value=" << str_value << std::endl;;
			return false;
		}
		return true;
	}
	/**
	 * @fn	bool Util::chk_dm2conf_long_item(std::string item, long &value, bool doWarnLog)
	 *
	 * @brief	dm2.confファイルからlong項目を読み取る。
	 *
	 * @author	Shinichi Kusayama
	 * @date	2023/08/29
	 *
	 * @param	item		検索キー
	 * @param	value		読み取った値
	 * @param	doWarnLog	WARNログ出力するか
	 *
	 * @return	読み取り成功/失敗
	 */
	bool Util::chk_dm2conf_long_item(std::string item, long &value, bool doWarnLog)
	{
		std::string str_value;
		if (!chk_dm2conf_str_item(item, str_value, doWarnLog)) return false;
		try
		{
			value = stol(str_value);
		}
		catch(...) {
			std::cerr << item << ": 数値変換エラー value=" << str_value << std::endl;
			return false;
		}
		return true;
	}
	/**
	 * @fn	bool Util::chk_dm2conf_unsigned_long_long_item(std::string item, long &value, bool doWarnLog)
	 *
	 * @brief	dm2.confファイルからunsigned long long項目を読み取る。
	 *
	 * @author	Ryosuke Takeuchi
	 * @date	2023/10/25
	 *
	 * @param	item		検索キー
	 * @param	value		読み取った値
	 * @param	doWarnLog	WARNログ出力するか
	 *
	 * @return	読み取り成功/失敗
	 */
	bool Util::chk_dm2conf_unsigned_long_long_item(std::string item, unsigned long long &value, bool doWarnLog)
	{
		std::string str_value;
		if (!chk_dm2conf_str_item(item, str_value, doWarnLog)) return false;
		try
		{
			value = stoull(str_value);
		}
		catch(...) {
			std::cerr <<  item << ": 数値変換エラー value=" << str_value << std::endl;
			return false;
		}
		return true;
	}

	/**
	 * @fn	bool Util::chk_dm2conf_str_item(std::string item, int &value, bool doWarnLog)
	 *
	 * @brief	dm2.confファイルから文字列項目を読み取る。
	 *
	 * @author	Shinichi Kusayama
	 * @date	2023/08/29
	 *
	 * @param	item		検索キー
	 * @param	value		読み取った値
	 * @param	doWarnLog	WARNログ出力するか
	 *
	 * @return	読み取り成功/失敗
	 */
	bool Util::chk_dm2conf_str_item(std::string item, std::string &value, bool doWarnLog)
	{
		value =  ReadConfigFile(item.c_str(), CONF_FILE);
		if((value.find("error") != std::string::npos) || (value.find("ERROR") != std::string::npos) || value == "" || value == "\"\"") {
			if (doWarnLog) {
				std::cerr << item << " 読み込みエラー" << std::endl;
			}
			return false;
		}
		std::string log = "[dm2.conf]" + item + ": " + value;
		if (item.find("SOCKET_TYPE_") != std::string::npos) {
			if (socket_type_map.find(value) != socket_type_map.end()) {
				int value_map = socket_type_map[value];
				value = std::to_string(value_map);
			}
		}
		if (doWarnLog) {
			std::cout << log << std::endl;
		}

		return true;
	}
	/**
	 * @fn	std::string Util::getConfDirPath()
	 *
	 * @brief	DM2.0用の環境変数を取得する
	 *
	 * @author	Shinichi Kusayama
	 * @date	2024/4/15
	 *
	 *
	 * @return	環境変数の値
	 */
	std::string Util::getConfDirPath(){
		const char *env_dir_tmp = getenv(ENV_DM2_CONF_PATH);
		std::string env_dir_path(env_dir_tmp ? env_dir_tmp : "");
		if (env_dir_path.empty()) {
			return "";
		}
		return env_dir_path;
	}
	/**
	 * @fn	bool Util::chkDirPath(const std::string &dirpath)
	 *
	 * @brief	ディレクトリの存在確認
	 *
	 * @author	Shinichi Kusayama
	 * @date	2024/4/15
	 *
	 *
	 * @return	環境変数の値
	 */
	bool Util::chkDirPath(const std::string &dirpath){
		struct stat s;
		if (stat(dirpath.c_str(), &s) == 0) {
			if (s.st_mode & S_IFDIR) return true;
		}
		return false;
	}
	/**
	 * @fn	std::string Util::getAndCheckConfDirPath()
	 *
	 * @brief	DM2.0用の環境変数を取得し、未設定の場合は警告して終了する
	 *
	 * @author	Shinichi Kusayama
	 * @date	2024/4/15
	 *
	 *
	 * @return	環境変数の値
	 */
	std::string Util::getAndCheckConfDirPath(){
		return getAndCheckConfDirPath(true);
	}
	/**
	 * @fn	std::string Util::getAndCheckConfDirPath()
	 *
	 * @brief	DM2.0用の環境変数を取得し、未設定の場合は警告する。終了するかは引数のフラグによる。
	 *
	 * @author	Shinichi Kusayama
	 * @date	2024/4/15
	 *
	 *
	 * @return	環境変数の値
	 */
	std::string Util::getAndCheckConfDirPath(bool exitFlg){
		std::string env_dir_path = getConfDirPath();
		if (env_dir_path != "") return env_dir_path;
		if (doExecInUsrLocalBin()) {
			if (exitFlg) {
				// 環境変数が未設定で、/usr/local/binから実行している場合は、エラーとする。
				std::cerr << "[ERROR] set DM2_CONF_DIR_PATH. "<< std::endl;
				std::cerr << "   ex.1) export DM2_CONF_DIR_PATH=/home/dm2/dm2/conf" << std::endl;
				std::cerr << "   ex.2) " << program_invocation_short_name   << " -d /home/dm2/dm2/conf" << std::endl;
				exit(1);
			}
			return "";
		} else {
			// settings.getConfigDirectory()の値（相対パス）に従う。
			std::cout << "[INFO] DM2_CONF_DIR_PATH = " + getConfigDirectory() << std::endl;
		}
		return env_dir_path;
	}
	/**
	 * @fn	bool Util::doExecInUsrLocalBin()
	 *
	 * @brief	実行パスが/usr/local/binかチェックする
	 *
	 * @author	Shinichi Kusayama
	 * @date	2024/4/15
	 *
	 *
	 * @return	/usr/local/binで実行しているか
	 */
	bool Util::doExecInUsrLocalBin(){
		char path[PATH_MAX + 1];
		const size_t LINKSIZE = 100;
		char link[LINKSIZE];
		snprintf(link, LINKSIZE, "/proc/%d/exe", getpid() );
		if (readlink( link, path, PATH_MAX) < 0) return 1;
		std::string path_str = std::string(path);
		std::string check_path = "/usr/local/bin";
		std::string path_str2 = path_str.substr(0, check_path.length());
		if (path_str2 == check_path) return true;
		return false;
	}
	/**
	* 設定ファイルの格納場所をセットする
	*
	* @author	Shinichi Kusayama
	* @date	2024/4/16
	*
	* @return	dirname			ディレクトリ名
	*
	*/
	std::string Util::getConfigDirectory() {
		return this->_dirname;
	}
	/**
	* 設定ファイルの格納場所をセットする
	*
	* @author	Shinichi Kusayama
	* @date	2024/4/16
	*
	* @param [in]	dirname			ディレクトリ名
	*
	*/
	void Util::setConfigDirectory(const std::string &dirname) {
		std::string suffix = "";
		if (dirname.substr(dirname.length() - 1, 1) != "/") suffix = "/";
		this->_dirname = dirname + suffix;
	}

	/**
	 * @fn	std::string Util::Trim(std::string inputstr)
	 *
	 * @brief	文字列からスペース、キャリッジリターン、タブを削除(トリム処理)
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	inputstr	変換対象文字列
	 *
	 * @return	トリムされた文字列
	 */
	std::string Util::Trim(std::string inputstr)
	{
		std::string result;
		std::string::size_type left = inputstr.find_first_not_of(" \t\v\r\n");
		if(left != std::string::npos){
			std::string::size_type right = inputstr.find_last_not_of(" \t\v\r\n");
			result = inputstr.substr(left, right - left + 1);
		}
		return result;
	}

	std::string Util::Split(std::string inputstr, int get_number)
	{
		char delimiter = ',';
		unsigned int first = 0;
		unsigned int last = inputstr.find_first_of(delimiter);
		std::string returnstr;
		int target = 1;

		while(first < inputstr.size()){
			std::string outputstr(inputstr, first, last - first);
			first = last + 1;
			last = inputstr.find_first_of(delimiter, first);
			if(last == std::string::npos){
				last = inputstr.size();
			}
			if(target >= get_number){
				returnstr = outputstr;
				break;
			}
			target++;
		}
		return returnstr;
	}

	std::string Util::SplitSpace(std::string inputstr, int get_number)
	{
		char delimiter = ' ';
		unsigned int first = 0;
		unsigned int last = inputstr.find_first_of(delimiter);
		std::string returnstr;
		int target = 1;

		while(first < inputstr.size()){
			std::string outputstr(inputstr, first, last - first);
			first = last + 1;
			last = inputstr.find_first_of(delimiter, first);
			if(last == std::string::npos){
				last = inputstr.size();
			}
			if(target >= get_number){
				returnstr = outputstr;
				break;
			}
			target++;
		}
		return returnstr;
	}

	float Util::GetRadwinStatus(std::string radwin_vehicle_antena_ip_address){
		
		FILE *fp;
		std::string snmp_get_res;
		std::string hex_rx_chain1;
		std::string hex_rx_chain2;
		std::string hex_rx_chain3;
		int chain1;
		int chain2;
		int chain3;
		int max_chain_rx = 0;
		std::string command_str = "snmpget -v 1 -c public " + radwin_vehicle_antena_ip_address + " .1.3.6.1.4.1.4458.1000.1.5.9.6.0";
		char const *cmdline = command_str.c_str();
		// char const *cmdline = "snmpget -v 1 -c public 192.168.0.110 .1.3.6.1.4.1.4458.1000.1.5.9.6.0";
		if((fp = popen(cmdline, "r")) == NULL){
			exit(EXIT_FAILURE);
		}
		
		__gnu_cxx::stdio_filebuf<char> *p_fb = new __gnu_cxx::stdio_filebuf<char>(fp, std::ios_base::in);
		std::istream input(static_cast<std::streambuf *>(p_fb));

		while(getline(input, snmp_get_res)){
			snmp_get_res = snmp_get_res.substr(snmp_get_res.find(": ") + 2);
			//3つのアンテナのうち最大値を取得
			hex_rx_chain1 = Util::SplitSpace(snmp_get_res, 1);
			hex_rx_chain2 = Util::SplitSpace(snmp_get_res, 2);
			hex_rx_chain3 = Util::SplitSpace(snmp_get_res, 3);
			std::cout << "chain1 = " <<  hex_rx_chain1 << ", chain2 = " <<  hex_rx_chain2 << ", chain3 = " <<  hex_rx_chain3 << std::endl;
			chain1 = std::stoi(hex_rx_chain1, nullptr, 16);
			chain2 = std::stoi(hex_rx_chain2, nullptr, 16);
			chain3 = std::stoi(hex_rx_chain3, nullptr, 16);
			std::cout << "chain1 = " <<  chain1 << ", chain2 = " <<  chain2 << ", chain3 = " <<  chain3 << std::endl;
			max_chain_rx = chain1;
			if( chain2 > max_chain_rx)
				max_chain_rx = chain2;
			if( chain3 > max_chain_rx)
				max_chain_rx = chain3;
			std::cout << "max_chain_rx = " << max_chain_rx << std::endl;
			
		}
		delete p_fb;
		pclose(fp);

		std::cout << "max_chain_rx = " << max_chain_rx << std::endl;
		return max_chain_rx;
	}

	_iwconfig_status Util::GetWifiStatus(std::string _if_name){
		struct _iwconfig_status _res = {0};
		struct iwreq iwr;
		struct iw_statistics stat;
		struct iw_range range;
		char essid[IW_ESSID_MAX_SIZE + 1];

		memset(&iwr, 0, sizeof(iwr));
		memset(&stat, 0, sizeof(stat));
		memset(&range, 0, sizeof(range));
		memset(essid, '\0', sizeof(essid));

		int sk = socket(AF_INET, SOCK_DGRAM, 0);
		strncpy(iwr.ifr_name, _if_name.c_str(), IFNAMSIZ);

		if(ioctl(sk, SIOCGIWAP, &iwr) < 0){
			close(sk);
			return _res;
		}
		const struct ether_addr *ether_wap = (const struct ether_addr *)iwr.u.ap_addr.sa_data;

		sprintf(_res.mac_address, "%02X:%02X:%02X:%02X:%02X:%02X",
		ether_wap->ether_addr_octet[0],
		ether_wap->ether_addr_octet[1],
		ether_wap->ether_addr_octet[2],
		ether_wap->ether_addr_octet[3],
		ether_wap->ether_addr_octet[4],
		ether_wap->ether_addr_octet[5]);
		std::cout << "AP address: " << _res.mac_address << std::endl;


		iwr.u.essid.pointer = (caddr_t) _res.essid;
		iwr.u.essid.length = IW_ESSID_MAX_SIZE + 1;
		iwr.u.essid.flags = 0;
		if(ioctl(sk, SIOCGIWESSID, &iwr) < 0){
			std::cout << "ioctl(sk, SIOCGIWESSID, &iwr) < 0" << std::endl;
			close(sk);
			return _res;
		}
		std::cout << "ESSID:" << _res.essid << std::endl;

		iwr.u.data.pointer = &stat;
		iwr.u.data.length = sizeof(stat);
		if(ioctl(sk, SIOCGIWSTATS, &iwr) < 0){
			std::cout << "ioctl(sk, SIOCGIWSTATS, &iwr) < 0" << std::endl;
			close(sk);
			return _res;
		}
		_res.qual = (int)stat.qual.qual;
		std::cout << "Link quality: " << _res.qual << std::endl;

		iwr.u.data.pointer = &range;
		iwr.u.data.length = sizeof(range);
		if(ioctl(sk, SIOCGIWRANGE, &iwr) < 0){
			std::cout << "ioctl(sk, SIOCGIWRANGE, &iwr) < 0" << std::endl;
			close(sk);
			return _res;
		}
		_res.max_qual = (int)range.max_qual.qual;
		std::cout << "Link Max quality: " << _res.max_qual << std::endl;

		close(sk);
		return _res;

	}

	std::vector<lid_wave_quality> Util::GetLidWaveQualityVector(std::string dm2_payload_str)
	{
		struct lid_wave_quality st_lwq_;
		std::vector<lid_wave_quality> vec_lwq_;
		int comma_num = std::count(begin(dm2_payload_str), end(dm2_payload_str), ',');
		for(int i = 2; i <= (comma_num + 1); i++){
			if(i % 2 == 0){
				st_lwq_.lid = stoi(Util::Split(dm2_payload_str, i));
			}else{
				st_lwq_.wave_quality = stof(Util::Split(dm2_payload_str, i));
				vec_lwq_.push_back(st_lwq_);
			}
		}
		return vec_lwq_;
	}

	int Util::MakeSharedId(const std::string file_path, int ipc_key_id, int shared_memory_size)
	{
		// 空のファイル作成
		FILE *fp;
		fp = fopen(file_path.c_str(), "w");
		fclose(fp);

		// IPC keyの取得
		const key_t key = ftok(file_path.c_str(), ipc_key_id);
		if(key == -1){
			std::cerr << "Failed to acquire key" << std::endl; 
			exit(EXIT_FAILURE); 
		}
		// 共有メモリIDの取得
		const int seg_id = shmget(key, shared_memory_size, 
								//   IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
								IPC_CREAT | S_IRUSR | S_IWUSR);
		if(seg_id == -1){
			std::cerr << "Failed to acquire segment" << std::endl;
			exit(EXIT_FAILURE);
		}
		return seg_id;	
	}

	int Util::GetSharedId(const std::string file_path, int ipc_key_id, int get_size)
	{
	    // 作成済みの共有メモリのIDを取得する
		const key_t key = ftok(file_path.c_str(), ipc_key_id);
		const int seg_id = shmget(key, get_size, 0);
		if(seg_id == -1){
			std::cerr << "Failed to acquire segment" << std::endl;
			// exit(EXIT_FAILURE);
		}	
		return seg_id;	
	}
	/**
	* @fn	Util::usage()
	* @brief	引数説明
	* @author	Shinichi Kusayama
	* @date	2024/4/17
	*/
	void Util::usage(const char *cmd)
	{
		printf("Usage: %s [options] \n"
			"\n"
			"Options: [] => Required arg. {} => Any arg. \n"
			"  -d <directory path>       Set Config Directory Path. \n"
			"  -h                        Show help. \n"
			"\n",
			cmd);
	}


}