#include "UnionOperator.h"

using namespace std;

namespace IS {

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	mngId	クエリ管理番号
	 * @param	param	実行パラメータ
	 */

	UnionOperator::UnionOperator(unsigned int mngId, string param)
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
	 * @date	2018/03/14
	 */

	UnionOperator::~UnionOperator()
	{

	}


	/**
	* タプルの受け取り処理
	* 他オペレータからコールされ、自身に渡されたタプルセットをキューイングする
	*
	* @author	Nagoya University
	* @date	2019/03/11
	*
	* @param[in,out]	ts	   	タプルセット
	*/

	void UnionOperator::receiveTupleSet(TupleSet &ts)
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
				// TODO:現状AttributeSizeが大きいものをleftとしてJOIN（動的と静的をjoinする場合は管理者カラムが付与されている動的をleftとする）
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
					else if (waitQMap[ts.info.recvTime].getSchemaRef().getTableName().compare(ts.getSchemaRef().getTableName()) < 0) {
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
	bool UnionOperator::process(vector<IS::TupleSet>& ts)
	{
		logger->debug("[" + this->type + "] ========== Union START ========== ");

#if MEASURE_MODE == 1
		long startTime = DmUtil::getTimeMicrosec();
		long procTime = startTime;
#endif

		// Unionは2つのtuplesetを使用
		if (ts.size() == 0) {
			logger->warn("[" + this->type + "] Can't procesing Union because tupleset.size() == 0");
			return false;
		}
		else if (ts.size() == 1) {
			logger->debug("[" + this->type + "] Can't procesing UNION because tupleset.size() == 1");
			return true;
		}

		// DEBUG 与えられたタプル情報の出力
		printInputInfos(ts, this->argument);

		TupleSet& tupleset1 = ts.at(0);
		TupleSet& tupleset2 = ts.at(1);

		// どちらかが0の場合は0でない方をUnion結果とする
		if (tupleset1.size() == 0 && tupleset2.size() != 0)
		{
			if (tupleset1.info.isRecursive || tupleset2.info.isRecursive) {
				tupleset2.info.isRecursive = true;
			}
			reverse(ts.begin(),ts.end());
			ts.pop_back();

			// DEBUG 抽出されたタプル情報の出力
			printOutputInfo(tupleset2);

#if MEASURE_MODE == 1
			long now = DmUtil::getTimeMicrosec();
			double msec = (now - startTime) / 1000.0;
			logger->info("[" + this->type + "] STAT_STEP4 total processing time: " + to_string(msec) + "[ms]");
#endif
			return true;
		}
		else if (tupleset1.size() != 0 && tupleset2.size() == 0)
		{
			if (tupleset1.info.isRecursive || tupleset2.info.isRecursive) {
				tupleset1.info.isRecursive = true;
			}
			ts.pop_back();

			// DEBUG 抽出されたタプル情報の出力
			printOutputInfo(tupleset1);

#if MEASURE_MODE == 1
			long now = DmUtil::getTimeMicrosec();
			double msec = (now - startTime) / 1000.0;
			logger->info("[" + this->type + "] STAT_STEP4 total processing time: " + to_string(msec) + "[ms]");
#endif
			return true;
		}

		// 前回処理時からAttributeに変更がある場合は再度チェックを行う
		if (previousAttrSize1 != tupleset1.getSchemaRef().getAttributeSize() || previousAttrSize2 != tupleset2.getSchemaRef().getAttributeSize())
		{
			int t1colSize, t2colSize;
			previousAttrSize1 = tupleset1.getSchemaRef().getAttributeSize();
			previousAttrSize2 = tupleset2.getSchemaRef().getAttributeSize();
			t1colSize = previousAttrSize1;
			t2colSize = previousAttrSize2;
			if (tupleset1.getSchemaRef().getTableType() == Schema::STREAM) {
				for (int i = 0; i < tupleset1.getSchemaRef().getAttributeSize(); i++) {
					if (tupleset1.getSchemaRef().getAttributeHidden(i)) t1colSize--;
				}
			}
			if (tupleset2.getSchemaRef().getTableType() == Schema::STREAM) {
				for (int i = 0; i < tupleset2.getSchemaRef().getAttributeSize(); i++) {
					if (tupleset2.getSchemaRef().getAttributeHidden(i)) t2colSize--;
				}
			}
			
			// カラム数の一致確認
			if (t1colSize != t2colSize)
			{
				logger->error("[" + this->type + "] Can't procesing UNION because Number of columns doesn't match. tuplset1 : " + std::to_string(t1colSize) + " tupleset2 : " + std::to_string(t2colSize));
				return false;
			}
			int j = 0;
			// 各カラムのデータ型チェック
			for (int i = 0; i < tupleset1.getSchemaRef().getAttributeSize(); i++)
			{
				if (tupleset1.getSchemaRef().getAttributeHidden(i)) continue;
				if (!stringUtil.checkDataTypeMatches(tupleset1.getSchemaRef().getAttribute(i).type, tupleset2.getSchemaRef().getAttribute(j++).type, true))
				{
					logger->error("[" + this->type + "] Can't procesing UNION because Data type doesn't match. index : " + std::to_string(i) + " tuplset1 : " + tupleset1.getSchemaRef().getAttribute(i).type + " tupleset2 : " + tupleset2.getSchemaRef().getAttribute(i).type);
					return false;
				}
			}
		}

#if MEASURE_MODE == 1
		long now = DmUtil::getTimeMicrosec();
		double msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP1 parameter analytics processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

		// 性能改善により当セクションは不要

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP2 setting schema processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

		// Tupleの設定
		tupleset1.getTupleVec().insert(tupleset1.getTupleVec().end(), tupleset2.getTupleVec().begin(), tupleset2.getTupleVec().end());

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP3 union processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

		if (tupleset1.info.isRecursive || tupleset2.info.isRecursive) {
			tupleset1.info.isRecursive = true;
		}
		// ts.at(1)を削除
		ts.pop_back();

		// DEBUG 抽出されたタプル情報の出力
		printOutputInfo(tupleset1);

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - startTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP4 total processing time: " + to_string(msec) + "[ms]");
#endif

		logger->debug("[" + this->type + "] ========== Union  END  ========== ");
		return true;
	}

}