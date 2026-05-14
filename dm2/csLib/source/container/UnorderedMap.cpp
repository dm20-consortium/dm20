#include "UnorderedMap.h"

namespace CS{
	/**
	 * @fn	void UnorderedMap<T1, T2>::UnorderedMapInsert(T1 key_, T2 value_)
	 *
	 * @brief	UnorderedMapにデータをインサート
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	key_  	UnorderedMapのkey値
	 * @param	value_	UnorderedMapのValue値
	 * @return	int -1: エラー, 0:正常
	 * 
	 */
	template int UnorderedMap<int, int>::UnorderedMapInsert(int key_, int value_);
	template int UnorderedMap<int, std::vector<int>>::UnorderedMapInsert(int key_, std::vector<int> value_);
	template int UnorderedMap<int, std::string>::UnorderedMapInsert(int key_, std::string value_);
	template int UnorderedMap<int, time_t>::UnorderedMapInsert(int key_, time_t value_);
	template int UnorderedMap<unsigned long long, time_t>::UnorderedMapInsert(unsigned long long key_, time_t value_);
	template int UnorderedMap<std::string, time_t>::UnorderedMapInsert(std::string key_, time_t value_);
	template int UnorderedMap<unsigned long long, struct send_message>::UnorderedMapInsert(unsigned long long key_, send_message value_);
	template <typename T1, typename T2>
	int UnorderedMap<T1, T2>::UnorderedMapInsert(T1 key_, T2 value_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		auto itr = u_map_.find(key_);
		if(itr != u_map_.end()){
			return -1;				
		}else{
			u_map_[key_] = value_;
			return 0;
		}
	}

	/**
	 * @fn	void UnorderedMap<T1, T2>::UnorderedMapInsertVector(T1 key_, int vector_max_, int vector_num_, std::string vector_value_)
	 *
	 * @brief	UnorderedMapのValue値がVector型のUnorderedMapにデータをインサート
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	key_  			UnorderedMapのkey値
	 * @param	vector_max_  	UnorderedMapのVector型のValue値の要素数の最大値
	 * @param	vector_num_ 	UnorderedMapのVector型のValue値のインサートする要素番号
	 * @param	vector_value_	UnorderedMapのVector型のValue値の値
	 */
	template void UnorderedMap<int, std::vector<int>>::UnorderedMapInsertVector(int key_, int vector_max_, int vector_num_, int vector_value_);
	template void UnorderedMap<unsigned long long, std::vector<unsigned long long>>::UnorderedMapInsertVector(unsigned long long key_, int vector_max_, int vector_num_, unsigned long long vector_value_);
	template void UnorderedMap<std::string, std::vector<std::string>>::UnorderedMapInsertVector(std::string key_, int vector_max_, int vector_num_, std::string vector_value_);
	template <typename T1, typename T2>
	void UnorderedMap<T1, T2>::UnorderedMapInsertVector(T1 key_, int vector_max_, int vector_num_, T1 vector_value_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		std::vector<T1> data(vector_max_);
		data.at(vector_num_) = vector_value_;
		u_map_[key_] = data;
	}

	
	/**
	 * @brief key_値のValueに(vector_max_-1)の要素数を持つvectorを作成。vector_valueをvectorの(vector_num_+1)要素目にInsert。vectorの第1要素のStrigにはVectorサイズをInsert 
	 * 
	 * @param key_ key値
	 * @param vector_max_ valueのVectorの最大要素数-1
	 * @param vector_num_ vector_value_をインサートする要素番号-1
	 * @param vector_value_ Insertする値
	 * 
	 * @return void
	 */
	template void UnorderedMap<std::string, std::vector<std::string>>::UnorderedMapInsertVectorPlusSize(std::string key_, int vector_max_, int vector_num_, std::string vector_value_);
	template <typename T1, typename T2>
	void UnorderedMap<T1, T2>::UnorderedMapInsertVectorPlusSize(T1 key_, int vector_max_, int vector_num_, T1 vector_value_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		std::vector<T1> data(vector_max_ + 1);
		data.at(0) = std::to_string(vector_max_ - 1);
		data.at(vector_num_ + 1) = vector_value_;
		u_map_[key_] = data;
	}

	/**
	 * @fn	void UnorderedMap<T1, T2>::UnorderedMapPushbackVector(T1 key_, T1 vector_value_)
	 *
	 * @brief	ValueがVectorのマップに対して指定のキー値のVectorに要素を追加
	 *
	 * @author	Nagoya University
	 * @date	2020/02/20
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	key_  			UnorderedMapのkey値
	 * @param	vector_value_	UnorderedMapのVector型のValue値の値
	 */
	template void UnorderedMap<int, std::vector<int>>::UnorderedMapPushbackVector(int key_, int vector_value_);
	template <typename T1, typename T2>
	void UnorderedMap<T1, T2>::UnorderedMapPushbackVector(T1 key_, T1 vector_value_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		auto itr = u_map_.find(key_);
		if(itr != u_map_.end()){
			itr->second.push_back(vector_value_);
		}else{
			throw std::out_of_range("未登録のIDを指定!");
		}
	}


	/**
	 * @fn	void UnorderedMap<T1, T2>::UnorderedMapPushbackVectorUnique(T1 key_, T1 vector_value_)
	 *
	 * @brief	ValueがVectorのマップに対して指定のキー値のVectorに要素を追加する。ただし重複データがある場合は追加しない。
	 *
	 * @author	Nagoya University
	 * @date	2020/02/20
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	key_  			UnorderedMapのkey値
	 * @param	vector_value_	UnorderedMapのVector型のValue値の値
	 */
	template void UnorderedMap<int, std::vector<int>>::UnorderedMapPushbackVectorUnique(int key_, int vector_value_);
	template void UnorderedMap<unsigned long long, std::vector<unsigned long long>>::UnorderedMapPushbackVectorUnique(unsigned long long key_, unsigned long long vector_value_);
	template <typename T1, typename T2>
	void UnorderedMap<T1, T2>::UnorderedMapPushbackVectorUnique(T1 key_, T1 vector_value_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		auto itr = u_map_.find(key_);
		if(itr != u_map_.end()){
			auto find_itr = find(itr->second.begin(), itr->second.end(), vector_value_);
			if(find_itr == itr->second.cend()){
				itr->second.push_back(vector_value_);
			}
		}else{
			throw std::out_of_range("未登録のIDを指定!");
		}
	}


	/**
	 * @fn	void UnorderedMap<T1, T2>::UnorderedMapUpdateVectorValue(T1 key_, int vector_num_, std::string vector_value_)
	 *
	 * @brief	UnorderedMapのValue値がVector型のUnorderedMapにデータをアップデート
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	key_  			UnorderedMapのkey値
	 * @param	vector_num_ 	UnorderedMapのVector型のValue値のインサートする要素番号
	 * @param	vector_value_	UnorderedMapのVector型のValue値の値
	 */
	template void UnorderedMap<std::string, std::vector<std::string>>::UnorderedMapUpdateVectorValue(std::string key_, int vector_num_, std::string vector_value_);
	template void UnorderedMap<int, std::vector<int>>::UnorderedMapUpdateVectorValue(int key_, int vector_num_, int vector_value_);
	template void UnorderedMap<unsigned long long, std::vector<unsigned long long>>::UnorderedMapUpdateVectorValue(unsigned long long key_, int vector_num_, unsigned long long vector_value_);
	template <typename T1, typename T2>
	void UnorderedMap<T1, T2>::UnorderedMapUpdateVectorValue(T1 key_, int vector_num_, T1 vector_value_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		u_map_[key_].at(vector_num_) = vector_value_;
	}

	/**
	* @brief key_値のVlaueであるVectorのvector_num_で指定する要素番号の値をvector_value_でアップデート。そして、Vectorの第１要素の値を１減らす。
	* 
	* @param key_
	* @param vector_num_
	* @param vector_value_
	* 
	* @return void
	*/
	template void UnorderedMap<std::string, std::vector<std::string>>::UnorderedMapUpdateVectorValueThenDecrease(std::string key_, int vector_num_, std::string vector_value_);
	template <typename T1, typename T2>
	void UnorderedMap<T1, T2>::UnorderedMapUpdateVectorValueThenDecrease(T1 key_, int vector_num_, T1 vector_value_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		int total_num = stoi(u_map_[key_].at(0));
		total_num--;
		u_map_[key_].at(0) = std::to_string(total_num);
		u_map_[key_].at(vector_num_ + 1) = vector_value_;
	}


	/**
	 * @fn	bool UnorderedMap<T1, T2>::UnorderedMapKeyExist(T1 key_)
	 *
	 * @brief	UnorderedMapにて引数のKey値が存在するかを確認
	 * 			排他処理実施
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @param	key_  UnorderedMapの検索対象key値
	 * @return	result_ true:引数のkey値が存在する
	 * 			false:引数のkey値が存在しない
	 */
	template bool UnorderedMap<std::string, time_t>::UnorderedMapKeyExist(std::string key_);
	template bool UnorderedMap<int, std::vector<int>>::UnorderedMapKeyExist(int key_);
	template bool UnorderedMap<unsigned long long, struct send_message>::UnorderedMapKeyExist(unsigned long long key_);
	template bool UnorderedMap<unsigned long long, std::vector<unsigned long long>>::UnorderedMapKeyExist(unsigned long long key_);
	template bool UnorderedMap<unsigned long long, std::string>::UnorderedMapKeyExist(unsigned long long key_);
	template <typename T1, typename T2>
	bool UnorderedMap<T1, T2>::UnorderedMapKeyExist(T1 key_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		bool result_;
		auto itr = u_map_.find(key_);
		if(itr != u_map_.end()){
			result_ = true;
		}else{
			result_ = false;
		}
		return result_;
	}

	/**
	 * @fn	bool UnorderedMap<T1, T2>::UnorderedMapKeyExistThenUpdateElseInsert(T1 key_, T2 value_)
	 *
	 * @brief	UnorderedMapにて引数のKey値が存在すればUpadateを実施
	 * 			排他処理実施
	 *
	 * @author	Nagoya University
	 * @date	2020/02/27
	 *
	 * @tparam	T1	Generic type parameter.
	 * @param	key_  UnorderedMapの検索対象key値
	 * @param	value_  Updateする値
	 */
	template void UnorderedMap<int, std::vector<int>>::UnorderedMapKeyExistThenUpdateElseInsert(int key_, std::vector<int> value_);
	template void UnorderedMap<unsigned long long, std::vector<unsigned long long>>::UnorderedMapKeyExistThenUpdateElseInsert(unsigned long long key_, std::vector<unsigned long long> value_);
	template <typename T1, typename T2>
	void UnorderedMap<T1, T2>::UnorderedMapKeyExistThenUpdateElseInsert(T1 key_, T2 value_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		auto itr = u_map_.find(key_);
		if(itr != u_map_.end()){
			//Update処理
			u_map_.at(key_) = value_;
		}else{
			//Insert処理
			u_map_[key_] = value_;
		}
	}


	/**
	 * @fn	bool UnorderedMap<T1, T2>::UnorderedMapKeyExistVector(T1 key_)
	 *
	 * @brief	Value値がVector型のUnorderedMapにて引数のKey値が存在するかを確認
	 * 			排他処理実施
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	key_  UnorderedMapの検索対象key値
	 * @return	result_ rue:引数のkey値が存在する
	 * 			false:引数のkey値が存在しない
	 */
	template bool UnorderedMap<std::string, std::vector<std::string>>::UnorderedMapKeyExistVector(std::string key_);
	template <typename T1, typename T2>
	bool UnorderedMap<T1, T2>::UnorderedMapKeyExistVector(T1 key_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		bool result_;
		auto itr = u_map_.find(key_);
		if(itr != u_map_.end()){
			result_ = true;
		}else{
			result_ = false;
		}
		return result_;
	}


	/**
	 * @fn	T2 UnorderedMap<T1, T2>::UnorderedMapGetValue(T1 key_)
	 *
	 * @brief	Key値を引数にしてUnorderedMapからvalue値を取得
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @exception	std::out_of_range	指定したKey値がUnorderedMapに存在しない
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	key_	UnorderedMapの検索対象key値
	 *
	 * @return	UnorderedMapGetValueSub(itr->second) キーに対応するValue値
	 */
	template int UnorderedMap<int, int>::UnorderedMapGetValue(int key_);
	template unsigned long long UnorderedMap<unsigned long long, unsigned long long>::UnorderedMapGetValue(unsigned long long key_);
	template std::string UnorderedMap<int, std::string>::UnorderedMapGetValue(int key_);
	template std::string UnorderedMap<unsigned long long, std::string>::UnorderedMapGetValue(unsigned long long key_);
	template time_t UnorderedMap<int, time_t>::UnorderedMapGetValue(int key_);
	template time_t UnorderedMap<std::string, time_t>::UnorderedMapGetValue(std::string key_);
	template std::vector<int> UnorderedMap<int, std::vector<int>>::UnorderedMapGetValue(int key_);
	template std::vector<unsigned long long> UnorderedMap<unsigned long long, std::vector<unsigned long long>>::UnorderedMapGetValue(unsigned long long key_);
	template <typename T1, typename T2>
	T2 UnorderedMap<T1, T2>::UnorderedMapGetValue(T1 key_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		auto itr = u_map_.find(key_);
		if(itr != u_map_.end()){
			return UnorderedMapGetValueSub(itr->second);
		}else{
			throw std::out_of_range("未登録のIDを指定!");
		}
	}

	/**
	 * @fn	T2 UnorderedMap<T1, T2>::UnorderedMapGetVectorValueAndClear(T1 key_)
	 *
	 * @brief	Key値を引数にしてUnorderedMapからvalue値を取得し、同時にvalue値を削除する
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @exception	std::out_of_range	指定したKey値がUnorderedMapに存在しない
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	key_	UnorderedMapの検索対象key値
	 *
	 * @return	UnorderedMapGetValueSub(itr->second) キーに対応するValue値
	 */

	template std::vector<int> UnorderedMap<int, std::vector<int>>::UnorderedMapGetVectorValueAndClear(int key_);
	template std::vector<unsigned long long> UnorderedMap<unsigned long long, std::vector<unsigned long long>>::UnorderedMapGetVectorValueAndClear(unsigned long long key_);
	template <typename T1, typename T2>
	T2 UnorderedMap<T1, T2>::UnorderedMapGetVectorValueAndClear(T1 key_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		T2 get_value;
		auto itr = u_map_.find(key_);
		if(itr != u_map_.end()){
			get_value = UnorderedMapGetValueSub(itr->second);
			itr->second.clear();
			return get_value;
		}else{
			throw std::out_of_range("未登録のIDを指定!");
		}
	}


	/**
	 * @fn	T1 UnorderedMap<T1, T2>::UnorderedMapGetValueVector(T1 key_, int vector_num_)
	 *
	 * @brief	Key値とVectorの要素番号を引数にしてValue値がVector型のUnorderedMapから指定要素番号のvalue値を取得
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @exception	std::out_of_range	指定したKey値がUnorderedMapに存在しない
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	key_	UnorderedMapの検索対象key値
	 * @param	vector_num_	Vectorの検索対象要素番号
	 *
	 * @return	u_map_[key_].at(vector_num_) 検索対象key値に対応するValue値
	 */
	template std::string UnorderedMap<std::string, std::vector<std::string>>::UnorderedMapGetValueVector(std::string key_, int vector_num_);
	template int UnorderedMap<int, std::vector<int>>::UnorderedMapGetValueVector(int key_, int vector_num_);
	template unsigned long long UnorderedMap<unsigned long long, std::vector<unsigned long long>>::UnorderedMapGetValueVector(unsigned long long key_, int vector_num_);
	template <typename T1, typename T2>
	T1 UnorderedMap<T1, T2>::UnorderedMapGetValueVector(T1 key_, int vector_num_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		auto itr = u_map_.find(key_);
		if(itr != u_map_.end()){
			return u_map_[key_].at(vector_num_);
		}else{
			throw std::out_of_range("未登録のIDを指定!:" + key_);
		}
	}


	/**
	 * @fn	int UnorderedMap<T1, T2>::UnorderedMapGetValueSub(int second_)
	 *
	 * @brief	UnorderedMapGetValueのサブ関数
	 * 			キーに対するintの値をそのまま返還
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	second_	UnorderedMapの検索対象key値
	 *
	 * @return	second_ intの値
	 */
	template int UnorderedMap<int, int>::UnorderedMapGetValueSub(int second_);
	template <typename T1, typename T2>
	int UnorderedMap<T1, T2>::UnorderedMapGetValueSub(int second_)
	{
		return second_;
	}

	/**
	 * @fn	std::vector<int> UnorderedMap<T1, T2>::UnorderedMapGetValueSub(std::vector<int> second_)
	 *
	 * @brief	UnorderedMapGetValueのサブ関数
	 * 			キーに対するvectorをそのまま返還
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	second_	UnorderedMapの検索対象key値
	 *
	 * @return	vector<int>
	 */
	template std::vector<int> UnorderedMap<int, std::vector<int>>::UnorderedMapGetValueSub(std::vector<int> second_);
	template <typename T1, typename T2>
	std::vector<int> UnorderedMap<T1, T2>::UnorderedMapGetValueSub(std::vector<int> second_)
	{
		return second_;
	}

	/**
	 * @fn	std::vector<unsigned long long> UnorderedMap<T1, T2>::UnorderedMapGetValueSub(std::vector<unsigned long long> second_)
	 *
	 * @brief	UnorderedMapGetValueのサブ関数
	 * 			キーに対するvectorをそのまま返還
	 *
	 * @author	
	 * @date	
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	second_	UnorderedMapの検索対象key値
	 *
	 * @return	vector<>
	 */
	template std::vector<unsigned long long> UnorderedMap<unsigned long long, std::vector<unsigned long long>>::UnorderedMapGetValueSub(std::vector<unsigned long long> second_);
	template <typename T1, typename T2>
	std::vector<unsigned long long> UnorderedMap<T1, T2>::UnorderedMapGetValueSub(std::vector<unsigned long long> second_)
	{
		return second_;
	}


	/**
	 * @fn	std::string UnorderedMap<T1, T2>::UnorderedMapGetValueSub(std::string second_)
	 *
	 * @brief	UnorderedMapGetValueのサブ関数
	 * 			キーに対するstringの値をトリムして返還
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	second_	UnorderedMapの検索対象key値
	 *
	 * @return	dm2util.Trim(second_) stringの値
	 */
	template std::string UnorderedMap<int, std::string>::UnorderedMapGetValueSub(std::string second_);
	template std::string UnorderedMap<unsigned long long, std::string>::UnorderedMapGetValueSub(std::string second_);
	template <typename T1, typename T2>
	std::string UnorderedMap<T1, T2>::UnorderedMapGetValueSub(std::string second_)
	{
		Util dm2util;
		return dm2util.Trim(second_);
	}



	/**
	 * @fn	void UnorderedMap<T1, T2>::UnorderedMapErase(T1 key_)
	 *
	 * @brief	Key値を引数にしてマップからエントリを削除
	 * 			本関数利用に際しては呼び出し側でtry/catchが必要
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	key_ UnorderedMapの検索対象key値
	 */
	template void UnorderedMap<int, int>::UnorderedMapErase(int key_);
	template void UnorderedMap<int, std::string>::UnorderedMapErase(int key_);
	template void UnorderedMap<int, time_t>::UnorderedMapErase(int key_);
	template void UnorderedMap<unsigned long long, time_t>::UnorderedMapErase(unsigned long long key_);
	template void UnorderedMap<std::string, time_t>::UnorderedMapErase(std::string key_);
	template void UnorderedMap<std::string, std::vector<std::string>>::UnorderedMapErase(std::string key_);
	template void UnorderedMap<int, std::vector<int>>::UnorderedMapErase(int key_);
	template void UnorderedMap<unsigned long long, std::vector<unsigned long long>>::UnorderedMapErase(unsigned long long key_);
	template void UnorderedMap<unsigned long long, struct send_message>::UnorderedMapErase(unsigned long long key_);
	template <typename T1, typename T2>
	void UnorderedMap<T1, T2>::UnorderedMapErase(T1 key_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		auto itr = u_map_.find(key_);
		if(itr != u_map_.end()){
			u_map_.erase(key_);
		}
	}


	/**
	 * @fn	int UnorderedMap<T1, T2>::UnorderedMapEraseRes(T1 key_)
	 *
	 * @brief	Key値を引数にしてマップからエントリを削除
	 * 			本関数利用に際しては呼び出し側でtry/catchが必要
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	key_ UnorderedMapの検索対象key値
	 * 
	 * @return res_ 1:削除対象データあり
	 * 				0:削除対象データなし
	 */
	template int UnorderedMap<std::string, time_t>::UnorderedMapEraseRes(std::string key_);
	template int UnorderedMap<unsigned long long, struct send_message>::UnorderedMapEraseRes(unsigned long long key_);
	template <typename T1, typename T2>
	int UnorderedMap<T1, T2>::UnorderedMapEraseRes(T1 key_)
	{
		int res_ = 0;
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		auto itr = u_map_.find(key_);
		if(itr != u_map_.end()){
			res_ = u_map_.erase(key_);
		}
		return res_;
	}


	/**
	 * @fn	void UnorderedMap<T1, T2>::UnorderedMapEraseOldTime(time_t aging_time_)
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
	template int UnorderedMap<int, time_t>::UnorderedMapEraseOldTime(time_t aging_time_);
	template int UnorderedMap<unsigned long long, time_t>::UnorderedMapEraseOldTime(time_t aging_time_);
	template int UnorderedMap<std::string, time_t>::UnorderedMapEraseOldTime(time_t aging_time_);
	template <typename T1, typename T2>
	int UnorderedMap<T1, T2>::UnorderedMapEraseOldTime(time_t aging_time_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		time_t now_time = time(NULL);
		int erase_count = 0;
		for(auto it = u_map_.begin(); it != u_map_.end();){
			if(it->second < (now_time - aging_time_)){
				it = u_map_.erase(it);
				erase_count++;
			}else{
				it++;
			}
		}
		return erase_count;
	}


	/**
	 * @fn	void UnorderedMap<T1, T2>::UnorderedMapEraseOldTimeNs(T1 now_time_)
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
	template void UnorderedMap<unsigned long long, struct send_message>::UnorderedMapEraseOldTimeNs(unsigned long long now_time_);
	template <typename T1, typename T2>
	void UnorderedMap<T1, T2>::UnorderedMapEraseOldTimeNs(T1 now_time_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		for(auto it = u_map_.begin(); it != u_map_.end();){
			if(it->first < now_time_){
				it = u_map_.erase(it);
			}
			else{
				it++;
			}
		}
		if(u_map_.empty()){
			std::unordered_map<T1, T2>().swap(u_map_);
		}
	}


	/**
	 * @fn	T1 UnorderedMap<T1, T2>::UnorderedMapVectorCombineAndDelete(T1 key_)
	 *
	 * @brief	Key値を引数にしてVector型のUnorderedMapからVector値を取得し、その要素内に空の要素がない場合は全ての要素のValue値を結合し、それを戻り値とする。
	 * 　　　　　更にそのレコードを削除する。
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	key_ UnorderedMapの検索対象key値
	 * @return  combined_str_ 結合されたVectorのValue 
	 * 
	 */
	template std::string UnorderedMap<std::string, std::vector<std::string>>::UnorderedMapVectorCombineAndDelete(std::string key_);
	template <typename T1, typename T2>
	T1 UnorderedMap<T1, T2>::UnorderedMapVectorCombineAndDelete(T1 key_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		std::string combined_str_ = "";
		auto itr = u_map_.find(key_);
		if(itr != u_map_.end()){
			if(any_of(u_map_[key_].begin(), u_map_[key_].end(), [](std::string str_){return str_ == "";}) == true){
				//vector内に空の要素が１つ以上ある
				//何もしない。
			}else{
				//vector内の要素がすべて埋まっている
				for(std::string str_ : u_map_[key_]){
					combined_str_.append(str_);
				}
				u_map_.erase(key_);
				if(u_map_.empty()){
					std::unordered_map<T1, T2>().swap(u_map_);
				}
			}			
		}
		return combined_str_;
	}


	/**
	* @brief  指定Key値のValueのVectorの第１要素の値が０(全ての分割データが集まった状態)の時、要素を全て結合してその値を返す。
	* 
	* @param key_ 指定key値
	* @param total_ 総分割数
	* 
	* @return 結合データ
	*/
	template std::string UnorderedMap<std::string, std::vector<std::string>>::UnorderedMapVectorCombineAndDeletePlusSize(std::string key_, int total_);
	template <typename T1, typename T2>
	T1 UnorderedMap<T1, T2>::UnorderedMapVectorCombineAndDeletePlusSize(T1 key_, int total_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		std::string combined_str_ = "";
		// combined_str_.reserve(MSGSIZE*total_);
		auto itr = u_map_.find(key_);
		if(itr != u_map_.end()){
			if(itr->second.at(0) == "0"){
				//vector内の要素がすべて埋まっている
				combined_str_.reserve(MSGSIZE*total_);
				for(std::string str_ : u_map_[key_]){
					combined_str_.append(str_);
				}
				combined_str_.erase(0,1);
				u_map_.erase(key_);
				if(u_map_.empty()){
					std::unordered_map<T1, T2>().swap(u_map_);
				}
			}else{
				//全てのデータがそろっていないので何もしない。
			}
		}
		return combined_str_;
	}

	/**
	 * @fn	void UnorderedMap<T1, T2>::UnorderedMapShow()
	 *
	 * @brief	UnorderedMapの全内容を表示
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 */
	template void UnorderedMap<int, int>::UnorderedMapShow();
	template void UnorderedMap<int, std::string>::UnorderedMapShow();
	template void UnorderedMap<unsigned long long, std::string>::UnorderedMapShow();
	template void UnorderedMap<int, time_t>::UnorderedMapShow();
	template void UnorderedMap<std::string, time_t>::UnorderedMapShow();
	template <typename T1, typename T2>
	void UnorderedMap<T1, T2>::UnorderedMapShow()
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		for(auto elm : u_map_){
			try{
				std::cout << elm.first << ", " << elm.second << std::endl;
			}catch(...){
				std::cout << "UnorderedMapShow Error" << std::endl;
			}
		}
	}


	/**
	 * @fn	void UnorderedMap<T1, T2>::UnorderedMapShowStruct()
	 *
	 * @brief	Value値が構造体であるUnorderedMapのKey値のみの一覧を表示
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 */
	template void UnorderedMap<unsigned long long, struct send_message>::UnorderedMapShowStruct();
	template <typename T1, typename T2>
	void UnorderedMap<T1, T2>::UnorderedMapShowStruct()
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		for(auto elm : u_map_){
			try{
				std::cout << elm.first << std::endl;
			}catch(...){
				std::cout << "UnorderedMapShow Error" << std::endl;
			}
		}
	}

	/**
	 * @fn	void UnorderedMap<T1, T2>::UnorderedMapShowVector()
	 *
	 * @brief	Value値がVector型であるUnorderedMapのKey値とVectorの先頭のみを表示
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 */
	template void UnorderedMap<std::string, std::vector<std::string>>::UnorderedMapShowVector();
	template <typename T1, typename T2>
	void UnorderedMap<T1, T2>::UnorderedMapShowVector()
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		for(auto elm : u_map_){
			try{
				std::cout << elm.first << ", " << elm.second.at(0) << std::endl;
			}catch(...){
				std::cout << "UnorderedMapShow Error" << std::endl;
			}
		}
	}


	/**
	 * @fn	std::vector<T1> UnorderedMap<T1, T2>::UnorderedMapKeyExtraction()
	 *
	 * @brief	UnorderedMapの全Key値を戻り値のVectorにpush_backする
	 *
	 * @author	Shinichi Kusayama
	 * @date	2023/12/29
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * 
	 * @return sm UnorderedMapの全Key値をVector値にpush_backした結果
	 */
	template std::vector<int> UnorderedMap<int, std::string>::UnorderedMapKeyExtraction();
	template std::vector<unsigned long long> UnorderedMap<unsigned long long, std::string>::UnorderedMapKeyExtraction();
	template <typename T1, typename T2>
	std::vector<T1> UnorderedMap<T1, T2>::UnorderedMapKeyExtraction()
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		std::vector<T1> sm;
		for(auto elm : u_map_){
			try{
				//UnorderedMapのValue値を戻り値であるVectorにpush_backする
				sm.push_back(elm.first);
			}catch(...){
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "UnorderedMapKeyExtraction Error" << std::endl;
			}
		}
		return sm;
	}

	/**
	 * @fn	std::vector<T2> UnorderedMap<T1, T2>::UnorderedMapValueExtraction()
	 *
	 * @brief	Value値がVector型であるUnorderedMapの全Value値を戻り値のVectorにpush_backする
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * 
	 * @return sm UnorderedMapの全Value値をVector値にpush_backした結果
	 */
	template std::vector<struct send_message> UnorderedMap<unsigned long long, struct send_message>::UnorderedMapValueExtraction();
	template <typename T1, typename T2>
	std::vector<T2> UnorderedMap<T1, T2>::UnorderedMapValueExtraction()
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		std::vector<T2> sm;
		for(auto elm : u_map_){
			try{
				//UnorderedMapのValue値を戻り値であるVectorにpush_backする
				sm.push_back(elm.second);
			}catch(...){
				std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "UnorderedMapValueExtraction Error" << std::endl;
			}
		}
		return sm;
	}


	/**
	 * @fn	T1  UnorderedMap<T1, T2>::UnorderedMapOldKey(time_t aging_time_)
	 *
	 * @brief	引数で指定した時間より古いデータのkey値(int)を取得
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	aging_time_	指定時間(秒)
	 *
	 * @return	elm.first　StationID
	 */
	template int UnorderedMap<int, time_t>::UnorderedMapOldKey(time_t aging_time_);
	template <typename T1, typename T2>
	T1  UnorderedMap<T1, T2>::UnorderedMapOldKey(time_t aging_time_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		for(auto elm : u_map_){
			if(elm.second < (time(NULL) - aging_time_)){
				return elm.first;
			}
		}
		return 0;
	}


	/**
	 * @fn	T1  UnorderedMap<T1, T2>::UnorderedMapOldKeyString(time_t aging_time_)
	 *
	 * @brief	引数で指定した時間より古いデータのkey値(String)を取得
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	aging_time_	指定時間(秒)
	 *
	 * @return	elm.first StationID
	 */
	template std::string UnorderedMap<std::string, time_t>::UnorderedMapOldKeyString(time_t aging_time_);
	template <typename T1, typename T2>
	T1  UnorderedMap<T1, T2>::UnorderedMapOldKeyString(time_t aging_time_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		for(auto elm : u_map_){
			if(elm.second < (time(NULL) - aging_time_)){
				return elm.first;
			}
		}
		return "0";
	}


	/**
	 * @fn	int UnorderedMap<T1, T2>::UnorderedMapSize()
	 *
	 * @brief	UnorderedMap内のデータ数を表示
	 *
	 * @author	Nagoya University
	 * @date	2019/03/26
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 *
	 * @return	u_map_.size() データ数
	 */
	template int UnorderedMap<int, int>::UnorderedMapSize();
	template int UnorderedMap<int, std::string>::UnorderedMapSize();
	template int UnorderedMap<int, time_t>::UnorderedMapSize();
	template int UnorderedMap<unsigned long long, time_t>::UnorderedMapSize();
	template int UnorderedMap<std::string, time_t>::UnorderedMapSize();
	template int UnorderedMap<unsigned long long, struct send_message>::UnorderedMapSize();
	template int UnorderedMap<std::string, std::vector<std::string>>::UnorderedMapSize();
	template int UnorderedMap<int, std::vector<int>>::UnorderedMapSize();
	template int UnorderedMap<unsigned long long, std::vector<unsigned long long>>::UnorderedMapSize();
	template <typename T1, typename T2>
	int UnorderedMap<T1, T2>::UnorderedMapSize()
	{
		return u_map_.size();
	}


	/**
	 * @fn	void UnorderedMap<T1, T2>::UnorderedMapUpdate(T1 key_, T2 value_)
	 *
	 * @brief	指定のKey値のValue値を指定のValue値で更新
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	key_  	UnorderedMapの指定Key値
	 * @param	value_	UnorderedMapの指定Key値のValue値
	 */
	template void UnorderedMap<int, int>::UnorderedMapUpdate(int key_, int value_);
	template void UnorderedMap<int, std::string>::UnorderedMapUpdate(int key_, std::string value_);
	template void UnorderedMap<int, std::vector<int>>::UnorderedMapUpdate(int key_, std::vector<int> value_);
	template void UnorderedMap<int, time_t>::UnorderedMapUpdate(int key_, time_t value_);
	template void UnorderedMap<unsigned long long, time_t>::UnorderedMapUpdate(unsigned long long key_, time_t value_);
	template void UnorderedMap<std::string, time_t>::UnorderedMapUpdate(std::string key_, time_t value_);
	template <typename T1, typename T2>
	void UnorderedMap<T1, T2>::UnorderedMapUpdate(T1 key_, T2 value_)
	{
		std::unique_lock<std::mutex> lk(mtx_); //排他処理
		u_map_.at(key_) = value_;
	}


	/**
	 * @fn	void UnorderedMap<T1, T2>::UnorderedMapMake(const char* filename)
	 *
	 * @brief	CSVファイルからUnorderedMapを作成
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @tparam	T1	Generic type parameter.
	 * @tparam	T2	Generic type parameter.
	 * @param	filename	CSVファイル名
	 */
	template void UnorderedMap<int, int>::UnorderedMapMake(const char* filename);
	template void UnorderedMap<unsigned long long, unsigned long long>::UnorderedMapMake(const char* filename);
	template void UnorderedMap<int, std::string>::UnorderedMapMake(const char* filename);
	template void UnorderedMap<unsigned long long, std::string>::UnorderedMapMake(const char* filename);
	template <typename T1, typename T2>
	void UnorderedMap<T1, T2>::UnorderedMapMake(const char* filename)
	{
		std::ifstream ifs(filename);
		if(!ifs){
			std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << filename << "ファイル読み込みエラー。" << std::endl;
			perror("UnorderedMapMake");
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
					temp1 = stoull(token);
					clm++;
				}else{
					temp2 = UnorderedMapMakeSub(token, temp2);
					UnorderedMapInsert(temp1,temp2);
					clm--;
				}
			}
		}
	}


	/**
	 * @fn	int UnorderedMap<T1, T2>::UnorderedMapMakeSub(std::string token_, int temp_)
	 *
	 * @brief	UnorderedMapMakeサブ関数
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
	template int UnorderedMap<int, int>::UnorderedMapMakeSub(std::string token_, int temp_);
	template int UnorderedMap<int, std::string>::UnorderedMapMakeSub(std::string token_, int temp_);
	template <typename T1, typename T2>
	int UnorderedMap<T1, T2>::UnorderedMapMakeSub(std::string token_, int temp_)
	{
		temp_ = stoi(token_);
		return temp_;
	}


	/**
	 * @fn	std::string UnorderedMap<T1, T2>::UnorderedMapMakeSub(std::string token_, std::string temp_)
	 *
	 * @brief	UnorderedMapMakeサブ関数
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
	template std::string UnorderedMap<int, int>::UnorderedMapMakeSub(std::string token_, std::string temp_);
	template std::string UnorderedMap<int, std::string>::UnorderedMapMakeSub(std::string token_, std::string temp_);
	template <typename T1, typename T2>
	std::string UnorderedMap<T1, T2>::UnorderedMapMakeSub(std::string token_, std::string temp_)
	{
		temp_ = token_;
		return temp_;
	}
}
