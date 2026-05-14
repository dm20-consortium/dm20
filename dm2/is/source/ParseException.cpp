/*
 * クラス ParseException
 *
 * クエリ解析に関する例外を通知するためのクラス．
 */

/* クラス定義ヘッダファイル */
#include "Parser.h"

using namespace IS;

/**
 * コンストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	msg	エラーメッセージ
 */
ParseException::ParseException(string msg)
{
	this->message = msg;
}

/**
 * エラーメッセージの文字列を返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	エラーメッセージ文字列
 */
string ParseException::getMessage() const{
	return "ParseException: " + this->message;
}

