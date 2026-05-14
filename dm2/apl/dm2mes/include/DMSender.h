#ifndef __ISSENDDATA_H__
#define __ISSENDDATA_H__

#include <is/DmManager.h> //IS利用ヘッダ
//#include <mutex>


// Hint: コンティニュアスクエリの数を増やす場合は、インデックス用のDefine文を追加してください。
#define IDX_SEND_TEST_STREAM 0

// Hint: コンティニュアスクエリの数に応じて、編集してください。
#define IDX_SEND_SIZE 1

class DMSender{
private:
	//std::mutex muTexSendThread;
	std::vector<std::vector<Tuple>> tuplesVector;
public:
    DatagramSocket* sendSock;
    long base_file_ts = 0;
    long base_start_ts = 0;
    long pre_timestamp_column = 0;
    bool warnFlg = true;
    DMSender();
    bool init(string ip, string userid, string password, bool isTcpMode, bool isSecureMode);
    virtual ~DMSender();
    bool doSend(string line, bool isNoSpace, int addTimestamp, bool &waitFlg, double times);
    bool sendIs(string schema_name, vector<string> msg, bool doCompress, int addTimestamp, int delay);
    void sendThread(string _schema_name, std::vector<Tuple> tuples, bool doCompress);
    vector<string> parse1DArray(const string& str);
    vector<vector<string>> parse2DArray(const string& str);
    std::vector<std::string> split2(std::string str, char del);
    std::vector<std::string> split(std::string str, char del);
};
#endif
