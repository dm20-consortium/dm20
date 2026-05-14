#include "Condition.h"

#include <string>

using namespace std;
using std::string;		// string

namespace IS {
	// 初期化
	const string Condition::LE  = "<=";
	const string Condition::GE  = ">=";
	const string Condition::NE  = "<>";
	const string Condition::NE2 = "!=";
	const string Condition::EQ  = "=";
	const string Condition::LT  = "<";
	const string Condition::GT  = ">";

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	Condition::Condition()
	{
	}

	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	Condition::~Condition()
	{
	}


	/**
	 * stateからcondition情報を生成する
	 * tupleはターゲットの型情報を識別するために必要
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	schema	スキーマ情報
	 * @param	state 	評価式
	 * @param[in,out]	cond 	コンディション
	 *
	 * @return	True/false.
	 */

	bool Condition::getCondition(const Schema &schema, const string &state, Condition &cond)
	{
		vector<string> stateList;
		// 比較演算子の判定、およびsplit文字列の判定
		if (state.find(Condition::LE) != string::npos) {
			cond.cond = Condition::LE;
			 stringUtil.split(state, Condition::LE, stateList);
		}
		else if (state.find(Condition::GE) != string::npos) {
			cond.cond = Condition::GE;
			stringUtil.split(state, Condition::GE, stateList);
		}
		else if (state.find(Condition::NE) != string::npos) {
			cond.cond = Condition::NE;
			stringUtil.split(state, Condition::NE, stateList);
		}
		else if (state.find(Condition::NE2) != string::npos) {
			cond.cond = Condition::NE;
			stringUtil.split(state, Condition::NE2, stateList);
		}
		else if (state.find(Condition::EQ) != string::npos) {
			cond.cond = Condition::EQ;
			stringUtil.split(state, Condition::EQ, stateList);
		}
		else if (state.find(Condition::LT) != string::npos) {
			cond.cond = Condition::LT;
			stringUtil.split(state, Condition::LT, stateList);
		}
		else if (state.find(Condition::GT) != string::npos) {
			cond.cond = Condition::GT;
			stringUtil.split(state, Condition::GT, stateList);
		}

		any value;
		int valueIdx = 0;
		if(schema.isExistAttribute(stateList.at(0)) && !schema.isExistAttribute(stateList.at(1))) {
			//REL_COMMENT logger->trace("colName is left." + stateList.at(0) + " " + stateList.at(1));
			// 列名が左項
			cond.fieldName = stateList.at(0);
			valueIdx = 1;
			cond.valuePosition = Condition::Val_pos::right;
		}
		else if (!schema.isExistAttribute(stateList.at(0)) && schema.isExistAttribute(stateList.at(1))) {
			//REL_COMMENT logger->trace("colName is right." + stateList.at(0) + " " + stateList.at(1));
			// 列名が右項
			cond.fieldName = stateList.at(1);
			valueIdx = 0;
			cond.valuePosition = Condition::Val_pos::left;
		}
		else {
			//REL_COMMENT logger->trace("colName is both." + stateList.at(0) + " " + stateList.at(1));
			// 両項が列名
			cond.fieldName = stateList.at(0);
			cond.fieldName2 = stateList.at(1);

			if (schema.isExistAttribute(cond.fieldName)) {
				cond.fieldIdx = schema.getAttributeIdx(cond.fieldName);
			}
			if (schema.isExistAttribute(cond.fieldName2)) {
				cond.fieldIdx2 = schema.getAttributeIdx(cond.fieldName2);
			}
			if (cond.fieldIdx == -1 || cond.fieldIdx2 == -1) {
				return false;
			}
			cond.valuePosition = Condition::Val_pos::nothing;
			//REL_COMMENT logger->trace("fieldName1:" + cond.fieldName + " index:" + to_string(cond.fieldIdx) + " type:" + schema.getAttributeType(cond.fieldIdx) + "  fieldName2:" + cond.fieldName2 + " index:" + to_string(cond.fieldIdx2) + " type:" + schema.getAttributeType(cond.fieldIdx2));
			return true;
		}
		
		try {
			int now_flag = 0;
			if (stateList.at(valueIdx).find("unix_now") != std::string::npos || stateList.at(valueIdx).find("UNIX_NOW") != std::string::npos) {
				now_flag = 1;
			} else if (stateList.at(valueIdx).find("its_now") != std::string::npos || stateList.at(valueIdx).find("ITS_NOW") != std::string::npos) {
				now_flag = 2;
			}
			if (now_flag > 0) {
				long now = DmUtil::getTimeMillisec();
				if (now_flag == 2) now -= 1072915195000;
				string now_state = stateList.at(valueIdx);
				vector<string> nowList;
				if (now_state.find("+") != std::string::npos) {
					stringUtil.split(now_state, "+", nowList);
					if (nowList.size() > 1) now += stol(nowList[1]);
				} else if (now_state.find("-") != std::string::npos) {
					stringUtil.split(now_state, "-", nowList);
					if (nowList.size() > 1) now -= stol(nowList[1]);
				} else if (now_state.find("*") != std::string::npos) {
					stringUtil.split(now_state, "*", nowList);
					if (nowList.size() > 1) now *= stol(nowList[1]);
				} else if (now_state.find("/") != std::string::npos) {
					stringUtil.split(now_state, "/", nowList);
					if (nowList.size() > 1) now /= stol(nowList[1]);
				} else if (now_state.find("%") != std::string::npos) {
					stringUtil.split(now_state, "%", nowList);
					if (nowList.size() > 1) now %= stol(nowList[1]);
				}
				stateList.at(valueIdx) = to_string(now);
			}
			cond.fieldIdx = schema.getAttributeIdx(cond.fieldName);
			if (cond.fieldIdx == -1) return false;

			if (stringUtil.isNumber(stateList.at(valueIdx))) {
				// number
				if (stringUtil.contain(stateList.at(valueIdx), ".")) {
					// double
					cond.val = stringUtil.getAnyValFromString(stateList.at(valueIdx), "double");
				}
				else if (stringUtil.contain(stateList.at(valueIdx), "-")) {
					// long
					cond.val = stringUtil.getAnyValFromString(stateList.at(valueIdx), "long");
				}
				else {
					// unsigned long long
					cond.val = stringUtil.getAnyValFromString(stateList.at(valueIdx), "ulong");
				}
			}
			else if (stringUtil.isString(stateList.at(valueIdx))) {
				// string
				if (stringUtil.contain(stateList.at(valueIdx), "'")) {
					vector<string> deleteChar = { "'" };
					stringUtil.deleteStrings(stateList.at(valueIdx), deleteChar);
				}
				cond.val = stringUtil.getAnyValFromString(stateList.at(valueIdx),"string");
			}
			else {
				// 比較先の型に合わせる
				cond.val = stringUtil.getAnyValFromString(stateList.at(valueIdx), schema.getAttributeType(cond.fieldIdx));
				if (cond.val.type() == typeid(void)) {
					logger->error("[getCondition] Not found fieldName:" + stateList.at(valueIdx) + " state:" + state);
					return false;
				}
			}

		}
		catch (std::exception& e) {
			string what(e.what());
			logger->error("what:" + what + " fieldName:" + cond.fieldName + " val:" + stateList.at(valueIdx));
			return false;
		}
		return true;
	}

	/**
	* condition情報と該当の情報を元に条件がマッチしているかを判定
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param	cond	コンディション情報
	* @param	val 	評価値
	*
	* @return	真の場合にtrue,偽の場合にfalseを返す
	*/

	bool Condition::isMatchCondition(const Condition &cond, const any &val)
	{
		try {
			if (cond.valuePosition == Condition::Val_pos::right) {
				// 各比較演算子を識別し、anyの型を意識して比較(固定値は右項)
				return isMatchCondition(cond, val, cond.val);
			}
			else {
				// 各比較演算子を識別し、anyの型を意識して比較(固定値は左項)
				return isMatchCondition(cond, cond.val, val);
			}
			return false;
		}
		catch (std::exception& e) {
			string what(e.what());
			logger->error("what:" + what);
			string valType = Demangle(val.type()).realname;
			string val2Type = Demangle(cond.val.type()).realname;
			logger->error("val      => fieldName  any_type:" + valType);
			logger->error("cond.val => fieldName:" + cond.fieldName + " index:" + to_string(cond.fieldIdx) + " any_type:" + val2Type);
			return false;
		}
	}

	/**
	* condition情報と該当の情報を元に条件がマッチしているかを判定
	*
	* @author	Nagoya University
	* @date	2018/10/10
	*
	* @param	cond	コンディション情報
	* @param	val 	評価値1
	* @param	val2 	評価値2
	*
	* @return	真の場合にtrue,偽の場合にfalseを返す
	*/

	bool Condition::isMatchCondition(const Condition &cond, const any &val, const any &val2)
	{
		int found = 0;
		try {
			// 各比較演算子を識別し、anyの型を意識して比較
			// !=
			// ==
			if (cond.cond == Condition::EQ) {
				if (val.type() == typeid(int)) {
					if (val2.type() == typeid(int) && ++found && any_cast<int>(val) == any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<int>(val) == any_cast<long>(val2)) return true;
					if (val2.type() == typeid(double) && ++found && any_cast<int>(val) == any_cast<double>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<int>(val) == any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<int>(val) == any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<int>(val) == any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(long)) {
					if (val2.type() == typeid(long) && ++found && any_cast<long>(val) == any_cast<long>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<long>(val) == any_cast<int>(val2)) return true;
					if (val2.type() == typeid(double) && ++found && any_cast<long>(val) == any_cast<double>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<long>(val) == any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<long>(val) == any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<long>(val) == any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(double)) {
					if (val2.type() == typeid(double) && ++found && any_cast<double>(val) == any_cast<double>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<double>(val) == any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<double>(val) == any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<double>(val) == any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<double>(val) == any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<double>(val) == any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(string)) {
					if (val2.type() == typeid(string) && ++found && any_cast<string>(val) == any_cast<string>(val2)) return true;
					if (val2.type() == typeid(char) && ++found) {
						char chs[1];
						chs[0] = any_cast<char>(val2);
						if (strcmp((any_cast<string>(val)).c_str(), chs) == 0) return true;
					}
				}
				else if (val.type() == typeid(char)) {
					if (val.type() == typeid(char) && ++found && any_cast<char>(val) == any_cast<char>(val2)) return true;
					if (val2.type() == typeid(string) && ++found) {
						char chs[1];
						chs[0] = any_cast<char>(val);
						if (strcmp(chs, (any_cast<string>(val2)).c_str()) == 0) return true;
					}
				}
				else if (val.type() == typeid(bool)) {
					if (val2.type() == typeid(bool) && ++found && any_cast<bool>(val) == any_cast<bool>(val2)) return true;
				}
				else if (val.type() == typeid(unsigned long long)) {
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<unsigned long long>(val) == any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<unsigned long long>(val) == any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<unsigned long long>(val) == any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<unsigned long long>(val) == any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<unsigned long long>(val) == any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<unsigned long long>(val) == any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(unsigned int)) {
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<unsigned int>(val) == any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<unsigned int>(val) == any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<unsigned int>(val) == any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<unsigned int>(val) == any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<unsigned int>(val) == any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<unsigned int>(val) == any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(uint64_t)) {
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<uint64_t>(val) == any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<uint64_t>(val) == any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<uint64_t>(val) == any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<uint64_t>(val) == any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<uint64_t>(val) == any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<uint64_t>(val) == any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(vector<unsigned long long>)) {
					++found;
					string valStr = "[";
					vector<unsigned long long> list = any_cast<vector<unsigned long long>>(val);
					for (unsigned int i = 0; i < list.size(); i++) {
						if (i > 0) valStr.append(",");
						valStr.append(stringUtil.getAnyString(list.at(i)));
					}
					valStr.append("]");
					string val2Str = stringUtil.getAnyString(val2);
					if (valStr == val2Str) return true;
					if (val2Str.find("_") != string::npos || val2Str.find("%") != string::npos) {
						if (matchRegexPattern(val2Str, valStr)) return true;
					}
				}
				else if (val.type() == typeid(vector<unsigned int>)) {
					++found;
					string valStr = "[";
					vector<unsigned int> list = any_cast<vector<unsigned int>>(val);
					for (unsigned int i = 0; i < list.size(); i++) {
						if (i > 0) valStr.append(",");
						valStr.append(stringUtil.getAnyString(list.at(i)));
					}
					valStr.append("]");
					string val2Str = stringUtil.getAnyString(val2);
					if (valStr == val2Str) return true;
					if (val2Str.find("_") != string::npos || val2Str.find("%") != string::npos) {
						if (matchRegexPattern(val2Str, valStr)) return true;
					}
				}
				else if (val.type() == typeid(vector<uint64_t>)) {
					++found;
					string valStr = "[";
					vector<uint64_t> list = any_cast<vector<uint64_t>>(val);
					for (unsigned int i = 0; i < list.size(); i++) {
						if (i > 0) valStr.append(",");
						valStr.append(stringUtil.getAnyString(list.at(i)));
					}
					valStr.append("]");
					string val2Str = stringUtil.getAnyString(val2);
					if (valStr == val2Str) return true;
					if (val2Str.find("_") != string::npos || val2Str.find("%") != string::npos) {
						if (matchRegexPattern(val2Str, valStr)) return true;
					}
				}
				else if (val.type() == typeid(vector<any>)) {
					++found;
					string valStr = "[";
					vector<any> list = any_cast<vector<any>>(val);
					for (unsigned int i = 0; i < list.size(); i++) {
						if (i > 0) valStr.append(",");
						valStr.append(stringUtil.getAnyString(list.at(i)));
					}
					valStr.append("]");
					//cout << Demangle(val2.type()) << endl;
					string val2Str = stringUtil.getAnyString(val2);
					if (valStr == val2Str) return true;
					if (val2Str.find("_") != string::npos || val2Str.find("%") != string::npos) {
						if (matchRegexPattern(val2Str, valStr)) return true;
					}
				}
			}
			else if (cond.cond == Condition::NE) {
				if (val.type() == typeid(int)) {
					if (val2.type() == typeid(int) && ++found && any_cast<int>(val) != any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<int>(val) != any_cast<long>(val2)) return true;
					if (val2.type() == typeid(double) && ++found && any_cast<int>(val) != any_cast<double>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<int>(val) >= any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<int>(val) >= any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<int>(val) >= any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(long)) {
					if (val2.type() == typeid(long) && ++found && any_cast<long>(val) != any_cast<long>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<long>(val) != any_cast<int>(val2)) return true;
					if (val2.type() == typeid(double) && ++found && any_cast<long>(val) != any_cast<double>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<long>(val) != any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<long>(val) != any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<long>(val) != any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(double)) {
					if (val2.type() == typeid(double) && ++found && any_cast<double>(val) != any_cast<double>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<double>(val) != any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<double>(val) != any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<double>(val) != any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<double>(val) != any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<double>(val) != any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(string)) {
					if (val2.type() == typeid(string) && ++found && any_cast<string>(val) != any_cast<string>(val2)) return true;
					if (val2.type() == typeid(char) && ++found) {
						char chs[1];
						chs[0] = any_cast<char>(val2);
						if (strcmp((any_cast<string>(val)).c_str(), chs) != 0) return true;
					}
				}
				else if (val.type() == typeid(char)) {
					if (val.type() == typeid(char) && ++found && any_cast<char>(val) != any_cast<char>(val2)) return true;
					if (val2.type() == typeid(string) && ++found) {
						char chs[1];
						chs[0] = any_cast<char>(val);
						if (strcmp(chs, (any_cast<string>(val2)).c_str()) != 0) return true;
					}
				}
				else if (val.type() == typeid(bool)) {
					if (val2.type() == typeid(bool) && ++found && any_cast<bool>(val) != any_cast<bool>(val2)) return true;
				}
				else if (val.type() == typeid(unsigned long long)) {
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<unsigned long long>(val) != any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<unsigned long long>(val) != any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<unsigned long long>(val) != any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<unsigned long long>(val) != any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<unsigned long long>(val) != any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<unsigned long long>(val) != any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(unsigned int)) {
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<unsigned int>(val) != any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<unsigned int>(val) != any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<unsigned int>(val) != any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<unsigned int>(val) != any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<unsigned int>(val) != any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<unsigned int>(val) != any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(uint64_t)) {
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<uint64_t>(val) != any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<uint64_t>(val) != any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<uint64_t>(val) != any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<uint64_t>(val) != any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<uint64_t>(val) != any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<uint64_t>(val) != any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(vector<unsigned long long>)) {
					++found;
					string valStr = "[";
					vector<unsigned long long> list = any_cast<vector<unsigned long long>>(val);
					for (unsigned int i = 0; i < list.size(); i++) {
						if (i > 0) valStr.append(",");
						valStr.append(stringUtil.getAnyString(list.at(i)));
					}
					valStr.append("]");
					string val2Str = stringUtil.getAnyString(val2);
					
					if (val2Str.find("_") != string::npos || val2Str.find("%") != string::npos) {
						if (!matchRegexPattern(val2Str, valStr)) return true;
					} else if (valStr != val2Str) {
						return true;
					}
				}
				else if (val.type() == typeid(vector<unsigned int>)) {
					++found;
					string valStr = "[";
					vector<unsigned int> list = any_cast<vector<unsigned int>>(val);
					for (unsigned int i = 0; i < list.size(); i++) {
						if (i > 0) valStr.append(",");
						valStr.append(stringUtil.getAnyString(list.at(i)));
					}
					valStr.append("]");
					string val2Str = stringUtil.getAnyString(val2);
					
					if (val2Str.find("_") != string::npos || val2Str.find("%") != string::npos) {
						if (!matchRegexPattern(val2Str, valStr)) return true;
					} else if (valStr != val2Str) {
						return true;
					}
				}
				else if (val.type() == typeid(vector<uint64_t>)) {
					++found;
					string valStr = "[";
					vector<uint64_t> list = any_cast<vector<uint64_t>>(val);
					for (unsigned int i = 0; i < list.size(); i++) {
						if (i > 0) valStr.append(",");
						valStr.append(stringUtil.getAnyString(list.at(i)));
					}
					valStr.append("]");
					string val2Str = stringUtil.getAnyString(val2);
					
					if (val2Str.find("_") != string::npos || val2Str.find("%") != string::npos) {
						if (!matchRegexPattern(val2Str, valStr)) return true;
					} else if (valStr != val2Str) {
						return true;
					}
				}
				else if (val.type() == typeid(vector<any>)) {
					++found;
					string valStr = "[";
					vector<any> list = any_cast<vector<any>>(val);
					for (unsigned int i = 0; i < list.size(); i++) {
						if (i > 0) valStr.append(",");
						valStr.append(stringUtil.getAnyString(list.at(i)));
					}
					valStr.append("]");
					string val2Str = stringUtil.getAnyString(val2);
					
					if (val2Str.find("_") != string::npos || val2Str.find("%") != string::npos) {
						if (!matchRegexPattern(val2Str, valStr)) return true;
					} else if (valStr != val2Str) {
						return true;
					}
				}
			}
			// <=
			else if (cond.cond == Condition::LE) {
				if (val.type() == typeid(int)) {
					if(val2.type() == typeid(int) && ++found && any_cast<int>(val) <= any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<int>(val) <= any_cast<long>(val2)) return true;
					if (val2.type() == typeid(double) && ++found && any_cast<int>(val) <= any_cast<double>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<int>(val) <= any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<int>(val) <= any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<int>(val) <= any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(long)) {
					if (val2.type() == typeid(long) && ++found && any_cast<long>(val) <= any_cast<long>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<long>(val) <= any_cast<int>(val2)) return true;
					if (val2.type() == typeid(double) && ++found && any_cast<long>(val) <= any_cast<double>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<long>(val) <= any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<long>(val) <= any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<long>(val) <= any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(double)) {
					if (val2.type() == typeid(double) && ++found && any_cast<double>(val) <= any_cast<double>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<double>(val) <= any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<double>(val) <= any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<double>(val) <= any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<double>(val) <= any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<double>(val) <= any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(string)) {
					if (val2.type() == typeid(string) && ++found && any_cast<string>(val) <= any_cast<string>(val2)) return true;
					if (val2.type() == typeid(char) && ++found) {
						char chs[1];
						chs[0] = any_cast<char>(val2);
						if (strcmp((any_cast<string>(val)).c_str(), chs) <= 0) return true;
					}
				}
				else if (val.type() == typeid(char)) {
					if (val.type() == typeid(char) && ++found && any_cast<char>(val) <= any_cast<char>(val2)) return true;
					if (val2.type() == typeid(string) && ++found) {
						char chs[1];
						chs[0] = any_cast<char>(val);
						if (strcmp(chs, (any_cast<string>(val2)).c_str()) <= 0) return true;
					}
				}
				else if (val.type() == typeid(bool)) {
					if (val2.type() == typeid(bool) && ++found && any_cast<bool>(val) <= any_cast<bool>(val2)) return true;
				}
				else if (val.type() == typeid(unsigned long long)) {
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<unsigned long long>(val) <= any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<unsigned long long>(val) <= any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<unsigned long long>(val) <= any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<unsigned long long>(val) <= any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<unsigned long long>(val) <= any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<unsigned long long>(val) <= any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(unsigned int)) {
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<unsigned int>(val) <= any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<unsigned int>(val) <= any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<unsigned int>(val) <= any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<unsigned int>(val) <= any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<unsigned int>(val) <= any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<unsigned int>(val) <= any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(uint64_t)) {
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<uint64_t>(val) <= any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<uint64_t>(val) <= any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<uint64_t>(val) <= any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<uint64_t>(val) <= any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<uint64_t>(val) <= any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<uint64_t>(val) <= any_cast<uint64_t>(val2)) return true;
				}
			}
			// >=
			else if (cond.cond == Condition::GE) {
				if (val.type() == typeid(int)) {
					if (val2.type() == typeid(int) && ++found && any_cast<int>(val) >= any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<int>(val) >= any_cast<long>(val2)) return true;
					if (val2.type() == typeid(double) && ++found && any_cast<int>(val) >= any_cast<double>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<int>(val) >= any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<int>(val) >= any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<int>(val) >= any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(long)) {
					if (val2.type() == typeid(long) && ++found && any_cast<long>(val) >= any_cast<long>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<long>(val) >= any_cast<int>(val2)) return true;
					if (val2.type() == typeid(double) && ++found && any_cast<long>(val) >= any_cast<double>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<long>(val) >= any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<long>(val) >= any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<long>(val) >= any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(double)) {
					if (val2.type() == typeid(double) && ++found && any_cast<double>(val) >= any_cast<double>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<double>(val) >= any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<double>(val) >= any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<double>(val) >= any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<double>(val) >= any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<double>(val) >= any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(string)) {
					if (val2.type() == typeid(string) && ++found && any_cast<string>(val) >= any_cast<string>(val2)) return true;
					if (val2.type() == typeid(char) && ++found) {
						char chs[1];
						chs[0] = any_cast<char>(val2);
						if (strcmp((any_cast<string>(val)).c_str(), chs) >= 0) return true;
					}
				}
				else if (val.type() == typeid(char)) {
					if (val.type() == typeid(char) && ++found && any_cast<char>(val) >= any_cast<char>(val2)) return true;
					if (val2.type() == typeid(string) && ++found) {
						char chs[1];
						chs[0] = any_cast<char>(val);
						if (strcmp(chs, (any_cast<string>(val2)).c_str()) >= 0) return true;
					}
				}
				else if (val.type() == typeid(bool)) {
					if (val2.type() == typeid(bool) && ++found && any_cast<bool>(val) >= any_cast<bool>(val2)) return true;
				}
				else if (val.type() == typeid(unsigned long long)) {
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<unsigned long long>(val) >= any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<unsigned long long>(val) >= any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<unsigned long long>(val) >= any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<unsigned long long>(val) >= any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<unsigned long long>(val) >= any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<unsigned long long>(val) >= any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(unsigned int)) {
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<unsigned int>(val) >= any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<unsigned int>(val) >= any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<unsigned int>(val) >= any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<unsigned int>(val) >= any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<unsigned int>(val) >= any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<unsigned int>(val) >= any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(uint64_t)) {
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<uint64_t>(val) >= any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<uint64_t>(val) >= any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<uint64_t>(val) >= any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<uint64_t>(val) >= any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<uint64_t>(val) >= any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<uint64_t>(val) >= any_cast<uint64_t>(val2)) return true;
				}
			}
			// <
			else if (cond.cond == Condition::LT) {
				if (val.type() == typeid(int)) {
					if (val2.type() == typeid(int) && ++found && any_cast<int>(val) < any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<int>(val) < any_cast<long>(val2)) return true;
					if (val2.type() == typeid(double) && ++found && any_cast<int>(val) < any_cast<double>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<int>(val) < any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<int>(val) < any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<int>(val) < any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(long)) {
					if (val2.type() == typeid(long) && ++found && any_cast<long>(val) < any_cast<long>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<long>(val) < any_cast<int>(val2)) return true;
					if (val2.type() == typeid(double) && ++found && any_cast<long>(val) < any_cast<double>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<long>(val) < any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<long>(val) < any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<long>(val) < any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(double)) {
					if (val2.type() == typeid(double) && ++found && any_cast<double>(val) < any_cast<double>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<double>(val) < any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<double>(val) < any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<double>(val) < any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<double>(val) < any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<double>(val) < any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(string)) {
					if (val2.type() == typeid(string) && ++found && any_cast<string>(val) < any_cast<string>(val2)) return true;
					if (val2.type() == typeid(char) && ++found) {
						char chs[1];
						chs[0] = any_cast<char>(val2);
						if (strcmp((any_cast<string>(val)).c_str(), chs) < 0) return true;
					}
				}
				else if (val.type() == typeid(char)) {
					if (val.type() == typeid(char) && ++found && any_cast<char>(val) < any_cast<char>(val2)) return true;
					if (val2.type() == typeid(string) && ++found) {
						char chs[1];
						chs[0] = any_cast<char>(val);
						if (strcmp(chs, (any_cast<string>(val2)).c_str()) < 0) return true;
					}
				}
				else if (val.type() == typeid(bool)) {
					if (val2.type() == typeid(bool) && ++found && any_cast<bool>(val) < any_cast<bool>(val2)) return true;
				}
				else if (val.type() == typeid(unsigned long long)) {
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<unsigned long long>(val) < any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<unsigned long long>(val) < any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<unsigned long long>(val) < any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<unsigned long long>(val) < any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<unsigned long long>(val) < any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<unsigned long long>(val) < any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(unsigned int)) {
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<unsigned int>(val) < any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<unsigned int>(val) < any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<unsigned int>(val) < any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<unsigned int>(val) < any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<unsigned int>(val) < any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<unsigned int>(val) < any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(uint64_t)) {
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<uint64_t>(val) < any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<uint64_t>(val) < any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<uint64_t>(val) < any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<uint64_t>(val) < any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<uint64_t>(val) < any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<uint64_t>(val) < any_cast<uint64_t>(val2)) return true;
				}
			}
			// >
			else if (cond.cond == Condition::GT) {
				if (val.type() == typeid(int)) {
					if (val2.type() == typeid(int) && ++found && any_cast<int>(val) > any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<int>(val) > any_cast<long>(val2)) return true;
					if (val2.type() == typeid(double) && ++found && any_cast<int>(val) > any_cast<double>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<int>(val) > any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<int>(val) > any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<int>(val) > any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(long)) {
					if (val2.type() == typeid(long) && ++found && any_cast<long>(val) > any_cast<long>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<long>(val) > any_cast<int>(val2)) return true;
					if (val2.type() == typeid(double) && ++found && any_cast<long>(val) > any_cast<double>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<long>(val) > any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<long>(val) > any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<long>(val) > any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(double)) {
					if (val2.type() == typeid(double) && ++found && any_cast<double>(val) > any_cast<double>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<double>(val) > any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<double>(val) > any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<double>(val) > any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<double>(val) > any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<double>(val) > any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(string)) {
					if (val2.type() == typeid(string) && ++found && any_cast<string>(val) > any_cast<string>(val2)) return true;
					if (val2.type() == typeid(char) && ++found) {
						char chs[1];
						chs[0] = any_cast<char>(val2);
						if (strcmp((any_cast<string>(val)).c_str(), chs) > 0) return true;
					}
				}
				else if (val.type() == typeid(char)) {
					if (val.type() == typeid(char) && ++found && any_cast<char>(val) > any_cast<char>(val2)) return true;
					if (val2.type() == typeid(string) && ++found) {
						char chs[1];
						chs[0] = any_cast<char>(val);
						if (strcmp(chs, (any_cast<string>(val2)).c_str()) > 0) return true;
					}
				}
				else if (val.type() == typeid(bool)) {
					if (val2.type() == typeid(bool) && ++found && any_cast<bool>(val) > any_cast<bool>(val2)) return true;
				}
				else if (val.type() == typeid(unsigned long long)) {
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<unsigned long long>(val) > any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<unsigned long long>(val) > any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<unsigned long long>(val) > any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<unsigned long long>(val) > any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<unsigned long long>(val) > any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<unsigned long long>(val) > any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(unsigned int)) {
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<unsigned int>(val) > any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<unsigned int>(val) > any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<unsigned int>(val) > any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<unsigned int>(val) > any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<unsigned int>(val) > any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<unsigned int>(val) > any_cast<uint64_t>(val2)) return true;
				}
				else if (val.type() == typeid(uint64_t)) {
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<uint64_t>(val) > any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(int) && ++found && any_cast<uint64_t>(val) > any_cast<int>(val2)) return true;
					if (val2.type() == typeid(long) && ++found && any_cast<uint64_t>(val) > any_cast<long>(val2)) return true;
					if (val2.type() == typeid(unsigned int) && ++found && any_cast<uint64_t>(val) > any_cast<unsigned int>(val2)) return true;
					if (val2.type() == typeid(unsigned long long) && ++found && any_cast<uint64_t>(val) > any_cast<unsigned long long>(val2)) return true;
					if (val2.type() == typeid(uint64_t) && ++found && any_cast<uint64_t>(val) > any_cast<uint64_t>(val2)) return true;
				}
			}
		}
		catch (std::exception& e) {
			string what(e.what());
			logger->error("what:" + what + " fieldName:" + cond.fieldName);
			string valType = Demangle(val.type()).realname;
			string val2Type = Demangle(val2.type()).realname;
			logger->error("val1=> fieldName :" + cond.fieldName + " index:" + to_string(cond.fieldIdx) + " any_type:" + valType);
			logger->error("val2=> fieldName2:" + cond.fieldName2 + " index:" + to_string(cond.fieldIdx2) + " any_type:" + val2Type);
		}
		if (found == 0) {
			string valType = Demangle(val.type()).realname;
			string val2Type = Demangle(val2.type()).realname;
			logger->error("No conditions were found for comparison.");
			logger->error("val1=> fieldName :" + cond.fieldName + " index:" + to_string(cond.fieldIdx) + " any_type:" + valType);
			logger->error("val2=> fieldName2:" + cond.fieldName2 + " index:" + to_string(cond.fieldIdx2) + " any_type:" + val2Type);
		}
		return false;
	}

	/**
	* SQLライクな正規表現がマッチしているかを判定
	*
	* @author	Shinichi Kusayama
	* @date	2025/2/20
	*
	* @param	pattern	SQLライクな正規表現
	* @param	text 	比較する文字列
	*
	* @return	真の場合にtrue,偽の場合にfalseを返す
	*/

	bool Condition::matchRegexPattern(const std::string& pattern, const std::string& text) {
		// `_` を `.` に変換し、`%` を `.*` に変換する
		std::string regexPattern;
		for (char c : pattern) {
			if (c == '_') {
				regexPattern += '.';  // `_` は任意の1文字
			} else if (c == '%') {
				regexPattern += ".*"; // `%` は任意の0文字以上
			} else {
				regexPattern += std::regex_replace(std::string(1, c), std::regex(R"([.^$|(){}[\]\\])"), R"(\$&)"); 
			}
			/* 下記は、[]を正規表現として含めたいケース用
			// bool insideCharacterClass = false; // `[` の中かどうかを判定
			} else if (c == '[') {
				regexPattern += '[';
				insideCharacterClass = true;  // 文字クラスの開始
			} else if (c == ']') {
				regexPattern += ']';
				insideCharacterClass = false; // 文字クラスの終了
			} else {
				if (!insideCharacterClass) {
					// `.` や `(` などの特殊文字をエスケープ
					regexPattern += std::regex_replace(std::string(1, c), std::regex(R"([.^$|(){}+?\\])"), R"(\$&)");
				} else {
					// `[` の中ならそのまま追加（特別なエスケープは不要）
					regexPattern += c;
				}
			}
			*/
		}

		// 正規表現としてマッチング
		std::regex re("^" + regexPattern + "$");
		return std::regex_match(text, re);
	}
}
