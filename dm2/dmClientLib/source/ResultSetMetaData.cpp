#include "is/ResultSetMetaData.h"

/**
 * コンストラクタ
 *
 * @author	Nagoya University
 * @date	2018/03/15
 */

ResultSetMetaData::ResultSetMetaData()
{
}

/**
 * コンストラクタ（クエリ管理番号、列名リスト、データ型リスト）
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	mngId   	クエリ管理番号
 * @param	nameList	列名リスト
 * @param	typeList	データ型リスト
 */

ResultSetMetaData::ResultSetMetaData(const unsigned int mngId, const vector<string> &nameList, const vector<string> &typeList)
{
	this->mngId = mngId;
	this->nameList = nameList;
	this->typeList = typeList;
}

/**
 * デストラクタ
 *
 * @author	Nagoya University
 * @date	2018/03/15
 */

ResultSetMetaData::~ResultSetMetaData()
{
}

/**
 * インデックス指定で列名を取得する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	col	インデックス
 *
 * @return	列名
 * 			指定したインデックスが存在しない場合は"ERROR"を返却
 */

string ResultSetMetaData::getColumnName(const int col)
{
	if (col >= nameList.size()) {
		return "ERROR";
	}
	return nameList.at(col);
}

/**
 * インデックス指定でデータ型文字列を取得する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @param	col	インデックス
 *
 * @return	データ型文字列
 * 			指定したインデックスが存在しない場合は"ERROR"を返却
 */

string ResultSetMetaData::getColumnType(const int col)
{
	if (col >= typeList.size()) {
		return "ERROR";
	}
	return typeList.at(col);
}

/**
 * 列数を取得する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @return	列数
 */

int ResultSetMetaData::getColumnSize()
{
	return nameList.size();
}

/**
 * クエリ管理番号を取得する
 *
 * @author	Nagoya University
 * @date	2018/03/15
 *
 * @return	クエリ管理番号
 */

unsigned int ResultSetMetaData::getManagementId()
{
	return mngId;
}
