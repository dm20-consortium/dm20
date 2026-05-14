#ifndef UTIL_H
#define UTIL_H

#include "Cs.h"
#define ENV_DM2_CONF_PATH "DM2_CONF_DIR_PATH"

namespace CS{
	/**
	 * @class	Util
	 *
	 * @brief	DM2.0CSユーティリティクラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	// template <typename T1, typename T2>
	class Util
	{
	private:
		std::string _dirname = "../../conf/";
		std::map<std::string, int> socket_type_map = {
			{"udp", 0},
			{"udp_multicast", 1},
			{"dtls", 2},
			{"udp_etm", 10},
			{"udp_pki", 11}
		};
	public:
		std::string PrintSend_message(send_message &sm);
		std::string PrintClient_data(clientdata &cd);

		bool chk_dm2conf_int_item(std::string item, int &value, bool doWarnLog);
		bool chk_dm2conf_long_item(std::string item, long &value, bool doWarnLog);
		bool chk_dm2conf_unsigned_long_long_item(std::string item, unsigned long long &value, bool doWarnLog);
		bool chk_dm2conf_str_item(std::string item, std::string &value, bool doWarnLog);

		std::string getConfDirPath();
		bool chkDirPath(const std::string &dirpath);
		std::string getAndCheckConfDirPath();
		std::string getAndCheckConfDirPath(bool exitFlg);
		bool doExecInUsrLocalBin();
		std::string getConfigDirectory();
		void setConfigDirectory(const std::string &dirname);
		
		//コンフィグファイルからパラーメータ値を取得する
		std::string ReadConfigPath(const char* key, const std::string &filename);
		std::string ReadConfigFile(const char* key, const std::string &filename);
		std::string ReadAplConfigFile(const char* key, const std::string &filename);
		
		std::string Trim(std::string inputstr);

		std::string Split(std::string inputstr, int get_number);

		std::string SplitSpace(std::string inputstr, int get_number);

		float GetRadwinStatus(std::string radwin_vehicle_antena_ip_address);
		
		_iwconfig_status GetWifiStatus(std::string _if_name);

		std::vector<lid_wave_quality> GetLidWaveQualityVector(std::string dm2_payload_str);

		int MakeSharedId(const std::string file_path, int ipc_key_id, int shared_memory_size);
 		int GetSharedId(const std::string file_path, int ipc_key_id, int get_size);
		void usage(const char *cmd);
	};
}
#endif
