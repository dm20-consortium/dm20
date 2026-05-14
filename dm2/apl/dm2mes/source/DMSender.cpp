/*
 * DMSender.cpp
 *
 *  Created on: 2019/07/09
 *      Author: Nagoya University
 */

#include "DMAccess.h"
#include "DMSender.h"


DMSender::DMSender() {
}
DMSender::~DMSender() {
	// TODO Auto-generated destructor stub
}

bool DMSender::init(string ip, string userid, string password, bool isTcpMode, bool isSecureMode) {
	string _ip = DEFAULT_DM_IP;
	string _userid = DEFAULT_DM_USER;
	string _password = DEFAULT_DM_PASS;
	if (ip != "") _ip = ip;
	if (userid != "") _userid = userid;
	if (password != "") _password = password;
	int _port = 9002;
	if (isSecureMode) _port = 11002;
	try {
		//UDPオブジェクト取得
		sendSock = DmManager::getDatagramSocket(_ip, _port, _userid, _password, isTcpMode);
	} catch (ConnectionFailedException const&) {
		cerr << "DM2.0 の DBシステム(IS)を起動して下さい" << endl;
		cerr << "IP:" << _ip << ",UserID:" << _userid << endl;
		return false;
	} catch (...) {
		cerr << "[DM2.0 接続エラー]";
		cerr << "IP:" << _ip << ",UserID:" << _userid << endl;
		return false;
	}
	return true;
}
bool DMSender::doSend(string line, bool isNoSpace, int addTimestamp, bool &waitFlg, double times) {
	string s = line;
	bool doSend  = false;
    long timestamp_column = 0;

	if (isNoSpace) {
		string tmp = line;
		tmp.erase(std::remove_if(tmp.begin(), tmp.end(), ::isspace), tmp.end());
		if (tmp.empty()) {
			waitFlg = false;
			return false;
		}
	}
	if (addTimestamp != 0) {
		vector<string> col = split(s, ',');
		string timestamp_column_str;
		if (addTimestamp < 0) {
			auto ts_column = col.end();
			for (; addTimestamp != 0; addTimestamp++) {
				--ts_column;
			}
			timestamp_column_str = *ts_column;
		} else {
			auto ts_column = col.begin();
			addTimestamp--;
			for (; addTimestamp != 0; addTimestamp--) {
				++ts_column;
			}
			timestamp_column_str = *ts_column;
		}
		try {
			timestamp_column = std::atol(timestamp_column_str.c_str());
			if (timestamp_column == 0) {
				waitFlg = false;
				cerr << "Column Index " << addTimestamp << " is not Timestamp. Value:" << timestamp_column << endl;
				return false;
			}
		} catch (...) {
			waitFlg = false;
			cerr << "Column Index " << addTimestamp << "is not Timestamp. Value:" << timestamp_column << endl;
			return false;
		}
		if (base_file_ts == 0) {
			base_file_ts = timestamp_column;
			base_start_ts = DmUtil::getTimeMillisec();
			doSend = false;
			waitFlg = false;
		} else {
			long now_ts = DmUtil::getTimeMillisec();
			long dur = (timestamp_column - base_file_ts) - (now_ts - base_start_ts) * times;
			if (dur <= 0) {
				// 送信タイミング内
				if (pre_timestamp_column == timestamp_column) {
					doSend = false;
				} else {
					doSend = true;
					// 別のTS検出。一旦送信。呼出し側でpush_back
				}
				waitFlg = false;
				warnFlg = true;
			} else {
				if (dur >= 60000 && warnFlg) {
					int dur_sec = dur / 1000;
					cout << "[Warn]" << dur_sec << " second Until the Next Data comes" << endl;
					warnFlg = false;
				}
				// 送信タイミング外のデータが届いた
				// waitFlgはtrueのまま、呼出側で次のデータが来るまで待つ
				doSend = true;
				//cout << "Duration:" << dur << ",timestamp_column:" << timestamp_column << ",now_ts:" << now_ts << endl;
				//cout << "base_file_ts:" << base_file_ts << ",base_start_ts:" << base_start_ts << endl;
			}
		}
		pre_timestamp_column = timestamp_column;
	} else {
		doSend = true;
		waitFlg = false;
	}
	return doSend;
}

vector<string> DMSender::parse1DArray(const string& str) {
    vector<string> result;
    string cleanedStr = str.substr(2, str.size() - 2);  // 角括弧を取り除く
    vector<string> nums = split(cleanedStr, ',');
    for (const auto& num : nums) {
        result.push_back(num);
    }
    return result;
}
vector<vector<string>> DMSender::parse2DArray(const string& str) {
    vector<vector<string>> result;
    string cleanedStr = str.substr(2, str.size() - 4);  // 外側の角括弧を取り除く
    vector<string> arrays = split(cleanedStr, ']');
	bool first = true;
    for (auto& arr : arrays) {
		if (first) {
			arr = ",[" + arr;
			first = false;
		}
        if (arr.size() > 2) {  // 有効な配列かチェック
            result.push_back(parse1DArray(arr));
        }
    }
    return result;
}
bool DMSender::sendIs(string schema_name, vector<string> v_line, bool doCompress, int addTimestamp, int delay) {
	string _schema_name = "message_info";
	if (schema_name != "") {
		_schema_name = schema_name;
	}
    std::vector<Tuple> tuples;
	long ts = DmUtil::getTimeMillisec();
	if (schema_name == "") {
		Tuple tuple(1);
		for (string line : v_line) {
			tuple.setValue(0, line, ts);
			tuples.push_back(tuple);
		}
		usleep(1000 * delay);
		sendSock->sendStreamData(_schema_name, tuples, doCompress);
		tuples.clear();
	} else {
		for (string line : v_line) {
			vector<string> col = split(line, ',');
			vector<string> col2 = split2(line, ',');
			if (addTimestamp == -1) {
				col.pop_back();
				col2.pop_back();
				if (addTimestamp == -2) {
					col.pop_back();
					col2.pop_back();
				}
			}
			Tuple tuple(col2.size());
			int idx = 0;
			vector<string> vs;
			string prefix1 = "[";
			string suffix1 = "]";
			string prefix2 = "\"";
			string suffix2 = "\"";
			string combine = "";
			for(auto itr = col.begin(); itr != col.end(); ++itr) {
				if (combine != "" || itr->find(prefix1 + prefix1) == 0 || itr->find(prefix2 + prefix2) == 0) {
					tuple.resize(col.size());
					if (itr->find(prefix1 + prefix1) == 0 || itr->find(prefix2 + prefix2) == 0) {
						combine = *itr;
					} else {
						combine += "," + *itr;
					}
					if (itr->find(suffix1 + suffix1) != string::npos || itr->find(suffix2 + suffix2) != string::npos) {
						vector<vector<string>> v2s = parse2DArray(combine);
						tuple.setValue(idx, v2s, ts);
						idx++;
						combine = "";
					}
				} else if (vs.size() > 0 || itr->find(prefix1) == 0 || itr->find(prefix2) == 0) {
					// 可変長配列のケース
					// ex1) [1]の場合, splitメソッド処理後により, [1]のまま
					// ex2) [a,b]の場合, splitメソッド処理後により, [a と b] に分割
					// ex3) "a,b"の場合, splitメソッド処理後により, "a と b" に分割
					if (itr->find(prefix1) == 0 || itr->find(prefix2) == 0) {
						itr->replace(0, 1, "");
					}
					if (*itr != "") {
						if (itr->find(suffix1) == itr->size() - 1 || itr->find(suffix2) == itr->size() - 1) {
							// "]"を検出したので整形し, タプルにセット
							itr->replace(itr->size() - 1, 1, "");
							vs.push_back(*itr);
							if (itr->find(suffix1) == itr->size() - 1) {
								// case [a,b]
								tuple.setValue(idx, vs, ts);
							} else {
								// case "a,b"
								string s = "";
								for (auto itr2 = vs.begin(); itr2 != vs.end(); ++itr2) {
									if (s != "") s += ",";
									s += *itr2;
								}
								tuple.setValue(idx, s, ts);
							}
							idx++;
							vs.clear();
						} else {
							vs.push_back(*itr);
						}
					} else {
						// 継続して検出する。もし"]"が見つからない場合は、セットしない
						vs.push_back(*itr);
					}
				} else {
					// 可変長配列以外のケース
					tuple.setValue(idx, *itr, ts);
					idx++;
				}
			}
			if (tuple.size() != idx) tuple.resize(idx);
			tuples.push_back(tuple);
		}
		usleep(1000 * delay);
		sendSock->sendStreamData(_schema_name, tuples, doCompress);
		//std::thread t1(&DMSender::sendThread, this, _schema_name, tuples, doCompress);
		//t1.detach();
		tuples.clear();
	}
	return true;
}
void DMSender::sendThread(string _schema_name, std::vector<Tuple> _tuples, bool doCompress) {
	sendSock->sendStreamData(_schema_name, _tuples, doCompress);
}
// 可変長配列のケース
// ex1) [1]の場合, splitメソッド処理後により, [1]のまま
// ex2) [a,b]の場合, splitメソッド処理後により, [a と b] に分割
// ex3) "a,b"の場合, splitメソッド処理後により, "a と b" に分割
std::vector<std::string> DMSender::split2(std::string str, char del) {

	vector<string> strList = split(str, del);
	vector<string> retList;
	vector<string> vs;
	string prefix1 = "[";
	string suffix1 = "]";
	string prefix2 = "\"";
	string suffix2 = "\"";
	for (auto itr = strList.begin(); itr != strList.end() ; ++itr) {
		if (vs.size() > 0 || itr->find(prefix1) == 0 || itr->find(prefix2) == 0) {
			itr->replace(0, 1, "");
			if (*itr != "") {
				if (itr->find(suffix1) == itr->size() - 1 || itr->find(suffix2) == itr->size() - 1) {
					itr->replace(itr->size() - 1, 1, "");
					vs.push_back(*itr);
					string s = "";
					for (auto itr2 = vs.begin(); itr2 != vs.end(); ++itr2) {
						if (s != "") s += ",";
						s += *itr2;
					}
					retList.push_back(s);
					vs.clear();
				} else {
					vs.push_back(*itr);
				}
			} else {
				vs.push_back(*itr);
			}
		} else {
			// 可変長配列以外のケース
			retList.push_back(*itr);
		}
	}
	return retList;
}
std::vector<std::string> DMSender::split(std::string str, char del) {
    int first = 0;
    int last;
    auto check = str.find_first_of(del);
    std::vector<std::string> result;
    if (check == string::npos) {
    	result.push_back(str);
        return result;
    } else {
    	last = check;
    }

    while (first < (int)str.size()) {
        std::string subStr(str, first, last - first);
        result.push_back(subStr);
        first = last + 1;
        last = str.find_first_of(del, first);
        if (str.find_first_of(del, first) == std::string::npos) {
            last = str.size();
        }
    }
    return result;
}

