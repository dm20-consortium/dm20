/**
* ヘッダファイル BitString.h
* @author    Nagoya University
* @date      2022/02/04
*/

#ifndef BITSTRING_H
#define BITSTRING_H

#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;

namespace UF_BitString {

	/* バイト配列(16進数)とビット配列(2進数)の比較判定 */
	bool isMatched(std::string & byte_str_x, std::string & bit_str_y, int start_bit);
	string byteStrToBitStr(const string &byte_str);
	void checkBitStr(const string &bit_str);
	int strToInt(const string &str);

}
#endif
