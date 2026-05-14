#include "CreateOperator.h"

using std::cout;		// cout
using std::endl;		// endl
using namespace std;

namespace IS {

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2019/01/30
	 *
	 * @param	user			ユーザ名
	 * @param	mngId			クエリ管理番号
	 * @param	streamName		ストリーム名
	 * @param	parameter		ストリーム構成カラム情報
	 * @param	streamClass		ストリームクラス情報
	 * @param	classVal		クラスパラメータ情報
	 * @param	streamStartFix	開始調整情報
	 * @param	startFixVal		開始調整パラメータ情報
	 */

	CreateOperator::CreateOperator(const string &user, const unsigned int mngId, const string &streamName, const string &parameter, const string &streamClass, const string &classVal, const string &streamStartFix, const string &startFixVal)
	{
		this->type = MyName + "_MNGID:" + std::to_string(mngId);
		this->user = user;
		this->mngId = mngId;
		this->streamName	= streamName;
		this->parameter		= parameter;
		this->streamClass	= streamClass;
		this->classVal		= classVal;
		this->streamStartFix	= streamStartFix;
		this->startFixVal		= startFixVal;
		this->argument.append(" STREAM_NAME:" + streamName);
		this->argument.append(" COL_INFO:" + parameter);
		if (streamClass.length() != 0) {
			this->argument.append(" CLASS:" + streamClass);
			this->argument.append(" CLASS_VAL:" + classVal);
		}
		if (streamStartFix.length() != 0) {
			this->argument.append(" START_FIX:" + streamStartFix);
			this->argument.append(" START_FIX_VAL:" + startFixVal);
		}
		expect_data_size = 0;
	}

	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2019/01/30
	 */

	CreateOperator::~CreateOperator()
	{
	}

	/**
	* オペレータが期待するタプル数が存在するかを判定する
	* ※複数テーブルを必要とするOperatorはオーバーライドして必要なサイズチェックを実施する
	*
	* @author	Nagoya University
	* @date	2019/01/30
	*
	* @param	bufSize	タプル数
	*
	* @return	期待するタプル数存在する場合にtrue
	*/

	bool CreateOperator::isExpectDataSize(unsigned int bufSize)
	{
		logger->debug("[" + this->type + "] getExpectDataSize:" + std::to_string(expect_data_size) + " bufSize : " + std::to_string(bufSize));
		return (expect_data_size <= bufSize) ? true : false;
	}

	/**
	* オペレータ処理
	*
	* @author	Nagoya University
	* @date	2019/01/30
	*
	* @param [in,out]	ts	タプルセット
	*
	* @return	正常にデータ処理を実施でき、次のオペレータに渡す際にtrue
	*/

	bool CreateOperator::process(vector<IS::TupleSet>& ts)
	{
		logger->debug("[" + this->type + "] ========== CreateOperator START ========== ");
		IS::TupleSet retTupleSet;
		long startFixLongVal = -1;

		// DEBUG 与えられたタプル情報の出力
		printInputInfo(retTupleSet, argument);

		// ストリームの生成
		IS::QueueManager &QM = IS::QueueManager::get_instance();
		vector<Attribute> attrs;
		for (string colInfo : stringUtil.split(this->parameter, ",")) {
			colInfo = stringUtil.trim(colInfo);
			vector<string> col = stringUtil.split(colInfo, " ");
			Attribute attr;
			attr.name = col[0];
			attr.type = col[1];
			if (col.size() == 4 && strcasecmp(col[2].c_str() ,"default") == 0){
				logger->debug("COLUMN has default value -> " +  col[3]);
				attr.defaultValue = col[3];  /* default句対応 */
			}
		
			attrs.push_back(attr);
		}
		// ストリームクラス別のチェック処理
		if (strcasecmp(this->streamClass.c_str(), "timer_fix") == 0) {
			if ( !(attrs.size() == 1 && stringUtil.checkDataTypeMatches("long", attrs[0].type, false))) {
				IS::QueryManager &QeryM = IS::QueryManager::get_instance();
				QeryM.returnError(mngId, ErrorCode::INVALID_ARG_ERR, " The definition of TIMER_FIX type requires one column and long type.");
				return false;
			}
		}

		Schema schema(this->streamName, attrs, Schema::STREAM);
		schema.appendAdminColumn();
		schema.setAccessPrivileges(this->user + "=arR");
		Queue* queue = QM.createQueue(this->streamName, schema);
		if (queue == NULL) {
			IS::QueryManager &QeryM = IS::QueryManager::get_instance();
			QeryM.returnError(mngId, ErrorCode::INVALID_ARG_ERR, " Target stream already exists. streamName: " + this->streamName);
			return false;
		}
		retTupleSet.SetSchema(schema);

		if (streamStartFix.length() != 0) {
			long tmp = std::stol(startFixVal);
			if (tmp >= 0 && tmp < 100) {
				startFixLongVal = tmp;
			} else {
				logger->warn("[" + this->type + "] START_FIX illegal Argument [0 < x < 100]:" + startFixVal);
			}
		}
		// ストリームクラス別の付加処理
		if (strcasecmp(this->streamClass.c_str(), "timer_fix") == 0) {
			// タイマーを設定する
			queue->settingTimer(this->user, std::stol(this->classVal), startFixLongVal, schema);
		}
		else if (strcasecmp(this->streamClass.c_str(), "timer_cron") == 0) {
			// TODO : Cron文字列を扱えるライブラリの導入
		}
		else if (this->streamClass != ""){
			logger->error("[" + this->type + "] Not defined stream_class :" + this->streamClass);
		}

		retTupleSet.info = ts.at(0).info;
		ts.clear();
		ts.push_back(retTupleSet);

		logger->debug("[" + this->type + "] ========== CreateOperator  END  ========== ");
		return true;
	}

}
