#include "SessionManager.h"

using namespace std;

namespace IS {

	/**
	* 初期化処理
	*
	* @author	Nagoya University
	* @date	2019/07/16
	*
	*/
	void SessionManager::init(const string &_confDirPath)
	{
		logFilePath = _confDirPath + logFilePathSuffix;
		rotateLoginHistory();
		readLoginHistory();
		// DB接続
		connection Conn(("dbname=" + settings.getParameter("DATABASE_IS_NAME") + " user=" + settings.getParameter("USER_NAME") + " password=" + settings.getParameter("DB_PASS") + " hostaddr=" + settings.getParameter("DATABASE_ADDR") + " port=" + settings.getParameter("DATABASE_PORT")));

		try {
			work T(Conn);
			result R1(T.exec("SELECT * FROM user_login;"));
			for (result::const_iterator c = R1.begin(); c != R1.end(); ++c) {
				string user = c[0].as(string());
				string hash = c[1].as(string());
				userMap[user] = hash;
			}
			T.commit();
		}
		catch (const sql_error &e) {
			string what(e.what());
			logger->warn("[init] Not found 'user_login' table. So create 'user_login' table.");
			// テーブル生成(user_login)
			string query;
			settings.getQueryStr("create_user_login.sql", query);
			work T(Conn);
			result R1(T.exec(query));
			T.commit();
		}
		catch (const usage_error &e) {
			string what(e.what());
			logger->warn("[init] Not found 'user_login' table. So create 'user_login' table.");
			// テーブル生成(user_login)
			string query;
			settings.getQueryStr("create_user_login.sql", query);
			work T(Conn);
			result R1(T.exec(query));
			T.commit();
		}

		try {
			work T(Conn);
			result R1(T.exec("SELECT * FROM login_history;"));
			T.commit();
		}
		catch (const sql_error &e) {
			string what(e.what());
			logger->warn("[init] Not found 'login_history' table. So create 'login_history' table.");
			// テーブル生成(user_login)
			string query;
			settings.getQueryStr("create_login_history.sql", query);
			work T(Conn);
			result R1(T.exec(query));
			T.commit();
		}
		catch (const usage_error &e) {
			string what(e.what());
			logger->warn("[init] Not found 'login_history' table. So create 'login_history' table.");
			// テーブル生成(user_login)
			string query;
			settings.getQueryStr("create_login_history.sql", query);
			work T(Conn);
			result R1(T.exec(query));
			T.commit();
		}

		//Conn.disconnect();
	}

	/**
	* セッションの生成 (DB未使用)
	*
	* @author	Shinichi Kusayama
	* @date	2025/02/04
	*
	* @param	data	受信データ
	* @param	user 	ユーザ名
	* @param	hash 	ハッシュ値
	* @param	pid		プロセスID
	*
	* @return	ユーザー認証結果
	*/
	bool SessionManager::createSession(const RecvData &data, const string &user, const string &hash, const string &pid)
	{
		IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		isp.init();
		bool isExistUser = false;
		string sessionKey = "", res = "";

		if (userMap.find(user) != userMap.end()) {
			if (userMap[user] == hash) isExistUser = true;
		}
		if (!isExistUser) {
			// ユーザ認証失敗
			logger->warn("Access from the user who is not registered user_login. user:" + user);
			isp.createErrorResult(0, ErrorCode::AUTHORITY_ERR, "Login failed due to unregistered user or password. user:" + user, res);
		} else {
			string ip = string(inet_ntoa(data.client.sin_addr));
			string loginKey = user + "," + ip + "," + pid;
			if (loginMap.find(loginKey) != loginMap.end()) {
				// 既に一度セッションキー取得済みの場合
				sessionKey = loginMap[loginKey];
			} else {
				// 初回時
				uuid_t value;
				char uuidStr[36 + 1]; // 36 byte string + null
				uuid_generate(value);
				uuid_unparse_upper(value, uuidStr);
				sessionKey = uuidStr;
				string loginInfo = user + "," + ip + "," + pid + "," + sessionKey + "," + to_string(DmUtil::getTimeMillisec());
				logger->debug("[createSession] created. user,ip,pid,sessionKey,unixTime:" + loginInfo);
				writeLoginHistory(loginInfo);
			}
			sessionMap[sessionKey] = user;
			// 正常レスポンス生成
			isp.createGeneralSingleResponseXML("create_session", sessionKey, res);
		}
		// レスポンスの返却
		IS::ResponseOperator		*opX = new IS::ResponseOperator(data);
		opX->TCPSend(res);
		opX->exit();
		delete opX;
		isp.finalize();

		return true;
	}
	/**
	* セッションの生成 (DB使用)
	*
	* @author	Nagoya University
	* @date	2019/07/16
	*
	* @param	data	受信データ
	* @param	user 	ユーザ名
	* @param	hash 	ハッシュ値
	* @param	pid		プロセスID
	*
	* @return	ユーザー認証結果
	*/
	bool SessionManager::createSessionDB(const RecvData &data, const string &user, const string &hash, const string &pid)
	{
		// userとhashの照合
		// DB接続
		connection Conn(("dbname=" + settings.getParameter("DATABASE_IS_NAME") + " user=" + settings.getParameter("USER_NAME") + " password=" + settings.getParameter("DB_PASS") + " hostaddr=" + settings.getParameter("DATABASE_ADDR") + " port=" + settings.getParameter("DATABASE_PORT")));
		string sessionKey = "", res = "";
		bool isExistUser = false;
		IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		try {
			work T(Conn);
			Conn.prepare("user_check", "SELECT user_name FROM user_login WHERE user_name = $1 AND hash = $2;");
			result R1 = T.exec_prepared("user_check", user, hash);
			for (result::const_iterator c = R1.begin(); c != R1.end(); ++c) {
				isExistUser = true;
			}
			if (isExistUser) {
				Conn.prepare("session_check", "SELECT session_key FROM login_history WHERE user_name = $1 AND ip = $2 AND pid = $3 AND session_key IS NOT NULL AND invalid = '0';");
				result R1 = T.exec_prepared("session_check", user, string(inet_ntoa(data.client.sin_addr)), pid);
				for (result::const_iterator c = R1.begin(); c != R1.end(); ++c) {
					sessionKey = c[0].as(string());
				}
			}

			T.commit();
		}
		catch (const exception &e) {
			string what(e.what());
			logger->error("[createSession] RDBAccess ERROR. cause:" + what);
			return false;
		}
		isp.init();
		if (!isExistUser) {
			// ユーザ認証に失敗した場合
			// 異常レスポンス生成
			logger->warn("Access from the user who is not registered user_login. user:" + user);

			// ログイン履歴(セッションキーなし)を登録
			work T(Conn);
			Conn.prepare("insert_login_history", "INSERT INTO login_history VALUES($1,$2,$3,NULL,'1',$4);");
			T.exec_prepared("insert_login_history", user, string(inet_ntoa(data.client.sin_addr)), stoi(pid), DmUtil::getTimeMillisec());
			T.commit();

			isp.createErrorResult(0, ErrorCode::AUTHORITY_ERR, "Login failed due to unregistered user or password. user:" + user, res);
		}
		else {
			// セッションキーの生成
			if (sessionKey.length() == 0) {
				uuid_t value;
				char uuidStr[36 + 1]; // 36 byte string + null
				uuid_generate(value);
				uuid_unparse_upper(value, uuidStr);
				sessionKey = uuidStr;
				logger->debug("[createSession] sessionKey created. user:" + string(user) + " sessionKey:" + sessionKey);

				// ログイン履歴(セッションキーあり)を登録
				work T(Conn);
				Conn.prepare("insert_login_history", "INSERT INTO login_history VALUES($1,$2,$3,$4,'0',$5);");
				T.exec_prepared("insert_login_history", user, string(inet_ntoa(data.client.sin_addr)), stoi(pid), sessionKey, DmUtil::getTimeMillisec());
				T.commit();
			}
			sessionMap[sessionKey] = user;

			// 正常レスポンス生成
			isp.createGeneralSingleResponseXML("create_session", sessionKey, res);
		}
		isp.finalize();
		// レスポンスの返却
		IS::ResponseOperator		*opX = new IS::ResponseOperator(data);
		opX->TCPSend(res);
		opX->exit();
		delete opX;

		//Conn.disconnect();
		return true;
	}

	/**
	* セッションの照合 (DB未使用)
	*
	* @author	Shinichi Kusayama
	* @date	2025/02/04
	*
	* @param[in]	hash 	ハッシュ値
	* @param[in,out]	user 	ユーザ名
	*
	* @return	照合結果
	*/
	bool SessionManager::checkSession(const string &hash, string &user)
	{
		if (sessionMap.find(hash) != sessionMap.end()) {
			user = sessionMap[hash];
			return true;
		}
		return false;
	}
	/**
	* セッションの照合 (DB使用)
	*
	* @author	Nagoya University
	* @date	2019/07/16
	*
	* @param[in]	hash 	ハッシュ値
	* @param[in,out]	user 	ユーザ名
	*
	* @return	照合結果
	*/
	bool SessionManager::checkSessionDB(const string &hash, string &user)
	{
		if (hash == "") return false;
		// userとhashの照合
		if (sessionMap.count(hash) == 0) {

			// hashの照合
			// DB接続
			connection Conn(("dbname=" + settings.getParameter("DATABASE_IS_NAME") + " user=" + settings.getParameter("USER_NAME") + " password=" + settings.getParameter("DB_PASS") + " hostaddr=" + settings.getParameter("DATABASE_ADDR") + " port=" + settings.getParameter("DATABASE_PORT")));
			bool isExistUser = false;

			try {
				work T(Conn);
				Conn.prepare("session_history_check", "SELECT user FROM login_history WHERE session_key =$1 order by time desc;");
				result R1 = T.exec_prepared("session_history_check", hash);
				for (result::const_iterator c = R1.begin(); c != R1.end(); ++c) {
					user = c[0].as(string());
					isExistUser = true;
					sessionMap[hash] = user;
					break;
				}
				T.commit();
			}
			catch (const exception &e) {
				string what(e.what());
				logger->error("[createSession] RDBAccess ERROR. cause:" + what);
				return false;
			}
			if (!isExistUser) return false;
		}

		// ユーザ情報の取得
		user = sessionMap[hash];

		return true;
	}
	/**
	* ログイン履歴をファイルから読み込み
	*
	* @author	Shinichi Kusayama
	* @date	2025/02/25
	*
	* @param	loginfo	ログイン情報
	*/
	void SessionManager::readLoginHistory()
	{
		ifstream inputfile(logFilePath, std::ios::app);
		if (inputfile.is_open()) {
			std::string str_buf, str_conma_buf;
			while (getline(inputfile, str_buf)) {
				vector<string> vs = stringUtil.split(str_buf, ",");
				if (vs.size() >= 4) {
					string user = vs[0];
					string key = user + "," + vs[1] + "," + vs[2];		// user,ip,pid
					string sessionKey = vs[3];
					loginMap[key] = sessionKey;
					sessionMap[sessionKey] = user;
				}
			}
		}
		inputfile.close();
		return;
	}
	/**
	* ログイン履歴をファイルに書き込み
	*
	* @author	Shinichi Kusayama
	* @date	2025/02/04
	*
	* @param	loginfo	ログイン情報
	*/
	void SessionManager::writeLoginHistory(const string &loginfo)
	{
		ofstream outputfile(logFilePath, std::ios::app);
		if (outputfile.is_open()) {
			outputfile << loginfo + "\n";
		}
		outputfile.close();
		return;
	}
	/**
	* ログローテーション
	*
	* @author	Shinichi Kusayama
	* @date	2025/02/04
	*
	*/
	void SessionManager::rotateLoginHistory()
	{
		struct stat statBuf;
		if (stat(logFilePath.c_str(), &statBuf) == 0) {
			// ファイル情報取得成功
			if ((size_t)statBuf.st_size > maxFileSize) {
				// 現在日時取得
				std::time_t now = std::time(nullptr);
				char buf[20];
				std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", std::localtime(&now));
				// 現在のログファイルを日時付きでローテーション
				std::string newFilePath = logFilePath + "_" + std::string(buf);
				std::rename(logFilePath.c_str(), newFilePath.c_str());
			}
		}
	}
}
