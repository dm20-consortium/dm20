#ifndef DM_EXCEPTION_H
#define DM_EXCEPTION_H

#include <exception> 

/**
 * DBシステム接続失敗時のException
 *
 * @author	Nagoya University
 * @date	2018/03/14
 */

class ConnectionFailedException : std::exception {
private:
	string message = "";
public:
	ConnectionFailedException() {};
	ConnectionFailedException(string msg) {
		this->message = msg;
	};
	string getMessage() const {
		return this->message;
	}
};

/**
 * DBシステムとの接続でタイムアウトが発生した際のException
 *
 * @author	Nagoya University
 * @date	2018/03/14
 */

class ConnectionTimeoutException : std::exception {
private:
	string message = "";
public:
	ConnectionTimeoutException() {};
	ConnectionTimeoutException(string msg) {
		this->message = msg;
	};
	string getMessage() const {
		return this->message;
	}
};

/**
 * クエリ構文エラーなどが発生した際のException
 *
 * @author	Nagoya University
 * @date	2018/03/14
 */

class SQLException : std::exception{
private:
	string message = "";
public:
	SQLException() {};
	SQLException(string msg) {
		this->message = msg;
	};
	string getMessage() const {
		return this->message;
	}
};

/**
 * 値のキャストに失敗した際のException
 *
 * @author	Nagoya University
 * @date	2018/03/14
 */

class CastException : std::exception {
private:
	string message = "";
public:
	CastException() {};
	CastException(string msg) {
		this->message = msg;
	};
	string getMessage() const {
		return this->message;
	}
};

/**
* 不正な引数を指定した際のException
*
* @author	Nagoya University
* @date	2018/11/02
*/

class InvalidArgumentException : std::exception {
private:
	string message = "";
public:
	InvalidArgumentException() {};
	InvalidArgumentException(string msg) {
		this->message = msg;
	};
	string getMessage() const {
		return this->message;
	}
};

/**
* 権限などによるException
*
* @author	Nagoya University
* @date	2019/07/18
*/

class AuthorityException : std::exception {
private:
	string message = "";
public:
	AuthorityException() {};
	AuthorityException(string msg) {
		this->message = msg;
	};
	string getMessage() const {
		return this->message;
	}
};

#endif  // DM_EXCEPTION_H