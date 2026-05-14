/*
 * DMReceiver.cpp
 *
 *  Created on: 2019/08/27
 *      Author: Nagoya University
 */

#include <iostream>
#include <fstream>
#include <functional>
#include <thread>

#include "DMAccess.h"
#include "DMReceiver.h"

template
<
    typename TYPE,
    std::size_t SIZE
>
std::size_t array_length(const TYPE (&)[SIZE])
{
    return SIZE;
}

DMReceiver::DMReceiver() {
	mngId = 0;
}

DMReceiver::~DMReceiver() {
}


string DMReceiver::makeQuery(string master_schema, string schema_name, string window, string columns, string paritition_keys, string where) {
	string outMsg = "";
	string _master_schema = "message_info";
	string _columns = "*";
	if (master_schema != "") _master_schema = master_schema;
	if (columns != "") _columns = columns;
	string _schema_name = "message_info";
	if (schema_name != "") {
		_schema_name = schema_name;
		if (master_schema == "") {
			_master_schema = _schema_name;
		}
	}
	string _window = "rows 1";
	if (window != "") _window = window;
	if (paritition_keys != "") _window = "PARTITION BY " + paritition_keys + " " + _window;
	string _where = "";
	if (where != "") _where = " WHERE " + where;
	string _query = "MASTER " + _master_schema + " SELECT " + _columns + " FROM " + _schema_name + " [" + _window + "]" + _where;
	return _query;
}

bool DMReceiver::continuousQuery(string master_schema, string schema_name, string query, string window, int addTimestamp, 
	string columns, string plus_schema_name, bool duplication, string paritition_keys, string where) {
	string outMsg = "";
	string _query;
	if (query != "") {
		_query = query;
	} else {
		_query = makeQuery(master_schema, schema_name, window, columns, paritition_keys, where);
		if (duplication) {
			_query = _query + " DUPLICATION";
		}
	}
	this->addTimestamp = addTimestamp;
	if (plus_schema_name == "") {
		mngId = this->continuousQueryExec(&_query,
				std::bind(&DMReceiver::callbackForStream, std::ref(*this), std::placeholders::_1), &outMsg);
	} else {
		_plus_schema_name = plus_schema_name;
		mngId = this->continuousQueryExec(&_query,
				std::bind(&DMReceiver::callbackForStreamPlusSendIs, std::ref(*this), std::placeholders::_1), &outMsg);
	}
	if (mngId == 0) {
		cerr << outMsg << endl;
		return false;
	}
	return true;
}
bool DMReceiver::destQuery(string master_schema, string schema_name, string query, string window, 
	unsigned long long destSID, unsigned long long execSID, string columns, string paritition_keys, string where) {
	string outMsg = "";
	string _query;
	if (query != "") {
		_query = query;
	} else {
		_query = makeQuery(master_schema, schema_name, window, columns, paritition_keys, where);
	}
	this->addTimestamp = addTimestamp;
	mngId = this->destQueryExec(&_query, destSID, execSID, &outMsg);
	cout << outMsg << endl;
	if (mngId == 0) return false;
	return true;
}
string DMReceiver::getValue(string input, int colSize) {
	if (colSize == 1) return input;
	if (input.find("[") == 0 && input.find("]") == input.size() - 1) return input;
	if (input.find(",") == input.npos) return input;
	return "\"" + input + "\"" ;
}
void DMReceiver::callbackForStream(ResultSet rs) {
	ResultSetMetaData rsmd = rs.getResultSetMetaData();
	int colSize = rsmd.getColumnSize();
	string s = "";
	/* 型チェック
	for (int i = 0; i < colSize; i++) {
		if (i > 0) s += ",";
		s += rsmd.getColumnType(i);
	}
	cout << s << endl;
	*/
	while (rs.next()) {
		s = "";
		for (int i = 0; i < colSize; i++) {
			if (i > 0) s += ",";
			string val = getValue(rs.getString(i), colSize);
			s += val;
			if (this->addTimestamp == -2) {
				if (i == rsmd.getColumnSize() - 1) {
					s += "," + to_string(rs.getEpochTime(0));
					s += "," + to_string(DmUtil::getTimeMillisec());
				}
			}
		}
		cout << s << endl;
	}
}

void DMReceiver::callbackForStreamPlusSendIs(ResultSet rs) {
	ResultSetMetaData rsmd = rs.getResultSetMetaData();
	int colSize = rsmd.getColumnSize();
	string s = "";
	while (rs.next()) {
		for (int i = 0; i < colSize; i++) {
			if (i > 0) s += ",";
			string val = getValue(rs.getString(i), colSize);
			s += val;
			if (this->addTimestamp == -2) {
				if (i == rsmd.getColumnSize() - 1) {
					s += "," + to_string(rs.getEpochTime(0));
					s += "," + to_string(DmUtil::getTimeMillisec());
				}
			}
		}
		s += "|";
	}
	thread createThread(&DMReceiver::SendIs, this, s);
	createThread.detach();
}
void DMReceiver::SendIs(string ord_s) {
	string s = ord_s;
	DMSender dms;
	int delay = 0;
	double times = 1.0;
	bool isNoSpace = false;
	string schema_name = _plus_schema_name;
	if (!dms.init(_ip, _userid, _password, _isTransportMode, _isSecureMode)) return;
	vector<string> v_input = dms.split(s, '|');
	vector<std::string> v_line;
	for (string line: v_input)
	{
		cout << line << endl;
		bool waitFlg = true;
		while (waitFlg) {
			// 入力があった場合はDMへの送信処理を行う。
			if (dms.doSend(line, isNoSpace, addTimestamp, waitFlg, times) == false) {
				if (addTimestamp != 0) {
					v_line.push_back(line);
				}
			} else {
				if ((int)v_line.size() > 0) {
					if (dms.sendIs(schema_name, v_line, false, addTimestamp, delay) == false) break;
					v_line.clear();
				}
				if (addTimestamp != 0 && waitFlg == false) {
					v_line.push_back(line);
				}
			}
		}
	}
	// 同期ありの場合、時間外のまま残るケースあり
	if (addTimestamp != 0 && (int)v_line.size() > 0) {
		bool waitFlg = true;
		while (waitFlg) {
			if (dms.doSend(v_line[0], isNoSpace, addTimestamp, waitFlg, times) == false) {
				if (dms.sendIs(schema_name, v_line, false, addTimestamp, delay) == false) break;
			}
		}
	}
}
// この関数は、コンティニュアスクエリのキャンセル要求を行っている箇所です。
// 基本的には編集不要です。
bool DMReceiver::cancelQueryForStream(void) {
	if (!this->cancelQuery(mngId)) {
		return false;
	}
	return true;
}

