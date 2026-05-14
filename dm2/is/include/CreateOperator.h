#ifndef CREATEOPERATOR_H
#define CREATEOPERATOR_H

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
	 * ストリームを生成するオペレータ
	 *
	 * @author	Nagoya University
	 * @date	2019/01/30
	 */

	class CreateOperator : public Operator
	{
	private:
		const string MyName = "Create";
		LoggerPtr logger = Logger::getLogger("CreateOperator");
		StringUtil stringUtil;

		string user;
		string streamName = "";
		string parameter = "";
		string streamClass = "";
		string classVal = "";
		string streamStartFix = "";
		string startFixVal = "";

	public:
		CreateOperator(const string &user, const unsigned int mngId, const string &streamName, const string &parameter, const string &streamClass, const string &classVal, const string &streamStartFix, const string &startFixVal);
		virtual ~CreateOperator();

		// 処理
		virtual bool process(vector<IS::TupleSet>& ts);
		virtual bool isExpectDataSize(unsigned int bufSize);
	};
}
#endif  // CREATEOPERATOR_H