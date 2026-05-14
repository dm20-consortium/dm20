#include "DmiOperator.h"

namespace IS {

	/**
	 * コンストラクタ
	 *
	 * @author	Naoto Komatsu
	 * @date	2024/02/01
	 *
	 * @param	user		ユーザ名
	 * @param	mngId		クエリ管理番号
	 * @param	streamName	ストリーム名
	 * @param	parameter	ストリーム構成カラム情報
	 * @param	streamClass	ストリームクラス情報
	 * @param	classVal	クラスパラメータ情報
	 */

	DmiOperator::DmiOperator(const string &user, unsigned int mngId, const RecvData &data, const string& dmi)
	{
		this->type = MyName + "_MNGID:" + std::to_string(mngId);
		this->user = user;
		this->mngId = mngId;
		this->streamName	= streamName;
		this->dmiName = dmi;
	}

	/**
	 * デストラクタ
	 *
	 * @author	Naoto Komatsu
	 * @date	2024/02/01
	 */

	DmiOperator::~DmiOperator()
	{
	}

	/**
	* オペレータ処理
	*
	* @author	Naoto Komatsu
	* @date	2024/02/01
	*
	* @param [in,out]	ts	タプルセット
	*
	* @return	正常にデータ処理を実施でき、次のオペレータに渡す際にtrue
	*/

	bool DmiOperator::process(vector<IS::TupleSet>& ts)
	{
		logger->debug("[" + this->type + "] ========== DmiOperator START ========== ");
		
		TupleSet& tupleset = ts.at(0);

		{
			long recvTime = DmUtil::getTimeMicrosec();
			Schema schema = tupleset.getSchemaRef();
			string tablename = schema.getTableName();
			for (int idx = 0; idx < tupleset.size(); idx++) {
				any val;
				int tsGrpIdx = schema.getAttributeIdx(tablename + "." + Schema::COL_TS_GRP);
				if (tsGrpIdx != -1) {
					tupleset.getTuple(idx).getValueByIdx(tsGrpIdx, val);
					string strVal = stringUtil.getAnyString(val) + to_string(recvTime) + ",";
					tupleset.getTuple(idx).setValue(tsGrpIdx, strVal, 0, false);
				}
			}
		}
		
		Schema schema = tupleset.getSchemaRef();
		string tablename = schema.getTableName();

		if (this->dmiName == "ros") {
			ros2dmiCtrl.wrapPublishTo(tupleset, tablename);
		}
		else if (this->dmiName == "udp") {
			if (tupleset.size() != 0) {
				udpdmiCtrl.wrapSendTo(tupleset, tablename);
			}
		} else if (this->dmiName == "protobuf") {
				pbdmiCtrl.wrapSendTo(tupleset, tablename);
    }
		else {
			/* nothing to do */
		}
		

		

		logger->debug("[" + this->type + "] ========== DmiOperator  END  ========== ");
		return true;
	}
}
