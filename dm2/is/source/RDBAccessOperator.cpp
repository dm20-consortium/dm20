#include "RDBAccessOperator.h"
#include "Settings.h"


#include <cctype>

using std::cout;		// cout
using std::endl;		// endl
using namespace std;

namespace IS {

	/**
	* PostgeSQLからのNotify通知を受信するクラス
	*
	* @author	Nagoya University
	* @date	2020/01/28
	*
	*/
	class notifyReceiver : pqxx::notification_receiver
	{
	private:
		std::function<void(string)> callback;
	public:
		notifyReceiver(pqxx::connection & conn, const std::string & channel, std::function<void(string)> func) : notification_receiver(conn, channel)
		{
			this->callback = func;
		}
		virtual void operator() (const std::string & payload, int pid)
		{
			this->callback(payload);
		}
	};

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param 	mngId	クエリ管理番号
	 * @param 	param	パラメータ
	 * @param	masterName	マスター句に指定されたストリーム名
	 */

	RDBAccessOperator::RDBAccessOperator(unsigned int mngId, string param, string masterName)
	{
		this->type = MyName + "_MNGID:" + std::to_string(mngId);
		this->mngId = mngId;
		this->argument = param;
		this->masterName = masterName;
		expect_data_size = 1;

		// キュー用メモリの用意
		if (this->masterName.length() > 0) {
			IS::QueueManager &QM = IS::QueueManager::get_instance();
			// マスター句に指定されたトリガーとなるストリームにリスナーを登録
			QM.addQueueListener(this->masterName, this);
			isCache = true;
		}

		// キャッシュ化しないテーブル名を取得
		vector<string> noCachedTableList;
		string noCacheTables = settings.getParameter("NOT_USED_CACHE_TABLE");
		if (noCacheTables.length() != 0) {
			stringUtil.toLower(noCacheTables);
			stringUtil.split(noCacheTables, ",", noCachedTableList);
		}

		// リネーム解決
		// TODO:サブクエリの結果をリネームする場合はクエリパーサー共に未対応
		IS::InformationSourceManager &ism = IS::InformationSourceManager::get_instance();
		map<string, Schema> rdbTableMap = ism.getRdbSchemaMap();
		Tokenizer* tokens = new Tokenizer(this->argument);
		string previousToken = "";
		bool fromOver = false;
		while (tokens->hasMoreTokens()) {
			tokenList.push_back(tokens->lookAheadToken());
			string tokenS = tokens->lookAheadToken();
			stringUtil.toLower(tokenS);
			// TODO: クエリパーサーからFromListを受け取ってキャッシュ化しないテーブルを含む場合はキャッシュしない対応
			// 暫定対応:キャッシュ化しないテーブル名文字列が存在した場合はキャッシュ化しない
			if (find(noCachedTableList.begin(), noCachedTableList.end(), tokenS) != noCachedTableList.end()) {
				isCache = false;
				logger->debug("[" + this->type + "] No used Cache. table:" + tokenS);
			}
			if (rdbTableMap.count(tokenS) != 0 && rdbTableMap[tokenS].getRelationType() == IS::Schema::relationType::VIEW) {
				relationTableList = rdbTableMap[tokenS].getRelationTableList();
			}

			if (tokens->compareSymbol(Symbol::AS)) {
				tokenList.push_back(tokens->lookAheadToken());
				renameMap[tokenList.at(tokenList.size() - 3)] = tokenList.at(tokenList.size() - 1);
				logger->trace("RENAME_MAP key:" + tokenList.at(tokenList.size() - 3) + " val:" + tokenList.at(tokenList.size() - 1));
			}
			else if (tokens->compareSymbol(Symbol::FROM)) {
				tokenList.push_back(tokens->lookAheadToken());
				fromOver = true;
			}
			else if (!fromOver && tokens->compareToken("*")) {
				// 初回FROM句以前の*を対象とする
				tokenList.push_back(tokens->lookAheadToken());
				isAsterisk = true;
			}
			tokens->getToken();
		}
		delete tokens;
		
		if (isCache) {
			// RDBからのNotifyを受信するためのスレッドを起動
			thread notifyReceiverThread(&RDBAccessOperator::createListenThread, this);
			notifyReceiverThread.detach();
		}

		/* SQLコマンド別に戻すTupleSetが異なるので何を戻すか判断する */
		if(!this->argument.empty()){
			vector<string> queryTokens = stringUtil.split(this->argument ,  " ");
			string cmd =  queryTokens.at(0);
			if (strcasecmp(cmd.c_str() , "insert") == 0){
					isCreateAffectedRow = true;
			}else if (strcasecmp(cmd.c_str() , "update") == 0){
					isCreateAffectedRow = true;
			}else if (strcasecmp(cmd.c_str() , "delete") == 0){
					isCreateAffectedRow = true;
			}else if (strcasecmp(cmd.c_str() , "create") == 0){
					isCreateResult = true;
			}else if (strcasecmp(cmd.c_str() , "drop") == 0){
					isCreateResult = true;
			}
		}

	}

	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	RDBAccessOperator::~RDBAccessOperator()
	{
		//cout << "JoinOperator destractor" << endl;
		if (this->masterName.length() > 0) {
			IS::QueueManager &QM = IS::QueueManager::get_instance();
			// マスター句に指定されたトリガーリスナーを削除
			QM.removeQueueListener(this->masterName, this);
			if (isCache) {
				isCache = false;
				int cnt = 0;
				while (!isExitListen) {
					// TriggerListenerの終了を待つ
					usleep(10 * 1000);
					logger->trace("[" + getType() + "][destoractor] loop cnt:" + to_string(cnt) + " isExitListen:" + to_string(isExitListen));
					cnt++;
					if (cnt > 30) {
						logger->warn("[" + getType() + "] destractor Timeout. loop cnt:" + to_string(cnt) + " isExitListen:" + to_string(isExitListen) + " so forcibly terminated.");
						break;
					}
				};
			}
			
		}
	}

	/**
	* 子オペレータからデータが要求された場合にコールされる処理。
	* requestParentProc()をコールする事でさらに上位の親が存在するかを確認し
	* 存在しない場合はtrueが返却され、自オペレータを起こす
	*
	* @author	Nagoya University
	* @date	2019/03/07
	*/

	void RDBAccessOperator::requestData(long referenceTime)
	{
		//REL_COMMENT logger->trace("[" + getType() + "][requestData] IN");
		if (requestParentProc(callerObj, referenceTime))
		{
			TupleSet ts;
			ts.info.type = TupleSet::OccurrenceType::pull;
			ts.info.recvTime = referenceTime;
			pthread_mutex_lock(&bufMtx);
			buffer.push_back(ts);
			pthread_mutex_unlock(&bufMtx);
			notify_one();
		}
		//REL_COMMENT logger->trace("[" + getType() + "][requestData] OUT");
	}

	/**
	* オペレータ処理
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param [in,out]	ts	タプルセット
	*
	* @return	正常にデータ処理を実施でき、次のオペレータに渡す際にtrue
	*/

	bool RDBAccessOperator::process(vector<IS::TupleSet>& ts)
	{
		logger->debug("[" + this->type + "] ========== RDBAccess START ========== ");

#if MEASURE_MODE == 1
		long startTime = DmUtil::getTimeMicrosec();
		long procTime = startTime;
#endif

		// DEBUG 与えられたタプル情報の出力
		printInputInfos(ts, this->argument);

		// 初回の場合はパラメータを解析する
		if (outputSchema.getAttributes().size() == 0) {
			// メインテーブル名の取得
			string tableName;
			vector<string> params;
			stringUtil.split(this->argument, " ", params);
			for (unsigned int i = 0; i < params.size(); i++)
			{
				if (strcasecmp(params.at(i).c_str(), "from") == 0) {
					tableName = stringUtil.split(params.at(i + 1), ",")[0];
					vector<string> delList = { ";","(" };
					stringUtil.deleteStrings(tableName, delList);
					break;
				}
			}

			// schemaの設定
			outputSchema = IS::Schema(tableName, IS::Schema::RDB);
		}

#if MEASURE_MODE == 1
		long now = DmUtil::getTimeMicrosec();
		double msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP1 parameter analytics processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

		if (isUpdate) {
			bool isCommit = true;
			try {
				// DB接続
				connection Conn(("dbname=" + settings.getParameter("DATABASE_NAME") + " user=" + settings.getParameter("USER_NAME") + " password=" + settings.getParameter("DB_PASS")  + " hostaddr=" + settings.getParameter("DATABASE_ADDR") + " port=" + settings.getParameter("DATABASE_PORT")));
				work T(Conn);

#if MEASURE_MODE == 1
				now = DmUtil::getTimeMicrosec();
				msec = (now - procTime) / 1000.0;
				logger->info("[" + this->type + "] STAT_STEP2 getting connection processing time: " + to_string(msec) + "[ms]");
				procTime = now;
#endif
				string query = this->argument;
				if (editQuery.length() != 0 && isEditSuccess) {
					query = editQuery;
				}
				// SQLの実行
				result R2(T.exec(query));

#if MEASURE_MODE == 1
				now = DmUtil::getTimeMicrosec();
				msec = (now - procTime) / 1000.0;
				logger->info("[" + this->type + "] STAT_STEP3 exec database access processing time: " + to_string(msec) + "[ms]");
				procTime = now;
#endif

				// 初回の場合はSchema情報を構成する
				if (outputSchema.getAttributes().size() == 0) {
					// カラム情報の取得
					unordered_map<string, string> tableList;
					string columnStr = "";
					string typeStr = "";
					result::const_iterator columns = R2.begin();
					for (unsigned int y = 0; y < columns.size(); y++) {
						if (ism.getTableNameByOid(R2.column_table(y)).length() != 0) {
							tableList[ism.getTableNameByOid(R2.column_table(y))] = ism.getTableNameByOid(R2.column_table(y));
						}
					}

					IS::Attribute attr;
					string tableName = "";
					string renameCol = "";
					for (unsigned int y = 0; y < columns.size(); y++) {
						if (columnStr.length() != 0 ) columnStr.append(",");
						if (typeStr.length() != 0) typeStr.append(",");
						
						tableName.clear();
						renameCol.clear();
						if (tableList.size() > 1) {
							// Tableが複数(Join)存在する場合
							tableName = ism.getTableNameByOid(R2.column_table(y));
							if (renameMap.find(tableName) != renameMap.end()) {
								for (auto itr = renameMap.begin(); itr != renameMap.end(); ++itr) {
									if (itr->second == R2.column_name(y)) {
										// カラム名がrenameされている場合はテーブル名を付与しない
										tableName = "";
										renameCol = itr->first + " AS " + itr->second;
										break;
									}
								}
								if (tableName.length() > 0) tableName = renameMap[tableName];
							}
						}
						if (tableName.length() > 0) {
							attr.name = tableName + "." + R2.column_name(y);
						}
						else {
							// Tableが単一の場合
							attr.name = R2.column_name(y);
						}
						attr.type = getTypeString(R2.column_type(y));
						//cout << R2.column_type(y) << "," << attr.type << endl;
						if (strcasecmp(attr.type.c_str(), "geometry") == 0 || strcasecmp(attr.type.c_str(), "geography") == 0) {
							existGeoData = true;
							columnStr.append("ST_AsEWKT(");
							if (renameCol.length() != 0)
								columnStr.append(renameCol);
							else
								columnStr.append(attr.name);
							columnStr.append(")");
						}
						else {
							if (renameCol.length() != 0)
								columnStr.append(renameCol);
							else
								columnStr.append(attr.name);
						}
						
						typeStr.append(attr.type);
						outputSchema.append(attr);
					}
					if (existGeoData) {
						if (isAsterisk) {
							for (unsigned int i = 0; i < tokenList.size(); i++) {
								if (tokenList.at(i) == "*") {
									tokenList.at(i) = columnStr;
									break;
								}
							}
						}
						else {
							int colNum = 0;
							string beforeRenameColName = "";
							for (Attribute attr : outputSchema.getAttributes()) {
								if (strcasecmp(attr.type.c_str(), "geometry") == 0 || strcasecmp(attr.type.c_str(), "geography") == 0) {
									beforeRenameColName.clear();
									for (auto i = renameMap.begin(); i != renameMap.end(); ++i) {
										if (i->second == attr.name) beforeRenameColName = i->first;
									}
									if (beforeRenameColName.length() != 0) {
										// リネームカラムの場合
										for (unsigned int i = 0; i < tokenList.size(); i++) {
											if (tokenList.at(i) == beforeRenameColName) {
												tokenList.at(i) = "ST_AsEWKT(" + beforeRenameColName + ")";
												colNum++;
												break;
											}
										}
									}
									else {
										for (unsigned int i = 0; i < tokenList.size(); i++) {
											if (tokenList.at(i) == attr.name) {
												tokenList.at(i) = "ST_AsEWKT(" + attr.name + ")";
												colNum++;
												break;
											}
										}
									}
								}
								else {
									colNum++;
								}
							}
							if (outputSchema.getAttributeSize() != colNum) {
								logger->warn("[" + this->type + "] column num is mismatch. schema.AttributeSize:" + to_string(outputSchema.getAttributeSize()) + " hitsize:" + to_string(colNum) + " So Execute ST_AsEWKT () for each line. columnStr:" + columnStr);
								isCommit = false;
							}
						}

						for (string str : tokenList) {
							editQuery.append(" " + str);
						}
						logger->debug("[" + this->type + "] EditQuery :" + editQuery);
						logger->debug("[" + this->type + "] TYPE   " + typeStr);
					}
				}
				
				retTupleSet.clearTuples();
				retTupleSet.SetSchema(outputSchema);

#if MEASURE_MODE == 1
				now = DmUtil::getTimeMicrosec();
				msec = (now - procTime) / 1000.0;
				logger->info("[" + this->type + "] STAT_STEP4 schema settings processing time: " + to_string(msec) + "[ms]");
				procTime = now;
#endif

				any val;
				if (existGeoData && !isEditSuccess) {
					try {
						if (isCommit) {
							result R3(T.exec(editQuery));
							R2 = R3;
							isEditSuccess = true;
						}
						else {
							getST_AsEWKT_forEachLine(T, R2, retTupleSet);
							isCommit = true;
						}
					}
					catch (const exception &e) {
						isCommit = false;
						string what(e.what());
						string retErrMsg = "[" + this->type + "] RDBAccess ERROR. cause:" + what;
						logger->warn(retErrMsg);
						logger->warn("[" + this->type + "] Org  Query:" + this->argument);
						logger->warn("[" + this->type + "] Edit Query:" + editQuery);
						logger->warn("[" + this->type + "] So Execute ST_AsEWKT () for each line.");
						work Tran(Conn);
						getST_AsEWKT_forEachLine(Tran, R2, retTupleSet);
						Tran.commit();
					}
				}
				if (retTupleSet.size() == 0) {
					IS::Tuple tuple(outputSchema.getAttributeSize());
					retTupleSet.resize(R2.size());
					unsigned int y = 0, idx = 0;
					for (result::const_iterator c = R2.begin(); c != R2.end(); ++c) {
						tuple.clearValue();
						for (y = 0; y < c.size(); y++)
						{
							bool isNull = false;
							if (c[y].is_null() || c[y].as(string()) == "{NULL}") isNull = true;
							stringUtil.getAnyValFromString(c[y].as(string()), outputSchema.getAttribute(y).type, val, isNull);
							tuple.setValue(y, val, 0, isNull);
						}
						retTupleSet.setTuple(idx++, tuple);
					}
				}

#if MEASURE_MODE == 1
				now = DmUtil::getTimeMicrosec();
				msec = (now - procTime) / 1000.0;
				logger->info("[" + this->type + "] STAT_STEP5 generate tupleset processing time: " + to_string(msec) + "[ms]");
				procTime = now;
#endif

				if(isCommit) T.commit();
				//Conn.disconnect();

#if MEASURE_MODE == 1
				now = DmUtil::getTimeMicrosec();
				msec = (now - procTime) / 1000.0;
				logger->info("[" + this->type + "] STAT_STEP6 close connection processing time: " + to_string(msec) + "[ms]");
				procTime = now;
#endif
				if (isCreateAffectedRow){
					setResponseTupleSet("affected_rows" , to_string(R2.affected_rows()));
				}else if(isCreateResult){
					setResponseTupleSet("result" , "true");
				}
			}
			catch (const exception &e) {
				string what(e.what());
				string retErrMsg = "[" + this->type + "] RDBAccess ERROR. cause:" + what;
				logger->error(retErrMsg);
				if(isCreateAffectedRow){
					setResponseTupleSet("affected_rows" , "-1");
				}else if(isCreateResult){
					setResponseTupleSet("result" , "false");
				}else{
					IS::QueryManager &QM = IS::QueryManager::get_instance();
					if (QM.isOneShot(mngId)) {
						QM.returnError(mngId, ErrorCode::QUERY_PARSE_ERR, retErrMsg);
					}
					return false;
				}			
			}
			// キャッシュする場合は次回以降アップデートしない(Triggerされるまでは)
			if (isCache) {
				isUpdate = false;
				if (ts.size() > 0) {
					cachedTime = ts.at(0).info.recvTime;
					logger->debug("[" + this->type + "] Cache ResultSet. Cache Time:" + to_string(cachedTime));
				}
			}
			
		}
		else {
			// Cacheを利用する
			logger->debug("[" + this->type + "] Use Cached ResultSet. Get Time:" + to_string(cachedTime));
		}

		if (ts.size() > 0) {
			retTupleSet.info = ts.at(0).info;
		}		

		ts.clear();
		ts.reserve(1);
		ts.push_back(retTupleSet);

		// DEBUG 抽出されたタプル情報の出力
		printOutputInfo(retTupleSet);

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - startTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP7 total processing time: " + to_string(msec) + "[ms]");
#endif
		logger->debug("[" + this->type + "] ========== RDBAccess  END  ========== ");
		return true;
	}

	/**
	 * Oidに合った型名を取得する
	 * 参考：https://hackage.haskell.org/package/postgresql-simple-0.2.2.0/docs/src/Database-PostgreSQL-Simple-BuiltinTypes.html
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	num	Oid
	 *
	 * @return	型名
	 */

	inline string RDBAccessOperator::getTypeString(int num)
	{
		switch (num) {
		case   16:
			return "Bool";
			break;
		case   17:
			return "ByteA";
			break;
		case   18:
			return "Char";
			break;
		case   19:
			return "Name";
			break;
		case   20:
			return "Int8";
			break;
		case   21:
			return "Int2";
			break;
		case   23:
			return "Int4";
			break;
		case   24:
			return "RegProc";
			break;
		case   25:
			return "Text";
			break;
		case   26:
			return "Oid";
			break;
		case   27:
			return "Tid";
			break;
		case   28:
			return "Xid";
			break;
		case   29:
			return "Cid";
			break;
		case  142:
			return "Xml";
			break;
		case  600:
			return "Point";
			break;
		case  601:
			return "LSeg";
			break;
		case  602:
			return "Path";
			break;
		case  603:
			return "Box";
			break;
		case  604:
			return "Polygon";
			break;
		case  628:
			return "Line";
			break;
		case  650:
			return "Cidr";
			break;
		case  700:
			return "Float4";
			break;
		case  701:
			return "Float8";
			break;
		case  702:
			return "AbsTime";
			break;
		case  703:
			return "RelTime";
			break;
		case  704:
			return "TInterval";
			break;
		case  705:
			return "Unknown";
			break;
		case  718:
			return "Circle";
			break;
		case  790:
			return "Money";
			break;
		case  829:
			return "MacAddr";
			break;
		case  869:
			return "Inet";
			break;
		case  1000:
			return "vector(bool)";
			break;
		case  1005:
			return "vector(int2)";
			break;
		case  1007:
			return "vector(int4)";
			break;
		case  1009:
			return "vector(text)";
			break;
		case  1014:
			return "Char";
			break;
		case  1015:
			return "vector(varchar)";
			break;
		case  1016:
			return "vector(int8)";
			break;
		case  1022:
			return "vector(double)";
			break;
		case 1042:
			return "BpChar";
			break;
		case 1043:
			return "VarChar";
			break;
		case 1082:
			return "Date";
			break;
		case 1083:
			return "Time";
			break;
		case 1114:
			return "Timestamp";
			break;
		case 1184:
			return "TimestampTZ";
			break;
		case 1186:
			return "Interval";
			break;
		case 1266:
			return "TimeTZ";
			break;
		case 1560:
			return "Bit";
			break;
		case 1562:
			return "VarBit";
			break;
		case 1700:
			return "Numeric";
			break;
		case 1790:
			return "RefCursor";
			break;
		case 2249:
			return "Record";
			break;
		case 2278:
			return "Void";
			break;
		case 2950:
			return "UUID";
			break;
		default:
			// 上記に定義されていないOidはユーザ定義と見做し、起動時に保持したユーザ定義データ情報から検索する
			string ret = ism.getUserDefinedTypeNameByOid(num);
			if (ret.length() > 0) {
				return ret;
			}
			logger->error("[" + this->type + "] Not defined Oid. So Can't convert type string. Oid:" + std::to_string(num));
			return "";
		}
	}

	/**
	* RDBからのTRIGGERをListenするための処理
	*
	* @author	Nagoya University
	* @date	2020/01/28
	*
	*/
	void RDBAccessOperator::createListenThread()
	{
		string channel = "dm2_db_update";
		connection Conn(("dbname=" + settings.getParameter("DATABASE_NAME") + " user=" + settings.getParameter("USER_NAME") + " password=" + settings.getParameter("DB_PASS")  + " hostaddr=" + settings.getParameter("DATABASE_ADDR") + " port=" + settings.getParameter("DATABASE_PORT")));

		notifyReceiver rec(Conn, channel, std::bind(&RDBAccessOperator::notifyFromRDB, this, std::placeholders::_1));
		
		while (isCache) {
			Conn.await_notification(0, 50 * 1000);
		}
		//Conn.disconnect();
		isExitListen = true;
		logger->debug("[" + this->type + "] [createListenThread] Trigger ListenThread END");
	}

	/**
	* RDBからのNotifyを受けるコールバック関数
	*
	* @author	Nagoya University
	* @date	2020/01/28
	*
	* @param	table	更新されたテーブル名
	*/
	void RDBAccessOperator::notifyFromRDB(string table)
	{
		logger->trace("[" + this->type + "][notifyFromRDB] Notify update. payload:" + table);
		for (string token : tokenList) {
			stringUtil.toLower(token);
			// 参照テーブルに通知されたテーブルが存在するかをチェックする
			if (token == table) {
				isUpdate = true;
				logger->debug("[" + this->type + "][notifyFromRDB] Notify update. table:" + table);
				break;
			}
		}
		if (find(relationTableList.begin(), relationTableList.end(), table) != relationTableList.end()) {
			isUpdate = true;
			logger->debug("[" + this->type + "][notifyFromRDB] Notify update. relationTable:" + table);
		}
	}

	/**
	* resuponse用TupleSetを生成する
	*
	* @author	Nagoya University
	* @date	2022/02/24
	*
	* @param[in]	生成するカラム名称
	* @param[in]	リザルト
	*/
	void RDBAccessOperator::setResponseTupleSet(const string columnName , const string returnValue)
	{
		Attribute attribute;
		std::vector<Attribute> attrs;
		IS::Tuple tuple(1);
		retTupleSet.resize(1);
		tuple.clearValue();
		tuple.setValue(0, returnValue , 0);
		retTupleSet.setTuple(0,tuple);

		attribute.name = columnName;
		attribute.type = "integer";
		attrs.push_back(attribute);
		retTupleSet.SetSchema(IS::Schema(columnName, attrs , IS::Schema::RDB));;
	
		return ;
	}
	/**
	* ST_AsEWKTを各行ずつ実行し、TupleSetを生成する
	*
	* @author	Nagoya University
	* @date	2020/01/28
	*
	* @param[in,out]	T	トランザクション
	* @param[in]	R	リザルト
	* @param[in,out]	ts	タプルセット
	*/
	void RDBAccessOperator::getST_AsEWKT_forEachLine(work &T, const result &R, IS::TupleSet &ts)
	{
		any val;
		retTupleSet.resize(R.size());
		IS::Tuple tuple(outputSchema.getAttributeSize());
		unsigned int y = 0, idx = 0;
		for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
			tuple.clearValue();
			for (y = 0; y < c.size(); y++)
			{
				if (strcasecmp(outputSchema.getAttribute(y).type.c_str(), "geometry") == 0 || strcasecmp(outputSchema.getAttribute(y).type.c_str(), "geography") == 0)
				{
					// SQLの実行
					result R3(T.exec("SELECT ST_AsEWKT('" + c[y].as(string()) + "');"));
					// データ部の取得
					for (result::const_iterator c = R3.begin(); c != R3.end(); ++c) {
						val = c[0].as(string());
					}
				}
				else
				{
					stringUtil.getAnyValFromString(c[y].as(string()), outputSchema.getAttribute(y).type,val);
				}
				tuple.setValue(y, val, 0, c[y].is_null());
			}
			ts.setTuple(idx++, tuple);
		}
	}

}
