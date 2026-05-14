#include "JoinOperator.h"
#include "Condition.h"

using namespace std;

namespace IS {

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	mngId	クエリ管理番号
	 * @param	param	実行パラメータ
	 */

	JoinOperator::JoinOperator(unsigned int mngId, string param)
	{
		this->type = MyName + "_MNGID:" + std::to_string(mngId);
		this->mngId = mngId;
		this->argument = param;
		expect_data_size = 2;
		// キュー用メモリの用意
	}

	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	JoinOperator::~JoinOperator()
	{

	}

	/**
	* タプルの受け取り処理
	* 他オペレータからコールされ、自身に渡されたタプルセットをキューイングする
	*
	* @author	Nagoya University
	* @date	2019/03/11
	*
	* @param	ts	   	タプルセット
	*/

	void JoinOperator::receiveTupleSet(TupleSet &ts)
	{
		// キューイング
		if (ts.getSchemaRef().getTableName() != Schema::UNKNOWN) {
			//REL_COMMENT logger->trace("[" + getType() + "] [receiveTupleSet] MUTEX bufMtx lock");
			pthread_mutex_lock(&bufMtx);
			logger->debug("[" + getType() + "][receiveTupleSet] OccurrenceType:" + to_string((int)ts.info.type) + " info.recvTime:" + to_string(ts.info.recvTime));
			if (waitQMap.count(ts.info.recvTime) == 0) {
				// 新規の相手待ちTupleの場合
				waitQMap[ts.info.recvTime] = ts;
				if (ts.info.isRecursive) {
					requestParentProc(ts.fromOperator, ts.info.recvTime);
				}
			}
			else {
				// 既に相手待ちだったTupleの場合
				// TODO:現状AttributeSizeが大きいものをleftとしてJOIN
				if (waitQMap[ts.info.recvTime].getSchemaRef().getAttributeSize() > ts.getSchemaRef().getAttributeSize()) {
					buffer.push_back(waitQMap[ts.info.recvTime]);
					waitQMap.erase(ts.info.recvTime);
					buffer.push_back(ts);
				}
				else if (waitQMap[ts.info.recvTime].getSchemaRef().getAttributeSize() < ts.getSchemaRef().getAttributeSize()) {
					buffer.push_back(ts);
					buffer.push_back(waitQMap[ts.info.recvTime]);
					waitQMap.erase(ts.info.recvTime);
				}
				else {
					// AttributeSizeが同じ場合はテーブル名の比較で判断
					if (waitQMap[ts.info.recvTime].getSchemaRef().getTableName().compare(ts.getSchemaRef().getTableName()) > 0) {
						buffer.push_back(waitQMap[ts.info.recvTime]);
						waitQMap.erase(ts.info.recvTime);
						buffer.push_back(ts);
					}
					else if(waitQMap[ts.info.recvTime].getSchemaRef().getTableName().compare(ts.getSchemaRef().getTableName()) < 0){
						buffer.push_back(ts);
						buffer.push_back(waitQMap[ts.info.recvTime]);
						waitQMap.erase(ts.info.recvTime);
					}
					else {
						logger->warn("[" + getType() + "] [receiveTupleSet] table join order cannot be guaranteed, because attribute size is same and table name is same.");
						buffer.push_back(waitQMap[ts.info.recvTime]);
						waitQMap.erase(ts.info.recvTime);
						buffer.push_back(ts);
					}
				}
				notify_one();
			}
			logger->debug("[" + getType() + "] [receiveTupleSet] buffer.push bufsize:" + std::to_string(buffer.size()) + " waitQMap.size:" + to_string(waitQMap.size()));
			pthread_mutex_unlock(&bufMtx);
			//REL_COMMENT logger->trace("[" + getType() + "] [receiveTupleSet] MUTEX bufMtx unlock");
		}
		else {
			logger->error("[" + getType() + "] [receiveTupleSet] Input TableName is UNKNOWN");
		}
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

	bool JoinOperator::process(vector<IS::TupleSet>& ts)
	{
		logger->debug("[" + this->type + "] ========== Join START ========== ");
		IS::TupleSet retTupleSet;

#if MEASURE_MODE == 1
		long startTime = DmUtil::getTimeMicrosec();
		long procTime = startTime;
		int step = 1;
#endif

		// Joinは2つのtuplesetを使用
		if (ts.size() <= 1) {
			logger->error("[" + this->type + "] Can't join because ts.size() <= 1");
			return false;
		}

		TupleSet& tupleset1 = ts.at(0);
		TupleSet& tupleset2 = ts.at(1);

		// DEBUG 与えられたタプル情報の出力
		printInputInfos(ts, this->argument);

		// 初回時のみschemaを構成
		if (outputSchema.getAttributeSize() == 0 || outputSchema.getAttributeSize() != (tupleset1.getSchemaRef().getAttributeSize() + tupleset2.getSchemaRef().getAttributeSize())) {
			// schemaの設定
			outputSchema = IS::Schema(tupleset1.getSchemaRef().getTableName(), tupleset1.getSchemaRef().getTableType());
			outputSchema.copyAttributes(tupleset1.getSchemaRef());
			outputSchema.addAttributes(tupleset2.getSchemaRef());

			stringUtil.split(this->argument, " AND ", paramList);
			if (paramList.size() != 0) {
				isUseHashJoin = true; 
			}

			// statement毎にcondition情報に整形
			for (string state : paramList) {
				state = stringUtil.deleteSpace(state);
				Condition cond;
				if (cond.getCondition(outputSchema, state, cond)) {
					if (cond.fieldIdx != -1 && cond.fieldIdx2 != -1) {
						// 両辺がカラム指定の場合
						if (tupleset1.getSchemaRef().isExistAttribute(cond.fieldName) && tupleset2.getSchemaRef().isExistAttribute(cond.fieldName2)) {
							// join順序が式の順序と一致している場合
							isReverseArgList.push_back(false);
							//REL_COMMENT logger->trace("[" + this->type + "] isReverse:false");
							cond.fieldIdx = tupleset1.getSchemaRef().getAttributeIdx(cond.fieldName);
							//REL_COMMENT logger->trace("[" + this->type + "] tupleset1 name:" + cond.fieldName + " idx:" + to_string(cond.fieldIdx));
							cond.fieldIdx2 = tupleset2.getSchemaRef().getAttributeIdx(cond.fieldName2);
							//REL_COMMENT logger->trace("[" + this->type + "] tupleset2 name:" + cond.fieldName2 + " idx:" + to_string(cond.fieldIdx2));
						}
						else if (tupleset1.getSchemaRef().isExistAttribute(cond.fieldName2) && tupleset2.getSchemaRef().isExistAttribute(cond.fieldName)) {
							// join順序と式の順序が逆の場合
							isReverseArgList.push_back(true);
							//REL_COMMENT logger->trace("isReverse:true");
							cond.fieldIdx = tupleset2.getSchemaRef().getAttributeIdx(cond.fieldName);
							//REL_COMMENT logger->trace("[" + this->type + "] tupleset2 name:" + cond.fieldName + " idx:" + to_string(cond.fieldIdx));
							cond.fieldIdx2 = tupleset1.getSchemaRef().getAttributeIdx(cond.fieldName2);
							//REL_COMMENT logger->trace("[" + this->type + "] tupleset1 name:" + cond.fieldName2 + " idx:" + to_string(cond.fieldIdx2));
						}
						else {
							logger->error("[" + this->type + "] isExistAttribute failed. Is the field name correct? fieldName:" + cond.fieldName + " fieldName2:" + cond.fieldName2);
							return false;
						}
					}
					else {
						// どちらかの辺が固定値の場合
						if (tupleset1.getSchemaRef().isExistAttribute(cond.fieldName)) {
							// 左側のjoinテーブルに指定カラムが存在する場合
							isReverseArgList.push_back(false);
							cond.fieldIdx = tupleset1.getSchemaRef().getAttributeIdx(cond.fieldName);
							//REL_COMMENT logger->trace("[" + this->type + "] tupleset1 name:" + cond.fieldName + " idx:" + to_string(cond.fieldIdx));
						}
						else if(tupleset2.getSchemaRef().isExistAttribute(cond.fieldName)){
							// 右側のjoinテーブルに指定カラムが存在する場合
							isReverseArgList.push_back(true);
							cond.fieldIdx = tupleset2.getSchemaRef().getAttributeIdx(cond.fieldName);
							//REL_COMMENT logger->trace("[" + this->type + "] tupleset2 name:" + cond.fieldName + " idx:" + to_string(cond.fieldIdx));
						}						
					}
					conditions.push_back(cond);
					if (cond.cond != Condition::EQ) {
						// イコール以外の比較演算子が存在する場合はHashJoinを使えない
						isUseHashJoin = false;
					}
				}
				else {
					logger->error("[" + this->type + "] getCondition failed. Is the field name correct? fieldName:" + cond.fieldName + " fieldName2:" + cond.fieldName2);
					return false;
				}
			}			
		}
		retTupleSet.SetSchema(outputSchema);

#if MEASURE_MODE == 1
		long now = DmUtil::getTimeMicrosec();
		double msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " schema settings processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

		if (isUseHashJoin) {
			// Hash Join
			bool ret = hashJoin(tupleset1, tupleset2, retTupleSet);
			if (!ret) {
				return false;
			}
		}
		else {
			// Nested Loop Join
			nestedLoopJoin(tupleset1, tupleset2, retTupleSet);
		}

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " join processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

		retTupleSet.info = tupleset1.info;
		if (tupleset1.info.isRecursive || tupleset2.info.isRecursive) {
			retTupleSet.info.isRecursive = true;
		}

		for (unsigned int i = 0; i < ts.size(); i++) {
			ts.at(i).clear();
		}
		ts.clear();
		ts.reserve(1);
		ts.push_back(retTupleSet);

		// DEBUG 抽出されたタプル情報の出力
		printOutputInfo(retTupleSet);

		retTupleSet.clear();

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - startTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " total processing time: " + to_string(msec) + "[ms]");
#endif

		logger->debug("[" + this->type + "] ========== Join  END  ========== ");
		return true;
	}

	/**
	* TupleをJoinする
	*
	* @author	Nagoya University
	* @date	2020/02/6
	*
	* @param [in]	tuple1	 	タプル1
	* @param [in]	tuple2	 	タプル2
	* @param [in,out]	joinTuple	 	結合タプル
	*/

	void JoinOperator::join(const Tuple &tuple1, const Tuple &tuple2, Tuple &joinTuple)
	{
		any val;
		long timestamp;
		int dst_index = 0;
		for (int idx = 0; idx < tuple1.size(); idx++)
		{
			tuple1.getValue(idx, val, timestamp);
			joinTuple.setValue(dst_index++, val, timestamp);
		}
		for (int idx = 0; idx < tuple2.size(); idx++)
		{
			tuple2.getValue(idx, val, timestamp);
			joinTuple.setValue(dst_index++, val, timestamp);
		}
	}

	/**
	* HashJoinアルゴリズムを適用しJoinする
	*
	* @author	Nagoya University
	* @date	2020/02/18
	*
	* @param [in,out]	tupleset1	 	タプル1
	* @param [in,out]	tupleset2	 	タプル2
	* @param [in,out]	retTupleSet	 	結合タプル
	*/
	bool JoinOperator::hashJoin(TupleSet &tupleset1, TupleSet &tupleset2, TupleSet &retTupleSet)
	{
		TupleSet& smallSet = tupleset1;
		TupleSet& largeSet = tupleset2;

		if (tupleset1.size() < tupleset2.size()) {
			if (smallKeyIdxList.size() != 0 && isReverse) {
				// 初回でない、かつtupleset2がsmallSetであった場合(途中で行数の変動によりsmallとlargeが逆になった場合)
				// keyIdxListを入れ替える
				vector<int> tmpList = smallKeyIdxList;
				smallKeyIdxList = largeKeyIdxList;
				largeKeyIdxList = tmpList;
			}
			isReverse = false;
		}
		else {
			// Setをswapする
			TupleSet tmp = smallSet;
			smallSet = largeSet;
			largeSet = tmp;
			if (smallKeyIdxList.size() != 0 && !isReverse) {
				// 初回でない、かつtupleset1がsmallSetであった場合(途中で行数の変動によりsmallとlargeが逆になった場合)
				// keyIdxListを入れ替える
				vector<int> tmpList = smallKeyIdxList;
				smallKeyIdxList = largeKeyIdxList;
				largeKeyIdxList = tmpList;
			}
			isReverse = true;
		}

		// 初回時のみkeyの探索を実施
		if (smallKeyIdxList.size() == 0) {
			// TODO : 実証実験前提での実装のためORは未実装
			vector<string> paramList, stateList;
			stringUtil.split(this->argument, " AND ", paramList);

			// statement毎にcondition情報に整形
			for (string state : paramList) {
				stateList.clear();
				state = stringUtil.deleteSpace(state);
				stringUtil.split(state, "=", stateList);
				if (stateList.size() == 2) {
					if (!(smallSet.getSchemaRef().isExistAttribute(stateList.at(0))) && smallSet.getSchemaRef().isExistAttribute(stateList.at(1))) {
						// 右辺がbuildTableの場合
						smallKeyIdxList.push_back(smallSet.getSchemaRef().getAttributeIdx(stateList.at(1)));
						largeKeyIdxList.push_back(largeSet.getSchemaRef().getAttributeIdx(stateList.at(0)));
					}
					else if (smallSet.getSchemaRef().isExistAttribute(stateList.at(0)) && !(smallSet.getSchemaRef().isExistAttribute(stateList.at(1)))) {
						smallKeyIdxList.push_back(smallSet.getSchemaRef().getAttributeIdx(stateList.at(0)));
						largeKeyIdxList.push_back(largeSet.getSchemaRef().getAttributeIdx(stateList.at(1)));
					}
					else {
						logger->error("[" + this->type + "] Failed to interpret conditional expression. statement:" + state + " Exists in both or does not exist.");
						return false;
					}
				}
				else {
					logger->error("[" + this->type + "] Failed to interpret conditional expression. statement:" + state + " Expect: a.col1=b.col2");
					return false;
				}
			}
		}

		any val;
		unsigned int tupleIdx = 0;
		unordered_map<size_t, unordered_map<string, vector<unsigned int>>> hashTable;
		string joinKey;

		for (int i = 0; i < smallSet.size(); i++) {
			joinKey.clear();
			for (int index : smallKeyIdxList) {
				if (joinKey.length() != 0) joinKey.append("_");
				smallSet.getTuple(i).getValueByIdx(index, val);
				joinKey.append(stringUtil.getAnyString(val));
			}
			size_t hash = std::hash<std::string>()(joinKey);
			if (hashTable.count(hash) == 0) {
				unordered_map<string, vector<unsigned int>> keyTupleMp;
				vector<unsigned int> idxList;
				idxList.push_back(tupleIdx);
				keyTupleMp[joinKey] = idxList;
				hashTable[hash] = keyTupleMp;
			}
			else {
				hashTable[hash][joinKey].push_back(tupleIdx);
			}
			tupleIdx++;
		}

		// TODO:事前に抽出される行数を予め見積もる事が出来れば事前にresizeする事によるパフォーマンス向上が見込める
		//unsigned int index = 0;
		//retTupleSet.resize(smallSet.size() * largeSet.size());
		Tuple joinTuple(outputSchema.getAttributeSize());

		for (int i = 0; i < largeSet.size(); i++) {
			joinKey.clear();
			for (int index : largeKeyIdxList) {
				if (joinKey.length() != 0) joinKey.append("_");
				largeSet.getTuple(i).getValueByIdx(index, val);
				joinKey.append(stringUtil.getAnyString(val));
			}
			size_t hash = std::hash<std::string>()(joinKey);
			if (hashTable.count(hash) != 0 && hashTable[hash].count(joinKey) != 0) {
				if (!isReverse) {
					for (unsigned int idx : hashTable[hash][joinKey]) {
						join(smallSet.getTuple(idx), largeSet.getTuple(i), joinTuple);
						retTupleSet.addTuple(joinTuple);
						//retTupleSet.setTuple(index++, joinTuple);
					}
				}
				else {
					for (unsigned int idx : hashTable[hash][joinKey]) {
						join(largeSet.getTuple(i), smallSet.getTuple(idx), joinTuple);
						retTupleSet.addTuple(joinTuple);
						//retTupleSet.setTuple(index++, joinTuple);
					}
				}
			}
		}
		//retTupleSet.resize(index);
		logger->debug("[" + this->type + "] Use Hash Join. INPUT(Left:" + to_string(tupleset1.size()) + " Right:" + to_string(tupleset2.size()) + ") OUTPUT:" + to_string(retTupleSet.size()) + " Cond:" + this->argument);
		return true;
	}

	/**
	* NestedLoopJoinアルゴリズムを適用しJoinする
	*
	* @author	Nagoya University
	* @date	2020/02/19
	*
	* @param [in,out]	tupleset1	 	タプル1
	* @param [in,out]	tupleset2	 	タプル2
	* @param [in,out]	retTupleSet	 	結合タプル
	*/
	bool JoinOperator::nestedLoopJoin(TupleSet &tupleset1, TupleSet &tupleset2, TupleSet &retTupleSet)
	{
		unsigned int setIndex = 0;
		unsigned int match = 0;
		any val, val2;
		if (conditions.size() == 0) {
			// CROSS JOINの場合は先にサイズを確保
			retTupleSet.resize(tupleset1.size() * tupleset2.size());
		}

		Tuple joinTuple(outputSchema.getAttributeSize());
		for (int i = 0; i < tupleset1.size(); i++) {
			for (int j = 0; j < tupleset2.size(); j++) {
				match = 0;
				for (unsigned int idx = 0; idx < conditions.size(); idx++) {
					string log;
					// ひとつでも条件を満たさなかった場合は次の行の走査
					if (match != idx) break;
					Condition cond = conditions[idx];

					if (isReverseArgList[idx]) {
						tupleset2.getTuple(j).getValueByIdx(cond.fieldIdx, val);
						//REL_COMMENT logger->trace("[" + this->type + "] tupleSet2[" + to_string(j) + "] name:" + cond.fieldName + " val:" + stringUtil.getAnyString(val));
					}
					else {
						tupleset1.getTuple(i).getValueByIdx(cond.fieldIdx, val);
						//REL_COMMENT logger->trace("[" + this->type + "] tupleSet1[" + to_string(i) + "] name:" + cond.fieldName + " val:" + stringUtil.getAnyString(val));
					}

					// 片辺が列名の場合
					if (cond.valuePosition != Condition::Val_pos::nothing && cond.isMatchCondition(cond, val)) {
						// 条件に一致
						match++;
						continue;
					}
					// 両辺が列名の場合
					else if (cond.valuePosition == Condition::Val_pos::nothing) {
						if (isReverseArgList[idx]) {
							tupleset1.getTuple(i).getValueByIdx(cond.fieldIdx2, val2);
							//REL_COMMENT logger->trace("[" + this->type + "] tupleSet1[" + to_string(i) + "] name:" + cond.fieldName2 + " val2:" + stringUtil.getAnyString(val2));
						}
						else {
							tupleset2.getTuple(j).getValueByIdx(cond.fieldIdx2, val2);
							//REL_COMMENT logger->trace("[" + this->type + "] tupleSet2[" + to_string(j) + "] name:" + cond.fieldName2 + " val2:" + stringUtil.getAnyString(val2));
						}

						//REL_COMMENT logger->trace("ASHDA Condition :" + stringUtil.getAnyString(val) + " " + cond.cond + " " + stringUtil.getAnyString(val2));
						if (cond.isMatchCondition(cond, val, val2)) {
							//REL_COMMENT logger->trace("[" + this->type + "] Condition TRUE");
							// 条件に一致
							match++;
							continue;
						}
					}
				}
				if (match == conditions.size()) {
					join(tupleset1.getTuple(i), tupleset2.getTuple(j), joinTuple);
					if (conditions.size() == 0) {
						retTupleSet.setTuple(setIndex++, joinTuple);
					}
					else {
						retTupleSet.addTuple(joinTuple);
					}
				}
			}
		}
		logger->debug("[" + this->type + "] Use Nested Loop Join. INPUT(Left:" + to_string(tupleset1.size()) + " Right:" + to_string(tupleset2.size()) + ") OUTPUT:" + to_string(retTupleSet.size()) + " Cond:" + this->argument);
		return true;
	}
}