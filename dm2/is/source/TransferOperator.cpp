#include "TransferOperator.h"

using std::cout;		// cout
using std::endl;		// endl
using namespace std;

namespace IS {

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	mngId	クエリ管理番号
	 * @param	dstSID	転送先SID
	 * @param	streamName	ストリーム名
	 * @param	retry		再送可否
	 * @param	lifeTime	再送時間
	 */

	//TransferOperator::TransferOperator(unsigned int mngId, int dstSID, string streamName, int retry, int lifeTime)
	TransferOperator::TransferOperator(unsigned int mngId, unsigned long long dstSID, string streamName, int retry, int lifeTime)
	{
		this->type = MyName + "_MNGID:" + std::to_string(mngId);
		this->dstSID = dstSID;
		this->streamName = streamName;
		this->mngId = mngId;
		this->retry = retry;
		this->lifeTime = lifeTime;
		this->argument.append(" DST_ID:" + to_string(dstSID));
		this->argument.append(" STREAM_NAME:" + streamName);
		this->argument.append(" RETRY:" + (retry == 0 ? "OFF" : "ON LIFE_TIME:" + to_string(lifeTime)));
		this->fdDirPath = settings.getFDDirectory();
	}

   /**
	* コンストラクタ(クエリ管理番号の返却)
	*
	* @author	Nagoya University
	* @date	2018/08/24
	*
	* @param	dstSID	転送先SID
	* @param	reqDstSID	リクエスト元から見た転送先SID
	* @param	mngId	クエリ管理番号
	* @param	requestedMngId	リクエスト元から発行されたクエリ管理番号
	* @param	retry		再送可否
	* @param	lifeTime	再送時間
	*/

	//TransferOperator::TransferOperator(unsigned int mngId, int dstSID, int reqDstSID, unsigned int requestedMngId, int retry, int lifeTime)
	TransferOperator::TransferOperator(unsigned int mngId, unsigned long long dstSID, unsigned long long reqDstSID, unsigned int requestedMngId, int retry, int lifeTime)
	{
		this->type = MyName + "_MNGID:" + std::to_string(mngId);
		this->dstSID = dstSID;
		this->reqDstSID = reqDstSID;
		this->mngId = mngId;
		this->requestedMngId = requestedMngId;
		this->retry = retry;
		this->lifeTime = lifeTime;
		this->argument.append(" DST_ID:" + to_string(dstSID));
		this->argument.append(" OWN_ID:" + to_string(reqDstSID));
		this->argument.append(" REQMNGID_ID:" + to_string(requestedMngId));
		this->argument.append(" RETRY:" + (retry == 0 ? "OFF" : "ON LIFE_TIME:" + to_string(lifeTime)));
		this->fdDirPath = settings.getFDDirectory();
	}

	/**
	* コンストラクタ(エラー番号の返却)
	*
	* @author	Nagoya University
	* @date	2018/08/24
	*
	* @param	dstSID	転送先SID
	* @param	reqDstSID	リクエスト元から見た転送先SID
	* @param	mngId	クエリ管理番号
	* @param	requestedMngId	リクエスト元から発行されたクエリ管理番号
	* @param	retry		再送可否
	* @param	lifeTime	再送時間
	* @param	code	エラー番号
	*/

	//TransferOperator::TransferOperator(unsigned int mngId, int dstSID, int reqDstSID, unsigned int requestedMngId, int retry, int lifeTime, IS::ErrorCode code)
	TransferOperator::TransferOperator(unsigned int mngId, unsigned long long dstSID, unsigned long long reqDstSID, unsigned int requestedMngId, int retry, int lifeTime, IS::ErrorCode code)
	{
		this->type = MyName + "_MNGID:" + std::to_string(mngId);
		this->dstSID = dstSID;
		this->reqDstSID = reqDstSID;
		this->mngId = mngId;
		this->requestedMngId = requestedMngId;
		this->retry = retry;
		this->lifeTime = lifeTime;
		this->code = code;
		this->argument.append(" DST_ID:" + to_string(dstSID));
		this->argument.append(" OWN_ID:" + to_string(reqDstSID));
		this->argument.append(" REQMNGID_ID:" + to_string(requestedMngId));
		this->argument.append(" RETRY:" + (retry == 0 ? "OFF" : "ON LIFE_TIME:" + to_string(lifeTime)));
		this->argument.append(" ERR_CODE:" + to_string((int)code));
		this->fdDirPath = settings.getFDDirectory();
	}

	/**
	* コンストラクタ(クエリ転送)
	*
	* @author	Nagoya University
	* @date	2025/01/20
	*
	* @param	dstSID	転送先SID
	* @param	retry		再送可否
	* @param	lifeTime	再送時間
	*/

	TransferOperator::TransferOperator(unsigned long long dstSID, int retry, int lifeTime)
	{
		this->type = MyName + "_MNGID:" + std::to_string(mngId);
		this->dstSID = dstSID;
		this->retry = retry;
		this->lifeTime = lifeTime;
		this->argument.append(" DST_ID:" + to_string(dstSID));
		this->argument.append(" REQMNGID_ID:" + to_string(requestedMngId));
		this->argument.append(" RETRY:" + (retry == 0 ? "OFF" : "ON LIFE_TIME:" + to_string(lifeTime)));
		this->fdDirPath = settings.getFDDirectory();
	}
	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	TransferOperator::~TransferOperator()
	{
		if (processNum != 0) {
			logger->debug("[PERFORMANCE_STAT][" + getType() + "] TotalProcAvgTime(NoRes) :" + to_string(totalProcessNoResTimeAVG) + "[ms] Fastest:" + to_string(totalProcessNoResTimeEarliest) + "[ms] Slowest:" + to_string(totalProcessNoResTimeSlowest) + "[ms] noResponseNum:" + to_string(processNum - notifiedNum));
			logger->debug("[PERFORMANCE_STAT][" + getType() + "] TotalProcAvgTime(Notify):" + to_string(totalProcessTimeAVG) + "[ms] Fastest:" + to_string(totalProcessTimeEarliest) + "[ms] Slowest:" + to_string(totalProcessTimeSlowest) + "[ms] notifiedNum:" + to_string(notifiedNum));
		}
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
	bool TransferOperator::process(vector<IS::TupleSet>& ts)
	{
		logger->debug("[" + this->type + "] ========== Transfer START ========== ");
		if (code != ErrorCode::NO_ERR) {
			logger->warn("[" + this->type + "] ERROR CODE: " + to_string((int)code) + ",CANCEL QUERY MNGID:" + std::to_string(mngId));
			QeryM.cancelQuery(mngId);
			return false;
		}

#if MEASURE_MODE == 1
		long startTime = DmUtil::getTimeMicrosec();
		long procTime = startTime;
		int step = 1;
#endif
		bool setProto = false;
		long tupleCnt = 0;
		// Transferは1つのtuplesetを使用
		TupleSet& tupleset = ts.at(0);

		// 返却するTupleが存在しない場合かつ、他ISからの要求でない場合は返却しない
		if (tupleset.size() == 0 && this->requestedMngId == 0) {
			long completeTime = DmUtil::getTimeMicrosec();
			if (tupleset.info.recvTime != 0) {
				double totalProcTime = ((completeTime - tupleset.info.recvTime) / 1000.0);
				if (totalProcessNoResTimeSlowest < totalProcTime) totalProcessNoResTimeSlowest = totalProcTime;
				if (totalProcessNoResTimeEarliest == 0 || totalProcessNoResTimeEarliest > totalProcTime) totalProcessNoResTimeEarliest = totalProcTime;
				totalProcessNoResTimeAVG = (double)((totalProcessNoResTimeAVG * (processNum - notifiedNum)) + totalProcTime) / (double)(processNum + 1 - notifiedNum);
				logger->debug("[PERFORMANCE][" + getType() + "] totalProcTime(NoRes):" + to_string(totalProcTime) + "[ms] AVGTime(NoRes):" + to_string(totalProcessNoResTimeAVG) + "[ms] procNum:" + to_string(processNum + 1 - notifiedNum));
			}
			logger->debug("[" + this->type + "] ========== Transfer  END  (No Tuple) ========== ");
			return false;
		}

		//自身のStationIDの読込(dm2.confから読み込む)
		string mySid = settings.getParameter("MY_STATION_ID");

		// DEBUG 与えられたタプル情報の出力
		printInputInfo(tupleset, this->argument);

		if (this->streamName.length() > 0) {
			// ストリーム名指定があればストリーム名を変更
			Schema schema = tupleset.getSchema();
			schema.renameTableName(tupleset.getSchemaRef().getTableName(), this->streamName);
			tupleset.SetSchema(schema);
		}
		string tblName = tupleset.getSchemaRef().getTableName();

		UdpSendInterface udpsendinterface;
		sockaddr_un server_addr = udpsendinterface.Init(this->fdDirPath + FD_IStoCS);
		struct send_message buf;
		buf.src_station_id = 0;
		buf.lane_id = 0;

		Schema schema = tupleset.getSchemaRef();
		string tablename = schema.getTableName();
		// 時刻情報を追加付与
		{
			long recvTime = DmUtil::getTimeMicrosec();
			for (int idx = 0; idx < tupleset.size(); idx++) {
				any val;
				int tsGrpIdx = schema.getAttributeIdx(tablename + "." + Schema::COL_TS_GRP);
				if (tsGrpIdx != -1) {
					tupleset.getTuple(idx).getValueByIdx(tsGrpIdx, val);
					string strVal = stringUtil.getAnyString(val) + to_string(recvTime) + ",";
					tupleset.getTuple(idx).setValue(tsGrpIdx, strVal, 0, false);
				}
			}
		}
		try {
			//buf.src_station_id = stoi(mySid);
			buf.src_station_id = stoull(mySid);
			buf.dst_station_id = dstSID;

			// 自身が車両のDBである場合はレーンIDを付与する
			if (settings.getSIDType() == Settings::SID_TYPE::CAR) {
				// レーンIDの付与
				string name = QueuM.getLaneIdSchema(tblName);
				if (name.length() == 0 || tupleset.size() == 0) {
					buf.lane_id = LM.getLaneId();
				}
				else {
					any val;
					tupleset.getTuple(0).getValueByIdx(tupleset.getSchemaRef().getAttributeIdx(name), val);
					//buf.lane_id = stoi(stringUtil.getAnyString(val));
					buf.lane_id = stoull(stringUtil.getAnyString(val));
				}
			}
			tupleCnt = tupleset.size();
		}
		catch (const std::invalid_argument&) {
			logger->error("[" + this->type + "][process] LINE:" + std::to_string(__LINE__) + " stoi エラー sid:" + to_string(buf.src_station_id) + " laneId:" + to_string(buf.lane_id));
		}

#if MEASURE_MODE == 1
		long now = DmUtil::getTimeMicrosec();
		double msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " read config parameter processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif
		string retXML;
		if (tablename == "object_info" || tablename == "object_info_processed") {
			setProto = true;
			IS::ProtobufParser &pp = IS::ProtobufParser::get_instance();
			retXML = pp.objectInfoSerializeToString_0_6_0(tupleset);
		} else if (tablename == "object_info_0_8_0" || tablename == "object_info_0_8_0_processed") {
			setProto = true;
			IS::ProtobufParser &pp = IS::ProtobufParser::get_instance();
			retXML = pp.objectInfoSerializeToString_0_8_0(tupleset);
		} else if (tablename == "object_info_0_8_1" || tablename == "object_info_0_8_1_processed") {
			setProto = true;
			IS::ProtobufParser &pp = IS::ProtobufParser::get_instance();
			retXML = pp.objectInfoSerializeToString_0_8_1(tupleset);
		} else if (tablename == "freespace_info") {
			setProto = true;
			IS::ProtobufParser &pp = IS::ProtobufParser::get_instance();
			retXML = pp.freespaceInfoSerializeToString_0_6_0(tupleset);
		} else if (tablename == "freespace_info_0_8_0") {
			setProto = true;
			IS::ProtobufParser &pp = IS::ProtobufParser::get_instance();
			retXML = pp.freespaceInfoSerializeToString_0_8_0(tupleset);
		} else if (tablename == "freespace_info_0_8_1") {
			setProto = true;
			IS::ProtobufParser &pp = IS::ProtobufParser::get_instance();
			retXML = pp.freespaceInfoSerializeToString_0_8_1(tupleset);
		} else if (tablename == "signal_info") {
			setProto = true;
			IS::ProtobufParser &pp = IS::ProtobufParser::get_instance();
			retXML = pp.signalInfoSerializeToString_0_6_0(tupleset);
		} else if (tablename == "sensor_info") {
			setProto = true;
			IS::ProtobufParser &pp = IS::ProtobufParser::get_instance();
			retXML = pp.sensorInfoSerializeToString_0_8_0(tupleset);
		} else if (tablename == "sensor_info_0_8_1") {
			setProto = true;
			IS::ProtobufParser &pp = IS::ProtobufParser::get_instance();
			retXML = pp.sensorInfoSerializeToString_0_8_1(tupleset);
		} else {
			//retXML = "";
			IS::ProtobufParser &pp = IS::ProtobufParser::get_instance();
			retXML = pp.SerializeToStringDynamically(tupleset);
			if (retXML != "") {
				setProto = true;
			} else {
				// START CSが分割送信を対応するまでの暫定対応 (CSを使わない場合は自身で分割する必要があるためコメントで残す)//
				//vector<string> retXMLList;
				//int realLength = 0;	// 分割ヘッダなしの電文長
				IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
				isp.init();
				// ストリームXMLを生成する
				// データ転送の電文生成
				if (requestedMngId == 0) {
					int attrSize = tupleset.getSchemaRef().getAttributeSize();
					if (tblName == tupleset.info.orgTableName && attrSize > tupleset.info.orgAttributeSize) {
						// ユーザ定義関数を使用した場合、元のSchemaにない新たなAttributeとしてTupleSet内のSchemaに追加されている。
						// そのまま転送すると、転送先で属性数チェックでエラーとなるため、除去する。
						tupleset.removeColumnOverOrgSize();
					}
					//realLength = isp.createStreamXMLList(mySid, std::to_string(dstSID), tupleset, IPv4_UDP_MAX_BYTE, retXMLList);
					// タプルが大量にある場合においてDOMAPIはコストがかかるので文字列結合にてXMLを生成する
					//isp.createStreamXML(mySid, std::to_string(dstSID), tupleset, retXML);
					isp.createStreamXMLStr(mySid, std::to_string(dstSID), tupleset, retXML);
					//cout << "[process]retXML [" << retXML.substr(0,50) << "]"<< endl;
					// END CSが分割送信を対応するまでの暫定対応 //
				}
				// クエリ管理番号の返却
				else {
					//realLength = isp.createQueryResultList(std::to_string(this->reqDstSID), std::to_string(dstSID), mngId, requestedMngId, tupleset, IPv4_UDP_MAX_BYTE, retXMLList);
					isp.createQueryResult(mySid, std::to_string(dstSID), mngId, requestedMngId, std::to_string(this->reqDstSID), tupleset, retXML);
				}
				isp.finalize();
			}
		}
		if (retXML == "") return true;

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " create xml processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif
		// 送信処理
		if (setProto) {
			string length_s = to_string(tablename.length());
			if (length_s.length() == 1) {
				retXML = "30" +length_s + tablename + retXML;
			} else {
				retXML = "3" + length_s + tablename + retXML;
			}
		}
		char compressFlg = settings.getParameter("COMPRESS_FLG")[0];
		if (compressFlg == '1' || compressFlg == '2') {
			char outbuf[UNCOMPRESSED_BUF_SIZE];
			long key = DmUtil::getTimeMicrosec();
			int sendSize = stringUtil.setCompressedBufWithHeader(retXML, outbuf, compressFlg, key);
			if (sendSize > 0) {
				udpsendinterface.IsStreamSendtoCs(server_addr, buf.lane_id, buf.src_station_id, buf.dst_station_id, this->retry, this->lifeTime, outbuf, sendSize, fdDirPath);
			} else {
				logger->warn("[" + this->type + "] CompressProc is Failed. Retry by Uncompressed Data");
				string s = "0" + retXML;
				udpsendinterface.IsStreamSendtoCs(server_addr, buf.lane_id, buf.src_station_id, buf.dst_station_id, this->retry, this->lifeTime, s, fdDirPath);
			}
			if (tupleCntSizeMap.find(tupleCnt) == tupleCntSizeMap.end()) {
				tupleCntSizeMap[tupleCnt] = sendSize;
				logger->info("[" + this->type + "] " + tablename + " CompressInfo[before/after/count]," + to_string(retXML.length()) + "/" + to_string(sendSize) + "/" + to_string(tupleCnt));
			}
		} else {
			string s = retXML;
			udpsendinterface.IsStreamSendtoCs(server_addr, buf.lane_id, buf.src_station_id, buf.dst_station_id, this->retry, this->lifeTime, s, fdDirPath);
		}
		logger->debug("[" + this->type + "] Transfer by UDP(CS).  MNGID:" + std::to_string(mngId) + " sendto(dstId):" + std::to_string(buf.dst_station_id) + " srcId:" + std::to_string(buf.src_station_id) + " Size:" + std::to_string(retXML.length()) + " byte TupleSize:" + std::to_string(tupleset.size()) );
		logger->debug("[" + this->type + "] Send payload:" + retXML + "\n");

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " sendto(by CS) processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

		long completeTime = DmUtil::getTimeMicrosec();
		if (tupleset.info.recvTime != 0) {
			double totalProcTime = ((completeTime - tupleset.info.recvTime) / 1000.0);
			if (totalProcessTimeSlowest < totalProcTime) totalProcessTimeSlowest = totalProcTime;
			if (totalProcessTimeEarliest == 0 || totalProcessTimeEarliest > totalProcTime) totalProcessTimeEarliest = totalProcTime;
			totalProcessTimeAVG = (double)((totalProcessTimeAVG * (notifiedNum)) + totalProcTime) / (double)(notifiedNum + 1);
			logger->debug("[PERFORMANCE][" + getType() + "] totalProcTime:" + to_string(totalProcTime) + "[ms] AVGTime:" + to_string(totalProcessTimeAVG) + "[ms] procNum:" + to_string(processNum + 1) + " notifiedNum:" + to_string(++notifiedNum));
		}
		if (tupleset.size() == 0 && (Operator::isDataReady() == false)) {
			// 継続クエリの管理番号返却時、キャンセル要求時はexecuteを通らないため終了可能
			exitReady = true;
		}

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - startTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " total processing time: " + to_string(msec) + "[ms]");
#endif

		logger->debug("[" + this->type + "] ========== Transfer  END  ========== ");
		return true;
	}
	/**
	* オペレータ処理
	*
	* @author       Nagoya University
	* @date 2025/01/20
	*
	* @param [in]       queryXML      クエリXML
	*
	* @return       正常にデータ処理を実施でき、次のオペレータに渡す際にtrue
	*/
	bool TransferOperator::process(string queryXML)
	{
			logger->debug("[" + this->type + "] ========== Transfer START ========== ");
			if (code != ErrorCode::NO_ERR) {
					logger->warn("[" + this->type + "] ERROR CODE: " + to_string((int)code) + ",CANCEL QUERY MNGID:" + std::to_string(mngId));
					return false;
			}

#if MEASURE_MODE == 1
			long startTime = DmUtil::getTimeMicrosec();
			long procTime = startTime;
			int step = 1;
#endif
			if (queryXML.size() == 0) {
					logger->debug("[" + this->type + "] ========== Transfer  END  (No QueryXML) ========== ");
					return false;
			}

			//自身のStationIDの読込(dm2.confから読み込む)
			string mySid = settings.getParameter("MY_STATION_ID");

			UdpSendInterface udpsendinterface;
			sockaddr_un server_addr = udpsendinterface.Init(this->fdDirPath + FD_IStoCS);
			struct send_message buf;
			
			buf.src_station_id = stoull(mySid);
			buf.dst_station_id = dstSID;

			// 送信処理
			char compressFlg = settings.getParameter("COMPRESS_FLG")[0];
			if (compressFlg == '1' || compressFlg == '2') {
					char outbuf[UNCOMPRESSED_BUF_SIZE];
					long key = DmUtil::getTimeMicrosec();
					int sendSize = stringUtil.setCompressedBufWithHeader(queryXML, outbuf, compressFlg, key);
					if (sendSize > 0) {
							udpsendinterface.IsStreamSendtoCs(server_addr, 0, buf.src_station_id, buf.dst_station_id, this->retry, this->lifeTime, outbuf, sendSize, fdDirPath);
					} else {
							logger->warn("[" + this->type + "] CompressProc is Failed. Retry by Uncompressed Data");
							string s = "0" + queryXML;
							udpsendinterface.IsStreamSendtoCs(server_addr, 0, buf.src_station_id, buf.dst_station_id, this->retry, this->lifeTime, s, fdDirPath);
					}
			} else {
					string s = queryXML;
					udpsendinterface.IsStreamSendtoCs(server_addr, 0, buf.src_station_id, buf.dst_station_id, this->retry, this->lifeTime, s, fdDirPath);
			}
			logger->debug("[" + this->type + "] Transfer by UDP(CS).  MNGID:" + std::to_string(mngId) + " sendto(dstId):" + std::to_string(buf.dst_station_id) + " srcId:" + std::to_string(buf.src_station_id) + " Size:" + std::to_string(queryXML.length()) + " byte");
			logger->debug("[" + this->type + "] Send payload:" + queryXML + "\n");

#if MEASURE_MODE == 1
			now = DmUtil::getTimeMicrosec();
			msec = (now - startTime) / 1000.0;
			logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " total processing time: " + to_string(msec) + "[ms]");
#endif

			logger->debug("[" + this->type + "] ========== Transfer  END  ========== ");
			return true;
	}

}
