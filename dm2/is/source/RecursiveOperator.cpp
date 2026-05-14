#include "RecursiveOperator.h"
#include "Settings.h"

using namespace std;

namespace IS {

	TupleSet prevTupleSet;

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param 	mngId	クエリ管理番号
	 * @param	param	The parameter.
	 * @param	masterName	トリガー
	 */

	RecursiveOperator::RecursiveOperator(unsigned int mngId, string param, string masterName)
	{
		this->type = MyName + "_MNGID:" + std::to_string(mngId);
		this->mngId = mngId;
		this->argument = param;
		this->masterName = masterName;
		expect_data_size = 1;
		// キュー用メモリの用意
		if (this->masterName.length() > 0) {
			IS::QueueManager &QM = IS::QueueManager::get_instance();
			// マスター句に指定されたトリガーとなるストリームにリスナーを登録
			QM.addQueueListener(this->masterName, this);
		}
	}

	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	RecursiveOperator::~RecursiveOperator()
	{
		if (this->masterName.length() > 0) {
			IS::QueueManager &QM = IS::QueueManager::get_instance();
			// マスター句に指定されたトリガーリスナーを削除
			QM.removeQueueListener(this->masterName, this);
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

	bool RecursiveOperator::process(vector<IS::TupleSet>& ts)
	{
		logger->debug("[" + this->type + "] ========== Recursive START ========== ");
		IS::TupleSet retTupleSet;
		bool ret = true;

		// DEBUG 与えられたタプル情報の出力
		printInputInfos(ts, this->argument);

		// TupleSetが存在しない場合
		// 指定されたテーブルを作成する
		if (ts.size() == 0 || ts.at(0).size() == 0) {

			IS::Schema schema = IS::Schema(this->argument, IS::Schema::RDB);
			retTupleSet.SetSchema(schema);
			retTupleSet.info = ts.at(0).info;

			ts.clear();
			ts.push_back(retTupleSet);
			returnEvent(ts.at(0), this);
			ret = false;

			// TupleSetを保持
			prevTupleSet = retTupleSet;
		}
		// INPUTデータが存在する場合
		// 渡されたTupleSetをargumentに指定されたテーブル名に変更する
		else
		{
			// テーブル名の変更
			IS::Schema schema = IS::Schema(this->argument, IS::Schema::STREAM);
			string toName = "";
			for (Attribute attr : ts.at(0).getSchemaRef().getAttributes()) {
				if (attr.name.find(".") != std::string::npos) {
					toName = this->argument + "." + stringUtil.split(attr.name,".")[1];
				}
				else {
					toName = this->argument + "." + attr.name;
				}
				attr.name = toName;
				schema.append(attr);
			}
			retTupleSet.SetSchema(schema);

			for (Tuple tuple : ts.at(0).getTuples())
			{
				retTupleSet.addTuple(tuple);
			}
			retTupleSet.info = ts.at(0).info;
			retTupleSet.info.isRecursive = true;

			ts.clear();
			ts.push_back(retTupleSet);
			
			// 前回保持したデータと渡されたデータに差異があるかを確認
			if (prevTupleSet.size() != ts.at(0).size())
			{
				// 差異がある場合は再帰処理を実行
				returnEvent(ts.at(0), this);
				ret = false;
			}
			else
			{
				// 差異がない場合は次オペレータへ処理を委譲
				// 性質上、初期項が必ず列の最後尾になるため、最後にリバースして順序を入れ替える
				retTupleSet.reverse();
				ts.clear();
				ts.push_back(retTupleSet);

			}
			prevTupleSet = retTupleSet;

		}

		// DEBUG 抽出されたタプル情報の出力
		printOutputInfo(retTupleSet);

		logger->debug("[" + this->type + "] ========== Recursive  END  ========== ");
		return ret;
	}
}