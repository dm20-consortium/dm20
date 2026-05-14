#ifndef CONDITION_H
#define CONDITION_H

#include "is/StringUtil.h"
#include "is/Schema.h"
#include "is/Tuple.h"
#include "is/DmUtil.h"

#include <string>
#include <string.h>
#include <iostream>
#include <log4cxx/logger.h>
#include <cassert>
#include <regex>
#include <climits>
using namespace std;
using namespace log4cxx;

namespace IS {

	/**
	 * 評価式を管理するクラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	class Condition
	{
	private:
		bool matchRegexPattern(const std::string& pattern, const std::string& text);
	public:
		Condition();
		~Condition();
		StringUtil stringUtil;
		LoggerPtr logger = Logger::getLogger("Condition");

		enum Val_pos {
			left = 0,
			right = 1,
			nothing = 2
		};

		string fieldName;
		string fieldName2;
		int fieldIdx = -1;
		int fieldIdx2 = -1;
		string cond = "";
		any val;
		Val_pos valuePosition;

		static const string LE;
		static const string GE;
		static const string NE;
		static const string NE2;
		static const string EQ;
		static const string LT;
		static const string GT;

		bool getCondition(const Schema &schema, const string &state, Condition &cond);
		bool isMatchCondition(const Condition &cond, const any &val);
		bool isMatchCondition(const Condition &cond, const any &val, const any &val2);
	};
}
#endif //CONDITION_H