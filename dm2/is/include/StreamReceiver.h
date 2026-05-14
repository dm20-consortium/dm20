#ifndef STREAMRECEIVER_H
#define STREAMRECEIVER_H

// StreamReceiverクラス
// Streamを受信する

#include <thread>
#include <log4cxx/logger.h>

#include "NetworkSource.h"
#include "FileSource.h"
#include "is/Schema.h"
#include "is/TupleSet.h"
#include "QueueManager.h"
#include "BufferQueue.h"
#include "SessionManager.h"

using namespace std;
using namespace log4cxx;

namespace IS {

	/**
	 * ストリームデータの受信処理を扱うクラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	class StreamReceiver : public NetworkSource, public FileSource
	{
	private:
		LoggerPtr logger = Logger::getLogger("StreamReceiver");

	public:
		StreamReceiver(string name, int port);
		virtual ~StreamReceiver();

		void init();       // impl
		void start();      // impl
		void start(bool isSSL);      // impl
		void stop();       // impl
		void receiveProcess(RecvData &data) override; // impl
		void workerProc(BufferQueue& q);

		void fileReadStart(string file, int num);
		void fileRead(string file, int num);

	};
}
#endif  // STREAMRECEIVER_H
