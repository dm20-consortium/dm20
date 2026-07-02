
#include "is/StringUtil.h"

#include <iostream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <regex>

using std::right;
using std::setw;

using namespace std;

namespace IS {
	/**
	 * 対象文字列から空白を削除する。ただし、文字列内のスペース（"a " or 'a '）は削除しない。
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	buf	対象文字列
	 *
	 * @return	結果文字列
	 */
	string StringUtil::deleteSpace(string buf)
	{
		vector<string> innerListS, innerListD;

		// 文字列を含む場合は文字列内のスペースを削除しないようにする
		if (contain(buf, "'")) {
			innerListS = getStringInBracketFirst(buf, "'", "'");
			for (unsigned int i = 0; i < innerListS.size(); i++) {
				replaceOnce(buf, "'" + innerListS.at(i) + "'", "'replaceS" + to_string(i) + "'");
			}
		}
		if (contain(buf, "\"")) {
			innerListD = getStringInBracketFirst(buf, "\"", "\"");
			for (unsigned int i = 0; i < innerListD.size(); i++) {
				replaceOnce(buf, "\"" + innerListD.at(i) + "\"", "\"replaceD" + to_string(i) + "\"");
			}
		}

		size_t pos;
		while ((pos = buf.find_first_of(" 　\t")) != string::npos) {
			buf.erase(pos, 1);
		}

		for (unsigned int i = 0; i < innerListD.size(); i++) {
			if (contain(buf, "replaceD" + to_string(i))) {
				replaceOnce(buf, "replaceD" + to_string(i), innerListD.at(i));
			}
		}
		for (unsigned int i = 0; i < innerListS.size(); i++) {
			if (contain(buf, "replaceS" + to_string(i))) {
				replaceOnce(buf, "replaceS" + to_string(i), innerListS.at(i));
			}
		}
		return buf;
	}

	/**
	 * 対象文字列の前後の空白を削除する
	 *
	 * @author	Nagoya University
	 * @date	2019/03/15
	 *
	 * @param [in]	target	対象文字列
	 *
	 * @return	結果文字列
	 */
	string StringUtil::trim(const string& target)
	{
		string result;
		const char* trimCharacterList = " \t\v\r\n";

		// 左側からトリムする文字以外が見つかる位置を検索
		string::size_type left = target.find_first_not_of(trimCharacterList);

		if (left != string::npos)
		{
			// 左側からトリムする文字以外が見つかった場合は、同じように右側からも検索
			string::size_type right = target.find_last_not_of(trimCharacterList);
			result = target.substr(left, right - left + 1);
		}
		return result;
	}

	/**
	 * 指定された削除文字をtarget文字列から削除する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param [in,out]	target		 	対象文字列
	 * @param	deleteStrList	削除文字リスト
	 *
	 */
	void StringUtil::deleteStrings(string &target, const vector<string> &deleteStrList)
	{
		for (string deleteStr : deleteStrList) {
			for (size_t c = target.find_first_of(deleteStr); c != string::npos; c = target.find_first_of(deleteStr)) {
				target.erase(c, 1);
			}
		}
		return;
	}
	/**
	 * 指定された削除文字をtarget文字列から削除する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	target		 	対象文字列
	 * @param	deleteStrList	削除文字リスト
	 *
	 * @return	結果文字列
	 */
	string StringUtil::getDeleteStrings(const string target, const vector<string> &deleteStrList)
	{
		string str = target;
		for (string deleteStr : deleteStrList) {
			for (size_t c = str.find_first_of(deleteStr); c != string::npos; c = str.find_first_of(deleteStr)) {
				str.erase(c, 1);
			}
		}
		return str;
	}

	/**
	 * 対象文字列から改行を削除する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/15
	 *
	 * @param [in,out]	targetStr	対象文字列
	 */

	void StringUtil::deleteNl(std::string &targetStr)
	{
		const char CR = '\r';
		const char LF = '\n';
		std::string destStr;
		for (std::string::const_iterator it = targetStr.begin();
			it != targetStr.end(); ++it) {
			if (*it != CR && *it != LF) {
				destStr += *it;
			}
		}
		targetStr = destStr;
	}

	/**
	 * 対象文字列を区切り文字でsplitする
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	target	対象文字列
	 * @param	regex 	区切り文字
	 *
	 * @return	結果文字列
	 */

	vector<string> StringUtil::split(const string &target, const string &regex)
	{
		vector<string> result;

		using string = std::string;
		string::size_type pos = 0;
		if (target.length() == 0) {
			//cout << "[split] target string is length:0. Return resultList.size():0" << endl;
			return result;
		}

		while (pos != string::npos)
		{
			string::size_type p = target.find(regex, pos);

			if (p == string::npos)
			{
				result.push_back(target.substr(pos));
				break;
			}
			else {
				result.push_back(target.substr(pos, p - pos));
			}

			pos = p + regex.size();
		}
		return result;
	}

	/**
	* 対象文字列を区切り文字でsplitする
	*
	* @author	Nagoya University
	* @date	2018/03/14
	*
	* @param	target	対象文字列
	* @param	regex 	区切り文字
	* @param	result 	結果文字列
	* 
	*/

	void StringUtil::split(const string &target, const string &regex, vector<string> &result)
	{
		using string = std::string;
		string::size_type pos = 0;
		result.clear();
		if (target.length() == 0) {
			//cout << "[split] target string is length:0" << endl;
			return;
		}

		while (pos != string::npos)
		{
			string::size_type p = target.find(regex, pos);

			if (p == string::npos)
			{
				result.push_back(target.substr(pos));
				break;
			}
			else {
				result.push_back(target.substr(pos, p - pos));
			}

			pos = p + regex.size();
		}
		return;
	}

	/**
	* ユーザ定義関数や文字列('文字'or"文字")を含む文字列をsplitする.
	* EX. "x,y,UF_X(a,b,c)" => ["x", "y", "UF_X(a,b,c)"]
	* EX. "x,y,'a,b,c'"     => ["x", "y", "a,b,c"]
 	*
	* @author	Nagoya University
	* @date	2019/06/26
	*
	* @param	target	対象文字列
	* @param	regex 	区切り文字
	* @param	result 	結果文字列
	*
	*/

	void StringUtil::splitExcludeBrackets(const string &target, const string &regex, vector<string> &result)
	{
		string parameter = target;

		vector<string> innerListK= getStringInBracketFirst(parameter,"(",")");
		for (unsigned int i = 0; i < innerListK.size(); i++) {
			replaceOnce(parameter, "(" + innerListK.at(i) + ")", "(replaceK" + to_string(i) + ")");
		}
		vector<string> innerListS = getStringInBracketFirst(parameter, "'", "'");
		for (unsigned int i = 0; i < innerListS.size(); i++) {
			replaceOnce(parameter, "'" + innerListS.at(i) + "'", "'replaceS" + to_string(i) + "'");
		}
		vector<string> innerListD = getStringInBracketFirst(parameter, "\"", "\"");
		for (unsigned int i = 0; i < innerListD.size(); i++) {
			replaceOnce(parameter, "\"" + innerListD.at(i) + "\"", "\"replaceD" + to_string(i) + "\"");
		}
		split(parameter, regex, result);

		for (unsigned int i = 0; i < innerListD.size(); i++) {
			for (unsigned int j = 0; j < result.size(); j++) {
				if (contain(result.at(j), "replaceD" + to_string(i))) {
					replaceOnce(result.at(j), "replaceD" + to_string(i), innerListD.at(i));
				}
			}
		}
		for (unsigned int i = 0; i < innerListS.size(); i++) {
			for (unsigned int j = 0; j < result.size(); j++) {
				if (contain(result.at(j), "replaceS" + to_string(i))) {
					replaceOnce(result.at(j), "replaceS" + to_string(i), innerListS.at(i));
				}
			}
		}
		for (unsigned int i = 0; i < innerListK.size(); i++) {
			for (unsigned int j = 0; j < result.size(); j++) {
				if (contain(result.at(j), "replaceK" + to_string(i))) {
					replaceOnce(result.at(j), "replaceK" + to_string(i), innerListK.at(i));
				}
			}
		}
	}

	/**
	* 対象文字列を指定サイズで分割する
	*
	* @author	Nagoya University
	* @date	2018/05/23
	*
	* @param	target	対象文字列
	* @param	size 	指定サイズ
	* @param [in,out]	result	結果文字配列
	*
	*/

	void StringUtil::splitBySize(const string &target, const unsigned int size, vector<string> &result)
	{
		unsigned int sumLen = 0;
		string splitStr = "";

		while (sumLen != target.length()) {
			splitStr = target.substr(sumLen, target.length() - sumLen > size ? size : target.length() - sumLen);
			result.push_back(splitStr);
			sumLen = sumLen + splitStr.length();
			splitStr.clear();
		}
		return;
	}
	/**
	* target文字列のbeforeをafterに1度だけ置換する
	*
	* @author	Nagoya University
	* @date	2018/03/14
	*
	* @param [in,out]	target	対象文字列
	* @param	before	置換対象文字列
	* @param	after 	置換後文字列
	*
	* @return	A string.
	*/

	void StringUtil::replaceOnce(string &target, const string &before, const string &after)
	{
		std::string::size_type  Pos(target.find(before));

		if (Pos == std::string::npos) return;
		target.replace(Pos, before.length(), after);
		return;
	}

	/**
	 * target文字列のbeforeをafterに置換する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param [in,out]	target	対象文字列
	 * @param	before	置換対象文字列
	 * @param	after 	置換後文字列
	 *
	 */

	void StringUtil::replaceAll(string &target, const string &before, const string &after)
	{
		std::string::size_type  Pos(target.find(before));

		while (Pos != std::string::npos)
		{
			target.replace(Pos, before.length(), after);
			Pos = target.find(before, Pos + after.length());
		}

		return;
	}
	/**
	 * target文字列のbeforeをafterに置換する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param [in,out]	target	対象文字列
	 * @param	before	置換対象文字列
	 * @param	after 	置換後文字列
	 *
	 */

	void StringUtil::replace(string &target, const string &before, const string &after)
	{
		replaceAll(target, before, after);
		return;
	}

	/**
	* target文字列を大文字に変換する
	*
	* @author	Nagoya University
	* @date	2019/08/08
	*
	* @param [in,out]	target	対象文字列
	*
	*/

	void StringUtil::toUpper(string &target)
	{
		std::transform(target.begin(), target.end(), target.begin(), [](unsigned char c) { return std::toupper(c); });
	}

	/**
	* target文字列を小文字に変換する
	*
	* @author	Nagoya University
	* @date	2019/08/08
	*
	* @param [in,out]	target	対象文字列
	*
	*/

	void StringUtil::toLower(string &target)
	{
		std::transform(target.begin(), target.end(), target.begin(), [](unsigned char c) { return std::tolower(c); });
	}

	/**
	 * XML文字列の先頭行からサイズを取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	target	XML文字列
	 *
	 * @return	sizeに指定された数値
	 */

	int StringUtil::getXMLSize(const string &target)
	{
		vector<string> rows, data;
		split(target, "\n", rows);
		if (rows.size() >= 2) {
			split(rows.at(1), " ", data);
		} else if (rows.size() == 1) {
			data = rows;
		} else {
			data.push_back(target);
		}
		for (string str : data)
		{
			if (contain(str, "size")) {
				vector<string> number;
				split(str, "\"", number);
				for (string numstr : number)
				{
					if (std::all_of(numstr.cbegin(), numstr.cend(), IsDigit())) {
						int size;
						try {
							size = stoi(numstr);
						}
						catch (const std::invalid_argument&) {
							cerr << "LINE:" << std::to_string(__LINE__) << " stoi エラー" << " target:" << target << endl;;
							return 0;
						}
						return size;
					}
				}
			}
		}
		cerr << "Not found size attribute. So return size is 0." << endl;
		return 0;
	}

	/**
	* XML文字列のヘッダ情報を取得する
	*
	* @author	Nagoya University
	* @date	2018/05/23
	*
	* @param	target	XML文字列
	* @param [in,out]	result	ヘッダ情報
	*/

	void StringUtil::getHeaderStr(const string &target, string &result)
	{
		result = split(target, "\n").at(1);
	}

	/**
	* XML文字列の先頭からヘッダ情報を取得する
	*
	* @author	Nagoya University
	* @date	2018/05/23
	*
	* @param [in,out]	target	XML文字列
	* @param [in,out]	key	クエリ管理番号と生成時間を連結させたデータユニークキー
	* @param [in,out]	flagment	フラグメント値
	* @param [in,out]	flagmentMax	フラグメント最大値
	*/

	void StringUtil::getHeaderInfo(string &target, string &key, int &flagment, int &flagmentMax)
	{
		vector<string> rows, data, number;
		string mngId = "";
		string ct = "";
		bool isFirst = false;
		split(target, "\n", rows);
		string line1;
		if (rows.size() >= 1) {
			line1 = rows.at(0);
		} else {
			line1 = target;
		}
		if (line1.find(">") != std::string::npos) {
			line1 = line1.substr(0, line1.find(">") + 1);
		}
		split(line1, " ", data);
		if (data.size() == 1 && rows.size() >= 2) {
			string line2 = rows.at(1);
			if (line2.find(">") != std::string::npos) {
				line2 = line2.substr(0, line2.find(">") + 1);
			}
			split(line2, " ", data);
			isFirst = true;
		}
		for (string str : data)
		{
			// 管理番号
			if (contain(str, "id")) {
				split(str, "\"", number);
				for (string numstr : number)
				{
					if (std::all_of(numstr.cbegin(), numstr.cend(), IsDigit()) && numstr.length() != 0) {
						mngId = numstr;
					}
				}
				number.clear();
			}
			// 生成時間
			else if (contain(str, "ct")) {
				split(str, "\"", number);
				for (string numstr : number)
				{
					if (std::all_of(numstr.cbegin(), numstr.cend(), IsDigit()) && numstr.length() != 0) {
						ct = numstr;
					}
				}
				number.clear();
			}
			// フラグメント情報(Max)
			else if (contain(str, "flagmentMax")) {
				split(str, "\"", number);
				for (string numstr : number)
				{
					if (std::all_of(numstr.cbegin(), numstr.cend(), IsDigit()) && numstr.length() != 0) {
						try {
							flagmentMax = std::stoi(numstr);
						} catch(const std::invalid_argument& ) {
							cerr << "[getHeaderInfo] invalid_argument Line:" << std::to_string(__LINE__) << " Failed to get flagmentMax target:" << target << endl;
						}
					}
				}
				number.clear();
			}
			// フラグメント情報
			else if (contain(str, "flagment")) {
				split(str, "\"", number);
				for (string numstr : number)
				{
					if (std::all_of(numstr.cbegin(), numstr.cend(), IsDigit()) && numstr.length() != 0) {
						try {
							flagment = std::stoi(numstr);
						} catch(const std::invalid_argument& ) {
							cerr << "[getHeaderInfo] invalid_argument Line:" << std::to_string(__LINE__) << " Failed to get flagment target:" << target;
						}
					}
				}
				number.clear();
			}

		}
		key = mngId + ct;

		// 先頭ではないヘッダ情報を除去する
		if (!isFirst) {
			int delLen = target.find("\n") + 1; // +1は改行
			target = target.substr(delLen, target.length() - delLen);
		}
		return;
		/* 別バージョン（未使用）
		string mngId, ct;
		vector<string> tagStrList;

		//<stream ct="x" flagment="x" flagmentMax="x" key="x" size="x">を抽出
		string::size_type begin = target.find("<stream");
		if (begin == std::string::npos) return;
		string::size_type end = target.find(">", begin);
		if (end == std::string::npos) return;
		string headerStr = target.substr(begin, end - begin);
		//<stream ct="x、flagment="x、flagmentMax="x、key="x、size="x">で分割
		split(headerStr, "\" ", tagStrList);
		for (string tagStr : tagStrList) {
			vector<string> keyValue;
			split(tagStr, "=\"", keyValue);
			if (keyValue.size() < 2) continue;
			// keyValue[0]: flagment、keyValue[1]: x からの抽出
			if (contain(keyValue[0], "id")) {
				mngId = keyValue[1];
			} else if (contain(keyValue[0], "ct")) {
				ct = keyValue[1];
			} else if (contain(keyValue[0], "flagment")) {
				int num;
				try {
					num = std::stoi(keyValue[1]);
					if (contain(keyValue[0], "flagmentMax")) {
						flagmentMax = num;
					} else {
						flagment = num;
					}
				} catch(const std::invalid_argument& ) {
					cerr << "[getHeaderInfo] invalid_argument Line:" << std::to_string(__LINE__) << " Failed to get flagmentMax target:" << target << endl;
				}
			}
		}
		key = mngId + ct;
		cout << "target:" << target << endl;
		cout << "key:" << key << endl;
		cout << "flagmentMax:" << flagmentMax << endl;
		cout << "flagment:" << flagment << endl;
		return;
		*/
	}

	/**
	* 文字列に走査文字が含まれるかを判定する
	*
	* @author	Nagoya University
	* @date	2018/03/14
	*
	* @param	s	対象文字列
	* @param	v	走査文字
	*
	* @return	True	含まれる
	* 			false	含まれない
	*/

	bool StringUtil::contain(const string& s, const string& v) const {
		return s.find(v) != std::string::npos;
	}

	/**
	* 文字列の先頭がsuffixと一致するかを判定する
	*
	* @author	Nagoya University
	* @date	2020/01/29
	*
	* @param	s	対象文字列
	* @param	prefix	走査文字
	*
	* @return	True	含まれる
	* 			false	含まれない
	*/

	bool StringUtil::starts_with(const std::string& s, const std::string& prefix) {
		auto size = prefix.size();
		if (s.size() < size) return false;
		return std::equal(std::begin(prefix), std::end(prefix), std::begin(s));
	}
	/**
	* 文字列の末尾がsuffixと一致するかを判定する
	*
	* @author	Nagoya University
	* @date	2020/01/29
	*
	* @param	s	対象文字列
	* @param	suffix	走査文字
	*
	* @return	True	含まれる
	* 			false	含まれない
	*/

	bool StringUtil::ends_with(const string& s, const string& suffix) {
		if (s.size() < suffix.size()) return false;
		return equal(rbegin(suffix), rend(suffix), rbegin(s));
	}
	/**
	* any型のデータをstring型に変換する
	*
	* @author	Nagoya University
	* @date	2018/03/14
	*
	* @param	val	any型データ
	*
	* @return	any型からstring型に変換した文字列
	*/

	string StringUtil::getAnyString(const any &val)
	{
		string valStr;
		try {
			const std::type_info& typeInfo = val.type();
			if (typeInfo == typeid(int)) {
				valStr = to_string(any_cast<int>(val));
			}
			else if (typeInfo == typeid(char))
			{
				string str(1, any_cast<char>(val));
				valStr = str;
			}
			else if (typeInfo == typeid(const char *))
			{
				const char *pc_str = any_cast<const char *>(val);
				valStr = string(pc_str);
			}
			else if (typeInfo == typeid(long))
			{
				valStr = to_string(any_cast<long>(val));
			}
			else if (typeInfo == typeid(unsigned int)) {
				valStr = to_string(any_cast<unsigned int>(val));
			}
			else if (typeInfo == typeid(unsigned long long)) {
				valStr = to_string(any_cast<unsigned long long>(val));
			}
			else if (typeInfo == typeid(uint64_t)) {
				valStr = to_string(any_cast<uint64_t>(val));
			}
			else if (typeInfo == typeid(double))
			{
				valStr = to_string(any_cast<double>(val));
			}
			else if (typeInfo == typeid(string))
			{
				valStr = any_cast<string>(val);
			}
			else if (typeInfo == typeid(bool))
			{
				valStr = to_string(any_cast<bool>(val));
			}
			else if (typeInfo == typeid(void))
			{
				valStr = "(null)";
			}
			else if (typeInfo == typeid(vector<any>))
			{
				vector<any> list = any_cast<vector<any>>(val);
				valStr.append("[");
				for (unsigned int i = 0; i < list.size(); i++) {
					if (i != 0) valStr.append(",");
					valStr.append(getAnyString(list.at(i)));
				}
				valStr.append("]");
			}
			else if (typeInfo == typeid(vector<string>))
			{
				vector<string> list = any_cast<vector<string>>(val);
				valStr.append("[");
				for (unsigned int i = 0; i < list.size(); i++) {
					if (i != 0) valStr.append(",");
					valStr.append(list.at(i));
				}
				valStr.append("]");
			}
			else if (typeInfo == typeid(vector<int>))
			{
				vector<int> list = any_cast<vector<int>>(val);
				valStr.append("[");
				for (unsigned int i = 0; i < list.size(); i++) {
					if (i != 0) valStr.append(",");
					valStr.append(std::to_string(list.at(i)));
				}
				valStr.append("]");
			}
			else if (typeInfo == typeid(vector<unsigned int>))
			{
				vector<unsigned int> list = any_cast<vector<unsigned int>>(val);
				valStr.append("[");
				for (unsigned int i = 0; i < list.size(); i++) {
					if (i != 0) valStr.append(",");
					valStr.append(std::to_string(list.at(i)));
				}
				valStr.append("]");
			}
			else if (typeInfo == typeid(vector<double>))
			{
				vector<double> list = any_cast<vector<double>>(val);
				valStr.append("[");
				for (unsigned int i = 0; i < list.size(); i++) {
					if (i != 0) valStr.append(",");
					valStr.append(std::to_string(list.at(i)));
				}
				valStr.append("]");
			}
			else if (typeInfo == typeid(vector<long>))
			{
				vector<long> list = any_cast<vector<long>>(val);
				valStr.append("[");
				for (unsigned int i = 0; i < list.size(); i++) {
					if (i != 0) valStr.append(",");
					valStr.append(std::to_string(list.at(i)));
				}
				valStr.append("]");
			}
			else if (typeInfo == typeid(vector<unsigned long long>))
			{
				vector<unsigned long long> list = any_cast<vector<unsigned long long>>(val);
				valStr.append("[");
				for (unsigned int i = 0; i < list.size(); i++) {
					if (i != 0) valStr.append(",");
					valStr.append(std::to_string(list.at(i)));
				}
				valStr.append("]");
			}
			else if (typeInfo == typeid(vector<uint64_t>))
			{
				vector<uint64_t> list = any_cast<vector<uint64_t>>(val);
				valStr.append("[");
				for (unsigned int i = 0; i < list.size(); i++) {
					if (i != 0) valStr.append(",");
					valStr.append(std::to_string(list.at(i)));
				}
				valStr.append("]");
			}
			else if (typeInfo == typeid(vector<char>))
			{
				vector<char> list = any_cast<vector<char>>(val);
				valStr.append("[");
				for (unsigned int i = 0; i < list.size(); i++) {
					if (i != 0) valStr.append(",");
					string str(1, list.at(i));
					valStr.append(str);
				}
				valStr.append("]");
			}
			else if (typeInfo == typeid(vector<bool>))
			{
				vector<bool> list = any_cast<vector<bool>>(val);
				valStr.append("[");
				for (unsigned int i = 0; i < list.size(); i++) {
					if (i != 0) valStr.append(",");
					valStr.append(std::to_string(list.at(i)));
				}
				valStr.append("]");
			}
			else if (typeInfo == typeid(vector<vector<any>>)) {
				vector<vector<any>> list = any_cast<vector<vector<any>>>(val);
				valStr.append("[");
				for (unsigned int i = 0; i < list.size(); i++) {
					if (i != 0) valStr.append(",");
					valStr.append(getAnyString(list.at(i)));
				}
				valStr.append("]");
			}
			else if (typeInfo == typeid(vector<vector<string>>))
			{
				vector<vector<string>> list = any_cast<vector<vector<string>>>(val);
				valStr.append("[");
				for (unsigned int i = 0; i < list.size(); i++) {
					if (i != 0) valStr.append(",");
					valStr.append(getAnyString(list.at(i)));
				}
				valStr.append("]");
			}
			else if (typeInfo == typeid(vector<vector<int>>))
			{
				vector<vector<int>> list = any_cast<vector<vector<int>>>(val);
				valStr.append("[");
				for (unsigned int i = 0; i < list.size(); i++) {
					if (i != 0) valStr.append(",");
					valStr.append(getAnyString(list.at(i)));
				}
				valStr.append("]");
			}
			else if (typeInfo == typeid(vector<vector<unsigned int>>))
			{
				vector<vector<unsigned int>> list = any_cast<vector<vector<unsigned int>>>(val);
				valStr.append("[");
				for (unsigned int i = 0; i < list.size(); i++) {
					if (i != 0) valStr.append(",");
					valStr.append(getAnyString(list.at(i)));
				}
				valStr.append("]");
			}
			else if (typeInfo == typeid(vector<vector<double>>))
			{
				vector<vector<double>> list = any_cast<vector<vector<double>>>(val);
				valStr.append("[");
				for (unsigned int i = 0; i < list.size(); i++) {
					if (i != 0) valStr.append(",");
					valStr.append(getAnyString(list.at(i)));
				}
				valStr.append("]");
			}
			else if (typeInfo == typeid(vector<vector<long>>))
			{
				vector<vector<long>> list = any_cast<vector<vector<long>>>(val);
				valStr.append("[");
				for (unsigned int i = 0; i < list.size(); i++) {
					if (i != 0) valStr.append(",");
					valStr.append(getAnyString(list.at(i)));
				}
				valStr.append("]");
			}
			else if (typeInfo == typeid(vector<vector<unsigned long long>>))
			{
				vector<vector<unsigned long long>> list = any_cast<vector<vector<unsigned long long>>>(val);
				valStr.append("[");
				for (unsigned int i = 0; i < list.size(); i++) {
					if (i != 0) valStr.append(",");
					valStr.append(getAnyString(list.at(i)));
				}
				valStr.append("]");
			}
			else if (typeInfo == typeid(vector<vector<uint64_t>>))
			{
				vector<vector<uint64_t>> list = any_cast<vector<vector<uint64_t>>>(val);
				valStr.append("[");
				for (unsigned int i = 0; i < list.size(); i++) {
					if (i != 0) valStr.append(",");
					valStr.append(getAnyString(list.at(i)));
				}
				valStr.append("]");
			}
			else if (typeInfo == typeid(vector<vector<char>>))
			{
				vector<vector<char>> list = any_cast<vector<vector<char>>>(val);
				valStr.append("[");
				for (unsigned int i = 0; i < list.size(); i++) {
					if (i != 0) valStr.append(",");
					valStr.append(getAnyString(list.at(i)));
				}
				valStr.append("]");
			}
			else if (typeInfo == typeid(vector<vector<bool>>))
			{
				vector<vector<bool>> list = any_cast<vector<vector<bool>>>(val);
				valStr.append("[");
				for (unsigned int i = 0; i < list.size(); i++) {
					if (i != 0) valStr.append(",");
					valStr.append(getAnyString(list.at(i)));
				}
				valStr.append("]");
			}
			else {
#if __cplusplus < 201703L || defined(USE_EXP_ANY)
				bool has_value = !(val.empty());
#else
				bool has_value = val.has_value();
#endif
				if (has_value)
				{
					valStr = "(null)";
				}
				else {
					cerr << "[getAnyString] No setting Type. typeName:" << Demangle(val.type()) << endl;
				}
			}
		}
		catch (const exception &e) {
			string what(e.what());
			cerr << "[getAnyString] ERROR. cause:" << what << endl;
		}
		return valStr;
	}
	/**
	 * string型のデータをvector<string>型に変換する
	 *
	 * @author	Shinichi Kusayama
	 * @date	2023/12/12
	 *
	 * @param	args	string型データ
	 *
	 * @return	string型からvector<string>型に変換した文字列
	 */

	vector<string> StringUtil::getVectorString(string &args)
	{
		vector<string> delStr = { "[","]" };
		vector<string> ret;
		string vals = args;
		deleteStrings(vals, delStr);
		for (string val : split(vals, ",")) {
			ret.push_back(val);
		}
		return ret;
	}


	/**
	 * 対象文字列で()内の文字列を配列で取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/15
	 *
	 * @param	input	対象文字列
	 * @param [in,out]	ret ()内の文字配列
	 * @note	18698文字をregex_searchするとライブラリの不具合？によりコアダンプする
	 */

	void StringUtil::getStringInBracket(string input, vector<string> &ret)
	{
		vector<string> deleteChar = { "(",")" };
		if(input.length() > 15000) {
			// 暫定:18000文字強でregex_searchがコアダンプするため手動でsubstrする
			int firstIn = input.find('(',0);
			int endIn = input.find(')',firstIn);
			string sub = input.substr(firstIn + 1, endIn - firstIn - 1);
			ret.push_back(sub);
			return;
		} 
		string tmp;
		try {
			std::smatch m;
			std::regex e("\\([^()]*\\)");
			
			while (std::regex_search(input, m, e)) {
				for (auto x : m) {
					tmp = x;
					deleteStrings(tmp, deleteChar);
					ret.push_back(tmp);
				}
				input = m.suffix().str();
			}
		}
		catch (const regex_error& err) {
			cerr << err.code() << ':' << err.what() << endl;
		}
		return;
	}
	/**
	* 対象文字列で最も階層が浅い指定文字内の文字列を配列で取得する
	*
	* @author	Nagoya University
	* @date	2019/06/26
	*
	* @param	input	対象文字列
	* @param	left	指定文字(左)
	* @param	right	指定文字(右)
	* @return	string	()内の文字列
	*
	*/
	vector<string>  StringUtil::getStringInBracketFirst(string input, string left, string right)
	{
		vector<string> ret;
		if (input.find(left) == std::string::npos && input.find(right) == std::string::npos) return ret;
		int num = 0;
		string inner = "";
		for (unsigned int i = 0; i<input.size(); i++) {
			string ch{ input[i] };
			
			if (ch == left) {
				if (num > 0 && left == right) {
					num--;
					if (num == 0) {
						ret.push_back(inner);
						inner.clear();
					}
				}
				else {
					num++;
					if (num == 1)continue;
				}
			}
			else if (ch == right) {
				num--;
				if (num == 0) {
					ret.push_back(inner);
					inner.clear();
				}
			}
			else {
			}
			if (num > 0) inner.append(ch);
		}
		return ret;
	}

	/**
	 * String型から型を指定してAnyを取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	str 	文字列
	 * @param	type	データ型
	 *
	 * @return	any型データ
	 */

	any StringUtil::getAnyValFromString(const string &str, const string &type)
	{
		any val;
		getAnyValFromString(str, type, val, false);
		return val;
	}
	/**
	 * String型から型を指定してAnyを取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	str 	文字列
	 * @param	type	データ型
	 * @param	isNull	NULL判定
	 *
	 * @return	any型データ
	 */

	any StringUtil::getAnyValFromString(const string &str, const string &type, const bool &isNull)
	{
		any val;
		getAnyValFromString(str, type, val, isNull);
		return val;
	}


	/**
	* String型から型を指定してAnyを取得する
	*
	* @author	Nagoya University
	* @date	2020/02/03
	*
	* @param	str 	文字列
	* @param	type	データ型
	* @param[in,out]	val	any型データ
	*/

	void StringUtil::getAnyValFromString(const string &str, const string &type, any &val)
	{
		getAnyValFromString(str, type, val, false);
	}
	/**
	* String型から型を指定してAnyを取得する
	*
	* @author	Nagoya University
	* @date	2020/02/03
	*
	* @param	str 	文字列
	* @param	type	データ型
	* @param[in,out]	val	any型データ
	* @param	isNull	NULL判定
	*/

	void StringUtil::getAnyValFromString(const string &str, const string &type, any &val, const bool &isNull)
	{
		try {
			if (str.length() == 0) {
				val = (string)"";
				return;
			}
			if (typeMap.count(type) != 0) {
				switch (typeMap[type])
				{
				case LONG:
					if (isNull) {
						val = (long)NULL;
					} else {
						val = std::stol(str.c_str());
					}
					break;
				// unsigned long long 型
				case ULONG:
					if (isNull) {
						val = (unsigned long long)NULL;
					} else {
						val = std::stoull(str.c_str());
					}
					break;
				case DOUBLE:
					if (isNull) {
						val = (double)NULL;
					} else {
						val = std::stod(str.c_str());
					}
					break;
				case INT:
					if (isNull) {
						val = (int)NULL;
					} else {
						val = std::stoi(str.c_str());
					}
					break;
				case UINT:
					if (isNull) {
						val = (unsigned int)NULL;
					} else {
						unsigned long tmpL = std::stoul(str.c_str());
						if (tmpL > std::numeric_limits<unsigned int>::max()) {
							cerr << "[getAnyValFromString] ERROR. cause: unsigned_int_max over str:" << str << " type:" << type;
						}
						unsigned int tmpI = tmpL;
						val = tmpI;
					}
					break;
				case STRING:
					val = str;
					break;
				case BOOL:
					if (isNull) {
						val = (bool)NULL;
					} else {
						bool ret;
						if (convertBool(str, ret)) {
							val = ret;
						}
					}
					break;
				}
			}
			else if (contain(type, "vector"))
			{
				vector<string> vecType;
				getStringInBracket(type, vecType);
				if (str.substr(0, 2) == "{{" || str.substr(0, 2) == "[[") {
					// 2D-vector
					val = get2VectorAny(str, vecType.at(0), isNull);
				} else {
					// vector
					val = getVectorAny(str, vecType.at(0), isNull);
				}
			}
			else if (type[0] == '_')
			{
				// array from RDB
				string arrayType = type;
				arrayType.erase(arrayType.begin() + 0);
				val = getVectorAny(str, arrayType, isNull);
			}
			else {
				cerr << "[getAnyValFromString] Not defined Type. So Can't convert value. type:" << type << endl;
			}
		}
		catch (const exception &e) {
			string what(e.what());
			cerr << "[getAnyValFromString] ERROR. cause:" << what << " str:" << str << " type:" << type << endl;
		}
	}


	/**
	* データ型のサポート可否をチェックする
	*
	* @author	Nagoya University
	* @date	2019/02/27
	*
	* @param	type	データ型
	*
	* @return	サポート可否
	*/

	bool StringUtil::checkSupportDataType(const string &type)
	{
		string typeS = type;
		toLower(typeS);
		if (typeMap.count(typeS) != 0) {
		
		}
		else if (contain(type, "vector"))
		{
			// vector
			vector<string> vecType;
			getStringInBracket(type, vecType);
			return checkSupportDataType(vecType.at(0));
		}
		else if (type[0] == '_')
		{
			// array ? from RDB
			string arrayType = type;
			arrayType.erase(arrayType.begin() + 0);
			return checkSupportDataType(arrayType);
		}
		else {
			cerr << "[checkSupportDataType] Not defined Type. So Can't convert value. type:" << type << endl;
			return false;
		}

		return true;
	}

	/**
	* データ型が一致するかをチェックする（厳密ではなく暗黙的Castが可能なものを一致とする）
	*
	* @author	Nagoya University
	* @date	2020/02/25
	*
	* @param	type1	データ型1
	* @param	type2	データ型2
	* @param	isOutputErr	ログ出力の有無
	*
	* @return	一致可否
	*/

	bool StringUtil::checkDataTypeMatches(const string &type1, const string &type2, bool isOutputErr)
	{
		string type1S = type1;
		string type2S = type2;
		toLower(type1S);
		toLower(type2S);
		if (typeMap.count(type1S) != 0 && typeMap.count(type2S) != 0) {
			if (typeMap[type1S] == typeMap[type2S]) return true;
		}
		else if (contain(type1, "vector") && contain(type2, "vector"))
		{
			// vector
			vector<string> vecType1;
			getStringInBracket(type1, vecType1);
			vector<string> vecType2;
			getStringInBracket(type2, vecType2);
			return checkDataTypeMatches(vecType1.at(0), vecType2.at(0), isOutputErr);
		}
		else if (type1[0] == '_' && type2[0] == '_')
		{
			// array ? from RDB
			string arrayType1 = type1;
			arrayType1.erase(arrayType1.begin() + 0);
			string arrayType2 = type2;
			arrayType2.erase(arrayType2.begin() + 0);
			return checkDataTypeMatches(arrayType1, arrayType2, isOutputErr);
		}
		else {
			if (isOutputErr) {
				cerr << "[checkDataTypeMatches] Not defined Type or No matches data type. Please confirm data type. type1:" << type1 << " type2:" << type2 << endl;
			}
			return false;
		}

		return false;
	}

	/**
	* String型から型を指定してAnyを取得する
	*
	* @author	Nagoya University
	* @date	2018/05/31
	*
	* @param	str 	文字列
	* @param	type	データ型
	* @param	isNull	NULL判定
	*
	* @return	any型配列データ
	*/
	vector<any> StringUtil::getVectorAny(const string &str, const string &type, const bool &isNull)
	{
		vector<string> delStr = { "[","]","{","}" };
		vector<any> ret;
		string vals = str;
		deleteStrings(vals, delStr);
		for (string val : split(vals, ",")) {
			ret.push_back(getAnyValFromString(val, type, isNull));
		}
		return ret;
	}

	/**
	* String型から型を指定してAnyを取得する
	*
	* @author	Shinichi Kusayama
	* @date	2023/11/24
	*
	* @param	str 	文字列
	* @param	type	データ型
	* @param	isNull	NULL判定
	*
	* @return	any型2次元配列データ
	*/
	vector<vector<any>> StringUtil::get2VectorAny(const string &str, const string &type, const bool &isNull)
	{
		vector<vector<any>> ret;
		string prefix, suffix;
		if (str.find("{") != string::npos && str.find("}") != string::npos) {
			prefix = "{";
			suffix = "}";
		} else if (str.find("[") != string::npos && str.find("]") != string::npos) {
			prefix = "[";
			suffix = "]";
		} else {
			cerr << "[get2VectorAny] Not defined Array Type Str. str:" << str << endl;
			return ret;
		}
		if (str.length() <= 4) return ret;
		// [[1,2,3],[4,5,6]]の先頭[[と末尾]]を削除
		string vector2str = str.substr(2, str.length() - 4);
		for (string vector1str : split(vector2str, suffix + "," + prefix)) {
			vector<any> vector1;
			for (string val : split(vector1str, ",")) {
				vector1.push_back(getAnyValFromString(val, type, isNull));
			}
			ret.push_back(vector1);
		}
		
		return ret;
	}

	/**
	* String型からbool値を取得する
	*
	* @author	Nagoya University
	* @date	2019/02/27
	*
	* @param	str 	文字列
	* @param	ret	変換結果
	*
	* @return	変換成功可否
	*/
	bool StringUtil::convertBool(const string &str, bool &ret)
	{
		if (str == "1" || strcasecmp(str.c_str(), "t") == 0 || strcasecmp(str.c_str(), "true") == 0) {
			ret = true;
			return true;
		}
		else if (str == "0" || strcasecmp(str.c_str(), "f") == 0 || strcasecmp(str.c_str(), "false") == 0) {
			ret = false;
			return true;
		}
		else {
			cerr << "[convertBool] Not defined bool Type Str. So Can't convert str -> bool. str:" << str << endl;
			return false;
		}
	}

	/**
	* EWKT文字列からSRIDを取得する
	*
	* @author	Nagoya University
	* @date	2018/05/31
	*
	* @param	input	対象文字列
	* @return	SRID
	*
	*/

	int StringUtil::getSRID(const string &input)
	{
		if (contain(input, "SRID")) {
			return std::stoi(split(split(input, ";").at(0), "=").at(1));
		}
		return 0;
	}

	/**
	* 数値か判定する
	*
	* @author	Nagoya University
	* @date	2019/08/26
	*
	* @param	input	対象文字列
	* @return	数値:true, それ以外:false
	*
	*/
	bool StringUtil::isNumber(const string &input)
	{
		regex isNumber("^-?\\d+(\\.?\\d+)?$");
		return regex_match(input, isNumber);
	}

	/**
	* 文字列か判定する
	*
	* @author	Nagoya University
	* @date	2019/08/26
	*
	* @param	input	対象文字列
	* @return	文字列:true, それ以外:false
	*
	*/
	bool StringUtil::isString(const string &input)
	{
		if (strcmp(input.substr(0, 1).c_str(), "'") == 0 && strcmp(input.substr(input.length() - 1, 1).c_str(), "'") == 0) return true;
		if (strcmp(input.substr(0, 1).c_str(), "\"") == 0 && strcmp(input.substr(input.length() - 1, 1).c_str(), "\"") == 0) return true;
		return false;
	}
	/**
	* クラス名を取得する
	*
	* @author	Shinichi Kusayama
	* @date	2022/03/04
	*
	* @param	id	クラス型情報
	* @return	クラス名
	*
	*/
	string StringUtil::getClassName(const type_info& id)
	{
		string rtn = "";
		int stat;
		char *name = abi::__cxa_demangle(id.name(),0,0,&stat);
		if (name != NULL) {
			if (stat == 0) {
				rtn = name;
			}
			free(name);
		}
		if (rtn != "") {
			// 「ネームスペース::クラス名」からクラス名だけを取り出す
			if (rtn.find("::") != std::string::npos) {
				vector<string> nameList;
				split(rtn, "::", nameList);
				rtn = nameList[1];
			}
		}
		return rtn;
	}
	/**
	* ヘッダー付き圧縮バッファー生成処理
	*
	* @author	Shinichi Kusayama
	* @date	2022/03/01
	*
	* @param	inStr	圧縮前データ
	* @param	outBuf	圧縮後ヘッダ付きデータ
	* @param	compressFlg	ヘッダの圧縮フラグ
	* @param	key	ヘッダのキー情報
	*
	* @return	正常にデータ処理を実施できた場合はtrue
	*/
	int StringUtil::setCompressedBufWithHeader(string inStr, char *outBuf, char compressFlg, long key)
	{
		int sendSize = SEND_BUF_SIZE;
		Bytef compbuf[COMPRESSED_BUF_SIZE];
		char compbuf2[UNCOMPRESSED_BUF_SIZE];
		int length = COMPRESSED_BUF_SIZE;

		if (compressFlg == '1') {
			if (!compress(inStr.c_str(), compbuf)) {
				return -1;
			}
		} else if (compressFlg == '2') {
			if (!compressUsingZstd(inStr, inStr.length(), compbuf2, &length)) {
				return -1;
			}
		} else {
			return -1;
		}
		//下記は１メッセージあたりの物標数を調査するためのコード
		//if (length > SEND_BUF_SIZE) {
		//	cerr << "length:" << to_string(length) << ",inStr:" << inStr << endl;
		//}
		int seq = 0;
		// 圧縮フラグ<char>、長さ<int>、キー情報<long>、シーケンス番号<int>、ペイロードの順番にセットする
		char *bufTemp_p = outBuf;
		memset(bufTemp_p, 0, SEND_BUF_SIZE);
		memcpy(bufTemp_p, &compressFlg, sizeof(compressFlg));
		bufTemp_p += sizeof(compressFlg);
		memcpy(bufTemp_p, &length, sizeof(length));
		bufTemp_p += sizeof(length);
		memcpy(bufTemp_p, &key, sizeof(key));
		bufTemp_p += sizeof(key);
		memcpy(bufTemp_p, &seq, sizeof(seq));
		bufTemp_p += sizeof(seq);
		if (compressFlg == '2') {
			sendSize = length + sizeof(compressFlg) + sizeof(length) + sizeof(key) + sizeof(seq);
			memcpy(bufTemp_p, compbuf2, length);
		} else {
			memcpy(bufTemp_p, compbuf, length);
		}
		return sendSize;
	}
	/**
	* ヘッダー設定処理
	*
	* @author	Shinichi Kusayama
	* @date	2022/06/29
	*
	* @param	buf	展開前データ
	* @param	headerInfo	ヘッダー情報
	*
	*/
	void StringUtil::getIsHeader(char *buf, IsHeaderInfo &info)
	{
		info.payload_p = buf;
		// 圧縮フラグ<char>、長さ<int>、キー情報<long>、シーケンス番号<int>、ペイロードの順番に展開する
		memcpy(&info.header.compressFlg, info.payload_p, sizeof(info.header.compressFlg));
		if (info.header.compressFlg != '1' && info.header.compressFlg != '2') {
			if (info.header.compressFlg == '0') {
				info.headerSize = sizeof(info.header.compressFlg);
				info.payload_p += info.headerSize;
			} else if (info.header.compressFlg == '3') {
				string bufStr = string(buf, 100);
				int schema_name_len = stoi(bufStr.substr(1,2));
				info.headerSize = schema_name_len + 3;
				info.header.schema_name = bufStr.substr(3, schema_name_len);
				info.payload_p += info.headerSize;
			} else {
				// ヘッダー不明につき、なしと見なす
				info.headerSize = 0;
			}
			return;
		}
		info.payload_p += sizeof(info.header.compressFlg);
		memcpy(&info.header.length, info.payload_p, sizeof(info.header.length));
		info.payload_p += sizeof(info.header.length);
		memcpy(&info.header.key, info.payload_p, sizeof(info.header.key));
		info.payload_p += sizeof(info.header.key);
		memcpy(&info.header.seq, info.payload_p, sizeof(info.header.seq));
		info.payload_p += sizeof(info.header.seq);
		info.headerSize = sizeof(info.header.compressFlg) + sizeof(info.header.length) + sizeof(info.header.key) + sizeof(info.header.seq);
		//cout << "compressFlg:" << info.header.compressFlg << ",length:" << info.header.length << ",key:" << info.header.key << ",seq:" << info.header.seq << endl;
		return;
	}
	/**
	* 圧縮処理
	*
	* @author	Shinichi Kusayama
	* @date	2022/03/01
	*
	* @param	inStr	圧縮前データ
	* @param	outBuf	圧縮後データ
	*
	* @return	正常にデータ処理を実施できた場合はtrue
	*/
	bool StringUtil::compress(string inStr, Bytef *outBuf)
	{
		bool rtn = true;
		z_stream z;
		z.zalloc = Z_NULL;
		z.zfree = Z_NULL;
		z.opaque = Z_NULL;
		if (deflateInit(&z, Z_BEST_COMPRESSION) != Z_OK) {
			cerr << "[compress] deflateInit Error." << endl;
			return false;
		}
		z.avail_in = 0;
		z.next_out = outBuf;
		z.avail_out = COMPRESSED_BUF_SIZE;
		z.next_in = (Bytef *)inStr.c_str();
		z.avail_in = inStr.length();
		int status = deflate(&z, Z_FINISH);
		if (status != Z_STREAM_END) {
			rtn = false;
		}
		if (deflateEnd(&z) != Z_OK) {
			cerr << "[compress] deflateEnd Error." << endl;
			rtn = false;
		}
		return rtn;
	}
	/**
	* 展開処理
	*
	* @author	Shinichi Kusayama
	* @date	2022/05/10
	*
	* @param	inBuf	展開前データ
	* @param	outBuf	展開後データ
	*
	* @return	展開に失敗した場合は-1を、成功した場合はサイズを返す
	*/
	int StringUtil::decompress(char *inBuf, char *outBuf)
	{
		int len = -1;
		z_stream z;
		z.zalloc = Z_NULL;
		z.zfree = Z_NULL;
		z.opaque = Z_NULL;

		z.next_in = Z_NULL;
		z.avail_in = 0;
		if (inflateInit(&z) != Z_OK) {
			cerr << "[decompress] inflateInit Error." << endl;
			return len;
		}
		z.next_out = (Bytef *)outBuf;
		z.avail_out = UNCOMPRESSED_BUF_SIZE;
		z.next_in = (Bytef *)inBuf;
		z.avail_in = COMPRESSED_BUF_SIZE;

		int status = inflate(&z, Z_NO_FLUSH);
		if (status == Z_STREAM_END) {
			string tmpBufStr = string(outBuf);
			len = tmpBufStr.length();
		} else {
			cerr << "[decompress] Z_STREAM_END Error." << endl;
			len = -1;
		}
		if (inflateEnd(&z) != Z_OK) {
			cerr << "[decompress] deflateEnd Error." << endl;
		}
		return len;
	}
	/**
	* 圧縮処理 (Zstandard)
	*
	* @author	Shinichi Kusayama
	* @date	2023/10/12
	*
	* @param	inStr	圧縮前データ
	* @param	inSize	圧縮前データサイズ
	* @param	outBuf	圧縮後データ
	* @param	outSize	圧縮後データサイズ
	*
	* @return	正常にデータ処理を実施できた場合はtrue
	*/
	bool StringUtil::compressUsingZstd(string inStr, int inSize, void *outBuf, int *outSize)
	{
		bool rtn = true;
		void* const fBuff = (void*)inStr.c_str();
		size_t fSize = (size_t)inSize;
		size_t const cBuffSize = ZSTD_compressBound(fSize);
		const int compressionLevel = 3;		// Level: 1 - 22

		size_t const cSize = ZSTD_compress(outBuf, cBuffSize, fBuff, fSize, compressionLevel);
		if (ZSTD_isError(cSize)) {
			cerr << "[compressUsingZstd] Compress Error." << ZSTD_getErrorName(cSize) << endl;
			rtn = false;
		}
		*outSize = cSize;
		return rtn;
	}
	/**
	* 展開処理 (Zstandard)
	*
	* @author	Shinichi Kusayama
	* @date	2022/03/01
	*
	* @param	inStr	展開前データ
	* @param	outBuf	展開後データ
	* @param	inSize	展開前データサイズ
	* @param	outSize	展開後データサイズ
	*
	* @return	正常にデータ処理を実施できた場合はtrue
	*/
	bool StringUtil::decompressUsingZstd(char *inBuf, char *outBuf, int inSize, int *outSize)
	{
		bool rtn = true;
		unsigned long long const rSize = ZSTD_getDecompressedSize(inBuf, inSize);
		if (rSize == 0) {
			cerr << "[compressUsingZstd] Decompress Error. inSize:" << inSize << ",rSize:" << rSize << endl;
			return false;
		}

		size_t const dSize = ZSTD_decompress(outBuf, rSize, inBuf, inSize);

		if (dSize == rSize) {
			*outSize = dSize;
		} else {
			cerr << "[compressUsingZstd] Decompress Error." << ZSTD_getErrorName(dSize) << endl;
			rtn = false;
		}
		return rtn;
	}
	/**
	* XML文字列から指定されたタグの中身を取得する
	*
	* @author	Shinichi Kusayama
	* @date	2023/08/16
	*
	* @param [in]	target	XML文字列
	* @param [in]	key	タグ
	* @return	タグの中身（タグが見つからない場合は空文字）
	*/

	string StringUtil::getValueByXMLTag(string target, string key)
	{
		string ret = "";
		if (target.find("<" + key + ">") == std::string::npos || target.find("</" + key + ">") == std::string::npos) {
			return ret;
		}
		vector<string> words, words2;
		split(target, "<" + key + ">", words);
		if (words.size() < 2) {
			return ret;
		}
		split(words[1], "</" + key + ">", words2);
		if (words.size() < 1) {
			return ret;
		}
		return words2[0];
	}
}
