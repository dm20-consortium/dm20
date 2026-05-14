#include "DropOperator.h"

using std::cout;		// cout
using std::endl;		// endl
using namespace std;

namespace IS {

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2019/04/17
	 *
	 * @param	user		ユーザ名
	 * @param	mngId		クエリ管理番号
	 * @param	streamName	ストリーム名
	 */

	DropOperator::DropOperator(const string &user, const unsigned int mngId, const string &streamName)
	{
		this->type = MyName + "_MNGID:" + std::to_string(mngId);
		this->user = user;
		this->mngId = mngId;
		this->streamName	= streamName;
		expect_data_size = 0;
	}

	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2019/04/17
	 */

	DropOperator::~DropOperator()
	{
	}

	/**
	* オペレータが期待するタプル数が存在するかを判定する
	* ※複数テーブルを必要とするOperatorはオーバーライドして必要なサイズチェックを実施する
	*
	* @author	Nagoya University
	* @date	2019/04/17
	*
	* @param	bufSize	タプル数
	*
	* @return	期待するタプル数存在する場合にtrue
	*/

	bool DropOperator::isExpectDataSize(unsigned int bufSize)
	{
		logger->debug("[" + this->type + "] getExpectDataSize:" + std::to_string(expect_data_size) + " bufSize : " + std::to_string(bufSize));
		return (expect_data_size <= bufSize) ? true : false;
	}

	/**
	* オペレータ処理
	*
	* @author	Nagoya University
	* @date	2019/04/17
	*
	* @param [in,out]	ts	タプルセット
	*
	* @return	正常にデータ処理を実施でき、次のオペレータに渡す際にtrue
	*/

	bool DropOperator::process(vector<IS::TupleSet>& ts)
	{
		logger->debug("[" + this->type + "] ========== DropOperator START ========== ");
		IS::TupleSet retTupleSet;

		// DEBUG 与えられたタプル情報の出力
		printInputInfo(retTupleSet, argument);
		Schema schema(this->streamName, Schema::STREAM);
		retTupleSet.SetSchema(schema);

		// ストリームの削除
		IS::QueueManager &QM = IS::QueueManager::get_instance();
		if (QM.removeQueue(this->streamName) != true) {
			string retErrMsg = "Target stream already delete, or stream has any listener, So Can't remove stream.";
			logger->error("[" + this->type + "] " + retErrMsg);
			IS::QueryManager &QeryM = IS::QueryManager::get_instance();
			QeryM.returnError(mngId, ErrorCode::INVALID_ARG_ERR, retErrMsg);
			return false;
		}

		retTupleSet.info = ts.at(0).info;
		ts.clear();
		ts.push_back(retTupleSet);

		logger->debug("[" + this->type + "] ========== DropOperator  END  ========== ");
		return true;
	}

}