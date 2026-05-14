#ifndef DROPOPERATOR_H
#define DROPOPERATOR_H

#include <log4cxx/logger.h>

#include "Operator.h"
#include "is/StringUtil.h"
#include "QueueManager.h"
#include "QueryManager.h"
#include "is/DmUtil.h"
#include "ErrorCode.h"

using namespace log4cxx;

namespace IS {

	/**
	 * ストリームを削除するオペレータ
	 *
	 * @author	Nagoya University
	 * @date	2019/04/17
	 */

	class DropOperator : public Operator
	{
	private:
		const string MyName = "Drop";
		LoggerPtr logger = Logger::getLogger("DropOperator");
		StringUtil stringUtil;

		string user;
		string streamName = "";

	public:
		DropOperator(const string &user, const unsigned int mngId, const string &streamNam);
		virtual ~DropOperator();

		// 処理
		virtual bool process(vector<IS::TupleSet>& ts);
		virtual bool isExpectDataSize(unsigned int bufSize);
	};
}
#endif  // DROPOPERATOR_H