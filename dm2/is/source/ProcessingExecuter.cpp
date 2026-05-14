
#include "ProcessingExecuter.h"

using namespace std;

namespace IS {

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/10/29
	 *
	 * @param	data	受信データ
	 * @param	func	処理文字列
	 */

	ProcessingExecuter::ProcessingExecuter(const RecvData &data, const string func)
	{
		this->replyInfo = data;
		this->funcName = func;
	}

	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/10/29
	 */

	ProcessingExecuter::~ProcessingExecuter()
	{
	}

	/**
	 * クエリ実行スレッド処理
	 *
	 * @author	Nagoya University
	 * @date	2018/10/29
	 */

	void ProcessingExecuter::run()
	{
		//REL_COMMENT logger->trace("[run] IN  funcName:" + this->funcName);
		IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		IS::StringUtil stringUtil;
		string ret = "";
		vector<string> cmdList;
		vector<vector<string>> dataList;
		stringUtil.split(this->funcName, "@", cmdList);
		isp.init();
		if ("recv_num" == cmdList.at(0)) {
			// ストリーム受信数の取得
			IS::QueueManager &QM = IS::QueueManager::get_instance();
			int num = 0;

			if (cmdList.size() > 1) {
				num = QM.getStreamRcvInfo(cmdList.at(1));
			}
			else {
				num = QM.getStreamRcvInfo();
			}
			vector<string> row;
			row.push_back(to_string(num));
			dataList.push_back(row);
			// XMLの生成
			isp.createGeneralDataListResponseXML(dataList, ret);

		}
		else if ("register_query" == cmdList.at(0)) {
			// クエリ登録数の取得
			IS::QueryManager &QM = IS::QueryManager::get_instance();
			int num = QM.getRegisterQueryNum();
			vector<string> row;
			row.push_back(to_string(num));
			dataList.push_back(row);
			// XMLの生成
			isp.createGeneralDataListResponseXML(dataList, ret);

		}
		else if ("query_info" == cmdList.at(0)) {
			// クエリ稼働状況の取得
			IS::QueryManager &QM = IS::QueryManager::get_instance();
			vector<QueryStatus> qsList = QM.getQueryStatus();
			for (QueryStatus qs : qsList) {
				vector<string> row;
				row.push_back(to_string(qs.mngId));
				stringUtil.deleteNl(qs.query);
				row.push_back(qs.query);
				row.push_back(qs.user);
				stringUtil.deleteNl(qs.operatorTreeXML);
				row.push_back(qs.operatorTreeXML);
				row.push_back(to_string(qs.operatorNum));
				row.push_back(to_string(qs.notifiedIntervalAVG));
				row.push_back(to_string(qs.totalProcessTimeAVG));
				row.push_back(to_string(qs.unprocessedDataNum));
				row.push_back(qs.latestOperatorName);
				row.push_back(to_string(qs.latestOpeProcAVG));
				row.push_back(to_string(qs.inputStreamTupleNumPerSec));
				row.push_back(to_string(qs.inputRelationTupleNumPerSec));
				row.push_back(to_string(qs.outputTupleNumPerSec));
				dataList.push_back(row);
			}
			// XMLの生成
			isp.createGeneralDataListResponseXML(dataList, ret);

		}
		else {
			string msg = "[ERROR] Not Define funcName. : " + this->funcName;
			logger->error(msg);
			returnError(ErrorCode::INVALID_ARG_ERR, msg);
			isp.finalize();
			return;
		}
		isp.finalize();

		// レスポンスの返却
		IS::ResponseOperator		*opX = new IS::ResponseOperator(replyInfo);
		logger->trace("[run] response send payload:" + ret);
		opX->TCPSend(ret);
		opX->exit();
		delete opX;

		//REL_COMMENT logger->trace("[run] OUT funcName:" + this->funcName);
	}



	/**
	 * クエリ実行処理の開始
	 *
	 * @author	Nagoya University
	 * @date	2018/10/29
	 */

	void ProcessingExecuter::start()
	{
		thread procThread(&ProcessingExecuter::run, this);
		procThread.detach();
	}


	/**
	* エラー返却処理
	*
	* @author	Nagoya University
	* @date	2018/11/02
	*
	* @param	code	The code.
	*/

	inline void ProcessingExecuter::returnError(ErrorCode code, const string &msg)
	{
		TupleSet tupleset;
		vector<TupleSet> ts;
		ts.push_back(tupleset);

		IS::ResponseOperator		*opX = new IS::ResponseOperator(0, replyInfo, code, msg);
		opX->process(ts);
		opX->exit();
		delete opX;

	}

}
