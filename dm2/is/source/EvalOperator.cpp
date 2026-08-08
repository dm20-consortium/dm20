#include "EvalOperator.h"
#include "is/TupleSet.h"
#include "is/DmException.h"

using std::cout;		// cout
using std::endl;		// endl
using namespace std;

namespace IS {
	// 初期化
	static const string  LIB_PREFIX = "_LIB";
	static const string  ARG_TYPE_PREFIX = "_ARG_TYPE";
	static const string  RET_TYPE_PREFIX = "_RET_TYPE";
	static const string  RET_NAME_PREFIX = "_RET_NAME";
	static const string  AGG_FLAG_PREFIX = "_AGG_FLAG";

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	mngId	クエリ管理番号
	 * @param	param	実行パラメータ
	 */

	EvalOperator::EvalOperator(unsigned int mngId, string param)
	{
		this->type = MyName + "_MNGID:" + std::to_string(mngId);
		this->argument = param;
		this->mngId = mngId;
		// キュー用メモリの用意
	}

	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	EvalOperator::~EvalOperator()
	{
		if (evalHandle != NULL) {
			dlclose(evalHandle);
			evalHandle = NULL;
			evalFunc = NULL;
			evalLibLoaded = false;
		}
	}

	/**
	* パラメータをチェックする。クエリ解析時に呼び出される。
	*
	* @author	Nagoya University
	* @date	2019/12/09
	* @return success
	*/

	bool EvalOperator::checkParameter(string &msg, bool &doAggregate)
	{
		if (args.size() == 0) {
			// parameterを分解
			parameter = stringUtil.deleteSpace(this->argument);
			// 関数名(arg1,arg2,...)を想定
			vector<string> params;
			stringUtil.split(parameter, "(", params);
			functionName = params.at(0);
			stringUtil.toUpper(functionName);
			stringUtil.splitExcludeBrackets(stringUtil.getStringInBracketFirst(parameter, "(", ")")[0], ",", args);
		}

		// ユーザ定義関数名を取得
		libName = settings.readConfigFile((functionName + IS::LIB_PREFIX).c_str(), settings.CONF_FILE_EVAL);
		if (libName.length() == 0) {
			msg.append("[" + this->type + "] " + functionName + IS::LIB_PREFIX + " is not defined. file:" + settings.CONF_FILE_EVAL);
			logger->error(msg);
			return false;
		}

		// ユーザ定義関数の引数・戻り値リストを取得
		argTypeStr = settings.readConfigFile((functionName + IS::ARG_TYPE_PREFIX).c_str(), settings.CONF_FILE_EVAL);
		retTypeStr = settings.readConfigFile((functionName + IS::RET_TYPE_PREFIX).c_str(), settings.CONF_FILE_EVAL);
		retNameStr = settings.readConfigFile((functionName + IS::RET_NAME_PREFIX).c_str(), settings.CONF_FILE_EVAL, false);
		string aggFlagStr = settings.readConfigFile((functionName + IS::AGG_FLAG_PREFIX).c_str(), settings.CONF_FILE_EVAL, false);
		stringUtil.split(retTypeStr, ",", retsType);
		if (!retNameStr.empty()) {
			stringUtil.split(retNameStr, ",", retsName);
			// 別名指定ありだが、型の数と不一致
			if (retsType.size() != retsName.size()) {
				msg.append("[" + this->type + "] Lack of parameter. RET_TYPE size:" + std::to_string(retsType.size()) + " RET__NAME size:" + std::to_string(retsName.size()));
				logger->error(msg);
				return false;
			}
		}
		if (!aggFlagStr.empty()) {
			stringUtil.toUpper(aggFlagStr);
			if (aggFlagStr == "TRUE") doAggregate = true;
		}
		return true;
	}

	/**
	 * 初期化処理
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in]	tupleset	タプルセット
	 *
	 */
	void EvalOperator::initializeEval(TupleSet& tupleset)
	{
		// 初回処理時、もしくは前回からAttributeサイズに変化がある場合(再帰クエリ)
		if (argProcList.size() == 0 || previousAttributeNum != tupleset.getSchemaRef().getAttributeSize()) {
			argProcList.clear();
			retsName.clear();
			previousAttributeNum = tupleset.getSchemaRef().getAttributeSize();

			// 引数に応じた処理を予め把握しておく
			string targetName;
			string type = "";
			for (unsigned int i = 0; i < args.size(); i++) {
				targetName = args.at(i);
				if (tupleset.getSchemaRef().isExistAttribute(targetName)) {
					argProcList.push_back(tupleset.getSchemaRef().getAttributeIdx(targetName));
					type = to_string(tupleset.getSchemaRef().getAttributeIdx(targetName));
				}
				else {
					if (stringUtil.isNumber(targetName)) {
						argProcList.push_back(static_cast<int>(procType::NUMBER));
						type = "NUMBER";
					}
					else if (stringUtil.isString(targetName)) {
						argProcList.push_back(static_cast<int>(procType::STRING));
						type = "STRING";
					}
					else {
						logger->debug("[" + this->type + "] Not found target column. name:" + targetName);
						// 指定フィールドが見つからなかった場合は直値と判断
						argProcList.push_back(static_cast<int>(procType::OTHER));
						type = "OTHER";
					}
				}
				logger->debug("Idx: " + std::to_string(i) + ", name: " + targetName + ", type: " + type);
			}

			// schemaの設定
			outputSchema = IS::Schema(tupleset.getSchemaRef().getTableName(), tupleset.getSchemaRef().getTableType());

			// schema情報の更新
			if (retsType.size() > 1) {
				outputSchema.copyAttributes(tupleset.getSchemaRef());
				if (retNameStr.length() > 0) {
					stringUtil.split(retNameStr, ",", retsName);
				}
				for (unsigned int i = 0; i < retsType.size(); i++) {
					IS::Attribute attr;
					if (retNameStr.length() == 0) {
						attr.name = functionName + "_" + std::to_string(i);
						retsName.push_back(attr.name);
					}
					else {
						attr.name = retsName.at(i);
					}
					attr.type = retsType.at(i);
					outputSchema.append(attr);
				}
			}
			else {
				// 戻り値が単一の場合
				outputSchema.copyAttributes(tupleset.getSchemaRef());

				IS::Attribute attr;
				attr.name = parameter;
				attr.type = retTypeStr;
				outputSchema.append(attr);
				retsName.push_back(attr.name);
			}
		}

	}
	/**
	 * TupleSetからユーザ定義関数に渡す引数リストへ変換する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in]	tupleset	タプルセット
	 * @param [out]	argList	引数リスト
	 *
	 * @return	最新時刻
	 */
	long EvalOperator::createArgList(TupleSet& tupleset, vector<vector<string>>& argList)
	{
		argList.resize(tupleset.size());
		
		any val;
		long time = 0, getTime = 0;
		// Tuple毎に処理を実施
		for (int idx = 0; idx < tupleset.size(); idx++) {
			time = 0;
			for (unsigned int i = 0; i < args.size(); i++) {
				string targetName = args.at(i);
				vector<string> delList = { "'","\"" };
				switch (argProcList.at(i)) {
				case static_cast<int>(procType::NUMBER) :
					// 数値である場合
					argList.at(idx).push_back(targetName);
					break;
				case static_cast<int>(procType::STRING) :
					// 文字列である場合
					stringUtil.deleteStrings(targetName, delList);
					argList.at(idx).push_back(targetName);
					break;
				case static_cast<int>(procType::OTHER) :
					// 指定された属性が見つからなかった場合は直値と判断
					argList.at(idx).push_back(targetName);
					break;
				default :
					// 指定された属性が見つかった場合は値をListに加える
					tupleset.getTuple(idx).getValueByIdx(argProcList.at(i), val);
					argList.at(idx).push_back(stringUtil.getAnyString(val));
					tupleset.getTuple(idx).getTimestampByIdx(argProcList.at(i), getTime);
					// 属性の中で最新の値をセット
					if (time < getTime) time = getTime;
					//logger->info("[" + this->type + "] argList[" + std::to_string(idx) + "][" + std::to_string(i) + "].push_back: columnName(" + tupleset.getSchema().getAttributeName(argProcList.at(i)) + "):" + stringUtil.getAnyString(val));
					break;

				}
			}
			// 属性なしの場合は現在時刻をセット
			// ToDo: 元の属性を引き継ぐべきか？入れ子になった場合、引き継げるか？
			if (time == 0) time = DmUtil::getTimeMillisec();
			// 引数の末尾に時刻をセット
			//argList.at(idx).push_back(stringUtil.getAnyString(time));
		}
		return time;
	}
	/**
	 * 戻り値リスト作成
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in]	tupleset	タプルセット
	 * @param [out]	retList	戻り値リスト
	 * @param [in]	retList	最新時刻
	 *
	 */
	void EvalOperator::appendReturnValue(TupleSet& tupleset, const vector<vector<string>>& retList, const long &time)
	{
		if (retList.size() <= 0) {
			tupleset.resize(0);
			logger->warn("[" + this->type + "] Data was not returned. parameter:" + parameter);
			return;
		}
		if (retList.size() == (unsigned int)tupleset.size() && retsName.size() == 1) {
			// 行数が一致する場合
			//REL_COMMENT logger->trace("[" + this->type + "] rows num is same. Append column. retList.size():" + std::to_string(retList.size()) + " tupleset.size():" + std::to_string(tupleset.size()));

			int resize = tupleset.getTuple(0).size() + retList.at(0).size();
			int insertColNum = tupleset.getTuple(0).size();
			//REL_COMMENT logger->trace("[" + this->type + "] resize:" + to_string(resize) + " insertCOlNum:" + to_string(insertColNum));
			any val;
			for (int idx = 0; idx < tupleset.size(); idx++) {
				tupleset.getTuple(idx).resize(resize);
				for (unsigned int i = 0; i < retList.at(0).size(); i++) {
					stringUtil.getAnyValFromString(retList.at(idx).at(i), retsType.at(i), val);
					tupleset.getTuple(idx).setValue(insertColNum + i, val, time);
				}
			}
		}
		else {
			// 行数が不一致の場合
			int resize = tupleset.getTuple(0).size() + retList.at(0).size();
			int insertColNum = tupleset.getTuple(0).size();
			tupleset.resize(retList.size());
			unsigned int setIndex = 0;
			any val;
			for (unsigned int idx = 0; idx < retList.size(); idx++) {
				Tuple tuple = tupleset.getTuple(idx);
				tuple.resize(resize);
				for (unsigned int i = 0; i < retList.at(0).size(); i++) {
					stringUtil.getAnyValFromString(retList.at(idx).at(i), retsType.at(i), val);
					tuple.setValue(insertColNum + i, val, time);
				}
				tupleset.setTuple(setIndex++, tuple);
			}
			outputSchema.appendEvalColumnRange(parameter, insertColNum, insertColNum + retList.at(0).size() - 1);
			tupleset.SetSchema(outputSchema);
			
			// 下記の方法は不採用。全タプルをクリアした上でセットしている。このやり方だと、a, b とカラムがある中、UF_PLUS(a)を呼び出した時、a, b列が消える。
			/*
			tupleset.clearTuples();
			tupleset.resize(retList.size());
			Tuple tuple(retList.at(0).size());
			unsigned int setIndex = 0;
			any val;
			for (unsigned int idx = 0; idx < retList.size(); idx++) {
				tuple.clearValue();
				for (unsigned int i = 0; i < retList.at(0).size(); i++) {
					stringUtil.getAnyValFromString(retList.at(idx).at(i), retsType.at(i), val);
					tuple.setValue(i, val, time);
				}
				tupleset.setTuple(setIndex++, tuple);
			}
			*/
		}
		return;
	}
	/**
	 * ユーザ定義関数のライブラリロード処理
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @return	正常にロードできたか？
	 */
	bool EvalOperator::loadEvalFunction() 
	{
		if (evalLibLoaded) return true;

		string evalLibPath = settings.getConfigDirectory() + settings.EVAL_LIB_DIR + libName;
		evalHandle = dlopen(evalLibPath.c_str(), RTLD_LAZY);
		if (evalHandle == NULL) {
			logger->error("[" + this->type + "] error: dlopen. libName : " + libName);
			logger->error(std::string(dlerror()));
			return false;
		}

		evalFunc = (multiFunc)dlsym(evalHandle, functionName.c_str());
		if (evalFunc == NULL) {
			logger->error("[" + this->type + "] error: dlsym. info : " + std::string(dlerror()));
			dlclose(evalHandle);
			return false;
		}
		evalLibLoaded = true;
		return true;
	}
	/**
	 * ユーザ定義関数の実行処理
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in]	argList	引数リスト
	 * @param [in,out]	retList	戻り値リスト
	 *
	 * @return	正常にデータ処理を実施でき、次のオペレータに渡す際にtrue
	 */
	bool EvalOperator::executeEvalFunction(const vector<vector<string>>& argList, vector<vector<string>>& retList)
	{
		try {
			// 取得した引数を元に動的関数を実行
			retList = evalFunc(argList);
		}
		catch (const CastException &ex) {
			logger->error("[" + this->type + "] UserFunction(" + functionName + ") throws exception.  msg:" + ex.getMessage());
			return false;
		}
		catch (const exception &ex) {
			string what(ex.what());
			logger->error("[" + this->type + "] UserFunction(" + functionName + ") throws exception. what:" + what);
			return false;
		}
		return true;
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
	bool EvalOperator::process(vector<IS::TupleSet>& ts)
	{
		logger->debug("[" + this->type + "] ========== Eval START ========== ");
		bool ret = true;
		vector<vector<string>> argList, retList;

		// evalは1つのtuplesetを使用
		TupleSet& tupleset = ts.at(0);

		// DEBUG 与えられたタプル情報の出力
		printInputInfo(tupleset, this->argument);

		// 初期化
		initializeEval(tupleset);
		tupleset.SetSchema(outputSchema);

		if (tupleset.size() == 0) {
			logger->debug("[" + this->type + "] ========== Eval  END  ========= ");
			return true;
		}
		// DLLロード
		if (!loadEvalFunction()) {
			return false;
		}

		// ユーザ定義関数用引数リスト作成
		long time = createArgList(tupleset, argList);
		// ユーザ定義関数実行
		if (!executeEvalFunction(argList, retList)) {
			return false;
		}
		// 戻り値をTupleSetへ反映
		appendReturnValue(tupleset, retList, time);

		// DEBUG 抽出されたタプル情報の出力
		printOutputInfo(tupleset);

		logger->debug("[" + this->type + "] ========== Eval  END  ========= ");
		return ret;
	}

}
