#ifndef INFORMATIONSOURCEMANAGER_H
#define INFORMATIONSOURCEMANAGER_H
#include "is/Schema.h"
#include "InformationSourceParser.h"
#include "Settings.h"
#include "Parser.h"

#include <xercesc/dom/DOMElement.hpp>
#include <map>  
#include <sys/types.h>
#include <dirent.h>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <pqxx/pqxx>
#include <log4cxx/logger.h>

using namespace std;
using namespace pqxx;
using namespace log4cxx;

namespace IS {

	/**
	 * 情報源を管理するクラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	class InformationSourceManager
	{
	private:
		InformationSourceManager() = default;
		~InformationSourceManager() = default;

		LoggerPtr logger = Logger::getLogger("InformationSourceManager");
		IS::Settings &settings = IS::Settings::get_instance();
		static map<string, DOMDocument*> streamSchemaDomMap;
		static map<string, DOMDocument*> rdbSchemaDomMap;
		static map<string, Schema> streamSchemaMap;
		static map<string, Schema> rdbSchemaMap;
		map<int, string> userDefinedOidList;
		map<int, string> tableNameOidList;

		void checkAndUpdateSystemTable();
		bool checkSystemTable(work &tran, string tableName, string columnName);
		void dropSystemTable(work &tran, string tableName);
		void createSystemTable(work &tran, string tableName);
		void settingStreamSchema();
		void settingRdbSchema();
		void settingTrigger();
		void saveUserDefinedInfo(const string &tableName, const string &columnName);
		void extractRelatedTables(work &T, const string &tableName, vector<string> &relationTableList);
		Schema getStreamSchemaFromFile(const string &tablename, const string &schema_path);
		string getCopyPath(const string &search_path, const string &tableName, string &schema_path);

	public:
		InformationSourceManager(const InformationSourceManager&) = delete;
		InformationSourceManager& operator=(const InformationSourceManager&) = delete;
		InformationSourceManager(InformationSourceManager&&) = delete;
		InformationSourceManager& operator=(InformationSourceManager&&) = delete;

		static InformationSourceManager& get_instance() {
			static InformationSourceManager inst;
			return inst;
		}
		bool init();
		map<string, Schema> getStreamSchemaMap();
		map<string, Schema> getRdbSchemaMap();
		map<string, Schema> getAllSchemaMap();

		vector<string> getStreamSchemaNames();
		vector<string> getAllSchemaNames();
		Schema getSchema(const string &tablename);
		void getStreamSchema(const string &tablename, Schema &schema);
		void getRdbSchema(const string &tablename, Schema &schema);
		string getUserDefinedTypeNameByOid(int oid);
		string getTableNameByOid(int oid);
	};
}
#endif  // INFORMATIONSOURCEMANAGER_H