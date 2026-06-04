#include "RequestOperator.h"
#include "Settings.h"

using std::cout;		// cout
using std::endl;		// endl
using namespace std;

namespace IS {

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/08/24
	 *
	 * @param 	mngId	クエリ管理番号
	 * @param	dstSID	転送先SID
	 * @param 	operatorTreeXML	転送するオペレータツリー
	 */

	//RequestOperator::RequestOperator(unsigned int mngId, unsigned int dstSID, string operatorTreeXML)
	RequestOperator::RequestOperator(unsigned int mngId, unsigned long long dstSID, string operatorTreeXML)
	{
		this->type = MyName + "_MNGID:" + std::to_string(mngId);
		this->dstSID = dstSID;
		this->operatorTreeXML = operatorTreeXML;
		this->mngId = mngId;
		this->argument.append(" DST_ID:" + to_string(dstSID));
		this->argument.append(" OP_TREE:" + operatorTreeXML);
		this->fdDirPath = settings.getFDDirectory();
	}

	/**
	* コンストラクタ
	*
	* @author	Nagoya University
	* @date	2019/12/24
	*
	* @param 	mngId	クエリ管理番号
	* @param	dstSID	転送先SID
	* @param 	operatorTreeXML	転送するオペレータツリー
	* @param	requestVehicleSID	要求車両SID
	* @param 	range	IS間連携する領域の半径(m)
	* @param	receptionMngId	受付クエリ管理番号(主にNEARBYにて車両から見てエッジ側で受付されたクエリ管理番号)
	* @param	receptionEdgeId	受付エッジSID
	* @param	edgeSIDListStr	連携先エッジSID文字列(カンマ区切り)
	*/

	//RequestOperator::RequestOperator(unsigned int mngId, unsigned int dstSID, string operatorTreeXML, unsigned int requestVehicleSID, int range, unsigned int receptionMngId, unsigned int receptionEdgeId, const string &edgeSIDListStr)
	RequestOperator::RequestOperator(unsigned int mngId, unsigned long long dstSID, string operatorTreeXML, unsigned long long requestVehicleSID, int range, unsigned int receptionMngId, unsigned long long receptionEdgeId, const string &edgeSIDListStr)
	{
		this->type = MyName + "_MNGID:" + std::to_string(mngId);
		this->mngId = mngId;
		this->dstSID = dstSID;
		this->operatorTreeXML = operatorTreeXML;
		this->requestVehicleSID = requestVehicleSID;
		this->range = range;
		this->receptionMngId = receptionMngId;
		this->receptionEdgeId = receptionEdgeId;
		this->edgeSIDListStr = edgeSIDListStr;
		this->argument.append(" DST_ID:" + to_string(dstSID));
		this->argument.append(" REQUEST (VEHICLE_ID:" + to_string(requestVehicleSID) + " RANGE:" + to_string(range) + ")");
		this->argument.append(" RECEPTION (EDGE_ID:" + to_string(receptionEdgeId) + " MNGID:" + to_string(receptionMngId) + ")");
		this->argument.append(" TRAN_ID_LIST:" + edgeSIDListStr);
		this->argument.append(" OP_TREE:" + operatorTreeXML);
		this->fdDirPath = settings.getFDDirectory();
	}

	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/08/24
	 */

	RequestOperator::~RequestOperator()
	{
	}

	/**
	* オペレータ終了処理
	*
	* @author	Nagoya University
	* @date	2018/08/28
	*/

	void RequestOperator::exit()
	{
		//REL_COMMENT logger->trace("[" + this->type + "] exit IN");

		exit_flag = true;
		// 依頼先にクエリのキャンセル要求をする
		if (this->requestedMngId != 0) {
			// キャンセルIDを正常に受信しているISに対して実施
			logger->debug("[exit] cancel request to another IS (SID:" + to_string(dstSID) + ") mngID:" + to_string(this->requestedMngId));
			TupleSet tupleset;
			vector<TupleSet> ts;
			ts.push_back(tupleset);
			process(ts);
			this->requestedMngId = 0;
		}
		Operator::exit();
		//REL_COMMENT logger->trace("[" + this->type + "] exit OUT");
	}

	/**
	* オペレータ処理
	*
	* @author	Nagoya University
	* @date	2018/08/24
	*
	* @param [in,out]	ts	タプルセット
	*
	* @return	正常にデータ処理を実施でき、次のオペレータに渡す際にtrue
	*/
	bool RequestOperator::process(vector<IS::TupleSet>& ts)
	{
		logger->debug("[" + this->type + "] ========== Request START ========== ");
		IS::TupleSet retTupleSet;

#if MEASURE_MODE == 1
		long startTime = DmUtil::getTimeMicrosec();
		long procTime = startTime;
		int step = 1;
#endif

		// DEBUG 与えられたタプル情報の出力
		printInputInfos(ts, this->argument);

		//自身のStationIDの読込(dm2.confから読み込む)
		string mySid = settings.getParameter("MY_STATION_ID");

		// 「own」を自身のSIDに置換する
		stringUtil.replaceAll(this->operatorTreeXML, "own", mySid);

		UdpSendInterface udpsendinterface;
		udpsendinterface.Init(fdDirPath + FD_IStoCS);
		struct send_message buf;

		try {
			//buf.src_station_id = stoi(mySid);
			buf.src_station_id = stoull(mySid);
		}
		catch (const std::invalid_argument& ) {
			logger->error("[" + this->type + "] LINE:" + std::to_string(__LINE__) + " stoi エラー");
		}
		buf.dst_station_id = dstSID;

		// 自身が車両のDBである場合はレーンIDを付与する
		if (settings.getSIDType() == Settings::SID_TYPE::CAR) {
			// レーンIDの付与
			int retryNum = 0;
			while (retryNum < 3) {
				buf.lane_id = LM.getLaneId();
				if (buf.lane_id != 0) break;
				usleep(1 * 1000 * 1000);
				retryNum++;
			}
			if (buf.lane_id == 0) {
				logger->error("[" + this->type + "][process] Can't request by cs, because lane_id is unknown.");
				return false;
			}
		}
		else {
			// 車両DB以外はレーンIDを参照しないため、ダミーのレーンIDを代入する
			buf.lane_id = 0;
		}
		
#if MEASURE_MODE == 1
		long now = DmUtil::getTimeMicrosec();
		double msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " read config parameter processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

		// ストリームXMLを生成する
		string retXML;
		// START CSが分割送信を対応するまでの暫定対応 (CSを使わない場合は自身で分割する必要があるためコメントで残す)//
		//vector<string> retXMLList;
		IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		isp.init();
		//int realLength = 0;	// 分割ヘッダなしの電文長
		if (exit_flag == false) {
			// クエリ転送の電文を生成
			//realLength = isp.createOperatorTreeXMLList(mySid, std::to_string(dstSID), this->operatorTreeXML, this->mngId, IPv4_UDP_MAX_BYTE, retXMLList);
			isp.createOperatorTreeXML(mySid, std::to_string(dstSID), this->operatorTreeXML, this->mngId, this->requestVehicleSID, this->range, this->receptionMngId, this->receptionEdgeId, this->edgeSIDListStr, retXML);
		}
		else {
			//realLength = isp.createCancelXMLList(this->requestedMngId, IPv4_UDP_MAX_BYTE, retXMLList);
			isp.createCancelXML(this->requestedMngId, this->receptionEdgeId, stoi(mySid), retXML);
		}
		isp.finalize();

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " create xml processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

#if MEASURE_MODE == 1
		startTime = DmUtil::getTimeMicrosec();
#endif
		char compressFlg = settings.getParameter("COMPRESS_FLG")[0];
		if (compressFlg == '1' || compressFlg == '2') {
			char outbuf[IPv4_UDP_MAX_BYTE * 10];
			long key = DmUtil::getTimeMicrosec();
			int sendSize = stringUtil.setCompressedBufWithHeader(retXML, outbuf, compressFlg, key);
			if (sendSize > 0) {
				udpsendinterface.IsStreamSendtoCs(buf.lane_id, buf.src_station_id, buf.dst_station_id, 1, 60, outbuf, sendSize, fdDirPath);
			} else {
				logger->warn("[" + this->type + "] CompressProc is Failed. Retry by Uncompressed Data");
				string s = "0" + retXML;
				udpsendinterface.IsStreamSendtoCs(buf.lane_id, buf.src_station_id, buf.dst_station_id, 1, 60, retXML, fdDirPath);
			}
		} else {
			string s = compressFlg + retXML;
			udpsendinterface.IsStreamSendtoCs(buf.lane_id, buf.src_station_id, buf.dst_station_id, 1, 60, retXML, fdDirPath);
		}
		logger->debug("[" + this->type + "] Request by UDP(CS). sendto(dstId):" + std::to_string(buf.dst_station_id) + " srcId:" + std::to_string(buf.src_station_id) + " laneId:" + std::to_string(buf.lane_id) + " size:" + std::to_string(retXML.length()));
		logger->debug("[" + this->type + "] Send payload:" + retXML);

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " sendto(by CS) processing time: " + to_string(msec) + "[ms]");
		msec = (now - startTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " total processing time: " + to_string(msec) + "[ms]");
#endif

		logger->debug("[" + this->type + "] ========== Request  END  ========== ");
		return true;
	}

	/**
	* 依頼先から返却されたクエリ管理番号をセットする
	*
	* @author	Nagoya University
	* @date	2018/08/28
	*
	* @param destId		依頼時に指定したSID
	* @param mngId		依頼先から発行されたクエリ管理番号
	* @param senderId	依頼先のSID
	* @return true/false
	*/

	//bool RequestOperator::setRequestedMngId(unsigned int destId, unsigned int mngId, unsigned int senderId)
	bool RequestOperator::setRequestedMngId(unsigned long long destId, unsigned int mngId, unsigned long long senderId)
	{
		if (this->dstSID == destId) {
			this->requestedMngId = mngId;
			if (settings.getSIDType() == Settings::SID_TYPE::CAR) {
				// 車両の場合のみ受け付けたエッジのSIDが不明のため保持する(車両以外の場合は受付エッジSIDは既知のためコンストラクタ初期値をそのまま利用する)
				this->receptionEdgeId = senderId;
			}
			logger->info("[" + this->type + "][setRequestedMngId] SET  destId:" + std::to_string(destId) + " mngId:" + std::to_string(mngId) + " receptionSID:" + to_string(senderId));
			return true;
		}
		return false;
	}

	/**
	* キャンセルの依頼元SIDからキャンセルする範囲を設定する
	*
	* @author	Nagoya University
	* @date	2019/12/25
	* @param senderId	依頼元のSID
	*
	*/
	//void RequestOperator::setCancellationRange(const unsigned int senderId)
	void RequestOperator::setCancellationRange(const unsigned long long senderId)
	{
		if (this->requestVehicleSID != senderId) {
			logger->debug("[setCancellationRange] Only execute own cancel process. requestVehicleSID:" + to_string(requestVehicleSID) + " cancelRequestSenderID:" + to_string(senderId));
			// 依頼元SIDが要求車両SIDでない場合はLocationManagerによる車両移動に伴うキャンセルのため
			// 本RequestOperatorが依頼した依頼先へのキャンセル要求は伝播させない
			this->requestedMngId = 0;
		}
	}

	/**
	* 依頼先SIDを取得する
	*
	* @author	Nagoya University
	* @date	2020/01/07
	* @return destSID	依頼先SID
	*
	*/
	//unsigned int RequestOperator::getDestSID()
	unsigned long long RequestOperator::getDestSID()
	{
		return dstSID;
	}
}
