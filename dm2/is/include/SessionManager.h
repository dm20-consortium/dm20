#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

// SessionManagerクラス

#include <uuid/uuid.h>
#include <log4cxx/logger.h>
#include <pqxx/pqxx>

#include "is/DmUtil.h"
#include "Settings.h"
#include "is/StringUtil.h"
#include "ErrorCode.h"
#include "ResponseOperator.h"

using namespace std;
using namespace log4cxx;
using namespace pqxx;

namespace IS {

	/**
	 * セッションを管理するクラス
	 *
	 * @author	Nagoya University
	 * @date	2019/07/16
	 */

	class SessionManager
	{
	private:
		LoggerPtr logger = Logger::getLogger("SessionManager");
		SessionManager() = default;
		~SessionManager() = default;

		//IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		IS::Settings &settings = IS::Settings::get_instance();
		StringUtil stringUtil;
		map<string, string> sessionMap;		// 登録済みセッションキー, ユーザ名
		map<string, string> userMap;		// user_loginテーブル保持
		map<string, string> loginMap;		// login_historyテーブル保持 (user, ip, pid => sessionKey)
		string logFilePath;
		const string logFilePathSuffix = "/is_conf/login_history.csv";
		const std::size_t maxFileSize = 50 * 1024 * 1024; // 50MB
		
		void readLoginHistory();
		void writeLoginHistory(const string &loginfo);
		void rotateLoginHistory();
	public:
		SessionManager(const SessionManager&) = delete;
		SessionManager& operator=(const SessionManager&) = delete;
		SessionManager(SessionManager&&) = delete;
		SessionManager& operator=(SessionManager&&) = delete;

		static SessionManager& get_instance() {
			static SessionManager inst;
			return inst;
		}
		void init(const string &_confDirPath);
		bool createSession(const RecvData &data, const string &user, const string &hash, const string &pid);
		bool createSessionDB(const RecvData &data, const string &user, const string &hash, const string &pid);
		bool checkSession(const string &hash, string &user);
		bool checkSessionDB(const string &hash, string &user);

	};
}
#endif  // SESSIONMANAGER_H
