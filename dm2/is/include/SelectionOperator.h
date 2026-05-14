#ifndef SELECTIONOPERATOR_H
#define SELECTIONOPERATOR_H

#include <log4cxx/logger.h>

#include "Operator.h"
#include "is/StringUtil.h"
#include "Condition.h"

using namespace log4cxx;

namespace IS {

	/**
	 * 行抽出操作を扱うオペレータ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	class SelectionOperator : public Operator
	{
	private:
		const string MyName = "Selection";
		LoggerPtr logger = Logger::getLogger("SelectionOperator");
		StringUtil stringUtil;
		int previousAttributeNum = 0;
		vector<Condition> conditions;
		vector<string> priorityList = {
			Condition::EQ, 
			Condition::LT, 
			Condition::GT, 
			Condition::LE, 
			Condition::GE, 
			Condition::NE, 
			Condition::NE2 
		};

	public:
		SelectionOperator(unsigned int mngId, string param);
		virtual ~SelectionOperator();

		// 処理
		virtual bool process(vector<IS::TupleSet>& ts);
	};
}

#endif  // SELECTIONOPERATOR_H