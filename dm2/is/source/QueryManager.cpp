#include "QueryManager.h"

using namespace std;

namespace IS {

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param 		  	id   	クエリ管理番号
	 * @param 		  	sock 	ソケット
	 * @param 		  	addr 	アドレス
	 * @param 		  	query	クエリXML文字列
	 * @param [in,out]	qe   	クエリ実行処理オブジェクト
	 */

	QueryInfo::QueryInfo(unsigned int id, int sock, struct sockaddr_in addr, string query, IS::QueryExecuter *qe, bool isContinuousQuery)
	{
		this->queryId = id;
		this->sock = sock;
		this->addr = addr;
		this->query = query;
		this->QE = qe;
		this->isContinuousQuery = isContinuousQuery;
	}

	/**
	 * 初期化処理
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void QueryManager::init()
	{
		mp.clear();
		sockmng.clear();
		presetQueryList.clear();
		MNGID = 0;
		this->pid = DmUtil::getPID();

		pthread_mutex_init(&QImapMtx, NULL);
		pthread_mutex_init(&mngIdMtx, NULL);
		pthread_mutex_init(&sockMtx, NULL);
	}

	/**
	 * 実行処理
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void QueryManager::run()
	{
	}

	/**
	 * 処理の開始
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void QueryManager::start()
	{
		thread queuingThread(&QueryManager::run, this);
		queuingThread.detach();
	}

	/**
	 * 処理の停止
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void QueryManager::stop()
	{
		exit_flag = true;
		vector<unsigned int> mngIdList;
		string cancelId = "";

		pthread_mutex_lock(&QImapMtx);
		for (auto itr = mp.begin(); itr != mp.end(); ++itr) {
			mngIdList.push_back(itr->first);
			cancelId.append((cancelId.length() != 0 ? "," : "") + to_string(itr->first));
		}
		pthread_mutex_unlock(&QImapMtx);

		logger->debug("CANCEL(EXIT) LIST :" + cancelId);
		for (unsigned int mngId : mngIdList) {
			logger->debug("MNGID:" + std::to_string(mngId) + " CANCEL FOR EXIT");
			cancelQuery(mngId);
		}
		pthread_mutex_destroy(&QImapMtx);
		pthread_mutex_destroy(&mngIdMtx);
		pthread_mutex_destroy(&sockMtx);
	}

	/**
	 * クエリを解析する
	 *
	 * @author	Shinichi Kusayama
	 * @date	2023/3/21
	 *
	 * @param	query	クエリ
	 */
	string QueryManager::parseQuery(const string &query) {
		QueryParser *QP = NULL;
		try {
			QP = new IS::QueryParser(query);
		}
		catch (const IS::ParseException& ex) {
			string what(ex.what());
			string retErrMsg = "Query Parser Exception : " + what + " msg:" + ex.getMessage() + " query:" + query;
			logger->error(retErrMsg);
			if (QP != NULL) delete QP;
			return retErrMsg;
		}
		if (QP != NULL) delete QP;
		return "";
	}
	/**
	 * クエリを追加する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	user	ユーザ名
	 * @param	data	受信データ
	 */

	int QueryManager::getMngId()
	{
		// 管理番号を発行
		pthread_mutex_lock(&mngIdMtx);
		unsigned int mngId = ++MNGID;
		pthread_mutex_unlock(&mngIdMtx);

		return mngId;
	}

	/**
	 * クエリを追加する（Case1: アプリからの要求, Case2: CSからクエリ形式での要求, Case3: 別のaddQueryからの要求）
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	mngId			管理ID
	 * @param	user			ユーザ名
	 * @param	data			受信データ
	 * @param	returnMngId		管理IDを返却するか判定するフラグ
	 * @param	query_header_	クエリのヘッダ情報
	 */

	void QueryManager::addQuery(const int &mngId, const string &user, const RecvData &data, const bool &returnMngId, const query_header &query_header_)
	{
		bool startUpMode = false;
		if (user == "public") {
			string s = data.payload;
			stringUtil.toUpper(s);
			if (stringUtil.contain(s, "CREATE TIMER")) startUpMode = true;
		}
		// QueryExecuterに処理を委譲
		QueryExecuter *QE = new QueryExecuter(user, mngId, data, returnMngId, query_header_, startUpMode);
		QE->start();
		// QEオブジェクトの管理
		QueryInfo info(mngId, data.sock, data.client, data.payload, QE, query_header_.continuous);
		pthread_mutex_lock(&QImapMtx);
		mp.insert(std::pair<unsigned int, IS::QueryInfo>(mngId, info));
		pthread_mutex_unlock(&QImapMtx);
		if (query_header_.continuous) {
			pthread_mutex_lock(&sockMtx);
			vector<unsigned int> mngIdList;
			auto itr = sockmng.find(data.sock);
			if( itr != sockmng.end() ) {
				mngIdList = sockmng.at(data.sock);
			}
			mngIdList.push_back(mngId);
			sockmng[data.sock] = mngIdList;
			pthread_mutex_unlock(&sockMtx);
		}
		logger->debug("MNGID:" + std::to_string(mngId) + " QE MAP ADD");
		return;
	}
	/**
	 * クエリを追加する (プリセットクエリ登録用)
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	user	ユーザ名
	 * @param	data	受信データ
	 */
	void QueryManager::addQuery(const string &user, const RecvData &data, const query_header &query_header_)
	{
		int mngId = getMngId();
		addQuery(mngId, user, data, false, query_header_);
		presetQueryList.insert(std::pair<unsigned int, string>(mngId, user));
	}
	/**
	 * クエリを追加する (DMIからの登録用)
	 *
	 * @author	Shinichi Kusayama
	 * @date	2026/04/08
	 *
	 * @param	mngId			管理ID
	 * @param	user			ユーザ名
	 * @param	query			クエリ文字列
	 * @param	dmiName			DMIの名称
	 */
	void QueryManager::addQuery(const int &mngId, const string &user, const string &query, const string &dmiName)
	{
        RecvData info;
		info.sock = -1;
        struct sockaddr_in addr;
		bzero( &addr, sizeof(addr) );
		info.client = addr;
		info.payload = query;
		info.isClose = false;

		query_header query_header_;
		query_header_.dstSID = 0;
		query_header_.port = -1;
		query_header_.continuous = true;
		query_header_.dmiName = dmiName;
        addQuery(mngId, user, info, true, query_header_);
	}
	/**
	* オペレーターツリーからクエリを追加する
	*
	* @author	Nagoya University
	* @date	2019/03/06
	*
	* @param	user	ユーザ名
	* @param	data	受信データ
	*/

	void QueryManager::addOperatorTree(const string &user, const RecvData &data)
	{
		// 管理番号を発行
		pthread_mutex_lock(&mngIdMtx);
		unsigned int mngId = ++MNGID;
		pthread_mutex_unlock(&mngIdMtx);

		// QueryExecuterに処理を委譲
		QueryExecuter *QE = new QueryExecuter(user, mngId, data);
		QE->isOperatorTreeXML = true;
		QE->start();

		// QEオブジェクトの管理
		QueryInfo info(mngId, data.sock, data.client, data.payload, QE, true);
		pthread_mutex_lock(&QImapMtx);
		mp.insert(std::pair<unsigned int, IS::QueryInfo>(mngId, info));
		pthread_mutex_unlock(&QImapMtx);
		logger->debug("MNGID:" + std::to_string(mngId) + " QE MAP ADD");


		if (stringUtil.contain(settings.getParameter("HISTORY_RECORD_CLASS"), stringUtil.getClassName(typeid(*this)))) {
			createQueryHistory("add", mngId, data.sock, data.payload, "From OperatorTree", user);
		}
	}

	/**
	* CSからオペレータツリー形式でのクエリを受信時に追加する
	*
	* @author	Nagoya University
	* @date	2018/08/27
	*
	* @param	query	The query.
	*/

	void QueryManager::addQuery(const string &query)
	{
		// 管理番号を発行
		pthread_mutex_lock(&mngIdMtx);
		unsigned int mngId = ++MNGID;
		pthread_mutex_unlock(&mngIdMtx);

		// QueryExecuterに処理を委譲
		// TODO:user名
		QueryExecuter *QE = new QueryExecuter("public", mngId, query);
		QE->start();

		// QEオブジェクトの管理
		struct sockaddr_in addr;
		bzero( &addr, sizeof(addr) );
		QueryInfo info(mngId, 0, addr, query, QE, true);
		pthread_mutex_lock(&QImapMtx);
		mp.insert(std::pair<unsigned int, IS::QueryInfo>(mngId, info));
		pthread_mutex_unlock(&QImapMtx);
		logger->debug("MNGID:" + std::to_string(mngId) + " QE MAP ADD");

		if (stringUtil.contain(settings.getParameter("HISTORY_RECORD_CLASS"), stringUtil.getClassName(typeid(*this)))) {
			thread createThread(&QueryManager::createQueryHistory, this, "add", mngId, 0, query, "From CS", "System");
			createThread.detach();
		}
	}

	/**
	* リクエスト処理を追加する
	*
	* @author	Nagoya University
	* @date	2019/12/23
	*
	* @param	mngId	クエリ管理番号
	* @param	destSID	リクエスト先SID
	* @param	mngInfo	連携情報
	* @param[in]	edgeSIDListStr		連携先エッジSID文字列(カンマ区切り)
	*/

	//void QueryManager::addRequest(const unsigned int mngId, const unsigned int destSID, PosMng::MngInfo mngInfo, const string &edgeSIDListStr)
	void QueryManager::addRequest(const unsigned int mngId, const unsigned long long destSID, PosMng::MngInfo mngInfo, const string &edgeSIDListStr)
	{
		try {
			QueryInfo QI = mp.at(mngId);
			QI.QE->addRequest(destSID, mngInfo, edgeSIDListStr);

			logger->debug("[addRequest] MNGID:" + std::to_string(mngId) + " QE MAP UPDATE(Add Request to other edge(ID:" + to_string(destSID) + "))");
		}
		catch (std::out_of_range const &) {
			logger->error("[addRequest] Not Found QueryExecuter. MngId : " + std::to_string(mngId));
		}
		catch (...) {
			logger->error("[addRequest] Internal error. MngId : " + std::to_string(mngId));
		}
	}

	/**
	* CSから受信したクエリ結果(キャンセル時のクエリ管理番号)を登録する
	*
	* @author	Nagoya University
	* @date	2018/08/27
	*
	* @param	query	The query.
	*/

	void QueryManager::registerResult(const string &query)
	{
		IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		isp.init();
		unsigned int requestMngId = isp.getAttrValueFromRootTag("reqId", query);
		isp.finalize();
		try {
			QueryInfo QI = mp.at(requestMngId);
			QI.QE->registerCancelId(query);

			logger->debug("MNGID:" + std::to_string(requestMngId) + " QE MAP UPDATE(CancelID)");
		}
		catch (std::out_of_range const &) {
			logger->error("[registerResult] Not Found QueryExecuter. MngId : " + std::to_string(requestMngId));
		}
		catch (...) {
			logger->error("Internal error. MngId : " + std::to_string(requestMngId));
		}
		if (stringUtil.contain(settings.getParameter("HISTORY_RECORD_CLASS"), stringUtil.getClassName(typeid(*this)))) {
			createQueryHistory("cancel", requestMngId, 0, query, "From CS", "System");
		}
	}

	/**
	* 途中でエラーを返却する
	*
	* @author	Nagoya University
	* @date	2019/03/05
	*
	* @param	mngId	クエリ管理番号
	* @param	code	エラーコード
	* @param	msg	エラーメッセージ
	*/

	void QueryManager::returnError(const unsigned int mngId, const ErrorCode &code, const string &msg)
	{
		try {
			QueryInfo QI = mp.at(mngId);
			QI.QE->returnError(code, msg);
			logger->debug("MNGID:" + std::to_string(mngId) + " QE returnError");
		}
		catch (std::out_of_range const &) {
			logger->error("[returnError] Not Found QueryExecuter. MngId : " + std::to_string(mngId));
		}
	}


	/**
	* クエリをキャンセル（終了）し、レスポンスを返却する (アプリからのキャンセル要求。ユーザチェックあり)
	*
	* @author	Nagoya University
	* @date	2019/06/21
	*
	* @param	user	ユーザ名
	* @param	data	受信データ
	* @param	mngId	クエリ管理番号
	*/

	void QueryManager::cancelQuery(const string &user, const RecvData &data, const unsigned int mngId)
	{
		logger->debug("user:" + user + ", mngId" + to_string(mngId)); 
		bool ret = false;
		try {
			pthread_mutex_lock(&QImapMtx);
			QueryInfo QI = mp.at(mngId);
			
			//if (QI.isContinuousQuery) {
			ret = QI.QE->checkUser(user);
			if (ret == false) {
				string ip = inet_ntoa(data.client.sin_addr);
				if (ip == "127.0.0.1") {
					// プリセットクエリ対応
					ret = QI.QE->checkUser("public");
				}
			}
			pthread_mutex_unlock(&QImapMtx);
			if (ret == false) {
				string errMsg = "Query(MNGID:" + to_string(mngId) + ") is not registered by you(user:" + user + "). So couldn't execute cancel request due to permision error.";
				logger->warn(errMsg);
				IS::ResponseOperator		*opX = new IS::ResponseOperator(mngId, data, IS::ErrorCode::AUTHORITY_ERR, errMsg);
				// クエリキャンセル応答フラグを設定
				opX->currentResponseType = IS::ResponseOperator::responseType::RESPONSE_CANCEL;
				TupleSet tupleset;
				vector<TupleSet> ts;
				ts.push_back(tupleset);
				opX->process(ts);
				opX->exit();
				delete opX;
				return;
			}
		}
		catch (std::out_of_range const &) {
			pthread_mutex_unlock(&QImapMtx);
		}
		if (exit_flag) return;
		ret = cancelQuery(mngId);
		IS::ResponseOperator		*opX;
		if (ret) {
			opX = new IS::ResponseOperator(mngId, data);
		} else {
			string errMsg = "Query(MNGID:" + to_string(mngId) + ") is not found";
			opX = new IS::ResponseOperator(mngId, data, IS::ErrorCode::AUTHORITY_ERR, errMsg);
		}
		// クエリキャンセル応答フラグを設定
		opX->currentResponseType = IS::ResponseOperator::responseType::RESPONSE_CANCEL;
		TupleSet tupleset;
		vector<TupleSet> ts;
		ts.push_back(tupleset);
		opX->process(ts);
		opX->exit();
		delete opX;
		logger->info("Cancel MNGID:" + std::to_string(mngId));
	}

	/**
	 * クエリをキャンセル（終了）する （システム内部からの呼び出し。ユーザチェックなし）
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	mngId	クエリ管理番号
	 * @return	成功可否
	 */
	bool QueryManager::cancelQuery(const unsigned int mngId)
	{
		return cancelQuery(mngId, "System");
	}
	/**
	 * クエリをキャンセル（終了）する （システム内部からの呼び出し。ユーザチェックなし）
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	mngId	クエリ管理番号
	 * @return	成功可否
	 */
	bool QueryManager::cancelQuery(const unsigned int mngId, const string &user)
	{
		logger->debug("[cancelQuery] user:" + user + ", mngId:" + to_string(mngId)); 
		bool isContinuousQuery;
		try {
			pthread_mutex_lock(&QImapMtx);
			QueryInfo QI = mp.at(mngId);
			isContinuousQuery = QI.isContinuousQuery;
			if (isContinuousQuery) {
				while (true) {
					logger->debug("[cancelQuery] isExitRun: " + to_string(QI.QE->isExitRun));
					if (QI.QE->isExitRun == false) {
						usleep(100 * 1000);
					} else {
						QI.QE->exitExecuter();
						delete QI.QE;

						int delNum = mp.erase(mngId);
						logger->debug("[cancelQuery] MNGID:" + std::to_string(mngId) + " erase after mp.size:" + to_string(mp.size()) + " delNum:" + to_string(delNum));
						break;
					}
				}
			}
			pthread_mutex_unlock(&QImapMtx);
		}
		catch (std::out_of_range const &) {
			pthread_mutex_unlock(&QImapMtx);
			logger->warn("[cancelQuery] Not Found QueryExecuter. MngId : " + std::to_string(mngId));
			return false;
		}
		return true;
	}
	/**
	* クエリをキャンセル（終了）する (IS連携。ユーザチェックなし)
	*
	* @author	Nagoya University
	* @date	2019/12/24
	*
	* @param	mngId	クエリ管理番号
	* @param	receptionSID	受付SID
	* @param	senderSID	送信者SID
	*/

	//void QueryManager::cancelQuery(const unsigned int mngId, const int receptionSID, const int senderSID)
	void QueryManager::cancelQuery(const unsigned int mngId, const unsigned long long receptionSID, const unsigned long long senderSID)
	{
		string msg = "[cancelQuery] mngId:" + to_string(mngId) + " receptionSID:" + to_string(receptionSID) + " senderSID:" + to_string(senderSID);
		logger->debug(msg);
		IS::LocationManager &LM = IS::LocationManager::get_instance();
		unsigned int cancelMngId = 0;
		string mngKey = to_string(receptionSID) + "_" + to_string(mngId);
		if (LM.isExistTask(mngKey, cancelMngId)) {
			logger->debug("[cancelQuery] Found LM Task. Get CancelId:" + to_string(cancelMngId) + " mngKey:" + mngKey);
			LM.deleteTask(mngKey);

			try {
				pthread_mutex_lock(&QImapMtx);
				QueryInfo QI = mp.at(cancelMngId);
				QI.QE->settingCancellationRange(senderSID);
				QI.QE->exitExecuter();
				delete QI.QE;

				mp.erase(cancelMngId);
				pthread_mutex_unlock(&QImapMtx);

				logger->debug("MNGID:" + std::to_string(cancelMngId) + " QE MAP CANCEL");
			}
			catch (std::out_of_range const &) {
				pthread_mutex_unlock(&QImapMtx);
				logger->warn("[cancelQuery] Not Found QueryExecuter. MngId : " + std::to_string(cancelMngId));
			}
		}
		else {
			logger->warn("[cancelQuery] Not Found LM Task. Get CancelId:" + to_string(cancelMngId) + " mngKey:" + mngKey + " senderId:" + to_string(senderSID));

		}
		if (stringUtil.contain(settings.getParameter("HISTORY_RECORD_CLASS"), stringUtil.getClassName(typeid(*this)))) {
			createQueryHistory("cancel", cancelMngId, 0, "", msg, "System");
		}
	}
	/**
	 * クエリをキャンセル（終了）する（socketクローズ検知によるキャンセル。ユーザチェックなし）
	 *   socketの番号からmngIdを割り出し、あればキャンセルする。
	 * 
	 * @author	Shinichi Kusayama
	 * @date	2022/02/12
	 *
	 * @param	mngId	クエリ管理番号
	 * @return	成功可否
	 */
	void QueryManager::cancelQuery(const RecvData &data)
	{
		if( sockmng.find(data.sock) == sockmng.end() ) {
			return;
		}
		vector<unsigned int> mngIdList = sockmng.at(data.sock);
		for (unsigned int mngId : mngIdList) {
			if( mp.find(mngId) == mp.end() ) {
				continue;
			}
			if (presetQueryList.find(mngId) != presetQueryList.end() ) {
				// プリセットクエリを登録したアプリが終了しても、プリセットクエリ自体は動かし続ける。ユーザによる明示的なキャンセルはこの関数以外で行われる。
				continue;
			}
			logger->warn("[cancelQuery] MNGID:" + to_string(mngId) + " because Connection is Failed. Socket:" + to_string(data.sock));
			cancelQuery(mngId);
		}
		pthread_mutex_lock(&sockMtx);
		sockmng.erase(data.sock);
		pthread_mutex_unlock(&sockMtx);
	}

	/**
	* リクエスト処理を削除する
	*
	* @author	Nagoya University
	* @date	2019/12/23
	*
	* @param	mngId	クエリ管理番号
	* @param	destSID	リクエスト先SID
	* @param	mngInfo	連携情報
	*/

	//void QueryManager::cancelRequest(const unsigned int mngId, const unsigned int destSID, PosMng::MngInfo mngInfo)
	void QueryManager::cancelRequest(const unsigned int mngId, const unsigned long long destSID, PosMng::MngInfo mngInfo)
	{
		try {
			QueryInfo QI = mp.at(mngId);
			QI.QE->cancelRequest(destSID, mngInfo);

			logger->debug("[cancelRequest] MNGID:" + std::to_string(mngId) + " QE MAP UPDATE(Cancel Request to other edge(ID:" + to_string(destSID) + "))");
		}
		catch (std::out_of_range const &) {
			logger->error("[cancelRequest] Not Found QueryExecuter. MngId : " + std::to_string(mngId));
		}
		catch (...) {
			logger->error("[cancelRequest] Internal error. MngId : " + std::to_string(mngId));
		}
	}

	/**
	* リクエストオペレータを削除する
	*
	* @author	Nagoya University
	* @date	2020/01/06
	*
	* @param	mngId	クエリ管理番号
	* @param	destSID	リクエスト先SID
	*/

	//void QueryManager::removeRequestOp(const unsigned int mngId, const unsigned int destSID)
	void QueryManager::removeRequestOp(const unsigned int mngId, const unsigned long long destSID)
	{
		try {
			QueryInfo QI = mp.at(mngId);
			QI.QE->removeRequestOp(destSID);

			logger->debug("[removeRequestOp] MNGID:" + std::to_string(mngId) + " QE MAP UPDATE(Remove RequestOP edge(ID:" + to_string(destSID) + "))");
		}
		catch (std::out_of_range const &) {
			logger->error("[removeRequestOp] Not Found QueryExecuter. MngId : " + std::to_string(mngId));
		}
		catch (...) {
			logger->error("[removeRequestOp] Internal error. MngId : " + std::to_string(mngId));
		}
	}

	/**
	* ワンショットクエリかどうかを取得する
	*
	* @author	Nagoya University
	* @date	2019/6/24
	*
	* @return One-shot Queryの場合はtrue,継続クエリの場合はfalse
	*/

	bool QueryManager::isOneShot(const unsigned int mngId)
	{
		try {
			QueryInfo QI = mp.at(mngId);
			return QI.QE->isOneShotQuery();
		}
		catch (std::out_of_range const &) {
			logger->error("[isOneShot] Not Found QueryExecuter. MngId : " + std::to_string(mngId));
		}
		return false;
	}

	/**
	 * 定義済みクエリの登録処理
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	path	相対パス
	 *
	 */

	int QueryManager::registerPresetQueryUsingPath(const string &path, const bool &continuous)
	{
		int i, dirElements;
		string search_path;
		int registeredCnt = 0;

		struct stat stat_buf;
		struct dirent **namelist = NULL;

		// dirElements にはディレクトリ内の要素数が入る
		dirElements = scandir(path.c_str(), &namelist, NULL, NULL);
		IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		isp.init();
		if (dirElements == -1) {
			logger->error("[ERROR] Not found Path : " + path);
		}

		else {

			//ディレクトリかファイルかを順番に識別
			for (i = 0; i<dirElements; i += 1) {

				// "." と ".." を除く
				if ((strcmp(namelist[i]->d_name, ".\0") != 0) && (strcmp(namelist[i]->d_name, "..\0") != 0)) {

					//search_pathには検索対象のフルパスを格納する
					search_path = path + string(namelist[i]->d_name);
					logger->debug("search_path : " + search_path);
					// ファイル情報の取得の成功
					if (stat(search_path.c_str(), &stat_buf) == 0) {

						// ディレクトリだった場合の処理
						if ((stat_buf.st_mode & S_IFMT) == S_IFDIR) {
							continue;
						}
						//ファイルだった場合の処理
						else if(stringUtil.ends_with(search_path, ".xml")){
							string queryXML, query;
							isp.parseXmlFileToString(search_path, queryXML);
							if (queryXML.length() != 0) {
								isp.getQueryString(queryXML, query);
								RecvData info;
								info.sock = -1;
								info.payload = query;
								query_header query_header_;
								query_header_.dstSID = isp.getDestinationSID(queryXML);
								query_header_.port = -1;
								query_header_.continuous = continuous;
								query_header_.dmiName = "";
								addQuery("public", info, query_header_);
								logger->debug("Register Preset Query : " + queryXML + ", Path: " + search_path);
								registeredCnt++;
							}
						}
					}
					// ファイル情報の取得の失敗
					else {
						logger->error("File Read failed. search_path : " + search_path);
					}
				}
			}
		}
		isp.finalize();
		for (int i = 0; i < dirElements; i++) {
			free(namelist[i]);
		}
		free(namelist);
		return registeredCnt;
	}
	/**
	 * 定義済みクエリの登録処理
	 *
	 * @author	Shinichi Kusayama
	 * @date	2024/4/23
	 *
	 * @param	schema	スキーマ
	 * @param	window	ウィンドウ
	 *
	 */
	void QueryManager::registerPresetQueryUsingSchema(const string &schema, const string &window)
	{
		string _window = "1";
		if (window != "") _window = window;
		//string preset_query = "\<query\>\<destination\>90000000\<\/destination\>\<body\>MASTER " + schema + "SELECT * FROM " + schema " [rows 1]\<\/body\>\<\/query\>";
		string preset_query = "<query><destination>90000000</destination><body>MASTER " + schema + " SELECT * FROM " + schema + " [rows " + _window + "]</body></query>";
		RecvData info;
		info.sock = -1;
		info.payload = preset_query;
		query_header query_header_;
		query_header_.dstSID = 90000000;
		query_header_.port = -1;
		query_header_.continuous = true;
		query_header_.dmiName = "";
		addQuery("public", info, query_header_);
		return;
	}

	/**
	* クエリ登録数を取得する
	*
	* @author	Nagoya University
	* @date	2018/10/29
	*
	* @return クエリ登録数
	*/

	int QueryManager::getRegisterQueryNum()
	{
		return mp.size();
	}

	/**
	* クエリ稼動状況を取得する
	*
	* @author	Nagoya University
	* @date	2018/10/29
	*
	* @return クエリ稼動状況
	*/

	vector<QueryStatus> QueryManager::getQueryStatus()
	{
		vector<QueryStatus> qsList;
		for (auto i = mp.begin(); i != mp.end(); ++i) {
			QueryStatus qs = i->second.QE->getQueryStatus();
			qsList.push_back(qs);

			//cout << "mngId:" << qs.mngId << ",query:" << qs.query << ",user:" << qs.user << ",operatorTreeXML:" << qs.operatorTreeXML << ",operatorNum:" << qs.operatorNum << ",notifiedIntervalAVG:" << qs.notifiedIntervalAVG << endl;
			//cout << "totalProcessTimeAVG:" << qs.totalProcessTimeAVG << ",unprocessedDataNum:" << qs.unprocessedDataNum << ",latestOperatorName:" << qs.latestOperatorName << ",latestOpeProcAVG:" << qs.latestOpeProcAVG << endl;
			//cout << "inputStreamTupleNumPerSec:" << qs.inputStreamTupleNumPerSec << ",inputRelationTupleNumPerSec:" << qs.inputRelationTupleNumPerSec << ",outputTupleNumPerSec:" << qs.outputTupleNumPerSec << endl;
		}
		return qsList;
	}
	/**
	 * クエリ登録履歴をDBに登録する
	 *
	 * @author	Shinichi Kusayama
	 * @date	2022/02/11
	 */

	void QueryManager::createQueryHistory(string action, int mngId, int sock, string payload, string memo, const string &user)
	{
		long ts = DmUtil::getTimeMillisec();
		string query = "";
		string destination = "";
		
		if (action == "add") {
			query = stringUtil.getValueByXMLTag(payload, "body");
			destination = stringUtil.getValueByXMLTag(payload, "destination");
		}
		pthread_mutex_init(&createMtx, NULL);
		pthread_mutex_lock(&createMtx);
		try {
			// DB接続
			connection Conn(("dbname=" + settings.getParameter("DATABASE_IS_NAME") + " user=" + settings.getParameter("USER_NAME") + " password=" + settings.getParameter("DB_PASS") + " host=" + settings.getParameter("DATABASE_ADDR") + " port=" + settings.getParameter("DATABASE_PORT")));
			// クエリ登録履歴を登録
			work T(Conn);
			Conn.prepare("insert_query_history", "INSERT INTO query_history VALUES($1,$2,$3,$4,$5,$6,$7,$8,$9);");
			T.exec_prepared("insert_query_history", ts, action, this->pid, mngId, sock, destination, query, user, memo);
			T.commit();
			//Conn.disconnect();
		}
		catch (std::exception& e) {
			string what(e.what());
			string funcname = __func__;
			logger->warn(funcname + ", PostgreSQL Error: " + what);
		}
		pthread_mutex_unlock(&createMtx);

	}
}
