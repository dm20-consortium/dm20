#include "Map.h"

namespace CS{
	/**
	 * @fn	void Map<T1, T2>::MapInsert(T1 key_, T2 value_)
	 *
	 * @brief	Mapにデータをインサート
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	key_  	Mapのkey値
	 * @param	value_	MapのValue値
	 */
	template void Map<time_t, std::vector<struct tracelog>>::MapInsert(time_t key_, std::vector<struct tracelog> value_);
	template void Map<int, std::vector<int>>::MapInsert(int key_, std::vector<int> value_);
	template void Map<int, int>::MapInsert(int key_, int value_);
	template void Map<int, std::string>::MapInsert(int key_, std::string value_);
	template void Map<int, time_t>::MapInsert(int key_, time_t value_);
	template void Map<std::string, time_t>::MapInsert(std::string key_, time_t value_);
	template void Map<unsigned long long, struct send_message>::MapInsert(unsigned long long key_, send_message value_);
	template void Map<unsigned long long, struct lid_wave_quality>::MapInsert(unsigned long long key_, lid_wave_quality value_);
	template <typename T1, typename T2>
	void Map<T1, T2>::MapInsert(T1 key_, T2 value_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		map_[key_] = value_;
	}


	/**
	 * @fn	void Map<T1, T2>::MapInsertPair(std::pair<T1, T2> pair_value_)
	 *
	 * @brief	std::pairに格納されたデータをMapにインサート
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	pair_value_  	pairに格納されたインサート用データ
	 */
	template void Map<time_t, std::vector<struct tracelog>>::MapInsertPair(std::pair<time_t, std::vector<struct tracelog>> pair_value_);
	template void Map<int, std::vector<int>>::MapInsertPair(std::pair<int, std::vector<int>> pair_value_);
	template void Map<unsigned long long, struct lid_wave_quality>::MapInsertPair(std::pair<unsigned long long, struct lid_wave_quality> pair_value_);
	template <typename T1, typename T2>
	void Map<T1, T2>::MapInsertPair(std::pair<T1, T2> pair_value_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		map_[pair_value_.first] = pair_value_.second;
	}



	/**
	 * @fn	std::vector<T2> Map<T1, T2>::MapConditionSearchValueVector(int get_time_)
	 *
	 * @brief	
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	key_	Mapの検索対象key値
	 * @param	vector_num_	Vectorの検索対象要素番号
	 *
	 * @return	map_[key_].at(vector_num_) 検索対象key値に対応するValue値
	 */
	template std::vector<struct lid_wave_quality> Map<unsigned long long, struct lid_wave_quality>::MapConditionSearchValueVector(int get_time_);
	template <typename T1, typename T2>
	std::vector<T2> Map<T1, T2>::MapConditionSearchValueVector(int get_time_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		std::vector<T2> vec;
		auto it = map_.begin();
		while(it != map_.end()){
			if(it->first > (T1)((time(NULL) - get_time_) * 1000000000)){
				vec.push_back(it->second);
			}
			++it;
		}
		return vec;
	}



	/**
	 * @fn	void Map<T1, T2>::MapErase(T1 key_)
	 *
	 * @brief	Key値を引数にしてマップからエントリを削除
	 * 			本関数利用に際しては呼び出し側でtry/catchが必要
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	key_ Mapの検索対象key値
	 */
	template void Map<int, int>::MapErase(int key_);
	template void Map<int, std::string>::MapErase(int key_);
	template void Map<int, time_t>::MapErase(int key_);
	template void Map<std::string, time_t>::MapErase(std::string key_);
	template void Map<std::string, std::vector<std::string>>::MapErase(std::string key_);
	template void Map<unsigned long long, struct send_message>::MapErase(unsigned long long key_);
	template <typename T1, typename T2>
	void Map<T1, T2>::MapErase(T1 key_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		auto itr = map_.find(key_);
		if(itr != map_.end()){
			map_.erase(key_);
		}
	}


	/**
	 * @fn	void Map<T1, T2>::MapEraseOldTime(time_t aging_time_)
	 *
	 * @brief	現在時刻から引数である時間を引いた値より更に古い時刻をValue値にもつレコードを削除
	 * 			本関数利用に際しては呼び出し側でtry/catchが必要
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	aging_time_ 削除対象時間
	 * 
	 */
	template void Map<time_t, std::vector<tracelog>>::MapEraseOldTime(time_t aging_time_);
	template <typename T1, typename T2>
	void Map<T1, T2>::MapEraseOldTime(time_t aging_time_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		
		auto it = map_.begin();
		while(it != map_.end()){
			
			if(it->first < (time(NULL) - aging_time_)){
				map_.erase(it++);
			}
			else{
				++it;
			}
		}
	}



	/**
	 * @fn	void Map<T1, T2>::MapEraseOldTimeNs(T1 now_time_)
	 *
	 * @brief	ナノ秒で表した引数の時刻よりも古い(小さい)値を持つKey値のレコードを削除
	 * 			本関数利用に際しては呼び出し側でtry/catchが必要
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	now_time_ 時間(ナノ秒)
	 * 
	 */
	template void Map<unsigned long long, struct send_message>::MapEraseOldTimeNs(unsigned long long now_time_);
	template void Map<unsigned long long, struct lid_wave_quality>::MapEraseOldTimeNs(unsigned long long now_time_);
	template <typename T1, typename T2>
	void Map<T1, T2>::MapEraseOldTimeNs(T1 now_time_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		for(auto it = map_.begin(); it != map_.end();){
			if(it->first < now_time_){
				it = map_.erase(it);
			}
			else{
				it++;
			}
		}
	}


	/**
	 * @fn	void Map<T1, T2>::MapGetConditionValue(float condition_key_)
	 *
	 * @brief	Mapを先頭から引いてkey値が引数の値よりも大きくなるときのvalue値を取得する
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	condition_key_ 比較対象のkey値
	 * 
	 */
	template int Map<float, int>::MapGetConditionValue(float condition_key_);
	template <typename T1, typename T2>
	T2 Map<T1, T2>::MapGetConditionValue(T1 condition_key_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		auto it = map_.end();
		while(it != map_.begin()){
			if(condition_key_ <= it->first){
				return it->second;
			}
			else{
				--it;
			}
		}
		return 0;
	}


	/**
	 * @fn	void Map<T1, T2>::MapShow()
	 *
	 * @brief	Mapの全内容を表示
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 */
	template void Map<int, int>::MapShow();
	template void Map<float, int>::MapShow();
	template void Map<int, std::string>::MapShow();
	template void Map<int, time_t>::MapShow();
	template void Map<std::string, time_t>::MapShow();
	template <typename T1, typename T2>
	void Map<T1, T2>::MapShow()
	{
		for(auto elm : map_){
			try{
				std::cout << elm.first << ", " << elm.second << std::endl;
			}catch(...){
				std::cout << "MapShow Error" << std::endl;
			}
		}
	}


	/**
	 * @fn	void Map<T1, T2>::MapShowStruct()
	 *
	 * @brief	Value値が構造体であるMapのKey値のみの一覧を表示
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 */
	template void Map<unsigned long long, struct send_message>::MapShowStruct();
	template void Map<time_t, std::vector<tracelog>>::MapShowStruct();
	template <typename T1, typename T2>
	void Map<T1, T2>::MapShowStruct()
	{
		for(auto elm : map_){
			try{
				std::cout << elm.first << std::endl;
			}catch(...){
				std::cout << "MapShow Error" << std::endl;
			}
		}
	}

	/**
	 * @fn	void Map<T1, T2>::MapShowVector()
	 *
	 * @brief	Value値がVector型であるMapのKey値とVectorの先頭のみを表示
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 */
	template void Map<std::string, std::vector<std::string>>::MapShowVector();
	template <typename T1, typename T2>
	void Map<T1, T2>::MapShowVector()
	{
		for(auto elm : map_){
			try{
				std::cout << elm.first << ", " << elm.second.at(0) << std::endl;
			}catch(...){
				std::cout << "MapShow Error" << std::endl;
			}
		}
	}


/**
 * @brief map_elmで指定したValueのvectorのvector_elm要素の値を取得
 * 
 * @param map_elm mapの要素番号
 * @param vector_elm vectorの要素番号
 * 
 * @return int　vectorのvector_elm番の値
 */
	template int Map<int, std::vector<int>>::MapGetVectorValueAt(int map_elm, int vector_elm);
	template <typename T1, typename T2>
	int Map<T1, T2>::MapGetVectorValueAt(int map_elm, int vector_elm)
	{
		auto iter = map_.begin();
		iter = std::next(iter,map_elm);
		try{
			return iter->second.at(vector_elm);
		}catch(...){
			return 0;
		}
	}


	/**
	* @brief Value値がstruct tracelogであるmapのみ対応。tracelog.station_idが0以外のValueの要素数を取得
	* 
	* (Detailed description)
	* 
	* @param tracelog_number_
	* 
	* @return int　tracelog.station_idが0以外のValueの要素数
	*/
	template int Map<time_t, std::vector<struct tracelog>>::MapVectorNotZeroCount(int tracelog_number_);
	template <typename T1, typename T2>
	int Map<T1, T2>::MapVectorNotZeroCount(int tracelog_number_)
	{
		int count = 1;
		int i = 0;
		for(auto elm : map_){
			i = 0;
			for(std::vector<struct tracelog>::const_iterator ci = elm.second.begin(); ci != elm.second.end(); ci++){
				if(ci->station_id != 0){
					i++;
				}
			}
			if(count >= tracelog_number_){
				break;
			}
			count++;
		}
		return i;
	}


	/**
	* @brief 指定tracelog_number_のValueのVectorの指定trace_point_のtracelog.log_timeの値を取得 
	* 
	* @param tracelog_number_ tracelog番号
	* @param trace_point_ トレースポイント
	* 
	* @return time_t ログ時刻
	*/
	template time_t Map<time_t, std::vector<struct tracelog>>::MapVectorGetLogtime(int tracelog_number_, int trace_point_);
	template <typename T1, typename T2>
	time_t Map<T1, T2>::MapVectorGetLogtime(int tracelog_number_, int trace_point_)
	{
		int count = 1;
		int tp = 1;
		time_t log_time_ = 0;
		for(auto elm : map_){
			tp = 1;
			for(std::vector<struct tracelog>::const_iterator ci = elm.second.begin(); ci != elm.second.end(); ci++){
				if(tp >= trace_point_){
					log_time_ = ci->log_time;
					break;
				}
				tp++;
			}
			if(count >= tracelog_number_){
				break;
			}
			count++;
		}
		return log_time_;
	}


	/**
	* @brief 指定tracelog_number_のValueのVectorの指定trace_point_のtracelog.queue_sizeの値を取得 
	* 
	* @param tracelog_number_ tracelog番号
	* @param trace_point_ トレースポイント
	* 
	 * @return int queue_size
	 */
	template int Map<time_t, std::vector<struct tracelog>>::MapVectorGetQueueSize(int tracelog_number_, int trace_point_);
	template <typename T1, typename T2>
	int Map<T1, T2>::MapVectorGetQueueSize(int tracelog_number_, int trace_point_)
	{
		int count = 1;
		int tp = 1;
		int queue_size_ = 0;
		for(auto elm : map_){
			tp = 1;
			for(std::vector<struct tracelog>::const_iterator ci = elm.second.begin(); ci != elm.second.end(); ci++){
				if(tp >= trace_point_){
					queue_size_ = ci->queue_size;
					break;
				}
				tp++;
			}
			if(count >= tracelog_number_){
				break;
			}
			count++;
		}
		return queue_size_;
	}


	/**
	 * @brief 指定number_目のMapのkey値とValueのペアを取得
	 * 
	 * @param number_ mapの要素番号
	 * 
	 * @return std::pair<T1,T2> Mapのkey値とValueのペア
	 */
	template std::pair<time_t, std::vector<tracelog>> Map<time_t, std::vector<struct tracelog>>::MapPair(int number_);
	template std::pair<int, std::vector<int>> Map<int, std::vector<int>>::MapPair(int number_);
	template std::pair<unsigned long long, struct lid_wave_quality> Map<unsigned long long, struct lid_wave_quality>::MapPair(int number_);
	template <typename T1, typename T2>
	std::pair<T1, T2> Map<T1, T2>::MapPair(int number_)
	{
		// int count = 1;
		// time_t fi = 0;
		// std::vector<tracelog> se;
		int count = 1;
		T1 first_ = (T1)0;
		T2 second_ = (T2){0};

		for(auto elm : map_){
			if(count >= number_){
				first_ = elm.first;
				second_ = elm.second;
				break;
			}
			count++;
		}
		return std::make_pair(first_, second_);
	}


	/**
	 * @fn	int Map<T1, T2>::MapSize()
	 *
	 * @brief	Map内のデータ数を表示
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 *
	 * @return	map_.size() データ数
	 */
	template int Map<int, int>::MapSize();
	template int Map<int, std::vector<int>>::MapSize();
	template int Map<int, std::string>::MapSize();
	template int Map<int, time_t>::MapSize();
	template int Map<std::string, time_t>::MapSize();
	template int Map<unsigned long long, struct send_message>::MapSize();
	template int Map<time_t, std::vector<struct tracelog>>::MapSize();
	template int Map<unsigned long long, struct lid_wave_quality>::MapSize();
	template <typename T1, typename T2>
	int Map<T1, T2>::MapSize()
	{
		return map_.size();
	}


	/**
	 * @fn	void Map<T1, T2>::MapUpdate(T1 key_, T2 value_)
	 *
	 * @brief	指定のKey値のValue値を指定のValue値で更新
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	key_  	Mapの指定Key値
	 * @param	value_	Mapの指定Key値のValue値
	 */
	template void Map<int, int>::MapUpdate(int key_, int value_);
	template void Map<int, std::vector<int>>::MapUpdate(int key_, std::vector<int> value_);
	template void Map<int, std::string>::MapUpdate(int key_, std::string value_);
	template void Map<int, time_t>::MapUpdate(int key_, time_t value_);
	template void Map<std::string, time_t>::MapUpdate(std::string key_, time_t value_);
	template <typename T1, typename T2>
	void Map<T1, T2>::MapUpdate(T1 key_, T2 value_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		map_.at(key_) = value_;
	}


	/**
	 * @fn	void Map<T1, T2>::MapMake(const char* filename)
	 *
	 * @brief	CSVファイルからMapを作成
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	filename	CSVファイル名
	 */
	template void Map<float, int>::MapMake(const char* filename);
	template void Map<int, int>::MapMake(const char* filename);
	template void Map<int, std::string>::MapMake(const char* filename);
	template <typename T1, typename T2>
	void Map<T1, T2>::MapMake(const char* filename)
	{
		std::ifstream ifs(filename);
		if(!ifs){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << filename << "ファイル読み込みエラー。" << std::endl;
			perror("MapMake");
		}
		std::string str;
		while(getline(ifs, str)){
			std::string token;
			std::istringstream stream(str);
			int clm = 0;
			int temp1 = 0;
			T2 temp2;
			while(getline(stream, token, ',')){
				if(clm==0){
					temp1 = stoi(token);
					clm++;
				}else{
					temp2 = MapMakeSub(token, temp2);
					MapInsert(temp1,temp2);
					clm--;
				}
			}
		}
	}


	/**
	 * @fn	int Map<T1, T2>::MapMakeSub(std::string token_, int temp_)
	 *
	 * @brief	MapMakeサブ関数
	 * 			値をintにして返還
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @tparam	T1	Generic type parameter.
	 * @param	token_	キー
	 * @param	temp_ 	値
	 *
	 * @return	int型の値
	 */
	template int Map<int, int>::MapMakeSub(std::string token_, int temp_);
	template int Map<int, std::string>::MapMakeSub(std::string token_, int temp_);
	template <typename T1, typename T2>
	int Map<T1, T2>::MapMakeSub(std::string token_, int temp_)
	{
		temp_ = stoi(token_);
		return temp_;
	}


	/**
	 * @fn	std::string Map<T1, T2>::MapMakeSub(std::string token_, std::string temp_)
	 *
	 * @brief	MapMakeサブ関数
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @tparam	T1	Generic type parameter.
	 * @param	token_	キー
	 * @param	temp_ 	値
	 *
	 * @return	string型の値
	 */
	template std::string Map<int, int>::MapMakeSub(std::string token_, std::string temp_);
	template std::string Map<int, std::string>::MapMakeSub(std::string token_, std::string temp_);
	template <typename T1, typename T2>
	std::string Map<T1, T2>::MapMakeSub(std::string token_, std::string temp_)
	{
		temp_ = token_;
		return temp_;
	}


	/**
	 * @brief	Mapクリア
	 *
	 * @author	Nagoya University
	 * @date	2020/03/23
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 *
	 */
	template void Map<time_t, std::vector<struct tracelog>>::MapClear();
	template void Map<int, std::vector<int>>::MapClear();
	template <typename T1, typename T2>
	void Map<T1, T2>::MapClear()
	{
		map_.clear();
	}

}
