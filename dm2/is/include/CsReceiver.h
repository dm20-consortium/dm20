#ifndef CSRECEIVER_H
#define CSRECEIVER_H

#include <log4cxx/logger.h>
#include <unordered_map>
#include <thread>

#include "NetworkSource.h"
#include "QueueManager.h"
#include "QueryManager.h"
#include "UdpReceiveInterface.h"
#include "InformationSourceParser.h"
#include "BufferQueue.h"
#include "Settings.h"

using namespace std;
using namespace CS;
using namespace log4cxx;

namespace IS {

	/**
	 * CsReceiverクラス
	 * CS経由でUDP受信するクラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	class CsReceiver : public UdpReceiveInterface
	{
	private:
		LoggerPtr logger = Logger::getLogger("CsReceiver");
		IS::Settings &settings = IS::Settings::get_instance();
		std::unordered_map<string, vector<string>> recvDataMap;
		void dataIntegration(string &buf, string &result);
	public:
		CsReceiver();
		virtual ~CsReceiver();
		void start();
		void notify(send_message_upper buffer);

		void workerProc(BufferQueue& q);

	};
}
#endif  // CSRECEIVER_H
