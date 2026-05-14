#ifndef UNIONOPERATOR_H
#define UNIONOPERATOR_H

#include <log4cxx/logger.h>

#include "Operator.h"
#include "is/StringUtil.h"

using namespace log4cxx;

namespace IS {

	/**
	 * 和集合操作を扱うオペレータ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	class UnionOperator : public Operator
	{
	private:
		const string MyName = "Union";
		LoggerPtr logger = Logger::getLogger("UnionOperator");
		StringUtil stringUtil;
		int previousAttrSize1 = 0;
		int previousAttrSize2 = 0;
		std::map<long, TupleSet> waitQMap;
	public:
		UnionOperator(unsigned int mngId, string param);
		virtual ~UnionOperator();

		// 処理
		virtual bool process(vector<IS::TupleSet>& ts);
		virtual void receiveTupleSet(TupleSet &ts);
	};
}

#endif  // UNIONOPERATOR_H