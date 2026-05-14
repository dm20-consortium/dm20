#include "BufferQueue.h"

#include <iostream>
#include <thread>

using namespace std;

namespace IS {

	/**
	* サイズを指定する
	*
	* @author	Nagoya University
	* @date	2018/04/26
	*
	* @param	size	Queueサイズ
	*/
	BufferQueue::BufferQueue(int size)
	{
		capacity = size;
	}

	/**
	* データをキューイングする
	*
	* @author	Nagoya University
	* @date	2018/04/26
	*
	* @param	data	データ
	*/
	void BufferQueue::Push(const RecvData &data) //エンキュー処理
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
	* @date	2018/04/26
	*
	* @return   データ
	*/
	void BufferQueue::Pop(RecvData &data) //デキュー処理
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