#ifndef EVALOPERATOR_H
#define EVALOPERATOR_H

#include "Operator.h"
#include "is/StringUtil.h"
#include "Settings.h"

#include <log4cxx/logger.h>

using namespace log4cxx;

namespace IS {

	/**
	 * ユーザ定義関数を扱うオペレータクラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	class EvalOperator : public Operator
	{
	private:
		const string MyName = "Eval";
		LoggerPtr logger = Logger::getLogger("EvalOperator");
		StringUtil stringUtil;
		IS::Settings &settings = IS::Settings::get_instance();
		string parameter = "";
		string functionName;

		// evalFunction.confからのパラメータ
		string libName = "";
		string argTypeStr = "";
		string retTypeStr = "";
		string retNameStr = "";

		vector<string> args, retsType, retsName;
		IS::Schema outputSchema;
		vector<int> argProcList;
		int previousAttributeNum = 0;
		enum procType {
			NUMBER = -1,
			STRING = -2,
			OTHER = -3
		};

	public:
		static const string  LIB_PREFIX;
		static const string  TYPE_PREFIX;

		EvalOperator(unsigned int mngId, string param);
		virtual ~EvalOperator();

		// 処理
		virtual bool process(vector<IS::TupleSet>& ts);
		bool checkParameter(string &msg, bool &doAggregate);
	};
}
#endif  // EVALOPERATOR_H
