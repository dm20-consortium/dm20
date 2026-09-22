#include "Queue.h"

namespace CS{
	/******************************************
	* DM2.0通信用Queueクラス
	* 
	*******************************************/

	/**
	* @fn	Queue::Queue(int max_cs_queue_size_);
	*
	* @brief	コンストラクタ
	* 			キューサイズを定義
	*
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @param	max_cs_queue_size_	キューの最大サイズ
	*/
	template Queue<send_message_vector>::Queue(int max_cs_queue_size_);
	template Queue<csmng_message>::Queue(int max_cs_queue_size_);
	template <typename T1>
	Queue<T1>::Queue(int max_cs_queue_size_)
	{
		capacity = max_cs_queue_size_;
	}

	/**
	* @fn	void Queue::Push;
	*
	* @brief	キューイング処理
	*
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @param	data	キューへの入力データ
	*/
	template void Queue<send_message_vector>::Push(send_message_vector data);
	template void Queue<csmng_message>::Push(csmng_message data);
	template <typename T1>
	void Queue<T1>::Push(T1 data) //エンキュー処理
	{
		std::unique_lock<std::mutex> lk(mtx_);
		not_full_.wait(lk, [this]{
			return q_.size() < capacity;
		});
		q_.push(std::move(data));
		not_empty_.notify_all();

	}


	/**
	* @fn	int Queue<T1>::Size()
	*
	* @brief	キューサイズ取得
	*
	* @author	Nagoya University
	* @date	2018/03/12
	*
	*/
	template int Queue<send_message_vector>::Size();
	template int Queue<csmng_message>::Size();
	template <typename T1>
	int Queue<T1>::Size()
	{
		std::unique_lock<std::mutex> lk(mtx_);
		return q_.size();
	}


	/**
	* @fn	bool Queue<T1>::Empty()
	*
	* @brief	emptyの確認
	*
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @return 要素数が0であればTrue, 0でなければFalseを返す
	*/
	template bool Queue<send_message_vector>::Empty();
	template bool Queue<csmng_message>::Empty();
	template <typename T1>
	bool Queue<T1>::Empty()
	{
		std::unique_lock<std::mutex> lk(mtx_);
		return q_.empty();
	}

	/**
	* @fn	Queue::Pop();
	*
	* @brief	デキュー処理
	*
	* @author	Nagoya University
	* @date	2018/03/12
	*
	* @return	キューデータを出力して、キューから削除する。
	*/
	template send_message_vector Queue<send_message_vector>::Pop();
	template csmng_message Queue<csmng_message>::Pop();
	template <typename T1>
	T1 Queue<T1>::Pop() //デキュー処理
	{
		std::unique_lock<std::mutex> lk(mtx_);
		not_empty_.wait(lk, [this]{
			return !q_.empty();
		});
		T1 ret = std::move(q_.front());
		q_.pop();
		if(q_.empty()){
			std::queue<T1>().swap(q_);
		}
		not_full_.notify_all();
		return ret;
	}
	/**
	* @fn	T1 Queue<T1>::Front()
	*
	* @brief	キューの先頭要素を取得
	*
	* @date	2025/02/28
	*
	* @return	キューの先頭要素
	*/
	template <typename T1>
	T1 Queue<T1>::Front()
	{
		std::unique_lock<std::mutex> lk(mtx_);
		not_empty_.wait(lk, [this]{
			return !q_.empty();
		});
		return q_.front();
	}
}