#ifndef RENAMECOLUMNOPERATOR_H
#define RENAMECOLUMNOPERATOR_H

#include <log4cxx/logger.h>

#include "Operator.h"
#include "is/StringUtil.h"

using namespace log4cxx;

namespace IS {

	/**
	 * カラム名変更操作を扱うオペレータ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	class RenameColumnOperator : public Operator
	{
	private:
		const string MyName = "RenameColumn";
		LoggerPtr logger = Logger::getLogger("RenameColumnOperator");
		StringUtil stringUtil;
		vector<string> fromList, toList, retList;
		string fromStr, toStr;
		IS::Schema outputSchema;
	public:
		RenameColumnOperator(unsigned int mngId, string param);
		virtual ~RenameColumnOperator();

		// 処理
		virtual bool process(vector<IS::TupleSet>& ts);
	};
}
#endif  // RENAMECOLUMNOPERATOR_H