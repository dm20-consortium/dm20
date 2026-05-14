#ifndef MAP_H
#define MAP_H

#include <thread>
#include <map>  //Map用
#include <queue>          //キューバッファ用
#include <array>
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
	 * @class	Map
	 *
	 * @brief	DM2.0通信用通信部用mapクラス
	 *
	 * @author	Nagoya University
	 * @date	2019/05/17
	 */
	class Map {
	private:
		std::map<T1, T2, std::greater<T1>> map_; //MAP作成
		std::mutex mtx_; //mutex
		std::condition_variable cv_; //条件排他
	public:
		void MapInsert(T1 key_, T2 value_);
		// void MapInsertPair(std::pair<time_t, std::vector<struct tracelog>> pair_value_);
		void MapInsertPair(std::pair<T1, T2> pair_value_);
		// void MapCopy(Map<unsigned long long, struct lid_wave_quality> src_map_);

		void MapInsertVector(T1 key_, int vector_max_, int vector_num_, std::string vector_value_);

		std::vector<T2> MapConditionSearchValueVector(int key_);

		T1 MapSearchValueVector(T1 key_, int vector_num_);

		int MapSearchValueSub(int second_);

		std::string MapSearchValueSub(std::string second_);
		
		void MapErase(T1 key_);

		void MapEraseOldTime(time_t aging_time_);
		
		void MapEraseOldTimeNs(T1 now_time_);

		T2 MapGetConditionValue(T1 condition_key_);
		
		void MapShow();

		void MapShowStruct();
		
		void MapShowVector();
		
		int MapGetVectorValueAt(int map_elm, int vector_elm);
	
		int MapVectorNotZeroCount(int tracelog_number_);
	
		time_t MapVectorGetLogtime(int tracelog_number_, int trace_point_);
	
		int MapVectorGetQueueSize(int tracelog_number_, int trace_point_);

		std::pair<T1, T2> MapPair(int number_);

		int MapSize();

		void MapUpdate(T1 key_, T2 value_);

		void MapMake(const char* filename);

		int MapMakeSub(std::string token_, int temp_);
		std::string MapMakeSub(std::string token_, std::string temp_);

		void MapClear();
	};
}
#endif