#ifndef QUEUE_H
#define QUEUE_H

#include <thread>
#include <unordered_map>  //Unordered UnorderedMap用
#include <queue>          //キューバッファ用
#include <condition_variable> //条件排他用

#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>

#include <fstream>
#include <sstream>

#include "Cs.h"


namespace CS{
	/**
	 * @class	Queue
	 *
	 * @brief	DM2.0CSQueue管理クラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */
	template <typename T1>
	class Queue {
	private:
		/** @brief	キューサイズの設定 */
		unsigned int capacity;
		/** @brief	キューオブジェクト */
		std::queue<T1> q_;
		/** @brief	queue入出力保護用mutex */
		std::mutex mtx_;
		/** @brief	Condition Variableによる排他 */
		// std::condition_variable cv_;
		std::condition_variable not_empty_;
		std::condition_variable not_full_;
		

	public:

		Queue(int max_cs_queue_size_);

		void Push(T1 data);

		int Size();

		bool Empty();

		T1 Pop();
		
		T1 Front();
	};
}
#endif