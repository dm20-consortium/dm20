#ifndef CALCUTIL_H
#define CALCUTIL_H

#include <string>
#include <vector>
#include <log4cxx/logger.h>

using namespace std;
using namespace log4cxx;

namespace IS {

	/**
	 * 計算ユーティリティークラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	class CalcUtil
	{
	private:
		LoggerPtr logger = Logger::getLogger("CalcUtil");
	public:

		double CalcString(const string &state);


	};
}
#endif  // CALCUTIL_H

