#ifndef RECURSIVEOPERATOR_H
#define RECURSIVEOPERATOR_H

#include <log4cxx/logger.h>

#include "Operator.h"
#include "is/StringUtil.h"
#include "QueueManager.h"

using namespace log4cxx;

namespace IS {

	/**
	 * 再帰処理を扱うオペレータ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	class RecursiveOperator : public Operator
	{
	private:
		const string MyName = "Recursive";
		LoggerPtr logger = Logger::getLogger("RecursiveOperator");
		StringUtil stringUtil;
		string masterName = "";
	public:
		RecursiveOperator(unsigned int mngId, string param, string masterName);
		virtual ~RecursiveOperator();

		// 処理
		virtual bool process(vector<IS::TupleSet>& ts);
	};
}
#endif  // RECURSIVEOPERATOR_H