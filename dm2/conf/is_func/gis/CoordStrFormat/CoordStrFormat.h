/**
* ヘッダファイル CoordStrFormat.h
* @author    Nagoya University
* @date      2020/03/16
*/

#ifndef COORDSTRFORMAT_H
#define COORDSTRFORMAT_H

#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;

// 0.0のdouble型の数値文字列からマイナス符号を削除する
std::string deleteZeroMinus(std::string str);

// double型の数値文字列から末尾の不要な0を削除する
std::string deleteExtraZero(std::string str);

// GEOSから返却されたGeometry型のWKT文字列から不要な文字を削除する
std::string stringFormatGeosToDm2(std::string wktStr);

#endif
