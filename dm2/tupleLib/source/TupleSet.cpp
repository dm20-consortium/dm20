#include "is/TupleSet.h"
#include "is/ArrivalTupleListener.h"

#include <algorithm>

namespace IS {

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	TupleSet::TupleSet()
	{
		tuples.clear();
	}

	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	TupleSet::~TupleSet()
	{
		if (tuples.size() > 0) {
			clearTuples();
		}
		if (schema.getAttributeSize() > 0) {
			schema.clear();
		}
	};

	/**
	 * スキーマ情報の取得
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @return	The schema.
	 */

	Schema TupleSet::getSchema() const
	{
		return schema;
	};

	/**
	* スキーマ情報の取得
	*
	* @author	Nagoya University
	* @date	2018/03/14
	*
	* @return	The schema.
	*/

	Schema& TupleSet::getSchemaRef()
	{
		return schema;
	};

	/**
	 * スキーマ情報の設定
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	in	The in.
	 */

	void TupleSet::SetSchema(Schema in)
	{
		schema = in;
	}

	/**
	 * インデックス指定でタプルを取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	idx	指定インデックス
	 *
	 * @return	タプル
	 */

	Tuple& TupleSet::getTuple(unsigned int idx)
	{
		return tuples.at(idx);
	}

	/**
	 * タプルをセットする
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	tuple	追加タプル
	 */

	void TupleSet::addTuple(const Tuple &tuple)
	{
		tuples.push_back(tuple);
	}

	/**
	* タプルをセットする
	*
	* @author	Nagoya University
	* @date	2020/02/03
	*
	* @param	idx	インデックス
	* @param	tuple	追加タプル
	*/

	void TupleSet::setTuple(const int idx, const Tuple &tuple)
	{
		tuples[idx] = tuple;
	}

	/**
	* タプルを削除する
	* ※処理順番を意識する必要がある場合は注意
	*
	* @author	Nagoya University
	* @date	2020/02/21
	*
	* @param	idx	インデックス
	*/

	void TupleSet::deleteTuple(unsigned int idx)
	{
		//tuples.erase(tuples.begin() + idx); // インデックスの再配置があるので遅い
		tuples[idx] = tuples.back();
		tuples.pop_back();
	}

	/**
	* タプルとスキーマをクリアする
	*
	* @author	Nagoya University
	* @date	2020/01/28
	*
	*/

	void TupleSet::clear()
	{
		if (tuples.size() > 0) {
			clearTuples();
		}
		if (schema.getAttributeSize() > 0) {
			schema.clear();
		}
	}

	/**
	* タプルをクリアする
	*
	* @author	Nagoya University
	* @date	2020/01/28
	*
	*/

	void TupleSet::clearTuples()
	{
		tuples.clear();
		tuples.shrink_to_fit();
	}

	/**
	 * 設定されているタプル数を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @return	An int.
	 */

	int TupleSet::size() const
	{
		return tuples.size();
	}

	/**
	* 指定した要素数でリサイズする
	*
	* @author	Nagoya University
	* @date	2020/02/03
	* @param	size	リサイズする要素数
	*/

	void TupleSet::resize(const int size)
	{
		tuples.resize(size);
	}

	/**
	 * 全タプルを取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @return	タプルリスト
	 */

	std::vector<Tuple> TupleSet::getTuples() const
	{
		return tuples;
	}

	/**
	* Tuple配列への参照を取得する
	*
	* @author	Nagoya University
	* @date	2020/02/25
	*
	* @return	タプル配列ポインタ
	*/

	std::vector<Tuple>& TupleSet::getTupleVec()
	{
		return tuples;
	}

	/**
	* 順序を逆転させる
	*
	* @author	Nagoya University
	* @date	2018/10/26
	*
	*/

	void TupleSet::reverse()
	{
		std::reverse(tuples.begin(), tuples.end());
	}

	/**
	* 指定の行のカラムのタイムスタンプを更新する(性能評価用)
	*
	* @author	Nagoya University
	* @date	2019/2/18
	*
	* @param	rowIdx	行指定
	* @param	colIdx	列指定
	* @param	timestamp	時間
	*/
	void TupleSet::updateTimestampByIdx(int rowIdx, int colIdx, long timestamp)
	{
		tuples.at(rowIdx).updateTimestampByIdx(colIdx, timestamp);
	}

	/**
	* スキーマのカラム名を更新する
	*
	* @author	Nagoya University
	* @date	2019/3/6
	*
	* @param	from	変更前カラム名
	* @param	to	変更後カラム名
	*/
	void TupleSet::updateSchemaColumnName(const string &from, const string &to)
	{
		schema.renameColumnName(from, to);
	}

	/**
	* 指定カラムを削除する
	*
	* @author       Shinichi Kusayama
	* @date 2022/07/25
	*
	* @param        colIdx カラムIdx
	*/
	void TupleSet::removeColumn(const int colIdx)
	{
		// schemaの削除
		schema.deleteAttribute(colIdx);

		// Tupleの削除
		for (unsigned int i = 0; i < tuples.size(); i++) {
			tuples.at(i).deleteValueByIdx(colIdx);
		}
	}
	/**
	* 指定カラムを削除する
	*
	* @author       Shinichi Kusayama
	* @date 2022/07/25
	*
	*/
	void TupleSet::removeColumnOverOrgSize()
	{
		int attrSize = schema.getAttributeSize();
		if (attrSize <= info.orgAttributeSize) {
			return;
		}
		int lastOrgIdx = info.orgAttributeSize - 1;
		for (int colIdx = attrSize - 1; colIdx > lastOrgIdx; colIdx--) {
			// schemaの削除
			schema.deleteAttribute(colIdx);

			// Tupleの削除
			for (unsigned int i = 0; i < tuples.size(); i++) {
				tuples.at(i).deleteValueByIdx(colIdx);
			}
		}
	}
	/**
	* 指定カラムを削除する
	*
	* @author	Nagoya University
	* @date	2019/3/6
	*
	* @param	colName	カラム名
	*/
	void TupleSet::removeColumn(const string &colName)
	{
		int idx = schema.getAttributeIdx(colName);

		// schemaの削除
		schema.deleteAttribute(idx);

		// Tupleの削除
		for (unsigned int i = 0; i < tuples.size(); i++) {
			tuples.at(i).deleteValueByIdx(idx);
		}
	}
	/**
	* コスト値を使ってソートする
	*
	* @author	Shinichi Kusayama
	* @date	2025/02/24
	*
	*/
	void TupleSet::sortByCost()
	{
		std::sort(tuples.begin(), tuples.end(), [](const Tuple &a, const Tuple &b) {
			return a.getCost() < b.getCost() ;
		});
	}
	/**
	* 第1カラムのみ出力する
	*
	* @author	Shinichi Kusayama
	* @date	2025/02/24
	*
	*/
	void TupleSet::debugKey()
	{
		for (unsigned int i = 0; i < tuples.size(); i++) {
			std::cout << tuples.at(i).getStringValueByIdx(0) << "," << tuples.at(i).getCost() << std::endl;
		}
	}
}
