#ifndef SETTINGS_H
#define SETTINGS_H

#include "UdpSendInterface.h"
#include "is/DmUtil.h"
#include "is/StringUtil.h"

#include <string>
#include <vector>
#include <log4cxx/logger.h>
#include <pqxx/pqxx>

using namespace std;
using namespace log4cxx;
using namespace CS;
using namespace pqxx;

#define IPv4_UDP_MAX_BYTE 65000
#define IPv4_DTLS_MAX_BYTE (1<<14)


namespace IS {

	/**
	 * 設定ファイルから設定値を読み込む処理を扱うクラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	class Settings
	{
	private:
		Settings() = default;
		~Settings() = default;
		StringUtil stringUtil;

		std::unordered_map<string, string> settingMap;

		LoggerPtr logger = Logger::getLogger("Settings");
		string _dirname = "../../conf/";
	public:
		// _dirname (setConfigDirectory) からの相対パスになるファイル・ディレクトリ Start
		const string CONF_FILE_CS = "dm2.conf";
		const string CONF_FILE_SYS = "is_conf/system.conf";
		const string CONF_FILE_DB = "is_conf/RDB.conf";
		const string CONF_FILE_EVAL = "is_conf/evalFunction.conf";
		const string CONF_FILE_LOG = "log4cxx/dm2is.properties";
		const string CONF_FILE_CS_MAP = "cs_map/edge_lid_edge_sid.csv";
		const string SQL_FILE_PATH = "is_conf/sql/";
		const string CONF_DIR_SCHEMA = "schema/";
		const string CONF_DIR_PRESET_TIMER = "presetTimer/";
		const string CONF_DIR_PRESET_QUERY = "presetQuery/";
		const string EVAL_LIB_DIR = "is_func/";
		const string CONF_YAML_DMI = "dmiConf.yml";
		// _dirname (setConfigDirectory) からの相対パスになるファイル・ディレクトリ End

		Settings(const Settings&) = delete;
		Settings& operator=(const Settings&) = delete;
		Settings(Settings&&) = delete;
		Settings& operator=(Settings&&) = delete;
		static Settings& get_instance() {
			static Settings inst;
			return inst;
		}

		enum SID_TYPE {
			CAR = 0,
			EDGE = 1,
			CLOUD = 2
		};
		SID_TYPE mySIDType = CAR;

		void initSettings();
		string readConfigFile(const char* key, const string &filename);
		string readConfigFile(const char* key, const string &filename, bool doLogError);
		string getParameter(const string &key);
		SID_TYPE getSIDType();
		bool getQueryStr(const string &fname, string &queryStr);
		string getConfigDirectory();
		string getFDDirectory();
		void setConfigDirectory(const string &dirname);
		void setMyStationID(string my_sid);
		bool IsFileExist(const std::string& name);
		//bool execCmd(const char* cmd, std::string& stdOut, int& exitCode);
	};
}
#endif  // SETTINGS_H
