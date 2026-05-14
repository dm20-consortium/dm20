#ifndef RENAMETABLEOPERATOR_H
#define RENAMETABLEOPERATOR_H

#include <log4cxx/logger.h>

#include "Operator.h"
#include "is/StringUtil.h"

using namespace log4cxx;

namespace IS {

	/**
	 * テーブル名変更操作を扱うオペレータ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	class RenameTableOperator : public Operator
	{
	private:
		const string MyName = "RenameTable";
		LoggerPtr logger = Logger::getLogger("RenameTableOperator");
		StringUtil stringUtil;
		vector<string> fromList, toList;
		IS::Schema inputSchema;
		IS::Schema outputSchema;
	public:
		RenameTableOperator(unsigned int mngId, string param);
		virtual ~RenameTableOperator();

		// 処理
		virtual bool process(vector<IS::TupleSet>& ts);
	};
}

#endif  // RENAMETABLEOPERATOR_H