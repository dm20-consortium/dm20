#ifndef EVALOPERATOR_H
#define EVALOPERATOR_H

#include "Operator.h"
#include "is/StringUtil.h"
#include "Settings.h"

#include <cstdlib>
#include <stdlib.h>
#include <dlfcn.h>

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
		// 使用するユーザ関数の定義パターン
		using multiFunc = vector<vector<string>>(*)(vector<vector<string>>);

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

		void* evalHandle = NULL;
		multiFunc evalFunc = NULL;
		bool evalLibLoaded = false;
		bool doWarnLog = true;
		void initializeEval(TupleSet& tupleset);
		long createArgList(TupleSet& tupleset, vector<vector<string>>& argList);
		void appendReturnValue(TupleSet& tupleset, const vector<vector<string>>& retList, const long &time);
		bool loadEvalFunction();
		bool executeEvalFunction(const vector<vector<string>>& argList, vector<vector<string>>& retList);
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
