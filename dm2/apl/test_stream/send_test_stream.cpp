#include <is/DmManager.h>

#include <iostream>
#include <string>
#include <time.h>
#include <thread>

using namespace std;

int main(int argc, char *argv[])
{
	int ch;
	bool add_vector = false;
	bool set_tuple = true;
	bool set_vector = true;
	while ((ch = getopt(argc, argv, "atv")) != -1) {
		switch (ch) {
		case 'a':
			add_vector = true;
			break;
		case 't':
			set_tuple = false;
			break;
		case 'v':
			set_vector = false;
			break;
		default:
			break;
		}
	}
	string ip = "127.0.0.1";
	int port = 9002;
	string dbUser = "dm2sampleuser";
	string dbPass = "dm2samplepassword";
	string stream_name ="test_stream";
	int tuple_col_size = 21;
    int sleepMillis = 1000;	// 送信間隔(ms)
	
	// UDP送信オブジェクトの取得
	DatagramSocket* sendSock = DmManager::getDatagramSocket(ip, port, dbUser, dbPass);
	
	vector<Tuple> tuples;
	Tuple tuple(tuple_col_size);
	long now = DmUtil::getTimeMillisec();
	// 値の設定
	int int_value = 1;
	long long_value = 2;
	double double_value = 3.1;
	std::string str_value = "abc";
	unsigned int uint_value = 4;
	unsigned long long ulong_value = 5;
	bool bool_value = true;

	vector<int> vector_int;
	vector<long> vector_long;
	vector<double> vector_double;
	vector<std::string> vector_string;
	vector<unsigned int> vector_uint;
	vector<unsigned long long> vector_ulong;
	vector<bool> vector_bool;
	
	vector<vector<int>> vector2_int;
	vector<vector<long>> vector2_long;
	vector<vector<double>> vector2_double;
	vector<vector<std::string>> vector2_string;
	vector<vector<unsigned int>> vector2_uint;
	vector<vector<unsigned long long>> vector2_ulong;
	vector<vector<bool>> vector2_bool;


	if (set_vector) {
		vector_int.push_back(int_value);
		vector_long.push_back(long_value);
		vector_double.push_back(double_value);
		vector_string.push_back(str_value);
		vector_uint.push_back(uint_value);
		vector_ulong.push_back(ulong_value);
		vector_bool.push_back(bool_value);
		if (add_vector) {
			vector_int.push_back(int_value + 1);
			vector_long.push_back(long_value + 1);
			vector_double.push_back(double_value + 1);
			vector_string.push_back(str_value + "efg");
			vector_uint.push_back(uint_value + 1);
			vector_ulong.push_back(ulong_value + 1);
			vector_bool.push_back(false);
		}
		int push_count = 1;
		if (add_vector) push_count = 2;
		for (int i = 0; i < push_count; i++) {
			vector2_int.push_back(vector_int);
			vector2_long.push_back(vector_long);
			vector2_double.push_back(vector_double);
			vector2_string.push_back(vector_string);
			vector2_uint.push_back(vector_uint);
			vector2_ulong.push_back(vector_ulong);
			vector2_bool.push_back(vector_bool);
		}
	}
	// タプルへの登録
	tuple.setValue(0, int_value, now);
	tuple.setValue(1, long_value, now);
	tuple.setValue(2, double_value, now);
	tuple.setValue(3, str_value, now);
	tuple.setValue(4, uint_value, now);
	tuple.setValue(5, ulong_value, now);
	tuple.setValue(6, bool_value, now);
	if (set_tuple) {
		tuple.setValue(7, vector_int, now);
		tuple.setValue(8, vector_long, now);
		tuple.setValue(9, vector_double, now);
		tuple.setValue(10, vector_string, now);
		tuple.setValue(11, vector_uint, now);
		tuple.setValue(12, vector_ulong, now);
		tuple.setValue(13, vector_bool, now);
		tuple.setValue(14, vector2_int, now);
		tuple.setValue(15, vector2_long, now);
		tuple.setValue(16, vector2_double, now);
		tuple.setValue(17, vector2_string, now);
		tuple.setValue(18, vector2_uint, now);
		tuple.setValue(19, vector2_ulong, now);
		tuple.setValue(20, vector2_bool, now);
	}

	tuples.push_back(tuple);
	// DBシステムにストリームデータを送信する
	sendSock->sendStreamData(stream_name, tuples);
	
	delete sendSock;

	return 0;
}
