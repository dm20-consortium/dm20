/*
 * DMAccess.cpp
 *
 *  Created on: 2019/08/27
 *      Author: Nagoya University
 */

#include "DMAccess.h"

const std::string QUERY_DIR_PATH = "../sql";

DMAccess::DMAccess() {
	// TODO Auto-generated constructor stub
}

DMAccess::~DMAccess() {
	// TODO Auto-generated destructor stub
}

bool DMAccess::DMConnect() {
	if (!DMConnect(DEFAULT_DM_IP, DEFAULT_DM_USER, DEFAULT_DM_PASS, false, false)) {
		return false;
	}
	return true;
}
bool DMAccess::DMConnect(string ip, string userid, string password, bool isTransportMode, bool isSecureMode) {
	if (ip != "") _ip = ip;
	if (userid != "") _userid = userid;
	if (password != "") _password = password;
	_isTransportMode = isTransportMode;
	_isSecureMode = isSecureMode;
	int _port = 9001;
	if (isSecureMode) _port = 11001;
	try {
		_DMcon = DmManager::getDBConnection(_ip, _port, _userid, _password, isTransportMode);
	} catch (ConnectionFailedException const&) {
		cerr << "DM2.0 の DBシステム(IS)を起動して下さい" << endl;
		cerr << "IP:" << _ip << ",UserID:" << _userid << endl;
		return false;
	} catch (...) {
		cerr << "[DM2.0 接続エラー] ";
		cerr << "IP:" << _ip << ",UserID:" << _userid << endl;
		return false;
	}
	return true;
}

void DMAccess::DMDisconnect(void) {
	_DMcon->disconnect();

}

void DMAccess::DMDisconnect(Connection *con) {
	con->disconnect();
}

void DMAccess::DMReconnect(void) {
	_DMcon->reconnect();
}
bool DMAccess::cancelQuery(int mngId) {
	try {
		_DMcon->cancelQuery(mngId);
	} catch (...) {
		return false;
	}
	return true;
}
string DMAccess::getFile(string fname) {
	string query_file_path(QUERY_DIR_PATH);
	query_file_path += "/";
	query_file_path += fname;
	ifstream ifs(query_file_path.c_str());
	if (ifs.fail()) {
		cerr << "Query File Read Error:" << fname << endl;
		return "";
	}
	istreambuf_iterator<char> it(ifs);
	istreambuf_iterator<char> last;
	string query_str(it, last);
	return query_str;
}
// ワンショットクエリ実行
bool DMAccess::oneShotQueryExec(string _queryStr, bool putHeader) {
	string _query = _queryStr;

	ResultSet rs;
	try {
		rs = _DMcon->execute(_query);
	} catch (ConnectionFailedException const&) {
		cerr << "[" << __func__ << "] Connection Failed." << endl;
		return false;
	} catch (SQLException const&) {
		cerr << "[" << __func__ << "] Query is wrong." << endl;
		return true;
	} catch (...) {
		cerr << "[" << __func__ << "] unexpected error." << endl;
		return false;
	}
	string mes = "";
	ResultSetMetaData rsmd = rs.getResultSetMetaData();
	long row_cnt = 0;
	while (rs.next()) {
		string mes = "";
		string name = "";
		string type = "";
		for (int i = 0; i < rsmd.getColumnSize(); i++) {
			if (i > 0) mes += ",";
			/* 以下、型に合わせてチェックするケース用のコード
			string setVal;
			string setType = rsmd.getColumnType(i);
			if (setType == "int8") {
				long numval = rs.getLong(i);
				setVal = to_string(numval);
			} else 
			if (setType == "int4") {
				int numval = rs.getInt(i);
				setVal = to_string(numval);
			} else 
			if (setType == "vector(double)") {
				vector<double> vdval = rs.getVectorDouble(i);
				setVal = "[";
				for (int i = 0; i < vdval.size(); i++) {
					setVal += to_string(vdval[i]) + ",";
				}
				setVal += "]";
			} else 
			if (setType == "vector(int4)") {
				vector<int> vdval = rs.getVectorInt(i);
				setVal = "[";
				for (int i = 0; i < vdval.size(); i++) {
					setVal += to_string(vdval[i]) + ",";
				}
				setVal += "]";
			} else 
			if (setType == "vector(int8)") {
				// 1次元配列検証用。現状、setTypeだけでは次元数まで判定できない。
				vector<vector<long>> sss = rs.get2VectorLong(i);;
				for (int i = 0; i < sss.size(); i++) {
					for (int j = 0; j < sss.at(i).size(); j++) {
						cout << sss.at(i).at(j) << endl;
					}
				}
				vector<long> vdval = rs.getVectorLong(i);
				setVal = "[";
				for (int i = 0; i < vdval.size(); i++) {
					setVal += to_string(vdval[i]) + ",";
				}
				setVal += "]";
			} else 
			if (setType == "varchar" || setType == "geography" || setType == "geometry") {
				string sval = rs.getString(i);
				setVal = sval;
			} else 
			{
				cout << "unknown column name:" << setType << endl;
			}
			mes += setVal;
			*/
			mes += rs.getString(i);
			if (row_cnt == 0) {
				if (i > 0) {
					name += ",";
					type += ",";
				}
				name += rsmd.getColumnName(i);
				type += rsmd.getColumnType(i);
			}
		}
		if (putHeader && row_cnt == 0) {
			cout << name << endl;
			cout << type << endl;
			cout << std::string(name.length(), '=') << endl;
		}
		cout << mes << endl;
		row_cnt++;
	}
	rs.close();
	
	transform(_query.begin(), _query.end(), _query.begin(), [](unsigned char c) { return std::tolower(c); });
	if (_query.find("select ") == string::npos) {
		if (mes == "") cout << "[" << __func__ << "] Query is successfully accepted." << endl;
	} else {
		if (putHeader) cout << "(" << row_cnt << " row)" << endl;
	}
	return true;
}
// コンティニュアスクエリ実行
unsigned int DMAccess::continuousQueryExec(string *queryStr, std::function<void (ResultSet)> fc, string *outMsg) {
	unsigned int mngId = 0;
	try {
		mngId = _DMcon->registerQuery(*queryStr, fc);
		*outMsg = "Continuous Query is accepted successfully. mngId: " + to_string(mngId);
	} catch (ConnectionFailedException const&) {
		cerr << "[" << __func__ << "] Connection Failed." << endl;
		*outMsg = "Connection For Continuous Query Failed.";
		return 0;
	} catch (ConnectionTimeoutException const&) {
		cerr << "[" << __func__ << "] Connection Timeout." << endl;
		*outMsg = "Continuous Query request is Timeout. Sorry. Please Retry.";
		return 0;
	} catch (SQLException const&e) {
		cerr << "[" << __func__ << "] SQL is wrong." << endl;
		*outMsg = "Continuous Query SQL Error:" + e.getMessage();
		return 0;
	} catch (...) {
		cerr << "[" << __func__ << "] unexpected error." << endl;
		*outMsg = "Continuous Query Unexpected Error.";
		return 0;
	}
	return mngId;
}
// プリセットクエリ実行
unsigned int DMAccess::destQueryExec(string *queryStr, unsigned long long destSID, unsigned long long execSID, string *outMsg) {
	unsigned int mngId = 0;
	try {
		mngId = _DMcon->registerQuery(*queryStr, destSID, execSID);
		string mngMsg = "";
		if (mngId != 0) mngMsg = "mngId: " + to_string(mngId);
		*outMsg = "Destination Query is accepted successfully. " + mngMsg;
	} catch (ConnectionFailedException const&) {
		cerr << "[" << __func__ << "] Connection Failed." << endl;
		*outMsg = "Destination Query is Failed.";
		return 0;
	} catch (ConnectionTimeoutException const&) {
		cerr << "[" << __func__ << "] Connection Timeout." << endl;
		*outMsg = "Destination Query request is Timeout. Sorry. Please Retry.";
		return 0;
	} catch (SQLException const&e) {
		cerr << "[" << __func__ << "] SQL is wrong." << endl;
		*outMsg = "Destination Query SQL Error:" + e.getMessage();
		return 0;
	} catch (...) {
		cerr << "[" << __func__ << "] unexpected error." << endl;
		*outMsg = "Destination Query Unexpected Error.";
		return 0;
	}
	return mngId;
}
bool DMAccess::cancelQuery(unsigned int mngId) {
	try {
		_DMcon->cancelQuery(mngId);
	} catch (ConnectionFailedException const&) {
		cerr << "[" << __func__ << "] Connection Failed." << endl;
		return false;
	} catch (InvalidArgumentException const&e) {
		cerr << "[" << __func__ << "] Invalid Argument. " + e.getMessage() << endl;
	} catch (...) {
		cerr << "[" << __func__ << "] unexpected error." << endl;
		return false;
	}
	return true;
}


int DMAccess::getColumnNumber(ResultSetMetaData *p_rsmd, string argCname, string argCtype) {
	int column_number = -1;
	for (int i = 0; i < p_rsmd->getColumnSize(); i++) {
		string cname = p_rsmd->getColumnName(i);
		string cname2 = cname;
		if (cname.find(".") >= 0) {
			cname2 = cname.substr(cname.find(".") + 1);
		}
		if (cname2 == argCname) {
			if (argCtype == "" || p_rsmd->getColumnType(i) == argCtype) {
				column_number = i;
				break;
			}
		}
	}
	return column_number;
}
