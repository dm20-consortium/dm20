#include "TupleQueue.h"

using namespace std;

namespace IS {

	/**
	* コンストラクタ(無指定の場合は10000)
	*
	* @author	Nagoya University
	* @date	2018/10/25
	*
	*/
	TupleQueue::TupleQueue()
	{
		capacity = 100000;
	}

	/**
	* サイズを指定する
	*
	* @author	Nagoya University
	* @date	2018/10/25
	*
	* @param	size	Queueサイズ
	*/
	TupleQueue::TupleQueue(int size)
	{
		capacity = size;
	}

	/**
	* データをキューイングする
	*
	* @author	Nagoya University
	* @date	2018/10/25
	*
	* @param	data	データ
	*/
	void TupleQueue::Push(const vector<Tuple> &data) //エンキュー処理
	{
		std::unique_lock<std::mutex> lk(mtx_); //
		while (q_.size() == capacity) {
			cv_.wait(lk); //要素数の変更があるまでwait
		}
		q_.push(data); //dataをキューに格納
		cv_.notify_one(); //待機中のスレッドを１つ起床
	}

	/**
	* キューイングしたデータを取得する
	*
	* @author	Nagoya University
	* @date	2018/10/25
	*
	* @return   データ
	*/
	void TupleQueue::Pop(vector<Tuple> &data) //デキュー処理
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		while (q_.empty()) {
			cv_.wait(lk); //要素数の変更があるまでwait
		}
		data = q_.front(); //出力キューの表示
		q_.pop(); //出力したキューの削除
		cv_.notify_one(); //待機中のスレッドを１つ起床
		//return data; //出力データをリターン
	}
}
