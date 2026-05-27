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
				line += rs.getString(i);
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
	string query = "MASTER object_info_0_8_1 SELECT * FROM object_info_0_8_1 [rows 1]";
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
