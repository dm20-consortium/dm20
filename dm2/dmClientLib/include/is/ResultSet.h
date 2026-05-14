#ifndef RESULTSET_H
#define RESULTSET_H

#include "is/ResultSetMetaData.h"
#include "is/StringUtil.h"
#include "is/DmException.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <geos_c.h>

using namespace std;

/**
 * クエリ結果を扱うクラス
 *
 * @author	Nagoya University
 * @date	2018/03/15
 */

class ResultSet
{
private:
	int _index;
	IS::StringUtil stringUtil;
	std::vector<std::unordered_map<string, string>> _results;
	std::vector<std::unordered_map<string, string>> _timestamps;
	std::vector<std::unordered_map<string, string>> _isNulls;
	ResultSetMetaData _metaData;
	string _reference = "";

public:
	ResultSet();
	ResultSet(const vector<std::unordered_map<string, string>> &datas, const vector<std::unordered_map<string, string>> &timestamps, const vector<std::unordered_map<string, string>> &isNulls, const ResultSetMetaData &meta);
	~ResultSet();

	bool next();
	ResultSetMetaData getResultSetMetaData();
	string getString(const string &key);
	string getString(const int columnIndex);
	int getInt(const string &key);
	int getInt(const int columnIndex);
	unsigned int getUnsignedInt(const string &key);
	unsigned int getUnsignedInt(const int columnIndex);
	double getDouble(const string &key);
	double getDouble(const int columnIndex);
	long getLong(const string &key);
	long getLong(const int columnIndex);
	unsigned long long getUnsignedLongLong(const string &key);
	unsigned long long getUnsignedLongLong(const int columnIndex);
	bool getBool(const string &key);
	bool getBool(const int columnIndex);
	vector<string> getVectorString(const string &key);
	vector<string> getVectorString(const int columnIndex);
	vector<int> getVectorInt(const string &key);
	vector<int> getVectorInt(const int columnIndex);
	vector<unsigned int> getVectorUnsignedInt(const string &key);
	vector<unsigned int> getVectorUnsignedInt(const int columnIndex);
	vector<double> getVectorDouble(const string &key);
	vector<double> getVectorDouble(const int columnIndex);
	vector<long> getVectorLong(const string &key);
	vector<long> getVectorLong(const int columnIndex);
	vector<unsigned long long> getVectorUnsignedLongLong(const string &key);
	vector<unsigned long long> getVectorUnsignedLongLong(const int columnIndex);
	vector<bool> getVectorBool(const string &key);
	vector<bool> getVectorBool(const int columnIndex);
	vector<vector<string>> get2VectorString(const string &key);
	vector<vector<string>> get2VectorString(const int columnIndex);
	vector<vector<int>> get2VectorInt(const string &key);
	vector<vector<int>> get2VectorInt(const int columnIndex);
	vector<vector<unsigned int>> get2VectorUnsignedInt(const string &key);
	vector<vector<unsigned int>> get2VectorUnsignedInt(const int columnIndex);
	vector<vector<double>> get2VectorDouble(const string &key);
	vector<vector<double>> get2VectorDouble(const int columnIndex);
	vector<vector<long>> get2VectorLong(const string &key);
	vector<vector<long>> get2VectorLong(const int columnIndex);
	vector<vector<unsigned long long>> get2VectorUnsignedLongLong(const string &key);
	vector<vector<unsigned long long>> get2VectorUnsignedLongLong(const int columnIndex);
	vector<vector<bool>> get2VectorBool(const string &key);
	vector<vector<bool>> get2VectorBool(const int columnIndex);
	long getEpochTime(const string &key);
	long getEpochTime(const int columnIndex);
	bool isNull(const string &key);
	bool isNull(const int columnIndex);
	void close();
	string getReference();
	void setReference(const string &reference);

	GEOSGeometry* getGEOSGeometry(const string &key);
	GEOSGeometry* getGEOSGeometry(const int columnIndex);
	GEOSGeometry* createGEOSGeometry(const string &key);
};
#endif  // RESULTSET_H
