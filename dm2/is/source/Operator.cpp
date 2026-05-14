#include "Operator.h"
#include "ResponseOperator.h"


using namespace std;

namespace IS {

    /**
     * コンストラクタ
     *
     * @author	Nagoya University
     * @date	2018/03/13
     */

    Operator::Operator()
    {
		pthread_mutex_init(&bufMtx, NULL);
		pthread_mutex_init(&inputMtx, NULL);
		pthread_mutex_init(&outputMtx, NULL);
		operationReady = false;
		this->pid = DmUtil::getPID();
    }

    /**
     * デストラクタ
     *
     * @author	Nagoya University
     * @date	2018/03/13
     */

    Operator::~Operator()
    {
		unsigned int cnt = 0;
		// Operatorを起動済み、かつスレッド処理が終了していない場合はwaitする
		while (!exitReady && start_flag) { 
			usleep(1 * 1000);
			cnt++;
			if (cnt > 300) {
				logger->warn("[" + getType() + "] destractor Timeout. This operator has no response, so forcibly terminated. exitReady:" + to_string(exitReady) + ",start_flag:" + to_string(start_flag) + ",processNumSum:" + to_string(processNumSum));
				break;
			}
		};
		if (processNumSum != 0) {
			pthread_mutex_lock(&bufMtx);
			int bufsize = buffer.size();
			buffer.clear();
			buffer.shrink_to_fit();
			pthread_mutex_unlock(&bufMtx);
			//logger->debug("[PERFORMANCE_STAT][" + getType() + "] ProcAvgTime:" + to_string(processTimeAVG) + "[ms]  ProcMaxTime:" + to_string(processTimeMax) + "[ms]  ProcMinTime:" + to_string(processTimeMin) + "[ms] procNum:" + to_string(processNum) + " remainingQueueNum:" + to_string(bufsize) + " inputSum:" + to_string(inputSum) + " outputSum:" + to_string(outputSum) + " getTimeAVG:" + to_string(getTimeAVG) + "[ms] fireTimeAVG:" + to_string(fireTimeAVG) + "[ms] allTimeAVG:" + to_string(allTimeAVG) + "[ms]");
		}
		pthread_mutex_destroy(&bufMtx);
		pthread_mutex_destroy(&inputMtx);
		pthread_mutex_destroy(&outputMtx);
    }

	/**
	 * タプルの受け取り処理
	 * 他オペレータからコールされ、自身に渡されたタプルセットをキューイングする
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	ts	   	タプルセット
	 */

	void Operator::receiveTupleSet(TupleSet &ts)
	{
		// キューイング
		if (ts.getSchemaRef().getTableName() != Schema::UNKNOWN) {
			//REL_COMMENT logger->trace("[" + getType() + "] [receiveTupleSet] MUTEX bufMtx lock");
			pthread_mutex_lock(&bufMtx);
			buffer.push_back(ts);
			logger->debug("[" + getType() + "] [receiveTupleSet] buffer.push_back[type:" + to_string((int)ts.info.type) + "] bufsize:" + std::to_string(buffer.size()) + " info.recvTime:" + to_string(ts.info.recvTime));
			pthread_mutex_unlock(&bufMtx);
			//REL_COMMENT logger->trace("[" + getType() + "] [receiveTupleSet] MUTEX bufMtx unlock");
		}
		else {
			if (expect_data_size != 0) {
				logger->error("[" + getType() + "] [receiveTupleSet] Input TableName is UNKNOWN");
			}
		}
		callerObj = ts.fromOperator;

		notify_one();
    }

	/**
	 * 子オペレータからデータが要求された場合にコールされる処理。
	 * requestParentProc()をコールする事でさらに上位の親が存在するかを確認し
	 * 存在しない場合はtrueが返却され、自オペレータを起こす
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void Operator::requestData(long referenceTime)
	{
		//REL_COMMENT logger->trace("[" + getType() + "][requestData] IN");
		if (requestParentProc(callerObj, referenceTime))
		{
			notify_one();
		}
		//REL_COMMENT logger->trace("[" + getType() + "][requestData] OUT");
	}

	/**
	 * 自オペレータを起こす
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	inline void Operator::notify_one()
	{
		//REL_COMMENT logger->trace("[" + getType() + "] [notify_one] IN");
		{   // ★ここの括弧はdataReadyLockの有効範囲を示す            
			// 同期(処理中のキューイングを避ける：１回ずつ処理する)
			// ★処理中にもキューイングする場合は要検討（継承したオペレータで実装すること）
			//std::lock_guard<std::mutex> dataReadyLock(mtx);
			// ★現状Operatorのデータ操作中に別オペレータからのキューイング時にブロックされる事を避けるため
			//   データ操作中でもキューイングされる事を許容する
			if (data_ready == false) {
				data_ready = true;
			}
			
		}
		//REL_COMMENT logger->trace("[" + getType() + "] [notify_one] dataReadyLock break");
		// 自オペレータを起こす
		cond.notify_one();
		//logger->debug("[" + getType() + "] [notify_one] notify_one called ");
		//REL_COMMENT logger->trace("[" + getType() + "] [notify_one] OUT");
	}

	/**
	* タプルセットを取得する
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param [in,out]	ts	タプルセット
	*/

	inline long Operator::getTupleSet(std::vector<TupleSet> &ts)
	{
		long totalSize = 0;
		//REL_COMMENT logger->trace("[" + getType() + "] getTupleSet IN");
		//REL_COMMENT logger->trace("[" + getType() + "] [getTupleSet] MUTEX bufMtx lock. Size:" + to_string(buffer.size()));
		pthread_mutex_lock(&bufMtx);
		// オペレータの処理に必要なデータを取得してくる
		while (!buffer.empty())
		{
			ts.push_back(buffer.front());
			totalSize += buffer.front().size();
			buffer.front().clear();
			buffer.pop_front();
			if (ts.size() == expect_data_size) {
				buffer.shrink_to_fit();
				if (buffer.empty()) {
					// バッファが空になった場合は確保されているメモリを解放するために空バッファと入れ替える(メモリ使用量肥大化抑止対策)
					deque<TupleSet> tmp;
					buffer.swap(tmp);
				}
				break;
			}
		}
		pthread_mutex_unlock(&bufMtx);
		//REL_COMMENT logger->trace("[" + getType() + "] [getTupleSet] MUTEX bufMtx unlock Size:" + to_string(buffer.size()));
		//REL_COMMENT logger->trace("[" + getType() + "] getTupleSet OUT");
		return totalSize;
	}

	/**
	 * オペレータが待機状態かを返す
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @return	True オペレータが待機状態
	 * 			false オペレータが処理中
	 */

	bool Operator::isReady()
	{
		return operationReady;
	}

	/**
	 * データの到着可否を返す
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @return	True	処理データ受付済み
	 * 			false	待機中
	 */

	bool Operator::isDataReady()
	{
		return data_ready;
	}

    /**
     * オペレータスレッド起動処理
     *
     * @author	Nagoya University
     * @date	2018/03/13
     */

    void Operator::start()
    {
		//REL_COMMENT logger->trace("[" + getType() + "] start IN");

        thread test(&Operator::execute, this );

		//REL_COMMENT logger->trace("[" + getType() + "] detach");
        test.detach();
		start_flag = true;
		//REL_COMMENT logger->trace("[" + getType() + "] start OUT");
    }

    /**
     * オペレータ終了処理
     *
     * @author	Nagoya University
     * @date	2018/03/13
     */

    void Operator::exit()
    {
		logger->debug("[" + getType() + "] exit_flag ON");
        exit_flag = true;
        // 自オペレータを起こす
		notify_one();
    }
	void Operator::process_close()
	{
		return ;
	}

    /**
     * スレッド処理
     *
     * @author	Nagoya University
     * @date	2018/03/13
     */

    void Operator::execute()
    {
		logger->debug("[" + getType() + "] execute. ARG: " + this->argument);
        
        std::unique_lock<std::mutex> dataReadyLock(mtx);
		bool isNext = false;
		long startTime, endTime;
		double procTime = 0.0;
		double getTime = 0.0;
		double fireTime = 0.0;
		long tupleSum = 0;
		while (1)
		{
			if (exit_flag) {
				process_close();
				// exit_loop
				//REL_COMMENT logger->trace("[" + getType() + "] exit_loop");
				break;
			}
			else {
				if (buffer.size() > 10) {
					logger->warn("[" + getType() + "] ready bufsize:" + std::to_string(buffer.size()));
				}
				logger->debug("[" + getType() + "] ready bufsize:" + std::to_string(buffer.size()));
			}
			// 駆動待ち
			data_ready = false;
			while (!data_ready && !exit_flag)
			{
				// 後続処理を行うのに必要なバッファ数が、1でないケースがある。通常は、expect_data_size = 1だが、下記のケースは異なる。
				//  DropOperator：バッファ数に関係なく、Dropさせる。expect_data_size = 0
				//  JoinOperator：結合するために、バッファは最低2個必要。expect_data_size = 2
				if ( (expect_data_size == 0 && buffer.size() > 0) ||
					 (expect_data_size != 0 && buffer.size() >= expect_data_size) ) {
					break;
				}

				operationReady = true;
				cond.wait(dataReadyLock);
				operationReady = false;
			}
			

			if (exit_flag)
			{
				process_close();
				// exit_loop
				//REL_COMMENT logger->trace("[" + getType() + "] exit_loop");
				break;
			}
			logger->debug("[" + getType() + "] wake up");

			startTime = DmUtil::getTimeMicrosec();
			
			// データの取得
			long tupleNum = getTupleSet(ts);
			if (ts.size() == 0) {
				// notify_oneされたが既に処理済みでbufferが空のケース
				continue;
			}
			calcTupleNum(tupleNum, tupleNumAVG, tupleNumMin, tupleNumMax);
			tupleSum += tupleNum;

			endTime = DmUtil::getTimeMicrosec();

			// getTupleSet()の計測
			calcTime(startTime, endTime, getTime, getTimeAVG, getTimeMin, getTimeMax);


			startTime = DmUtil::getTimeMicrosec();
			//REL_COMMENT logger->trace("[" + getType() + "] [execute] MUTEX inputMtx lock");
			pthread_mutex_lock(&inputMtx);
			setPerformInfo(startTime, ts, inputList, inputSum);
			pthread_mutex_unlock(&inputMtx);
			//REL_COMMENT logger->trace("[" + getType() + "] [execute] MUTEX inputMtx unlock");

			// 処理を行う
			isNext = process(ts);

			endTime = DmUtil::getTimeMicrosec();
			//REL_COMMENT logger->trace("[" + getType() + "] [execute] MUTEX outputMtx lock");
			pthread_mutex_lock(&outputMtx);
			setPerformInfo(endTime, ts, outputList, outputSum);
			pthread_mutex_unlock(&outputMtx);
			//REL_COMMENT logger->trace("[" + getType() + "] [execute] MUTEX outputMtx unlock");

			// process()の計測
			calcTime(startTime, endTime, procTime, processTimeAVG, processTimeMin, processTimeMax);
			
			startTime = DmUtil::getTimeMicrosec();

			// 結果を次へ渡す
			if (isNext && !exit_flag) {
				//REL_COMMENT logger->trace("[" + getType() + "][execute] ts.size():" + std::to_string(ts.size()));
				if (!fireEvent(ts.at(0), this)) {
					ResponseOperator *rOpe = dynamic_cast<ResponseOperator*>(this);
					if (rOpe != NULL) {
						rOpe->checkTerminate();
					}
				}
			}

			endTime = DmUtil::getTimeMicrosec();

			// fireEvent()の計測
			calcTime(startTime, endTime, fireTime, fireTimeAVG, fireTimeMin, fireTimeMax);

			double allTime = getTime + procTime + fireTime;
			allTimeAVG = (double)((allTimeAVG * (processNum)) + allTime) / (double)(processNum + 1);
			if (allTimeMin == 0 || allTime < allTimeMin) allTimeMin = allTime;
			if (allTimeMax == 0 || allTime > allTimeMax) allTimeMax = allTime;
			processNum++;
			processNumSum++;
			if (Logger::getRootLogger()->getLevel()->toInt() <= Level::DEBUG_INT) {
				logger->debug("[PERFORMANCE][" + getType() + "] getTime:" + to_string(getTime) + "[ms] getAVG:" + to_string(getTimeAVG) + "[ms] procTime:" + to_string(procTime) + "[ms] AVGTime:" + to_string(processTimeAVG) + "[ms] fireTime:" + to_string(fireTime) + "[ms] fireAVG:" + to_string(fireTimeAVG) + "[ms] procNum:" + to_string(processNum) + " bufferSize:" + to_string(buffer.size()) + " allAVG:" + to_string(allTimeAVG) + "[ms]");
			}
			
			for (unsigned int i = 0; i < ts.size(); i++) {
				ts.at(i).clear();
			}
			ts.clear();
			ts.shrink_to_fit();
			ts.reserve(expect_data_size);
		}
		exitReady = true;
		//REL_COMMENT logger->trace("[" + getType() + "] execute OUT");
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

    bool Operator::process(std::vector<TupleSet>& ts)
    {
        // 稀にスーパークラスのprocessが実行される事がある。
		logger->debug("[Operator::process]" + getType() + " is in super-class-process-method.");
        return true;
    }

    /**
     * ノードID付与
     *
     * @author	Nagoya University
     * @date	2019/05/22
     *
     * @param [in]	nodeId	ノードID
     *
     */
	void Operator::setNodeId(string nodeId)
	{
		this->type.append("_" + nodeId);
		this->nodeId = nodeId;
	}

	/**
	* 統計情報の計算
	*
	* @author	Nagoya University
	* @date	2019/05/22
	*
	* @param [in]	startTime	開始時間
	* @param [in]	endTime		終了時間
	* @param [in,out]	time	計測時間
	* @param [in,out]	timeAVG	平均時間
	* @param [in,out]	timeMin	最短時間
	* @param [in,out]	timeMax	最長時間
	*
	*/
	inline void Operator::calcTime(const double &startTime, const double &endTime, double &time, double &timeAVG, double &timeMin, double &timeMax)
	{
		time = (endTime - startTime) / 1000.0;
		timeAVG = (double)((timeAVG * (processNum)) + time) / (double)(processNum + 1);
		if (timeMin == 0 || time < timeMin) timeMin = time;
		if (timeMax == 0 || time > timeMax) timeMax = time;
	}
	
	/**
	* 統計情報の計算
	*
	* @author	Shinichi Kusayama
	* @date	20230815
	*
	* @param [in,out]	tupleNum	計測タプル数
	* @param [in,out]	tupleNumAVG	平均タプル数
	* @param [in,out]	tupleNumMin	最短タプル数
	* @param [in,out]	tupleNumMax	最長タプル数
	*
	*/
	inline void Operator::calcTupleNum(const long &tupleNum, double &tupleNumAVG, long &tupleNumMin, long &tupleNumMax)
	{
		tupleNumAVG = (double)((tupleNumAVG * (processNum)) + tupleNum) / (double)(processNum + 1);
		if (tupleNumMin == 0 || tupleNum < tupleNumMin) tupleNumMin = tupleNum;
		if (tupleNumMax == 0 || tupleNum > tupleNumMax) tupleNumMax = tupleNum;
	}
	/**
	* 性能情報Listに情報を登録・古いものを削除する
	*
	* @author	Nagoya University
	* @date	2019/10/30
	*
	* @param [in]	time	登録時間
	* @param [in]	ts		タプルセット
	* @param [in,out]	list	性能情報リスト
	*
	*/
	void Operator::setPerformInfo(const long time, const vector<TupleSet> &ts, deque<performInfo> &list, int &sum)
	{
		//REL_COMMENT logger->trace("[setPerformInfo] IN");
		// 新規データ登録
		performInfo pi;
		pi.time = time;
		int tupleSum = 0;

		for (unsigned int i = 0; i < ts.size(); i++) {
			tupleSum += ts.at(i).size();
		}
		pi.num = tupleSum;
		list.push_back(pi);
		//REL_COMMENT logger->trace("[setPerformInfo] [ADD] pi.time:" + to_string(pi.time) +  " pi.num:" + to_string(pi.num));
		sum += tupleSum;

		// 古いデータ削除 (TODO:性能向上のためには削除処理を別で実施すべき)
		deque<performInfo>::iterator it = list.begin();
		while (it != list.end()) {
			if (it->time < (time - (1 * 1000 * 1000))) {
				//REL_COMMENT logger->trace("[setPerformInfo] [DEL] it->time:" + to_string(it->time) + " it->num:" + to_string(it->num));
				it = list.erase(it);
			}
			else {
				list.shrink_to_fit();
				break;
			}
		}
		//REL_COMMENT logger->trace("[setPerformInfo] OUT");
	}

	/**
	* 性能情報を取得する
	*
	* @author	Nagoya University
	* @date	2019/10/30
	*
	* @param [in]	type	情報種別(input/output)
	* @return	過去1秒間に対象となったタプル数
	*
	*/
	int Operator::getPerformInfoNum(const string type)
	{
		//REL_COMMENT logger->trace("[" + getType() + "] [getPerformInfoNum] IN type:" + type);
		int ret = 0;
		long now = DmUtil::getTimeMicrosec();
		//REL_COMMENT logger->trace("[" + getType() + "] [getPerformInfoNum] now:" + to_string(now));
		if (type == "input") {
			//REL_COMMENT logger->trace("[" + getType() + "] [getPerformInfoNum] MUTEX inputMtx lock");
			pthread_mutex_lock(&inputMtx);
			deque<performInfo>::iterator it = inputList.begin();
			while (it != inputList.end()) {
				//REL_COMMENT logger->trace("[" + getType() + "] [getPerformInfoNum] [LOOP] it->time:" + to_string(it->time) + " it->num:" + to_string(it->num) + " Judge time:"  + to_string(it->time) + " > " + to_string((now - (1 * 1000 * 1000))) + " ?" );
				if (it->time > (now - (1 * 1000 * 1000))) {
					//REL_COMMENT logger->trace("[" + getType() + "] [getPerformInfoNum] [PLUS] it->time:" + to_string(it->time) + " it->num:" + to_string(it->num) + " Judge time:" + to_string((now - (1 * 1000 * 1000))) + " ?");
					ret += it->num;
				}
				it++;
			}
			pthread_mutex_unlock(&inputMtx);
			//REL_COMMENT logger->trace("[" + getType() + "] [getPerformInfoNum] MUTEX inputMtx unlock");
		}
		else if (type == "output") {
			//REL_COMMENT logger->trace("[" + getType() + "] [getPerformInfoNum] MUTEX outputMtx lock");
			pthread_mutex_lock(&outputMtx);
			deque<performInfo>::iterator it = outputList.begin();
			while (it != outputList.end()) {
				//REL_COMMENT logger->trace("[" + getType() + "] [getPerformInfoNum] [LOOP] it->time:" + to_string(it->time) + " it->num:" + to_string(it->num) + " Judge time:" + to_string(it->time) + " > " + to_string((now - (1 * 1000 * 1000))) + " ?");
				if (it->time > (now - (1 * 1000 * 1000))) {
					ret += it->num;
					//REL_COMMENT logger->trace("[" + getType() + "] [getPerformInfoNum] [PLUS] it->time:" + to_string(it->time) + " it->num:" + to_string(it->num) + " Judge time:" + to_string((now - (1 * 1000 * 1000))) + " ?");
				}
				it++;
			}
			pthread_mutex_unlock(&outputMtx);
			//REL_COMMENT logger->trace("[" + getType() + "] [getPerformInfoNum] MUTEX outputMtx unlock");
		}
		else {
			logger->warn("[" + getType() + "] [getPerformInfoNum] No defined type:" + type);
			return 0;
		}
		//REL_COMMENT logger->trace("[" + getType() + "] [getPerformInfoNum] out ret:" + to_string(ret));
		return ret;
	}
	/**
	* 分を取得
	*
	* @author	Shinichi Kusayama
	* @date	2022/10/12
	*
	*
	*/
	int Operator::getMinute(string hhmm) {
		struct tm t;
		int minute;
		if (hhmm == "") {
			struct timespec ts;
			clock_gettime(CLOCK_REALTIME, &ts);
			localtime_r(&ts.tv_sec, &t);
			minute = t.tm_hour * 60 + t.tm_min;
		} else {
			std::istringstream is(hhmm);
			is >> std::get_time(&t,"%H:%M");
			minute = t.tm_hour * 60 + t.tm_min;
		}
		return minute;
	}

	/**
	 * Operatorに与えられたタプル情報、パラメータ情報を出力する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param [in,out]	ts		 	タプルセット
	 * @param 		  	parameter	条件パラメータ
	 */

	void Operator::printInputInfo(TupleSet& ts, const string &parameter)
	{
		if (Logger::getRootLogger()->getLevel()->toInt() > Level::DEBUG_INT) return;
		string log = "";
		log.append("\n--------> IN\n");
		log.append(" DATA : ");
		log.append(ts.getSchemaRef().getTableName());
		log.append( " COL_SIZE : ");
		log.append(to_string(ts.getSchemaRef().getAttributeSize()));
		log.append("\n");
		appendAttribute(ts, log);
		appendAttributeType(ts, log);

		if (ts.size() > 0) {
			int i = 0;
			for (int idx = 0; idx < ts.size(); idx++) {
				appendValue(ts.getTuple(idx), log);
				if (++i >= 10 && Logger::getRootLogger()->getLevel()->toInt() == Level::DEBUG_INT) {
					log.append("    and more.....(tuples.size:");
					log.append( to_string(ts.size()));
					log.append(") If you want to display all tuples, set log level 'trace'\n");
					break;
				}
			}
		}
		log.append("\n CONDITION : \n");
		log.append(parameter);
		log.append("\n");
		logger->debug(log);
	}

	/**
	 * Operatorに与えられた複数のタプル情報、パラメータ情報を出力する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param [in,out]	tsList   	タプルセットリスト
	 * @param 		  	parameter	条件パラメータ
	 */

	void Operator::printInputInfos(vector<IS::TupleSet>& tsList, const string &parameter)
	{
		if (Logger::getRootLogger()->getLevel()->toInt() > Level::DEBUG_INT) return;
		string log = "";
		log.append("\n--------> IN\n");
		for (unsigned int tsIdx = 0; tsIdx < tsList.size(); tsIdx++) {
			log.append(" DATA : ");
			log.append(tsList.at(tsIdx).getSchemaRef().getTableName());
			log.append(" COL_SIZE : ");
			log.append(to_string(tsList.at(tsIdx).getSchemaRef().getAttributeSize()));
			log.append("\n");
			appendAttribute(tsList.at(tsIdx), log);
			appendAttributeType(tsList.at(tsIdx), log);
			if (tsList.at(tsIdx).size() > 0) {
				int i = 0;
				for (int idx = 0; idx < tsList.at(tsIdx).size(); idx++) {
					appendValue(tsList.at(tsIdx).getTuple(idx), log);
					if (++i >= 10 && Logger::getRootLogger()->getLevel()->toInt() == Level::DEBUG_INT) {
						log.append("    and more.....(tuples.size:");
						log.append( to_string(tsList.at(tsIdx).size()));
						log.append(") If you want to display all tuples, set log level 'trace'\n");
						break;
					}
				}
			}
		}
		log.append("\n CONDITION : \n");
		log.append(parameter);
		log.append("\n");
		logger->debug(log);
	}

	/**
	 * Operatorに与えられたタプル情報、パラメータ情報を出力
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param [in,out]	ts	タプルセット
	 */

	void Operator::printOutputInfo(TupleSet& ts)
	{
		if (Logger::getRootLogger()->getLevel()->toInt() > Level::DEBUG_INT) return;
		string log = "";
		log.append("\n--------> OUT\n");
		log.append(" DATA : ");
		log.append(ts.getSchemaRef().getTableName());
		log.append(" COL_SIZE : ");
		log.append(to_string(ts.getSchemaRef().getAttributeSize()));
		log.append("\n");
		appendAttribute(ts, log);
		appendAttributeType(ts, log);
		if (ts.size() > 0) {
			int i = 0;
			for (int idx = 0; idx < ts.size(); idx++) {
				appendValue(ts.getTuple(idx), log);
				if ( ++i >= 10 && Logger::getRootLogger()->getLevel()->toInt() == Level::DEBUG_INT) {
					log.append("    and more.....(tuples.size:");
					log.append(to_string(ts.size()));
					log.append(") If you want to display all tuples, set log level 'trace'\n");
					break;
				}
			}
		}
		log.append("\n");
		logger->debug(log);
	}

	/**
	 * Operatorに与えられたタプル情報、パラメータ情報を出力 (For Debug)
	 *
	 * @author	Shinichi Kusayama
	 * @date	2023/04/12
	 *
	 * @param [in,out]	ts	タプルセット
	 */

	void Operator::cOutputInfo(TupleSet& ts)
	{
		string log = "";
		log.append("\n--------> OUT\n");
		log.append(" DATA : ");
		log.append(ts.getSchemaRef().getTableName());
		log.append(" COL_SIZE : ");
		log.append(to_string(ts.getSchemaRef().getAttributeSize()));
		log.append("\n");
		appendAttribute(ts, log);
		appendAttributeType(ts, log);
		if (ts.size() > 0) {
			for (int idx = 0; idx < ts.size(); idx++) {
				appendValue(ts.getTuple(idx), log);
			}
		}
		log.append("\n");
		cout << log;
	}
	/**
	 * valueを表示する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	tuple	タプル
	 *
	 * @return	出力文字列
	 */

	inline void Operator::appendValue(const Tuple &tuple, string &result)
	{
		char str[printBufSize];
		any val;
		string valStr;
		for (int i = 0; i < tuple.size(); i++) {
			tuple.getValueByIdx(i, val);
			if (i != 0) {
				result.append(",");
			}
			valStr = stringUtil.getAnyString(val);
			if (printBufSize <= valStr.length()) {
				result.append(valStr);
			}
			else {
				sprintf(str, printFormat.c_str(), valStr.c_str());
				result.append(string(str, (valStr.length() <= DisplayChar ? DisplayChar : valStr.length())));
			}
		}
		result.append("\n");
	}
	/**
	 * Atrribute名を出力する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param [in,out]	ts	タプルセット
	 *
	 * @return	出力文字列
	 */

	void Operator::appendAttribute(TupleSet& ts, string &result)
	{
		char str[printBufSize];	
		for (int i = 0; i < ts.getSchemaRef().getAttributeSize(); i++) {

			if (i != 0) {
				result.append(",");
			}
			if (ts.getSchemaRef().getAttributeName(i).length() >= printBufSize) {
				result.append(ts.getSchemaRef().getAttributeName(i));
			}
			else {
				sprintf(str, printFormat.c_str(), ts.getSchemaRef().getAttributeName(i).c_str());
				result.append(string(str, (ts.getSchemaRef().getAttributeName(i).length() <= DisplayChar ? DisplayChar : ts.getSchemaRef().getAttributeName(i).length())));
			}
		}
		result.append("\n");
	}

	/**
	* Atrributeの型を出力する
	*
	* @author	Nagoya University
	* @date	2019/07/30
	*
	* @param [in,out]	ts	タプルセット
	*
	* @return	出力文字列
	*/

	void Operator::appendAttributeType(TupleSet& ts, string &result)
	{
		char str[printBufSize];
		for (int i = 0; i < ts.getSchemaRef().getAttributeSize(); i++) {

			if (i != 0) {
				result.append(",");
			}
			sprintf(str, printFormat.c_str(), ts.getSchemaRef().getAttributeType(i).c_str());
			result.append(string(str, (ts.getSchemaRef().getAttributeType(i).length() <= DisplayChar ? DisplayChar : ts.getSchemaRef().getAttributeType(i).length())));
		}
		result.append("\n");
	}

}
