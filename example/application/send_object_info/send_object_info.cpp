#include <is/DmManager.h>

#include <iostream>
#include <string>
#include <time.h>
#include <thread>

using namespace std;

int main(int argc, const char* argv[])
{
    string ip = "127.0.0.1";
    int port = 9002;
    string dbUser = "dm2sampleuser";
    string dbPass = "dm2samplepassword";
    string stream_name ="object_info_0_8_1";

    // Tuple の列数
    int tuple_col_size = 88;

    // 送信間隔(ms)
    int sleepMillis = 1000;

    // UDP送信オブジェクトの取得
    DatagramSocket* sendSock =
        DmManager::getDatagramSocket(
            ip,
            port,
            dbUser,
            dbPass);

    int value = 0;

    while (true)
    {
        value++;

        vector<Tuple> tuples;

        Tuple tuple(tuple_col_size);

        long now = DmUtil::getTimeMillisec();

        // 全列へ値を設定
        for (int i = 0; i < tuple_col_size; i++)
        {
            tuple.setValue(
                i,
                to_string(value),
                now);
        }

        tuples.push_back(tuple);

        // ストリームデータ送信
        sendSock->sendStreamData(
            stream_name,
            tuples);

        // 周期待ち
        this_thread::sleep_for(
            chrono::milliseconds(sleepMillis));
    }

    delete sendSock;

    return 0;
}