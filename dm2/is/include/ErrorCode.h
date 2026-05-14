#pragma once

namespace IS {

	/** エラーコード定義 */
	enum ErrorCode
	{
		/** エラーなし */
		NO_ERR           =  0,
		/** クエリ構文エラー */
		QUERY_PARSE_ERR  =  1,
		/** 不正引数エラー */
		INVALID_ARG_ERR = 2,
		/** 権限エラー */
		AUTHORITY_ERR = 3,
		/** 不明なエラー */
		UNKNOWN_ERR      = 99,
	};
}
