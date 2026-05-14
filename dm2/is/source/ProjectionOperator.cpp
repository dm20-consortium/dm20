#include "ProjectionOperator.h"

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

	ProjectionOperator::ProjectionOperator(unsigned int mngId, string param)
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
	 * @date	2018/03/13
	 */

	ProjectionOperator::~ProjectionOperator()
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

	bool ProjectionOperator::process(vector<IS::TupleSet>& ts)
	{
		logger->debug("[" + this->type + "] ========== Projection START ========== ");
		IS::TupleSet retTupleSet;
		vector<string> delList = { "'" };

#if MEASURE_MODE == 1
		long startTime = DmUtil::getTimeMicrosec();
		long procTime = startTime;
		int step = 1;
#endif

		// Selectionは1つのtuplesetを使用
		TupleSet& tupleset = ts.at(0);

		// DEBUG 与えられたタプル情報の出力
		printInputInfo(tupleset, this->argument);

#if MEASURE_MODE == 1
		long now = DmUtil::getTimeMicrosec();
		double msec = (now - procTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " parameter analytics processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

		try {
			// 初回処理時、もしくは前回からAttributeサイズに変化がある場合(再帰クエリ)
			if (indexList.size() == 0 || previousAttributeNum != tupleset.getSchemaRef().getAttributeSize()) {
				indexList.clear();
				invalidIndexList.clear();
				previousAttributeNum = tupleset.getSchemaRef().getAttributeSize();

				// parameterを分解
				if (paramList.size() == 0) {
					parameter = stringUtil.deleteSpace(this->argument);
					stringUtil.splitExcludeBrackets(parameter, ",", paramList);
				}

				// schemaの設定
				// 初回時にスキーマ情報と変換テーブルを作成する
				outputSchema = IS::Schema(tupleset.getSchemaRef().getTableName(), tupleset.getSchemaRef().getTableType());

				for (unsigned int i = 0; i < paramList.size(); i++) {
					//bool isCalc = false;
					//REL_COMMENT logger->trace("[" + this->type + "] name:" + paramList.at(i));
					int minIdx = -1;
					int maxIdx = -1;
					if (tupleset.getSchemaRef().isExistAttribute(paramList.at(i), minIdx, maxIdx)) {
						if (minIdx == -1) {
							// 関数列でない、通常の列（属性）の場合
							int index = tupleset.getSchemaRef().getAttributeIdx(paramList.at(i));
							outputSchema.append(tupleset.getSchemaRef().getAttribute(index));
							indexList.push_back(index);
							//REL_COMMENT logger->trace("[" + this->type + "] name:" + paramList.at(i) + " index:" + to_string(index));
						} else {
							// 関数列の場合、戻り値の範囲をチェック
							for (int index = minIdx; index <= maxIdx; index++) {
								outputSchema.append(tupleset.getSchemaRef().getAttribute(index));
								indexList.push_back(index);
							}
						}
					}
					// 数値が指定されている場合はそのまま登録
					else if (stringUtil.isNumber(paramList.at(i))) {
						IS::Attribute attr;
						attr.name = paramList.at(i);
						if (paramList.at(i).find('.') != std::string::npos) {
							attr.type = "double";
						}
						else {
							attr.type = "int";
						}
						outputSchema.append(attr);
						indexList.push_back(static_cast<int>(VAL_TYPE::NUMBER));
						//REL_COMMENT logger->trace("[" + this->type + "] VAL_TYPE::NUMBER");
					}
					else if (stringUtil.isString(paramList.at(i))) {
						IS::Attribute attr;
						stringUtil.deleteStrings(paramList.at(i), delList);
						attr.name = paramList.at(i);
						attr.type = "string";
						outputSchema.append(attr);
						indexList.push_back(static_cast<int>(VAL_TYPE::STRING));
						//REL_COMMENT logger->trace("[" + this->type + "] VAL_TYPE::STRING");
					}
					// クエリパーサーが四則演算に対応していないためコメント
					//// 計算を必要とする指定である場合
					//else if (ProjectionOperator::calcCheck(paramList.at(i))) {
					//	isCalc = true;
					//	// TODO : 括弧演算子は一旦削除
					//	vector<string> delStr = { "(",")" };
					//	stringUtil.deleteStrings(paramList.at(i), delStr);
					//}
					else {
						logger->error("[" + this->type + "] Not found projection column:" + paramList.at(i));
						invalidIndexList.push_back(i);
					}
					//cout << "i:" << i << ",val:" << paramList.at(i) << "Size:" <<  paramList.size() << endl;
				}
				// 上記のカラム特定に１個以上失敗した時に、テーブルの全属性と合わせて、エラー表示する。
				// 既に上記で出力しているため、コメントアウト
				// if (paramList.size() != idx) {
				// 	string paramStr, attrStr;
				// 	for (unsigned int i = 0; i < paramList.size(); i++) { if (paramStr.length() != 0) paramStr.append(","); paramStr.append(paramList.at(i)); }
				// 	for (Attribute attr : tupleset.getSchemaRef().getAttributes()) { if (attrStr.length() != 0) attrStr.append(","); attrStr.append(attr.name); }
				// 	logger->error("[" + this->type + "] Not found target column. paramList.size:" + to_string(paramList.size()) + " idx:" + to_string(idx));
				// 	logger->error("[" + this->type + "] paramList:" + paramStr);
				// 	logger->error("[" + this->type + "] attrList:" + attrStr);
				// }
				unsigned int adminAttrSize = tupleset.getSchemaRef().getAdminAttributeSize();
				unsigned int adminStartIdx = tupleset.getSchemaRef().getAttributeSize() - adminAttrSize;
				for (unsigned int i = 0; i < adminAttrSize; i++) {
					string attrName = tupleset.getSchemaRef().getAttributeName(adminStartIdx + i);
					int index = tupleset.getSchemaRef().getAttributeIdx(attrName);
					outputSchema.append(tupleset.getSchemaRef().getAttribute(index));
					indexList.push_back(index);
				}
			}
			retTupleSet.SetSchema(outputSchema);

#if MEASURE_MODE == 1
			now = DmUtil::getTimeMicrosec();
			msec = (now - procTime) / 1000.0;
			logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " schema settings processing time: " + to_string(msec) + "[ms]");
			procTime = now;
#endif

			any val;
			long timestamp;
			Tuple newTuple(indexList.size());
			retTupleSet.resize(tupleset.size());

			// 指定されたAttributeを抽出
			for (int tupleIdx = 0; tupleIdx < tupleset.size(); tupleIdx++) {
				newTuple.clearValue();
				unsigned int idx = 0;

				for (unsigned int i = 0; i < indexList.size(); i++) {
					if (indexList[i] >= 0) {
						tupleset.getTuple(tupleIdx).getValue(indexList[i], val, timestamp);
						newTuple.setValue(idx, val, timestamp);
						idx++;
						continue;
					}
					else if (indexList[i] == static_cast<int>(VAL_TYPE::NUMBER)) {
						if (paramList[i].find('.') != std::string::npos) {
							newTuple.setValue(idx, std::stod(paramList[i]), 0);
						}
						else {
							newTuple.setValue(idx, std::stoi(paramList[i]), 0);
						}
						idx++;
						continue;
					}
					else if (indexList[i] == static_cast<int>(VAL_TYPE::STRING)) {
						newTuple.setValue(idx, paramList[i], 0);
						idx++;
						continue;
					}
					// クエリパーサーが四則演算に対応していないためコメント
					//else if (indexList[i] == static_cast<int>(VAL_TYPE::CALC)) {
					//	for (int i = 0; i < tupleset.getTuple(tupleIdx).size(); i++) {
					//		tupleset.getTuple(tupleIdx).getValue(i, val, attrName, timestamp);
					//		attrName = tupleset.getSchema().getAttribute(i).name;

					//		// TODO : 計算式に存在するカラム名を採用するが、複数のカラムを用いる式の場合は要検討
					//		if (paramList[i].find(attrName) != std::string::npos) {
					//			// カラム名を値に置換
					//			stringUtil.replaceOnce(paramList[i], attrName, stringUtil.getAnyString(val));
					//			if (attrName.find(".") != string::npos) {
					//				// スキーマ指定がある場合は指定なしでも一応置換
					//				stringUtil.replaceOnce(paramList[i], stringUtil.split(attrName, ".")[1], stringUtil.getAnyString(val));
					//			}
					//			// 計算
					//			double ret = calcUtil.CalcString(paramList[i]);
					//			any retVal = stringUtil.getAnyValFromStringAndAny(std::to_string(ret), val);

					//			// 値を登録
					//			newTuple.setValue(idx, retVal, attrName, timestamp);
					//			idx++;
					//			break;
					//		}
					//	}
					//}
				}
				retTupleSet.setTuple(tupleIdx, newTuple);
			}
			if (invalidIndexList.size() > 0) {
				for (unsigned int i = 0; i < invalidIndexList.size(); i++) {
					logger->error("[" + this->type + "] Not found Target Column. name:" + paramList[invalidIndexList[i]]);
				}
			}
#if MEASURE_MODE == 1
			now = DmUtil::getTimeMicrosec();
			msec = (now - procTime) / 1000.0;
			logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " projection processing time: " + to_string(msec) + "[ms]");
			procTime = now;
#endif

		}
		catch (invalid_argument &e) {
			string what(e.what());
			logger->error("[" + this->type + "] invalid_argument error. what:" + what);
			return false;
		}
		catch (exception &e) {
			string what(e.what());
			logger->error("[" + this->type + "] exception error. what:" + what);
			return false;
		}

		retTupleSet.info = tupleset.info;
		ts.clear();
		ts.push_back(retTupleSet);

		// DEBUG 抽出されたタプル情報の出力
		printOutputInfo(retTupleSet);

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - startTime) / 1000.0;
		logger->info("[" + this->type + "] STAT_STEP" + to_string(step++) + " total processing time: " + to_string(msec) + "[ms]");
#endif

		logger->debug("[" + this->type + "] ========== Projection  END  ========== ");
		return true;
	}

	/**
	 * 計算指定かを判断する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	state	評価式
	 *
	 * @return	True	計算式あり
	 * 			false	計算式なし
	 */

	bool ProjectionOperator::calcCheck(const string &state)
	{
		vector<string> opeList = { "+","-","*","/" };

		for (string ope : opeList)
		{
			if (state.find(ope) != std::string::npos) {
				return true;
			}
		}
		return false;
	}

}
