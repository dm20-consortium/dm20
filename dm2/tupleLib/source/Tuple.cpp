#include "is/Tuple.h"
#include "is/StringUtil.h"

#include <iostream>

using std::cout;                     // cout
using std::cerr;                     // cerr
using std::endl;                     // endl
using std::string;                   // string

namespace IS {

    /**
     * コンストラクタ
     *
     * @author	Nagoya University
     * @date	2018/03/14
     */

    Tuple::Tuple()
    {
        values.clear();
        timestamps.clear();
		isNull.clear();
    }

    /**
     * コンストラクタ：（サイズ指定）
     *
     * @author	Nagoya University
     * @date	2018/03/14
     *
     * @param	size	サイズ
     */

    Tuple::Tuple(int size)
    {
        // create tuple which have empty data
        values.resize(size);
        timestamps.resize(size);
		isNull.resize(size);

        //cout << "tuple created. size=" << values.size() << " capacity=" << values.capacity() << endl;
    }

	/**
	* コンストラクタ：（スキーマ指定）
	*
	* @author	Nagoya University
	* @date	2019/07/25
	*
	* @param	schema	スキーマ
	*/

	Tuple::Tuple(const Schema &schema)
	{
		values.resize(schema.getAttributeSize());
		timestamps.resize(schema.getAttributeSize());
		isNull.resize(schema.getAttributeSize());
	}

    /**
     * コンストラクタ：（タプル、タプル）
     * タプル同士をJOIN操作したタプルを生成する
     *
     * @author	Nagoya University
     * @date	2018/03/14
     *
     * @param	t1	タプル1
     * @param	t2	タプル2
     */

    Tuple::Tuple(Tuple t1, Tuple t2)
    {
        // ★add columns = join?

        // change vector's size & capacity
        int resize = t1.values.size() + t2.values.size();
        values.resize(resize);          // any::clear()の状態か？
        timestamps.resize(resize);
		isNull.resize(resize);
        //cout << "tuple created. size=" << values.size() << " capacity=" << values.capacity() << endl;

        // set t1 values, and set t2 values after t1 values
        int dst_index = 0;
        for(int src_index = 0; src_index < (int)t1.values.size(); src_index++, dst_index++)
        {
            values[dst_index] = t1.values[src_index];
            timestamps[dst_index] = t1.timestamps[src_index];
			isNull[dst_index] = t1.isNull[src_index];
        }
        for(int src_index = 0; src_index < (int)t2.values.size(); src_index++, dst_index++)
        {
            values[dst_index] = t2.values[src_index];
            timestamps[dst_index] = t2.timestamps[src_index];
			isNull[dst_index] = t2.isNull[src_index];
        }
    }

	/**
	* デストラクタ
	*
	* @author	Nagoya University
	* @date	2018/03/14
	*
	*/

	Tuple::~Tuple()
	{
		values.clear();
		timestamps.clear();
		isNull.clear();
	}

    /**
     * サイズを取得する
     *
     * @author	Nagoya University
     * @date	2018/03/14
     *
     * @return	サイズ
     */

    int Tuple::size() const
    {
        return values.size();
    }

    /**
     * index指定でデータをセットする（NULL指定なし）
     *
     * @author	Nagoya University
     * @date	2018/03/14
     *
     * @param	index	 	指定インデックス
     * @param	val		 	値
     * @param	timestamp	タイムスタンプ
     *
     * @return	True 
     */
	bool Tuple::setValue(const int index, const any &val, const long timestamp)
    {
        if(!checkIndex(index)) { return false; }

        values[index] = val;
        timestamps[index] = timestamp;
		isNull[index] = false;

        return true;
    }

	/**
	 * index指定でデータをセットする（NULL指定あり）
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	index	 	指定インデックス
	 * @param	val		 	値
	 * @param	timestamp	タイムスタンプ
	 * @param	isnull   	NULL判定
	 *
	 * @return	True 
	 */
	bool Tuple::setValue(const int index, const any &val, const long timestamp, const bool isnull)
	{
		if (!checkIndex(index)) { return false; }

		values[index] = val;
		timestamps[index] = timestamp;
		isNull[index] = isnull;

		return true;
	}

	/**
	 * 受信時刻をセットする
	 *
	 * @author	Shinichi Kusayama
	 * @date	2023/12/06
	 *
	 * @param	time
	 */
	void Tuple::setRecvTime(const long time)
	{
		recvTime = time;
		return;
	}

	/**
	 * コストをセットする
	 *
	 * @author	Shinichi Kusayama
	 * @date	2025/02/24
	 *
	 * @param	time
	 */
	 void Tuple::setCost(const double _cost)
	 {
		 cost = _cost;
		 return;
	 }
 
	/**
	* index指定でデータを更新する
	*
	* @author	Nagoya University
	* @date	2020/02/19
	*
	* @param	idx  	index
	* @param	val		 	値
	*
	* @return	True/False
	*/

	bool Tuple::updateValueByIdx(const unsigned int idx, const any &val)
	{
		if (values.size() <= idx) return false;
		values[idx] = val;
		return true;
	}

	/**
	* 指定したインデックスのtimestampを更新する(性能評価用)
	*
	* @author	Nagoya University
	* @date	2019/02/18
	*
	* @param	idx		 	インデックス
	* @param	timestamp  	タイムスタンプ
	*
	*/

	void Tuple::updateTimestampByIdx(int idx, long timestamp)
	{
		timestamps[idx] = timestamp;
	}

	/**
	* データが条件を満たす場合に更新する
	*
	* @author	Nagoya University
	* @date	2020/02/19
	*
	* @param	idx  	index
	* @param	val		 	値
	*
	* @return	True/False
	*/
	void Tuple::updateValueIf(int idx, const any &caseVal, const any &updateVal)
	{
		StringUtil stringUtil;
		any val;
		if (!getValueByIdx(idx, val)) return;
		string strVal = stringUtil.getAnyString(val);
		string strCaseVal = stringUtil.getAnyString(caseVal);
		if (strVal == strCaseVal) {
			if (!updateValueByIdx(idx, updateVal)) {
				cerr << "[updateValueIf] Idx: " << idx << ", val:" + stringUtil.getAnyString(updateVal) << endl;
			}
		}
	}
    /**
     * index指定で列データを取得する
     *
     * @author	Nagoya University
     * @date	2018/03/14
     *
     * @param 		  	index	 	指定インデックス
     * @param [in,out]	val		 	値
     * @param [in,out]	timestamp	タイムスタンプ
     *
     * @return	True 
     */
	bool Tuple::getValue(const int index, any& val, long& timestamp) const
    {
        if(!checkIndex(index)) { return false; }

        val = values[index];
        timestamp = timestamps[index];

        return true;
    }

	/**
	 * index指定で列データを取得する（NULL判定あり）
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param 		  	index	 	インデックス指定
	 * @param [in,out]	val		 	値
	 * @param [in,out]	timestamp	タイムスタンプ
	 * @param [in,out]	isnull   	NULL判定
	 *
	 * @return	True 
	 */
	bool Tuple::getValue(const int index, any& val, long& timestamp, bool& isnull)
	{
		if (!checkIndex(index)) { return false; }

		val = values[index];
		timestamp = timestamps[index];
		isnull = isNull[index];

		return true;
	}

	/**
	* INDEX指定で値を取得する
	*
	* @author	Nagoya University
	* @date	2020/02/17
	*
	* @param [in]		idx		インデックス
	* @param [in,out]	val	   	取得値
	*
	* @return	True	取得成功
	* 			false	取得失敗（列名不正）
	*/

	bool Tuple::getValueByIdx(const unsigned int idx, any& val) const
	{
		if (!checkIndex(idx)) { return false; }
		val = values[idx];
		return true;
	}

	/**
	* INDEX指定で値を取得する
	*
	* @author	Shinichi Kusayama
	* @date	2025/02/23
	*
	* @param [in]		idx		インデックス
	*
	* @return	取得値
	*/
	int Tuple::getIntValueByIdx(const unsigned int idx) const
	{
		int val = 0;
		if (!checkIndex(idx)) { return val; }
		const std::type_info& typeInfo = values[idx].type();
		if (typeInfo == typeid(int)) {
			val = any_cast<int>(values[idx]);
		}
		return val;
	}
	/**
	* INDEX指定で値を取得する
	*
	* @author	Shinichi Kusayama
	* @date	2025/02/23
	*
	* @param [in]		idx		インデックス
	*
	* @return	取得値
	*/
	string Tuple::getStringValueByIdx(const unsigned int idx) const
	{
		string val = "";
		if (!checkIndex(idx)) { return val; }
		const std::type_info& typeInfo = values[idx].type();
		if (typeInfo == typeid(string)) {
			val = any_cast<string>(values[idx]);
		}
		else if (typeInfo == typeid(int)) {
			val = to_string(any_cast<int>(values[idx]));
		}
		else if (typeInfo == typeid(long)) {
			val = to_string(any_cast<long>(values[idx]));
		}
		else if (typeInfo == typeid(unsigned long long)) {
			val = to_string(any_cast<unsigned long long>(values[idx]));
		}
		else if (typeInfo == typeid(uint64_t)) {
			val = to_string(any_cast<uint64_t>(values[idx]));
		}
		return val;
	}

	/**
	* タイムスタンプを取得する
	*
	* @author	Shinichi Kusayama
	* @date	2025/08/13
	*
	* @return	タイムスタンプ
	*/

	long Tuple::getTimestamp() const
	{
		return timestamps[0];
	}

	/**
	* index指定でタイムスタンプを取得する
	*
	* @author	Nagoya University
	* @date	2019/08/02
	*
	* @param [in]	idx			index
	* @param [in,out]	time	   	取得値
	*
	* @return	True	取得成功
	* 			false	取得失敗（列名不正）
	*/

	bool Tuple::getTimestampByIdx(const unsigned int idx, long& time) const
	{
		if (!checkIndex(idx)) { return false; }
		time = timestamps[idx];
		return true;
	}

	/**
	* タプルの値をクリアする
	*
	* @author	Nagoya University
	* @date	2020/01/29
	*
	*/
	void Tuple::clearValue()
	{
		int size = values.size();
		values.clear();
		timestamps.clear();
		isNull.clear();
		recvTime = 0;

		values.resize(size);
		timestamps.resize(size);
		isNull.resize(size);
	}

	/**
	 * 受信時刻を返却する
	 *
	 * @author	Shinichi Kusayama
	 * @date	2023/12/06
	 *
	 * @param	time
	 */
	long Tuple::getRecvTime()
	{
		return recvTime;
	}

	/**
	 * コストを返却する
	 *
	 * @author	Shinichi Kusayama
	 * @date	2025/02/24
	 *
	 * @param	time
	 */
	 double Tuple::getCost() const
	 {
		 return cost;
	 }

	/**
	* タプルのサイズをリサイズする
	*
	* @author	Nagoya University
	* @date	2020/02/20
	*
	*/
	void Tuple::resize(int size)
	{
		values.resize(size);
		timestamps.resize(size);
		isNull.resize(size);
	}

	/**
	* タプルのサイズをスキーマ情報を元にリサイズする
	*
	* @author	Shinichi Kusayama
	* @date	2024/05/07
	*
	*/
	void Tuple::resize(const Schema &schema)
	{
		values.resize(schema.getAttributeSize());
		timestamps.resize(schema.getAttributeSize());
		isNull.resize(schema.getAttributeSize());
	}

	/**
	* タプルの要素を入れ替える
	*
	* @author	Nagoya University
	* @date	2020/02/26
	*
	*/
	void Tuple::swap(const unsigned int fromIdx, const unsigned int toIdx)
	{
		iter_swap(values.begin() + fromIdx, values.end() + toIdx);
		iter_swap(timestamps.begin() + fromIdx, timestamps.end() + toIdx);
		iter_swap(isNull.begin() + fromIdx, isNull.end() + toIdx);
	}

	/**
	 * 引数で渡された時間よりも新しいTupleである場合にtrueを返却する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	time		判定時間
	 * @param	schema		スキーマ情報
	 *
	 * @return	True	渡された時間よりも新しいTupleの場合
	 * 			false	渡された時間よりも古いTupleの場合
	 */

	bool Tuple::checkTimestamp(const long time, const Schema &schema)
	{
		for (unsigned int i = 0; i < timestamps.size(); i++)
		{
			//cout << "[Tuple]timestamps[i]" << timestamps[i] << ",i:" << i << ",time:" << time << endl;
			// 指定時間より古く、管理者カラムではない場合はfalse
			if (timestamps[i] <= time && schema.getAttribute(i).hidden == false) return false;
		}
		return true;
	}

	/**
	 * Tupleの中で最も最新の時間を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @return	最新の時間
	 */

	long Tuple::getNewestTimestamp() const
	{
		return *std::max_element(timestamps.begin(), timestamps.end());
	}

	/**
	* timestampの時間差ログを取得する(性能評価用)
	*
	* @author	Nagoya University
	* @date	2019/02/18
	*
	* @return	string	カラム毎に設定されたタイムスタンプ差を文字列で返却
	*/

	string Tuple::getLogTimestamp() const
	{
		string ret = "";
		long firstTime = 0;
		for (unsigned int i=0; i<timestamps.size(); i++)
		{
			if (timestamps[i] == 0) continue;
			if (firstTime == 0) {
				firstTime = timestamps[i]; 
			}
			else {
				if (timestamps[i] != firstTime && timestamps[i-1] != timestamps[i]) {
					ret.append(std::to_string(i - 1) + "-" + std::to_string(i) + ":" + std::to_string(timestamps[i] - timestamps[i - 1]) + "msec ");
				}
			}

		}
		return ret;
	}

	/**
	* カラムを削除する
	*
	* @author	Nagoya University
	* @date	2019/09/09
	*
	* @param	idx	インデックス
	* @return	bool
	*/

	bool Tuple::deleteValueByIdx(const int idx)
	{
		values.erase(values.begin() + idx);
		timestamps.erase(timestamps.begin() + idx);
		isNull.erase(isNull.begin() + idx);
		return true;
	}

    /**
     * 指定列番号のデータを抽出したタプルを生成する（未使用）
     *
     * @author	Nagoya University
     * @date	2018/03/14
     *
     * @param	selectColumns	指定列番号リスト
     *
     * @return	抽出後のタプル
     */

    Tuple Tuple::getSubset(const std::vector<int> &selectColumns)
    {
		// ★コピーコンストラクタだと遅い？ポインタのほうが良い？
        int size = selectColumns.size();
        Tuple subset(size);

        for(int index = 0; index < size; index++)
        {
            int colNo = selectColumns[index];
            subset.values[index] = values[colNo];
            subset.timestamps[index] = timestamps[colNo];
			subset.isNull[index] = isNull[colNo];
        }

        return subset;
    }

    /**
     * 指定indexの存在判定を実施する
     *
     * @author	Nagoya University
     * @date	2018/03/14
     *
     * @param	index	指定index
     *
     * @return	True	指定indexがタプルに存在する場合
     * 			false	指定indexがタプルに存在しない場合
     */

    inline bool Tuple::checkIndex(int index) const
    {
        if((index < 0) || (index >= (int)values.size())) 
        {
			cerr << "index is out of bound." << " index=" << index << " size=" << values.size() << endl;
            return false; 
        }
        return true;
    }

    /**
     * タプル情報全体を出力する
     *
     * @author	Nagoya University
     * @date	2018/03/14
     */

    void Tuple::dump()
    {
		StringUtil stringUtil;
        // format is "index:value(type_strings):sourcename:timestamp"
        for(int index = 0; index < (int)values.size(); index++)
        {
            cout << "index[" << index << "],value["; 
            const std::type_info& typeInfo = values[index].type();
			cout << stringUtil.getAnyString(values[index]);
            cout << "],type[" <<  Demangle(values[index].type()) << "],ts[" << timestamps[index] << "]" << endl;
        }
    }

	/**
	 * any型データを出力する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	val	any型データ
	 */

	void Tuple::dumpAny(any val)
	{
		StringUtil stringUtil;
		cout << "value[" << stringUtil.getAnyString(val);
		cout << "],type["  << Demangle(val.type()) << "]" << endl;
	}


	/**
	 * テーブル名をセットする（DMI用）
	 *
	 * @author	Shinichi Kusayama
	 * @date	2024/05/09
	 *
	 * @param	tableName	テーブル名
	 */

	void Tuple::setTableName(const string &_tableName)
	{
		tableName = _tableName;
	}

	/**
	 * テーブル名を取得する（DMI用）
	 *
	 * @author	Shinichi Kusayama
	 * @date	2024/05/09
	 *
	 * @return	tableName	テーブル名
	 */

	string Tuple::getTableName()
	{
		return tableName;
	}
}
