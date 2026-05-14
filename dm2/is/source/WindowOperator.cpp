#include "WindowOperator.h"

using namespace std;

namespace IS {

	/**
	 * コンストラクタ（個数Window）
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param 	mngId	クエリ管理番号
	 * @param	masterName	マスター句に指定されたストリーム名
	 * @param	streamName	ストリーム名
	 * @param	schemaName	スキーマ名(一時ストリーム使用時のみ指定)
	 * @param	num		  	取得個数
	 * @param	limit	  	絞込み個数
	 */

	WindowOperator::WindowOperator(unsigned int mngId, string masterName, string streamName, string schemaName, int num, int _limit, string seed, vector<int> &_partitionList)
	{
		this->num = num;
		this->limit = _limit;
		this->costId = seed;
		if (limit > 0) {
			CostSortMode = true;
		}
		this->partitionList.resize(_partitionList.size());
		std::copy(_partitionList.begin(), _partitionList.end(), this->partitionList.begin());
		if (getNumPartitionSize() > 0) doPartition = true;
		init(mngId, masterName, streamName, schemaName, "WINDOW[num]:" + std::to_string(num));

	}

	/**
	 * コンストラクタ（時間Window）
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param 	mngId	クエリ管理番号
	 * @param	masterName	マスター句に指定されたストリーム名
	 * @param	streamName	ストリーム名
	 * @param	schemaName	スキーマ名(一時ストリーム使用時のみ指定)
	 * @param	msec	  	取得時間範囲
	 */

	WindowOperator::WindowOperator(unsigned int mngId, string masterName, string streamName, string schemaName, long msec, vector<int> &_partitionList)
	{
		this->msec = msec;
		this->partitionList.resize(_partitionList.size());
		std::copy(_partitionList.begin(), _partitionList.end(), this->partitionList.begin());
		if (getNumPartitionSize() > 0) doPartition = true;
		init(mngId, masterName, streamName, schemaName, "WINDOW[time]:" + std::to_string(msec) + " ALLOW_DUPLICATE:" + std::to_string(allowDuplicateData));
	}

	/**
	* コンストラクタ（時間Window＋重複データ許容指定）
	*
	* @author	Nagoya University
	* @date	2019/01/30
	*
	* @param 	mngId	クエリ管理番号
	* @param	masterName	マスター句に指定されたストリーム名
	* @param	streamName	ストリーム名
	* @param	schemaName	スキーマ名(一時ストリーム使用時のみ指定)
	* @param	msec	  	取得時間範囲
	* @param	allowDuplicateData	  	重複したデータの通知を許容するか
	*/

	WindowOperator::WindowOperator(unsigned int mngId, string masterName, string streamName, string schemaName, long msec, bool allowDuplicateData)
	{
		this->msec = msec;
		this->allowDuplicateData = allowDuplicateData;
		init(mngId, masterName, streamName, schemaName, "WINDOW[time]:" + std::to_string(msec) + " ALLOW_DUPLICATE:" + std::to_string(allowDuplicateData));
	}

	/**
	* コンストラクタ（受信したデータ全通知）
	*
	* @author	Nagoya University
	* @date	2018/08/27
	*
	* @param 	mngId	クエリ管理番号
	* @param	masterName	マスター句に指定されたストリーム名
	* @param	streamName	ストリーム名
	* @param	schemaName	スキーマ名(一時ストリーム使用時のみ指定)
	*/

	WindowOperator::WindowOperator(unsigned int mngId, string masterName, string streamName, string schemaName)
	{
		notifyAllFlg = true;
		init(mngId, masterName, streamName, schemaName, "WINDOW[-]:notifyAll ");
	}
	/**
	 * 共通の初期処理
	 *
	 * @author	Shinichi Kusayama
	 * @date	2023/12/06
	 */
	void WindowOperator::init(unsigned int mngId, string masterName, string streamName, string schemaName, const string _argument)
	{
		this->type = MyName + "_MNGID:" + std::to_string(mngId);
		this->mngId = mngId;
		this->masterName = masterName;
		this->streamName = streamName;
		this->schemaName = schemaName;
		expect_data_size = 1;
		IS::QueueManager &QM = IS::QueueManager::get_instance();

		if (masterName.length() > 0) {
			// マスター句に指定されたトリガーとなるストリームにリスナーを登録
			QM.addQueueListener(masterName, this);
			this->argument.append("MASTER:" + masterName + " ");
		}

		if (schemaName.length() > 0) {
			// 一時ストリームを作成
			QM.createTempQueue(streamName, schemaName);
			this->argument.append("TEMP_STRM:" + streamName + " as " + schemaName + " ");
		}
		QM.addQueueReferrer(streamName, this);

		this->argument.append("STREAM:" + streamName + " ");
		this->argument.append(_argument);
		bootTime = DmUtil::getTimeMillisec();
		
		logger->debug("[" + this->type + "]" + _argument + ", bootTime : " + std::to_string(bootTime));
		
		pthread_mutex_init(&hashmap_mtx, NULL);
		int deleteHashmapMillisec = defaultDeleteHashSec * 1000;		// 10秒
		// 削除タイマーを設定する
		this->deleteHashmapWorker = new TimerWorker(deleteHashmapMillisec, std::bind(&WindowOperator::deleteHashmap, this));

	}

	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	WindowOperator::~WindowOperator()
	{
		if (this->deleteHashmapWorker != NULL) {
			this->deleteHashmapWorker->abort_thread();
			delete deleteHashmapWorker;
			deleteHashmapWorker = NULL;
		}
		pthread_mutex_destroy(&hashmap_mtx);

		IS::QueueManager &QM = IS::QueueManager::get_instance();
		QM.removeQueueListener(masterName, this);
		QM.removeQueueReferrer(streamName, this);
		if (this->schemaName.length() > 0) {
			// 一時ストリームを削除
			QM.removeQueue(this->streamName);
		}
		if (processNum != 0) {
			logger->debug("[PERFORMANCE_STAT][" + getType() + "] NotifiedIntervalAvgTime:" + to_string(notifiedIntervalAVG) + "[ms] procNum:" + to_string(processNum));
		}
	}
	/**
	* ハッシュマップを削除する
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/06
	*
	*/
	void WindowOperator::deleteHashmap()
	{
		long delTime = DmUtil::getTimeMicrosec() - defaultDeleteHashSec * 1000 * 1000;
		vector<string> delList;
		pthread_mutex_lock(&hashmap_mtx);
		for (auto itr = tupleHashMap.begin(); itr != tupleHashMap.end(); ++itr) {
			if (itr->second < delTime) {
				delList.push_back(itr->first);
			}
		}
		for (string del: delList) {
			tupleHashMap.erase(del);
		}
		pthread_mutex_unlock(&hashmap_mtx);
	}

	/**
	* 取得先のストリーム名を取得する
	*
	* @author	Nagoya University
	* @date	2018/03/14
	*
	* @return	取得先のストリーム名
	*/

	string WindowOperator::getTargetStreamName()
	{
		return this->streamName;
	}

	/**
	 * 時間Windowのサイズを取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @return	時間Windowの時間の値
	 */

	long WindowOperator::getTimeWindowSize()
	{
		return msec;
	}

	/**
	* 通知済み時間を取得する
	*
	* @author	Nagoya University
	* @date	2019/01/23
	*
	* @return	個数Windowの個数の値
	*/

	long  WindowOperator::getNotifiedTime()
	{
		return notifiedTime;
	}

	/**
	* 通知済み時間を更新する
	* 通知済みデータを許容している場合は更新しない
	*
	* @param	notifiedTime	通知済み時間
	*
	* @author	Nagoya University
	* @date	2019/01/23
	*
	*/

	void WindowOperator::setNotifiedTime(const long notifiedTime)
	{
		if (allowDuplicateData) return;
		this->notifiedTime = notifiedTime;
	}

	/**
	* 重複通知の設定を更新する
	*
	* @param	allowDuplicateData	重複通知可否
	*
	* @author	Nagoya University
	* @date	2019/05/16
	*
	*/

	void WindowOperator::setAllowDuplicateData(const bool allowDuplicateData)
	{
		this->allowDuplicateData = allowDuplicateData;
	}

	/**
	 * 個数Windowのサイズを取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @return	個数Windowの個数の値
	 */

	int  WindowOperator::getNumWindowSize()
	{
		return num;
	}

	/**
	 * Partitionのサイズを取得する
	 *
	 * @author	Shinichi Kusayama
	 * @date	2025/08/12
	 *
	 * @return	Partitionの個数の値
	 */

	int  WindowOperator::getNumPartitionSize()
	{
		return partitionList.size();
	}

	/**
	 * 前回受信時刻を取得する
	 *
	 * @author	Shinichi Kusayama
	 * @date	2025/08/12
	 *
	 * @return	受信時刻
	 */

	long  WindowOperator::getLastRecvTime()
	{
		return lastRecvTime;
	}

	/**
	 * 前回受信時刻をセットする
	 *
	 * @author	Shinichi Kusayama
	 * @date	2025/08/12
	 *
	 */

	void  WindowOperator::setLastRecvTime(long recvTime)
	{
		this->preLastRecvTime = this->lastRecvTime;
		this->lastRecvTime = recvTime;
		return;
	}

	/**
	 * Partitionのリストを取得する
	 *
	 * @author	Shinichi Kusayama
	 * @date	2025/08/12
	 *
	 * @return	Partitionのリスト
	 */

	vector<int>  WindowOperator::getPartitionList()
	{
		return partitionList;
	}

	/**
	* 処理開始間隔平均時間を取得する
	*
	* @author	Nagoya University
	* @date	2019/10/29
	*
	* @return	処理開始間隔平均時間
	*/

	double  WindowOperator::getNotifiedIntervalAVG()
	{
		return notifiedIntervalAVG;
	}

	/**
	* 全通知かを取得する
	*
	* @author	Nagoya University
	* @date	2018/08/28
	*
	* @return	全通知可否
	*/

	bool  WindowOperator::isNotifyAll()
	{
		return notifyAllFlg;
	}

	/**
	 * 子オペレータからデータが要求された場合にコールされる
	 * WindowOperatorは必ず親Operatorが存在しないため、Queueから取得する処理を実施する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	void WindowOperator::requestData(long referenceTime)
	{
		// windowオペレータには必ずトリガーとなるmasterが設定されるためリクエストをする必要がない（ただしリカーシブ時などは特殊で必要になるケースもあるため一旦コメント）
		//REL_COMMENT logger->trace("[" + this->type + "][requestData] IN");
		// Queueからデータを取得する
		IS::QueueManager &QM = IS::QueueManager::get_instance();
		TupleSet ts;
		ts.info.type = TupleSet::OccurrenceType::pull;
		ts.info.recvTime = referenceTime;

		// キュー取得時はmsec時間で取得（受信時間はマイクロ秒）
		referenceTime = referenceTime / 1000;

		if (this->num > 0) {
			QM.getTupleSet(this->streamName, num, referenceTime, ts);
		}
		else {
			logger->trace("[" + this->type + "][requestData] stream:" + this->streamName + " msec:" + to_string(msec) + " notifiedTime:" + to_string(notifiedTime) + " referenceTime:" + to_string(referenceTime));
			// 再帰処理時の時間WINDOWは重複データを許容するため通知済み時間を0とする
			QM.getTupleSet(this->streamName, msec, 0, referenceTime, ts);
		}
		// 通知する(取得件数が0でも待ちデータ削除のため通知する必要あり)
		Operator::receiveTupleSet(ts);

		if (ts.size() == 0) {
			//REL_COMMENT logger->trace("[" + this->type + "] ts.size() == 0 so not Notify Operator. Time : " + DmUtil::getTimeMillisecStr());
		}
		//REL_COMMENT logger->trace("[" + this->type + "][requestData] OUT");
		
	}
	
	/**
	* オペレータ処理
	*
	* @author	Shinichi Kusayama
	* @date	2023/12/09
	*
	*/
	void WindowOperator::duplicateMessage() {
		duplicate_cnt++;
		string msg = "[" + this->type + "] Duplicate Data : No." + to_string(duplicate_cnt) + ")";
		logger->debug(msg);
		if (duplicate_cnt % 100000 == 1) {
			logger->info(msg);
		}
		if (duplicate_cnt == std::numeric_limits<long>::max()) duplicate_cnt = 0;
	}
	
	/**
	* キーとなる行取得
	*
	* @author	Shinichi Kusayama
	* @date	2024/03/01
	*
	* @param [in]	t	タプル
	*
	* @return	取得できたか否かか
	*/
	bool WindowOperator::getKeyLine(Tuple &t) {
		bool getKey = false;
		string id = t.getStringValueByIdx(id_idx);
		if (id == costId) {
			keyX[id] = t.getIntValueByIdx(x_idx) / (double)10000000;
			keyY[id] = t.getIntValueByIdx(y_idx) / (double)10000000;
			keyZ[id] = t.getIntValueByIdx(z_idx) / (double)100;
			setKeyLine = true;
			getKey = true;
		}
		return getKey;
	}
	
	/**
	* コスト取得
	*
	* @author	Shinichi Kusayama
	* @date	2024/03/01
	*
	* @param [in]	t	タプル
	*/
	void WindowOperator::getCost(Tuple &t) {
		double x = t.getIntValueByIdx(x_idx) / (double)10000000;
		double y = t.getIntValueByIdx(y_idx) / (double)10000000;
		double z = t.getIntValueByIdx(z_idx) / (double)100;	
		double distance = DmUtil::get3DDistance(keyX[costId], keyY[costId], keyZ[costId], x, y, z);
		t.setCost(distance);
		//cout << "id:" << t.getStringValueByIdx(id_idx) << ",distance:" << distance << ",[x1,y1,z1,z2,y2,z2]:" << keyX[costId] << "," << keyY[costId] << "," << keyZ[costId] << "," << x << "," << y << "," << z << endl;
	}
	
	/**
	* ハッシュ値による判定（重複除去用）
	*
	* @author	Shinichi Kusayama
	* @date	2025/08/13
	*
	* @param [in]	t	タプル
	*
	* @return	判定結果
	*/
	bool WindowOperator::matchHash(const Tuple &t, const Schema &schema) {
		bool match = false;
		any hashval;
		t.getValueByIdx(schema.getAttributeIdx(Schema::COL_HASH), hashval);
		string hashStr = stringUtil.getAnyString(hashval);
		//cout << "hashStr:" << hashStr << endl;
		pthread_mutex_lock(&hashmap_mtx);
		if (tupleHashMap.find(hashStr) == tupleHashMap.end()) {
			tupleHashMap[hashStr] = DmUtil::getTimeMicrosec();
		} else {
			match = true;
		}
		pthread_mutex_unlock(&hashmap_mtx);
		if (match) {
			duplicateMessage();
		}
		return match;
	}
	/*
	* PARTITIONキー生成
	*
	* @author	Shinichi Kusayama
	* @date	2025/08/13
	*
	* @param [in]	t	タプル
	*
	* @return	キー
	*/
	string WindowOperator::makePartitionKey(const Tuple &t) {
		string keyGroup;
		for (int colIdx : partitionList) {
			any key;
			t.getValueByIdx(colIdx, key);
			string keyStr = stringUtil.getAnyString(key);
			keyGroup += keyStr + "|";
		}
		return keyGroup;
	}
	/**
	* オペレータ処理
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param [in,out]	ts	タプルセット
	*
	* @return	正常にデータ処理を実施でき、次のオペレータに渡す際にtrue
	*/

	bool WindowOperator::process(vector<IS::TupleSet>& ts)
	{
		logger->debug("[" + this->type + "] ========== WindowOperator START ========== ");
		bool ret = false;
		ts.at(0).info.orgAttributeSize = ts.at(0).getSchemaRef().getAttributeSize();
		ts.at(0).info.orgTableName = ts.at(0).getSchemaRef().getTableName();
#if MEASURE_MODE == 1
		long startTime = DmUtil::getTimeMicrosec();
		long procTime = startTime;
		int step = 1;
		double msec = (startTime - ts.at(0).info.recvTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " queuing to fire windowOP processing time: " + to_string(msec) + "[ms]");
#endif
		if (lastNotifiedTime == 0) {
			lastNotifiedTime = ts.at(0).info.recvTime;
		}
		else {
			double intervalTime = (ts.at(0).info.recvTime - lastNotifiedTime) / 1000.0;
			notifiedIntervalAVG = (double)((notifiedIntervalAVG * (processNum - 1)) + intervalTime) / (double)(processNum);
			logger->debug("[PERFORMANCE][" + getType() + "] intervalTime:" + to_string(intervalTime) + "[ms] Avg:" + to_string(notifiedIntervalAVG) + "[ms] last:" + to_string(lastNotifiedTime) + "[ms] new:" + to_string(ts.at(0).info.recvTime) + "[ms] procNum:" + to_string(processNum + 1));
			lastNotifiedTime = ts.at(0).info.recvTime;
		}

		// DEBUG 与えられたタプル情報の出力
		printInputInfos(ts, this->argument);

#if MEASURE_MODE == 1
		long now = DmUtil::getTimeMicrosec();
		msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " parameter analytics processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif
		Schema schema = ts.at(0).getSchema();
		if (doPartition) {
			//ret = proc_by_partition(ts, schema);
			if (getNumWindowSize() == 1) {
				ret = proc_by_partition_create_time_rows1(ts, schema);
			} else {
				ret = proc_by_partition_create_time_rowsn(ts, schema);
			}
		} else {
			if (CostSortMode) {
				if (schema.getTableName() == "object_info_0_8_1") {
					ret = proc_by_cost_sort(ts, schema);
				}
			} else {
				ret = proc_normal(ts, schema);
			}
		}

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - startTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " total processing time: " + to_string(msec) + "[ms]");
#endif

		logger->debug("[" + this->type + "] ========== WindowOperator  END  ========== ");
		return ret;
	}
	/**
	* PARTITION BY 用オペレータ処理
	*
	* @author	Shinichi Kusayama
	* @date	2025/08/13
	*
	* @param [in,out]	ts	タプルセット
	* @param [in,out]	schema	スキーマ
	*
	* @return	正常にデータ処理を実施でき、次のオペレータに渡す際にtrue
	*/

	bool WindowOperator::proc_by_partition(vector<IS::TupleSet>& ts, const Schema &schema)
	{
		TupleSet& tupleset1 = ts.at(0);
		int WindowNumSize = getNumWindowSize();
		if (WindowNumSize > 0) {
			IS::TupleSet retTupleSet;
			retTupleSet.info = tupleset1.info;
			retTupleSet.SetSchema(schema);
			int addNum = 0;
			int tupleInputSize = tupleset1.size();
			retTupleSet.resize(tupleInputSize);		// PARTITION BY用にタプル入力元のサイズに合わせる
			//cout << "schema:" << schema.getTableName() << ",tupleInputSize:" << tupleInputSize << ",allowDuplicateData:" << this->allowDuplicateData << endl;
			for (int i = 0; i < tupleInputSize; i++)
			{
				Tuple t = tupleset1.getTuple(i);
				if (this->allowDuplicateData == false) {
					// ハッシュ値による重複除去
					if (matchHash(t, schema)) continue;
				}
				retTupleSet.setTuple(addNum, t);
				addNum++;
			}
			if(tupleInputSize != addNum) retTupleSet.resize(addNum);
			retTupleSet.reverse();
			for (unsigned int i = 0; i < ts.size(); i++) {
				ts.at(i).clear();
			}
			ts.clear();
			ts.reserve(1);
			ts.push_back(retTupleSet);
			retTupleSet.clear();
		} else {
			tupleset1.reverse();
		}
		printOutputInfo(tupleset1);
		return true;
	}
	/**
	* PARTITION BY 用オペレータ処理（生成時刻比較 ROWS 1用）
	*
	* @author	Shinichi Kusayama
	* @date	2025/08/13
	*
	* @param [in,out]	ts	タプルセット
	* @param [in,out]	schema	スキーマ
	*
	* @return	正常にデータ処理を実施でき、次のオペレータに渡す際にtrue
	*/

	bool WindowOperator::proc_by_partition_create_time_rows1(vector<IS::TupleSet>& ts, const Schema &schema)
	{
		TupleSet& tupleset1 = ts.at(0);
		int tupleInputSize = tupleset1.size();
		int maxTupleSize = tupleInputSize + latestTuple1.size();
		int WindowNumSize = getNumWindowSize();
		//cout << "latestTuple1:" << latestTuple1.size() << ", tupleInputSize:" << tupleInputSize << endl;
		if (WindowNumSize > 0) {
			IS::TupleSet retTupleSet;
			retTupleSet.info = tupleset1.info;
			retTupleSet.SetSchema(schema);
			int addNum = 0;
			retTupleSet.resize(maxTupleSize);
			
			//cout << "schema:" << schema.getTableName() << ",tupleInputSize:" << tupleInputSize << ",allowDuplicateData:" << this->allowDuplicateData << endl;
			for (int i = 0; i < tupleInputSize; i++)
			{
				Tuple t = tupleset1.getTuple(i);
				if (this->allowDuplicateData == false) {
					// ハッシュ値による重複除去
					if (matchHash(t, schema)) continue;
				}
				string key = makePartitionKey(t);
				auto it = latestTuple1.find(key);
				if (it == latestTuple1.end()) {
					latestTuple1[key] = t;
				} else {
					if (t.getTimestamp() > it->second.getTimestamp()) {
						it->second = t;
					}
				}
			}
			for (unsigned int i = 0; i < ts.size(); i++) {
				ts.at(i).clear();
			}
			ts.clear();
			//cout << "[After]latestTuple1:" << latestTuple1.size() << endl;
			for (auto &kv : latestTuple1) {
				if (this->allowDuplicateData == false) {
					if (kv.second.getRecvTime() <= this->preLastRecvTime) continue;
				}
				retTupleSet.setTuple(addNum, kv.second);
				addNum++;
			}
			if(maxTupleSize != addNum) retTupleSet.resize(addNum);
			retTupleSet.reverse();
			for (unsigned int i = 0; i < ts.size(); i++) {
				ts.at(i).clear();
			}
			ts.clear();
			ts.reserve(1);
			ts.push_back(retTupleSet);
			retTupleSet.clear();
			if (addNum == 0) return false;
		} else {
			tupleset1.reverse();
		}
		printOutputInfo(tupleset1);
		return true;
	}

	/**
	* PARTITION BY 用オペレータ処理（生成時刻比較 ROWS n用）
	*
	* @author	Shinichi Kusayama
	* @date	2025/08/13
	*
	* @param [in,out]	ts	タプルセット
	* @param [in,out]	schema	スキーマ
	*
	* @return	正常にデータ処理を実施でき、次のオペレータに渡す際にtrue
	*/

	bool WindowOperator::proc_by_partition_create_time_rowsn(vector<IS::TupleSet>& ts, const Schema &schema)
	{
		TupleSet& tupleset1 = ts.at(0);
		int tupleInputSize = tupleset1.size();
		int WindowNumSize = getNumWindowSize();
		int sumlatestTupleN = 0;
		for (auto &dq : latestTupleN) {
			sumlatestTupleN += dq.second.size();
		}
		int maxTupleSize = tupleInputSize + sumlatestTupleN;
		//cout << "latestTupleN:" << sumlatestTupleN << ", tupleInputSize:" << tupleInputSize << endl;
		if (WindowNumSize > 0) {
			IS::TupleSet retTupleSet;
			retTupleSet.info = tupleset1.info;
			retTupleSet.SetSchema(schema);
			int addNum = 0;
			retTupleSet.resize(maxTupleSize);
			
			//cout << "schema:" << schema.getTableName() << ",tupleInputSize:" << tupleInputSize << ",allowDuplicateData:" << this->allowDuplicateData << endl;
			for (int i = 0; i < tupleInputSize; i++)
			{
				Tuple t = tupleset1.getTuple(i);
				if (this->allowDuplicateData == false) {
					// ハッシュ値による重複除去
					if (matchHash(t, schema)) continue;
				}
				string key = makePartitionKey(t);
				auto &dq = latestTupleN[key];
				dq.push_back(t);
				// Timestamp昇順に並べる
				std::sort(dq.begin(), dq.end(),
						[](const Tuple &a, const Tuple &b) {
							return a.getTimestamp() < b.getTimestamp();
						});
				// 件数制限（古い方を削る）
				while ((int)dq.size() > WindowNumSize) {
					dq.pop_front();
				}
			}
			for (unsigned int i = 0; i < ts.size(); i++) {
				ts.at(i).clear();
			}
			ts.clear();
			for (auto &dq : latestTupleN) {
				for (Tuple &t : dq.second) {
					if (this->allowDuplicateData == false) {
						if (t.getRecvTime() <= this->preLastRecvTime) continue;
					}
					retTupleSet.setTuple(addNum, t);
					addNum++;
				}
			}
			if(maxTupleSize != addNum) retTupleSet.resize(addNum);
			retTupleSet.reverse();
			for (unsigned int i = 0; i < ts.size(); i++) {
				ts.at(i).clear();
			}
			ts.clear();
			ts.reserve(1);
			ts.push_back(retTupleSet);
			retTupleSet.clear();
			if (addNum == 0) return false;
		} else {
			tupleset1.reverse();
		}
		printOutputInfo(tupleset1);
		return true;
	}


	/**
	* COST SORT 用オペレータ処理
	*
	* @author	Shinichi Kusayama
	* @date	2025/08/13
	*
	* @param [in,out]	ts	タプルセット
	* @param [in,out]	schema	スキーマ
	*
	* @return	正常にデータ処理を実施でき、次のオペレータに渡す際にtrue
	*/

	bool WindowOperator::proc_by_cost_sort(vector<IS::TupleSet>& ts, const Schema &schema)
	{
		TupleSet& tupleset1 = ts.at(0);
		int WindowNumSize = getNumWindowSize();
		if (WindowNumSize > 0) {
			IS::TupleSet retTupleSet;
			retTupleSet.info = tupleset1.info;
			retTupleSet.SetSchema(schema);
			int addNum = 0;
			int tupleInputSize = tupleset1.size();
			retTupleSet.resize(WindowNumSize);
			
			//cout << "schema:" << schema.getTableName() << ",tupleInputSize:" << tupleInputSize << ",allowDuplicateData:" << this->allowDuplicateData << endl;
			//long now = DmUtil::getTimeMillisec();
			if (setKeyLine) {
				for (int i = 0; i < tupleInputSize; i++)
				{
					Tuple t = tupleset1.getTuple(i);
					getCost(t);
					bool isKeyLine = getKeyLine(t);
					if (this->allowDuplicateData == false) {
						// ハッシュ値による重複除去
						if (matchHash(t, schema)) continue;
					}
					if (!isKeyLine) {
						retTupleSet.setTuple(addNum, t);
						addNum++;
					}
					if (addNum >= WindowNumSize) break;
				}
				if (WindowNumSize != addNum) retTupleSet.resize(addNum);
				if (addNum > limit) {
					retTupleSet.sortByCost();
					retTupleSet.resize(limit);
				}
				//retTupleSet.debugKey();
			} else {
				for (int i = 0; i < tupleInputSize; i++)
				{
					if (i >= WindowNumSize) break;
					Tuple t = tupleset1.getTuple(i);
					getKeyLine(t);
					if (this->allowDuplicateData == false) {
						// ハッシュ値による重複除去
						if (matchHash(t, schema)) continue;
					}
					retTupleSet.setTuple(addNum, t);
					addNum++;
				}
				if(WindowNumSize != addNum) retTupleSet.resize(addNum);
			}
			retTupleSet.reverse();
			for (unsigned int i = 0; i < ts.size(); i++) {
				ts.at(i).clear();
			}
			ts.clear();
			ts.reserve(1);
			ts.push_back(retTupleSet);
			retTupleSet.clear();
		} else {
			tupleset1.reverse();
		}
		printOutputInfo(tupleset1);
		return true;
	}
	/**
	* 通常オペレータ処理
	*
	* @author	Shinichi Kusayama
	* @date	2025/08/13
	*
	* @param [in,out]	ts	タプルセット
	* @param [in,out]	schema	スキーマ
	*
	* @return	正常にデータ処理を実施でき、次のオペレータに渡す際にtrue
	*/

	bool WindowOperator::proc_normal(vector<IS::TupleSet>& ts, const Schema &schema)
	{
		TupleSet& tupleset1 = ts.at(0);
		int WindowNumSize = getNumWindowSize();
		if (WindowNumSize > 0) {
			IS::TupleSet retTupleSet;
			retTupleSet.info = tupleset1.info;
			retTupleSet.SetSchema(schema);
			int addNum = 0;
			int tupleInputSize = tupleset1.size();
			retTupleSet.resize(WindowNumSize);
			//cout << "schema:" << schema.getTableName() << ",tupleInputSize:" << tupleInputSize << ",allowDuplicateData:" << this->allowDuplicateData << endl;
			//long now = DmUtil::getTimeMillisec();
			
			for (int i = 0; i < tupleInputSize; i++)
			{
				if (i >= WindowNumSize) break;
				Tuple t = tupleset1.getTuple(i);
				if (this->allowDuplicateData == false) {
					// ハッシュ値による重複除去
					if (matchHash(t, schema)) continue;
				}
				retTupleSet.setTuple(addNum, t);
				addNum++;
			}
			if(WindowNumSize != addNum) retTupleSet.resize(addNum);
			retTupleSet.reverse();
			for (unsigned int i = 0; i < ts.size(); i++) {
				ts.at(i).clear();
			}
			ts.clear();
			ts.reserve(1);
			ts.push_back(retTupleSet);
			retTupleSet.clear();
		} else {
			tupleset1.reverse();
		}
		printOutputInfo(tupleset1);
		return true;
	}
}
