#ifndef BUFFERQUEUE_H
#define BUFFERQUEUE_H

// BufferQueueクラス
// StreamをQueuingする

#include <queue>
#include <condition_variable>
#include <mutex>

#include "NetworkSource.h"

using namespace std;

namespace IS {

	/**
	* ストリームデータをバッファリングするQueueクラス
	*
	* @author	Nagoya University
	* @date	2018/04/26
	*/

	class BufferQueue {
	private:
		unsigned int capacity;
		std::queue<RecvData> q_;
		std::mutex mtx_;
		std::condition_variable cv_;
	public:
		BufferQueue(int max_cs_queue_size_);
		void Push(const RecvData &data);
		void Pop(RecvData &data);
	};
}
#endif  // BUFFERQUEUE_H