#ifndef UNORDEREDMAP_H
#define UNORDEREDMAP_H

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
	template <typename T1, typename T2>
	/**
	 * @class	UnorderedMap
	 *
	 * @brief	DM2.0通信用通信部用unordered_mapクラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */
	class UnorderedMap {
	private:
		std::unordered_map<T1, T2> u_map_; //非順序MAP作成
		// std::unordered_map<T1, std::vector<T2>> u_map_vector_; //非順序MAP作成
		std::mutex mtx_; //mutex
		std::condition_variable cv_; //条件排他
	public:
		int UnorderedMapInsert(T1 key_, T2 value_);

		void UnorderedMapInsertVector(T1 key_, int vector_max_, int vector_num_, T1 vector_value_);
		void UnorderedMapInsertVectorPlusSize(T1 key_, int vector_max_, int vector_num_, T1 vector_value_);
		
		void UnorderedMapPushbackVector(T1 key_, T1 vector_value_);
		
		void UnorderedMapPushbackVectorUnique(T1 key_, T1 vector_value_);
		
		void UnorderedMapUpdateVectorValue(T1 key_, int vector_num_, T1 vector_value_);
		void UnorderedMapUpdateVectorValueThenDecrease(T1 key_, int vector_num_, T1 vector_value_);

		bool UnorderedMapKeyExist(T1 key_);

		void UnorderedMapKeyExistThenUpdateElseInsert(T1 key_, T2 value_);

		bool UnorderedMapKeyExistVector(T1 key_);

		T2 UnorderedMapGetValue(T1 key_);
		
		T1 UnorderedMapGetValueVector(T1 Key_, int vector_num_);
		
		T2 UnorderedMapGetVectorValueAndClear(T1 key_);

		int UnorderedMapGetValueSub(int second_);
		std::vector<int> UnorderedMapGetValueSub(std::vector<int> second_);
		std::vector<unsigned long long> UnorderedMapGetValueSub(std::vector<unsigned long long> second_);
		std::string UnorderedMapGetValueSub(std::string second_);

		void UnorderedMapErase(T1 key_);

		int UnorderedMapEraseRes(T1 key_);

		int UnorderedMapEraseOldTime(time_t aging_time_);
		
		void UnorderedMapEraseOldTimeNs(T1 now_time_);

		T1 UnorderedMapVectorCombineAndDelete(T1 key_);
		T1 UnorderedMapVectorCombineAndDeletePlusSize(T1 key_, int total_);

		void UnorderedMapShow();
		void UnorderedMapShowStruct();
		void UnorderedMapShowVector();
		std::vector<T1> UnorderedMapKeyExtraction();
		std::vector<T2> UnorderedMapValueExtraction();


		T1 UnorderedMapOldKey(time_t aging_time_);

		T1 UnorderedMapOldKeyString(time_t aging_time_);

		int UnorderedMapSize();

		void UnorderedMapUpdate(T1 key_, T2 value_);

		void UnorderedMapMake(const char* filename);

		int UnorderedMapMakeSub(std::string token_, int temp_);
		std::string UnorderedMapMakeSub(std::string token_, std::string temp_);
	};
}
#endif