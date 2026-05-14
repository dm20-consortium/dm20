#ifndef PROJECTIONOPERATOR_H
#define PROJECTIONOPERATOR_H

#include "Operator.h"
#include "is/StringUtil.h"
#include "CalcUtil.h"

#include <log4cxx/logger.h>

using namespace log4cxx;

namespace IS {

	/**
	 * 列データ抽出操作を扱うオペレータ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	class ProjectionOperator : public Operator
	{
	private:
		const string MyName = "Projection";
		LoggerPtr logger = Logger::getLogger("ProjectionOperator");
		StringUtil stringUtil;
		CalcUtil calcUtil;
		string parameter = "";
		vector<string> paramList;
		vector<int> indexList;
		vector<int> invalidIndexList;
		IS::Schema outputSchema;
		int previousAttributeNum = 0;
		enum VAL_TYPE {
			NUMBER = -1,
			STRING = -2,
			CALC   = -3
		};
	public:
		ProjectionOperator(unsigned int mngId, string param);
		virtual ~ProjectionOperator();

		// 処理
		virtual bool process(vector<IS::TupleSet>& ts);
		bool calcCheck(const string &state);
	};
}

#endif  // PROJECTIONOPERATOR_H