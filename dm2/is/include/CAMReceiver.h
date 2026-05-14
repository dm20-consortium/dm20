#ifndef CAMRECEIVER_H
#define CAMRECEIVER_H

#include <deque>
#include <condition_variable>
#include <mutex>
#include <log4cxx/logger.h>

#include "NetworkSource.h"
#include <is/Schema.h>
#include <is/TupleSet.h>
#include "QueueManager.h"

using namespace std;
using namespace log4cxx;

namespace IS {

    /**
     * CAMデータを扱うクラス
     *
     * @author	Nagoya University
     * @date	2018/03/14
     */

    class CAMdata
    {
	private:
		LoggerPtr logger = Logger::getLogger("CAMdata");
    public:
        // 各項目の先頭を指すインデックス
        static const int MD_Head_PROTOCOL_VERSION = 0;
        static const int MD_Head_MESSAGE_ID = 4;
        static const int MD_Head_STATION_ID = 8;
        static const int MD_Head_GENERATION_TIME = 12;
        static const int MD_Head_STATION = 20;
        static const int MD_Head_LATITUDE = 24;
        static const int MD_Head_LONGITUDE = 28;
        static const int MD_Head_ALTITUDE = 32;
        static const int MD_Head_HEADING = 36;
        static const int MD_Head_SPEED = 40;
        static const int MD_Head_DRIVING_DIRECTION = 44;
        static const int MD_MessageSize = 48;

        unsigned int protocolVersion;
        unsigned int messageID;
        unsigned int stationID;
        unsigned long long time;
        unsigned int stationType;   // 0:UNKNOWN, 1:PEDESTRAIN, 2:CYCLIST, 4:MOTORCYCE, 5]PASSENGER_CAR, 15:ROAD_SIDE_UNIT
        int lat;                    // 0.0000001deg
        int lon;                    // 0.0000001deg
        int alt;                    // 0.01m
        int heading;                // 0.1deg
        int speed;                  // 0.01m/sec
        int direction;              // 0.1deg

        CAMdata(char data[]);

        unsigned int getUINT(char* data);
        unsigned long long getULONG(char* data);
        int getINT(char* data);

        void dump();
    };

	/**
	* CAMReceiverクラス
	* CAM(Coorporative awareness message)をUDP受信するクラス
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*/

    class CAMReceiver : public NetworkSource
    {
    private:
		LoggerPtr logger = Logger::getLogger("CAMReceiver");
    public:
        CAMReceiver(string name, int port);
        virtual ~CAMReceiver();

        void init();       // impl
        void start();      // impl
		void start(bool isSSL);      // impl
        void stop();       // impl
		void receiveProcess(RecvData &data) override; // impl
    };
}
#endif  // CAMRECEIVER_H
