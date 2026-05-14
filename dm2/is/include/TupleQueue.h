#ifndef TUPLEQUEUE_H
#define TUPLEQUEUE_H

// TUPLEQUEUE_Hクラス
// TupleをQueuingする

#include <queue>
#include <condition_variable>
#include <mutex>

#include "is/Tuple.h"

using namespace std;

namespace IS {

	/**
	* ストリームデータをバッファリングするQueueクラス
	*
	* @author	Nagoya University
	* @date	2018/04/26
	*/

	class TupleQueue {
	private:
		unsigned int capacity;
		std::queue<vector<Tuple>> q_;
		std::mutex mtx_;
		std::condition_variable cv_;
	public:
		TupleQueue();
		TupleQueue(int max_cs_queue_size_);
		void Push(const vector<Tuple> &data);
		void Pop(vector<Tuple> &data);
	};
}
#endif  // TUPLEQUEUE_H