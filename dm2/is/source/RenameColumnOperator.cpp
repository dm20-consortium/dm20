#include "RenameColumnOperator.h"

using namespace std;

namespace IS {

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param 	mngId	クエリ管理番号
	 * @param	param	The parameter.
	 */

	RenameColumnOperator::RenameColumnOperator(unsigned int mngId, string param)
	{
		this->type = MyName + "_MNGID:" + std::to_string(mngId);
		this->mngId = mngId;
		this->argument = param;
		// キュー用メモリの用意
	}

	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	RenameColumnOperator::~RenameColumnOperator()
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

	bool RenameColumnOperator::process(vector<IS::TupleSet>& ts)
	{
		logger->debug("[" + this->type + "] ========== RenameColumn START ========== ");

#if MEASURE_MODE == 1
		long startTime = DmUtil::getTimeMicrosec();
		long procTime = startTime;
		int step = 1;
#endif

		//// Selectionは1つのtuplesetを使用
		TupleSet& tupleset = ts.at(0);

		// DEBUG 与えられたタプル情報の出力
		printInputInfo(tupleset, this->argument);

		// 初回のみパラメータを解析
		if (fromList.size() == 0 && toList.size() == 0) {

			// parameterを分解
			vector<string> paramList;
			stringUtil.split(this->argument, " as ", paramList);

			// statement毎にcondition情報に整形
			if (paramList.size() != 2)
			{
				logger->error("[" + this->type + "] Arguments are invalid. Expect:\"(XXX.)aaa as ccc\" param : " + this->argument + " paramList.size() : " + std::to_string(paramList.size()));
				return false;
			}

			fromStr = stringUtil.deleteSpace(paramList.at(0));
			if (stringUtil.isString(fromStr)) {
				// リネーム元が文字列直値の場合
				vector<string> delList = { "'" };
				stringUtil.deleteStrings(fromStr, delList);
			}
			stringUtil.split(fromStr, ".", fromList);

			toStr = stringUtil.deleteSpace(paramList.at(1));
			stringUtil.split(toStr, ".", toList);
			if (toList.size() != 1) {
				logger->error("[" + this->type + "] Arguments are invalid. Table name not required for renameColumnName. param : " + this->argument);
				return false;
			}
			stringUtil.getStringInBracket(fromStr, retList);

		}

		outputSchema = tupleset.getSchema();

#if MEASURE_MODE == 1
		long now = DmUtil::getTimeMicrosec();
		double msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " parameter analytics processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

		// ユーザ定義関数のリネーム
		// カラム名指定同士のリネーム
		// テーブル指定カラムからカラム名のみ指定のリネーム
		if ((retList.size() > 0) ||
			(fromList.size() == 1 && toList.size() == 1) ||
			(fromList.size() == 2 && toList.size() == 1)) {
			// スキーマ指定なしカラム名の変更
			if (!outputSchema.renameColumnName(fromStr, toStr)) {
				return false;
			}
		}
		else {
			// 指定違反エラー
			logger->error("[" + this->type + "] Arguments are invalid. param : " + this->argument);
		}

		tupleset.SetSchema(outputSchema);

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " rename column processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

		// DEBUG 抽出されたタプル情報の出力
		printOutputInfo(tupleset);

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - startTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " total processing time: " + to_string(msec) + "[ms]");
#endif

		logger->debug("[" + this->type + "] ========== RenameColumn  END  ========== ");
		return true;
	}

}