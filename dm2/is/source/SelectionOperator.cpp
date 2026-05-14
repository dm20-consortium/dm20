#include "SelectionOperator.h"

using namespace std;

namespace IS {

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param 	mngId	クエリ管理番号
	 * @param	param	実行パラメータ
	 */

	SelectionOperator::SelectionOperator(unsigned int mngId, string param)
	{
		this->type = MyName + "_MNGID:" + std::to_string(mngId);
		this->mngId = mngId;
		this->argument = param;
		conditions.clear();
		// キュー用メモリの用意
	}

	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	SelectionOperator::~SelectionOperator()
	{

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
	bool SelectionOperator::process(vector<IS::TupleSet>& ts)
	{
		logger->debug("[" + this->type + "] ========== Selection START ========== ");
		IS::TupleSet retTupleSet;
		
#if MEASURE_MODE == 1
		long startTime = DmUtil::getTimeMicrosec();
		long procTime = startTime;
		int step = 1;
#endif

		//// Selectionは1つのtuplesetを使用
		TupleSet& tupleset = ts.at(0);

		// DEBUG 与えられたタプル情報の出力
		printInputInfo(tupleset, this->argument);

		if (tupleset.size() == 0) {
			// DEBUG 抽出されたタプル情報の出力
			printOutputInfo(tupleset);

#if MEASURE_MODE == 1
			long now = DmUtil::getTimeMicrosec();
			double msec = (now - procTime) / 1000.0;
			logger->info("[" + this->type + "] STAT_STEP3 total processing time: " + to_string(msec) + "[ms]");
#endif
			logger->debug("[" + this->type + "] ========== Selection  END  ========== ");
			return true;
		}

		// 初回処理時、もしくは前回からAttributeサイズに変化がある場合(再帰クエリ)
		if (conditions.size() == 0 || previousAttributeNum != tupleset.getSchemaRef().getAttributeSize()) {
			conditions.clear();
			previousAttributeNum = tupleset.getSchemaRef().getAttributeSize();

			// argumentを分解
			// TODO : 実証実験前提での実装のためORは未実装
			vector<string> paramList;
			vector<Condition> tmp;
			stringUtil.split(this->argument, " AND ", paramList);

			// statement毎にcondition情報に整形
			for (string state : paramList) {
				state = stringUtil.deleteSpace(state);
				Condition cond;
				if (cond.getCondition(tupleset.getSchemaRef(), state, cond)) {
					tmp.push_back(cond);
				}
				else {
					logger->error("[" + this->type + "] getCondition failed. Is the field name correct? fieldName:" + cond.fieldName);
					return false;
				}
			}

			// "="など条件の厳しいものを優先するよう条件を並び替える
			for (unsigned int i = 0; i < priorityList.size(); i++) {
				for (auto it = tmp.begin(); it != tmp.end();) {
					Condition cond = *it;
					if (cond.cond == priorityList.at(i)) {
						conditions.push_back(cond);
						it = tmp.erase(it);
					}
					else {
						++it;
					}
				}
			}
		}

		
#if MEASURE_MODE == 1
		long now = DmUtil::getTimeMicrosec();
		double msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " parameter analytics processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

		// schemaの設定
		retTupleSet.SetSchema(tupleset.getSchema());

		any val, val2;

		// 指定されたAttributeを抽出
		for (int index = 0; index < tupleset.size(); index++) {

			unsigned int match = 0;
			for (unsigned int i = 0; i < conditions.size(); i++) {

				// ひとつでも条件を満たさなかった場合は次の行の走査
				if (match != i) break;

				tupleset.getTuple(index).getValueByIdx(conditions[i].fieldIdx, val);
				// 片辺が列名の場合
				if (conditions[i].valuePosition != Condition::Val_pos::nothing && conditions[i].isMatchCondition(conditions[i], val)) {
					// 条件に一致
					match++;
					continue;
				}
				// 両辺が列名の場合
				else if (conditions[i].valuePosition == Condition::Val_pos::nothing) {
					tupleset.getTuple(index).getValueByIdx(conditions[i].fieldIdx2, val2);
					//REL_COMMENT logger->trace("val1:" + attrName + "(" + to_string(conditions[i].fieldIdx) + ") val2:" + attrName2 + "(" + to_string(conditions[i].fieldIdx2) + ")");
					if (conditions[i].isMatchCondition(conditions[i], val, val2)) {
						// 条件に一致
						match++;
						continue;
					}
				}
			}
			if (match == conditions.size()) {
				retTupleSet.addTuple(tupleset.getTuple(index));
			}
		}

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " selection processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

		retTupleSet.info = tupleset.info;
		logger->debug("selection tuple. input:" + to_string(tupleset.size()) + " output:" + to_string(retTupleSet.size()));

		ts.clear();
		ts.push_back(retTupleSet);

		// DEBUG 抽出されたタプル情報の出力
		printOutputInfo(retTupleSet);
		
#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - startTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " total processing time: " + to_string(msec) + "[ms]");
#endif
		logger->debug("[" + this->type + "] ========== Selection  END  ========== ");
		return true;
	}

}