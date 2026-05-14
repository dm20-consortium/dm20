#ifndef JOINOPERATOR_H
#define JOINOPERATOR_H

#include "Operator.h"
#include "is/StringUtil.h"
#include "Condition.h"

#include <functional>
#include <unordered_map>
#include <log4cxx/logger.h>

using namespace log4cxx;

namespace IS {

	/**
	 * 複数表の結合操作を扱うオペレータ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	class JoinOperator : public Operator
	{
	private:
		const string MyName = "Join";
		LoggerPtr logger = Logger::getLogger("JoinOperator");
		StringUtil stringUtil;
		string parameter = "";
		vector<string> paramList;
		vector<Condition> conditions;
		vector<bool> isReverseArgList;
		IS::Schema outputSchema;
		bool isUseHashJoin = false;
		vector<int> smallKeyIdxList, largeKeyIdxList;
		bool isReverse = false;

		std::map<long, TupleSet> waitQMap;

		void join(const Tuple &tuple1, const Tuple &tuple2, Tuple &joinTuple);
		bool hashJoin(TupleSet &tupleset1, TupleSet &tupleset2, TupleSet &retTupleSet);
		bool nestedLoopJoin(TupleSet &tupleset1, TupleSet &tupleset2, TupleSet &retTupleSet);
	public:
		JoinOperator(unsigned int mngId, string param);
		virtual ~JoinOperator();

		// 処理
		virtual bool process(vector<IS::TupleSet>& ts);
		virtual void receiveTupleSet(TupleSet &ts);
	};
}
#endif  // JOINOPERATOR_H