#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <bitset>
#include <cmath>
#include "BitString.h"

using namespace std;

#include <is/DmException.h>



/**
* ビット配列の値で入った Y を Y のサイズ分 X の比較開始ビット位置から比較し 等しければ true を返す
* X のビットのサイズが Y のビットのサイズより小さい あるいは 比較結果が不等号であれば false を返す.
* @author    Nagoya University
* @date      2022/02/04
* @param     byte_str_x  バイト配列(16進数文字列)
* @param     bit_str_y   ビット配列(2進数文字列)
* @param     start_bit_x 比較開始ビット位置(正の場合上位から、負の場合下位からのビット数)
* @return    bool
*/
bool UF_BitString::isMatched(std::string & byte_str_x, std::string & bit_str_y, int start_bit)
{
	bool ret = false;
	int num_of_bit_x, num_of_bit_y;		// 2進数桁数(1～64桁)
	int /*num_of_nbyte_x,*/ num_of_nbyte_y;	// 16進数桁数(1～16桁)

	/* 2進数での桁数 */
	num_of_bit_x = byte_str_x.size() * 4;
	num_of_bit_y = bit_str_y.size();
	/* 16進数での桁数 */
	/*num_of_nbyte_x = byte_str_x.size();*/
	num_of_nbyte_y = (bit_str_y.size() + 4 - 1) / 4;	// 切り上げ

	/* Xが16桁以内の16進数か、Yが64桁以内の2進数かチェック、エラー時は例外発生 */
	/* unsigned long byte_value_ll; */
	size_t idx;
	/* byte_value_ll = */stoul(byte_str_x, &idx, 16);
	if(byte_str_x.size() != idx) {
		throw CastException("Data type is not match!");
	}
	/* byte_value_ll= */stoull(bit_str_y, &idx, 2);
	if(bit_str_y.size() != idx) {
		throw CastException("Data type is not match!");
	}
	
	// 正の場合上位から、負の場合下位から、比較するXのビット位置をずらす
	//start_bit_x = start_bit;
	int start_bit_x;
	if(start_bit < 0) {
		start_bit_x = num_of_bit_x + start_bit;	// 負値で指定された場合(MATCH_LOW_BIT)、下位ビットから数える
	}
	else {
		start_bit_x = start_bit;
	}
	
	if(num_of_bit_x - start_bit_x >= num_of_bit_y) {
		try {
			/* Xをバイト配列(16進数)からビット配列(2進数)へ変換 */
			std::stringstream ss;
			for(int j = 0; j < num_of_nbyte_y; j++){
				int byte_value = stoi(byte_str_x.substr(j, 1), nullptr, 16);
				ss << std::bitset<4>(byte_value);
			}
			/* ビット配列でXとYを比較 */
			std::string bit_str_x = ss.str();
			if(bit_str_x.substr(start_bit_x, num_of_bit_y) == bit_str_y){
				ret = true;
			}
		} catch (invalid_argument const&) {
		  throw CastException("Data type is not match!");
		} catch (out_of_range const&) {
		  throw CastException("input is out of range!");
		}
	}
	return ret;
}



//void UF_BitString::byteStrToBitStr(const string &byte_str, const string &bit_str, size_t &num_of_bit)
string UF_BitString::byteStrToBitStr(const string &byte_str)
{
	// 16桁以内の16進数かチェック、エラー時は例外発生
	/* unsigned long byte_value_ll; */
	size_t idx;
	/* byte_value_ll = */stoul(byte_str, &idx, 16);
	if(byte_str.size() != idx) {
		throw CastException("Data type is not match!");
	}
	// バイト配列(16進数)からビット配列(2進数)へ変換
	std::stringstream ss;
	for(int i = 0; i < (int)byte_str.size(); i++){
		int byte_value = stoi(byte_str.substr(i, 1), nullptr, 16);
		ss << std::bitset<4>(byte_value);
	}
	string ret = ss.str();
	return ret;
}

void UF_BitString::checkBitStr(const string &bit_str)
{
	/* 64桁以内の2進数かチェック、エラー時は例外発生 */
	/* unsigned long byte_value_ll; */
	size_t idx;
	/* byte_value_ll= */stoul(bit_str, &idx, 2);
	if(bit_str.size() != idx) {
		throw CastException("Data type is not match!");
	}
}

int UF_BitString::strToInt(const string &str)
{
	size_t idx;
	int ret = static_cast<int>(stoul(str, &idx, 10));
	if(str.size() != idx) {
		throw CastException("Data type is not match!");
	}
	return ret;
}
