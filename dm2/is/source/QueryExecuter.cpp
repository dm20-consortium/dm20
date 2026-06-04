
#include "QueryExecuter.h"

#include "ProjectionOperator.h"
#include "SelectionOperator.h"
#include "RenameTableOperator.h"
#include "RenameColumnOperator.h"
#include "DropOperator.h"
#include "EvalOperator.h"
#include "JoinOperator.h"
#include "UnionOperator.h"
#include "RDBAccessOperator.h"
#include "RecursiveOperator.h"
#include "ResponseOperator.h"
#include "RequestOperator.h"
#include "WindowOperator.h"
#include "TransferOperator.h"
#include "CreateOperator.h"
#include "QueueManager.h"
#include "QueryManager.h"
#include "DmiOperator.h"

#include <iostream>
#include <thread>

using namespace std;

namespace IS {

	/**
	 * コンストラクタ (アプリ / CS経由でクエリが転送された場合)
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	user	ユーザ名
	 * @param	mngId	クエリ管理番号
	 * @param	data	受信データ
	 */

	QueryExecuter::QueryExecuter(const string &user, const unsigned int mngId, const RecvData &data, const bool &doReturn, const query_header &_query_header)
	{
		this->user = user;
		this->recvInfo = data;
		this->mngId = mngId;
		this->doReturn = doReturn;
		this->query_header_ = _query_header;
		pthread_mutex_init(&opListMtx, NULL);
	}

	/**
	 * コンストラクタ (アプリ経由でオペレータツリーが転送された場合)
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	user	ユーザ名
	 * @param	mngId	クエリ管理番号
	 * @param	data	受信データ
	 */

	QueryExecuter::QueryExecuter(const string &user, const unsigned int mngId, const RecvData &data)
	{
		this->user = user;
		this->recvInfo = data;
		this->mngId = mngId;
		this->doReturn = true;
		pthread_mutex_init(&opListMtx, NULL);
	}
	/**
	* コンストラクタ (CS経由でオペレータツリーが転送された場合)
	*
	* @author	Nagoya University
	* @date	2018/08/24
	*
	* @param	user	ユーザ名
	* @param	mngId	クエリ管理番号
	* @param	query	XML文字列
	*/

	QueryExecuter::QueryExecuter(const string &user, const unsigned int mngId, const string query)
	{
		this->user = user;
		this->recvInfo.payload = query;
		this->mngId = mngId;
		this->isFromCS = true;
		pthread_mutex_init(&opListMtx, NULL);
	}

	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	QueryExecuter::~QueryExecuter()
	{
		pthread_mutex_destroy(&opListMtx);
	}

	/**
	 * 初期化処理
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void QueryExecuter::init()
	{

	}

	/**
	 * クエリ実行スレッド処理
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void QueryExecuter::run()
	{
		//REL_COMMENT logger->trace("[run] IN MngId:" + std::to_string(this->mngId));
		
#if MEASURE_MODE == 1
		long startTime = DmUtil::getTimeMicrosec();
		long procTime = startTime;
		int step = 1;
		long now = 0;
		double msec = 0;
#endif
		bool isErr = false;
		IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		IS::QueryParser *QP = NULL;
		isp.init();
		try {
			Operator* startOp = NULL;
			if ( !this->isFromCS ) {
				if (isOperatorTreeXML) {
					isp.getOperatorTreeXML(this->recvInfo.payload, parseResult);
					logger->debug("MNGID:" + std::to_string(this->mngId) + ", OPERATOR_TREE(ADMIN):" + parseResult);

					if (parseResult.find("<data key=\"operator_type\">in</data>") == std::string::npos) {
						isOneShot = true;
					}
				}
				else {
					if (this->recvInfo.payload == "") {
						isErr = true;
					} else {
						logger->info("MNGID:" + std::to_string(this->mngId) + ", destSID:" + std::to_string(query_header_.dstSID) + ", QUERY:" + this->recvInfo.payload);
						// 継続クエリ判定暫定対応
						if (!query_header_.continuous) isOneShot = true;
						try {
							// クエリ解析器にてクエリを解析する
							QP = new IS::QueryParser(this->recvInfo.payload);
							parseResult = QP->getGraphListXML();
							logger->debug("MNGID:" + std::to_string(this->mngId) + ", OPERATOR_TREE():" + parseResult);

						}
						catch (const IS::ParseException& ex) {
							string what(ex.what());
							retErrMsg = "MNGID:" + std::to_string(this->mngId) + " Query Parser Exception : " + what + " msg:" + ex.getMessage() + " query:" + this->recvInfo.payload;
							logger->error(retErrMsg);
							if (isOneShot) {
								parseResult = rdbXmlPre + this->recvInfo.payload + rdbXmlSur;
							} else {
								isErr = true;
								returnError(ErrorCode::QUERY_PARSE_ERR, retErrMsg);
							}
						}
					}
				}
			}
			else {
				// CSからの処理委譲(クエリ転送)の場合
				this->requestedMngId = isp.getAttrValueFromRootTag("id", this->recvInfo.payload);
				isp.getOperatorTreeXML(this->recvInfo.payload, parseResult);
				logger->debug("MNGID:" + std::to_string(this->mngId) + ", OPERATOR_TREE(CS):" + parseResult);
			}
			if (!isErr) {
				// オペレータツリーの構築
				startOp = parseOperatorTree(parseResult);
				if (startOp == NULL) {
					isErr = true;
					logger->error("MNGID:" + std::to_string(this->mngId) + " parseOperatorTree Error ");
					returnError(retErrCode, retErrMsg);
					IS::QueryManager &QeryM = IS::QueryManager::get_instance();
					QeryM.cancelQuery(this->mngId);
				}
			}

#if MEASURE_MODE == 1
			now = DmUtil::getTimeMicrosec();
			msec = (now - procTime) / 1000.0;
			logger->info("[QE(MNGID:" + to_string(this->mngId) + ")] STAT_STEP" + to_string(step++) + " parseOperatorTree processing time: " + to_string(msec) + "[ms]");
			procTime = now;
#endif
			if (!isErr) {
				startOperator();
				if (isOneShot) {
					TupleSet ts;
					ts.info.recvTime = DmUtil::getTimeMicrosec();
					Schema schema("one-shot", Schema::RDB);
					ts.SetSchema(schema);
					startOp->receiveTupleSet(ts);
				}
				else {
					if (recvInfo.sock != -1) {
						if (this->isFromCS) {
							// 送信元SIDと送信先SIDとデータ要求範囲を取得
							int range;
							unsigned long long senderId, destId, requestVehicleId, receptionEdgeId;
							unsigned int receptionMngId = 0;
							string edgeSIDsStr = "";
							isp.getRequestInfo(this->recvInfo.payload, senderId, destId, requestVehicleId, range, receptionMngId, receptionEdgeId, edgeSIDsStr);
							if (range != 0) {
								runForNearBy(range, senderId, destId, requestVehicleId, receptionEdgeId, receptionMngId, edgeSIDsStr);
							}
						} else {
							// Request電文でなく、返却フラグONの場合、通常のアプリからのリクエストにつき、管理番号を返却
							if (this->isExistsRequest == false && this->doReturn == true) {
								returnMngID(this->mngId);
							}
						}
					}
				}
			}
		}
		catch (const std::invalid_argument& ex) {
			string what(ex.what());
			retErrMsg = "MNGID:" + std::to_string(this->mngId) + " Internal Exception :" + what;
			logger->error(retErrMsg);
			returnError(ErrorCode::UNKNOWN_ERR, retErrMsg);
		}
		catch (const std::exception& ex) {
			string what(ex.what());
			retErrMsg = "MNGID:" + std::to_string(this->mngId) + " QueryExecuter.run() Unknown Exception :" + what;
			logger->error(retErrMsg);
			returnError(ErrorCode::UNKNOWN_ERR, retErrMsg);
		}
		// 終了処理
		isp.finalize();
		if (QP != NULL) delete QP;
		isExitRun = true;
#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - startTime) / 1000.0;
		logger->info("[QE(MNGID:" + to_string(this->mngId) + ")] STAT_STEP" + to_string(step++) + " total processing time: " + to_string(msec) + "[ms]");
#endif
	}
	/**
	 * NEAR BY 用処理 (2020年度実装処理をメソッド化)
	 *
	 * @author	Shinichi Kusayama
	 * @date	2025/08/14
	 * @param [in]	range	         レンジ
	 * @param [in]	senderId	     送信元SID
	 * @param [in]	destId	         宛先SID
	 * @param [in]	requestVehicleId 要求車両ID
	 * @param [in]	receptionEdgeId	 受付エッジID
	 * @param [in]	receptionMngId	 受付管理ID
	 * @param [in]	edgeSIDsStr	     エッジSIDs
	 * 
	 */
	bool QueryExecuter::runForNearBy(int range, unsigned long long senderId, unsigned long long destId, unsigned long long requestVehicleId, unsigned long long receptionEdgeId, unsigned int receptionMngId, string edgeSIDsStr)
	{
		// NEARBY指定かつデータ要求範囲を指定している場合
		// LocationManagerに処理を登録
		logger->info("[run] Request from another IS. senderId:" + to_string(senderId) + " destId:" + to_string(destId) + " requestVehicleId:" + to_string(requestVehicleId) + " range:" + to_string(range) + " mngId:" + to_string(mngId) + " reception (edgeID:" + to_string(receptionEdgeId) + " mngId:" + to_string(receptionMngId) + ")");
		IS::LocationManager &LM = IS::LocationManager::get_instance();

		// 宛先が0(最寄エッジ)の場合は、自身のクエリ管理番号を車両に返却するため
		// 車両はエッジではこのクエリ番号しか稼動していないように見えるが
		// 移動先にて同じ転送クエリをそれぞれ別エッジのクエリ管理番号体系で展開しないといけないため、
		// 最寄エッジで払い出したクエリ管理番号で全エッジ共通で削除出来るように受付クエリ管理番号を設けた
		if (destId == 0) {
			receptionMngId = mngId;
			requestVehicleId = senderId;
			//receptionEdgeId = stoi(settings.getParameter("MY_STATION_ID"));
			receptionEdgeId = stoull(settings.getParameter("MY_STATION_ID"));
		}
		mngKey = to_string(receptionEdgeId) + "_" + to_string(receptionMngId);
		unsigned int cancelMngId = 0;
		if (LM.isExistTask(requestVehicleId, mngKey, cancelMngId)) {
			logger->info("[run] Task is already exist.(VehicleID:" + to_string(requestVehicleId) + " mngKey:" + mngKey + ") So return MngID(ID:" + to_string(cancelMngId) + ") and clear MngID(ID:" + to_string(this->mngId) + ").");
			returnMngID(cancelMngId);
			IS::QueryManager &QeryM = IS::QueryManager::get_instance();
			QeryM.cancelQuery(this->mngId);
			return false;
		}
		PosMng posMng(parseResult, range, requestVehicleId, receptionMngId, receptionEdgeId);

		if (edgeSIDsStr.length() == 0) edgeSIDsStr.append(settings.getParameter("MY_STATION_ID"));
		for (string edgeSID : stringUtil.split(edgeSIDsStr, ",")) {
			PosMng::TranInfo tranInfo;
			tranInfo.edgeSID = stoi(edgeSID);
			if (edgeSID == settings.getParameter("MY_STATION_ID")) {
				tranInfo.mngID = mngId;
			}
			else {
				tranInfo.mngID = 0;
			}
			posMng.Info.at(mngKey).tranInfoList.push_back(tranInfo);
		}
		LM.addLocationManagementTask(requestVehicleId, posMng);

		if (destId == 0) {
			// 車両から要求を受けたエッジが初回の連携依頼を実施する
			// エッジから連携依頼を受けた場合はdestIdがエッジのSIDとなり、
			// destID != 0の場合は、プローブ情報からの監視処理にて別エッジに連携依頼を実施する
			//unordered_map<unsigned int, unsigned int> edgeSIDmap = LM.getNearEdgeSID(requestVehicleId, mngKey);
			unordered_map<unsigned long long, unsigned long long> edgeSIDmap = LM.getNearEdgeSID(requestVehicleId, mngKey);

			// データ要求範囲のエッジに対して連携要求
			TupleSet tupleset;
			vector<TupleSet> ts;
			ts.push_back(tupleset);
			string edgeSIDListStr = "";
			for (auto itr = edgeSIDmap.begin(); itr != edgeSIDmap.end(); ++itr) {
				if (edgeSIDListStr.length() != 0) edgeSIDListStr.append(",");
				edgeSIDListStr.append(to_string(itr->first));
			}
			for (auto itr = edgeSIDmap.begin(); itr != edgeSIDmap.end(); ++itr) {
				if (itr->first != static_cast<unsigned int>(stoi(settings.getParameter("MY_STATION_ID")))) {
					PosMng::TranInfo tranInfo;
					tranInfo.edgeSID = itr->first;
					tranInfo.mngID = 0;		// まだ不明なため0で登録
					LM.addTranInfo(requestVehicleId, mngKey, tranInfo);
					Operator* request = new RequestOperator(posMng.Info.at(mngKey).receptionMngId, itr->first, posMng.Info.at(mngKey).tranOperatorTreeXML, requestVehicleId, posMng.Info.at(mngKey).range, receptionMngId, receptionEdgeId, edgeSIDListStr);

					addOperator(request);
					request->process(ts);
					this->isExistsRequest = true;
				}
			}
		}
		return true;
	}
	/**
	 * クエリ実行処理の終了
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void QueryExecuter::exitExecuter()
	{
		pthread_mutex_lock(&opListMtx);
		for (IS::Operator* op : opeList) {
			op->exit();
		}
		for (IS::Operator* op : opeList) {
			delete op;
		}
		pthread_mutex_unlock(&opListMtx);
	}

	/**
	 * クエリ実行処理の開始
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void QueryExecuter::start()
	{
		thread queuingThread(&QueryExecuter::run, this);
		queuingThread.detach();
	}

	/**
	 * クエリ実行処理の停止
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void QueryExecuter::stop()
	{

	}

	/**
	 * オペレータ間のリレーション情報を設定する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in,out]	from	連携元オペレータ
	 * @param [in,out]	to  	連携先オペレータ
	 */

	inline void QueryExecuter::relationingOperator(IS::Operator* from, IS::Operator* to)
	{
		from->addListener(to);
		to->addParent(from);
	}

	/**
	 * 各オペレータ処理の開始
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	inline void QueryExecuter::startOperator()
	{
		pthread_mutex_lock(&opListMtx);
		// 各オペレータの起動処理
		for (IS::Operator* op : opeList) {
			op->start();
		}

		// 各オペレータの起動を待つ
		for (IS::Operator* op : opeList) {
			while (!op->isReady()) { usleep(1 * 1000); }
		}
		pthread_mutex_unlock(&opListMtx);
	}

	/**
	* オペレータの追加
	*
	* @author	Nagoya University
	* @date	2020/01/07
	*/

	inline void QueryExecuter::addOperator(Operator* ope)
	{
		pthread_mutex_lock(&opListMtx);
		opeList.push_back(ope);
		pthread_mutex_unlock(&opListMtx);
	}

	/**
	 * クエリ管理番号の返却処理
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	mngId	クエリ管理番号
	 */

	inline void QueryExecuter::returnMngID(unsigned int mngId)
	{
		TupleSet tupleset;
		vector<TupleSet> ts;
		ts.push_back(tupleset);

		// ISLIBからの通常時
		if (!this->isFromCS) {
			logger->debug("[returnMngID] IN Use ResponseOperator. MyMngId:" + std::to_string(mngId));
			IS::ResponseOperator		*opX = new IS::ResponseOperator(mngId, recvInfo);
			// クエリ登録応答フラグを設定
			opX->currentResponseType = ResponseOperator::responseType::RESPONSE_QUERY;
			opX->process(ts);
			opX->exit();
			delete opX;
		}
		// CSからの連携時
		else {
			IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
			isp.init();
			unsigned long long destId = isp.getSenderSID(this->recvInfo.payload);
			unsigned long long reqDestId = isp.getDestinationSID(this->recvInfo.payload);
			isp.finalize();
			// 下記Protobuf用
			//unsigned long long destId = this->recvInfo.senderSID;
			//unsigned long long reqDestId = this->recvInfo.destinationSID;
			logger->debug("[returnMngID] IN Use TransferOperator. DestSID:" + std::to_string(destId) + " MyMngId:" + std::to_string(mngId) + " reqDestId(MySID):" + std::to_string(reqDestId) + " reqMngId:" + std::to_string(this->requestedMngId));
			IS::TransferOperator		*opX = new IS::TransferOperator(mngId, destId, reqDestId, this->requestedMngId, 0, 0);
			opX->process(ts);
			opX->exit();
			delete opX;
		}
		//REL_COMMENT logger->trace("[returnMngID] OUT");
	}

	/**
	 * エラー返却処理
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	code	The code.
	 */

	void QueryExecuter::returnError(const ErrorCode &code, const string &msg)
	{
		TupleSet tupleset;
		vector<TupleSet> ts;
		ts.push_back(tupleset);

		// ISLIBからの通常時
		if (!this->isFromCS) {
			IS::ResponseOperator		*opX = new IS::ResponseOperator(mngId, recvInfo, code, msg);
			opX->process(ts);
			opX->exit();
			delete opX;
		}
		// CSからの連携時
		else {
			IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
			isp.init();
			int destId = isp.getSenderSID(this->recvInfo.payload);
			int reqDestId = isp.getDestinationSID(this->recvInfo.payload);
			isp.finalize();
			IS::TransferOperator		*opX = new IS::TransferOperator(mngId, destId, reqDestId, this->requestedMngId, 0, 0, code);
			opX->process(ts);
			opX->exit();
			delete opX;
		}
	}

	/**
	 * オペレーターツリーを解析し、各オペレータを生成する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	operatorTreeXml	オペレーターツリーXML文字列
	 *
	 * @return	最初にキックすべきオペレータオブジェクト.
	 */

	inline Operator* QueryExecuter::parseOperatorTree(const string &operatorTreeXml)
	{
		//REL_COMMENT logger->trace("[parseOperatorTree] IN");
		map<std::string, Operator*> opeMap;
		Operator* firstOpe = NULL;

		XMLCh tempStr[operatorTreeXml.length() + 1];
		static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
		DOMImplementationLS *impl = (DOMImplementationLS*)DOMImplementationRegistry::getDOMImplementation(gLS);
		DOMLSParser       *domBuilder = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
		DOMLSInput        *input = impl->createLSInput();

		XMLString::transcode(operatorTreeXml.c_str(), tempStr, operatorTreeXml.length());
		input->setStringData(tempStr);
		IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		isp.init();
		try
		{
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMComments, false);
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMDatatypeNormalization, true);
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMEntities, false);
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, true);
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMElementContentWhitespace, false);
			DOMDocument* doc = domBuilder->parse(input);
			
			// verify that we get only 3 calls: for the text node, the CDATA section and the root element
			if (doc == NULL || doc->getDocumentElement() == NULL)
			{
				retErrMsg = "[parseOperatorTree] document is Null Line:" + std::to_string(__LINE__);
				logger->error(retErrMsg);
				outputXmlLog(doc);
				return firstOpe;
			}

			DOMElement* root = doc->getDocumentElement();

			// キー解析
			XMLString::transcode("key", tempStr, isp.BUF_MAX - 1);
			if (root->getElementsByTagName(tempStr) == NULL)
			{
				retErrMsg = "[parseOperatorTree] <key></key> is not defined";
				logger->error(retErrMsg);
				outputXmlLog(doc);
				return firstOpe;
			}
			DOMNodeList* keyList = root->getElementsByTagName(tempStr);

			for (unsigned int i = 0; i < keyList->getLength(); i++)
			{
				string id, forStr, attrName, attrType;
				isp.getTagValue(dynamic_cast<DOMElement*>(keyList->item(i)), "id", id);
				isp.getTagValue(dynamic_cast<DOMElement*>(keyList->item(i)), "for", forStr);
				isp.getTagValue(dynamic_cast<DOMElement*>(keyList->item(i)), "attr.name", attrName);
				isp.getTagValue(dynamic_cast<DOMElement*>(keyList->item(i)), "attr.type", attrType);
				logger->debug("id:" + id + " for:" + forStr + " attrName:" + attrName + " attrType:" + attrType);
			}

			// グラフ解析(オペレーターツリー単位)
			XMLString::transcode("graph", tempStr, isp.BUF_MAX - 1);
			if (root->getElementsByTagName(tempStr) == NULL)
			{
				retErrMsg = "[parseOperatorTree] <graph></graph> is not defined";
				logger->error(retErrMsg);
				outputXmlLog(doc);
				return firstOpe;
			}
			DOMNodeList* graphList = root->getElementsByTagName(tempStr);
			DOMNode* parentClone = graphList->item(0)->getParentNode()->cloneNode(true);
			DOMNodeList* graphListCopy = dynamic_cast<DOMElement*>(parentClone)->getElementsByTagName(tempStr);

			if (graphList->getLength() == 0) {
				retErrMsg = "[parseOperatorTree] graph element is not exist.";
				logger->error(retErrMsg);
				outputXmlLog(doc);
				return firstOpe;
			}

			// ノード解析(オペレータ単位)
			XMLString::transcode("node", tempStr, isp.BUF_MAX - 1);
			if (dynamic_cast<DOMElement*>(graphList->item(0))->getElementsByTagName(tempStr) == NULL)
			{
				retErrMsg = "[parseOperatorTree] <node></node> is not defined";
				logger->error(retErrMsg);
				outputXmlLog(doc);
				return firstOpe;
			}
			DOMNodeList* nodeList = dynamic_cast<DOMElement*>(graphList->item(0))->getElementsByTagName(tempStr);

			if (nodeList->getLength() == 0) {
				retErrMsg = "[parseOperatorTree] node element is not exist.";
				logger->error(retErrMsg);
				outputXmlLog(doc);
				return firstOpe;
			}

			// クエリ解析結果にprojectionもしくはjoinが含まれていた場合は、応答結果が可変となる
			bool isDynamicColumn = operatorTreeXml.find("projection") != string::npos || operatorTreeXml.find("join") != string::npos;

			WindowOperator* inOpe = nullptr;
			ResponseOperator* resOpe = nullptr;
			string streamName;
			for (unsigned int i = 0; i < nodeList->getLength(); i++)
			{
				string nodeId;
				isp.getTagValue(dynamic_cast<DOMElement*>(nodeList->item(i)), "id", nodeId);
				DOMNodeList* dataList = isp.getElementByCharTagName(dynamic_cast<DOMElement*>(nodeList->item(i)), (char*)"data");
				Operator* opeObj = getOperator(dataList, graphList, nodeId, isDynamicColumn);
				if (opeObj != NULL) {
					// WindowOperatorからテーブル名を取り出して、ResponseOperatorに設定する
					if (auto ope = dynamic_cast<WindowOperator*>(opeObj)) {
						inOpe = ope;
						streamName = inOpe->getTargetStreamName();
						// 既にResponseOperatorが出現していた場合は設定する
						if (resOpe != nullptr)
							resOpe->protobufMessageName = streamName;
					} else if (auto ope = dynamic_cast<ResponseOperator*>(opeObj)) {
						resOpe = ope;
						// WindowOperatorから取り出したテーブル名を設定
						if (streamName != "")
							resOpe->protobufMessageName = streamName;
					}
					opeObj->setNodeId(nodeId);
					opeMap[nodeId] = opeObj;
					addOperator(opeObj);
				}
				else {
					logger->error("[parseOperatorTree] getOperator return NULL. nodeId:" + nodeId);
					outputXmlLog(doc);
					return firstOpe;
				}
			}

			// リレーション解析
			XMLString::transcode("edge", tempStr, isp.BUF_MAX - 1);
			if (dynamic_cast<DOMElement*>(graphListCopy->item(0))->getElementsByTagName(tempStr) == NULL)
			{
				retErrMsg = "[parseOperatorTree] <edge></edge> is not defined";
				logger->error(retErrMsg);
				outputXmlLog(doc);
				return firstOpe;
			}
			DOMNodeList* edgeList = dynamic_cast<DOMElement*>(graphListCopy->item(0))->getElementsByTagName(tempStr);

#if MEASURE_MODE != 1
			if (edgeList->getLength() == 0) {
				retErrMsg = "[parseOperatorTree] edge element is not exist.";
				logger->error(retErrMsg);
				outputXmlLog(doc);
				return firstOpe;
			}
#else
			// 性能測定時はオペレータ単体の場合も許容
			if (opeMap.size() == 1) {
				for (auto itr = opeMap.begin(); itr != opeMap.end(); ++itr) {
					firstOpe = itr->second;
					logger->debug("[parseOperatorTree] firstOpe SET nid:" + itr->first);
				}
			}
#endif
			for (unsigned int i = 0; i < edgeList->getLength(); i++)
			{
				string sourceId, targetId;
				isp.getTagValue(dynamic_cast<DOMElement*>(edgeList->item(i)), "source", sourceId);
				isp.getTagValue(dynamic_cast<DOMElement*>(edgeList->item(i)), "target", targetId);
				if (sourceId.length() == 0 || targetId.length() == 0) {
					retErrMsg = "[parseOperatorTree] lack of edge information. (source or target is not exists)";
					logger->error(retErrMsg);
					outputXmlLog(doc);
					return firstOpe;
				}
				if (returnNodeIdMap.find(sourceId) != returnNodeIdMap.end()) {
					// 再帰オペレータのaddReturnを判別し、登録
					bool addReturn = false;
					vector<string> paramList;
					stringUtil.split(returnNodeIdMap[sourceId], ",", paramList);
					for (string nodeId : paramList) {
						if (nodeId == targetId) {
							opeMap[sourceId]->addReturn(opeMap[targetId]);
							logger->debug("[parseOperatorTree] reterningOperator info:" + sourceId + " => " + targetId);
							addReturn = true;
							break;
						}
					}
					if (addReturn) continue;
				}
				relationingOperator(opeMap[sourceId], opeMap[targetId]);
				logger->debug("[parseOperatorTree] relationingOperator info:" + sourceId + " => " + targetId);
				if (firstOpe == NULL && i == 0) {
					firstOpe = opeMap[sourceId];
				}

				// ToDo: isExistJoinを条件から外したが、それで良いか？Join時に重複が許容・不要の判断が別途必要
				if (isExistsAggregateFunc || query.find("DUPLICATION") != std::string::npos || query.find("duplication") != std::string::npos) {
					WindowOperator *wOpe = dynamic_cast<WindowOperator*>(opeMap[sourceId]);
					if (wOpe != NULL) {
						wOpe->setAllowDuplicateData(true);
					}
				}
			}

		}
		catch (DOMException&)
		{
			retErrMsg = "[parseOperatorTree] DOMException Line:" + std::to_string(__LINE__);
			logger->error(retErrMsg);
		}
		input->release();
		domBuilder->release();
		isp.finalize();
		//REL_COMMENT logger->trace("[parseOperatorTree] OUT");
		return firstOpe;
		
		/*
		cout << operatorTreeXml << endl;
logger->error("operatorTreeXml.size() = " +
              std::to_string(operatorTreeXml.size()));
logger->error("strlen(c_str) = " +
              std::to_string(strlen(operatorTreeXml.c_str())));
		static const XMLCh GRAPHML_NS[] =
    u"http://graphml.graphdrawing.org/xmlns";
		static const XMLCh TAG_KEY[]   = { chLatin_k, chLatin_e, chLatin_y, chNull };
		static const XMLCh TAG_GRAPH[] = { chLatin_g, chLatin_r, chLatin_a, chLatin_p, chLatin_h, chNull };
		static const XMLCh TAG_NODE[]  = { chLatin_n, chLatin_o, chLatin_d, chLatin_e, chNull };
		static const XMLCh TAG_EDGE[]  = { chLatin_e, chLatin_d, chLatin_g, chLatin_e, chNull };
		//REL_COMMENT logger->trace("[parseOperatorTree] IN");
		map<std::string, Operator*> opeMap;
		Operator* firstOpe = NULL;
		XMLCh tempStr[operatorTreeXml.length() + 1];
		static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
		DOMImplementationLS *impl = (DOMImplementationLS*)DOMImplementationRegistry::getDOMImplementation(gLS);
		DOMLSParser       *domBuilder = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
		DOMLSInput        *input = impl->createLSInput();
		//domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, false);

		XMLCh* xmlCh = XMLString::transcode(operatorTreeXml.c_str());
		input->setStringData(xmlCh);
		
		IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		isp.init();
		try
		{
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMComments, false);
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMDatatypeNormalization, true);
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMEntities, false);
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, true);
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMElementContentWhitespace, false);
			DOMDocument* doc = domBuilder->parse(input);
			dumpDom(doc->getDocumentElement());
			// verify that we get only 3 calls: for the text node, the CDATA section and the root element
			if (doc == NULL || doc->getDocumentElement() == NULL)
			{
				retErrMsg = "[parseOperatorTree] document is Null Line:" + std::to_string(__LINE__);
				logger->error(retErrMsg);
				outputXmlLog(doc);
				return firstOpe;
			}

			DOMElement* root = doc->getDocumentElement();

			// キー解析
			DOMNodeList* keyList = root->getElementsByTagNameNS(GRAPHML_NS, TAG_KEY);
			if (!keyList || keyList->getLength() == 0) {
				retErrMsg = "[parseOperatorTree] <key></key> is not defined";
				logger->error(retErrMsg);
				outputXmlLog(doc);
				return firstOpe;
			}

			for (unsigned int i = 0; i < keyList->getLength(); i++)
			{
				string id, forStr, attrName, attrType;
				isp.getTagValue(dynamic_cast<DOMElement*>(keyList->item(i)), "id", id);
				isp.getTagValue(dynamic_cast<DOMElement*>(keyList->item(i)), "for", forStr);
				isp.getTagValue(dynamic_cast<DOMElement*>(keyList->item(i)), "attr.name", attrName);
				isp.getTagValue(dynamic_cast<DOMElement*>(keyList->item(i)), "attr.type", attrType);
				logger->debug("id:" + id + " for:" + forStr + " attrName:" + attrName + " attrType:" + attrType);
			}

			// グラフ解析(オペレーターツリー単位)
			DOMNodeList* graphList = root->getElementsByTagNameNS(GRAPHML_NS, TAG_GRAPH);
			if (!graphList || graphList->getLength() == 0) {
				retErrMsg = "[parseOperatorTree] <graph></graph> is not defined";
				logger->error(retErrMsg);
				outputXmlLog(doc);
				return firstOpe;
			}
			DOMElement* graphElem =
				dynamic_cast<DOMElement*>(graphList->item(0));

static const XMLCh ANY_NS[] = { chAsterisk, chNull };
DOMNodeList* edgeListAnyNS =
    graphElem->getElementsByTagNameNS(
        ANY_NS,
        TAG_EDGE
    );

DOMNodeList* nodeListAnyNS =
    graphElem->getElementsByTagNameNS(
        ANY_NS,
        TAG_NODE
    );
logger->error("edgeListAnyNS length = " +
              std::to_string(edgeListAnyNS ? edgeListAnyNS->getLength() : 0));
logger->error("nodeListAnyNS length = " +
              std::to_string(nodeListAnyNS ? nodeListAnyNS->getLength() : 0));
char* geName = XMLString::transcode(graphElem->getNodeName());
char* geLocal = XMLString::transcode(graphElem->getLocalName());
char* geNS = XMLString::transcode(graphElem->getNamespaceURI());

logger->error(std::string("graphElem nodeName=") +
              (geName ? geName : "null"));
logger->error(std::string("graphElem localName=") +
              (geLocal ? geLocal : "null"));
logger->error(std::string("graphElem ns=") +
              (geNS ? geNS : "null"));

XMLString::release(&geName);
XMLString::release(&geLocal);
XMLString::release(&geNS);
			// リレーション解析
			DOMNodeList* edgeList = graphElem->getElementsByTagNameNS(GRAPHML_NS, TAG_EDGE);
			
			if (!edgeList || edgeList->getLength() == 0) {
				retErrMsg = "[parseOperatorTree] <edge></edge> is not defined";
				logger->error(retErrMsg);
				outputXmlLog(doc);
				return firstOpe;
			}
DOMElement* e =
    dynamic_cast<DOMElement*>(edgeList->item(0));
char* ns = XMLString::transcode(e->getNamespaceURI());
char* ln = XMLString::transcode(e->getLocalName());

	cout << ns << endl;
	cout << ln << endl;

XMLString::release(&ns);
XMLString::release(&ln);
			// ノード解析(オペレータ単位)
			DOMNodeList* nodeList = graphElem->getElementsByTagNameNS(GRAPHML_NS, TAG_NODE);
			if (!nodeList || nodeList->getLength() == 0) {
				retErrMsg = "[parseOperatorTree] <node></node> is not defined";
				logger->error(retErrMsg);
				outputXmlLog(doc);
				return firstOpe;
			}

			// クエリ解析結果にprojectionもしくはjoinが含まれていた場合は、応答結果が可変となる
			bool isDynamicColumn = operatorTreeXml.find("projection") != string::npos || operatorTreeXml.find("join") != string::npos;

			WindowOperator* inOpe = nullptr;
			ResponseOperator* resOpe = nullptr;
			string streamName;
			for (unsigned int i = 0; i < nodeList->getLength(); i++)
			{
				string nodeId;
				isp.getTagValue(dynamic_cast<DOMElement*>(nodeList->item(i)), "id", nodeId);
				DOMNodeList* dataList = isp.getElementByCharTagName(dynamic_cast<DOMElement*>(nodeList->item(i)), (char*)"data");
				Operator* opeObj = getOperator(dataList, graphList, nodeId, isDynamicColumn);
				if (opeObj != NULL) {
					// WindowOperatorからテーブル名を取り出して、ResponseOperatorに設定する
					if (auto ope = dynamic_cast<WindowOperator*>(opeObj)) {
						inOpe = ope;
						streamName = inOpe->getTargetStreamName();
						// 既にResponseOperatorが出現していた場合は設定する
						if (resOpe != nullptr)
							resOpe->protobufMessageName = streamName;
					} else if (auto ope = dynamic_cast<ResponseOperator*>(opeObj)) {
						resOpe = ope;
						// WindowOperatorから取り出したテーブル名を設定
						if (streamName != "")
							resOpe->protobufMessageName = streamName;
					}
					opeObj->setNodeId(nodeId);
					opeMap[nodeId] = opeObj;
					addOperator(opeObj);
				}
				else {
					logger->error("[parseOperatorTree] getOperator return NULL. nodeId:" + nodeId);
					outputXmlLog(doc);
					return firstOpe;
				}
			}

#if MEASURE_MODE == 1
			// 性能測定時はオペレータ単体の場合も許容
			if (opeMap.size() == 1) {
				for (auto itr = opeMap.begin(); itr != opeMap.end(); ++itr) {
					firstOpe = itr->second;
					logger->debug("[parseOperatorTree] firstOpe SET nid:" + itr->first);
				}
			}
#endif
			for (unsigned int i = 0; i < edgeList->getLength(); i++)
			{
				string sourceId, targetId;
				isp.getTagValue(dynamic_cast<DOMElement*>(edgeList->item(i)), "source", sourceId);
				isp.getTagValue(dynamic_cast<DOMElement*>(edgeList->item(i)), "target", targetId);
				if (sourceId.length() == 0 || targetId.length() == 0) {
					retErrMsg = "[parseOperatorTree] lack of edge information. (source or target is not exists)";
					logger->error(retErrMsg);
					outputXmlLog(doc);
					return firstOpe;
				}
				if (returnNodeIdMap.find(sourceId) != returnNodeIdMap.end()) {
					// 再帰オペレータのaddReturnを判別し、登録
					bool addReturn = false;
					vector<string> paramList;
					stringUtil.split(returnNodeIdMap[sourceId], ",", paramList);
					for (string nodeId : paramList) {
						if (nodeId == targetId) {
							opeMap[sourceId]->addReturn(opeMap[targetId]);
							logger->debug("[parseOperatorTree] reterningOperator info:" + sourceId + " => " + targetId);
							addReturn = true;
							break;
						}
					}
					if (addReturn) continue;
				}
				relationingOperator(opeMap[sourceId], opeMap[targetId]);
				logger->debug("[parseOperatorTree] relationingOperator info:" + sourceId + " => " + targetId);
				if (firstOpe == NULL && i == 0) {
					firstOpe = opeMap[sourceId];
				}

				// ToDo: isExistJoinを条件から外したが、それで良いか？Join時に重複が許容・不要の判断が別途必要
				if (isExistsAggregateFunc || query.find("DUPLICATION") != std::string::npos || query.find("duplication") != std::string::npos) {
					WindowOperator *wOpe = dynamic_cast<WindowOperator*>(opeMap[sourceId]);
					if (wOpe != NULL) {
						wOpe->setAllowDuplicateData(true);
					}
				}
			}

		}
		catch (DOMException&)
		{
			retErrMsg = "[parseOperatorTree] DOMException Line:" + std::to_string(__LINE__);
			logger->error(retErrMsg);
		}
    	XMLString::release(&xmlCh);
		input->release();
		domBuilder->release();
		isp.finalize();
		//REL_COMMENT logger->trace("[parseOperatorTree] OUT");
		return firstOpe;
		*/
	}

	/**
	 * オペレータを動的に生成する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in,out]	dataList	オペレータXML情報
	 * @param [in]	operatorTreeXmlList	オペレータツリーXMLリスト
	 * @return	オペレータオブジェクト
	 */

	Operator* QueryExecuter::getOperator(DOMNodeList* dataList, DOMNodeList* graphList, const string &nodeId, bool isDynamicColumn)
	{
		//REL_COMMENT logger->trace("[getOperator] IN");
		Operator* ope = NULL;
		string operatorStr = "";
		string parameterStr = "";
		string master = "";
		string streamName = "";
		string streamClass = "";
		string classVal = "";
		string streamStartFix = "";
		string startFixVal = "";
		string rangeSeed = "";
		string partitionList = "";
		int rangeWin = -1;
		long timeWin = -1;
		bool isNotifyAll = false;
		int rangeLimit = -1;
		IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		isp.init();
		for (unsigned int i = 0; i < dataList->getLength(); i++)
		{
			string key;
			isp.getTagValue(dynamic_cast<DOMElement*>(dataList->item(i)), "key", key);
			if ("operator_type" == key) {
				operatorStr = isp.getChildText(dataList->item(i));
			}
			else if ("parameter" == key) {
				parameterStr = isp.getChildText(dataList->item(i));
			}
			else if ("master" == key) {
				master = isp.getChildText(dataList->item(i));
				if (master.length() != 0) isOneShot = false;
			}
			else if ("range_window" == key) {
				rangeWin = std::stoi(isp.getChildText(dataList->item(i)));
			}
			else if ("time_window" == key) {
				timeWin = std::stol(isp.getChildText(dataList->item(i)));
			}
			else if ("partition" == key) {
				partitionList = isp.getChildText(dataList->item(i));
			}
			else if ("notify_all" == key) {
				isNotifyAll = true;
			}
			else if ("stream_name" == key) {
				streamName = isp.getChildText(dataList->item(i));
			}
			else if ("stream_class" == key) {
				streamClass = isp.getChildText(dataList->item(i));
			}
			else if ("val" == key) {
				classVal = isp.getChildText(dataList->item(i));
			}
			else if ("stream_fix" == key) {
				streamStartFix = isp.getChildText(dataList->item(i));
			}
			else if ("val_fix" == key) {
				startFixVal = isp.getChildText(dataList->item(i));
			}
			else if ("limit" == key) {
				rangeLimit = std::stoi(isp.getChildText(dataList->item(i)));
			}
			else if ("seed" == key) {
				rangeSeed = isp.getChildText(dataList->item(i));
			}
			else if ("nid" == key) {
				// nothing for debug operatorTree(graphml可視化用)
			}
			else {
				logger->error("MNGID:" + std::to_string(this->mngId) + " getOperator error Not define key : " + key);
			}
		}
		isp.finalize();
		// window operator
		if ("in" == operatorStr) {
			IS::QueueManager &QM = IS::QueueManager::get_instance();
			vector<string> paramList;
			stringUtil.split(parameterStr, ",", paramList);
			string schemaName = "";

			if (master.empty()) {
				retErrMsg = "[getOperator] Can't generating operator Because 'MASTER streamName' clause isn't specified.";
				logger->warn(retErrMsg);
				retErrCode = ErrorCode::QUERY_PARSE_ERR;
				return ope;
			}
			if (!QM.isFound(master)) {
				bool isFound = false;
				for (int i = 0; i < 10; i++) {
					sleep(1);
					isFound = QM.isFound(master);
					if (isFound) break;
				}
				if (!isFound) {
					retErrMsg = "[getOperator] Can't generating operator Because not found '" + master + "' stream.";
					logger->warn(retErrMsg);
					retErrCode = ErrorCode::AUTHORITY_ERR;
					return ope;
				}
			}
			if (!QM.isSelect(master, this->user)) {
				retErrMsg = "[getOperator] Can't generating operator Because User '" + this->user + "' doesn't have permission to read '" + master + "' stream.";
				logger->warn(retErrMsg);
				retErrCode = ErrorCode::AUTHORITY_ERR;
				return ope;
			}
			if (paramList.size() > 1) {
				if (!QM.isSelect(paramList.at(1), this->user)) {
					retErrMsg = "[getOperator] Can't generating operator Because User '" + this->user + "' doesn't have permission to read '" + paramList.at(1) + "' stream.";
					logger->warn(retErrMsg);
					retErrCode = ErrorCode::AUTHORITY_ERR;
					return ope;
				}
				schemaName = paramList.at(1);
				stringUtil.replaceOnce(paramList.at(0), "MNGID", to_string(this->mngId));
			}
			else {
				if (!QM.isSelect(paramList.at(0), this->user)) {
					retErrMsg = "[getOperator] Can't generating operator Because User '" + this->user + "' doesn't have permission to read '" + paramList.at(0) + "' stream.";
					logger->warn(retErrMsg);
					retErrCode = ErrorCode::AUTHORITY_ERR;
					return ope;
				}
			}
			vector<int> partitionIdxList;
			if (partitionList != "") {
				Schema schema;
				ism.getStreamSchema(paramList.at(0), schema);
				vector<string> partitionVector;
				stringUtil.split(partitionList, ",", partitionVector);
				for (string s: partitionVector) {
					int idx = schema.getAttributeIdx(s);
					if (idx == -1) {
						retErrMsg = "[getOperator] Can't generating operator Because Partition Attr is wrong. Attr[" + s + "] is not in " + paramList.at(0);
						logger->warn(retErrMsg);
						retErrCode = ErrorCode::AUTHORITY_ERR;
						return ope;
					}
					partitionIdxList.push_back(idx);
				}
			}
			if (rangeWin != -1) {
				ope = new WindowOperator(this->mngId, master, paramList.at(0), schemaName, rangeWin, rangeLimit, rangeSeed, partitionIdxList);
			}
			else if (timeWin != -1) {
				ope = new WindowOperator(this->mngId, master, paramList.at(0), schemaName, timeWin, partitionIdxList);
			}
			else if (isNotifyAll != false) {
				ope = new WindowOperator(this->mngId, master, paramList.at(0), schemaName);
			}
			else {
				// 時間、個数共に設定されていない場合
				ope = new WindowOperator(this->mngId, master, paramList.at(0), schemaName, (long)100, partitionIdxList);
				logger->error("MNGID:" + std::to_string(this->mngId) + " Invalied argment WindowOperator, So boot timewindow 100msec");
			}
		}
		// response operator
		else if ("out" == operatorStr) {
			if (isOneShot) {
				logger->debug("MNGID:" + std::to_string(this->mngId) + ", OutputType:One-shot Query");
				ope = new ResponseOperator(mngId, recvInfo);
			}
			else {

#if MEASURE_MODE == 1
				long procTime = DmUtil::getTimeMicrosec();
#endif
#if MEASURE_MODE == 1
				long now = DmUtil::getTimeMicrosec();
				double msec = (now - procTime) / 1000.0;
				cout << "[getOperator] getReceptionPort processTime(msec):" << msec << endl;
#endif
				logger->debug("MNGID:" + std::to_string(this->mngId) + ", OutputType:Continuous Query, ResponsePort:" + std::to_string(query_header_.port));
				if (!(query_header_.dmiName.empty())) {
					ope = new DmiOperator(this->user, mngId, query_header_.dmiName);
				}
				else if (query_header_.dstSID != 0) {
					int retry = 0;
					int lifeTime = 0;

					// 定義済みクエリの場合、再送設定の読込み
					// Protobuf化に伴い、OFF。Todo: 再送の在り方について再検討
					//if (recvInfo.sock < 0) {
					//	isp.init();
					//	lifeTime = isp.getRetryParam(recvInfo.payload);
					//	isp.finalize();
					//	if (lifeTime != 0) retry = 1;
					//}
					ope = new IS::TransferOperator(this->mngId, query_header_.dstSID, "", retry, lifeTime);
				}
				else if (query_header_.port != 0) {
					ope = new ResponseOperator(this->user, mngId, recvInfo, query_header_.port, isDynamicColumn);
				}
				else {
					logger->warn("MNGID:" + std::to_string(this->mngId) + ", OutputType:Continuous Query, no output");
				}
			}
		}
		// transfer operator
		else if ("transfer" == operatorStr) {
			vector<string> paramList;
			string destId = "";
			string streamName = "";
			string operatorTreeXML = "";
			stringUtil.split(parameterStr, ",", paramList);

			// 宛先SID (TODO:本来はクエリパーサがどこに対して依頼するかを把握すべき。将来的にはパーサ側でSID変換をする想定)
			if (paramList.size() > 0) {
				if (strcasecmp(paramList.at(0).c_str(), "own") == 0) {
					destId = settings.getParameter("MY_STATION_ID");
				}
				else if (strcasecmp(paramList.at(0).c_str(),"nearby") == 0) {
					destId = "0";
				}
				else if (strcasecmp(paramList.at(0).c_str(), "all") == 0) {
					//クラウドのStationIDの読込(dm2.confから読み込む)
					destId = settings.getParameter("CLOUD_STATION_ID");
				}
				else {
					destId = paramList.at(0);
				}
			}

			// ストリーム名
			if (paramList.size() > 1) {
				streamName = paramList.at(1);
				stringUtil.replaceAll(streamName, "MNGID", to_string(this->mngId));
			}

			int retry = 0;
			int lifeTime = 0;

			// 定義済みクエリの場合、再送設定の読込み
			// Protobuf化に伴い、OFF。Todo: 再送の在り方について再検討
			//if (recvInfo.sock < 0) {
			//	IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
			//	isp.init();
			//	lifeTime = isp.getRetryParam(recvInfo.payload);
			//	isp.finalize();
			//	if (lifeTime != 0) retry = 1;
			//}

			try {
				//ope = new TransferOperator(this->mngId, std::stoi(destId), streamName, retry, lifeTime);
				ope = new TransferOperator(this->mngId, std::stoull(destId), streamName, retry, lifeTime);
			}
			catch (...) {
				logger->error("LINE:" + std::to_string(__LINE__) + " stoi エラー destId:" + destId);
			}
		}
		// projection operator
		else if ("projection" == operatorStr) {
			ope = new ProjectionOperator(this->mngId, parameterStr);
		}
		// selection operator
		else if ("selection" == operatorStr) {
			ope = new SelectionOperator(this->mngId, parameterStr);
		}
		// RDBAccess operator
		else if ("RDB" == operatorStr) {
			ope = new RDBAccessOperator(this->mngId, parameterStr, master);
		}
		// eval opearator
		else if ("eval" == operatorStr) {
			ope = new EvalOperator(this->mngId, parameterStr);
			EvalOperator *eOpe = dynamic_cast<EvalOperator*>(ope);
			bool doAggregate = false;
			if (!eOpe->checkParameter(retErrMsg, doAggregate)) {
				retErrCode = ErrorCode::INVALID_ARG_ERR;
				return NULL;
			}
			if (doAggregate) isExistsAggregateFunc = true;
		}
		// join operator
		else if ("join" == operatorStr) {
			ope = new JoinOperator(this->mngId, parameterStr);
			isExistsJoin = true;
		}
		// recursive operator
		else if ("recursive" == operatorStr) {
			// パラメータをカンマで区切って2つ目の戻り先ノードID指定
			vector<string> paramList;
			stringUtil.split(parameterStr, ",", paramList);
			ope = new RecursiveOperator(this->mngId, paramList.at(0), master);
			for (unsigned int i = 1; i < paramList.size(); i++) {
				if (returnNodeIdMap.end() != returnNodeIdMap.find(nodeId)) {
					returnNodeIdMap[nodeId] = returnNodeIdMap[nodeId] + "," + paramList.at(i);
				}
				else {
					returnNodeIdMap[nodeId] = paramList.at(i);
				}
			}
			
		}
		// rename column operator
		else if ("renameColumn" == operatorStr) {
			ope = new RenameColumnOperator(this->mngId, parameterStr);
		}
		// rename table operator
		else if ("renameTable" == operatorStr) {
			ope = new RenameTableOperator(this->mngId, parameterStr);
		}
		// request operator
		else if ("request" == operatorStr) {
			vector<string> paramList;
			string destId = "";
			string streamName = "";
			string operatorTreeXML = "";
			stringUtil.split(parameterStr, ",", paramList);

			// 宛先SID (TODO:本来はクエリパーサがどこに対して依頼するかを把握すべき。将来的にはパーサ側でSID変換をする想定)
			if (paramList.size() > 0) {
				if (strcasecmp(paramList.at(0).c_str(),"nearby") == 0) {
					destId = "0";
				}
				else if (strcasecmp(paramList.at(0).c_str(), "all") == 0) {
					//クラウドのStationIDの読込(dm2.confから読み込む)
					destId = settings.getParameter("CLOUD_STATION_ID");

				}
				else if (strcasecmp(paramList.at(0).c_str(), "own") == 0) {
					//自身のStationIDの読込(dm2.confから読み込む)
					destId = settings.getParameter("MY_STATION_ID");
				}
				else {
					destId = paramList.at(0);
				}
			}

			// オペレータツリー
			if (paramList.size() > 1) {
				DOMNode* parent = graphList->item(0)->getParentNode();
				vector<DOMNode*> nodeList;
				int graphNum = graphList->getLength();
				int idx = 0;
				IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
				isp.init();
				for (int i = 0; i < graphNum; i++) {
					string gid;
					isp.getTagValue(dynamic_cast<DOMElement*>(graphList->item(idx)), "id", gid);
					if (gid != paramList.at(1)) {
						nodeList.push_back(graphList->item(idx)->cloneNode(true));
						parent->removeChild(graphList->item(idx));
					}
					else {
						idx++;
					}
				}

				if (graphList->getLength() == 1) {
					isp.outputXML(graphList->item(0)->getOwnerDocument(), operatorTreeXML);
					stringUtil.replaceOnce(operatorTreeXML, "MNGID", to_string(this->mngId));
				}
				else {
					logger->error("graphList->getLength() != 1. Not Found gid:" + paramList.at(1) + " or graphList so many");
				}
				isp.finalize();
				for (unsigned int i = 0; i < nodeList.size(); i++) {
					parent->appendChild(nodeList.at(i));
				}
			}
			else {
				logger->error("Not defined target graphID in operatorTree(request).");
			}

			TupleSet tupleset;
			vector<TupleSet> ts;
			ts.push_back(tupleset);
			try {
				if (paramList.size() <= 2) 
					//ope = new RequestOperator(this->mngId, std::stoi(destId), operatorTreeXML);
					ope = new RequestOperator(this->mngId, std::stoull(destId), operatorTreeXML);
				else {
					// データ取得要求範囲指定(受付時の情報や連携先の情報(第6～8引数)はエッジにて判断されるため、クエリ解析者(車両IS想定)は0 or 空文字指定)
					//ope = new RequestOperator(this->mngId, std::stoi(destId), operatorTreeXML, stoi(settings.getParameter("MY_STATION_ID")), stoi(paramList.at(2)), 0, 0, "");
					ope = new RequestOperator(this->mngId, std::stoull(destId), operatorTreeXML, stoi(settings.getParameter("MY_STATION_ID")), stoi(paramList.at(2)), 0, 0, "");
				}
			}
			catch (...) {
				logger->error("LINE:" + std::to_string(__LINE__) + " stoi エラー");
			}

			// 処理委譲実施
			if (!ope->process(ts)) {
				string streamStr = "Not defined";
				vector<string> posStreamList = stringUtil.split(settings.getParameter("ADD_LANE_ID_SCHEMA"), ",");
				for (string s : posStreamList) {
					vector<string> sList = stringUtil.split(s, ".");
					if (sList.size() == 2) {
						if (streamStr == "Not defined") streamStr.clear();
						else streamStr.append(",");
						streamStr = stringUtil.split(s, ".")[0];
					}
				}
				retErrMsg = "[getOperator] Can't request 'nearby' because unable to get location information.  Please confirm that '"+ streamStr +"' stream is flowing or wait for a while and retry.";
				logger->warn(retErrMsg);
				retErrCode = ErrorCode::INVALID_ARG_ERR;
				return NULL;
			}
			isExistsRequest = true;
		}
		// union operator
		else if ("union" == operatorStr) {
			ope = new UnionOperator(this->mngId, parameterStr);
		}
		// create operator
		else if ("create" == operatorStr) {
			ope = new IS::CreateOperator(this->user, this->mngId, streamName, parameterStr, streamClass, classVal, streamStartFix, startFixVal);
		}
		// drop operator
		else if ("drop" == operatorStr) {
			ope = new IS::DropOperator(this->user, this->mngId, streamName);
		}
		else
		{
			logger->error("MNGID:" + std::to_string(this->mngId) + " getOperator error Not define operator_type : " + operatorStr);
			return NULL;
		}
		//REL_COMMENT logger->trace("[getOperator] OUT");
		return ope;
	}

	/**
	* IS間連携にてクエリ委譲した後に委譲先から返却されたキャンセルIDを登録する
	*
	* @author	Nagoya University
	* @date	2018/08/28
	*
	* @param [in]	query	クエリ
	*
	*/
	void QueryExecuter::registerCancelId(const string &query)
	{
		IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		isp.init();
		//int senderId = isp.getSenderSID(query);
		unsigned long long senderId = isp.getSenderSID(query);
		if (isp.getErrorCode(query) != 0) {
			retErrMsg = "[registerCancelId] ERROR occured by other IS (SID:" + std::to_string(senderId) + "). code:" + std::to_string(isp.getErrorCode(query));
			logger->error(retErrMsg);
			returnError(static_cast<IS::ErrorCode>(isp.getErrorCode(query)), retErrMsg);
			isp.finalize();
			return;
		}
		
		int resMngId = isp.getAttrValueFromRootTag("id", query);
		int reqDestId = isp.getAttrValueFromRootTag("reqDestId", query);
		logger->debug("[registerCancelId] id:" + to_string(resMngId) + " reqDestId:" + to_string(reqDestId) + " mngKey:" + mngKey);
		int requestOPNum = 0, setCancelIdNum = 0;
		pthread_mutex_lock(&opListMtx);
		for (Operator* ope : opeList)
		{
			if (typeid(*ope) == typeid(RequestOperator)) {
				requestOPNum++;
				RequestOperator *rOpe = dynamic_cast<RequestOperator*>(ope);
				if (rOpe->setRequestedMngId(reqDestId, resMngId, senderId)) {
					setCancelIdNum++;

					IS::LocationManager &LM = IS::LocationManager::get_instance();
					LM.updateTranMngId(this->mngKey, reqDestId, resMngId);
				}
			}
		}
		pthread_mutex_unlock(&opListMtx);
		if (requestOPNum == setCancelIdNum) {
			// リクエストオペレータ数とキャンセルIDがセット完了した数が一致する場合はクライアントにキャンセルIDを返却する
			returnMngID(this->mngId);
		}
		isp.finalize();
	}


	/**
	* キャンセル依頼元のSIDを元にRequestOperatorに対してキャンセル範囲を設定する
	*
	* @author	Nagoya University
	* @date	2019/12/25
	*
	* @param [in]	senderId	送信元SID
	*
	*/
	//void QueryExecuter::settingCancellationRange(const int senderId)
	void QueryExecuter::settingCancellationRange(const unsigned long long senderId)
	{
		pthread_mutex_lock(&opListMtx);
		for (Operator* ope : opeList)
		{
			if (typeid(*ope) == typeid(RequestOperator)) {
				RequestOperator *rOpe = dynamic_cast<RequestOperator*>(ope);
				rOpe->setCancellationRange(senderId);
			}
		}
		pthread_mutex_unlock(&opListMtx);
	}

	/**
	* リクエスト処理を追加する
	*
	* @author	Nagoya University
	* @date	2019/12/23
	*
	* @param [in]	destSID	リクエスト先SID
	* @param	mngInfo	連携情報
	* @param[in]	edgeSIDListStr		連携先エッジSID文字列(カンマ区切り)
	*
	*/
	//void QueryExecuter::addRequest(const unsigned int destSID, PosMng::MngInfo mngInfo, const string &edgeSIDListStr)
	void QueryExecuter::addRequest(const unsigned long long destSID, PosMng::MngInfo mngInfo, const string &edgeSIDListStr)
	{
		Operator* request = new RequestOperator(this->mngId, destSID, mngInfo.tranOperatorTreeXML, mngInfo.requestedVehicleId, mngInfo.range, mngInfo.receptionMngId, mngInfo.receptionEdgeId, edgeSIDListStr);
		addOperator(request);
		TupleSet tupleset;
		vector<TupleSet> ts;
		ts.push_back(tupleset);
		request->process(ts);
	}

	/**
	* リクエスト処理を削除する
	*
	* @author	Nagoya University
	* @date	2019/12/23
	*
	* @param [in]	destSID	リクエスト先SID
	* @param	mngInfo	連携情報
	*
	*/
	//void QueryExecuter::cancelRequest(const unsigned int destSID, PosMng::MngInfo mngInfo)
	void QueryExecuter::cancelRequest(const unsigned long long destSID, PosMng::MngInfo mngInfo)
	{
		logger->trace("[cancelRequest] IN  destSID:" + to_string(destSID) + " opeList.size():" + to_string(opeList.size()));
		Operator* request = new RequestOperator(mngInfo.receptionMngId, destSID, mngInfo.tranOperatorTreeXML, mngInfo.requestedVehicleId, mngInfo.range, mngInfo.receptionMngId, mngInfo.receptionEdgeId, "");
		RequestOperator *rOpe = dynamic_cast<RequestOperator*>(request);

		rOpe->setRequestedMngId(destSID, mngInfo.receptionMngId, mngInfo.receptionEdgeId);
		request->exit();
		delete request;

		removeRequestOp(destSID);
		logger->trace("[cancelRequest] OUT destSID:" + to_string(destSID) + " opeList.size():" + to_string(opeList.size()));
	}

	/**
	* リクエストオペレータを削除する
	*
	* @author	Nagoya University
	* @date	2020/01/06
	*
	* @param [in]	destSID	リクエスト先SID
	*
	*/
	//void QueryExecuter::removeRequestOp(const unsigned int destSID)
	void QueryExecuter::removeRequestOp(const unsigned long long destSID)
	{
		logger->trace("[removeRequestOp] IN  destSID:" + to_string(destSID) + " opeList.size():" + to_string(opeList.size()));
		pthread_mutex_lock(&opListMtx);
		int beforeSize = opeList.size();
		for (auto it =opeList.begin(); it != opeList.end();) {
			Operator* ope = *it;
			if (typeid(*ope) == typeid(RequestOperator)) {
				RequestOperator *rOpe = dynamic_cast<RequestOperator*>(ope);
				if (rOpe->getDestSID() == destSID) {
					// キャンセル要求を伝播させない
					rOpe->setCancellationRange(destSID);
					ope->exit();
					delete ope;

					it = opeList.erase(it);
					logger->debug("[removeRequestOp] remove RequestOperator(destSID:" + to_string(destSID) + ") opeList.size():" + to_string(beforeSize) + " -> " + to_string(opeList.size()));
				}
				else {
					++it;
				}
			}
			else {
				++it;
			}
		}
		pthread_mutex_unlock(&opListMtx);
		logger->trace("[removeRequestOp] OUT destSID:" + to_string(destSID) + " opeList.size():" + to_string(opeList.size()));
	}

	/**
	* OneShot Queryかどうかを取得する
	*
	* @author	Nagoya University
	* @date	2019/06/24
	*
	*/
	bool QueryExecuter::isOneShotQuery()
	{
		return isOneShot;
	}

	/**
	* エラー発生時にXMLをロギングする
	*
	* @author	Nagoya University
	* @date	2019/06/25
	*
	*/
	void QueryExecuter::outputXmlLog(DOMDocument* doc)
	{
		string log;
		IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		isp.init();
		isp.outputXML(doc, log);
		isp.finalize();
		logger->error(log);
	}

	/**
	* Windowオペレータから処理開始間隔平均時間を取得する
	*
	* @author	Nagoya University
	* @date	2019/10/29
	* @return 処理開始間隔平均時間
	*/
	double QueryExecuter::getNotifiedIntervalAVG()
	{
		pthread_mutex_lock(&opListMtx);
		for (Operator* ope : opeList)
		{
			if (typeid(*ope) == typeid(WindowOperator)) {
				WindowOperator *wOpe = dynamic_cast<WindowOperator*>(ope);
				pthread_mutex_unlock(&opListMtx);
				return wOpe->getNotifiedIntervalAVG();
			}
		}
		pthread_mutex_unlock(&opListMtx);
		logger->warn("[MNGID:" + to_string(this->mngId) + "][getNotifiedIntervalAVG] No NotifiedIntervalAVG data because Not Found WindowOperator.");
		return 0;
	}

	/**
	* Responseオペレータから総処理平均時間を取得する
	*
	* @author	Nagoya University
	* @date	2019/10/29
	* @return 総処理平均時間
	*/
	double QueryExecuter::getTotalProcessTimeAVG()
	{
		pthread_mutex_lock(&opListMtx);
		for (Operator* ope : opeList)
		{
			if (typeid(*ope) == typeid(ResponseOperator)) {
				ResponseOperator *rOpe = dynamic_cast<ResponseOperator*>(ope);
				pthread_mutex_unlock(&opListMtx);
				return rOpe->getTotalProcessTimeAVG();
			}
		}
		pthread_mutex_unlock(&opListMtx);
		logger->warn("[[MNGID:" + to_string(this->mngId) + "]getTotalProcessTimeAVG] No TotalProcessTimeAVG data because Not Found ResponseOperator.");
		return 0;
	}

	/**
	* クエリの稼動状況を取得する
	*
	* @author	Nagoya University
	* @date	2019/10/29
	* @return クエリ稼動状況情報
	*/
	QueryStatus QueryExecuter::getQueryStatus()
	{
		//REL_COMMENT logger->trace("[getQueryStatus] IN");
		QueryStatus qs;

		// クエリID
		qs.mngId = mngId;

		// クエリ文字列
		qs.query = query;

		// クエリ登録ユーザ
		qs.user = user;

		// オペレーターツリーXML
		qs.operatorTreeXML = parseResult;

		// 稼動オペレーター数
		qs.operatorNum = opeList.size();

		// 処理開始間隔平均時間
		qs.notifiedIntervalAVG = getNotifiedIntervalAVG();

		// 総処理時間平均時間
		qs.totalProcessTimeAVG = getTotalProcessTimeAVG();

		int unprocessedDataSum = 0;
		string latestOpeName = "";
		double latestOpeProcTime = 0;
		pthread_mutex_lock(&opListMtx);
		for (Operator* ope : opeList)
		{
			unprocessedDataSum += ope->getBufferSize();
			double procTime = ope->getProcTimeAVG();
			if (latestOpeProcTime < procTime)
			{
				latestOpeProcTime = procTime;
				latestOpeName = ope->getType();
			}
		}
		pthread_mutex_unlock(&opListMtx);

		// 未処理データ数
		qs.unprocessedDataNum = unprocessedDataSum;

		// 最遅オペレータ識別名
		qs.latestOperatorName = latestOpeName;

		// 最遅オペレータ平均処理時間
		qs.latestOpeProcAVG = latestOpeProcTime;

		int inputStreamSum = 0, inputRelationSum = 0, outputSum = 0;
		pthread_mutex_lock(&opListMtx);
		for (Operator* ope : opeList)
		{
			if (typeid(*ope) == typeid(WindowOperator)) {
				inputStreamSum += ope->getPerformInfoNum("input");
			}
			else if (typeid(*ope) == typeid(RDBAccessOperator)) {
				// RDBAccessOperatorはRDBへの問い合わせにより得たTuple数をinputタプルとして合算する
				inputRelationSum += ope->getPerformInfoNum("output");
			}
			else if (typeid(*ope) == typeid(ResponseOperator) || typeid(*ope) == typeid(TransferOperator)) {
				// 秒間OUTPUTタプル数
				outputSum += ope->getPerformInfoNum("output");
			}
		}
		pthread_mutex_unlock(&opListMtx);

		// 秒間INPUTタプル数
		qs.inputStreamTupleNumPerSec = inputStreamSum;

		// 秒間INPUTタプル数
		qs.inputRelationTupleNumPerSec = inputRelationSum;

		// 秒間OUTPUTタプル数
		qs.outputTupleNumPerSec = outputSum;

		//REL_COMMENT logger->trace("[getQueryStatus] END");
		
		return qs;
	}

	/**
	* クエリ登録者かを判定する
	*
	* @author	Nagoya University
	* @date	2019/10/29
	* @return ユーザ一致可否
	*/
	bool QueryExecuter::checkUser(const string &user) {
		if (this->user == user) 
			return true;
		else 
			return false;
	}
}
