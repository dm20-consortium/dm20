#ifndef RESULTSET_METADATA_H
#define RESULTSET_METADATA_H

#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

/**
 * クエリ結果のメタデータを扱うクラス
 *
 * @author	Nagoya University
 * @date	2018/03/15
 */

class ResultSetMetaData
{
private:
	unsigned int mngId;
	vector<string> nameList;
	vector<string> typeList;

public:
	ResultSetMetaData();
	ResultSetMetaData(const unsigned int mngId, const vector<string> &nameList, const vector<string> &typeList);
	~ResultSetMetaData();

	string getColumnName(const int col);
	string getColumnType(const int col);
	int getColumnSize();
	unsigned int getManagementId();
};
#endif  // RESULTSET_METADATA_H
