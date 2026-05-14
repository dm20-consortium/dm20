#include "Settings.h"

#include <iostream>
#include <fstream>

using namespace std;

namespace IS {
	void Settings::setMyStationID(string my_sid)
	{
		this->settingMap["MY_STATION_ID"] = my_sid;
	}
	/**
	* 不変的な初期設定値を設定ファイルからロードする
	*
	* @author	Nagoya University
	* @date	2018/08/31
	*
	*/

	void Settings::initSettings()
	{
		this->settingMap.clear();

		// ******************** dm2.conf ******************** //
		// 自身のStationID
		//this->settingMap.insert(std::pair<string, string>("MY_STATION_ID", util.ReadConfigFile("MY_STATION_ID", CONF_FILE)));
		this->settingMap.insert(std::pair<string, string>("MY_STATION_ID", readConfigFile("MY_STATION_ID", CONF_FILE_CS)));

		string sidType = readConfigFile("SID_TYPE", CONF_FILE_SYS);
		if (sidType == "1") {
			mySIDType = EDGE;
			logger->debug("[initSettings] MY_STATION_ID:" + getParameter("MY_STATION_ID") + " NODE_TYPE: EDGE");
		} else if (sidType == "2") {
			mySIDType = CLOUD;
			logger->debug("[initSettings] MY_STATION_ID:" + getParameter("MY_STATION_ID") + " NODE_TYPE: CLOUD");
		} else {
			mySIDType = CAR;
			logger->debug("[initSettings] MY_STATION_ID:" + getParameter("MY_STATION_ID") + " NODE_TYPE: CAR");
		}
		// CloudのStationID
		this->settingMap.insert(std::pair<string, string>("CLOUD_STATION_ID", readConfigFile("CLOUD_STATION_ID", CONF_FILE_SYS)));

		// ******************* system.conf ******************* //
		// TCPタイムアウト時間（秒）
		this->settingMap.insert(std::pair<string, string>("TCP_TIMEOUT_SEC", readConfigFile("TCP_TIMEOUT_SEC", CONF_FILE_SYS)));
		// UDPタイムアウト時間（秒）
		this->settingMap.insert(std::pair<string, string>("UDP_SOCK_TIMEOUT_SEC", readConfigFile("UDP_SOCK_TIMEOUT_SEC", CONF_FILE_SYS)));
		// DTLSタイムアウト時間（秒）
		this->settingMap.insert(std::pair<string, string>("DTLS_SOCK_TIMEOUT_SEC", readConfigFile("DTLS_SOCK_TIMEOUT_SEC", CONF_FILE_SYS)));
		// DTLSタイムアウト許容回数
		this->settingMap.insert(std::pair<string, string>("DTLS_SOCK_TIMEOUT_CNT", readConfigFile("DTLS_SOCK_TIMEOUT_CNT", CONF_FILE_SYS)));
		// ストリーム受信スレッド数
		this->settingMap.insert(std::pair<string, string>("STREAM_RECEIVE_WORKER_NUM", readConfigFile("STREAM_RECEIVE_WORKER_NUM", CONF_FILE_SYS)));
		// CS受信スレッド数
		this->settingMap.insert(std::pair<string, string>("CS_RECEIVE_WORKER_NUM", readConfigFile("CS_RECEIVE_WORKER_NUM", CONF_FILE_SYS)));

		// SSL通信の有効無効指定
		this->settingMap.insert(std::pair<string, string>("IS_ENABLE_SSL", readConfigFile("IS_ENABLE_SSL", CONF_FILE_SYS)));
		// サーバ証明書ファイルの絶対パス
		this->settingMap.insert(std::pair<string, string>("SSL_CERTIFICATE_FILE_ABS_PATH", readConfigFile("SSL_CERTIFICATE_FILE_ABS_PATH", CONF_FILE_SYS)));
		// プライベートキーファイルの絶対パス
		this->settingMap.insert(std::pair<string, string>("SSL_PRIVATE_KEY_FILE_ABS_PATH", readConfigFile("SSL_PRIVATE_KEY_FILE_ABS_PATH", CONF_FILE_SYS)));
		// 証明書のパスフレーズ
		this->settingMap.insert(std::pair<string, string>("SSL_PEM_PASS_PHRASE", readConfigFile("SSL_PEM_PASS_PHRASE", CONF_FILE_SYS)));

		// 古いQUEUEの削除間隔(秒)
		this->settingMap.insert(std::pair<string, string>("QUEUE_DELETE_INTERVAL_SEC", readConfigFile("QUEUE_DELETE_INTERVAL_SEC", CONF_FILE_SYS)));
		// 削除対象のQUEUEを判定するための範囲(秒)
		this->settingMap.insert(std::pair<string, string>("QUEUE_DELETE_DEFAULT_RANGE_SEC", readConfigFile("QUEUE_DELETE_DEFAULT_RANGE_SEC", CONF_FILE_SYS)));
		// 削除対象のQUEUEを判定するための範囲(個数)
		this->settingMap.insert(std::pair<string, string>("QUEUE_DELETE_DEFAULT_RANGE_NUM", readConfigFile("QUEUE_DELETE_DEFAULT_RANGE_NUM", CONF_FILE_SYS)));

		// QUEUE受信スレッド数(デフォルト)
		this->settingMap.insert(std::pair<string, string>("QUEUE_RECEIVE_WORKER_DEFAULT_NUM", readConfigFile("QUEUE_RECEIVE_WORKER_DEFAULT_NUM", CONF_FILE_SYS)));

		// SID付与対象スキーマ
		this->settingMap.insert(std::pair<string, string>("ADD_SID_SCHEMA", readConfigFile("ADD_SID_SCHEMA", CONF_FILE_SYS)));
		// レーンID付与対象スキーマ
		this->settingMap.insert(std::pair<string, string>("ADD_LANE_ID_SCHEMA", readConfigFile("ADD_LANE_ID_SCHEMA", CONF_FILE_SYS)));
		// レーンID付与時の検索候補上限数
		this->settingMap.insert(std::pair<string, string>("ADD_LANE_SEARCH_LIMIT", readConfigFile("ADD_LANE_SEARCH_LIMIT", CONF_FILE_SYS)));
		// 優先的に参照するセンサ
		this->settingMap.insert(std::pair<string, string>("ADD_LANE_SEARCH_PRIORITY_SENSOR", readConfigFile("ADD_LANE_SEARCH_PRIORITY_SENSOR", CONF_FILE_SYS)));
		// 優先的に参照する経路計画ストリーム
		this->settingMap.insert(std::pair<string, string>("ADD_LANE_SEARCH_PRIORITY_PLAN_VID", readConfigFile("ADD_LANE_SEARCH_PRIORITY_PLAN_VID", CONF_FILE_SYS)));
		// 優先的に参照する経路計画ストリーム
		this->settingMap.insert(std::pair<string, string>("ADD_LANE_SEARCH_PRIORITY_PLAN_LANE", readConfigFile("ADD_LANE_SEARCH_PRIORITY_PLAN_LANE", CONF_FILE_SYS)));
		// レーンID付与時の距離閾値
		this->settingMap.insert(std::pair<string, string>("ADD_LANE_SEARCH_DIST_THRESHOLD", readConfigFile("ADD_LANE_SEARCH_DIST_THRESHOLD", CONF_FILE_SYS)));
		// レーンID付与時のヨー角閾値
		this->settingMap.insert(std::pair<string, string>("ADD_LANE_SEARCH_YAW_THRESHOLD", readConfigFile("ADD_LANE_SEARCH_YAW_THRESHOLD", CONF_FILE_SYS)));

		// スキーマ名
		this->settingMap.insert(std::pair<string, string>("QUERY_SCHEME_NAME", readConfigFile("QUERY_SCHEME_NAME", CONF_FILE_SYS)));
		// キャッシュ化しないテーブル名
		this->settingMap.insert(std::pair<string, string>("NOT_USED_CACHE_TABLE", readConfigFile("NOT_USED_CACHE_TABLE", CONF_FILE_SYS)));
		// エリア外監視間隔
		this->settingMap.insert(std::pair<string, string>("MONITORING_OUT_OF_AREA_INTERVAL_TIME", readConfigFile("MONITORING_OUT_OF_AREA_INTERVAL_TIME", CONF_FILE_SYS)));
		// SRID
		this->settingMap.insert(std::pair<string, string>("PLANE_NUMBER_SRID", readConfigFile("PLANE_NUMBER_SRID", CONF_FILE_SYS)));

		// 各クラスの情報をDBへ記録するモード
		this->settingMap.insert(std::pair<string, string>("HISTORY_RECORD_CLASS", readConfigFile("HISTORY_RECORD_CLASS", CONF_FILE_SYS)));
		// DBへ記録する周期
		this->settingMap.insert(std::pair<string, string>("HISTORY_RECORD_INTERVAL", readConfigFile("HISTORY_RECORD_INTERVAL", CONF_FILE_SYS)));
		// DBへ記録する時刻
		this->settingMap.insert(std::pair<string, string>("HISTORY_RECORD_HHMM", readConfigFile("HISTORY_RECORD_HHMM", CONF_FILE_SYS)));
		// 圧縮フラグ
		this->settingMap.insert(std::pair<string, string>("COMPRESS_FLG", readConfigFile("COMPRESS_FLG", CONF_FILE_SYS)));
		// タイマーモード
		this->settingMap.insert(std::pair<string, string>("TIMER_MODE", readConfigFile("TIMER_MODE", CONF_FILE_SYS)));


		// ******************** RDB.conf ******************** //
		// データベース名
		this->settingMap.insert(std::pair<string, string>("DATABASE_NAME", readConfigFile("DATABASE_NAME", CONF_FILE_DB)));
		// ISデータベース名
		this->settingMap.insert(std::pair<string, string>("DATABASE_IS_NAME", readConfigFile("DATABASE_IS_NAME", CONF_FILE_DB)));
		// IPアドレス
		this->settingMap.insert(std::pair<string, string>("DATABASE_ADDR", readConfigFile("DATABASE_ADDR", CONF_FILE_DB)));
		// ポート番号
		this->settingMap.insert(std::pair<string, string>("DATABASE_PORT", readConfigFile("DATABASE_PORT", CONF_FILE_DB)));
		// ユーザ名
		this->settingMap.insert(std::pair<string, string>("USER_NAME", readConfigFile("USER_NAME", CONF_FILE_DB)));
		// パスワード
		this->settingMap.insert(std::pair<string, string>("DB_PASS", readConfigFile("DB_PASS", CONF_FILE_DB)));

		//string output = "0", cmd = "grep \"IP_VERSION \" ../../makefile_cs -s | awk '{print $3}'";
		//execCmd(cmd.c_str(), output, exitCode);

	}

	/**
	 * 指定したファイルパスとkey文字列から設定値を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	key			キー文字列
	 * @param	filename	ファイルパス
	 *
	 * @return	設定値
	 */

	string Settings::readConfigFile(const char* key, const string &filename)
	{
		return readConfigFile(key, filename, true);
	}
	/**
	 * 指定したファイルパスとkey文字列から設定値を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	key			キー文字列
	 * @param	filename	ファイルパス
	 * @param	doLogError	エラー出力
	 *
	 * @return	設定値
	 */

	string Settings::readConfigFile(const char* key, const string &filename_str, bool doLogError)
	{
		const char *filename = filename_str.c_str();
		string pathname = this->_dirname + string(filename);
		std::ifstream ifscnf(pathname.c_str());
		if (!ifscnf) {
			cerr << "[readConfigFile] Failed read file :" << string(pathname) << endl;
			return "";
		}
		std::string strcnf;
		while (getline(ifscnf, strcnf)) {
			//読み込んだ行のスペースを全て削除
			strcnf.erase(remove(strcnf.begin(), strcnf.end(), ' '), strcnf.end());
			//先頭に#がある場合は読み飛ばす
			if (strcnf.substr(0, 0) == "#") {
				continue;
			}
			if (strcnf.find(key) == 0) {
				//keyにヒットする文字列がある行の途中に#がある場合はそれ以降の文字列を全て削除
				if (strcnf.find("#") != std::string::npos) {
					strcnf.erase(strcnf.find("#"));
				}
				//Trim処理をして=以降の文字列をリターン
				return strcnf.substr(strcnf.find('=') + 1);
			}
		}
		if (doLogError) {
			logger->error("[readConfigFile] Not found Key:" + string(key) + " file:" + string(pathname));
		}
		return "";
	}
	/**
	* ファイルの存在確認
	*
	* @author	Shinichi Kusayama
	* @date	2024/05/07
	*
	* @param	name			ファイル名
	*
	* @return	存在するか？
	*/
	bool Settings::IsFileExist(const std::string& name) {
		struct stat buffer;
		return (stat(name.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
	}
	/**
	* ロードした初期設定値を取得する
	*
	* @author	Nagoya University
	* @date	2018/08/31
	*
	* @param	key			キー文字列
	*
	* @return	設定値
	*/

	string Settings::getParameter(const string &key)
	{
		try {
			return this->settingMap.at(key);
		}
		catch (const std::out_of_range &) {
			logger->error("[getParameter] Not Found setting parameter. key:" + key);
			return "";
		}
	}

	/**
	* SIDの種別を取得する
	*
	* @author	Nagoya University
	* @date	2019/06/12
	*
	* @return	SID_TYPE(0:CAR, 1:EDGE, 2:CLOUD)
	*/
	Settings::SID_TYPE Settings::getSIDType()
	{
		return mySIDType;
	}

	/**
	* 指定したファイルからSQL文字列を取得する
	*
	* @author	Nagoya University
	* @date	2019/07/22
	*
	* @param [in]	fname			ファイル名
	* @param [in,out]	queryStr			クエリ文字列
	*
	* @return	成功:true/失敗:false
	*/
	bool Settings::getQueryStr(const string &fname, string &queryStr) {
		string query_file_path(_dirname + SQL_FILE_PATH);
		query_file_path += "/";
		query_file_path += fname;
		ifstream ifs(query_file_path.c_str());
		if (ifs.fail()) {
			logger->error("Query File Read Error:" + fname);
			return false;
		}
		istreambuf_iterator<char> it(ifs);
		istreambuf_iterator<char> last;
		string query_str(it, last);
		queryStr = query_str;
		return true;
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
	string Settings::getConfigDirectory() {
		return this->_dirname;
	}
	/**
	* FDファイルの格納場所をセットする
	*
	* @author	Shinichi Kusayama
	* @date	2024/4/16
	*
	* @return	dirname			ディレクトリ名
	*
	*/
	string Settings::getFDDirectory() {
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
	void Settings::setConfigDirectory(const string &dirname) {
		string suffix = "";
		if (dirname.substr(dirname.length() - 1, 1) != "/") suffix = "/";
		this->_dirname = dirname + suffix;
	}
	/**
	* linuxコマンドを実行する
	*
	* @author	Nagoya University
	* @date	2018/10/02
	*
	* @param [in]	cmd		実行コマンド文字列
	* @param [in,out]	stdOut	標準出力文字列
	* @param [in,out]	exitCode	実行コマンド終了ステータス
	*
	* @return	true:成功	false:失敗
	*/
	/*
	bool DmUtil::execCmd(const char* cmd, std::string& stdOut, int& exitCode) {
		IS::StringUtil stringUtil;
		std::shared_ptr<FILE> pipe(popen(cmd, "r"), [&](FILE* p) {exitCode = pclose(p); });
		if (!pipe) {
			return false;
		}
		std::array<char, 256> b;
		while (!feof(pipe.get())) {
			if (fgets(b.data(), b.size(), pipe.get()) != nullptr) {
				stdOut += b.data();
			}
		}
		stringUtil.deleteNl(stdOut);
		return true;
	}
	*/

}
