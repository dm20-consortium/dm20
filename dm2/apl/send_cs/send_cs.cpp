#include <is/ProtobufParser.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdlib>     // strtoull(), atoi()
#include <getopt.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>     // close()
#include <cstring>      // memset()
#include <cerrno>       // errno
#include <iostream>     // std::cerr
#include <random>
#include <atomic>

#define MSGSIZE 64000
#define UNCOMPRESSED_BUF_SIZE 65000

using namespace std;
using namespace IS;

StringUtil stringUtil;

void usage(char *cmd)
{
    cout << "Usage: " << cmd << " [options]\n"
            << "  -i <ip>          Destination IP (default: 127.0.0.1)\n"
            << "  -p <port>        Destination port (default: 55555)\n"
            << "  -s <sid>         Destination SID (default: 1901000001)\n"
            << "  -o <object_id>   Start Object ID (default: 100000000)\n"
            << "  -n <number>      Number of tuples (default: 1)\n"
            << "  -c <0|1|2>       Compression flag (default: 0)\n"
            << "  -l <limit>       Limit on the number of transmissions\n"
            << "  -I <Interval>    Transmission Interval (millisecond)\n"
            << "  -h               Show this help\n";
}
struct send_message{
    /** @brief	送信元SID */
    unsigned long long src_station_id;
    /** @brief	宛先SID */
    unsigned long long dst_station_id;
    /** @brief	送信元type */
    short src_station_type;
    /** @brief	宛先type */
    short dst_station_type;
    /** @brief	ペイロードサイズ */
    unsigned short payload_size;
    /** @brief	転送フラグ */
    short transmission_flag;
    /** @brief	重複チェックID */
    unsigned long long duplication_check_id;
    /** @brief	レーンID */
    unsigned long long lane_id;
    /** @brief	再送レベル */
    short retry_level;
    /** @brief	再送データID */
    unsigned long long retry_data_id;
    /** @brief	再送時間(ms) */
    int retry_lifetime;
    /** @brief	メッセージ種別 */
    short msg_type;
    /** @brief	CS詳細メッセージ  */
    short cs_message_detail;
    /** @brief	ファイルディスクリプタ名称 (署名格納用としても使用される) */
    char fd_name[72];
    /** @brief	署名サイズ */
    short sign_size;
    /** @brief	フラグメントシーケンス番号 */
    unsigned long long flagment_duplication_check_id;
    /** @brief	フラグメントトータル */
    int flagment_sum;
    /** @brief	フラグメントオフセット */
    int flagment_offset;
    /** @brief 優先度フラグ */
    int priority_level;
    /** @brief	DM2.0データペイロード */
    char dm2_payload[MSGSIZE];
};
unsigned long long createRandomId()
{
    static std::random_device rd;
    static std::mt19937_64 engine(rd());
    static std::uniform_int_distribution<unsigned long long> dist;

    return dist(engine);
}
string getTupleHash(const Tuple &tuple)
{
    string tuplesStr = "";
    for (int j = 0; j < tuple.size(); j++) {
        any val;
        tuple.getValueByIdx(j, val);
        tuplesStr += stringUtil.getAnyString(val);
        if (j == 0) {
            long ts;
            tuple.getTimestampByIdx(j, ts);
            tuplesStr += to_string(ts);
        }
    }
    std::hash<std::string> hasher;
    size_t hash1 = hasher(tuplesStr);
    return to_string(hash1);
}
int main(int argc, char *argv[])
{   
    // 設定値 -----------------------------  
    unsigned long long dummy_object_id = 100000000; // ダミー物標IDの開始番号。タプル数を増やすとインクリメントされる
    unsigned long long dest_sid = 1901000001;       // 宛先STATION_ID
    string ip = "127.0.0.1";                        // 宛先IP
    int dst_port = 55555;                           // 宛先ポート番号
    int tuples_count = 1;                           // タプル数
    char compressFlg = '0';                         // 圧縮フラグ
    int send_count = 1;                             // 送信回数
    int send_interval = 1000;                       // 送信間隔（ミリ秒）
    // -----------------------------
    //----------------------------------
    // 引数処理
    //----------------------------------
	int ch;
    while ((ch = getopt(argc, argv, "i:p:s:o:n:c:l:I:h")) != -1) {
        switch (ch) {
        case 'i':
            ip = optarg;
            break;
        case 'p':
            dst_port = atoi(optarg);
            break;
        case 'l':
            send_count = atoi(optarg);
            break;
        case 'I':
            send_interval = atoi(optarg);
            break;
        case 's':
            dest_sid = strtoull(optarg, nullptr, 10);
            break;
        case 'o':
            dummy_object_id = strtoull(optarg, nullptr, 10);
            break;
        case 'n':
            tuples_count = atoi(optarg);
            break;
        case 'c':
            if (optarg[0] == '0' ||
                optarg[0] == '1' ||
                optarg[0] == '2') {
                compressFlg = optarg[0];
            } else {
                cerr << "Invalid compress flag: " << optarg << endl;
                return -1;
            }
            break;
        case 'h':
        default:
            usage(argv[0]);
            return 0;
        }
    }

    //----------------------------------
    // ソケット作成
    //----------------------------------
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(dst_port);
    inet_pton(AF_INET,
            ip.c_str(),
            &addr.sin_addr);
    
    for (int send_idx = 0; send_idx < send_count; send_idx++) {
        auto start = chrono::steady_clock::now();
        if (send_idx != 0) usleep(send_interval * 1000);

        vector<Tuple> tuples;
        
        string tablename = "object_info_0_8_1";
        int user_table_column_count = 88;
        int is_management_column_count = 4;
        int all_column_count = user_table_column_count + is_management_column_count;
        

        long now = DmUtil::getTimeMillisec();
        unsigned long long unique_id = dummy_object_id;
        for (int tuple_idx = 0; tuple_idx < tuples_count; tuple_idx++) {
            Tuple tuple(user_table_column_count);
            unique_id++;
            //----------------------------------
            // ユーザ情報登録
            //----------------------------------
            for(int i = 0; i < user_table_column_count; i++)
            {
                if (i == 0) {
                    //unsigned long long unique_id = createRandomId();
                    tuple.setValue(i, unique_id, now);
                } else if (i == 38 || i == 82) {
                    tuple.setValue(i, (unsigned long long)unique_id, now);
                } else if (i == 1) {
                    tuple.setValue(i, now, now);
                } else if (i == 87) {
                    unsigned long long ull = 1;
                    vector <unsigned long long> vull;
                    vull.push_back(ull);
                    tuple.setValue(i, vull, now);
                } else {
                    tuple.setValue(i, i, now);
                }
            }
            //----------------------------------
            // IS管理者情報登録
            //----------------------------------
            tuple.resize(all_column_count);
            string tupleHash = getTupleHash(tuple);
            vector<string> vs;
            vs.push_back(to_string(now));
            tuple.setValue(user_table_column_count, string("dm2sampleuser"), 0, false);      // ユーザ名
            tuple.setValue(user_table_column_count + 1, string("0.0.0.0"), 0, false);        // IPアドレス
            tuple.setValue(user_table_column_count + 2, tupleHash, 0, false);   // ハッシュ値（重複制御のため、重要）
            tuple.setValue(user_table_column_count + 3, string(""), 0, false);               // 性能情報
            
            tuples.push_back(tuple);
        }
        

        //----------------------------------
        // protobuf化
        //----------------------------------

        ProtobufParser &parser = IS::ProtobufParser::get_instance();

        string protobufPayload =
            parser.objectInfoSerializeToString_0_8_1(tuples);

        //----------------------------------
        // protobufヘッダ付与
        //----------------------------------
        string length_s = to_string(tablename.length());
        protobufPayload = "3" + length_s + tablename + protobufPayload;
        //----------------------------------
        // 圧縮フラグ指定時は、圧縮し、圧縮要ヘッダ追加
        //----------------------------------
        string sendPayload;
        if (compressFlg == '1' || compressFlg == '2') {
            char outbuf[UNCOMPRESSED_BUF_SIZE];
            long key = DmUtil::getTimeMicrosec();
            int sendSize =
                stringUtil.setCompressedBufWithHeader(
                    protobufPayload,
                    outbuf,
                    compressFlg,
                    key);

            sendPayload.assign(outbuf, sendSize);
        }
        else {
            //sendPayload.push_back('0');
            //sendPayload += protobufPayload;
            // 圧縮フラグ0セット時に、IS側でバグが存在したため、そのままセット（最新版では修正したが、未反映の環境があるため）
            sendPayload = protobufPayload;
        }

        //----------------------------------
        // CS電文作成
        //----------------------------------

        struct send_message buf;
        buf.src_station_id = 1;
        buf.dst_station_id = dest_sid;
        buf.lane_id = 0;
        buf.flagment_offset = 0;
        buf.flagment_sum = 1;
        buf.transmission_flag = 0;
        buf.msg_type = 2;   // DM2TYPE_IS
        struct timespec ts;
        timespec_get(&ts, TIME_UTC);
        buf.duplication_check_id = ts.tv_sec * 1000000000 + ts.tv_nsec;

        buf.payload_size = sendPayload.size();
        //cout << sendPayload.size() << endl;
        memcpy(buf.dm2_payload,
            sendPayload.data(),
            sendPayload.size());
        
        //----------------------------------
        // CS送信
        //----------------------------------
        int send_size = sizeof(send_message) - MSGSIZE + buf.payload_size;
        sendto(sock, &buf, send_size, 0, (sockaddr*)&addr, sizeof(addr));
        
        auto end = chrono::steady_clock::now();
        cout << "send_no: " << send_idx + 1 << ", " << chrono::duration_cast<chrono::milliseconds>(end-start).count() << " ms, size: " << send_size << endl;
    }

}