#ifndef WINDOWOPERATOR_H
#define WINDOWOPERATOR_H

#include <log4cxx/logger.h>

#include "Operator.h"
#include "is/StringUtil.h"
#include "QueueManager.h"
#include "is/DmUtil.h"

using namespace log4cxx;

namespace IS {

	/**
	 * 動的情報(ストリームデータ)の取得処理を扱うオペレータ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	class WindowOperator : public Operator
	{
	private:
		const string MyName = "Window";
		LoggerPtr logger = Logger::getLogger("WindowOperator");
		StringUtil stringUtil;

		string masterName = "";
		string streamName = "";
		string schemaName = "";
		vector<int> partitionList;
		bool doPartition = false;
		int num = 0;
		int limit = 0;
		long msec = 0;
		long notifiedTime = 0;
		long bootTime = 0;
		bool notifyAllFlg = false;
		bool allowDuplicateData = false;
		long lastNotifiedTime = 0;
		double notifiedIntervalAVG = 0;
		long lastRecvTime = 0;
		long preLastRecvTime = 0;
		const int defaultDeleteHashSec = 10;		// 10秒  
		long duplicate_cnt = 0;
		void init(unsigned int mngId, string masterName, string streamName, string schemaName, const string _argument);

		pthread_mutex_t hashmap_mtx;
		TimerWorker* deleteHashmapWorker = NULL;
		void deleteHashmap();
		void duplicateMessage();
		map<string, long> tupleHashMap;

		bool matchHash(const Tuple &t, const Schema &schema);
		bool proc_normal(vector<IS::TupleSet>& ts, const Schema &schema);
		bool proc_by_cost_sort(vector<IS::TupleSet>& ts, const Schema &schema);
		bool proc_by_partition(vector<IS::TupleSet>& ts, const Schema &schema);
		bool proc_by_partition_create_time_rows1(vector<IS::TupleSet>& ts, const Schema &schema);
		bool proc_by_partition_create_time_rowsn(vector<IS::TupleSet>& ts, const Schema &schema);

		// proc_by_cost_sort用
		bool CostSortMode = false;
		string costId;
		const int id_idx = 0;
		const int x_idx = 22;
		const int y_idx = 21;  
		const int z_idx = 23; 
		map<string, double> keyX;
		map<string, double> keyY;
		map<string, double> keyZ;
		bool setKeyLine = false;
		bool getKeyLine(Tuple &t);
		void getCost(Tuple &t);
		// proc_by_partition_create_time_rows1用
		std::unordered_map<std::string, Tuple> latestTuple1;
		std::unordered_map<std::string, std::deque<Tuple>> latestTupleN;
		string makePartitionKey(const Tuple &t);
	public:
		WindowOperator(unsigned int mngId, string masterName, string streamName, string schemaName, int num, int _limit, string seed, vector<int> &_partitionList);
		WindowOperator(unsigned int mngId, string masterName, string streamName, string schemaName, long msec, vector<int> &_partitionList);
		WindowOperator(unsigned int mngId, string masterName, string streamName, string schemaName, long msec, bool allowDuplicateData);
		WindowOperator(unsigned int mngId, string masterName, string streamName, string schemaName);
		virtual ~WindowOperator();

		string  getTargetStreamName();
		long getTimeWindowSize();
		long getLastRecvTime();
		void setLastRecvTime(long recvTime);
		long getNotifiedTime();
		void setNotifiedTime(const long notifiedTime);
		void setAllowDuplicateData(const bool allowDuplicateData);
		int  getNumWindowSize();
		int  getNumPartitionSize();
		vector<int>  getPartitionList();
		double getNotifiedIntervalAVG();
		bool isNotifyAll();

		// 処理
		virtual bool process(vector<IS::TupleSet>& ts) override;
		virtual void requestData(long referenceTime);
	};
}
#endif  // WINDOWOPERATOR_H
