#ifndef IS_QUEUE_H
#define IS_QUEUE_H

#include "is/DmUtil.h"
#include "is/Tuple.h"
#include "TupleQueue.h"
#include "InformationSourceParser.h"
#include "Operator.h"
#include "is/StringUtil.h"
#include "Settings.h"
#include "TimerWorker.h"
#include "TimerThread.h"

#include <string>
#include <deque>
#include <thread>
#include <pthread.h>
#include <log4cxx/logger.h>

using namespace std;
using namespace log4cxx;

namespace IS {

	/**
	 * 削除予約情報
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	class DeleteReservation
	{
	public:
		long deleteMsec;
		int num;

		DeleteReservation(long deleteMsec, int num);
	};

	/**
	 * キュークラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	class Queue
	{
	private:
		LoggerPtr logger = Logger::getLogger("Queue");
		IS::Settings &settings = IS::Settings::get_instance();
		StringUtil stringUtil;

		string streamName;
		Schema schema;
		pthread_mutex_t queue_mtx;
		pthread_mutex_t listener_mtx;
		pthread_mutex_t referrer_mtx;
		TimerWorker* deleteQueueWorker = NULL;
		TimerWorker* timerStreamWorker = NULL;
		TimerThread* timerStreamThread = NULL;
		deque<Tuple> queue;
		vector<Operator*> listenerOperatorList;
		vector<Operator*> referrerOperatorList;
		vector<IS::DeleteReservation> deleteReservationList;

		TupleQueue tuple_rcv_q;

		bool isHandlingLocation = false;
		int sidIndex = 0;
		int laneIdIndex = 0;
		int max_worker_thread = 0;
		string createUser = "";

		// 計測用、実運用時は必要か判断する
		unsigned long long totalQueueNum = 0;
		unsigned long long  beforeQueueNum = 0;
		
		int defaultNum = 10;
		long defaultMsec = 10 * 1000;
		long deleteInterval = 1000;
		int deleteSearchIdx = 0;
		int streamRcvNum = 0;
		int timerMode = 0;

		void workerProc(TupleQueue& q);
		void getTupleSet(const long lastRecvTime, TupleSet &tupleset);
		void getTupleSetPartition(TupleSet &tupleset, const int num, vector<int> &partitionList);
		void getTupleSet(const int num, const long referenceTime, TupleSet &tupleset);
		void getTupleSet(const long rangeMsec, const long notifiedTime, const long referenceTime, TupleSet &tupleset);
		void createTimerStreamData(Schema schema);

	public:
		Queue(string streamName, Schema schema);
		~Queue();

		bool pushTuple(const vector<Tuple> &tuples);
		bool pushTuple(const string &user, const vector<Tuple> &tuples);
		void addTuple(vector<Tuple> &tuples);
		void addListener(Operator* ope);
		void removeListener(Operator* ope);
		int  getListenerSize();
		void addReferrer(Operator* ope);
		void removeReferrer(Operator* ope);
		void deleteQueue();
		void getTupleSetMtxLock(const long lastRecvTime, TupleSet &tupleset);
		void getTupleSetPartitionMtxLock(TupleSet &tupleset, const int num, vector<int> &partitionList);
		void getTupleSetMtxLock(const int num, const long referenceTime, TupleSet &tupleset);
		void getTupleSetMtxLock(const long rangeMsec, const long notifiedTime, const long referenceTime, TupleSet &tupleset);
		void setSchemaInfo(Schema &schemaInfo);
		bool isSelect(const string &user);
		bool isRefAdminCol(const string &user);
		int  getStreamRcvNum();
		void settingTimer(const string &user, const long timer_msec, const long start_fix_msec, Schema schema);
		void finalize();
	};
}
#endif  // IS_QUEUE_H
