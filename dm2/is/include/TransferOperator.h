#ifndef TRANSFEROPERATOR_H
#define TRANSFEROPERATOR_H

#include <log4cxx/logger.h>

#include "Operator.h"
#include "is/StringUtil.h"
#include "QueueManager.h"
#include "QueryManager.h"
#include "LocationManager.h"
#include "UdpSendInterface.h"
#include "InformationSourceParser.h"
#include "ProtobufParser.h"
#include "Settings.h"

using namespace CS;
using namespace log4cxx;

namespace IS {

	/**
	 * ストリームデータなどの転送処理を扱うオペレータ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	class TransferOperator : public Operator, public UdpSendInterface
	{
	private:
		const string MyName = "Transfer";
		LoggerPtr logger = Logger::getLogger("TransferOperator");
		StringUtil stringUtil;
		IS::Settings &settings = IS::Settings::get_instance();
		IS::QueueManager &QueuM = IS::QueueManager::get_instance();
		IS::QueryManager &QeryM = IS::QueryManager::get_instance();
		IS::LocationManager &LM = IS::LocationManager::get_instance();

		string fdDirPath = "";
		unsigned long long dstSID = 0;
		unsigned long long reqDstSID = 0;
		string streamName = "";
		unsigned int requestedMngId = 0;
		int retry = 0;
		int lifeTime = 0;

		map<long, long> tupleCntSizeMap;
		
		bool exit_flag = false;
		ErrorCode code = IS::ErrorCode::NO_ERR;

		// レスポンスなし時間
		double totalProcessNoResTimeAVG = 0;
		double totalProcessNoResTimeSlowest = 0;
		double totalProcessNoResTimeEarliest = 0;
		// レスポンスあり時間
		unsigned int notifiedNum = 0;
		double totalProcessTimeAVG = 0;
		double totalProcessTimeSlowest = 0;
		double totalProcessTimeEarliest = 0;

	public:
		TransferOperator(unsigned int mngId, unsigned long long dstSID, string streamName, int retry, int lifeTime);
		TransferOperator(unsigned int mngId, unsigned long long dstSID, unsigned long long reqDestId, unsigned int requestedMngId, int retry, int lifeTime);
		TransferOperator(unsigned int mngId, unsigned long long dstSID, unsigned long long reqDestId, unsigned int requestedMngId, int retry, int lifeTime, IS::ErrorCode code);
		TransferOperator(unsigned long long dstSID, int retry, int lifeTime);
		virtual ~TransferOperator();

		// 処理
		virtual bool process(vector<IS::TupleSet>& ts);
		bool process(string queryXML);

		//int getLaneId(TupleSet &ts);
		unsigned long long getLaneId(TupleSet &ts);
	};
}
#endif  // TRANSFEROPERATOR_H
