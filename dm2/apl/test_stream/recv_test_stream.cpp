#include <is/DmManager.h>

#include <iostream>
#include <string>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

using namespace std;

volatile sig_atomic_t eflag = 0;
int rowsNum = 0;
// シグナルハンドラ
void handler(int signum) {
  eflag = 1;
}
template <typename T>
string vectorToString(const vector<T>& v)
{
    string s = "[";
    for (size_t i = 0; i < v.size(); i++) {
        if (i > 0) s += ",";
        s += to_string(v[i]);
    }
    s += "]";
    return s;
}

// stringだけ特殊化
template <>
string vectorToString<string>(const vector<string>& v)
{
    string s = "[";
    for (size_t i = 0; i < v.size(); i++) {
        if (i > 0) s += ",";
        s += v[i];
    }
    s += "]";
    return s;
}
template <typename T>
string vector2ToString(const vector<vector<T>>& v)
{
    string s = "[";
    for (size_t i = 0; i < v.size(); i++) {
        if (i > 0) s += ",";
        s += vectorToString(v[i]);
    }
    s += "]";
    return s;
}using Getter = function<string(ResultSet&, int)>;

// getValue用のif分岐をmap化。型が増えた場合は、追記すること
static const unordered_map<string, Getter> getters = {
    {"int32",  [](ResultSet& rs, int i){ return to_string(rs.getInt(i)); }},
    {"int64",  [](ResultSet& rs, int i){ return to_string(rs.getLong(i)); }},
    {"double", [](ResultSet& rs, int i){ return to_string(rs.getDouble(i)); }},
    {"string", [](ResultSet& rs, int i){ return rs.getString(i); }},
    {"uint32", [](ResultSet& rs, int i){ return to_string(rs.getUnsignedInt(i)); }},
    {"uint64", [](ResultSet& rs, int i){ return to_string(rs.getUnsignedLongLong(i)); }},
    {"bool",   [](ResultSet& rs, int i){ return to_string(rs.getBool(i)); }},
    {"vector(int32)", [](ResultSet& rs, int i){
        return vectorToString(rs.getVectorInt(i));
    }},
    {"vector(int64)", [](ResultSet& rs, int i){
        return vectorToString(rs.getVectorLong(i));
    }},
    {"vector(double)", [](ResultSet& rs, int i){
        return vectorToString(rs.getVectorDouble(i));
    }},
    {"vector(string)", [](ResultSet& rs, int i){
        return vectorToString(rs.getVectorString(i));
    }},
    {"vector(uint32)", [](ResultSet& rs, int i){
        return vectorToString(rs.getVectorUnsignedInt(i));
    }},
    {"vector(uint64)", [](ResultSet& rs, int i){
        return vectorToString(rs.getVectorUnsignedLongLong(i));
    }},
    {"vector(bool)", [](ResultSet& rs, int i){
        return vectorToString(rs.getVectorBool(i));
    }},
    {"vector(vector(int32))", [](ResultSet& rs, int i){
        return vector2ToString(rs.get2VectorInt(i));
    }},
    {"vector(vector(int64))", [](ResultSet& rs, int i){
        return vector2ToString(rs.get2VectorLong(i));
    }},
    {"vector(vector(double))", [](ResultSet& rs, int i){
        return vector2ToString(rs.get2VectorDouble(i));
    }},
    {"vector(vector(string))", [](ResultSet& rs, int i){
        return vector2ToString(rs.get2VectorString(i));
    }},
    {"vector(vector(uint32))", [](ResultSet& rs, int i){
        return vector2ToString(rs.get2VectorUnsignedInt(i));
    }},
    {"vector(vector(uint64))", [](ResultSet& rs, int i){
        return vector2ToString(rs.get2VectorUnsignedLongLong(i));
    }},
    {"vector(vector(bool))", [](ResultSet& rs, int i){
        return vector2ToString(rs.get2VectorBool(i));
    }}
};
string getValue(ResultSet& rs, int index, const string& typeName)
{
    auto it = getters.find(typeName);
    if (it == getters.end())
        return "";

    return it->second(rs, index);
}
class CallBackListenerClass : public CallBackListener {
public:
	// インターフェースクラスと同様の関数名で定義する
	void callBack(ResultSet rs) {
		ResultSetMetaData rsmd = rs.getResultSetMetaData();
		int colNum = rsmd.getColumnSize();
		while (rs.next()) {
			string line = "";
			//受信したデータを表示
			for (int i = 0; i < colNum; i++) {
				if (i != 0) line += ",";
				line += getValue(rs, i, rsmd.getColumnType(i));
			}
			cout << line << endl;
		}
	}
};

int main(int argc, const char* argv[])
{
	if (signal(SIGINT, handler) == SIG_ERR) {
		cerr << "Signal Error" << endl;
	        return -1;
	}
	// MAIN 処理
	string ip = "127.0.0.1";
	int port = 9001;
	string dbUser = "dm2sampleuser";
	string dbPass = "dm2samplepassword";
	string query = "MASTER test_stream SELECT * FROM test_stream [rows 1]";
	// サーバに接続
	Connection* con = DmManager::getDBConnection(ip, port, dbUser, dbPass);

	unsigned int mngId = 0;
	try {
		CallBackListenerClass* cblc = new CallBackListenerClass();
		mngId = con->registerQuery(query, cblc);
	} catch (SQLException &e) {
		cout << "ERROR catch:" << e.getMessage() << endl;
	}
	
	while (true) {
		sleep(1);
		if (eflag == 1) break;
	}
	con->cancelQuery(mngId);
	con->disconnect();
}
