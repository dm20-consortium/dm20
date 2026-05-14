#include "Queue.h"
#include "WindowOperator.h"
long debug_cnt = 0;
namespace IS {

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	streamName	ストリーム名
	 * @param	schema	  	スキーマ情報
	 */

	Queue::Queue(string streamName, Schema schema)
	{
		this->streamName = streamName;
		this->schema = schema;
		queue.clear();
		listenerOperatorList.clear();
		referrerOperatorList.clear();
		pthread_mutex_init(&queue_mtx, NULL);
		pthread_mutex_init(&listener_mtx, NULL);
		pthread_mutex_init(&referrer_mtx, NULL);

		if (settings.getParameter("TIMER_MODE").length() != 0) {
			timerMode = stoi(settings.getParameter("TIMER_MODE"));
		}
		// キュー削除設定
		if (settings.getParameter("QUEUE_DELETE_INTERVAL_SEC").length() != 0) {
			deleteInterval = stoi(settings.getParameter("QUEUE_DELETE_INTERVAL_SEC")) * 1000;
		}
		if (settings.getParameter("QUEUE_DELETE_DEFAULT_RANGE_SEC").length() != 0) {
			defaultMsec = stoi(settings.getParameter("QUEUE_DELETE_DEFAULT_RANGE_SEC")) * 1000;
		}
		if (settings.getParameter("QUEUE_DELETE_DEFAULT_RANGE_NUM").length() != 0) {
			defaultNum = stoi(settings.getParameter("QUEUE_DELETE_DEFAULT_RANGE_NUM"));
		}
		// タイマーを設定する
		this->deleteQueueWorker = new TimerWorker(deleteInterval, std::bind(&Queue::deleteQueue, this));

		// キューイングスレッド
		string worker_num_str;
		if (streamName == schema.getTableName()) {
			worker_num_str = settings.readConfigFile(("QUEUE_RECEIVE_WORKER_NUM_" + streamName).c_str(), settings.CONF_FILE_SYS, false);
		}
		else {
			// 一時キューの場合はオリジナルのスレッド数に合わせる
			worker_num_str = settings.readConfigFile(("QUEUE_RECEIVE_WORKER_NUM_" + schema.getTableName()).c_str(), settings.CONF_FILE_SYS, false);
			this->schema.renameTableName(schema.getTableName(), streamName);
		}

		if (worker_num_str.length() == 0) {
			// デフォルト値を設定する
			worker_num_str = settings.getParameter("QUEUE_RECEIVE_WORKER_DEFAULT_NUM");
		}
		max_worker_thread = std::stoi(worker_num_str);
		for (int i = 0; i < max_worker_thread; i++) {
			thread workerThread(&Queue::workerProc, this, ref(tuple_rcv_q));
			workerThread.detach();
		}

		// 位置情報監視対象かを判断し、対象の場合は監視インデックスを事前に取得する
		IS::LocationManager &LM = IS::LocationManager::get_instance();
		string sidColName, laneIdColName;
		this->isHandlingLocation = LM.isHandlingLocation(streamName, sidColName, laneIdColName);
		if (this->isHandlingLocation) {
			sidIndex = schema.getAttributeIdx(sidColName);
			laneIdIndex = schema.getAttributeIdx(laneIdColName);
		}

		logger->debug("Name:" + streamName + " QueueingWorker thread created :" + std::to_string(max_worker_thread));

	}

	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	Queue::~Queue()
	{
		pthread_mutex_destroy(&queue_mtx);
		pthread_mutex_destroy(&listener_mtx);
		pthread_mutex_destroy(&referrer_mtx);
		finalize();
	}

	/**
	* 終了処理
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*/

	void Queue::finalize()
	{
		if (this->deleteQueueWorker != NULL) {
			this->deleteQueueWorker->abort_thread();
			delete deleteQueueWorker;
			deleteQueueWorker = NULL;
		}
		if (this->timerStreamWorker != NULL) {
			this->timerStreamWorker->abort_thread();
			delete timerStreamWorker;
			timerStreamWorker = NULL;
		}
	}

	/**
	 * タプル追加要求（ユーザ権限チェックあり）
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	user	ユーザ名
	 * @param	tuples	The tuple List.
	 *
	 * @return	True if it succeeds, false if it fails.
	 */

	bool Queue::pushTuple(const string &user, const vector<Tuple> &tuples)
	{
		//REL_COMMENT logger->trace("[pushTuple] Name:" + streamName + " IN");
		if (schema.isAppend(user)) {
			tuple_rcv_q.Push(tuples);
		}
		return true;
	}

	/**
	 * タプル追加要求（ユーザ権限チェックなし）
	 *
	 * @author	Shinichi Kusayama
	 * @date	2024/05/02
	 *
	 * @param	tuples	The tuple List.
	 *
	 * @return	
	 */

	bool Queue::pushTuple(const vector<Tuple> &tuples)
	{
		tuple_rcv_q.Push(tuples);
		return true;
	}

	/**
	 * タプルキューイング
	 *
	 * @author	Nagoya University
	 * @date	2018/10/25
	 *
	 * @param	q	The tuple queue.
	 *
	 */
	void Queue::workerProc(TupleQueue& q)
	{
		vector<Tuple> data;
		while (1) {
			q.Pop(data);
			addTuple(data);
		}
	}

	/**
	 * タプル追加処理
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param[in]	tuples	The tuple List.
	 */
	void Queue::addTuple(vector<Tuple> &tuples)
	{
		//REL_COMMENT logger->trace("[addTuple] Name:" + streamName + " IN tuple.size():" + std::to_string(tuples.size()));
		int    receiveTupleNum = 0;

#if MEASURE_MODE == 1
		long startTime = DmUtil::getTimeMicrosec();
		long procTime = startTime;
		int step = 1;
#endif

		//REL_COMMENT logger->trace("[addTuple] Name:" + streamName + " MUTEX queue_mtx lock Size:" + std::to_string(queue.size()));
		pthread_mutex_lock(&queue_mtx);

		long recvTime = DmUtil::getTimeMicrosec();
		{
			for (unsigned int i = 0; i < tuples.size(); i++) {
				/*
				debug_cnt++;
				if (debug_cnt % 10000 == 0) {
					long delay = recvTime / 1000 - tuples.at(i).getNewestTimestamp();
					logger->warn("[addTuple] Delay[msec]" + to_string(delay));
				}
				*/
				// 受信時間の付与
				tuples.at(i).setRecvTime(recvTime);
				string tablename = schema.getTableName();
				// 時刻グループ列の付与
				int tsGrpIdx = schema.getAttributeIdx(tablename + "." + Schema::COL_TS_GRP);
				if (tsGrpIdx != -1) {
					any val;
					tuples.at(i).getValueByIdx(tsGrpIdx, val);
					string strVal = stringUtil.getAnyString(val);
 					if (strVal  == "(null)") {
						strVal = to_string(recvTime) + ",";
					} else {
						strVal = strVal + to_string(recvTime) + ",";
					}
					tuples.at(i).setValue(tsGrpIdx, strVal, 0, false);
				}
				// queueに追加
				queue.push_back(tuples.at(i));
				totalQueueNum++;
				receiveTupleNum++;
			}
		}

#if MEASURE_MODE == 1
		long now = DmUtil::getTimeMicrosec();
		double msec = (now - procTime) / 1000.0;
		logger->info("[addTuple(" + streamName + ")] STAT_STEP" + to_string(step++) + "  queue.push_back complete time: " + to_string(msec) + "[ms]");
		logger->info("[addTuple(" + streamName + ")] queuing complete NowMicroSec: " + to_string(recvTime));
		procTime = now;
#endif

		// listenしているwindowオペレータへの通知
		//REL_COMMENT logger->trace("[addTuple] Name:" + streamName + " MUTEX listener_mtx lock. Size:" + to_string(listenerOperatorList.size()));
		pthread_mutex_lock(&listener_mtx);

		if (listenerOperatorList.size() > 0) {
			IS::QueueManager &QM = IS::QueueManager::get_instance();
			TupleSet ts;
			WindowOperator *wOpe = NULL;
			string targetStreamName;

			for (Operator* ope : listenerOperatorList)
			{
				ts.clear();
				ts.info.type = TupleSet::OccurrenceType::master;
				ts.info.recvTime = recvTime;

				wOpe = dynamic_cast<WindowOperator*>(ope);
				if (wOpe != NULL) {
					targetStreamName = wOpe->getTargetStreamName();
					int numWindowSize = wOpe->getNumWindowSize();
					int partitionSize = wOpe->getNumPartitionSize();
					vector<int> partitionList = wOpe->getPartitionList();
					long lastRecvTime = wOpe->getLastRecvTime();
					//cout << "[addTuple]this->streamName:" << this->streamName << ",wOpe->targetStreamName:" << targetStreamName  << endl;
					if (numWindowSize > 0)
					{
						// 個数Window
						if (targetStreamName == this->streamName) {
							if (partitionSize > 0) {
								//getTupleSetPartition(ts, numWindowSize, partitionList);
								getTupleSet(lastRecvTime, ts);
								wOpe->setLastRecvTime(recvTime);
							} else {
								//getTupleSet(ts);
								//getTupleSet(numWindowSize, tuples.at(0).getNewestTimestamp(), ts);
								getTupleSet(numWindowSize, lastRecvTime, ts);
								wOpe->setLastRecvTime(recvTime);
							}
						}
						else {
							//QM.getTupleSet(targetStreamName, wOpe->getNumWindowSize(), tuples.at(0).getNewestTimestamp(), ts);
							if (partitionSize > 0) {
								//QM.getTupleSetPartition(targetStreamName, ts, numWindowSize, partitionList);
								//QM.getTupleSet(targetStreamName, ts);
								QM.getTupleSet(targetStreamName, lastRecvTime, ts);
								wOpe->setLastRecvTime(recvTime);
							} else {
								//QM.getTupleSet(targetStreamName, ts);
								QM.getTupleSet(targetStreamName, numWindowSize, lastRecvTime, ts);
								wOpe->setLastRecvTime(recvTime);
							}
						}
						ope->receiveTupleSet(ts);
					}
					else if (wOpe->getTimeWindowSize() > 0)
					{
						// 時間Window
						if (targetStreamName == this->streamName) {
							getTupleSet(wOpe->getTimeWindowSize(), wOpe->getNotifiedTime(), tuples.at(0).getNewestTimestamp(), ts);
						}
						else {
							QM.getTupleSet(targetStreamName, wOpe->getTimeWindowSize(), wOpe->getNotifiedTime(), tuples.at(0).getNewestTimestamp(), ts);
						}
						ope->receiveTupleSet(ts);
						if (ts.size() > 0) {
							wOpe->setNotifiedTime(ts.getTuple(0).getNewestTimestamp());
						}
					}
					else if (wOpe->getTimeWindowSize() == 0 || wOpe->isNotifyAll())
					{
						// 全通知Windowの場合は受信したデータ数分を個数Windowとし通知
						if (targetStreamName == this->streamName) {
							getTupleSet(receiveTupleNum, tuples.at(0).getNewestTimestamp(), ts);
						}
						else {
							QM.getTupleSet(targetStreamName, receiveTupleNum, tuples.at(0).getNewestTimestamp(), ts);
						}
						ope->receiveTupleSet(ts);
					}
				}
				else {
					// RDBAccessOperator/RecursiveOperatorの場合
					Schema schema("dummyStream", Schema::STREAM);
					ts.SetSchema(schema);
					ope->receiveTupleSet(ts);
				}
			}
		}
		pthread_mutex_unlock(&listener_mtx);
		//REL_COMMENT logger->trace("[addTuple] Name:" + streamName + " MUTEX listener_mtx unlock. Size:" + to_string(listenerOperatorList.size()));
		pthread_mutex_unlock(&queue_mtx);

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - procTime) / 1000.0;
		logger->info("[addTuple(" + streamName + ")] STAT_STEP" + to_string(step++) + " listener handling time: " + to_string(msec) + "[ms]");
		msec = (now - startTime) / 1000.0;
		logger->info("[addTuple(" + streamName + ")] STAT_STEP" + to_string(step++) + " critival section(queue_mtx) processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif
		//REL_COMMENT logger->trace("[addTuple] Name:" + streamName + " MUTEX queue_mtx unlock Size:" + std::to_string(queue.size()));

		// 位置情報を扱うストリームの場合は管理対象が存在する場合のみ、位置情報を更新する
		/*
		if (this->isHandlingLocation && settings.getSIDType() != Settings::SID_TYPE::CAR) {
			IS::LocationManager &LM = IS::LocationManager::get_instance();
			any val;
			//for (unsigned int vehicleSID : LM.getLocationMngVehicleID())
			for (unsigned long long vehicleSID : LM.getLocationMngVehicleID())
			{
				for (unsigned int i = 0; i < tuples.size(); i++) {
					tuples.at(i).getValueByIdx(sidIndex, val);
					//if (static_cast<unsigned int>(stoi(stringUtil.getAnyString(val))) == vehicleSID) {
					if (static_cast<unsigned long long>(stoull(stringUtil.getAnyString(val))) == vehicleSID) {
						tuples.at(i).getValueByIdx(laneIdIndex, val);
						//int lane_id = stoi(stringUtil.getAnyString(val));
						unsigned long long lane_id = stoull(stringUtil.getAnyString(val));
						LM.updateLaneId(vehicleSID, lane_id);
						break;
					}
				}
			}
		}
		*/
#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - procTime) / 1000.0;
		logger->info("[addTuple(" + streamName + ")] STAT_STEP" + to_string(step++) + " location handling time: " + to_string(msec) + "[ms]");
		msec = (now - startTime) / 1000.0;
		logger->info("[addTuple(" + streamName + ")] STAT_STEP" + to_string(step++) + " total processing time: " + to_string(msec) + "[ms]");
#endif
	}

	/**
	 * キュー到着イベントをlistenするオペレータを追加する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in,out]	ope	オペレータオブジェクト
	 */

	void Queue::addListener(Operator* ope)
	{
		//REL_COMMENT logger->trace("[addListener] Name:" + streamName + " MUTEX listener_mtx lock. Size:" + to_string(listenerOperatorList.size()));
		pthread_mutex_lock(&listener_mtx);
		listenerOperatorList.push_back(ope);
		pthread_mutex_unlock(&listener_mtx);
		//REL_COMMENT logger->trace("[addListener] Name:" + streamName + " MUTEX listener_mtx unlock. Size:" + to_string(listenerOperatorList.size()));
	}

	/**
	 * キュー到着イベントをlistenするオペレータを削除する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in,out]	removeOpe	削除するオペレータオブジェクト
	 */

	void Queue::removeListener(Operator* removeOpe)
	{
		//REL_COMMENT logger->trace("[removeListener] Name:" + streamName + " MUTEX listener_mtx lock. Size:" + to_string(listenerOperatorList.size()));
		pthread_mutex_lock(&listener_mtx);
		for (unsigned int i = 0; i < listenerOperatorList.size(); i++)
		{
			if (removeOpe == listenerOperatorList.at(i)) {
				listenerOperatorList.erase(listenerOperatorList.begin() + i);
				logger->debug("[removeListener] Queue Listener remove.");
			}
		}
		pthread_mutex_unlock(&listener_mtx);
		//REL_COMMENT logger->trace("[removeListener] Name:" + streamName + " MUTEX listener_mtx unlock. Size:" + to_string(listenerOperatorList.size()));
	}

	/**
	* キュー到着イベントをlistenするオペレータの数を取得する
	*
	* @author	Nagoya University
	* @date	2019/04/17
	*
	* @return listenするオペレータ数
	*/
	int Queue::getListenerSize()
	{
		int size = 0;
		//REL_COMMENT logger->trace("[getListenerSize] Name:" + streamName + " MUTEX listener_mtx lock. Size:" + to_string(listenerOperatorList.size()));
		pthread_mutex_lock(&listener_mtx);
		size = listenerOperatorList.size();
		pthread_mutex_unlock(&listener_mtx);
		//REL_COMMENT logger->trace("[getListenerSize] Name:" + streamName + " MUTEX listener_mtx unlock. Size:" + to_string(listenerOperatorList.size()));
		return size;
	}

	/**
	* キューを参照するオペレータを追加する
	*
	* @author	Nagoya University
	* @date	2019/08/05
	*
	* @param [in,out]	ope	オペレータオブジェクト
	*/

	void Queue::addReferrer(Operator* ope)
	{
		//REL_COMMENT logger->trace("[addReferrer] Name:" + streamName + " MUTEX listener_mtx lock. Size:" + to_string(referrerOperatorList.size()));
		pthread_mutex_lock(&referrer_mtx);
		referrerOperatorList.push_back(ope);
		pthread_mutex_unlock(&referrer_mtx);
		//REL_COMMENT logger->trace("[addReferrer] Name:" + streamName + " MUTEX listener_mtx unlock. Size:" + to_string(referrerOperatorList.size()));
	}

	/**
	* キューを参照するオペレータを削除する
	*
	* @author	Nagoya University
	* @date	2019/08/05
	*
	* @param [in,out]	removeOpe	削除するオペレータオブジェクト
	*/

	void Queue::removeReferrer(Operator* removeOpe)
	{
		//REL_COMMENT logger->trace("[removeReferrer] Name:" + streamName + " MUTEX listener_mtx lock. Size:" + to_string(referrerOperatorList.size()));
		pthread_mutex_lock(&referrer_mtx);
		for (unsigned int i = 0; i < referrerOperatorList.size(); i++)
		{
			if (removeOpe == referrerOperatorList.at(i)) {
				referrerOperatorList.erase(referrerOperatorList.begin() + i);
				logger->debug("[removeReferrer] Queue Referrer remove.");
			}
		}
		pthread_mutex_unlock(&referrer_mtx);
		//REL_COMMENT logger->trace("[removeReferrer] Name:" + streamName + " MUTEX listener_mtx unlock. Size:" + to_string(referrerOperatorList.size()));
	}

	/**
	 * 全件タプルセットへと格納する (Partition By 到着順でrowsPerPartition件数分抽出)
	 *
	 * @author	Shinichi Kusayama
	 * @date	2025/8/12
	 *
	 * @param [in,out]	Tupleset	タプルセット.
	 * @param	rowsPerPartition	パーティション毎の最新1件（到着順）
	 * @param	partitionList		パーティションリスト
	 */

	void Queue::getTupleSetPartition(TupleSet &tupleset, const int rowsPerPartition, vector<int> &partitionList)
	{
		//REL_COMMENT logger->trace("[getTupleSet(num)] Name:" + streamName + " IN");
		// key: パーティションキー列値の組み合わせ
		std::unordered_map<string, int> countMap;
		int addNum = 0;
		tupleset.SetSchema(this->schema);

		const int num = queue.size();
		tupleset.resize(num);

		for (int i = queue.size() - 1; i >= 0; i--)
		{
			const Tuple &t = queue.at(i);
			// パーティションキーを抽出
			string keyGroup;
			for (int colIdx : partitionList) {
				any key;
				t.getValueByIdx(colIdx, key);
				string keyStr = stringUtil.getAnyString(key);
				keyGroup += keyStr + "|";
			}
			// 最新順に追加
			if (countMap[keyGroup] < rowsPerPartition) {
				try {
					tupleset.setTuple(addNum++, t);
				}
				catch (out_of_range const &) {
					logger->error("[getTupleSet(num)] Name:" + streamName + " ERROR deque already delete at " + std::to_string(i) + " size():" + std::to_string(queue.size()));
					break;
				}
				countMap[keyGroup]++;
			}
		}
		if(num != addNum) tupleset.resize(addNum);
		//REL_COMMENT logger->trace("[getTupleSet(num)] Name:" + streamName + " OUT addNum:" + std::to_string(addNum));
		return;
	}
	/**
	* 全件タプルセットへと格納する(mutex処理wrapper)
	*
	 * @author	Shinichi Kusayama
	 * @date	2025/8/12
	 *
	 * @param [in,out]	Tupleset	タプルセット.
	 * @param	partitionList		パーティションリスト
	*/

	void Queue::getTupleSetPartitionMtxLock(TupleSet &tupleset, const int num, vector<int> &partitionList)
	{
		//REL_COMMENT logger->trace("[getTupleSet num] Name:" + streamName + " MUTEX queue_mtx lock.");
		pthread_mutex_lock(&queue_mtx);

		getTupleSetPartition(tupleset, num, partitionList);

		pthread_mutex_unlock(&queue_mtx);
		//REL_COMMENT logger->trace("[getTupleSet num] Name:" + streamName + " MUTEX queue_mtx unlock.");
	}

	/**
	 * 全件タプルセットへと格納する
	 *
	 * @author	Shinichi Kusayama
	 * @date	2023/3/29
	 *
	 * @param	referenceTime		  	取得するための基準時間
	 * @param [in,out]	Tupleset	タプルセット.
	 */
	 
	void Queue::getTupleSet(const long lastRecvTime, TupleSet &tupleset)
	{
		//REL_COMMENT logger->trace("[getTupleSet(num)] Name:" + streamName + " IN");
		tupleset.SetSchema(this->schema);

		const int num = queue.size();
		tupleset.resize(num);

		int addNum = 0;
		for (int i = queue.size() - 1; i >= 0; i--)
		{
			//cout << "[getTupleSet]recvTime:" << queue.at(i).getRecvTime() << ",lastRecvTime:" << lastRecvTime <<  endl;
			if (queue.at(i).getRecvTime() <= lastRecvTime) {
				if (max_worker_thread == 1) {
					break;
				} else {
					// 受信スレッドが複数ある場合、受信時刻が新旧混在する可能性あり
					// 最後までキューを検索すると、時間がかかるため、混在する区間を1秒 (1000 * 1000 マイクロ秒) と見做す
					if (lastRecvTime - queue.at(i).getRecvTime() > 1000 * 1000) {
						break;
					} else {
						continue;
					}
				}
			}
			try {
				tupleset.setTuple(addNum++, queue.at(i));
			}
			catch (out_of_range const &) {
				logger->error("[getTupleSet(num)] Name:" + streamName + " ERROR deque already delete at " + std::to_string(i) + " size():" + std::to_string(queue.size()));
				break;
			}
		}
		//cout << "addNum:" << addNum << endl;
		if(num != addNum) tupleset.resize(addNum);

		//REL_COMMENT logger->trace("[getTupleSet(num)] Name:" + streamName + " OUT addNum:" + std::to_string(addNum));
		return;
	}
	/**
	* 全件タプルセットへと格納する(mutex処理wrapper)
	*
	* @author	Shinichi Kusayama
	* @date	2023/3/29
	*
	* @param [in,out]	tupleset	タプルセット.
	*/

	void Queue::getTupleSetMtxLock(const long lastRecvTime, TupleSet &tupleset)
	{
		//REL_COMMENT logger->trace("[getTupleSet num] Name:" + streamName + " MUTEX queue_mtx lock.");
		pthread_mutex_lock(&queue_mtx);

		getTupleSet(lastRecvTime, tupleset);

		pthread_mutex_unlock(&queue_mtx);
		//REL_COMMENT logger->trace("[getTupleSet num] Name:" + streamName + " MUTEX queue_mtx unlock.");
	}
	/**
	 * 個数指定でタプルセットを取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	num	取得するタプル数
	 * @param	lastRecvTime		  	取得するための基準時間
	 * @param [in,out]	Tupleset	タプルセット.
	 */
	 
	void Queue::getTupleSet(const int num, const long lastRecvTime, TupleSet &tupleset)
	{
		//REL_COMMENT logger->trace("[getTupleSet(num)] Name:" + streamName + " IN");
		tupleset.SetSchema(this->schema);

		int addNum = 0;
		tupleset.resize(num);

		for (int i = queue.size() - 1; i >= 0; i--)
		{
			//cout << "[getTupleSet]recvTime:" << queue.at(i).getRecvTime() << ",lastRecvTime:" << lastRecvTime <<  endl;
			// 前回受信時刻との比較
			if (queue.at(i).getRecvTime() <= lastRecvTime) {
				if (max_worker_thread == 1) {
					break;
				} else {
					// 受信スレッドが複数ある場合、受信時刻が新旧混在する可能性あり
					// 最後までキューを検索すると、時間がかかるため、混在する区間を1秒 (1000 * 1000 マイクロ秒) と見做す
					if (lastRecvTime - queue.at(i).getRecvTime() > 1000 * 1000) {
						break;
					} else {
						continue;
					}
				}
			}
			try {
				tupleset.setTuple(addNum++, queue.at(i));
				if (addNum >= num) break;
			}
			catch (out_of_range const &) {
				logger->error("[getTupleSet(num)] Name:" + streamName + " ERROR deque already delete at " + std::to_string(i) + " size():" + std::to_string(queue.size()));
				break;
			}
		}
		//cout << "addNum:" << addNum << endl;
		if(num != addNum) tupleset.resize(addNum);

		//REL_COMMENT logger->trace("[getTupleSet(num)] Name:" + streamName + " OUT addNum:" + std::to_string(addNum));
		return;
	}
	/**
	* 個数指定でタプルセットを取得する(mutex処理wrapper)
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param	num	取得するタプル数
	* @param	referenceTime		  	取得するための基準時間
	* @param [in,out]	tupleset	タプルセット.
	*/

	void Queue::getTupleSetMtxLock(const int num, const long referenceTime, TupleSet &tupleset)
	{
		//REL_COMMENT logger->trace("[getTupleSet num] Name:" + streamName + " MUTEX queue_mtx lock.");
		pthread_mutex_lock(&queue_mtx);

		getTupleSet(num, referenceTime, tupleset);

		pthread_mutex_unlock(&queue_mtx);
		//REL_COMMENT logger->trace("[getTupleSet num] Name:" + streamName + " MUTEX queue_mtx unlock.");
	}

	/**
	 * 時間範囲指定でタプルセットを取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	rangeMsec	時間ウインドウ(msec)
	 * @param	notifiedTime	通知済み時間(msec)
	 * @param	referenceTime		  	取得するための基準時間
	 * @param [in,out]	tupleset	タプルセット
	 */
	
	void Queue::getTupleSet(const long rangeMsec, const long notifiedTime, const long referenceTime, TupleSet &tupleset)
	{
		//REL_COMMENT logger->trace("[getTupleSet(msec)] Name:" + streamName + " IN");
		tupleset.SetSchema(this->schema);

		if (queue.size() == 0) return;
		long queueTime = 0;
		for (int i = queue.size() - 1; i >= 0; i--)
		{
			queueTime = queue.at(i).getNewestTimestamp();
			//REL_COMMENT logger->trace("[getTupleSet(msec)] queue[" + to_string(i) + "].time:" + to_string(queue.at(i).getNewestTimestamp()) + " referenceTime:" + to_string(referenceTime));
			if (queueTime <= referenceTime) {
				try {
					//REL_COMMENT logger->trace("[getTupleSet(msec)] checkTimestamp(arg:" + to_string(referenceTime - rangeMsec) + ") notifiedTime:" + to_string(notifiedTime) + " queueTime:" + to_string(queueTime));
					if (queue.at(i).checkTimestamp(max(referenceTime - rangeMsec, notifiedTime), this->schema)) {
						tupleset.addTuple(queue.at(i));
					}
					else {
						break;
					}
				}
				catch (out_of_range const &) {
					logger->error("[getTupleSet(msec)] Name:" + streamName + " ERROR deque already delete at " + std::to_string(i) + " size():" + std::to_string(queue.size()));
					break;
				}
			}
		}

		//REL_COMMENT logger->trace("[getTupleSet(msec)] Name:" + streamName + " OUT. tupleSet.size:" + std::to_string(tupleset.size()));
		return;
	}

	/**
	* 時間範囲指定でタプルセットを取得する(queue_mtx処理wrapper)
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param	rangeMsec	時間ウインドウ(msec)
	* @param	notifiedTime	通知済み時間(msec)
	* @param	referenceTime		  	取得するための基準時間
	* @param [in,out]	tupleset	タプルセット
	*/

	void Queue::getTupleSetMtxLock(const long rangeMsec, const long notifiedTime, const long referenceTime, TupleSet &tupleset)
	{
		//REL_COMMENT logger->trace("[getTupleSet msec] Name:" + streamName + " MUTEX queue_mtx lock.");
		pthread_mutex_lock(&queue_mtx);

		getTupleSet(rangeMsec, notifiedTime, referenceTime, tupleset);

		pthread_mutex_unlock(&queue_mtx);
		//REL_COMMENT logger->trace("[getTupleSet msec] Name:" + streamName + " MUTEX queue_mtx unlock.");
	}

	/**
	* スキーマ情報を取得する
	*
	* @author	Nagoya University
	* @date	2018/04/25
	*
	* @param [in,out]	schemaInfo	スキーマ情報
	*/
	void Queue::setSchemaInfo(Schema &schemaInfo)
	{
		schemaInfo = this->schema;
	}

	/**
	* 指定ユーザの読出し許可可否を取得する
	*
	* @author	Nagoya University
	* @date	2019/09/04
	*
	* @param [in]	user	ユーザ名
	* @return	許可可否
	*/
	bool Queue::isSelect(const string &user)
	{
		return schema.isSelect(user);
	}

	/**
	* 指定ユーザの管理者情報参照可否を取得する
	*
	* @author	Nagoya University
	* @date	2019/09/04
	*
	* @param [in]	user	ユーザ名
	* @return	許可可否
	*/
	bool Queue::isRefAdminCol(const string &user)
	{
		return schema.isRefAdminCol(user);
	}

	/**
	 * WindowOperatorから有効範囲情報を取得し、範囲外のQueueを削除する
	 * 有効範囲は時間Windowと個数Windowで削除量が少ない方を優先する(残存量が多い方を優先)
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void Queue::deleteQueue()
	{
		if (queue.size() == 0) return;
		long startDelTime = DmUtil::getTimeMicrosec();
		int maxNum = 0;
		long maxMsec = 0;
		//REL_COMMENT logger->trace("[deleteQueue] Name:" + streamName + " MUTEX referrer_mtx lock. Size:" + to_string(referrerOperatorList.size()));
		pthread_mutex_lock(&referrer_mtx);
		for (Operator* ope : referrerOperatorList)
		{
			// Queueのリスナーは必ずWindowOperatorなのでダウンキャスト
			WindowOperator *wOpe = dynamic_cast<WindowOperator*>(ope);
			if (wOpe != NULL) {
				if (wOpe->getNumWindowSize() > maxNum)
				{
					maxNum = wOpe->getNumWindowSize();
				}
				if (wOpe->getTimeWindowSize() > maxMsec)
				{
					maxMsec = wOpe->getTimeWindowSize();
				}
			}
		}
		pthread_mutex_unlock(&referrer_mtx);
		//REL_COMMENT logger->trace("[deleteQueue] Name:" + streamName + " MUTEX referrer_mtx unlock. Size:" + to_string(referrerOperatorList.size()));

		if (maxNum == 0) maxNum = defaultNum;
		if (maxMsec == 0 || maxMsec < defaultMsec) maxMsec = defaultMsec;
		//logger->warn("[deleteQueue] Name:" + streamName + ",Num:" + to_string(maxNum) + ",Msec" + to_string(maxMsec));
		//REL_COMMENT logger->trace("[deleteQueue] Name:" + streamName + " MUTEX queue_mtx lock.");
		pthread_mutex_lock(&queue_mtx);
		int queueSize = queue.size();
		//REL_COMMENT logger->trace("[deleteQueue] Name:" + streamName + " queue.size() :" + std::to_string(queueSize));
		long now = DmUtil::getTimeMillisec();
		int deleteNumByTime = 0;
		int deleteNumByNum = queue.size() - maxNum;

		// 削除予約を追加
		long newestTime = 0;
		int num = 0;

		for (unsigned int i = deleteSearchIdx; i < queue.size(); i++)
		{
			long getTime = queue.at(i).getNewestTimestamp();
			if (getTime > newestTime) newestTime = getTime;
			num++;
		}

		deleteSearchIdx = deleteSearchIdx + num;
		if (num > 0) {
			DeleteReservation dr(newestTime + maxMsec, num);
			deleteReservationList.push_back(dr);
		}

		// 今回の削除数を算出
		for (DeleteReservation data : deleteReservationList)
		{
			if (data.deleteMsec < now )
			{
				deleteNumByTime = deleteNumByTime + data.num;
			}
		}

		if (deleteNumByTime > deleteNumByNum)
		{
			// 時間Windowサイズよりも個数Windowサイズで削除した方が削除量が少ない場合
			if (deleteNumByNum > 0) {
				logger->trace("[deleteQueue] Name:" + streamName + " deleteByNum : " + std::to_string(deleteNumByNum) + "(" + to_string(queue.size()) + "->" + to_string(queue.size() - deleteNumByNum) + ") maxNum : " + std::to_string(maxNum) + " byTime : " + to_string(deleteNumByTime));
				for (int i = 0; i < deleteNumByNum; i++) {
					queue.pop_front();
				}

				// 削除予約の情報も削除
				vector<DeleteReservation>::iterator it = deleteReservationList.begin();
				long newTimestamp = queue.at(0).getNewestTimestamp();
				while (it != deleteReservationList.end()) {
					if (it->deleteMsec < newTimestamp) {
						it = deleteReservationList.erase(it);
					}
					else ++it;
				}
			}
		}
		else
		{
			if (deleteNumByTime > 0) {
				logger->trace("[deleteQueue] Name:" + streamName + " deleteByTime : " + std::to_string(deleteNumByTime) + "(" + to_string(queue.size()) + "->" + to_string(queue.size() - deleteNumByTime) + ") now : " + std::to_string(now) + " maxMsec : " + std::to_string(maxMsec) + " byNum : " + to_string(deleteNumByNum));
				vector<long> deleteList;
				// 個数Windowサイズよりも時間Windowサイズで削除した方が削除量が少ない場合
				for (DeleteReservation data : deleteReservationList)
				{
					if (data.deleteMsec < now )
					{
						queue.erase(queue.begin(), queue.begin() + data.num);
						deleteList.push_back(data.deleteMsec);
						deleteSearchIdx = deleteSearchIdx - data.num;
					}
				}
				
				// 削除予約を削除する
				for (long delTime : deleteList) {
					vector<DeleteReservation>::iterator it = deleteReservationList.begin();
					while (it != deleteReservationList.end()) {
						if (it->deleteMsec == delTime) {
							it = deleteReservationList.erase(it);
						}
						else ++it;
					}
				}
			}
		}
		pthread_mutex_unlock(&queue_mtx);
		//REL_COMMENT logger->trace("[deleteQueue] Name:" + streamName + " MUTEX queue_mtx unlock.");

		if ((totalQueueNum - beforeQueueNum) != 0 || (queueSize - queue.size()) != 0) {

			logger->trace("[deleteQueue] Name:" + this->streamName + " totalQueue:" + std::to_string(totalQueueNum) + " incremental(perDel):" + std::to_string(totalQueueNum - beforeQueueNum) + " deleteNum:" + std::to_string(queueSize - queue.size()));

		}
		this->streamRcvNum = totalQueueNum - beforeQueueNum;
		beforeQueueNum = totalQueueNum;
		logger->trace("[deleteQueue] OUT. Name:" + streamName + " deleteNum:" + std::to_string(queueSize - queue.size()) + " delete time:[" + std::to_string((DmUtil::getTimeMicrosec() - startDelTime) / 1000.0) + "]");

	}

	/**
	* ストリーム受信数を取得する
	*
	* @author	Nagoya University
	* @date	2018/10/29
	*
	* @return	ストリーム受信数
	*/
	int Queue::getStreamRcvNum()
	{
		//REL_COMMENT logger->trace("[getStreamRcvNum] Name:" + streamName + " IN");
		int num = 0;
		try {
			num = this->streamRcvNum;
		}
		catch (exception& e) {
			string what(e.what());
			logger->error("[getStreamRcvNum]: " + what);
		}

		//REL_COMMENT logger->trace("[getStreamRcvNum] Name:" + streamName + " OUT");
		return num;
	}

	/**
	* タイマー時間を設定する
	*
	* @author	Nagoya University
	* @date	2019/01/31
	*
	* @param	user			ユーザ名
	* @param	timer_msec		実行間隔
	* @param	start_fix_msec	開始調整時間
	* @param	schema			スキーマ
	*/
	void Queue::settingTimer(const string &user, const long timer_msec, const long start_fix_msec, Schema schema)
	{
		this->createUser = user;
		// タイマーを設定する
		if (timerMode == 0) {
			this->timerStreamThread = new TimerThread(std::bind(&Queue::createTimerStreamData, this, schema), start_fix_msec, timer_msec);
			this->timerStreamThread->start();
		} else {
			this->timerStreamWorker = new TimerWorker(timer_msec, std::bind(&Queue::createTimerStreamData, this, schema));
		}
	}

	/**
	* タイマーストリームのデータを生成し、PUSHする
	*
	* @author	Nagoya University
	* @date	2019/01/31
	*
	*/
	void Queue::createTimerStreamData(Schema schema)
	{
		long now = DmUtil::getTimeMillisec();
		vector<Tuple> tuples;
		Tuple tuple(schema);
		tuple.setValue(0, now, now);
		int userIdx = schema.getAttributeIdx(Schema::COL_USERID);
		tuple.setValue(userIdx, this->createUser, now);
		int ipIdx   = schema.getAttributeIdx(Schema::COL_IPADDR);
		tuple.setValue(ipIdx, "127.0.0.1", now);
		int hashIdx = schema.getAttributeIdx(Schema::COL_HASH);
		std::hash<std::string> hasher;
		size_t hash1 = hasher(to_string(now));
		tuple.setValue(hashIdx, to_string(hash1), now);
		int tsGrpIdx = schema.getAttributeIdx(Schema::COL_TS_GRP);
		tuple.setValue(tsGrpIdx, "", now);

		tuples.push_back(tuple);
		pushTuple(this->createUser, tuples);
	}

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	deleteMsec	削除時間
	 * @param	index	  	削除個数
	 */

	DeleteReservation::DeleteReservation(long deleteMsec, int index)
	{
		this->deleteMsec = deleteMsec;
		this->num = index;
	}
}
