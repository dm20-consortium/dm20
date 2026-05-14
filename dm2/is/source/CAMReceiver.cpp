#include "CAMReceiver.h"

#include <iostream>
#include <thread>

using namespace std;

namespace IS {

    /**
     * コンストラクタ
     *
     * @author	Nagoya University
     * @date	2018/03/13
     *
     * @param	data	The data.
     */

    CAMdata::CAMdata(char data[])
    {
        protocolVersion = getUINT(&data[MD_Head_PROTOCOL_VERSION]);
        messageID = getUINT(&data[MD_Head_MESSAGE_ID]);
        stationID = getUINT(&data[MD_Head_STATION_ID]);
        time = getULONG(&data[MD_Head_GENERATION_TIME]);
        stationType = getUINT(&data[MD_Head_STATION]);
        lat = getINT(&data[MD_Head_LATITUDE]);
        lon = getINT(&data[MD_Head_LONGITUDE]);
        alt = getINT(&data[MD_Head_ALTITUDE]);
        heading = getINT(&data[MD_Head_HEADING]);
        speed = getINT(&data[MD_Head_SPEED]);
        direction = getINT(&data[MD_Head_DRIVING_DIRECTION]);
    }

    /**
     * unsigned int を取得する
     *
     * @author	Nagoya University
     * @date	2018/03/13
     *
     * @param [in,out]	data	文字列
     *
     * @return	数値(unsigned int)
     */

    unsigned int CAMdata::getUINT(char* data)
    {
        // data[] is little endian.
        unsigned int val;
        val = data[0] + (data[1] << 8) + (data[2] << 16) + (data[3] << 24);
        return val;
    }

    /**
     * unsigned long long を取得する
     *
     * @author	Nagoya University
     * @date	2018/03/13
     *
     * @param [in,out]	data	文字列
     *
     * @return	数値(unsigned long long)
     */

    unsigned long long CAMdata::getULONG(char* data)
    {
        // data[] is little endian.
        unsigned long long val;
        val = data[0] + (data[1] << 8) + (data[2] << 16) + (data[3] << 24) 
                + (data[4] << 32) + (data[5] << 40) + (data[6] << 48) + (data[7] << 56);
        return val;
    }

    /**
     * int を取得する
     *
     * @author	Nagoya University
     * @date	2018/03/13
     *
     * @param [in,out]	data	文字列
     *
     * @return	数値(int)
     */

    int CAMdata::getINT(char* data)
    {
        // data[] is little endian.
        int val;
        val = data[0] + (data[1] << 8) + (data[2] << 16) + (data[3] << 24);
        return val;
    }

    /**
     * データを出力する
     *
     * @author	Nagoya University
     * @date	2018/03/13
     */

    void CAMdata::dump()
    {
		logger->debug("[CAM]--- dump ---");
		logger->debug("[CAM]protocolVersion =" + std::to_string(protocolVersion));
		logger->debug("[CAM]messageID       =" + std::to_string(messageID));
		logger->debug("[CAM]stationID       =" + std::to_string(stationID));
		logger->debug("[CAM]time            =" + std::to_string(time));
		logger->debug("[CAM]stationType     =" + std::to_string(stationType));
		logger->debug("[CAM]lat             =" + std::to_string(lat));
		logger->debug("[CAM]lon             =" + std::to_string(lon));
		logger->debug("[CAM]alt             =" + std::to_string(alt));
		logger->debug("[CAM]heading         =" + std::to_string(heading));
		logger->debug("[CAM]speed           =" + std::to_string(speed));
		logger->debug("[CAM]direction       =" + std::to_string(direction));
		logger->debug("[CAM]------------");
    }

    /**
     * コンストラクタ
     *
     * @author	Nagoya University
     * @date	2018/03/13
     *
     * @param	name	名称
     * @param	port	ポート番号
     */

    CAMReceiver::CAMReceiver(string name, int port)
    : NetworkSource(name, port)
    {
    }

    /**
     * デストラクタ
     *
     * @author	Nagoya University
     * @date	2018/03/13
     */

    CAMReceiver::~CAMReceiver()
    {
    }

    /**
     * 初期化処理
     *
     * @author	Nagoya University
     * @date	2018/03/13
     */

    void CAMReceiver::init()
    {
    }

    /**
     * 処理を開始する
     *
     * @author	Nagoya University
     * @date	2018/03/13
     */

    void CAMReceiver::start()
    {
		start(false);
    }

	/**
	* 処理を開始する(暗号化可否指定)
	*
	* @author	Nagoya University
	* @date	2019/07/31
	*
	* @param	isSSL	暗号化実施可否
	*/

	void CAMReceiver::start(bool isSSL)
	{
		if (!isSSL) {
			// 受信用スレッドの生成
			thread recvThread(&CAMReceiver::receiveUDPdata, this);
			recvThread.detach();
		}
		else {
			// 受信用スレッドの生成
			thread recvThread(&CAMReceiver::receiveDTLSdata, this, setupDTLS());
			recvThread.detach();
		}
	}

    /**
     * 処理を終了する
     *
     * @author	Nagoya University
     * @date	2018/03/13
     */

    void CAMReceiver::stop()
    {
		logger->debug("[CAM] exit_flag ON");
        exit_flag = true;
    }

	/**
	 * 受信時処理
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	data	受信データ
	 */

	void CAMReceiver::receiveProcess(RecvData &data)
	{
		IS::InformationSourceManager &ism = IS::InformationSourceManager::get_instance();

		// 受信データの解析(CAM->Tuple)
		char cary[data.payload.size()];
		data.payload.copy(cary, data.payload.size());
		CAMdata recvdata(cary);

		// Tuple生成
		IS::Schema schema;
		ism.getStreamSchema("CAM", schema);

		int attrSize = schema.getAttributeSize();
		vector<Tuple> tuples;
		Tuple camTuple(attrSize);

		camTuple.setValue(0, recvdata.stationID, 0);
		camTuple.setValue(1, recvdata.stationType, 0);
		camTuple.setValue(2, ((double)recvdata.lat / 10000000.0), 0);
		camTuple.setValue(3, ((double)recvdata.lon / 10000000.0), 0);
		camTuple.setValue(4, ((double)recvdata.speed / 100.0 * 60.0 * 60.0 / 1000.0), 0);
		camTuple.setValue(5, ((double)recvdata.direction / 10.0), 0);
		tuples.push_back(camTuple);

		// Tupleのキューイング
		IS::QueueManager &QM = IS::QueueManager::get_instance();
		QM.addTuple(schema, tuples);
	}

}
