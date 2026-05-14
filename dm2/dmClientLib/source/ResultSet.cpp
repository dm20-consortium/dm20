#include "is/ResultSet.h"

/**
 * コンストラクタ
 *
 * @author	Nagoya University
 * @date	2018/03/15
 */

ResultSet::ResultSet()
{
}

/**
 * コンストラクタ
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	datas	  	値のMap情報
 * @param	timestamps	タイムスタンプのMap情報
 * @param	isNulls   	NULL判定値のMap情報
 * @param	meta	  	メタデータ
 */

ResultSet::ResultSet(const vector<std::unordered_map<string, string>> &datas, const vector<std::unordered_map<string, string>> &timestamps, const vector<std::unordered_map<string, string>> &isNulls, const ResultSetMetaData &meta)
{
	_index = -1;
	_results = datas;
	_timestamps = timestamps;
	_isNulls = isNulls;
	_metaData = meta;

}

/**
 * デストラクタ
 *
 * @author	Nagoya University
 * @date	2018/03/15
 */

ResultSet::~ResultSet()
{
}

/**
 * 次の行へ移動する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @return	True	存在する場合
 * 			false	存在しない場合
 */

bool ResultSet::next()
{
	_index++;
	if (_results.size() <= _index) return false;
	return true;
}

/**
 * メタデータを取得する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @return	メタデータ
 */

ResultSetMetaData ResultSet::getResultSetMetaData()
{
	return _metaData;
}

/**
 * 列名指定でstring型データを取得する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	key	列名
 *
 * @return	string型データ
 */

string ResultSet::getString(const string &key)
{
	return _results.at(_index)[key];
}

/**
 * 列インデックス指定でstring型データを取得する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	columnIndex	列インデックス
 *
 * @return	string型データ
 */

string ResultSet::getString(const int columnIndex)
{
	return _results.at(_index)[_metaData.getColumnName(columnIndex)];
}

/**
 * 列名指定でint型データを取得する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @exception	CastException	キャスト変換失敗時
 *
 * @param	key	列名
 *
 * @return	int型データ
 */

int ResultSet::getInt(const string &key)
{
	if (_results.at(_index)[key].length() == 0) {
		return 0;
	}
	try {
		return std::stoi(_results.at(_index)[key].c_str());
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
}

/**
 * 列インデックス指定でint型データを取得する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @exception	CastException	キャスト変換失敗時
 *
 * @param	columnIndex	列インデックス
 *
 * @return	int型データ
 */

int ResultSet::getInt(const int columnIndex)
{
	if (_results.at(_index)[_metaData.getColumnName(columnIndex)].length() == 0) {
		return 0;
	}
	try {
		return std::stoi(_results.at(_index)[_metaData.getColumnName(columnIndex)].c_str());
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
}

/**
 * 列名指定でunsigned int型データを取得する
 *
 * @author	Shinichi Kusayama
 * @date	2024/04/09
 *
 * @exception	CastException	キャスト変換失敗時
 *
 * @param	key	列名
 *
 * @return	unsigned int型データ
 */

unsigned int ResultSet::getUnsignedInt(const string &key)
{
	if (_results.at(_index)[key].length() == 0) {
		return 0;
	}
	try {
		unsigned long tmpL = std::stoul(_results.at(_index)[key].c_str());
		if (tmpL > std::numeric_limits<unsigned int>::max()) {
			throw CastException();
		}
		return (unsigned int)tmpL;
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
}

/**
 * 列インデックス指定でunsigned int型データを取得する
 *
 * @author	Shinichi Kusayama
 * @date	2024/04/09
 *
 * @exception	CastException	キャスト変換失敗時
 *
 * @param	columnIndex	列インデックス
 *
 * @return	unsigned int型データ
 */

unsigned int ResultSet::getUnsignedInt(const int columnIndex)
{
	if (_results.at(_index)[_metaData.getColumnName(columnIndex)].length() == 0) {
		return 0;
	}
	try {
		unsigned long tmpL = std::stoul(_results.at(_index)[_metaData.getColumnName(columnIndex)].c_str());
		if (tmpL > std::numeric_limits<unsigned int>::max()) {
			throw CastException();
		}
		return (unsigned int)tmpL;
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
}

/**
 * 列名指定でdouble型データを取得する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @exception	CastException	キャスト変換失敗時
 *
 * @param	key	列名
 *
 * @return	double型データ
 */

double ResultSet::getDouble(const string &key)
{
	if (_results.at(_index)[key].length() == 0) {
		return 0;
	}
	try {
		return std::stod(_results.at(_index)[key]);
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
}

/**
 * 列インデックス指定でdouble型データを取得する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @exception	CastException	キャスト変換失敗時
 *
 * @param	columnIndex	列インデックス
 *
 * @return	double型データ
 */

double ResultSet::getDouble(const int columnIndex)
{
	if (_results.at(_index)[_metaData.getColumnName(columnIndex)].length() == 0) {
		return 0;
	}
	try {
		return std::stod(_results.at(_index)[_metaData.getColumnName(columnIndex)]);
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
}

/**
 * 列名指定でlong型データを取得する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @exception	CastException	キャスト変換失敗時
 *
 * @param	key	列名
 *
 * @return	long型データ
 */

long ResultSet::getLong(const string &key)
{
	if (_results.at(_index)[key].length() == 0) {
		return 0;
	}
	try {
		return std::stol(_results.at(_index)[key]);
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
}

/**
 * 列インデックス指定でlong型データを取得する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @exception	CastException	キャスト変換失敗時
 *
 * @param	columnIndex	列インデックス
 *
 * @return	long型データ
 */

long ResultSet::getLong(const int columnIndex)
{
	if (_results.at(_index)[_metaData.getColumnName(columnIndex)].length() == 0) {
		return 0;
	}
	try {
		return std::stol(_results.at(_index)[_metaData.getColumnName(columnIndex)]);
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
}

/**
 * 列名指定でunsigned long long型データを取得する
 *
 * @author	Shinichi Kusayama
 * @date	2024/04/09
 *
 * @exception	CastException	キャスト変換失敗時
 *
 * @param	key	列名
 *
 * @return	unsigned long long型データ
 */

unsigned long long ResultSet::getUnsignedLongLong(const string &key)
{
	if (_results.at(_index)[key].length() == 0) {
		return 0;
	}
	try {
		return std::stoull(_results.at(_index)[key]);
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
}

/**
 * 列インデックス指定でunsigned long long型データを取得する
 *
 * @author	Shinichi Kusayama
 * @date	2024/04/09
 *
 * @exception	CastException	キャスト変換失敗時
 *
 * @param	columnIndex	列インデックス
 *
 * @return	unsigned long long型データ
 */

unsigned long long ResultSet::getUnsignedLongLong(const int columnIndex)
{
	if (_results.at(_index)[_metaData.getColumnName(columnIndex)].length() == 0) {
		return 0;
	}
	try {
		return std::stoull(_results.at(_index)[_metaData.getColumnName(columnIndex)]);
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
}
/**
* 列名指定でbool型データを取得する
*
* @author	Nagoya University
* @date	2019/02/27
*
* @exception	CastException	キャスト変換失敗時
*
* @param	key	列名
*
* @return	bool型データ
*/

bool ResultSet::getBool(const string &key)
{
	if (_results.at(_index)[key].length() == 0) {
		return 0;
	}
	try {
		return _results.at(_index)[key] == "1" ? true : false;
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
}

/**
* 列インデックス指定でbool型データを取得する
*
* @author	Nagoya University
* @date	2019/02/27
*
* @exception	CastException	キャスト変換失敗時
*
* @param	columnIndex	列インデックス
*
* @return	bool型データ
*/

bool ResultSet::getBool(const int columnIndex)
{
	if (_results.at(_index)[_metaData.getColumnName(columnIndex)].length() == 0) {
		return 0;
	}
	try {
		return _results.at(_index)[_metaData.getColumnName(columnIndex)] == "1" ? true : false;
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
}

/**
* 列名指定でstring型配列データを取得する
*
* @author	Nagoya University
* @date	2018/05/31
*
* @param	key	列名
*
* @return	string型配列データ
*/
vector<string> ResultSet::getVectorString(const string &key)
{
	string vals = _results.at(_index)[key];
	vector<string> delStr = { "[","]" };
	vector<string> ret;
	vals = stringUtil.getDeleteStrings(vals, delStr);
	for (string val : stringUtil.split(vals, ",")) {
		ret.push_back(val);
	}
	return ret;
}

/**
* 列インデックス指定でstring型配列データを取得する
*
* @author	Nagoya University
* @date	2018/05/31
*
* @param	columnIndex	列インデックス
*
* @return	string型配列データ
*/
vector<string> ResultSet::getVectorString(const int columnIndex)
{
	return getVectorString(_metaData.getColumnName(columnIndex));
}

/**
* 列名指定でint型配列データを取得する
*
* @author	Nagoya University
* @date	2018/05/31
*
* @param	key	列名
*
* @return	int型配列データ
*/
vector<int> ResultSet::getVectorInt(const string &key)
{
	string vals = _results.at(_index)[key];
	vector<string> delStr = { "[","]" };
	vector<int> ret;
	vals = stringUtil.getDeleteStrings(vals, delStr);
	try {
		for (string val : stringUtil.split(vals, ",")) {
			ret.push_back(std::stoi(val));
		}
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
	return ret;
}

/**
* 列インデックス指定でint型配列データを取得する
*
* @author	Nagoya University
* @date	2018/05/31
*
* @param	columnIndex	列インデックス
*
* @return	int型配列データ
*/
vector<int> ResultSet::getVectorInt(const int columnIndex)
{
	return getVectorInt(_metaData.getColumnName(columnIndex));
}

/**
* 列名指定でunsigned int型配列データを取得する
*
* @author	Shinichi Kusayama
* @date	2024/04/12
*
* @param	key	列名
*
* @return	unsigned int型配列データ
*/
vector<unsigned int> ResultSet::getVectorUnsignedInt(const string &key)
{
	string vals = _results.at(_index)[key];
	vector<string> delStr = { "[","]" };
	vector<unsigned int> ret;
	vals = stringUtil.getDeleteStrings(vals, delStr);
	try {
		for (string val : stringUtil.split(vals, ",")) {
			ret.push_back(getUnsignedInt(val));
		}
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
	return ret;
}

/**
* 列インデックス指定でunsigned int型配列データを取得する
*
* @author	Shinichi Kusayama
* @date	2024/04/12
*
* @param	columnIndex	列インデックス
*
* @return	unsigned int型配列データ
*/
vector<unsigned int> ResultSet::getVectorUnsignedInt(const int columnIndex)
{
	return getVectorUnsignedInt(_metaData.getColumnName(columnIndex));
}

/**
* 列名指定でdouble型配列データを取得する
*
* @author	Nagoya University
* @date	2018/05/31
*
* @param	key	列名
*
* @return	double型配列データ
*/
vector<double> ResultSet::getVectorDouble(const string &key)
{
	string vals = _results.at(_index)[key];
	vector<string> delStr = { "[","]" };
	vector<double> ret;
	vals = stringUtil.getDeleteStrings(vals, delStr);
	try {
		for (string val : stringUtil.split(vals, ",")) {
			ret.push_back(std::stod(val));
		}
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
	return ret;
}

/**
* 列インデックス指定でdouble型配列データを取得する
*
* @author	Nagoya University
* @date	2018/05/31
*
* @param	columnIndex	列インデックス
*
* @return	double型配列データ
*/
vector<double> ResultSet::getVectorDouble(const int columnIndex)
{
	return getVectorDouble(_metaData.getColumnName(columnIndex));
}

/**
* 列名指定でlong型配列データを取得する
*
* @author	Nagoya University
* @date	2018/05/31
*
* @param	key	列名
*
* @return	long型配列データ
*/
vector<long> ResultSet::getVectorLong(const string &key)
{
	string vals = _results.at(_index)[key];
	vector<string> delStr = { "[","]" };
	vector<long> ret;
	vals = stringUtil.getDeleteStrings(vals, delStr);
	try {
		for (string val : stringUtil.split(vals, ",")) {
			ret.push_back(std::stol(val));
		}
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
	return ret;
}

/**
* 列インデックス指定でlong型配列データを取得する
*
* @author	Nagoya University
* @date	2018/05/31
*
* @param	columnIndex	列インデックス
*
* @return	long型配列データ
*/
vector<long> ResultSet::getVectorLong(const int columnIndex)
{
	return getVectorLong(_metaData.getColumnName(columnIndex));
}

/**
* 列名指定でunsigned long long型配列データを取得する
*
* @author	Shinichi Kusayama
* @date	2024/04/12
*
* @param	key	列名
*
* @return	unsigned long long型配列データ
*/
vector<unsigned long long> ResultSet::getVectorUnsignedLongLong(const string &key)
{
	string vals = _results.at(_index)[key];
	vector<string> delStr = { "[","]" };
	vector<unsigned long long> ret;
	vals = stringUtil.getDeleteStrings(vals, delStr);
	try {
		for (string val : stringUtil.split(vals, ",")) {
			ret.push_back(std::stoull(val));
		}
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
	return ret;
}

/**
* 列インデックス指定でunsigned long long型配列データを取得する
*
* @author	Shinichi Kusayama
* @date	2024/04/12
*
* @param	columnIndex	列インデックス
*
* @return	unsigned long long型配列データ
*/
vector<unsigned long long> ResultSet::getVectorUnsignedLongLong(const int columnIndex)
{
	return getVectorUnsignedLongLong(_metaData.getColumnName(columnIndex));
}

/**
* 列名指定でbool型配列データを取得する
*
* @author	Nagoya University
* @date	2019/02/27
*
* @param	key	列名
*
* @return	bool型配列データ
*/
vector<bool> ResultSet::getVectorBool(const string &key)
{
	string vals = _results.at(_index)[key];
	vector<string> delStr = { "[","]" };
	vector<bool> ret;
	vals = stringUtil.getDeleteStrings(vals, delStr);
	try {
		for (string val : stringUtil.split(vals, ",")) {
			ret.push_back(val == "1" ? true : false);
		}
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
	return ret;
}

/**
* 列インデックス指定でbool型配列データを取得する
*
* @author	Nagoya University
* @date	2019/02/27
*
* @param	columnIndex	列インデックス
*
* @return	bool型配列データ
*/
vector<bool> ResultSet::getVectorBool(const int columnIndex)
{
	return getVectorBool(_metaData.getColumnName(columnIndex));
}

/**
* 列名指定でstring型2次元配列データを取得する
*
* @author	Shinichi Kusayama
* @date	2023/11/26
*
* @param	key	列名
*
* @return	string型2次元配列データ
*/
vector<vector<string>> ResultSet::get2VectorString(const string &key)
{
	vector<vector<string>> ret;
	string vals = _results.at(_index)[key];
	if (vals.length() <= 4) return ret;
	if (vals.substr(0,2) != "[[" || vals.substr(vals.length() - 2, 2) != "]]") return ret;
	// [[1,2,3],[4,5,6]]の先頭[[と末尾]]を削除
	string vector2str = vals.substr(2, vals.length() - 4);
	for (string vector1str : stringUtil.split(vector2str, "],[")) {
		vector<string> vector1;
		for (string val : stringUtil.split(vector1str, ",")) {
			vector1.push_back(val);
		}
		ret.push_back(vector1);
	}
	return ret;
}

/**
* 列インデックス指定でstring型2次元配列データを取得する
*
* @author	Shinichi Kusayama
* @date	2023/11/26
*
* @param	key	列名
*
* @return	string型2次元配列データ
*/
vector<vector<string>> ResultSet::get2VectorString(const int columnIndex)
{
	return get2VectorString(_metaData.getColumnName(columnIndex));
}

/**
* 列名指定でint型2次元配列データを取得する
*
* @author	Shinichi Kusayama
* @date	2023/11/26
*
* @param	key	列名
*
* @return	int型2次元配列データ
*/
vector<vector<int>> ResultSet::get2VectorInt(const string &key)
{
	vector<vector<int>> ret;
	string vals = _results.at(_index)[key];
	if (vals.length() <= 4) return ret;
	if (vals.substr(0,2) != "[[" || vals.substr(vals.length() - 2, 2) != "]]") return ret;
	// [[1,2,3],[4,5,6]]の先頭[[と末尾]]を削除
	string vector2str = vals.substr(2, vals.length() - 4);
	try {
		for (string vector1str : stringUtil.split(vector2str, "],[")) {
			vector<int> vector1;
			for (string val : stringUtil.split(vector1str, ",")) {
				vector1.push_back(std::stoi(val));
			}
			ret.push_back(vector1);
		}
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
	return ret;
}
/**
* 列インデックス指定でint型2次元配列データを取得する
*
* @author	Shinichi Kusayama
* @date	2023/11/26
*
* @param	key	列名
*
* @return	int型2次元配列データ
*/
vector<vector<int>> ResultSet::get2VectorInt(const int columnIndex)
{
	return get2VectorInt(_metaData.getColumnName(columnIndex));
}

/**
* 列名指定でunsigned int型2次元配列データを取得する
*
* @author	Shinichi Kusayama
* @date	2024/04/12
*
* @param	key	列名
*
* @return	unsigned int型2次元配列データ
*/
vector<vector<unsigned int>> ResultSet::get2VectorUnsignedInt(const string &key)
{
	vector<vector<unsigned int>> ret;
	string vals = _results.at(_index)[key];
	if (vals.length() <= 4) return ret;
	if (vals.substr(0,2) != "[[" || vals.substr(vals.length() - 2, 2) != "]]") return ret;
	// [[1,2,3],[4,5,6]]の先頭[[と末尾]]を削除
	string vector2str = vals.substr(2, vals.length() - 4);
	try {
		for (string vector1str : stringUtil.split(vector2str, "],[")) {
			vector<unsigned int> vector1;
			for (string val : stringUtil.split(vector1str, ",")) {
				vector1.push_back(getUnsignedInt(val));
			}
			ret.push_back(vector1);
		}
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
	return ret;
}
/**
* 列インデックス指定でunsigned int型2次元配列データを取得する
*
* @author	Shinichi Kusayama
* @date	2024/04/12
*
* @param	key	列名
*
* @return	unsigned int型2次元配列データ
*/
vector<vector<unsigned int>> ResultSet::get2VectorUnsignedInt(const int columnIndex)
{
	return get2VectorUnsignedInt(_metaData.getColumnName(columnIndex));
}


/**
* 列名指定でdouble型2次元配列データを取得する
*
* @author	Shinichi Kusayama
* @date	2023/11/26
*
* @param	key	列名
*
* @return	double型2次元配列データ
*/
vector<vector<double>> ResultSet::get2VectorDouble(const string &key)
{
	vector<vector<double>> ret;
	string vals = _results.at(_index)[key];
	if (vals.length() <= 4) return ret;
	if (vals.substr(0,2) != "[[" || vals.substr(vals.length() - 2, 2) != "]]") return ret;
	// [[1,2,3],[4,5,6]]の先頭[[と末尾]]を削除
	string vector2str = vals.substr(2, vals.length() - 4);
	try {
		for (string vector1str : stringUtil.split(vector2str, "],[")) {
			vector<double> vector1;
			for (string val : stringUtil.split(vector1str, ",")) {
				vector1.push_back(std::stod(val));
			}
			ret.push_back(vector1);
		}
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
	return ret;
}

/**
* 列インデックス指定でdouble型2次元配列データを取得する
*
* @author	Shinichi Kusayama
* @date	2023/11/26
*
* @param	key	列名
*
* @return	double型2次元配列データ
*/
vector<vector<double>> ResultSet::get2VectorDouble(const int columnIndex)
{
	return get2VectorDouble(_metaData.getColumnName(columnIndex));
}
/**
* 列名指定でlong型2次元配列データを取得する
*
* @author	Shinichi Kusayama
* @date	2023/11/26
*
* @param	key	列名
*
* @return	long型2次元配列データ
*/
vector<vector<long>> ResultSet::get2VectorLong(const string &key)
{
	vector<vector<long>> ret;
	string vals = _results.at(_index)[key];
	if (vals.length() <= 4) return ret;
	if (vals.substr(0,2) != "[[" || vals.substr(vals.length() - 2, 2) != "]]") return ret;
	// [[1,2,3],[4,5,6]]の先頭[[と末尾]]を削除
	string vector2str = vals.substr(2, vals.length() - 4);
	try {
		for (string vector1str : stringUtil.split(vector2str, "],[")) {
			vector<long> vector1;
			for (string val : stringUtil.split(vector1str, ",")) {
				vector1.push_back(std::stol(val));
			}
			ret.push_back(vector1);
		}
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
	return ret;
}

/**
* 列インデックス指定でlong型2次元配列データを取得する
*
* @author	Shinichi Kusayama
* @date	2023/11/26
*
* @param	key	列名
*
* @return	long型2次元配列データ
*/
vector<vector<long>> ResultSet::get2VectorLong(const int columnIndex)
{
	return get2VectorLong(_metaData.getColumnName(columnIndex));
}
/**
* 列名指定でunsigned long long型2次元配列データを取得する
*
* @author	Shinichi Kusayama
* @date	2024/04/12
*
* @param	key	列名
*
* @return	unsigned long long型2次元配列データ
*/
vector<vector<unsigned long long>> ResultSet::get2VectorUnsignedLongLong(const string &key)
{
	vector<vector<unsigned long long>> ret;
	string vals = _results.at(_index)[key];
	if (vals.length() <= 4) return ret;
	if (vals.substr(0,2) != "[[" || vals.substr(vals.length() - 2, 2) != "]]") return ret;
	// [[1,2,3],[4,5,6]]の先頭[[と末尾]]を削除
	string vector2str = vals.substr(2, vals.length() - 4);
	try {
		for (string vector1str : stringUtil.split(vector2str, "],[")) {
			vector<unsigned long long> vector1;
			for (string val : stringUtil.split(vector1str, ",")) {
				vector1.push_back(std::stoull(val));
			}
			ret.push_back(vector1);
		}
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
	return ret;
}
/**
* 列インデックス指定でunsigned long long型2次元配列データを取得する
*
* @author	Shinichi Kusayama
* @date	2024/04/12
*
* @param	key	列名
*
* @return	unsigned long long型2次元配列データ
*/
vector<vector<unsigned long long>> ResultSet::get2VectorUnsignedLongLong(const int columnIndex)
{
	return get2VectorUnsignedLongLong(_metaData.getColumnName(columnIndex));
}
/**
* 列名指定でbool型2次元配列データを取得する
*
* @author	Shinichi Kusayama
* @date	2023/11/26
*
* @param	key	列名
*
* @return	bool型2次元配列データ
*/
vector<vector<bool>> ResultSet::get2VectorBool(const string &key)
{
	vector<vector<bool>> ret;
	string vals = _results.at(_index)[key];
	if (vals.length() <= 4) return ret;
	if (vals.substr(0,2) != "[[" || vals.substr(vals.length() - 2, 2) != "]]") return ret;
	// [[1,2,3],[4,5,6]]の先頭[[と末尾]]を削除
	string vector2str = vals.substr(2, vals.length() - 4);
	try {
		for (string vector1str : stringUtil.split(vector2str, "],[")) {
			vector<bool> vector1;
			for (string val : stringUtil.split(vector1str, ",")) {
				vector1.push_back(val == "1" ? true : false);
			}
			ret.push_back(vector1);
		}
	}
	catch (std::invalid_argument) {
		throw CastException();
	}
	return ret;
}

/**
* 列インデックス指定でbool型2次元配列データを取得する
*
* @author	Shinichi Kusayama
* @date	2023/11/26
*
* @param	key	列名
*
* @return	bool型2次元配列データ
*/
vector<vector<bool>> ResultSet::get2VectorBool(const int columnIndex)
{
	return get2VectorBool(_metaData.getColumnName(columnIndex));
}
/**
 * 列名指定でタイムスタンプデータを取得する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	key	列名
 *
 * @return	エポックミリ秒
 */

long ResultSet::getEpochTime(const string &key)
{
	return std::stol(_timestamps.at(_index)[key]);
}

/**
 * 列インデックス指定でタイムスタンプデータを取得する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	columnIndex	列インデックス
 *
 * @return	エポックミリ秒
 */

long ResultSet::getEpochTime(const int columnIndex)
{
	return std::stol(_timestamps.at(_index)[_metaData.getColumnName(columnIndex)]);
}

/**
 * 列名指定でNULL判定値を取得する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	key	列名
 *
 * @return	True	NULLの場合
 * 			false	NULLでない場合
 */

bool ResultSet::isNull(const string &key)
{
	if (_isNulls.at(_index)[key] == "1") {
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * 列インデックス指定でNULL判定値を取得する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	columnIndex	列インデックス
 *
 * @return	True	NULLの場合
 * 			false	NULLでない場合
 */

bool ResultSet::isNull(const int columnIndex)
{
	if (_isNulls.at(_index)[_metaData.getColumnName(columnIndex)] == "1") {
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * ResultSetをクリアする
 *
 * @author	Nagoya University
 * @date	2018/03/15
 */

void ResultSet::close()
{
	_results.clear();
	_timestamps.clear();
	_isNulls.clear();
}

/**
 * リファレンスを取得する。
 *
 * @author	Shinichi Kusayama
 * @date	2025/8/26
 *
 * @return	リファレンス
 */
string ResultSet::getReference()
{
	return _reference;
}
/**
 * リファレンスをセットする。
 *
 * @author	Shinichi Kusayama
 * @date	2025/8/26
 *
 * @param	reference	リファレンス
 *
 */
void ResultSet::setReference(const string &reference)
{
	_reference = reference;
}
/**
 * 列名指定でGEOSGeometry型データを取得する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	key	列名
 *
 * @return	GEOSGeometry型データ
 */

GEOSGeometry* ResultSet::getGEOSGeometry(const string &key)
{
	if (_results.at(_index)[key].length() == 0) {
		return NULL;
	}
	return createGEOSGeometry(_results.at(_index)[key]);
}

/**
 * 列インデックス指定でGEOSGeometry型データを取得する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	columnIndex	列インデックス
 *
 * @return	GEOSGeometry型データ
 */

GEOSGeometry* ResultSet::getGEOSGeometry(const int columnIndex)
{
	if (_results.at(_index)[_metaData.getColumnName(columnIndex)].length() == 0) {
		return NULL;
	}
	return createGEOSGeometry(_results.at(_index)[_metaData.getColumnName(columnIndex)]);
}

/**
 * EWKT文字列からGEOSGeometryオブジェクトを生成する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @exception	CastException	データが存在しない場合
 *
 * @param	input	EWKT文字列
 *
 * @return	GEOSGeometry型データ
 */

GEOSGeometry* ResultSet::createGEOSGeometry(const string &input)
{
	return GEOSGeomFromWKT(input.c_str());
}
