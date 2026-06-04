#include "InformationSourceManager.h"


namespace IS {

	map<string, DOMDocument*> InformationSourceManager::streamSchemaDomMap;
	map<string, DOMDocument*> InformationSourceManager::rdbSchemaDomMap;
	map<string, Schema> InformationSourceManager::streamSchemaMap;
	map<string, Schema> InformationSourceManager::rdbSchemaMap;

	/**
	 * 初期化処理
	 * ストリーム設定のスキーマ情報とRDBのスキーマ情報を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	bool InformationSourceManager::init()
	{
		//IS::InformationSourceParser::create();

		streamSchemaDomMap.clear();
		rdbSchemaDomMap.clear();

		streamSchemaMap.clear();
		rdbSchemaMap.clear();

		try {
			// update対象のテーブルがあるかチェックし、あれば更新する。
			checkAndUpdateSystemTable();

			// RDBからschema情報を取得する
			settingRdbSchema();
			
			// conf配下からschema情報を取得する
			settingStreamSchema();

			// RDBにトリガーを作成する
			settingTrigger();
		} catch (...) {
			return false;
		}
		return true;
		
	}

	/**
	 * ストリーム設定のスキーマ情報を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @return	ストリーム設定のスキーマ情報マップ
	 */

	map<string, Schema> InformationSourceManager::getStreamSchemaMap()
	{
		return streamSchemaMap;
	}

	/**
	 * RDBのスキーマ情報を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @return	RDBのスキーマ情報マップ
	 */

	map<string, Schema> InformationSourceManager::getRdbSchemaMap()
	{
		return rdbSchemaMap;
	}

	/**
	 * 全てのスキーマ情報を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @return	スキーマ情報
	 */

	map<string, Schema> InformationSourceManager::getAllSchemaMap()
	{
		//REL_COMMENT logger->trace("getAllSchemaMap IN");
		map<string, Schema> retMap;
		for (auto itr = streamSchemaMap.begin(); itr != streamSchemaMap.end(); ++itr) {
			retMap[itr->first] = itr->second;
			logger->debug("StreamSchemaMap ADD : " + itr->first);
		}
		for (auto itr = rdbSchemaMap.begin(); itr != rdbSchemaMap.end(); ++itr) {
			retMap[itr->first] = itr->second;
			logger->debug("RDBSchemaMap    ADD : " + itr->first);
		}
		return retMap;
	}

	/**
	 * ストリーム情報に存在する複数のストリーム名を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @return	ストリーム名配列
	 */

	vector<string> InformationSourceManager::getStreamSchemaNames()
	{
		vector<string> tableNames;
		for (auto itr = streamSchemaDomMap.begin(); itr != streamSchemaDomMap.end(); ++itr) {
			tableNames.push_back(itr->first);
		}
		return tableNames;
	}

	/**
	 * スキーマ設定から全てのストリーム名とテーブル名を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @return	全ストリーム名とテーブル名
	 */

	vector<string> InformationSourceManager::getAllSchemaNames()
	{
		vector<string> tableNames;
		for (auto itr = streamSchemaDomMap.begin(); itr != streamSchemaDomMap.end(); ++itr) {
			tableNames.push_back(itr->first);
		}
		for (auto itr = rdbSchemaDomMap.begin(); itr != rdbSchemaDomMap.end(); ++itr) {
			tableNames.push_back(itr->first);
		}
		return tableNames;
	}

	/**
	 * テーブル名を取得してスキーマ情報を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	tablename	指定テーブル名
	 *
	 * @return	指定のスキーマ情報
	 */

	Schema InformationSourceManager::getSchema(const string &tablename)
	{
		string schema_path = settings.getConfigDirectory() + settings.CONF_DIR_SCHEMA;
		IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		isp.init();
		for (auto itr = streamSchemaDomMap.begin(); itr != streamSchemaDomMap.end(); ++itr) {
			if (itr->first == tablename) {
				IS::Schema schema = IS::Schema(tablename, IS::Schema::STREAM);
				// TODO : DOMDocumentからschemaが生成できる事が理想
				//isp.setSchemaInfo(itr->second, &schema);
				isp.setSchemaInfo(&schema, tablename, schema_path);
				isp.finalize();
				return schema;
			}
		}
		isp.finalize();
		logger->error("[getSchema] Not found schema. targetName:" + tablename);
		return Schema();
	}

	/**
	* テーブル名を取得してスキーマ情報を取得する
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param	tablename	指定テーブル名(ストリーム名)
	* @param[in,out]	schema	スキーマ情報
	*
	*/

	void InformationSourceManager::getStreamSchema(const string &tablename, Schema &schema)
	{
		for (auto itr = streamSchemaMap.begin(); itr != streamSchemaMap.end(); ++itr) {
			if (itr->first == tablename) {
				schema = itr->second;
				return;
			}
		}
		logger->error("[getStreamSchema] Not found schema. targetName:" + tablename);
		schema = Schema();
	}

	/**
	* テーブル名を取得してスキーマ情報を取得する
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param	tablename	指定テーブル名(RDB)
	* @param[in,out]	schema	スキーマ情報
	*
	*/

	void InformationSourceManager::getRdbSchema(const string &tablename, Schema &schema)
	{
		for (auto itr = rdbSchemaMap.begin(); itr != rdbSchemaMap.end(); ++itr) {
			if (itr->first == tablename) {
				schema = itr->second;
				return;
			}
		}
		logger->error("[getRdbSchema] Not found schema. targetName:" + tablename);
		schema = Schema();
	}

	/**
	* テーブル名を取得してスキーマ情報を取得する
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param	tablename	指定テーブル名(ストリーム名)
	*
	* @return	指定のスキーマ情報
	*/

	inline Schema InformationSourceManager::getStreamSchemaFromFile(const string &tablename, const string &schema_path)
	{
		IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		isp.init();
		for (auto itr = streamSchemaDomMap.begin(); itr != streamSchemaDomMap.end(); ++itr) {
			if (itr->first == tablename) {
				IS::Schema schema = IS::Schema(tablename, IS::Schema::STREAM);
				isp.setSchemaInfo(&schema, tablename, schema_path);
				isp.finalize();
				return schema;
			}
		}
		isp.finalize();
		return Schema();
	}
	/**
	 * XMLファイルにcopyタグがある場合、コピー元のtable情報をコピーし、結果を一時ファイルとして格納する。
	 *
	 * @author	Shinichi Kusayama
	 * @date	2024/10/29
	 * @param	search_path	見つかったファイルのパス
	 * @param	tableName	テーブル名
	 * @param	schema_path	スキーマファイルが置かれているパス
	 *
	 * @return	copyタグがある場合は、一時ファイルのパス。タグがない場合は、空文字
	 */

	string InformationSourceManager::getCopyPath(const string &search_path, const string &tableName, string &schema_path)
	{
		StringUtil stringUtil;
		string copy_path = search_path;
		// copyタグが書かれている可能性のあるファイルを読込み
		ifstream ifs(search_path.c_str());
		if (ifs.fail()) {
			logger->error("Query File Read Error:" + search_path);
			return "";
		}
		istreambuf_iterator<char> it(ifs);
		istreambuf_iterator<char> last;
		string query_str(it, last);
		// copyタグが書かれているかチェック
		std::string::size_type begin_pos = query_str.find("<copy>");
		std::string::size_type end_pos = query_str.find("</copy>");
		if (begin_pos == std::string::npos) {
			return "";
		}
		// copyタグの中身で、ファイルパスを置換（コピー元のファイルパス）
		string copyTableName = query_str.substr(begin_pos + 6, end_pos- begin_pos - 6);
		stringUtil.replaceAll(copy_path, tableName, copyTableName);

		// 置換したファイルパス（コピー元）のスキーマ情報を取得
		ifstream copy_ifs(copy_path.c_str());
		if (copy_ifs.fail()) {
			logger->error("Query File Read Error:" + search_path);
			return "";
		}
		istreambuf_iterator<char> copy_it(copy_ifs);
		istreambuf_iterator<char> copy_last;
		string copy_query_str(copy_it, copy_last);
		// コピー元のスキーマ情報の中から、テーブル名を狙って、コピー先へと一括置換
		stringUtil.replaceAll(copy_query_str, copyTableName, tableName);

		// コピースキーマ情報を格納するディレクトリの存在チェック＆なければ作成
		string copy_dir_name = "copy/";
		string copy_dir_path = schema_path + copy_dir_name;
		struct stat stat_buf;
		if (stat(copy_dir_path.c_str(), &stat_buf) != 0) {
			mkdir(copy_dir_path.c_str(), 0755);
		}
		// コピースキーマ情報の保存先のパスを作成
		string tmp_file_path = "";
		vector<string> vs = stringUtil.split(search_path, "/");
		if (vs.size() > 0) {
			for (int i = 0; i < (int)vs.size() - 1; i++) {
				if (i != 0) tmp_file_path += "/";
				tmp_file_path += vs[i];
			}
			tmp_file_path = tmp_file_path + "/" + copy_dir_name + vs[vs.size() - 1];
		} else {
			tmp_file_path += search_path;
		}
		// コピースキーマ情報をファイルに書込み
		std::ofstream ofs(tmp_file_path);
		if (!ofs) {
			return "";
		}
		ofs << copy_query_str;
		ofs.close();
		schema_path = copy_dir_path;
		return tmp_file_path;
	}
	/**
	 * ストリーム設定ファイルからストリーム情報を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void InformationSourceManager::settingStreamSchema()
	{
		IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		StringUtil stringUtil;
		int i, dirElements;
		string search_path;
		string schema_path = settings.getConfigDirectory() + settings.CONF_DIR_SCHEMA;
		struct stat stat_buf;
		struct dirent **namelist = NULL;
		isp.init();
		// dirElements にはディレクトリ内の要素数が入る
		dirElements = scandir(schema_path.c_str(), &namelist, NULL, NULL);

		if (dirElements == -1) {
			logger->error("Directory is not found" + schema_path);
		}

		else {

			//ディレクトリかファイルかを順番に識別
			for (i = 0; i<dirElements; i += 1) {

				// "." と ".." を除く
				if ((strcmp(namelist[i]->d_name, ".\0") != 0) && (strcmp(namelist[i]->d_name, "..\0") != 0)) {

					//search_pathには検索対象のフルパスを格納する
					search_path = schema_path + string(namelist[i]->d_name);

					// ファイル情報の取得の成功
					if (stat(search_path.c_str(), &stat_buf) == 0) {

						// ディレクトリだった場合の処理
						if ((stat_buf.st_mode & S_IFMT) == S_IFDIR) {
							continue;
						}

						//ファイルだった場合の処理
						else if (stringUtil.ends_with(search_path, ".xml")) {
							string tableName = stringUtil.split(string(namelist[i]->d_name), ".")[0];
							string tmp_schema_path = schema_path;
							string copy_path = getCopyPath(search_path, tableName, tmp_schema_path);
							if (copy_path != "") {
								search_path = copy_path;
							}
							DOMDocument* dom = isp.parseXmlFile(search_path);
							if (dom != NULL) {
								streamSchemaDomMap[tableName] = dom;
								streamSchemaMap[tableName] = getStreamSchemaFromFile(tableName, tmp_schema_path);
								streamSchemaMap[tableName].appendAdminColumn();
							}
						}
					}

					// ファイル情報の取得失敗
					else {
						logger->error("Failed to acquire file information. Path:" + search_path);
					}
				}
			}
		}
		isp.finalize();
		for (int i = 0; i < dirElements; i++) {
			free(namelist[i]);
		}
		free(namelist);
	}

	/**
	 * RDBからテーブル情報を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void InformationSourceManager::settingRdbSchema()
	{
		IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		IS::Settings &settings = IS::Settings::get_instance();
		string schemeName = settings.getParameter("QUERY_SCHEME_NAME");
		vector<string> tableList;

		try {
			// DB接続
			connection Conn(("dbname=" + settings.getParameter("DATABASE_NAME") + " user=" + settings.getParameter("USER_NAME") + " password=" + settings.getParameter("DB_PASS")  + " host=" + settings.getParameter("DATABASE_ADDR") + " port=" + settings.getParameter("DATABASE_PORT")));
			work T(Conn);

			string query;
			query.append(" SELECT  cls.oid      AS object_id");
			query.append("       ,nms.nspname  AS schema_name");
			query.append("       ,cls.relname  AS table_name");
			query.append("       ,att.attnum   AS column_no");
			query.append("       ,att.attname  AS column_name");
			query.append("       ,typ.typname  AS column_type");
			query.append("       ,cls.relkind  AS object_type");
			query.append(" FROM        pg_catalog.pg_class       cls");
			query.append(" LEFT  JOIN  pg_catalog.pg_namespace   nms");
			query.append("        ON  cls.relnamespace = nms.oid");
			query.append(" LEFT  JOIN  pg_catalog.pg_attribute   att");
			query.append("        ON  cls.oid = att.attrelid");
			query.append("       AND  att.attnum > 0");
			query.append(" LEFT  JOIN  pg_catalog.pg_type typ");
			query.append("        ON  att.atttypid = typ.oid");
			query.append(" WHERE       cls.relkind IN ('r','v','m')");
			query.append("   AND       nms.nspname = '" + schemeName + "'");
			query.append(" ORDER BY    schema_name");
			query.append("           ,table_name");
			query.append("           ,column_no");
			query.append(";");

			// SQLの実行(テーブルカラム一覧取得)
			result R1(T.exec(query));
			IS::Schema schema;
			string tableName;
			IS::Schema::relationType relType = IS::Schema::relationType::RELATION;
			IS::Attribute attr;
			isp.init();
			for (result::const_iterator c = R1.begin(); c != R1.end(); ++c) {
				if (c[3].as(int()) == 1) {
					if (schema.getAttributeSize() != 0) {
						rdbSchemaDomMap[tableName] = isp.parseSchema(schema);
						rdbSchemaMap[tableName] = schema;
						logger->debug("[settingRdbSchema] rdbSchemaMap Append table:" + tableName + " relType:" + to_string(static_cast<int>(relType)));
					}
					tableName = c[2].as(string());
					tableNameOidList[c[0].as(int())] = tableName;
					if (c[6].as(string()) == "r") {
						relType = IS::Schema::relationType::RELATION;
					}
					else if (c[6].as(string()) == "m") {
						relType = IS::Schema::relationType::M_VIEW;
					}
					else if (c[6].as(string()) == "v") {
						relType = IS::Schema::relationType::VIEW;
					}
					schema = IS::Schema(tableName, IS::Schema::RDB, relType);
				}
				attr.name = c[4].as(string());
				attr.type = c[5].as(string());
				logger->trace("[settingRdbSchema] table:" + tableName + " column idx:" + to_string(c[3].as(int())) + " name:" + attr.name + " type:" + attr.type + " relType:" + c[6].as(string()));
				schema.append(attr);
				if (attr.type == "USER-DEFINED" || attr.type == "geometry" || attr.type == "geography") {
					saveUserDefinedInfo(tableName, attr.name);
				}
			}
			if (schema.getAttributeSize() != 0) {
				rdbSchemaDomMap[tableName] = isp.parseSchema(schema);
				rdbSchemaMap[tableName] = schema;
				logger->debug("[settingRdbSchema] rdbSchemaMap Append table:" + tableName);
			}

			vector<string> relationTableList;
			// Viewの参照テーブルを保持する
			for (auto itr = rdbSchemaMap.begin(); itr != rdbSchemaMap.end(); ++itr) {
				if (itr->second.getRelationType() == IS::Schema::relationType::VIEW) {
					relationTableList.clear();
					extractRelatedTables(T, itr->first, relationTableList);
					itr->second.setRelationTableList(relationTableList);
				}
			}
			isp.finalize();
			T.commit();
			//Conn.disconnect();
		}
		catch (const exception &e) {
			//cerr << e.what() << endl;
			throw;
		}
	}

	/**
	 * システムテーブルを更新する必要があるかチェックし、あれば更新（再作成）する。
	 *
	 * @author	Shinichi Kusayama
	 * @date	2023/08/15
	 *
	 */
	void InformationSourceManager::checkAndUpdateSystemTable()
	{
		/*
		string databaseName = "dm2_is_db";
		// 下記にカラム定義を更新するテーブル名と、トリガとなるカラム名を入れる。
		// 2023/8/15: recv_history (当該PG新設につき、追加)
		// 2023/8/15: operator_historyのカラム見直し. pid追加
		// 2023/8/29: query_historyのカラム見直し. action_user追加
		vector<string> table_name = {"recv_history", "operator_history", "query_history"};
		vector<string> column_name = {"", "last_buffer_size_long", "action_user"};
		try {
			// DB接続
			connection Conn(("dbname=" + databaseName + " user=" + settings.getParameter("USER_NAME") + " password=" + settings.getParameter("DB_PASS")  + " host=" + settings.getParameter("DATABASE_ADDR") + " port=" + settings.getParameter("DATABASE_PORT")));
			work tran(Conn);
			for(int i = 0; i < (int)(table_name.size()); ++i) {
				bool isExistColumn = checkSystemTable(tran, table_name[i], column_name[i]);
				if (!isExistColumn) {
					bool isExistTable = checkSystemTable(tran, table_name[i], "");
					if (isExistTable) {
						dropSystemTable(tran, table_name[i]);
					}
					createSystemTable(tran, table_name[i]);
					logger->warn("System-Table is Updated: " + table_name[i]);
				}
			}
			tran.commit();
			//Conn.disconnect();
		}
		catch (const exception &e) {
			cerr << e.what() << endl;
		}
		*/
		return;
	}
	/**
	 * システムテーブルをDROPする
	 *
	 * @author	Shinichi Kusayama
	 * @date	2023/08/15
	 * @param	databaseName	データベース名
	 * @param	tableName		テーブル名
	 *
	 * @return	レコードが存在した場合、true
	 */
	void InformationSourceManager::dropSystemTable(work &tran, string tableName)
	{
		string query = " DROP TABLE " + tableName + ";";
		result R1(tran.exec(query));
		return;
	}
	/**
	 * システムテーブルを作成する（create_<table_name>で検索し、見つかったファイルを使ってSQLを発行する）
	 *
	 * @author	Shinichi Kusayama
	 * @date	2023/08/15
	 * @param	tableName		テーブル名
	 * @param	columnName		カラム名
	 *
	 * @return	レコードが存在した場合、true
	 */
	void InformationSourceManager::createSystemTable(work &tran, string tableName)
	{
		string query;
		string fname = "create_" + tableName + ".sql";
		settings.getQueryStr(fname, query);
		// 読み込んだファイルを使って、DBへ登録する。
		result R1(tran.exec(query));
		return;
	}
	/**
	 * システムテーブルをチェックする
	 *
	 * @author	Shinichi Kusayama
	 * @date	2023/08/15
	 * @param	tableName		テーブル名
	 * @param	columnName		カラム名
	 *
	 * @return	レコードが存在した場合、true
	 */
	bool InformationSourceManager::checkSystemTable(work &tran, string tableName, string columnName)
	{
		bool ret = false;
		string query;
		query.append(" SELECT  att.attnum   AS column_no");
		query.append(" FROM        pg_catalog.pg_class       cls");
		query.append(" LEFT  JOIN  pg_catalog.pg_namespace   nms");
		query.append("        ON  cls.relnamespace = nms.oid");
		query.append(" LEFT  JOIN  pg_catalog.pg_attribute   att");
		query.append("        ON  cls.oid = att.attrelid");
		query.append("       AND  att.attnum > 0");
		query.append(" LEFT  JOIN  pg_catalog.pg_type typ");
		query.append("        ON  att.atttypid = typ.oid");
		query.append(" WHERE       cls.relkind IN ('r','v','m')");
		query.append("   AND       cls.relname = '" + tableName + "'");
		if (columnName != "") query.append("   AND       att.attname = '" + columnName + "'");
		query.append(";");

		// SQLの実行(テーブルカラム一覧取得)
		result R1(tran.exec(query));
		if (R1.begin() != R1.end()) {
			ret = true;
		}
		return ret;
	}

	/**
	* RDBの更新状況を補足するためにトリガーを作成する
	*
	* @author	Nagoya University
	* @date	2020/01/28
	*/
	void InformationSourceManager::settingTrigger()
	{
		vector<string> userList;
		StringUtil stringUtil;
		try {
			// DB接続
			connection Conn(("dbname=" + settings.getParameter("DATABASE_NAME") + " user=" + settings.getParameter("USER_NAME") + " password=" + settings.getParameter("DB_PASS") + " host=" + settings.getParameter("DATABASE_ADDR") + " port=" + settings.getParameter("DATABASE_PORT")));

			try {
				work T(Conn);
				// SQLの実行(トリガー関数の登録)
				string state;
				state.append("CREATE OR REPLACE FUNCTION notify_trigger() RETURNS trigger AS $$");
				state.append("  BEGIN");
				state.append("    PERFORM pg_notify(TG_ARGV[0], TG_ARGV[1]);");
				state.append("    RETURN NULL;");
				state.append("  END;");
				state.append("$$ LANGUAGE 'plpgsql';");
				result R1(T.exec(state));
				T.commit();
			}
			catch (const sql_error &e) {
				string what(e.what());
				logger->error("[settingTrigger] Failed to Create Function. what:" + what);
			}

			// SQLの実行(各テーブルにトリガーを設定する)
			string drop = "DROP TRIGGER IF EXISTS $1_update on $1;";
			string createTriggerTable;
			createTriggerTable.append("CREATE TRIGGER $1_update ");
			createTriggerTable.append("AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE ON $1 ");
			createTriggerTable.append("EXECUTE PROCEDURE notify_trigger('dm2_db_update','$1');");
			string createTriggerView;
			createTriggerView.append("CREATE TRIGGER $1_update ");
			createTriggerView.append("AFTER INSERT OR UPDATE OR DELETE ON $1 ");
			createTriggerView.append("EXECUTE PROCEDURE notify_trigger('dm2_db_update','$1');");
			string repState;
			for (auto itr = rdbSchemaMap.begin(); itr != rdbSchemaMap.end(); ++itr) {
				try {
					work T(Conn);
					repState = drop;
					stringUtil.replaceAll(repState, "$1", itr->first);
					result R1(T.exec(repState));
					if (itr->second.getRelationType() == IS::Schema::relationType::RELATION)
					{
						repState = createTriggerTable;
					}
					else if (itr->second.getRelationType() == IS::Schema::relationType::VIEW)
					{
						repState = createTriggerView;
					}
					else if (itr->second.getRelationType() == IS::Schema::relationType::M_VIEW)
					{
						logger->trace("[settingTrigger] create trigger SKIP because relationType : materized view. table:" + itr->first);
						T.commit();
						continue;
					}
					stringUtil.replaceAll(repState, "$1", itr->first);
					result R2(T.exec(repState));
					T.commit();
					logger->trace("[settingTrigger] create trigger. table:" + itr->first);
				}
				catch (const sql_error &e) {
					string what(e.what());
					logger->error("[settingTrigger] Failed to Create Trigger. table:" + itr->first + " what:" + what);
				}
			}

			//Conn.disconnect();
		}
		catch (const exception &e) {
			cerr << e.what() << endl;
			//throw;
		}
	}

	/**
	* RDBからユーザ定義のデータ型のOIDとデータ型名を取得する
	*
	* @author	Nagoya University
	* @date	2018/05/09
	* @param	tablename	指定テーブル名(ストリーム名)
	* @param	columnName	指定カラム名
	*/

	void InformationSourceManager::saveUserDefinedInfo(const string &tableName, const string &columnName)
	{
		IS::Settings &settings = IS::Settings::get_instance();

		string sql = "";
		sql.append("SELECT pg_attribute.atttypid, pg_type.typname ");
		sql.append("FROM pg_attribute, pg_type ");
		sql.append("WHERE ");
		sql.append("  pg_attribute.atttypid = pg_type.oid AND ");
		sql.append("  pg_attribute.attname = '" + columnName + "' AND ");
		sql.append("  (pg_attribute.atttypid < 26 OR pg_attribute.atttypid > 29) AND ");
		sql.append("  attrelid IN( SELECT pg_class.oid FROM pg_class, pg_namespace ");
		sql.append("    WHERE relname = '" + tableName + "' AND ");
		sql.append("          pg_class.relnamespace = pg_namespace.oid ");
		sql.append("  ); ");
		try {
			// DB接続
			connection Conn(("dbname=" + settings.getParameter("DATABASE_NAME") + " user=" + settings.getParameter("USER_NAME") + " password=" + settings.getParameter("DB_PASS")  + " host=" + settings.getParameter("DATABASE_ADDR") + " port=" + settings.getParameter("DATABASE_PORT")));
			work T(Conn);

			// SQLの実行(テーブル一覧取得)
			result R1(T.exec(sql));
			for (result::const_iterator c = R1.begin(); c != R1.end(); ++c) {
				if (userDefinedOidList.find(c[0].as(int())) == userDefinedOidList.end()) {
					userDefinedOidList[c[0].as(int())] = c[1].as(string());
				}
			}
			//Conn.disconnect();
		}
		catch (const exception &e) {
			cerr << e.what() << endl;
			return;
		}
	}

	/**
	* 指定したViewテーブルが参照するテーブル名を取得する
	*
	* @author	Nagoya University
	* @date	2020/02/05
	*
	* @param	T		トランザクション
	* @param[in]		tablename	指定テーブル名(View)
	* @param[in,out]	relationTableList	指定カラム名
	*/

	void InformationSourceManager::extractRelatedTables(work &T, const string &tableName, vector<string> &relationTableList)
	{
		result R2(T.exec("SELECT definition FROM pg_views WHERE viewname ='" + tableName + "';"));
		for (result::const_iterator c = R2.begin(); c != R2.end(); ++c) {
			Tokenizer* tokens = new Tokenizer(c[0].as(string()));
			while (tokens->hasMoreTokens()) {
				if (rdbSchemaMap.count(tokens->lookAheadToken()) != 0) {
					if (rdbSchemaMap[tokens->lookAheadToken()].getRelationType() != IS::Schema::relationType::VIEW) {
						relationTableList.push_back(tokens->lookAheadToken());
						logger->debug("[settingRdbSchema] View:" + tableName + " relationTable:" + tokens->lookAheadToken());
					}
					else {
						// 参照テーブルがViewだった場合は再帰的に参照テーブルを取得する
						extractRelatedTables(T, tokens->lookAheadToken(), relationTableList);
					}
				}
				tokens->getToken();
			}
			delete tokens;
		}
	}

	/**
	* ユーザ定義のOIDからデータ型名を返す
	*
	* @author	Nagoya University
	* @date	2018/05/09
	* @param	oid	データ型のID
	*
	* @return	データ型名
	*/

	string InformationSourceManager::getUserDefinedTypeNameByOid(int oid)
	{
		if (userDefinedOidList.find(oid) != userDefinedOidList.end()) {
			return userDefinedOidList[oid];
		}
		return "";
	}

	/**
	* OIDからテーブル名を返す
	*
	* @author	Nagoya University
	* @date	2020/01/21
	* @param	oid	オブジェクトID
	*
	* @return	テーブル名
	*/

	string InformationSourceManager::getTableNameByOid(int oid)
	{
		if (tableNameOidList.find(oid) != tableNameOidList.end()) {
			return tableNameOidList[oid];
		}
		return "";
	}
}
