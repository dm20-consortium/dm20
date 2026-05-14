#include "CsReceiver.h"

using namespace std;

namespace IS {
	BufferQueue cs_rcv_q(100000);

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	CsReceiver::CsReceiver()
	{
		// CSManagerの起動
		// TODO : シーケンス番号などを管理するのであれば必要？
		//IS::QueueManager &QM = IS::QueueManager::get_instance();
		//QM.init();
	}

	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	CsReceiver::~CsReceiver()
	{
	}

	/**
	 * 受信処理を開始する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 */

	void CsReceiver::start()
	{
		IS::Settings &settings = IS::Settings::get_instance();
		int max_worker_thread = std::stoi(settings.getParameter("CS_RECEIVE_WORKER_NUM"));
		string fd_name_str = settings.getFDDirectory() + FD_CStoIS;
		thread recvThread(&CsReceiver::run_is, this, fd_name_str, bind(&CsReceiver::notify, this, std::placeholders::_1));
		recvThread.detach();

		for (int i = 0; i < max_worker_thread; i++) {
			thread workerThread(&CsReceiver::workerProc, this, ref(cs_rcv_q));
			workerThread.detach();
		}
		logger->debug("CsReceiveWorker thread created :" + std::to_string(max_worker_thread));
	}

	/**
	 * CS経由でデータを通知された際のコールバック関数
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	buffer	受信バッファ
	 */

	void CsReceiver::notify(send_message_upper buffer)
	{
		
		// TODO : CSが分割送信を対応するまでの暫定対応 //
		//string receiveData, integData;
		//receiveData = buffer.dm2_payload;
		//dataIntegration(receiveData, integData);
		//if (integData.length() > 0) {
		//	cs_rcv_q.Push(integData);
		//}
		RecvData data;
		StringUtil stringUtil;
		data.schema_name = "";
		data.cs_ip_address = buffer.from_ip;
		data.payload = buffer.dm2_payload;
		char *payload_p = &data.payload[0];
		struct IsHeaderInfo headerInfo;
		stringUtil.getIsHeader(payload_p, headerInfo);
		logger->debug("CsReceiver - Flg:" + headerInfo.header.compressFlg);
		int len = 0;
		if (headerInfo.header.compressFlg == '1' || headerInfo.header.compressFlg == '2') {
			char bufTmp[IPv4_UDP_MAX_BYTE * 10];
			memset(bufTmp, 0, sizeof(bufTmp));
			if (headerInfo.header.compressFlg == '1') {
				len = stringUtil.decompress(headerInfo.payload_p, bufTmp);
			} else {
				int outSize;
				if (stringUtil.decompressUsingZstd(headerInfo.payload_p, bufTmp, headerInfo.header.length, &outSize)) {
					len = outSize;
				}
			}
			logger->debug("CsReceiver - Length:" + std::to_string(len));
			if (len > 0) {
				data.payload = string(bufTmp, len);
				char *payload_p = &data.payload[0];
				// 解凍されたバッファから再度ヘッダ情報を読み取る
				stringUtil.getIsHeader(payload_p, headerInfo);
			}
		}
		if (headerInfo.header.compressFlg == '0') {
			data.payload = string(headerInfo.payload_p, data.payload.length() - headerInfo.headerSize);
			len = data.payload.length();
		} else if (headerInfo.header.compressFlg == '3') {
			data.schema_name = headerInfo.header.schema_name;
			data.payload = string(headerInfo.payload_p, data.payload.length() - headerInfo.headerSize);
			len = data.payload.length();
		} else {
			len = data.payload.length();
		}

		logger->debug("IS_RCV SrcSID:" + std::to_string(buffer.src_station_id) + ", LaneID:" + std::to_string(buffer.lane_id) + ", pay_load:" + data.payload + "\n");
		if (len > 0) cs_rcv_q.Push(data);

	}

	void CsReceiver::workerProc(BufferQueue& q)
	{
		RecvData data;
		string user = "";
		IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		IS::QueueManager &QueuM = IS::QueueManager::get_instance();
		IS::QueryManager &QeryM = IS::QueryManager::get_instance();

		while (1) {
			q.Pop(data);

#if MEASURE_MODE == 1
			long procTime = DmUtil::getTimeMicrosec();
#endif
			if (data.schema_name != "") {
				QueuM.addQueueProc(data);
			} else {
				// XMLの種別を取得
				string rootTagName;
				isp.init();
				if (!isp.getRootTagName(data.payload, rootTagName)) {
					isp.finalize();
					continue;
				}
				isp.finalize();
#if MEASURE_MODE == 1
				long now = DmUtil::getTimeMicrosec();
				double msec = (now - procTime) / 1000.0;
				cout << "[notify] getRootTagName processTime(msec):" << msec << endl;
#endif
				// データ転送の場合
				if ("stream" == rootTagName)
				{
					logger->debug("[UDP(CS)] STREAM received data. Size:" + std::to_string(data.payload.length()));
					// QueueManagerにQueuein処理を委譲
					//QueuM.addQueue(data);
					QueuM.addQueueProc(data);
				}
				// クエリ転送の場合
				else if ("operatorTree" == rootTagName)
				{
					logger->debug("[UDP(CS)] OPERATORTREE received data. Size:" + std::to_string(data.payload.length()));
					// クエリの登録
					QeryM.addQuery(data.payload);

				}
				// クエリ結果の場合
				else if ("result" == rootTagName)
				{
					logger->debug("[UDP(CS)] RESULT received data. Size:" + std::to_string(data.payload.length()));
					// クエリの登録
					QeryM.registerResult(data.payload);

				}
				// クエリキャンセルの場合
				else if ("cancel" == rootTagName)
				{
					logger->debug("[UDP(CS)] CANCEL received data. Size:" + std::to_string(data.payload.length()));
					unsigned int mngId = isp.getAttrValueFromRootTag("id", data.payload);
					/*
					int receptionSID = isp.getAttrValueFromRootTag("receptionSID", data.payload);
					int senderSID = isp.getAttrValueFromRootTag("senderSID", data.payload);
					*/
					unsigned long long receptionSID = isp.getAttrValueULongLongFromRootTag("receptionSID", data.payload);
					unsigned long long senderSID = isp.getAttrValueULongLongFromRootTag("senderSID", data.payload);
					// 継続クエリのキャンセル要求
					QeryM.cancelQuery(mngId, receptionSID, senderSID);
				}
				else if ("query" == rootTagName)
				{
					logger->debug("[UDP(CS)] QUERY received data. Size:" + std::to_string(data.payload.length()));
					// クエリの登録
					int mngId = QeryM.getMngId();
					QeryM.addQuery(mngId, "public", data, false);;
				}
				else
				{
					logger->error("[ERROR] Not Define RootTagName. : " + rootTagName);
				}
			}
		}
	}

	/**
	* 受信データを統合する
	*
	* @author	Nagoya University
	* @date	2018/03/15
	*
	* @param [in,out]	buf	受信文字列
	* @param [in,out]	recvDataMap	受信途中のデータマップ
	* @param [in,out]	result	統合後の受信データ(全て受信していない場合は空文字を返却）
	*
	*/

	inline void CsReceiver::dataIntegration(string &buf, string &result)
	{
		StringUtil stringUtil;
		int flagment = 0, flagmentMax = 0;
		string key;

		// ヘッダ情報を取得する
		stringUtil.getHeaderInfo(buf, key, flagment, flagmentMax);
		logger->debug("UDP recv headerInfo  key:" + key + " flagment:" + std::to_string(flagment) + " max:" + std::to_string(flagmentMax));

		// resultMapに受信途中がないかチェック
		auto itr = recvDataMap.find(key);
		if (itr != recvDataMap.end()) {
			// 受信途中が存在する
			recvDataMap[key].at(flagment) = buf;
		}
		else {
			// 新規に受信
			vector<string> data(flagmentMax);
			data.at(flagment) = buf;
			recvDataMap[key] = data;
		}

		bool recvFinish = true;
		for (string str : recvDataMap[key]) {
			// 用意した配列に全てデータが入っていない場合は未受信データあり
			if (str.length() == 0) recvFinish = false;
		}

		if (recvFinish) {
			// 全てのデータを受信出来たらデータを繋ぎ合わせてMapは削除
			for (string str : recvDataMap[key]) {
				result.append(str);
			}
			recvDataMap.erase(key);
		}
	}
}
