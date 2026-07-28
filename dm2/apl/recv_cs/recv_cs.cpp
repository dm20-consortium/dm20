#include <iostream>
#include <vector>
#include <string>
#include <any>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include <is/ProtobufParser.h>

using namespace std;
using namespace IS;

#define MSGSIZE 64000

struct send_message
{
    unsigned long long src_station_id;
    unsigned long long dst_station_id;
    short src_station_type;
    short dst_station_type;
    unsigned short payload_size;
    short transmission_flag;
    unsigned long long duplication_check_id;
    unsigned long long lane_id;
    short retry_level;
    unsigned long long retry_data_id;
    int retry_lifetime;
    short msg_type;
    short cs_message_detail;
    char fd_name[72];
    short sign_size;
    unsigned long long flagment_duplication_check_id;
    int flagment_sum;
    int flagment_offset;
    int priority_level;
    char dm2_payload[MSGSIZE];
};

StringUtil stringUtil;

int main(int argc, char *argv[])
{
    int dst_port = 55555;                           // 宛先ポート番号
    int tuples_count = 1;                           // タプル数
    char compressFlg = '0';                         // 圧縮フラグ
    // -----------------------------
    //----------------------------------
    // 引数処理
    //----------------------------------
	int ch;
    while ((ch = getopt(argc, argv, "p:h")) != -1) {
        switch (ch) {
        case 'p':
            dst_port = atoi(optarg);
            break;
        case 'h':
        default:
            cout << "Usage: " << argv[0] << " [options]\n"
                    << "  -p <port>        Destination port (default: 55555)\n";
            return 0;
        }
    }
    //------------------------------------
    // UDP受信準備
    //------------------------------------
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(dst_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        return -1;
    }

    cout << "waiting... port: " << dst_port << endl;

    while (true)
    {
        send_message buf{};

        sockaddr_in from{};
        socklen_t fromlen = sizeof(from);

        int recvSize =
            recvfrom(sock,&buf, sizeof(buf), 0, (sockaddr*)&from, &fromlen);

        if (recvSize <= 0) continue;

        cout << endl;
        cout << "====== [send_message] =============" << endl;
        cout << "recv bytes : " << recvSize << endl;
        cout << "payload    : " << buf.payload_size << endl;
        cout << "src_sid    : " << buf.src_station_id << endl;
        cout << "dst_sid    : " << buf.dst_station_id << endl;
        cout << "flagment_sum :" << buf.flagment_sum << endl;
        cout << "==================================" << endl;
        if (buf.flagment_sum > 1) {
            cerr << "[ERROR] Integration-function not supported." << buf.flagment_sum << endl;
            continue;
        }
        //------------------------------------
        // payload取得
        //------------------------------------

        string payload = string(buf.dm2_payload, MSGSIZE);
        string tableName = "";
        //------------------------------------
        // protobufヘッダ解析
        //------------------------------------
		char *payload_p = &payload[0];
		struct IsHeaderInfo headerInfo;
		stringUtil.getIsHeader(payload_p, headerInfo);
		int len = 0;
		cout << "1st Flg (about Compress):" << headerInfo.header.compressFlg << endl;
		if (headerInfo.header.compressFlg == '1' || headerInfo.header.compressFlg == '2') {
			char bufTmp[650000];
			memset(bufTmp, 0, sizeof(bufTmp));
			if (headerInfo.header.compressFlg == '1') {
				len = stringUtil.decompress(headerInfo.payload_p, bufTmp);
			} else {
				int outSize;
				if (stringUtil.decompressUsingZstd(headerInfo.payload_p, bufTmp, headerInfo.header.length, &outSize)) {
					len = outSize;
				}
			}
			cout << "Length:" << len << endl;
			if (len > 0) {
				payload = string(bufTmp, len);
				char *payload_p = &payload[0];
				// 解凍されたバッファから再度ヘッダ情報を読み取る
				stringUtil.getIsHeader(payload_p, headerInfo);
			}
		} else if (headerInfo.header.compressFlg == '0') {
			payload = string(headerInfo.payload_p, payload.length() - headerInfo.headerSize);
			len = payload.length();
			char *payload_p = &payload[0];
			// 再度ヘッダ情報を読み取る
			stringUtil.getIsHeader(payload_p, headerInfo);
		}
		cout << "2nd Flg (about Protobuf):" << headerInfo.header.compressFlg << endl;
		if (headerInfo.header.compressFlg == '3') {
			tableName = headerInfo.header.schema_name;
			payload = string(headerInfo.payload_p, payload.length() - headerInfo.headerSize);
			len = payload.length();
		} else {
			len = payload.length();
		}
        cout << "table : " << tableName << endl;
		cout << "payload Length:" << len << endl;

        if (tableName != "object_info_0_8_1") {
            cerr << "[ERROR] This table is not supported." << tableName << endl;
            continue;
        }
        //------------------------------------
        // Deserialize
        //------------------------------------

        vector<Tuple> tuples;
        Schema schema;

        ProtobufParser &parser =
            ProtobufParser::get_instance();
        parser.objectInfoDeserializeToTuple_0_8_1(payload, tuples, schema);
        cout << "tuple count : " << tuples.size() << endl;

        cout << "-------- tuple --------" << endl;
        //------------------------------------
        // dump
        //------------------------------------
        for (size_t t = 0; t < tuples.size(); t++)
        {
            string s = "[Tuple No." + to_string(t + 1) + "]";
            Tuple &tuple = tuples[t];
            for (int i = 0; i < tuple.size(); i++)
            {
			    if (i > 0) s += ",";
                any val;
                tuple.getValueByIdx(i, val);
                s+= stringUtil.getAnyString(val);
            }
            cout << s << endl;
        }
    }
    close(sock);

    return 0;
}