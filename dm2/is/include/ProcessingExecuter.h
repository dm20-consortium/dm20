#ifndef PROCESSINGYEXECUTER_H
#define PROCESSINGYEXECUTER_H

// ProcessingExecuterクラス

#include <thread>
#include <log4cxx/logger.h>

#include "InformationSource.h"
#include "InformationSourceParser.h"
#include "is/Schema.h"
#include "is/Tuple.h"
#include "is/TupleSet.h"
#include "Operator.h"
#include "Parser.h"
#include "ErrorCode.h"
#include "NetworkSource.h"
#include "ResponseOperator.h"
#include "QueueManager.h"
#include "QueryManager.h"

using namespace std;
using namespace log4cxx;

namespace IS {

	/**
	 * クエリ実行処理
	 *
	 * @author	Nagoya University
	 * @date	2018/10/29
	 */

	class ProcessingExecuter
	{
	private:
		LoggerPtr logger = Logger::getLogger("ProcessingExecuter");
		RecvData replyInfo;
		string funcName;

		void run();
		void returnError(ErrorCode code, const string &msg);

	public:
		ProcessingExecuter(const RecvData &data, const string funcName);
		virtual ~ProcessingExecuter();

		void start();      // impl
	};
}
#endif  // PROCESSINGYEXECUTER_H