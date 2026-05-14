#include "RenameTableOperator.h"

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

	RenameTableOperator::RenameTableOperator(unsigned int mngId, string param)
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

	RenameTableOperator::~RenameTableOperator()
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

	bool RenameTableOperator::process(vector<IS::TupleSet>& ts)
	{
		logger->debug("[" + this->type + "] ========== RenameTable START ========== ");

#if MEASURE_MODE == 1
		long startTime = DmUtil::getTimeMicrosec();
		long procTime = startTime;
		int step = 1;
#endif

		// DEBUG 与えられたタプル情報の出力
		printInputInfo(ts.at(0), this->argument);

		if (fromList.size() == 0 && toList.size() == 0) {

			// argumentを分解
			vector<string> paramList;
			stringUtil.split(this->argument, " as ", paramList);

			// statement毎にcondition情報に整形
			if (paramList.size() != 2)
			{
				logger->error("[" + this->type + "] Arguments are invalid. Expect:\"XXX.aaa as XXX.ccc\" param : " + this->argument + " paramList.size() : " + std::to_string(paramList.size()));
			}

			string fromStr = stringUtil.deleteSpace(paramList.at(0));
			stringUtil.split(fromStr, ".", fromList);

			string toStr = stringUtil.deleteSpace(paramList.at(1));
			stringUtil.split(toStr, ".", toList);

			inputSchema = ts.at(0).getSchema();
		}

#if MEASURE_MODE == 1
		long now = DmUtil::getTimeMicrosec();
		double msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " parameter analytics processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

		bool isSameSchema = false;

		// 初回時以外はキャッシュを利用するため、入力スキーマの一致を確認
		if (outputSchema.getAttributeSize() != 0) {
			if (ts.at(0).getSchemaRef().getTableName() == inputSchema.getTableName() && ts.at(0).getSchemaRef().getAttributeSize() == inputSchema.getAttributeSize()) {
				int i = 0;
				for (; i < inputSchema.getAttributeSize(); i++) {
					if (inputSchema.getAttributeName(i) != ts.at(0).getSchemaRef().getAttributeName(i) || inputSchema.getAttributeType(i) != ts.at(0).getSchemaRef().getAttributeType(i)) {
						break;
					}
				}
				if (i == inputSchema.getAttributeSize()) isSameSchema = true;
			}
		}

		// 初回時、もしくは入力スキーマに変化があった場合
		if (outputSchema.getAttributeSize() == 0 || !isSameSchema) {
			if (fromList.size() == 1 && toList.size() == 1) {
				outputSchema = ts.at(0).getSchema();
				inputSchema = ts.at(0).getSchema();

				// テーブル名の変更
				outputSchema.renameTableName(fromList.at(0), toList.at(0));

				// スキーマのカラム情報にも付与
				vector<string> nameList;
				string toName = "";

				for (int i = 0; i < outputSchema.getAttributeSize(); i++) {
					//REL_COMMENT logger->debug("[" + this->type + "] before : " + attr.name);
					stringUtil.splitExcludeBrackets(outputSchema.getAttributeName(i), ".", nameList);
					if (nameList.size() == 2) {
						// すでにカラム名にスキーマ名が付与されている場合
						toName = outputSchema.getTableName() + "." + nameList[1];
					}
					else if (nameList.size() == 1) {
						// カラム名にスキーマ名が付与されていない場合
						toName = outputSchema.getTableName() + "." + outputSchema.getAttributeName(i);
					}
					else {
						logger->error("Failed to invalied schema name:" + outputSchema.getAttributeName(i));
						toName = toName = outputSchema.getTableName() + "." + nameList[nameList.size() - 1];
					}

					outputSchema.renameColumnName(outputSchema.getAttributeName(i), toName);
					//REL_COMMENT logger->debug("[" + this->type + "] after  : " + outputSchema.getAttribute(toName).name);
				}
			}
			else {
				// 指定違反エラー
				logger->error("[" + this->type + "] Arguments are invalid. param : " + this->argument);
			}
		}

		// schemaの設定
		ts.at(0).SetSchema(outputSchema);

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " rename table processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

		// DEBUG 抽出されたタプル情報の出力
		printOutputInfo(ts.at(0));

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - startTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " total processing time: " + to_string(msec) + "[ms]");
#endif

		logger->debug("[" + this->type + "] ========== RenameTable  END  ========== ");
		return true;
	}

}