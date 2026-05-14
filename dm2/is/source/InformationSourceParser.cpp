#include "InformationSourceParser.h"
#include "is/TupleSet.h"
#include "QueueManager.h"

#include <iostream>
#include <iomanip>
#include <mutex>
#include <string.h>

using namespace std;

namespace IS {

	std::mutex ispMtx;           // mutex for critical section


	/**
	* コンストラクタ
	*
	* @author	Nagoya University
	* @date	2018/04/09
	*
	* @param	tagName	タグ名
	*/

	SaxSingleResultHandler::SaxSingleResultHandler(string tagName) : DefaultHandler(){
		searchTagName = tagName;
	}

	/**
	* エレメント始端のロード時にコールされる
	*
	* @author	Nagoya University
	* @date	2018/04/09
	*
	* @param	uri	URIデータ
	* @param	localname	ローカルネーム
	* @param	qname	エレメント名
	* @param	attrs	Attribute情報
	*/

	void SaxSingleResultHandler::startElement(const XMLCh* const uri, const XMLCh* const localname,
		const XMLCh* const qname, const xercesc::Attributes &attrs) {

	}

	/**
	* エレメント終端のロード時にコールされる
	*
	* @author	Nagoya University
	* @date	2018/04/09
	*
	* @param	uri	URIデータ
	* @param	localname	ローカルネーム
	* @param	qname	エレメント名
	*/

	void SaxSingleResultHandler::endElement(const XMLCh* const uri, const XMLCh* const localname,
		const XMLCh* const qname) {
		char* qnames = XMLString::transcode(qname);
		if (strcmp(searchTagName.c_str(), qnames) == 0) {
			ret = charsStr;
		}
		XMLString::release(&qnames);
		charsStr = "";
	}

	/**
	* テキストデータのロード時にコールされる
	*
	* @author	Nagoya University
	* @date	2018/04/09
	*
	* @param	chars	テキストデータ
	* @param	length	テキストサイズ
	*/

	void SaxSingleResultHandler::characters(const XMLCh* const chars, const XMLSize_t length) {
		char* charss = XMLString::transcode(chars);
		string str = charss;
		if (chars != NULL && (str.find("\n") == std::string::npos)) {
			charsStr = charsStr + str;
		}
		XMLString::release(&charss);
	}

	/**
	* エレメント始端のロード時にコールされる
	*
	* @author	Nagoya University
	* @date	2018/04/09
	*
	* @param	uri	URIデータ
	* @param	localname	ローカルネーム
	* @param	qname	エレメント名
	* @param	attrs	Attribute情報
	*/

	inline void SaxTuplesAllHandler::startElement(const XMLCh* const uri, const XMLCh* const localname,
		const XMLCh* const qname, const xercesc::Attributes &attrs) {
		if (isErr) return;
		char* qnames = XMLString::transcode(qname);
		//cout << "start  : " << qnames << endl;
		if (strcmp(tupleTag, qnames) == 0) {
			tuple = new Tuple(schemaInfo);
			idx = 0;
#if MEASURE_MODE2 == 1
			createTime = DmUtil::getTimeMillisec();
#endif
		}
		for (unsigned int i = 0; i < attrs.getLength(); i++) {
			char *attr = XMLString::transcode(attrs.getLocalName(i));
			if (strcmp(timeAttr, attr) == 0) {
				char *timestamp_c = XMLString::transcode(attrs.getValue(i));
				timestamp = std::string(timestamp_c);
				XMLString::release(&timestamp_c);
			}
			XMLString::release(&attr);
		}

		XMLString::release(&qnames);
	}

	/**
	* エレメント終端のロード時にコールされる
	*
	* @author	Nagoya University
	* @date	2018/04/09
	*
	* @param	uri	URIデータ
	* @param	localname	ローカルネーム
	* @param	qname	エレメント名
	*/

	inline void SaxTuplesAllHandler::endElement(const XMLCh* const uri, const XMLCh* const localname,
		const XMLCh* const qname) {
		if (isErr) return;
		char* qnames = XMLString::transcode(qname);
		//cout << "end    : " << qnames << endl;
		if (strcmp(dataTag, qnames) == 0) {
			if (idx >= schemaInfo.getAttributeSize()) {
				cout << "XML's Attribute Size Over. IndexNo." << idx << " is Ignored. tblName:" << schemaInfo.getTableName();
			} else {
				data = charsStr;
				/* dataが null の場合は Default句で指定されたものがあればそれを設定する。 */
				if (strcasecmp(data.c_str() ,"(null)") == 0){
					string defaultValue = schemaInfo.getAttributeDefaultValue(idx);
					if (!defaultValue.empty() && strcasecmp(defaultValue.c_str(),"null"))
						data = defaultValue;
				}
#if MEASURE_MODE2 != 1
				tuple->setValue(idx, stringUtil.getAnyValFromString(data, schemaInfo.getAttributeType(idx)), std::stol(timestamp));
				// for testInput
#else
				tuple->setValue(idx, stringUtil.getAnyValFromString(data, schemaInfo.getAttributeType(idx)), createTime); // For Edge
				//if(idx==3) { tuple->setValue(idx, stringUtil.getAnyValFromString(data, schemaInfo.getAttributeType(idx)), schemaInfo.getAttributeName(idx), createTime); } else { tuple->setValue(idx, stringUtil.getAnyValFromString(data, schemaInfo.getAttributeType(idx)), schemaInfo.getAttributeName(idx), std::stol(timestamp)); } // For Cloud
#endif
			}
			idx++;
		}
		else if (strcmp(tupleTag, qnames) == 0) {
			ret.push_back(*tuple);
			delete tuple;
		}
		else if (strcmp(tableNameTag, qnames) == 0) {
			IS::QueueManager &QM = IS::QueueManager::get_instance();
			streamName = charsStr;
			QM.getSchema(charsStr, schemaInfo);
			if (schemaInfo.getTableName() == "UNKNOWN") {
				isErr = true;
			}
		}
		XMLString::release(&qnames);
		charsStr = "";
	}

	/**
	* テキストデータのロード時にコールされる
	*
	* @author	Nagoya University
	* @date	2018/04/09
	*
	* @param	chars	テキストデータ
	* @param	length	テキストサイズ
	*/

	inline void SaxTuplesAllHandler::characters(const XMLCh* const chars, const XMLSize_t length) {
		if (isErr) return;
		char* charss = XMLString::transcode(chars);
		if (chars != NULL && strstr(charss,"\n") == NULL) {
			//cout << "Char   : " << charss << "[" << length << "]" << endl;
			charsStr.append(charss);
		}
		XMLString::release(&charss);
	}

	/**
	 * Xercesの初期化処理を実施する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void InformationSourceParser::init()
	{
		//REL_COMMENT logger->trace("InformationSourceParser constracter preIN");
		ispMtx.lock();
		//REL_COMMENT logger->trace("InformationSourceParser constracter IN");
		try {
			XMLPlatformUtils::Initialize();
		}
		catch (const XMLException& toCatch) {
			char* message = XMLString::transcode(toCatch.getMessage());
			string messageStr(message);
			logger->error("Error during initialization! :\n" + messageStr);
			XMLString::release(&message);
			ispMtx.unlock();
			return;
		}
		ispMtx.unlock();
		//REL_COMMENT logger->trace("InformationSourceParser constracter OUT");
	}

	/**
	 * Xercesの終了処理を実施する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	void InformationSourceParser::finalize()
	{
		//REL_COMMENT logger->trace("InformationSourceParser destracter preIN");
		ispMtx.lock();
		//REL_COMMENT logger->trace("InformationSourceParser destracter IN");
		try {
			XMLPlatformUtils::Terminate();
		}
		catch (const XMLException& toCatch) {
			char* message = XMLString::transcode(toCatch.getMessage());
			string messageStr(message);
			logger->error("Error during Terminate! :\n" + messageStr);
			XMLString::release(&message);
			ispMtx.unlock();
			return;
		}
		ispMtx.unlock();
		//REL_COMMENT logger->trace("InformationSourceParser destracter OUT");
	}

	/**
	 * Continuous Query(継続クエリ)発行時のXML文字列を生成する
	 *
	 * @author	Naoto Komatsu
	 * @date	2024/2/1
	 *
	 * @param	query		 	クエリ文字列
	 * @param	receptionPort	受付ポート番号
	 * @param	key		セッションキー
	 * @param [in,out]	result XML文字列
	 *
	 */

	void InformationSourceParser::createQueryXML(const string &query, const int receptionPort, const string &key, const string &dmi, string &result)
	{
		XMLCh tempStr[BUF_MAX];
		XMLCh tempStr2[BUF_MAX];
		XMLString::transcode("Range", tempStr, BUF_MAX - 1);
		DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

		XMLString::transcode("query", tempStr, BUF_MAX - 1);
		DOMDocument*   doc = impl->createDocument(0, tempStr, 0);
		DOMElement*   rootQuery = doc->getDocumentElement();

		// サイズ情報
		XMLString::transcode("size", tempStr, BUF_MAX - 1);
		XMLString::transcode(replaceWord.c_str(), tempStr2, BUF_MAX - 1);
		rootQuery->setAttribute(tempStr, tempStr2);

		// キー情報
		XMLString::transcode("key", tempStr, BUF_MAX - 1);
		XMLString::transcode(key.c_str(), tempStr2, BUF_MAX - 1);
		rootQuery->setAttribute(tempStr, tempStr2);

		// DMI情報
		XMLString::transcode("dmi", tempStr, BUF_MAX - 1);
		XMLString::transcode(dmi.c_str(), tempStr2, BUF_MAX - 1);
		rootQuery->setAttribute(tempStr, tempStr2);

		// sender情報
		DOMElement*   sender = createElement(doc, "sender");
		appendDomText(doc, sender, "");
		rootQuery->appendChild(sender);

		// destination情報
		DOMElement*   destination = createElement(doc, "destination");
		appendDomText(doc, destination, "");
		rootQuery->appendChild(destination);

		// body情報
		DOMElement*   body = createElement(doc, "body");
		appendDomText(doc, body, query);
		rootQuery->appendChild(body);

		// port情報
		DOMElement*   port = createElement(doc, "port");
		appendDomText(doc, port, std::to_string(receptionPort));
		rootQuery->appendChild(port);

		outputXML(doc, result);
		replaceSizeInfo(result);

		// done with the document, must call release() to release the entire document resources
		doc->release();
		return;
	}
	/**
	 * クエリ結果のXML文字列を生成する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	mngId	クエリ管理番号
	 * @param	ts   	タプルセット
	 * @param [in,out]	ret	クエリ結果XML文字列
	 * 
	 */

	void InformationSourceParser::createQueryResult(const unsigned int mngId, TupleSet& ts, string &ret)
	{
		XMLCh tempStr1[BUF_MAX];
		XMLCh tempStr2[BUF_MAX];

		XMLString::transcode("Range", tempStr1, BUF_MAX - 1);
		DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr1);

		XMLString::transcode("result", tempStr1, BUF_MAX - 1);
		DOMDocument*   doc = impl->createDocument(0, tempStr1, 0);
		DOMElement*   result = doc->getDocumentElement();

		// 管理番号
		XMLString::transcode("id", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(mngId).c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// サイズ情報
		XMLString::transcode("size", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWord.c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// スキーマ情報
		DOMElement*   schema = createElement(doc, "schema");
		result->appendChild(schema);

		if (ts.size() > 0) {

			XMLCh chCol[BUF_MAX];
			XMLString::transcode("column", chCol, BUF_MAX - 1);
			XMLCh chName[BUF_MAX];
			XMLString::transcode("name", chName, BUF_MAX - 1);
			XMLCh chType[BUF_MAX];
			XMLString::transcode("type", chType, BUF_MAX - 1);

			// スキーマ情報
			for (Attribute attr : ts.getSchemaRef().getAttributes()) {
				DOMElement*   columnEl = doc->createElement(chCol);
				DOMElement*   nameEl = doc->createElement(chName);
				DOMElement*   typeEl = doc->createElement(chType);
				appendDomText(doc, nameEl, attr.name);
				appendDomText(doc, typeEl, attr.type);
				columnEl->appendChild(nameEl);
				columnEl->appendChild(typeEl);
				schema->appendChild(columnEl);
			}

			any val;
			long timestamp;
			bool isNull;
			XMLCh chTuple[BUF_MAX];
			XMLString::transcode("tuple", chTuple, BUF_MAX - 1);
			XMLCh chData[BUF_MAX];
			XMLString::transcode("data", chData, BUF_MAX - 1);
			XMLCh chTime[BUF_MAX];
			XMLString::transcode("time", chTime, BUF_MAX - 1);
			XMLCh chNull[BUF_MAX];
			XMLString::transcode("isNull", chNull, BUF_MAX - 1);

			// クエリ結果情報
			for (IS::Tuple tuple : ts.getTuples())
			{
				DOMElement*   tupleEl = doc->createElement(chTuple);
				for (int i = 0; i < tuple.size(); i++)
				{
					DOMElement*   dataEl = doc->createElement(chData);

					tuple.getValue(i, val, timestamp, isNull);
					appendDomText(doc, dataEl, stringUtil.getAnyString(val));
					
					if (timestamp != 0) {
						XMLString::transcode(std::to_string(timestamp).c_str(), tempStr2, BUF_MAX - 1);
						dataEl->setAttribute(chTime, tempStr2);
					}
					if (isNull == true) {
						XMLString::transcode(std::to_string(isNull).c_str(), tempStr2, BUF_MAX - 1);
						dataEl->setAttribute(chNull, tempStr2);
					}
					tupleEl->appendChild(dataEl);

				}
				result->appendChild(tupleEl);
			}
		}
		outputXML(doc, ret);
		replaceSizeInfo(ret);

		// done with the document, must call release() to release the entire document resources
		doc->release();
		return;
	}

	/**
	* クエリ結果のXML文字列を生成する(指定したサイズに分割して返却）
	*
	* @author	Nagoya University
	* @date	2018/05/23
	*
	* @param 	mngId	クエリ管理番号
	* @param	ts   	タプルセット
	* @param	sepSize	分割サイズ
	* @param [in,out]	ret	クエリ結果XML文字列配列
	*
	* @return	分割前電文長
	*/

	int InformationSourceParser::createQueryResultList(const unsigned int mngId, TupleSet& ts, const int sepSize, vector<string> &ret)
	{
		XMLCh tempStr1[BUF_MAX];
		XMLCh tempStr2[BUF_MAX];

		XMLString::transcode("Range", tempStr1, BUF_MAX - 1);
		DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr1);

		XMLString::transcode("result", tempStr1, BUF_MAX - 1);
		DOMDocument*   doc = impl->createDocument(0, tempStr1, 0);
		DOMElement*   result = doc->getDocumentElement();

		// 管理番号
		XMLString::transcode("id", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(mngId).c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// 生成時間
		XMLString::transcode("ct", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(DmUtil::getTimeMillisec()).c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// サイズ情報
		XMLString::transcode("size", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWord.c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// フラグメント情報
		XMLString::transcode("flagment", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWordFlg.c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// フラグメント情報(Max)
		XMLString::transcode("flagmentMax", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWordFlgMax.c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// スキーマ情報
		DOMElement*   schema = createElement(doc, "schema");
		result->appendChild(schema);

		if (ts.size() > 0) {

			XMLCh chCol[BUF_MAX];
			XMLString::transcode("column", chCol, BUF_MAX - 1);
			XMLCh chName[BUF_MAX];
			XMLString::transcode("name", chName, BUF_MAX - 1);
			XMLCh chType[BUF_MAX];
			XMLString::transcode("type", chType, BUF_MAX - 1);

			// スキーマ情報
			for (Attribute attr : ts.getSchemaRef().getAttributes()) {
				DOMElement*   columnEl = doc->createElement(chCol);
				DOMElement*   nameEl = doc->createElement(chName);
				DOMElement*   typeEl = doc->createElement(chType);
				appendDomText(doc, nameEl, attr.name);
				appendDomText(doc, typeEl, attr.type);
				columnEl->appendChild(nameEl);
				columnEl->appendChild(typeEl);
				schema->appendChild(columnEl);
			}

			any val;
			long timestamp;
			bool isNull;
			XMLCh chTuple[BUF_MAX];
			XMLString::transcode("tuple", chTuple, BUF_MAX - 1);
			XMLCh chData[BUF_MAX];
			XMLString::transcode("data", chData, BUF_MAX - 1);
			XMLCh chTime[BUF_MAX];
			XMLString::transcode("time", chTime, BUF_MAX - 1);
			XMLCh chNull[BUF_MAX];
			XMLString::transcode("isNull", chNull, BUF_MAX - 1);

			// クエリ結果情報
			for (IS::Tuple tuple : ts.getTuples())
			{
				DOMElement*   tupleEl = doc->createElement(chTuple);
				for (int i = 0; i < tuple.size(); i++)
				{
					DOMElement*   dataEl = doc->createElement(chData);

					tuple.getValue(i, val, timestamp, isNull);
					appendDomText(doc, dataEl, stringUtil.getAnyString(val));

					if (timestamp != 0) {
						XMLString::transcode(std::to_string(timestamp).c_str(), tempStr2, BUF_MAX - 1);
						dataEl->setAttribute(chTime, tempStr2);
					}
					if (isNull == true) {
						XMLString::transcode(std::to_string(isNull).c_str(), tempStr2, BUF_MAX - 1);
						dataEl->setAttribute(chNull, tempStr2);
					}
					tupleEl->appendChild(dataEl);

				}
				result->appendChild(tupleEl);
			}
		}
		string xmlResult = "";
		outputXML(doc, xmlResult);

		int len = convertToXMLList(xmlResult, ret, sepSize);

		// done with the document, must call release() to release the entire document resources
		doc->release();
		return len;
	}


	/**
	* クエリ結果のXML文字列を生成する
	* ※大量のタプルをDOMAPIにてXML生成するとコストがかかるため文字列結合にて生成する
	*
	* @author	Nagoya University
	* @date	2020/02/14
	*
	* @param	mngId	クエリ管理番号
	* @param	ts   	タプルセット
	* @param [in,out]	ret	クエリ結果XML文字列
	*
	*/

	void InformationSourceParser::createQueryResultStr(const unsigned int mngId, TupleSet& ts, string &ret, const int tcpPort)
	{
		ret.append("\r\n<result id=\"");
		ret.append(to_string(mngId));
		ret.append("\" port=\"");
		ret.append(to_string(tcpPort));
		ret.append("\" size=\"");
		ret.append(replaceWord);
		ret.append("\">\n");

		appendSchemaAndTupleStr(ts, ret);

		ret.append("</result>");
		replaceSizeInfo(ret);
		return;
	}

	/**
	* クエリ結果のXML文字列を生成する(指定したサイズに分割して返却）
	* ※大量のタプルをDOMAPIにてXML生成するとコストがかかるため文字列結合にて生成する
	*
	* @author	Nagoya University
	* @date	2020/02/14
	*
	* @param 	mngId	クエリ管理番号
	* @param	ts   	タプルセット
	* @param	sepSize	分割サイズ
	* @param [in,out]	ret	クエリ結果XML文字列配列
	*
	* @return	分割前電文長
	*/

	int InformationSourceParser::createQueryResultStrList(const unsigned int mngId, TupleSet& ts, const int sepSize, vector<string> &ret)
	{
		string str;
		str.append("\r\n<result id=\"");
		str.append(to_string(mngId));
		str.append("\" ct=\"");
		str.append(to_string(DmUtil::getTimeMillisec()));
		str.append("\" size=\"");
		str.append(replaceWord);
		str.append("\" flagment=\"");
		str.append(replaceWordFlg);
		str.append("\" flagmentMax=\"");
		str.append(replaceWordFlgMax);
		str.append("\">\n");

		appendSchemaAndTupleStr(ts, str);

		str.append("</result>");

		int len = convertToXMLList(str, ret, sepSize);

		return len;
	}

	/**
	* クエリ結果のXML文字列を生成する
	* ※クエリ管理番号を返却するCS転送用
	*
	* @author	Nagoya University
	* @date	2018/10/09
	*
	* @param[in] 	senderId	自身のSID
	* @param[in] 	destinationId	転送先SID
	* @param[in] 	mngId	クエリ管理番号
	* @param[in] 	requestedMngId	リクエスト元クエリ管理番号
	* @param[in]	requestedDestId	要求元送信先SID
	* @param[in]	ts   	タプルセット
	* @param [in,out]	ret	クエリ結果XML文字列
	*
	* @return	電文長
	*/

	int InformationSourceParser::createQueryResult(const string &senderId, const string &destinationId, const unsigned int mngId, const unsigned int requestedMngId, const string &requestedDestId, TupleSet& ts, string &ret)
	{
		XMLCh tempStr1[BUF_MAX];
		XMLCh tempStr2[BUF_MAX];

		XMLString::transcode("Range", tempStr1, BUF_MAX - 1);
		DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr1);

		XMLString::transcode("result", tempStr1, BUF_MAX - 1);
		DOMDocument*   doc = impl->createDocument(0, tempStr1, 0);
		DOMElement*   result = doc->getDocumentElement();

		// 管理番号
		XMLString::transcode("id", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(mngId).c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// 要求元クエリ管理番号
		XMLString::transcode("reqId", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(requestedMngId).c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// 要求元送信先SID
		XMLString::transcode("reqDestId", tempStr1, BUF_MAX - 1);
		XMLString::transcode(requestedDestId.c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// 生成時間
		XMLString::transcode("ct", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(DmUtil::getTimeMillisec()).c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// サイズ情報
		XMLString::transcode("size", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWord.c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// sender情報
		DOMElement*   sender = createElement(doc, "sender");
		appendDomText(doc, sender, senderId);
		result->appendChild(sender);

		// destination情報
		DOMElement*   destination = createElement(doc, "destination");
		appendDomText(doc, destination, destinationId);
		result->appendChild(destination);

		// スキーマ情報
		DOMElement*   schema = createElement(doc, "schema");
		result->appendChild(schema);

		if (ts.size() > 0) {

			XMLCh chCol[BUF_MAX];
			XMLString::transcode("column", chCol, BUF_MAX - 1);
			XMLCh chName[BUF_MAX];
			XMLString::transcode("name", chName, BUF_MAX - 1);
			XMLCh chType[BUF_MAX];
			XMLString::transcode("type", chType, BUF_MAX - 1);

			// スキーマ情報
			for (Attribute attr : ts.getSchemaRef().getAttributes()) {
				DOMElement*   columnEl = doc->createElement(chCol);
				DOMElement*   nameEl = doc->createElement(chName);
				DOMElement*   typeEl = doc->createElement(chType);
				appendDomText(doc, nameEl, attr.name);
				appendDomText(doc, typeEl, attr.type);
				columnEl->appendChild(nameEl);
				columnEl->appendChild(typeEl);
				schema->appendChild(columnEl);
			}

			any val;
			long timestamp;
			bool isNull;
			XMLCh chTuple[BUF_MAX];
			XMLString::transcode("tuple", chTuple, BUF_MAX - 1);
			XMLCh chData[BUF_MAX];
			XMLString::transcode("data", chData, BUF_MAX - 1);
			XMLCh chTime[BUF_MAX];
			XMLString::transcode("time", chTime, BUF_MAX - 1);
			XMLCh chNull[BUF_MAX];
			XMLString::transcode("isNull", chNull, BUF_MAX - 1);

			// クエリ結果情報
			for (IS::Tuple tuple : ts.getTuples())
			{
				DOMElement*   tupleEl = doc->createElement(chTuple);
				for (int i = 0; i < tuple.size(); i++)
				{
					DOMElement*   dataEl = doc->createElement(chData);

					tuple.getValue(i, val, timestamp, isNull);
					appendDomText(doc, dataEl, stringUtil.getAnyString(val));

					if (timestamp != 0) {
						XMLString::transcode(std::to_string(timestamp).c_str(), tempStr2, BUF_MAX - 1);
						dataEl->setAttribute(chTime, tempStr2);
					}
					if (isNull == true) {
						XMLString::transcode(std::to_string(isNull).c_str(), tempStr2, BUF_MAX - 1);
						dataEl->setAttribute(chNull, tempStr2);
					}
					tupleEl->appendChild(dataEl);

				}
				result->appendChild(tupleEl);
			}
		}
		outputXML(doc, ret);
		replaceSizeInfo(ret);
		
		// done with the document, must call release() to release the entire document resources
		doc->release();
		return ret.size();
	}

	/**
	* クエリ結果のXML文字列を生成する(指定したサイズに分割して返却）
	* ※クエリ管理番号を返却するCS転送用
	*
	* @author	Nagoya University
	* @date	2018/05/23
	*
	* @param[in] 	senderId	自身のSID
	* @param[in] 	destinationId	転送先SID
	* @param[in] 	mngId	クエリ管理番号
	* @param[in] 	requestedMngId	リクエスト元クエリ管理番号
	* @param[in]	ts   	タプルセット
	* @param[in]	sepSize	分割サイズ
	* @param [in,out]	ret	クエリ結果XML文字列配列
	*
	* @return	分割前電文長
	*/

	int InformationSourceParser::createQueryResultList(const string &senderId, const string &destinationId, const unsigned int mngId, const unsigned int requestedMngId, TupleSet& ts, const int sepSize, vector<string> &ret)
	{
		XMLCh tempStr1[BUF_MAX];
		XMLCh tempStr2[BUF_MAX];

		XMLString::transcode("Range", tempStr1, BUF_MAX - 1);
		DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr1);

		XMLString::transcode("result", tempStr1, BUF_MAX - 1);
		DOMDocument*   doc = impl->createDocument(0, tempStr1, 0);
		DOMElement*   result = doc->getDocumentElement();

		// 管理番号
		XMLString::transcode("id", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(mngId).c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// 要求元クエリ管理番号
		XMLString::transcode("reqId", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(requestedMngId).c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// 生成時間
		XMLString::transcode("ct", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(DmUtil::getTimeMillisec()).c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// サイズ情報
		XMLString::transcode("size", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWord.c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// フラグメント情報
		XMLString::transcode("flagment", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWordFlg.c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// フラグメント情報(Max)
		XMLString::transcode("flagmentMax", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWordFlgMax.c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);


		// sender情報
		DOMElement*   sender = createElement(doc, "sender");
		appendDomText(doc, sender, senderId);
		result->appendChild(sender);

		// destination情報
		DOMElement*   destination = createElement(doc, "destination");
		appendDomText(doc, destination, destinationId);
		result->appendChild(destination);

		// スキーマ情報
		DOMElement*   schema = createElement(doc, "schema");
		result->appendChild(schema);

		if (ts.size() > 0) {

			XMLCh chCol[BUF_MAX];
			XMLString::transcode("column", chCol, BUF_MAX - 1);
			XMLCh chName[BUF_MAX];
			XMLString::transcode("name", chName, BUF_MAX - 1);
			XMLCh chType[BUF_MAX];
			XMLString::transcode("type", chType, BUF_MAX - 1);

			// スキーマ情報
			for (Attribute attr : ts.getSchemaRef().getAttributes()) {
				DOMElement*   columnEl = doc->createElement(chCol);
				DOMElement*   nameEl = doc->createElement(chName);
				DOMElement*   typeEl = doc->createElement(chType);
				appendDomText(doc, nameEl, attr.name);
				appendDomText(doc, typeEl, attr.type);
				columnEl->appendChild(nameEl);
				columnEl->appendChild(typeEl);
				schema->appendChild(columnEl);
			}

			any val;
			long timestamp;
			bool isNull;
			XMLCh chTuple[BUF_MAX];
			XMLString::transcode("tuple", chTuple, BUF_MAX - 1);
			XMLCh chData[BUF_MAX];
			XMLString::transcode("data", chData, BUF_MAX - 1);
			XMLCh chTime[BUF_MAX];
			XMLString::transcode("time", chTime, BUF_MAX - 1);
			XMLCh chNull[BUF_MAX];
			XMLString::transcode("isNull", chNull, BUF_MAX - 1);

			// クエリ結果情報
			for (IS::Tuple tuple : ts.getTuples())
			{
				DOMElement*   tupleEl = doc->createElement(chTuple);
				for (int i = 0; i < tuple.size(); i++)
				{
					DOMElement*   dataEl = doc->createElement(chData);

					tuple.getValue(i, val, timestamp, isNull);
					appendDomText(doc, dataEl, stringUtil.getAnyString(val));

					if (timestamp != 0) {
						XMLString::transcode(std::to_string(timestamp).c_str(), tempStr2, BUF_MAX - 1);
						dataEl->setAttribute(chTime, tempStr2);
					}
					if (isNull == true) {
						XMLString::transcode(std::to_string(isNull).c_str(), tempStr2, BUF_MAX - 1);
						dataEl->setAttribute(chNull, tempStr2);
					}
					tupleEl->appendChild(dataEl);

				}
				result->appendChild(tupleEl);
			}
		}
		string xmlResult = "";
		outputXML(doc, xmlResult);

		int len = convertToXMLList(xmlResult, ret, sepSize);

		// done with the document, must call release() to release the entire document resources
		doc->release();
		return len;
	}

	/**
	 * エラー結果のXML文字列を生成する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param[in]	mngId	クエリ管理番号
	 * @param[in]	code 	エラーコード
	 * @param[in]	msg 	エラーメッセージ
	 * @param[in,out]	ret	エラー結果XML文字列
	 */

	void InformationSourceParser::createErrorResult(const unsigned int mngId, const ErrorCode code, const string &msg, string &ret)
	{
		XMLCh tempStr1[BUF_MAX];
		XMLCh tempStr2[BUF_MAX];

		XMLString::transcode("Range", tempStr1, BUF_MAX - 1);
		DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr1);

		XMLString::transcode("result", tempStr1, BUF_MAX - 1);
		DOMDocument*   doc = impl->createDocument(0, tempStr1, 0);
		DOMElement*   result = doc->getDocumentElement();

		// 管理番号
		XMLString::transcode("id", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(mngId).c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// サイズ情報
		XMLString::transcode("size", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWord.c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// エラーコード
		DOMElement*   error = createElement(doc, "error");
		appendDomText(doc, error, std::to_string((int)code));
		result->appendChild(error);

		// エラーメッセージ
		DOMElement*   msgEl = createElement(doc, "msg");
		appendDomText(doc, msgEl, msg);
		result->appendChild(msgEl);

		outputXML(doc, ret);
		replaceSizeInfo(ret);
		
		// done with the document, must call release() to release the entire document resources
		doc->release();
		return;
	}

	/**
	* エラー結果のXML文字列を生成する
	*
	* @author	Nagoya University
	* @date	2018/10/09
	*
	* @param[in] 	senderId	自身のSID
	* @param[in] 	destinationId	転送先SID
	* @param[in]	mngId	クエリ管理番号
	* @param[in] 	requestedMngId	リクエスト元クエリ管理番号
	* @param[in]	requestedDestId	要求元送信先SID
	* @param[in]	code 	エラーコード
	* @param[in,out]	ret	エラー結果XML文字
	* @return 電文長
	*/

	int InformationSourceParser::createErrorResult(const string &senderId, const string &destinationId, const unsigned int mngId, const unsigned int requestedMngId, const string &requestedDestId, const ErrorCode code, string &ret)
	{
		XMLCh tempStr1[BUF_MAX];
		XMLCh tempStr2[BUF_MAX];

		XMLString::transcode("Range", tempStr1, BUF_MAX - 1);
		DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr1);

		XMLString::transcode("result", tempStr1, BUF_MAX - 1);
		DOMDocument*   doc = impl->createDocument(0, tempStr1, 0);
		DOMElement*   result = doc->getDocumentElement();

		// 管理番号
		XMLString::transcode("id", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(mngId).c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// 要求元クエリ管理番号
		XMLString::transcode("reqId", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(requestedMngId).c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// 要求元送信先SID
		XMLString::transcode("reqDestId", tempStr1, BUF_MAX - 1);
		XMLString::transcode(requestedDestId.c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// 生成時間
		XMLString::transcode("ct", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(DmUtil::getTimeMillisec()).c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// サイズ情報
		XMLString::transcode("size", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWord.c_str(), tempStr2, BUF_MAX - 1);
		result->setAttribute(tempStr1, tempStr2);

		// sender情報
		DOMElement*   sender = createElement(doc, "sender");
		appendDomText(doc, sender, senderId);
		result->appendChild(sender);

		// destination情報
		DOMElement*   destination = createElement(doc, "destination");
		appendDomText(doc, destination, destinationId);
		result->appendChild(destination);

		// エラーコード
		DOMElement*   error = createElement(doc, "error");
		appendDomText(doc, error, std::to_string((int)code));
		result->appendChild(error);

		outputXML(doc, ret);
		replaceSizeInfo(ret);

		// done with the document, must call release() to release the entire document resources
		doc->release();
		return ret.size();
	}

	/**
	 * ストリームデータのXML文字列を生成する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param[in]	senderId	 	送信者ID
	 * @param[in]	destinationId	送り先ID
	 * @param[in]	tupleset	 	タプルセット
	 * @param[in,out]	result	 	XML変換結果
	 *
	 */

	void InformationSourceParser::createStreamXML(const string &senderId, const string &destinationId, TupleSet &tupleset, string &result)
	{
		XMLCh tempStr1[BUF_MAX];
		XMLCh tempStr2[BUF_MAX];

		XMLString::transcode("Range", tempStr1, BUF_MAX - 1);
		DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr1);

		XMLString::transcode("stream", tempStr1, BUF_MAX - 1);
		DOMDocument*   doc = impl->createDocument(0, tempStr1, 0);
		DOMElement*   rootStream = doc->getDocumentElement();

		// サイズ情報
		XMLString::transcode("size", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWord.c_str(), tempStr2, BUF_MAX - 1);
		rootStream->setAttribute(tempStr1, tempStr2);

		// sender情報
		DOMElement*   sender = createElement(doc, "sender");
		appendDomText(doc, sender, senderId);
		rootStream->appendChild(sender);

		// destination情報
		DOMElement*   destination = createElement(doc, "destination");
		appendDomText(doc, destination, destinationId);
		rootStream->appendChild(destination);

		// tablename情報
		DOMElement*   tablename = createElement(doc, "tablename");
		appendDomText(doc, tablename, tupleset.getSchemaRef().getTableName());
		rootStream->appendChild(tablename);

		any val;
		long timestamp;

		XMLCh chTuple[BUF_MAX];
		XMLString::transcode("tuple", chTuple, BUF_MAX - 1);
		XMLCh chData[BUF_MAX];
		XMLString::transcode("data", chData, BUF_MAX - 1);
		XMLCh chTime[BUF_MAX];
		XMLString::transcode("time", chTime, BUF_MAX - 1);

		// tuple情報
		for (Tuple tuple : tupleset.getTuples())
		{
			DOMElement*   tupleEle = doc->createElement(chTuple);
			for (int i = 0; i<tuple.size(); i++)
			{
				tuple.getValue(i, val, timestamp);
				DOMElement*   dataEle = doc->createElement(chData);
				appendDomText(doc, dataEle, stringUtil.getAnyString(val));

				XMLString::transcode(std::to_string(timestamp).c_str(), tempStr2, BUF_MAX - 1);
				dataEle->setAttribute(chTime, tempStr2);
				tupleEle->appendChild(dataEle);
			}
			rootStream->appendChild(tupleEle);
		}

		outputXML(doc, result);
		replaceSizeInfo(result);

		// done with the document, must call release() to release the entire document resources
		doc->release();
		return;
	}


	/**
	* ストリームデータのXML文字列を生成する
	* ※大量のタプルをDOMAPIにてXML生成するとコストがかかるため文字列結合にて生成する
	*
	* @author	Nagoya University
	* @date	2020/02/14
	*
	* @param[in]	senderId	 	送信者ID
	* @param[in]	destinationId	送り先ID
	* @param[in,out]	tupleset	 	タプルセット
	* @param[in,out]	ret	 	XML変換結果
	*
	*/

	void InformationSourceParser::createStreamXMLStr(const string &senderId, const string &destinationId, TupleSet &tupleset, string &ret)
	{
		ret.append("\r\n<stream size=\"");
		ret.append(replaceWord);
		ret.append("\">\n");

		ret.append("<sender>");
		ret.append(senderId);
		ret.append("</sender>");
		ret.append("<destination>");
		ret.append(destinationId);
		ret.append("</destination>");
		ret.append("<tablename>");
		ret.append(tupleset.getSchemaRef().getTableName());
		ret.append("</tablename>");

		any val;
		long timestamp;
		vector<bool> isStringList;

		for (int i = 0; i < tupleset.getSchemaRef().getAttributeSize(); i++) {
			if (stringUtil.checkDataTypeMatches("string", tupleset.getSchemaRef().getAttributeType(i), false)) {
				isStringList.push_back(true);
			}
			else {
				isStringList.push_back(false);
			}
		}

		for (int idx=0; idx < tupleset.size(); idx++)
		{
			ret.append("<tuple>");
			for (int i = 0; i<tupleset.getTuple(idx).size(); i++)
			{
				tupleset.getTuple(idx).getValue(i, val, timestamp);
				ret.append("<data time=\"");
				ret.append(to_string(timestamp));
				ret.append("\">");
				if (isStringList.at(i)) {
					ret.append("<![CDATA[");
					ret.append(stringUtil.getAnyString(val));
					ret.append("]]>");
				}
				else {
					ret.append(stringUtil.getAnyString(val));
				}
				ret.append("</data>");
			}
			ret.append("</tuple>");
		}
		ret.append("</stream>");
		replaceSizeInfo(ret);
		return;
	}

	/**
	* ストリームデータのXML文字列を生成する(配列)(CSの分割機能実装のため未使用)
	*
	* @author	Nagoya University
	* @date	2018/05/24
	*
	* @param[in]	senderId	 	送信者ID
	* @param[in]	destinationId	送り先ID
	* @param[in]	tupleset	 	タプルセット
	* @param[in]	sepSize	 		分割サイズ
	* @param[in,out]	ret			結果		
	*
	* @return	分割前文字列長
	*/

	int InformationSourceParser::createStreamXMLList(const string &senderId, const string &destinationId, TupleSet &tupleset, const int sepSize, vector<string> &ret)
	{
		XMLCh tempStr1[BUF_MAX];
		XMLCh tempStr2[BUF_MAX];
		string result;
		XMLString::transcode("Range", tempStr1, BUF_MAX - 1);
		DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr1);

		XMLString::transcode("stream", tempStr1, BUF_MAX - 1);
		DOMDocument*   doc = impl->createDocument(0, tempStr1, 0);
		DOMElement*   rootStream = doc->getDocumentElement();

		// 生成時間
		XMLString::transcode("ct", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(DmUtil::getTimeMicrosec()).c_str(), tempStr2, BUF_MAX - 1);
		rootStream->setAttribute(tempStr1, tempStr2);

		// サイズ情報
		XMLString::transcode("size", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWord.c_str(), tempStr2, BUF_MAX - 1);
		rootStream->setAttribute(tempStr1, tempStr2);

		// フラグメント情報
		XMLString::transcode("flagment", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWordFlg.c_str(), tempStr2, BUF_MAX - 1);
		rootStream->setAttribute(tempStr1, tempStr2);

		// フラグメント情報(Max)
		XMLString::transcode("flagmentMax", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWordFlgMax.c_str(), tempStr2, BUF_MAX - 1);
		rootStream->setAttribute(tempStr1, tempStr2);


		// sender情報
		DOMElement*   sender = createElement(doc, "sender");
		appendDomText(doc, sender, senderId);
		rootStream->appendChild(sender);

		// destination情報
		DOMElement*   destination = createElement(doc, "destination");
		appendDomText(doc, destination, destinationId);
		rootStream->appendChild(destination);

		// tablename情報
		DOMElement*   tablename = createElement(doc, "tablename");
		appendDomText(doc, tablename, tupleset.getSchemaRef().getTableName());
		rootStream->appendChild(tablename);

		any val;
		long timestamp;

		XMLCh chTuple[BUF_MAX];
		XMLString::transcode("tuple", chTuple, BUF_MAX - 1);
		XMLCh chData[BUF_MAX];
		XMLString::transcode("data", chData, BUF_MAX - 1);
		XMLCh chTime[BUF_MAX];
		XMLString::transcode("time", chTime, BUF_MAX - 1);

		// tuple情報
		for (Tuple tuple : tupleset.getTuples())
		{
			DOMElement*   tupleEle = doc->createElement(chTuple);
			for (int i = 0; i<tuple.size(); i++)
			{
				tuple.getValue(i, val, timestamp);
				DOMElement*   dataEle = doc->createElement(chData);
				appendDomText(doc, dataEle, stringUtil.getAnyString(val));

				XMLString::transcode(std::to_string(timestamp).c_str(), tempStr2, BUF_MAX - 1);
				dataEle->setAttribute(chTime, tempStr2);
				tupleEle->appendChild(dataEle);
			}
			rootStream->appendChild(tupleEle);
		}

		outputXML(doc, result);

		int len = convertToXMLList(result, ret, sepSize);

		// done with the document, must call release() to release the entire document resources
		doc->release();
		return len;
	}

	/**
	* 継続クエリのキャンセル時のXML文字列を生成する
	*
	* @author	Nagoya University
	* @date	2018/10/09
	*
	* @param		mngId			クエリ管理番号
	* @param[in]	receptionSID	クエリを受け付けたノードのSID
	* @param[in]	senderSID		依頼者SID
	* @param [in,out]	ret			XML文字列
	*
	* @return	電文長
	*/

	//int InformationSourceParser::createCancelXML(unsigned int mngId, const int receptionSID, const int senderSID, string &ret)
	int InformationSourceParser::createCancelXML(unsigned int mngId, const unsigned long long receptionSID, const unsigned long long senderSID, string &ret)
	{
		XMLCh tempStr1[BUF_MAX];
		XMLCh tempStr2[BUF_MAX];

		XMLString::transcode("Range", tempStr1, BUF_MAX - 1);
		DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr1);

		XMLString::transcode("cancel", tempStr1, BUF_MAX - 1);
		DOMDocument*   doc = impl->createDocument(0, tempStr1, 0);
		DOMElement*   resultEl = doc->getDocumentElement();

		// 生成時間
		XMLString::transcode("ct", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(DmUtil::getTimeMicrosec()).c_str(), tempStr2, BUF_MAX - 1);
		resultEl->setAttribute(tempStr1, tempStr2);

		// サイズ情報
		XMLString::transcode("size", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWord.c_str(), tempStr2, BUF_MAX - 1);
		resultEl->setAttribute(tempStr1, tempStr2);

		// 管理番号
		XMLString::transcode("id", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(mngId).c_str(), tempStr2, BUF_MAX - 1);
		resultEl->setAttribute(tempStr1, tempStr2);

		// 受付SID
		XMLString::transcode("receptionSID", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(receptionSID).c_str(), tempStr2, BUF_MAX - 1);
		resultEl->setAttribute(tempStr1, tempStr2);

		// 依頼者SID
		XMLString::transcode("senderSID", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(senderSID).c_str(), tempStr2, BUF_MAX - 1);
		resultEl->setAttribute(tempStr1, tempStr2);

		outputXML(doc, ret);
		replaceSizeInfo(ret);

		// done with the document, must call release() to release the entire document resources
		doc->release();
		return ret.length();
	}

	/**
	* OperatorTreeのXML文字列を生成する
	*
	* @author	Nagoya University
	* @date	2018/10/09
	*
	* @param[in]	senderId	 		送信元SID
	* @param[in]	destinationId	 	送信先SID
	* @param[in]	operatorTreeXML	 	オペレータツリーXML文字列
	* @param[in]	mngId				クエリ管理番号
	* @param[in]	requestVehicleId	要求した車両SID
	* @param[in]	range				データ取得要求範囲
	* @param[in]	receptionMngId		受け付けたクエリ管理番号
	* @param[in]	receptionEdgeID		エッジが受け付けたクエリ管理番号
	* @param[in]	edgeSIDListStr		連携先エッジSID文字列(カンマ区切り)
	* @param[in,out]	ret				結果
	*
	* @return	電文長
	*/

	//int InformationSourceParser::createOperatorTreeXML(const string &senderId, const string &destinationId, string operatorTreeXML, const unsigned int mngId, const int requestVehicleId, const int range, const unsigned int receptionMngId, const int receptionEdgeID, const string &edgeSIDListStr, string &ret)
	int InformationSourceParser::createOperatorTreeXML(const string &senderId, const string &destinationId, string operatorTreeXML, const unsigned int mngId, const unsigned long long requestVehicleId, const int range, const unsigned int receptionMngId, const unsigned long long receptionEdgeID, const string &edgeSIDListStr, string &ret)
	{
		XMLCh tempStr1[BUF_MAX];
		XMLCh tempStr2[BUF_MAX];

		XMLString::transcode("Range", tempStr1, BUF_MAX - 1);
		DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr1);

		XMLString::transcode("operatorTree", tempStr1, BUF_MAX - 1);
		DOMDocument*   doc = impl->createDocument(0, tempStr1, 0);
		DOMElement*   rootStream = doc->getDocumentElement();

		// クエリ管理番号
		XMLString::transcode("id", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(mngId).c_str(), tempStr2, BUF_MAX - 1);
		rootStream->setAttribute(tempStr1, tempStr2);

		// 生成時間
		XMLString::transcode("ct", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(DmUtil::getTimeMicrosec()).c_str(), tempStr2, BUF_MAX - 1);
		rootStream->setAttribute(tempStr1, tempStr2);

		// サイズ情報
		XMLString::transcode("size", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWord.c_str(), tempStr2, BUF_MAX - 1);
		rootStream->setAttribute(tempStr1, tempStr2);

		// sender情報
		DOMElement*   sender = createElement(doc, "sender");
		appendDomText(doc, sender, senderId);
		rootStream->appendChild(sender);

		// destination情報
		DOMElement*   destination = createElement(doc, "destination");
		appendDomText(doc, destination, destinationId);
		rootStream->appendChild(destination);

		// 取得要求範囲情報
		DOMElement*   rangeEle = createElement(doc, "range");
		appendDomText(doc, rangeEle, to_string(range));
		rootStream->appendChild(rangeEle);

		// 要求車両ID
		DOMElement*   reqVIdEle = createElement(doc, "requestVehicleId");
		appendDomText(doc, reqVIdEle, to_string(requestVehicleId));
		rootStream->appendChild(reqVIdEle);

		// 受付クエリ管理番号
		DOMElement*   receptionEle = createElement(doc, "receptionMngId");
		appendDomText(doc, receptionEle, to_string(receptionMngId));
		rootStream->appendChild(receptionEle);

		// 受付エッジSID番号
		DOMElement*   receptionEdgeEle = createElement(doc, "receptionEdgeId");
		appendDomText(doc, receptionEdgeEle, to_string(receptionEdgeID));
		rootStream->appendChild(receptionEdgeEle);

		// 連携先エッジSID文字列(カンマ区切り)
		DOMElement*   edgeIDsEle = createElement(doc, "edgeIDs");
		appendDomText(doc, edgeIDsEle, edgeSIDListStr);
		rootStream->appendChild(edgeIDsEle);

		// body情報
		DOMElement*   tablename = createElement(doc, "body");
		appendDomText(doc, tablename, operatorTreeXML);
		rootStream->appendChild(tablename);

		outputXML(doc, ret);
		replaceSizeInfo(ret);

		// done with the document, must call release() to release the entire document resources
		doc->release();
		return ret.length();
	}

	/**
	* OperatorTreeのXML文字列を生成する(配列)(CSの分割機能が実装されたので未使用)
	*
	* @author	Nagoya University
	* @date	2018/08/23
	*
	* @param	senderId	 	自身のSID
	* @param	destinationId	 	転送先SID
	* @param	operatorTreeXML	 	オペレータツリーXML文字列
	* @param	mngId				クエリ管理番号
	* @param	sepSize				分割サイズ
	* @param[in,out]	ret				 	結果
	*
	* @return	分割前電文長
	*/

	int InformationSourceParser::createOperatorTreeXMLList(const string &senderId, const string &destinationId, string operatorTreeXML, const unsigned int mngId, const int sepSize, vector<string> &ret)
	{
		XMLCh tempStr1[BUF_MAX];
		XMLCh tempStr2[BUF_MAX];
		string result;
		XMLString::transcode("Range", tempStr1, BUF_MAX - 1);
		DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr1);

		XMLString::transcode("operatorTree", tempStr1, BUF_MAX - 1);
		DOMDocument*   doc = impl->createDocument(0, tempStr1, 0);
		DOMElement*   rootStream = doc->getDocumentElement();

		// クエリ管理番号
		XMLString::transcode("id", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(mngId).c_str(), tempStr2, BUF_MAX - 1);
		rootStream->setAttribute(tempStr1, tempStr2);

		// 生成時間
		XMLString::transcode("ct", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(DmUtil::getTimeMicrosec()).c_str(), tempStr2, BUF_MAX - 1);
		rootStream->setAttribute(tempStr1, tempStr2);

		// サイズ情報
		XMLString::transcode("size", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWord.c_str(), tempStr2, BUF_MAX - 1);
		rootStream->setAttribute(tempStr1, tempStr2);

		// フラグメント情報
		XMLString::transcode("flagment", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWordFlg.c_str(), tempStr2, BUF_MAX - 1);
		rootStream->setAttribute(tempStr1, tempStr2);

		// フラグメント情報(Max)
		XMLString::transcode("flagmentMax", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWordFlgMax.c_str(), tempStr2, BUF_MAX - 1);
		rootStream->setAttribute(tempStr1, tempStr2);

		// sender情報
		DOMElement*   sender = createElement(doc, "sender");
		appendDomText(doc, sender, senderId);
		rootStream->appendChild(sender);

		// destination情報
		DOMElement*   destination = createElement(doc, "destination");
		appendDomText(doc, destination, destinationId);
		rootStream->appendChild(destination);

		// body情報
		DOMElement*   tablename = createElement(doc, "body");
		appendDomText(doc, tablename, operatorTreeXML);
		rootStream->appendChild(tablename);

		outputXML(doc, result);

		int len = convertToXMLList(result, ret, sepSize);

		// done with the document, must call release() to release the entire document resources
		doc->release();
		return len;
	}


	/**
	* 単一データをXMLに変換する
	*
	* @author	Nagoya University
	* @date	2018/10/09
	*
	* @param	rootTag	タグ名
	* @param	value	データ
	* @param [in,out]	ret XML文字列
	*
	*/

	void InformationSourceParser::createGeneralSingleResponseXML(const string &rootTag, const string &value, string &ret)
	{
		XMLCh tempStr1[BUF_MAX];
		XMLCh tempStr2[BUF_MAX];

		XMLString::transcode("Range", tempStr1, BUF_MAX - 1);
		DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr1);

		XMLString::transcode(rootTag.c_str(), tempStr1, BUF_MAX - 1);
		DOMDocument*   doc = impl->createDocument(0, tempStr1, 0);
		DOMElement*   resultEl = doc->getDocumentElement();

		// 生成時間
		XMLString::transcode("ct", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(DmUtil::getTimeMicrosec()).c_str(), tempStr2, BUF_MAX - 1);
		resultEl->setAttribute(tempStr1, tempStr2);

		// サイズ情報
		XMLString::transcode("size", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWord.c_str(), tempStr2, BUF_MAX - 1);
		resultEl->setAttribute(tempStr1, tempStr2);

		// 値
		XMLString::transcode("value", tempStr1, BUF_MAX - 1);
		XMLString::transcode(value.c_str(), tempStr2, BUF_MAX - 1);
		resultEl->setAttribute(tempStr1, tempStr2);

		outputXML(doc, ret);
		replaceSizeInfo(ret);

		// done with the document, must call release() to release the entire document resources
		doc->release();
	}

	/**
	* 複数データをXMLに変換する
	*
	* @author	Nagoya University
	* @date	2019/11/13
	*
	* @param	valList	2次元配列データ
	* @param [in,out]	ret XML文字列
	*
	*/

	void InformationSourceParser::createGeneralDataListResponseXML(const vector<vector<string>> &valList, string &ret)
	{
		XMLCh tempStr1[BUF_MAX];
		XMLCh tempStr2[BUF_MAX];

		XMLString::transcode("Range", tempStr1, BUF_MAX - 1);
		DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr1);

		XMLString::transcode("result", tempStr1, BUF_MAX - 1);
		DOMDocument*   doc = impl->createDocument(0, tempStr1, 0);
		DOMElement*   resultEl = doc->getDocumentElement();

		// 生成時間
		XMLString::transcode("ct", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(DmUtil::getTimeMicrosec()).c_str(), tempStr2, BUF_MAX - 1);
		resultEl->setAttribute(tempStr1, tempStr2);

		// サイズ情報
		XMLString::transcode("size", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWord.c_str(), tempStr2, BUF_MAX - 1);
		resultEl->setAttribute(tempStr1, tempStr2);

		XMLCh chName[BUF_MAX];
		XMLString::transcode("name", chName, BUF_MAX - 1);
		XMLCh chTuple[BUF_MAX];
		XMLString::transcode("tuple", chTuple, BUF_MAX - 1);
		XMLCh chData[BUF_MAX];
		XMLString::transcode("data", chData, BUF_MAX - 1);

		// name
		int index = 0;
		for (vector<string> row : valList)
		{
			for (string data : row) {
				DOMElement*   nameEl = doc->createElement(chName);
				appendDomText(doc, nameEl, to_string(index++));
				resultEl->appendChild(nameEl);
			}
			break;
		}

		// data
		for (vector<string> row : valList)
		{
			DOMElement*   tupleEle = doc->createElement(chTuple);
			for (unsigned int i = 0; i<row.size(); i++)
			{
				DOMElement*   dataEle = doc->createElement(chData);
				appendDomText(doc, dataEle, row[i]);
				tupleEle->appendChild(dataEle);
			}
			resultEl->appendChild(tupleEle);
		}

		outputXML(doc, ret);
		replaceSizeInfo(ret);
		
		// done with the document, must call release() to release the entire document resources
		doc->release();
	}

	/**
	 * 指定したタグ名のエレメントを生成する(繰り返し文内での使用は一時変数を定義するため非推奨)
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in,out]	doc	DOMDocumentオブジェクト
	 * @param 		  	tag	タグ名
	 *
	 * @return	エレメント
	 */

	inline DOMElement* InformationSourceParser::createElement(DOMDocument* doc, const string &tag)
	{
		XMLCh tempStr[BUF_MAX];
		XMLString::transcode(tag.c_str(), tempStr, BUF_MAX - 1);
		return doc->createElement(tempStr);
	}

	/**
	 * エレメント内にvalueを設定する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in,out]	doc  	DOMDocumentオブジェクト
	 * @param [in,out]	ele  	対象のエレメント
	 * @param 		  	value	設定する文字列
	 */

	inline void InformationSourceParser::appendDomText(DOMDocument* doc, DOMElement* ele, const string &value)
	{
		XMLCh tempStr[value.size() + 1];
		XMLString::transcode(value.c_str(), tempStr, value.size());
		DOMText* textNode = doc->createTextNode(tempStr);
		ele->appendChild(textNode);
	}

	/**
	 * エレメントに設定された文字列を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in,out]	ele	エレメント
	 *
	 * @return	文字列
	 */

	string InformationSourceParser::getChildText(const DOMNode* ele)
	{
		string ret = "";
		try {
			DOMNode* child = ele->getFirstChild();
			if (child == NULL) {
				// エレメントが存在しない場合
				return ret;
			}
			DOMText* text = static_cast<DOMText*>(child);
			if (text == NULL) {
				// テキストが存在しない場合
				return ret;
			}
			XMLCh* buffer = new XMLCh[XMLString::stringLen(text->getData()) + 1];
			XMLString::copyString(buffer, text->getData());
			XMLString::trim(buffer);
			char* content = XMLString::transcode(buffer);
			delete[] buffer;
			ret = content;
			XMLString::release(&content);
		}
		catch (const exception &e) {
			string what(e.what());
			logger->error("Not existed child element. what:" + what);
		}
		return ret;
	}

	/**
	* エレメントに設定された文字列を取得する
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param [in]	ele	エレメント
	* @param [in,out]	result	文字列
	*
	*/

	void InformationSourceParser::getChildText(const DOMNode* ele, string &result)
	{
		try {
			DOMNode* child = ele->getFirstChild();
			if (child == NULL) {
				// エレメントが存在しない場合
				return;
			}
			DOMText* text = static_cast<DOMText*>(child);
			if (text == NULL) {
				// テキストが存在しない場合
				return;
			}
			XMLCh* buffer = new XMLCh[XMLString::stringLen(text->getData()) + 1];
			XMLString::copyString(buffer, text->getData());
			XMLString::trim(buffer);
			char* content = XMLString::transcode(buffer);
			delete[] buffer;
			result = content;
			XMLString::release(&content);
		}
		catch (const exception &e) {
			string what(e.what());
			logger->error("Not existed child element. what:" + what);
		}
		return;
	}
	/**
	 * 指定タグ名のデータを複数取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in,out]	elem   	エレメント
	 * @param [in,out]	tagName	タグ名
	 *
	 * @return	指定タグ名のノードリスト
	 */

	DOMNodeList* InformationSourceParser::getElementByCharTagName(DOMElement* elem, char *tagName)
	{
		XMLCh* attributeName = XMLString::transcode(tagName);
		DOMNodeList* nlist = elem->getElementsByTagName(attributeName);
		XMLString::release(&attributeName);

		return nlist;
	}

	/**
	 * 指定エレメントのタグ名を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in,out]	element	指定エレメント
	 * @param [in,out]	result	タグ名文字列
	 */

	inline void InformationSourceParser::getTagName(DOMElement* element, string &result) {
		char* name = XMLString::transcode(element->getTagName());
		result = name;
		XMLString::release(&name);
		return;
	}

	/**
	 * タグ内のattributeIDを指定し、値を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in,out]	element	指定エレメント
	 * @param 		  	id	   	AttributeID
	 * @param	ret	値
	 */

	void InformationSourceParser::getTagValue(const DOMElement* element, const string &id, string &ret) {

		DOMNamedNodeMap* map = element->getAttributes();
		for (XMLSize_t i = 0; i < map->getLength(); i++) {
			DOMAttr* attr = static_cast<DOMAttr*>(map->item(i));
			char* attr_name = XMLString::transcode(attr->getName());
			string atrName = attr_name;
			if (id == atrName) {
				char* attr_value = XMLString::transcode(attr->getValue());
				ret = attr_value;
				XMLString::release(&attr_name);
				XMLString::release(&attr_value);
				break;
			}
			XMLString::release(&attr_name);
		}
		return;
	}

	/**
	 * ファイルパス指定でXMLファイルをパースする
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	filePath	ファイルパス
	 *
	 * @return	DOMDocumentオブジェクト
	 */

	DOMDocument* InformationSourceParser::parseXmlFile(const string &filePath)
	{
		DOMDocument* ret;
		XercesDOMParser* parser = new XercesDOMParser();
		parser->setValidationScheme(XercesDOMParser::Val_Always);
		parser->setDoNamespaces(true); // optional

		ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
		parser->setErrorHandler(errHandler);

		const char* xmlFile = filePath.c_str();

		try {
			parser->parse(xmlFile);
			ret = parser->getDocument();
			//write(ret);
			// TODO : ここではgetDocumentElement()配下の要素に対してアクセス出来るが戻り値では参照できない
			// たぶんDOMDocumentがポインタなので、スコープの関係上、戻り値では不完全なデータになる？？
		}
		catch (const XMLException& toCatch) {
			char* message = XMLString::transcode(toCatch.getMessage());
			string messageStr(message);
			logger->error("[parseXmlFile]XMLException message is: \n" + messageStr);
			XMLString::release(&message);
			return NULL;
		}
		catch (const DOMException& toCatch) {
			char* message = XMLString::transcode(toCatch.msg);
			string messageStr(message);
			logger->error("[parseXmlFile]DOMException message is: \n"  + messageStr);
			XMLString::release(&message);
			return NULL;
		}
		catch (const exception &e) {
			string what(e.what());
			logger->error("[parseXmlFile]Unexpected Exception message is: \n" + what);
			return NULL;
		}

		delete parser;
		delete errHandler;
		return ret;
	}

	/**
	 * 指定ファイルパスのXMLファイルをパースし、XML文字列で取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	filePath	ファイルパス
	 * @param	result	XML文字列
	 */

	void InformationSourceParser::parseXmlFileToString(const string &filePath, string &result)
	{
		DOMDocument* doc;
		XercesDOMParser* parser = new XercesDOMParser();
		parser->setValidationScheme(XercesDOMParser::Val_Always);
		parser->setDoNamespaces(true); // optional

		ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
		parser->setErrorHandler(errHandler);

		const char* xmlFile = filePath.c_str();

		try {
			parser->parse(xmlFile);
			doc = parser->getDocument();
			outputXML(doc, result);

			//write(ret);
			// TODO : ここではgetDocumentElement()配下の要素に対してアクセス出来るが戻り値では参照できない
			// たぶんDOMDocumentがポインタなので、スコープの関係上、戻り値では不完全なデータになる？？
		}
		catch (const XMLException& toCatch) {
			char* message = XMLString::transcode(toCatch.getMessage());
			string messageStr(message);
			logger->error("[parseXmlFileToString]XMLException message is: \n" + messageStr);
			XMLString::release(&message);
			return;
		}
		catch (const DOMException& toCatch) {
			char* message = XMLString::transcode(toCatch.msg);
			string messageStr(message);
			logger->error("[parseXmlFileToString]DOMException message is: \n" + messageStr);
			XMLString::release(&message);
			return;
		}
		catch (const SAXParseException& toCatch) {
			char* message = XMLString::transcode(toCatch.getMessage());
			string messageStr(message);
			logger->error("[parseXmlFileToString]SAXParseException message is: \n" + messageStr);
			XMLString::release(&message);
			return;
		}
		catch (const exception &e) {
			string what(e.what());
			logger->error("[parseXmlFileToString]Unexpected Exception message is: \n" + what);
			return;
		}

		delete parser;
		delete errHandler;
		return;
	}

	/**
	 * スキーマ情報をXML変換する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in,out]	schema	スキーマ情報
	 *
	 * @return	DOMDocumentオブジェクト
	 */

	DOMDocument* InformationSourceParser::parseSchema(const Schema &schema)
	{
		DOMDocument* ret;
		XMLCh tempStr1[BUF_MAX];
		XMLString::transcode("Range", tempStr1, BUF_MAX - 1);
		DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr1);

		XMLString::transcode("schema", tempStr1, BUF_MAX - 1);
		DOMDocument*   doc = impl->createDocument(0, tempStr1, 0);
		DOMElement*   rootSchema = doc->getDocumentElement();

		// tablename情報
		DOMElement*   tablename = createElement(doc, "tablename");
		appendDomText(doc, tablename, schema.getTableName());
		rootSchema->appendChild(tablename);

		// column情報

		for (Attribute attr : schema.getAttributes())
		{
			DOMElement*   columnEle = createElement(doc, "column");
			DOMElement*   nameEle = createElement(doc, "name");
			appendDomText(doc, nameEle, attr.name);

			DOMElement*   typeEle = createElement(doc, "type");
			appendDomText(doc, typeEle, attr.type);
			columnEle->appendChild(nameEle);
			columnEle->appendChild(typeEle);
			rootSchema->appendChild(columnEle);
		}

		ret = doc;
		// done with the document, must call release() to release the entire document resources
		doc->release();
		return ret;
	}

	/**
	 * 指定ファイルパスにXMLファイルを書き出す(未使用)
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in,out]	pmyDOMDocument	DOMDocumentオブジェクト
	 * @param 		  	filePath	  	指定ファイルパス
	 */

	void InformationSourceParser::writeXML(xercesc::DOMDocument* pmyDOMDocument, const char *filePath)
	{
		//LS設定
		XMLCh tempStr[BUF_MAX];
		XMLString::transcode("LS", tempStr, BUF_MAX - 1);
		DOMImplementation *implementation = DOMImplementationRegistry::getDOMImplementation(tempStr);
		DOMLSSerializer *serializer = ((DOMImplementationLS*)implementation)->createLSSerializer();
		if (serializer->getDomConfig()->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true)) {
			serializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
		}

		XMLCh* line = XMLString::transcode("\r\n");
		serializer->setNewLine(line);

		//ファイル名称をXMLChに変更.
		XMLCh *tempFilePath = XMLString::transcode(filePath);
		//ファイルの作成.
		XMLFormatTarget *formatTarget = new LocalFileFormatTarget(tempFilePath);

		DOMLSOutput *output = ((DOMImplementationLS*)implementation)->createLSOutput();
		output->setByteStream(formatTarget);

		//書き出し
		serializer->write(pmyDOMDocument, output);

		//解放
		serializer->release();
		XMLString::release(&line);
		XMLString::release(&tempFilePath);
		delete formatTarget;
		output->release();
	}

	/**
	 * DomDocumentオブジェクトからXML文字列を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in]	pmyDOMDocument	DOMDocumentオブジェクト
	 * @param [in,out]	result	XML文字列
	 */

	void InformationSourceParser::outputXML(const xercesc::DOMDocument* pmyDOMDocument, string &result)
	{
		//LS設定
		XMLCh tempStr[BUF_MAX];
		XMLString::transcode("LS", tempStr, BUF_MAX - 1);
		DOMImplementation *implementation = DOMImplementationRegistry::getDOMImplementation(tempStr);
		DOMLSSerializer *serializer = ((DOMImplementationLS*)implementation)->createLSSerializer();
		if (serializer->getDomConfig()->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true)) {
			serializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
		}
		XMLString::transcode("\r\n", tempStr, BUF_MAX - 1);
		serializer->setNewLine(tempStr);

		//書き出し
		XMLCh* toTranscode = serializer->writeToString(pmyDOMDocument->getDocumentElement());
		char* xml = XMLString::transcode(toTranscode);
		//cout << xml << endl;
		result = std::string(xml);
		XMLString::release(&xml);
		XMLString::release(&toTranscode);

		//解放
		serializer->release();

		return;
	}

	/**
	 * 再帰的にエレメントを探索し、出力する(未使用)
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in,out]	node	DOMNodeオブジェクト
	 */

	void InformationSourceParser::write(DOMNode* node) {

		if (node) {
			switch (node->getNodeType()) {
			case DOMNode::ELEMENT_NODE:
				writeElement(static_cast<DOMElement*>(node));
				break;
			case DOMNode::TEXT_NODE:
				writeText(static_cast<DOMText*>(node));
				break;
			default:
				// not used
				break;
			}

			DOMNode* child = node->getFirstChild();
			while (child) {
				DOMNode* next = child->getNextSibling();
				write(child);
				child = next;
			}
		}
	}

	/**
	 * エレメント情報を出力する(未使用)
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in,out]	element	DOMElementオブジェクト
	 */

	void InformationSourceParser::writeElement(DOMElement* element) {
		char* name = XMLString::transcode(element->getTagName());
		cout << "tag    : " << name << endl;
		XMLString::release(&name);

		DOMNamedNodeMap* map = element->getAttributes();
		for (XMLSize_t i = 0; i < map->getLength(); i++) {
			DOMAttr* attr = static_cast<DOMAttr*>(map->item(i));
			char* attr_name = XMLString::transcode(attr->getName());
			char* attr_value = XMLString::transcode(attr->getValue());
			cout << attr_name << ": " << attr_value << endl;

			XMLString::release(&attr_name);
			XMLString::release(&attr_value);
		}
	}

	/**
	 * テキスト情報を出力する(未使用)
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in,out]	text	DOMTextオブジェクト
	 */

	void InformationSourceParser::writeText(DOMText* text) {
		XMLCh* buffer = new XMLCh[XMLString::stringLen(text->getData()) + 1];
		XMLString::copyString(buffer, text->getData());
		XMLString::trim(buffer);
		char* content = XMLString::transcode(buffer);
		delete[] buffer;

		cout << "content :" << content << endl;
		XMLString::release(&content);
	}

	/**
	 * XML文字列からクエリ文字列を抽出する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param[in]	queryXml	XML文字列
	 * @param[in,out]	result	クエリ文字列
	 *
	 */

	void InformationSourceParser::getQueryString(const string &queryXml, string &result)
	{
		const int buf_size = queryXml.length();
		XMLCh tempStr[buf_size + 1];
		static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
		DOMImplementationLS *impl = (DOMImplementationLS*)DOMImplementationRegistry::getDOMImplementation(gLS);
		DOMLSParser       *domBuilder = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
		DOMLSInput        *input = impl->createLSInput();

		XMLString::transcode(queryXml.c_str(), tempStr, buf_size);
		input->setStringData(tempStr);

		try
		{
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMEntities, false);
			DOMDocument* doc = domBuilder->parse(input);

			// verify that we get only 3 calls: for the text node, the CDATA section and the root element
			if (doc == NULL || doc->getDocumentElement() == NULL)
			{
				logger->error("[getQueryString] document is Null Line:" + std::to_string(__LINE__));
				domBuilder->release();
				input->release();
				return;
			}

			DOMElement* root = doc->getDocumentElement();
			XMLString::transcode("body", tempStr, BUF_MAX - 1);
			if (root->getElementsByTagName(tempStr) == NULL)
			{
				logger->error("[getQueryString] <body></body> is not defined");
				domBuilder->release();
				input->release();
				return;
			}
			DOMNode* bodyNode = root->getElementsByTagName(tempStr)->item(0);

			DOMNode* child = bodyNode->getFirstChild();
			DOMText* text = static_cast<DOMText*>(child);
			XMLCh* buffer = new XMLCh[XMLString::stringLen(text->getData()) + 1];
			XMLString::copyString(buffer, text->getData());
			XMLString::trim(buffer);
			char* content = XMLString::transcode(buffer);
			delete[] buffer;

			result = content;
			XMLString::release(&content);

		}
		catch (DOMException&)
		{
			logger->error("[getQueryString] DOMException Line:" + std::to_string(__LINE__));
		}
		domBuilder->release();
		input->release();
		return;
	}


	/**
	* XML文字列からOperatorTreeXMLを抽出する
	*
	* @author	Nagoya University
	* @date	2018/08/24
	*
	* @param	queryXml	XML文字列
	* @param	operatorTreeXML	オペレーターツリーXML文字列
	*
	*/

	void InformationSourceParser::getOperatorTreeXML(const string &queryXml, string &operatorTreeXML)
	{
		const int buf_size = queryXml.length();
		XMLCh tempStr[buf_size + 1];
		static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
		DOMImplementationLS *impl = (DOMImplementationLS*)DOMImplementationRegistry::getDOMImplementation(gLS);
		DOMLSParser       *domBuilder = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
		DOMLSInput        *input = impl->createLSInput();

		XMLString::transcode(queryXml.c_str(), tempStr, buf_size);
		input->setStringData(tempStr);

		try
		{
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMEntities, false);
			DOMDocument* doc = domBuilder->parse(input);

			// verify that we get only 3 calls: for the text node, the CDATA section and the root element
			if (doc == NULL || doc->getDocumentElement() == NULL)
			{
				logger->error("[getOperatorTreeXML] document is Null Line:" + std::to_string(__LINE__));
				return;
			}

			DOMElement* root = doc->getDocumentElement();
			XMLString::transcode("body", tempStr, BUF_MAX - 1);
			if (root->getElementsByTagName(tempStr) == NULL)
			{
				logger->error("[getOperatorTreeXML] <body></body> is not defined");
				return;
			}
			DOMNode* bodyNode = root->getElementsByTagName(tempStr)->item(0);
			getChildText(bodyNode, operatorTreeXML);

		}
		catch (DOMException&)
		{
			logger->error("[getQueryString] DOMException Line:" + std::to_string(__LINE__));
		}
		domBuilder->release();
		input->release();
		return;
	}

	/**
	 * XML文字列から受付ポート番号を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	queryXml	XML文字列
	 *
	 * @return	受付ポート番号
	 */

	string InformationSourceParser::getStringByTagName(const string &queryXml, const string &tag)
	{
		string ret = "";
		const int buf_size = queryXml.length();
		XMLCh tempStr[buf_size + 1];
		static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
		DOMImplementationLS *impl = (DOMImplementationLS*)DOMImplementationRegistry::getDOMImplementation(gLS);
		DOMLSParser       *domBuilder = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
		DOMLSInput        *input = impl->createLSInput();

		XMLString::transcode(queryXml.c_str(), tempStr, buf_size);
		input->setStringData(tempStr);

		try
		{
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMEntities, false);
			DOMDocument* doc = domBuilder->parse(input);

			// verify that we get only 3 calls: for the text node, the CDATA section and the root element
			if (doc == NULL || doc->getDocumentElement() == NULL)
			{
				logger->error("[getStringByTagName] document is Null Line:" + std::to_string(__LINE__));
			}

			DOMElement* root = doc->getDocumentElement();
			XMLString::transcode(tag.c_str(), tempStr, buf_size - 1);
			if (root->getElementsByTagName(tempStr) == NULL)
			{
				logger->error("[getStringByTagName] <" + tag + "></" + tag + "> is not defined");
				domBuilder->release();
				input->release();
				return ret;
			}
			DOMNode* bodyNode = root->getElementsByTagName(tempStr)->item(0);
			if (bodyNode == NULL) {
				domBuilder->release();
				input->release();
				return ret;
			}

			string portStr;
			getChildText(bodyNode, portStr);
			ret = portStr;

		}
		catch (DOMException& e)
		{
			logger->error("[getStringByTagName] DOMException Line:" + std::to_string(__LINE__) + queryXml);
			domBuilder->release();
			input->release();
			throw e;
		}
		catch (const std::invalid_argument& e)
		{
			logger->error("[getStringByTagName] invalid_argument Line:" + std::to_string(__LINE__) + queryXml);
			domBuilder->release();
			input->release();
			throw e;
		}
		domBuilder->release();
		input->release();
		return ret;
	}

	/**
	 * XML文字列から受付ポート番号を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	queryXml	XML文字列
	 *
	 * @return	受付ポート番号
	 */

	int InformationSourceParser::getReceptionPort(const string &queryXml)
	{
		int ret = 0;
		string portStr = getStringByTagName(queryXml, "port");
		if (portStr.length() > 0) {
			ret = stoi(portStr);
		}
		return ret;
	}

	/**
	* XML文字列から送信元SIDを取得する
	*
	* @author	Nagoya University
	* @date	2018/08/28
	*
	* @param	queryXml	XML文字列
	*
	* @return	送信先SID
	*/

	//int InformationSourceParser::getSenderSID(const string &queryXml)
	unsigned long long InformationSourceParser::getSenderSID(const string &queryXml)
	{
		//int ret = 0;
		unsigned long long ret = 0;
		const int buf_size = queryXml.length();
		XMLCh tempStr[buf_size + 1];
		static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
		DOMImplementationLS *impl = (DOMImplementationLS*)DOMImplementationRegistry::getDOMImplementation(gLS);
		DOMLSParser       *domBuilder = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
		DOMLSInput        *input = impl->createLSInput();

		XMLString::transcode(queryXml.c_str(), tempStr, buf_size);
		input->setStringData(tempStr);

		try
		{
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMEntities, false);
			DOMDocument* doc = domBuilder->parse(input);

			// verify that we get only 3 calls: for the text node, the CDATA section and the root element
			if (doc == NULL || doc->getDocumentElement() == NULL)
			{
				logger->error("[getSenderSID] document is Null Line:" + std::to_string(__LINE__));
			}

			DOMElement* root = doc->getDocumentElement();
			XMLString::transcode("sender", tempStr, buf_size - 1);
			if (root->getElementsByTagName(tempStr) == NULL)
			{
				logger->error("[getSenderSID] <sender></sender> is not defined");
				return ret;
			}
			DOMNode* bodyNode = root->getElementsByTagName(tempStr)->item(0);
			string senderSID = getChildText(bodyNode);
			if (senderSID != "") {
				ret = std::stoull(senderSID);
			}
		}
		catch (DOMException& e)
		{
			logger->error("[getSenderSID] DOMException Line:" + std::to_string(__LINE__));
			throw e;
		}
		catch (const std::invalid_argument& e)
		{
			logger->error("[getSenderSID] invalid_argument Line:" + std::to_string(__LINE__) + queryXml);
			throw e;
		}
		domBuilder->release();
		input->release();
		return ret;
	}

	/**
	 * XML文字列から送信先SIDを取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	queryXml	XML文字列
	 *
	 * @return	送信先SID
	 */
	//int InformationSourceParser::getDestinationSID(const string &queryXml)
	unsigned long long InformationSourceParser::getDestinationSID(const string &queryXml)
	{
		//int ret = 0;
		unsigned long long ret = 0;
		if (queryXml.find("destination") == string::npos) return ret;
		const int buf_size = queryXml.length();
		XMLCh tempStr[buf_size + 1];
		static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
		DOMImplementationLS *impl = (DOMImplementationLS*)DOMImplementationRegistry::getDOMImplementation(gLS);
		DOMLSParser       *domBuilder = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
		DOMLSInput        *input = impl->createLSInput();

		XMLString::transcode(queryXml.c_str(), tempStr, buf_size);
		input->setStringData(tempStr);

		try
		{
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMEntities, false);
			DOMDocument* doc = domBuilder->parse(input);

			// verify that we get only 3 calls: for the text node, the CDATA section and the root element
			if (doc == NULL || doc->getDocumentElement() == NULL)
			{
				logger->error("[getDestinationSID] document is Null Line:" + std::to_string(__LINE__));
			}

			DOMElement* root = doc->getDocumentElement();
			XMLString::transcode("destination", tempStr, buf_size - 1);
			if (root->getElementsByTagName(tempStr) == NULL)
			{
				logger->error("[getDestinationSID] <destination></destination> is not defined");
				return ret;
			}
			DOMNode* bodyNode = root->getElementsByTagName(tempStr)->item(0);
			string destId = getChildText(bodyNode);
			if (destId != "") {
				//ret = std::stoi(getChildText(bodyNode));
				ret = std::stoull(getChildText(bodyNode));
			}
		}
		catch (DOMException& e)
		{
			logger->error("[getDestinationSID] DOMException Line:" + std::to_string(__LINE__) + queryXml);
			throw e;
		}
		catch (const std::invalid_argument& e)
		{
			logger->error("[getDestinationSID] invalid_argument Line:" + std::to_string(__LINE__) + queryXml);
			throw e;
		}
		domBuilder->release();
		input->release();
		return ret;
	}

	/**
	* XML文字列から送信元SID,送信先SID,データ要求範囲を取得する
	*
	* @author	Nagoya University
	* @date	2019/12/10
	*
	* @param [in]		xmlResult	XML文字列
	* @param [in,out]	senderId	送信元SID
	* @param [in,out]	destId	送信先SID
	* @param [in,out]	requestVehicleId	要求元車両SID
	* @param [in,out]	range	データ要求範囲
	* @param [in,out]	receptionMngId	受付クエリ管理番号
	* @param [in,out]	receptionEdgeId	受付けたエッジSID
	* @param [in,out]	edgeSIDListStr		連携先エッジSID文字列(カンマ区切り)
	*
	*/

	//bool InformationSourceParser::getRequestInfo(const string &xmlResult, int &senderId, int &destId, int &requestVehicleId, int &range, unsigned int &receptionMngId, int &receptionEdgeId, string &edgeSIDListStr)
	bool InformationSourceParser::getRequestInfo(const string &xmlResult, unsigned long long &senderId, unsigned long long &destId, unsigned long long &requestVehicleId, int &range, unsigned int &receptionMngId, unsigned long long &receptionEdgeId, string &edgeSIDListStr)
	{
		const int buf_size = xmlResult.length();
		XMLCh tempStr[buf_size + 1];
		static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
		DOMImplementationLS *impl = (DOMImplementationLS*)DOMImplementationRegistry::getDOMImplementation(gLS);
		DOMLSParser       *domBuilder = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
		DOMLSInput        *input = impl->createLSInput();

		XMLString::transcode(xmlResult.c_str(), tempStr, buf_size);
		input->setStringData(tempStr);

		try
		{
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMEntities, false);
			DOMDocument* doc = domBuilder->parse(input);

			// verify that we get only 3 calls: for the text node, the CDATA section and the root element
			if (doc == NULL || doc->getDocumentElement() == NULL)
			{
				logger->error("[getRequestInfo] document is Null Line:" + std::to_string(__LINE__));
			}

			DOMElement* root = doc->getDocumentElement();

			XMLString::transcode("sender", tempStr, buf_size - 1);
			if (root->getElementsByTagName(tempStr) == NULL)
			{
				logger->error("[getRequestInfo] <sender></sender> is not defined");
				return false;
			}
			DOMNode* senderNode = root->getElementsByTagName(tempStr)->item(0);
			//senderId = std::stoi(getChildText(senderNode));
			senderId = std::stoull(getChildText(senderNode));

			XMLString::transcode("destination", tempStr, buf_size - 1);
			if (root->getElementsByTagName(tempStr) == NULL)
			{
				logger->error("[getRequestInfo] <destination></destination> is not defined");
				return false;
			}
			DOMNode* destNode = root->getElementsByTagName(tempStr)->item(0);
			//destId = std::stoi(getChildText(destNode));
			destId = std::stoull(getChildText(destNode));

			XMLString::transcode("requestVehicleId", tempStr, buf_size - 1);
			if (root->getElementsByTagName(tempStr) == NULL)
			{
				logger->error("[getRequestInfo] <requestVehicleId></requestVehicleId> is not defined");
				return false;
			}
			DOMNode* reqVIdNode = root->getElementsByTagName(tempStr)->item(0);
			//requestVehicleId = std::stoi(getChildText(reqVIdNode));
			requestVehicleId = std::stoull(getChildText(reqVIdNode));

			XMLString::transcode("range", tempStr, buf_size - 1);
			if (root->getElementsByTagName(tempStr) == NULL)
			{
				logger->error("[getRequestInfo] <range></range> is not defined");
				return false;
			}
			DOMNode* rangeNode = root->getElementsByTagName(tempStr)->item(0);
			range = std::stoi(getChildText(rangeNode));

			XMLString::transcode("receptionMngId", tempStr, buf_size - 1);
			if (root->getElementsByTagName(tempStr) == NULL)
			{
				logger->error("[getRequestInfo] <receptionMngId></receptionMngId> is not defined");
				return false;
			}
			DOMNode* receptionNode = root->getElementsByTagName(tempStr)->item(0);
			receptionMngId = std::stoi(getChildText(receptionNode));

			XMLString::transcode("receptionEdgeId", tempStr, buf_size - 1);
			if (root->getElementsByTagName(tempStr) == NULL)
			{
				logger->error("[getRequestInfo] <receptionEdgeId></receptionEdgeId> is not defined");
				return false;
			}
			DOMNode* receptionEdgeNode = root->getElementsByTagName(tempStr)->item(0);
			//receptionEdgeId = std::stoi(getChildText(receptionEdgeNode));
			receptionEdgeId = std::stoull(getChildText(receptionEdgeNode));

			XMLString::transcode("edgeIDs", tempStr, buf_size - 1);
			if (root->getElementsByTagName(tempStr) == NULL)
			{
				logger->error("[getRequestInfo] <edgeIDs></edgeIDs> is not defined");
				return false;
			}
			DOMNode* edgeIDsNode = root->getElementsByTagName(tempStr)->item(0);
			edgeSIDListStr = getChildText(edgeIDsNode);

		}
		catch (DOMException& e)
		{
			logger->error("[getDestinationSID] DOMException Line:" + std::to_string(__LINE__) + xmlResult);
			throw e;
		}
		catch (const std::invalid_argument& e)
		{
			logger->error("[getDestinationSID] invalid_argument Line:" + std::to_string(__LINE__) + xmlResult);
			throw e;
		}
		domBuilder->release();
		input->release();
		return true;
	}

	/**
	* エラーコードを取得する
	*
	* @author	Nagoya University
	* @date	2018/03/15
	*
	* @param	xmlResult	XML文字列
	*
	* @return	エラーコードを返却、エラーコードがない場合は0を返却
	*/

	int InformationSourceParser::getErrorCode(const string &xmlResult)
	{
		string ret = "";
		const int buf_size = xmlResult.length();
		XMLCh tempStr[buf_size + 1];
		static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
		DOMImplementationLS *impl = (DOMImplementationLS*)DOMImplementationRegistry::getDOMImplementation(gLS);
		DOMLSParser       *domBuilder = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
		DOMLSInput        *input = impl->createLSInput();

		XMLString::transcode(xmlResult.c_str(), tempStr, buf_size);
		input->setStringData(tempStr);

		try
		{
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMEntities, false);
			DOMDocument* doc = domBuilder->parse(input);

			// verify that we get only 3 calls: for the text node, the CDATA section and the root element
			if (doc == NULL || doc->getDocumentElement() == NULL)
			{
				return 0;
			}

			DOMElement* root = doc->getDocumentElement();
			XMLString::transcode("error", tempStr, BUF_MAX - 1);
			DOMNodeList* nodeList = root->getElementsByTagName(tempStr);
			if (nodeList->getLength() == 0)
			{
				return 0;
			}

			DOMNode* child = root->getElementsByTagName(tempStr)->item(0)->getFirstChild();
			DOMText* text = static_cast<DOMText*>(child);
			XMLCh* buffer = new XMLCh[XMLString::stringLen(text->getData()) + 1];
			XMLString::copyString(buffer, text->getData());
			XMLString::trim(buffer);
			char* content = XMLString::transcode(buffer);
			delete[] buffer;

			ret = content;
			XMLString::release(&content);

		}
		catch (DOMException&)
		{
			fprintf(stderr, "checking testLSExceptions failed at line %i, xml=%s\n", __LINE__, xmlResult.c_str());
			ret = "0";
		}
		domBuilder->release();
		input->release();
		return std::stoi(ret);
	}

	/**
	 * ルートタグにある指定AttributeのValue(int)を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/08/28
	 *
	 * @param	attrName	Attribute文字列
	 * @param	queryXml	XML文字列
	 *
	 * @return	クエリ管理番号
	 */

	unsigned int InformationSourceParser::getAttrValueFromRootTag(const string &attrName, const string &queryXml)
	{
		unsigned int mngId = 0;
		const int buf_size = queryXml.length();
		XMLCh tempStr[buf_size + 1];
		static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
		DOMImplementationLS *impl = (DOMImplementationLS*)DOMImplementationRegistry::getDOMImplementation(gLS);
		DOMLSParser       *domBuilder = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
		DOMLSInput        *input = impl->createLSInput();

		XMLString::transcode(queryXml.c_str(), tempStr, buf_size);
		input->setStringData(tempStr);

		try
		{
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMEntities, false);
			DOMDocument* doc = domBuilder->parse(input);

			// verify that we get only 3 calls: for the text node, the CDATA section and the root element
			if (doc == NULL || doc->getDocumentElement() == NULL)
			{
				logger->error("[getAttrValueFromRootTag] document is Null Line:" + std::to_string(__LINE__));
			}

			DOMElement* root = doc->getDocumentElement();

			// resultから指定AttributeのValueを取得
			string queryId;
			getTagValue(root, attrName, queryId);
			if (queryId.length() != 0) {
				mngId = std::stoul(queryId);
			}

		}
		catch (DOMException& e)
		{
			logger->error("[getAttrValueFromRootTag] DOMException Line:" + std::to_string(__LINE__) + queryXml);
			throw e;
		}
		catch (const std::invalid_argument& e)
		{
			logger->error("[getAttrValueFromRootTag] invalid_argument Line:" + std::to_string(__LINE__) + queryXml);
			throw e;
		}
		domBuilder->release();
		input->release();
		return mngId;
	}


	/**
	 * ルートタグにある指定AttributeのValueを取得する。receptionSID、senderSID取得用。
	 *
	 * @author	Nagoya University
	 * @date	2018/08/28
	 *
	 * @param	attrName	Attribute文字列
	 * @param	queryXml	XML文字列
	 *
	 * @return	receptionSID or senderSID
	 */

	unsigned long long InformationSourceParser::getAttrValueULongLongFromRootTag(const string &attrName, const string &queryXml)
	{
		unsigned long long sid = 0;
		const int buf_size = queryXml.length();
		XMLCh tempStr[buf_size + 1];
		static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
		DOMImplementationLS *impl = (DOMImplementationLS*)DOMImplementationRegistry::getDOMImplementation(gLS);
		DOMLSParser       *domBuilder = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
		DOMLSInput        *input = impl->createLSInput();

		XMLString::transcode(queryXml.c_str(), tempStr, buf_size);
		input->setStringData(tempStr);

		try
		{
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMEntities, false);
			DOMDocument* doc = domBuilder->parse(input);

			// verify that we get only 3 calls: for the text node, the CDATA section and the root element
			if (doc == NULL || doc->getDocumentElement() == NULL)
			{
				logger->error("[getAttrValueFromRootTag] document is Null Line:" + std::to_string(__LINE__));
			}

			DOMElement* root = doc->getDocumentElement();

			// resultから指定AttributeのValueを取得
			string strSID;
			getTagValue(root, attrName, strSID);
			if (strSID.length() != 0) {
				sid = std::stoul(strSID);
			}

		}
		catch (DOMException& e)
		{
			logger->error("[getAttrValueFromRootTag] DOMException Line:" + std::to_string(__LINE__) + queryXml);
			throw e;
		}
		catch (const std::invalid_argument& e)
		{
			logger->error("[getAttrValueFromRootTag] invalid_argument Line:" + std::to_string(__LINE__) + queryXml);
			throw e;
		}
		domBuilder->release();
		input->release();
		return sid;
	}


	/**
	* ルートタグにある指定AttributeのValue(string)を取得する
	*
	* @author	Nagoya University
	* @date	2018/10/29
	*
	* @param	attrName	Attribute文字列
	* @param	queryXml	XML文字列
	*
	* @return	文字列
	*/

	string InformationSourceParser::getAttrValueStrFromRootTag(const string &attrName, const string &queryXml)
	{
		string ret;
		const int buf_size = queryXml.length();
		XMLCh tempStr[buf_size + 1];
		static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
		DOMImplementationLS *impl = (DOMImplementationLS*)DOMImplementationRegistry::getDOMImplementation(gLS);
		DOMLSParser       *domBuilder = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
		DOMLSInput        *input = impl->createLSInput();

		XMLString::transcode(queryXml.c_str(), tempStr, buf_size);
		input->setStringData(tempStr);

		try
		{
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMEntities, false);
			DOMDocument* doc = domBuilder->parse(input);

			// verify that we get only 3 calls: for the text node, the CDATA section and the root element
			if (doc == NULL || doc->getDocumentElement() == NULL)
			{
				logger->error("[getAttrValueFromRootTag] document is Null Line:" + std::to_string(__LINE__));
			}

			DOMElement* root = doc->getDocumentElement();

			// resultから指定AttributeのValueを取得
			string functionName;
			getTagValue(root, attrName, functionName);
			if (functionName.length() != 0) {
				ret = functionName;
			}
		}
		catch (DOMException& e)
		{
			logger->error("[getAttrValueFromRootTag] DOMException Line:" + std::to_string(__LINE__) + queryXml);
			throw e;
		}
		catch (const std::invalid_argument& e)
		{
			logger->error("[getAttrValueFromRootTag] invalid_argument Line:" + std::to_string(__LINE__) + queryXml);
			throw e;
		}
		domBuilder->release();
		input->release();
		return ret;
	}

	/**
	 * XML文字列からrootタグ名を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	queryXml	XML文字列
	 * @param [in,out]	result	rootタグ名
	 *
	 * @return	bool(取得成功/取得失敗)
	 */

	bool InformationSourceParser::getRootTagName(const string &queryXml, string &result)
	{
		const int buf_size = queryXml.length();
		XMLCh tempStr[buf_size + 1];
		static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
		DOMImplementationLS *impl = (DOMImplementationLS*)DOMImplementationRegistry::getDOMImplementation(gLS);
		DOMLSParser       *domBuilder = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
		DOMLSInput        *input = impl->createLSInput();

		XMLString::transcode(queryXml.c_str(), tempStr, buf_size);
		input->setStringData(tempStr);

		try
		{
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMEntities, false);
			DOMDocument* doc = domBuilder->parse(input);

			// verify that we get only 3 calls: for the text node, the CDATA section and the root element
			if (doc == NULL || doc->getDocumentElement() == NULL)
			{
				logger->error("[getRootTagName] document is Null Line:" + std::to_string(__LINE__) + ", payload:" + queryXml);
				return false;
			}

			DOMElement* root = doc->getDocumentElement();
			getTagName(root, result);

		}
		catch (DOMException&)
		{
			logger->error("[getRootTagName] DOMException Line:" + std::to_string(__LINE__) + ", payload:" + queryXml);
			return false;
		}
		domBuilder->release();
		input->release();
		return true;
	}

	/**
	* XML文字列からrootタグ名を取得する
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param	queryXml	XML文字列
	* @param [in,out]	rootTag	rootタグ名
	* @param [in,out]	key	ハッシュ値
	*
	* @return	bool(取得成功/取得失敗)
	*/

	bool InformationSourceParser::getRootTagNameAndKey(const string &queryXml, string &rootTag, string &key)
	{
		const int buf_size = queryXml.length();
		XMLCh tempStr[buf_size + 1];
		static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
		DOMImplementationLS *impl = (DOMImplementationLS*)DOMImplementationRegistry::getDOMImplementation(gLS);
		DOMLSParser       *domBuilder = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
		DOMLSInput        *input = impl->createLSInput();

		XMLString::transcode(queryXml.c_str(), tempStr, buf_size);
		input->setStringData(tempStr);

		try
		{
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMEntities, false);
			DOMDocument* doc = domBuilder->parse(input);

			// verify that we get only 3 calls: for the text node, the CDATA section and the root element
			if (doc == NULL || doc->getDocumentElement() == NULL)
			{
				logger->error("[getRootTagName] document is Null Line:" + std::to_string(__LINE__) + ", payload:" + queryXml);
				return false;
			}

			DOMElement* root = doc->getDocumentElement();
			getTagName(root, rootTag);
			getTagValue(root, "key", key);

		}
		catch (DOMException&)
		{
			logger->error("[getRootTagName] DOMException Line:" + std::to_string(__LINE__) + ", payload:" + queryXml);
			return false;
		}
		domBuilder->release();
		input->release();
		//delete domBuilder;
		//delete input;
		//delete impl;
		return true;
	}

	/**
	* XML文字列からユーザ認証情報を取得する
	*
	* @author	Nagoya University
	* @date	2019/07/16
	*
	* @param	queryXml	XML文字列
	* @param [in,out]	user	ユーザID
	* @param [in,out]	hash	ハッシュ値
	* @param [in,out]	pid	プロセスID
	*/

	void InformationSourceParser::getUserAuthInfo(const string &queryXml, string &user, string &hash, string &pid)
	{
		const int buf_size = queryXml.length();
		XMLCh tempStr[buf_size + 1];
		static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
		DOMImplementationLS *impl = (DOMImplementationLS*)DOMImplementationRegistry::getDOMImplementation(gLS);
		DOMLSParser       *domBuilder = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
		DOMLSInput        *input = impl->createLSInput();

		XMLString::transcode(queryXml.c_str(), tempStr, buf_size);
		input->setStringData(tempStr);

		try
		{
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMEntities, false);
			DOMDocument* doc = domBuilder->parse(input);

			// verify that we get only 3 calls: for the text node, the CDATA section and the root element
			if (doc == NULL || doc->getDocumentElement() == NULL)
			{
				logger->error("[getUserAuthInfo] document is Null Line:" + std::to_string(__LINE__));
			}

			DOMElement* root = doc->getDocumentElement();

			// resultから指定AttributeのValueを取得
			getTagValue(root, "user", user);
			getTagValue(root, "hash", hash);
			getTagValue(root, "pid", pid);

		}
		catch (DOMException&)
		{
			logger->error("[getRootTagName] DOMException Line:" + std::to_string(__LINE__));
		}
		domBuilder->release();
		input->release();
		return;
	}


	/**
	* XML文字列から再送設定をロードする
	*
	* @author	Nagoya University
	* @date	2019/11/27
	*
	* @param	queryXml	XML文字列
	* @return	lifeTime
	*/

	int InformationSourceParser::getRetryParam(const string &queryXml)
	{
		const int buf_size = queryXml.length();
		int ret = 0;
		XMLCh tempStr[buf_size + 1];
		static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
		DOMImplementationLS *impl = (DOMImplementationLS*)DOMImplementationRegistry::getDOMImplementation(gLS);
		DOMLSParser       *domBuilder = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
		DOMLSInput        *input = impl->createLSInput();

		XMLString::transcode(queryXml.c_str(), tempStr, buf_size);
		input->setStringData(tempStr);

		try
		{
			domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMEntities, false);
			DOMDocument* doc = domBuilder->parse(input);

			// verify that we get only 3 calls: for the text node, the CDATA section and the root element
			if (doc == NULL || doc->getDocumentElement() == NULL)
			{
				logger->error("[getUserAuthInfo] document is Null Line:" + std::to_string(__LINE__));
			}

			DOMElement* root = doc->getDocumentElement();
			XMLString::transcode("retry", tempStr, BUF_MAX - 1);
			if (root->getElementsByTagName(tempStr) != NULL && root->getElementsByTagName(tempStr)->item(0) != NULL)
			{
				string lifeTime = "";
				getTagValue(dynamic_cast<DOMElement*>(root->getElementsByTagName(tempStr)->item(0)), "life_time", lifeTime);
				ret = stoi(lifeTime);
			}
		}
		catch (DOMException&)
		{
			logger->error("[getRootTagName] DOMException Line:" + std::to_string(__LINE__));
		}
		domBuilder->release();
		input->release();
		return ret;
	}

	/**
	 * 初期処理で保持したスキーマ情報を元にスキーマ情報を生成する(未使用)
	 * 渡したDOMDocumentからschemaを生成するのが最も綺麗な作りであるが
	 * mapに保持しているDOMDocumentがおかしくなるため本関数は未使用(検討の余地あり)
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in,out]	doc   	DOMDocumentオブジェクト
	 * @param [in,out]	schema	スキーマ情報
	 */

	void InformationSourceParser::setSchemaInfo(DOMDocument* doc, Schema* schema)
	{
		//cout << "InformationSourceParser setSchemaInfo START" << endl;
		XMLCh tempStr[BUF_MAX];
		DOMElement* root = doc->getDocumentElement();
		XMLString::transcode("column", tempStr, BUF_MAX - 1);
		root->getElementsByTagName(tempStr);
		if (root->getElementsByTagName(tempStr) == NULL)
		{
			logger->error("[setSchemaInfo]<body></body> is not defined");
			return;
		}
		for (unsigned int i = 0; i<root->getElementsByTagName(tempStr)->getLength(); i++)
		{
			IS::Attribute attr;
			DOMNode* columnNode = root->getElementsByTagName(tempStr)->item(i);
			DOMNodeList* nameList = getElementByCharTagName(dynamic_cast<DOMElement*>(columnNode), (char*)"name");
			getChildText(nameList->item(0), attr.name);
			DOMNodeList* typeList = getElementByCharTagName(dynamic_cast<DOMElement*>(columnNode), (char*)"type");
			getChildText(typeList->item(0), attr.type);
			schema->append(attr);
		}
		//cout << "InformationSourceParser setSchemaInfo  END" << endl;
	}

	/**
	 * 設定ファイルを元にスキーマ情報を生成する
	 * 暫定対応。本来は上記のDOMDocumentを渡してschema情報を得られるのが望ましいが
	 * parserをdeleteする事で取得できたDOMDocumentがおかしくなるため
	 * 現状、もう一度ファイルから読み込む形で実装。
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in,out]	schema	スキーマ情報
	 * @param [in]		tablename	テーブル名
	 */

	void InformationSourceParser::setSchemaInfo(Schema* schema, const string &tablename, const string &schema_path)
	{
		DOMDocument* ret;
		XercesDOMParser* parser = new XercesDOMParser();
		parser->setValidationScheme(XercesDOMParser::Val_Always);
		parser->setDoNamespaces(true); // optional
		ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
		parser->setErrorHandler(errHandler);

		string path = schema_path + schema->getTableName() + ".xml";
		const char* xmlFile = path.c_str();

		try {
			parser->parse(xmlFile);
			ret = parser->getDocument();
			//write(ret);
			// TODO : ここではgetDocumentElement()配下の要素に対してアクセス出来るが戻り値では参照できない
			// たぶんDOMDocumentがポインタなので、スコープの関係上、戻り値では不完全なデータになる？？
		}
		catch (const XMLException& toCatch) {
			char* message = XMLString::transcode(toCatch.getMessage());
			string messageStr(message);
			logger->error("[setSchemaInfo] XMLException message is: \n" + messageStr);
			XMLString::release(&message);
			delete parser;
			delete errHandler;
			return;
		}
		catch (const DOMException& toCatch) {
			char* message = XMLString::transcode(toCatch.msg);
			string messageStr(message);
			logger->error("[setSchemaInfo] DOMException message is: \n" + messageStr);
			XMLString::release(&message);
			delete parser;
			delete errHandler;
			return;
		}
		catch (const exception &e) {
			string what(e.what());
			logger->error("[setSchemaInfo] Unexpected Exception message is: \n" + what);
			delete parser;
			delete errHandler;
			return;
		}

		XMLCh tempStr[BUF_MAX];
		DOMElement* root = ret->getDocumentElement();

		if (root == NULL || root == nullptr) {
			logger->error("InformationSourceParser root is NULL");
			return;
		}
		XMLString::transcode("access_privileges", tempStr, BUF_MAX - 1);
		if (root->getElementsByTagName(tempStr) != NULL && root->getElementsByTagName(tempStr)->item(0) != NULL)
		{
			DOMNode* bodyNode = root->getElementsByTagName(tempStr)->item(0);
			string accessTxt;
			getChildText(bodyNode, accessTxt);
			schema->setAccessPrivileges(accessTxt);
		}
		else {
			logger->error("[setSchemaInfo] <access_privileges></access_privileges> is not defined. file:" + tablename + ".xml");
		}		

		XMLString::transcode("column", tempStr, BUF_MAX - 1);
		if (root->getElementsByTagName(tempStr) == NULL)
		{
			logger->error("<column></column> is not defined");
			return;
		}
		for (unsigned int i = 0; i<root->getElementsByTagName(tempStr)->getLength(); i++)
		{
			IS::Attribute attr;
			DOMNode* columnNode = root->getElementsByTagName(tempStr)->item(i);
			DOMNodeList* nameList = getElementByCharTagName(dynamic_cast<DOMElement*>(columnNode), (char*)"name");
			getChildText(nameList->item(0), attr.name);
			DOMNodeList* typeList = getElementByCharTagName(dynamic_cast<DOMElement*>(columnNode), (char*)"type");
			getChildText(typeList->item(0), attr.type);
			schema->append(attr);
		}

		delete parser; // parserをdeleteするとDOMDocumentのデータがおかしくなるため、最後でdelete
		delete errHandler;
	}

	/**
	* ストリームXML文字列からストリーム名とタプル情報をSAXAPIにて取得する
	*
	* @author	Nagoya University
	* @date	2018/04/26
	*
	* @param			streamXml	ストリームXML文字列
	* @param [in,out]	streamName   	ストリーム名
	* @param [in,out]	tuples   	タプル配列
	*/

	void InformationSourceParser::getTuplesBySAX(const string &streamXml, string &streamName, vector<Tuple> &tuples)
	{
		//REL_COMMENT logger->trace("[getTuplesBySAX] IN");

#if MEASURE_MODE == 1
		long startTime = DmUtil::getTimeMicrosec();
		long procTime = startTime;
		int step = 1;
#endif

		SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
		parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
		parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional
		xercesc::MemBufInputSource xmlBuf(reinterpret_cast<const XMLByte*>(streamXml.c_str()), streamXml.size(), "streamXml", false);
		SaxTuplesAllHandler* defaultHandler = new SaxTuplesAllHandler();
		parser->setContentHandler(defaultHandler);
		parser->setErrorHandler(defaultHandler);

#if MEASURE_MODE == 1
		long now = DmUtil::getTimeMicrosec();
		double msec = (now - procTime) / 1000.0;
		logger->info("[getTuplesBySAX] STAT_STEP" + to_string(step++) + " ISP preProc processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

		try
		{
			parser->parse(xmlBuf);

#if MEASURE_MODE == 1
			now = DmUtil::getTimeMicrosec();
			msec = (now - procTime) / 1000.0;
			logger->info("[getTuplesBySAX] STAT_STEP" + to_string(step++) + " ISP parse xml processing time: " + to_string(msec) + "[ms]");
			procTime = now;
#endif

		}
		catch (const XMLException& toCatch) {
			char* message = XMLString::transcode(toCatch.getMessage());
			cout << "Exception message is: \n"
					<< message << ",[" << streamXml << "]\n";
			XMLString::release(&message);
		}
		catch (const SAXParseException& toCatch) {
			char* message = XMLString::transcode(toCatch.getMessage());
			cout << "Exception message is: \n"
					<< message << ",[" << streamXml << "]\n";
			XMLString::release(&message);
		}
		catch (...) {
			cout << "Unexpected Exception \n";
		}

		tuples = defaultHandler->ret;
		streamName = defaultHandler->streamName;

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - procTime) / 1000.0;
		logger->info("[getTuplesBySAX] STAT_STEP" + to_string(step++) + "  ISP ResultSetProc processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

		delete parser;
		delete defaultHandler;

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - procTime) / 1000.0;
		logger->info("[getTuplesBySAX] STAT_STEP" + to_string(step++) + " ISP releaseProc processing time: " + to_string(msec) + "[ms]");
		msec = (now - startTime) / 1000.0;
		logger->info("[getTuplesBySAX] STAT_STEP" + to_string(step++) + " ISP total processing time: " + to_string(msec) + "[ms]");
#endif

		//REL_COMMENT logger->trace("[getTuplesBySAX] OUT");
		return;
	}

	/**
	* ストリームXML文字列から指定したタグの情報をSAXAPIにて取得する(未使用)
	*
	* @author	Nagoya University
	* @date	2018/04/26
	*
	* @param	xmlResult	ストリームXML文字列
	* @param	tagName   	タグ名
	* @param	result	データ
	*/
	void InformationSourceParser::getSingleTextBySAX(const string &xmlResult, const string &tagName, string &result)
	{

#if MEASURE_MODE == 1
		long startTime = DmUtil::getTimeMicrosec();
		long procTime = startTime;
		int step = 1;
#endif

		SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
		parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
		parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional
		xercesc::MemBufInputSource xmlBuf(reinterpret_cast<const XMLByte*>(xmlResult.c_str()), xmlResult.size(), "xmlResult", false);
		SaxSingleResultHandler* defaultHandler = new SaxSingleResultHandler(tagName);
		parser->setContentHandler(defaultHandler);
		parser->setErrorHandler(defaultHandler);

#if MEASURE_MODE == 1
		long now = DmUtil::getTimeMicrosec();
		double msec = (now - procTime) / 1000.0;
		logger->info("[getSingleTextBySAX] STAT_STEP" + to_string(step++) + " ISP preProc processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

		try
		{
			parser->parse(xmlBuf);

#if MEASURE_MODE == 1
			now = DmUtil::getTimeMicrosec();
			msec = (now - procTime) / 1000.0;
			logger->info("[getSingleTextBySAX] STAT_STEP" + to_string(step++) + " ISP parse xml processing time: " + to_string(msec) + "[ms]");
			procTime = now;
#endif

		}
		catch (const XMLException& toCatch) {
			char* message = XMLString::transcode(toCatch.getMessage());
			cout << "Exception message is: \n"
				<< message << "\n";
			XMLString::release(&message);
			return;
		}
		catch (const SAXParseException& toCatch) {
			char* message = XMLString::transcode(toCatch.getMessage());
			cout << "Exception message is: \n"
				<< message << "\n";
			XMLString::release(&message);
			return;
		}
		catch (...) {
			cout << "Unexpected Exception \n";
			return;
		}

		result = defaultHandler->ret;

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - procTime) / 1000.0;
		logger->info("[getSingleTextBySAX] STAT_STEP" + to_string(step++) + "  ISP ResultSetProc processing time: " + to_string(msec) + "[ms]");
		procTime = now;
#endif

		delete parser;
		delete defaultHandler;

#if MEASURE_MODE == 1
		now = DmUtil::getTimeMicrosec();
		msec = (now - procTime) / 1000.0;
		logger->info("[getSingleTextBySAX] STAT_STEP" + to_string(step++) + " ISP releaseProc processing time: " + to_string(msec) + "[ms]");
		msec = (now - startTime) / 1000.0;
		logger->info("[getSingleTextBySAX] STAT_STEP" + to_string(step++) + " ISP total processing time: " + to_string(msec) + "[ms]");
#endif

		return;
	}

	/**
	* XML文字列を分割送信のヘッダ情報を付与してXML配列に分割する
	*
	* @author	Nagoya University
	* @date	2018/04/26
	*
	* @param[in]		xmlResult	XML文字列
	* @param[in,out]	ret   	XML配列
	* @param[in]		sepSize	分割サイズ
	*/
	int InformationSourceParser::convertToXMLList(const string &xmlResult, vector<string> &ret, const int sepSize)
	{
		// ヘッダを取得する
		string header;
		stringUtil.getHeaderStr(xmlResult, header);

		// UDP上限に達しないように電文を分割する(100はヘッダをappendする分のマージン)
		stringUtil.splitBySize(xmlResult, sepSize - 200, ret);

		// フラグメント最大値を付与
		stringUtil.replaceOnce(header, replaceWordFlgMax, std::to_string(ret.size()));

		int sumLength = 0;
		int noHeaderLen = 0;
		// フラグメント情報を付与しながら各電文にヘッダを付与する
		for (unsigned int i = 0; i < ret.size(); i++) {
			if (i != 0) {
				noHeaderLen = noHeaderLen + ret.at(i).length();
				ret.at(i) = header + "\n" + ret.at(i);
				stringUtil.replaceOnce(ret.at(i), replaceWordFlg, std::to_string(i));
			}
			else {
				stringUtil.replaceOnce(ret.at(i), replaceWordFlgMax, std::to_string(ret.size()));
				stringUtil.replaceOnce(ret.at(i), replaceWordFlg, std::to_string(i));
				noHeaderLen = noHeaderLen + ret.at(i).length();
			}
			sumLength = sumLength + ret.at(i).length();
		}

		int sizeLen = std::to_string(sumLength - replaceWord.length() * ret.size()).length();
		for (unsigned int i = 0; i < ret.size(); i++) {
			stringUtil.replaceOnce(ret.at(i), replaceWord, std::to_string(sumLength - (replaceWord.length() - sizeLen) * ret.size()));
		}
		return noHeaderLen - (replaceWord.length() - sizeLen);
	}

	/**
	* XML結果文字列に存在する置換文字列をサイズ情報に置換する
	*
	* @author	Nagoya University
	* @date	2019/09/18
	*
	* @param[in,out]		result	XML文字列
	*/
	void InformationSourceParser::replaceSizeInfo(string &result)
	{
		int emptySize = result.size() - replaceWord.length();
		int addSize = emptySize + to_string(emptySize).length();
		if (to_string(addSize).length() != to_string(emptySize).length()) {
			stringUtil.replaceOnce(result, replaceWord, std::to_string(addSize + (to_string(addSize).length() - to_string(emptySize).length())));
		}
		else {
			stringUtil.replaceOnce(result, replaceWord, std::to_string(addSize));
		}
	}

	/**
	* 指定したTupleSetを用いてschemaとTupleをXML文字列に変換する
	*
	* @author	Nagoya University
	* @date	2020/02/14
	*
	* @param[in,out]		ts	タプルセット
	* @param[in,out]		result	XML文字列
	*/
	void InformationSourceParser::appendSchemaAndTupleStr(TupleSet &ts, string &ret)
	{
		if (ts.size() == 0) {
			ret.append("<schema/>");
		}
		else {
			vector<bool> isStringList;
			ret.append("<schema>");
			// スキーマ情報
			for (int i = 0; i < ts.getSchemaRef().getAttributeSize(); i++) {
				ret.append("<column><name>");
				ret.append(ts.getSchemaRef().getAttributeName(i));
				ret.append("</name><type>");
				ret.append(ts.getSchemaRef().getAttributeType(i));
				ret.append("</type></column>");
				if (stringUtil.checkDataTypeMatches("string", ts.getSchemaRef().getAttributeType(i), false)) {
					isStringList.push_back(true);
				}
				else {
					isStringList.push_back(false);
				}
			}
			ret.append("</schema>");

			any val;
			long timestamp;
			bool isNull;

			// クエリ結果情報
			for (int idx = 0; idx < ts.size(); idx++)
			{
				ret.append("<tuple>");
				for (int i = 0; i < ts.getTuple(idx).size(); i++)
				{
					ts.getTuple(idx).getValue(i, val, timestamp, isNull);

					ret.append("<data");
					if (timestamp != 0) {
						ret.append(" time=\"");
						ret.append(to_string(timestamp));
						ret.append("\"");
					}
					if (isNull == true) {
						ret.append(" isNull=\"");
						ret.append(to_string(isNull));
						ret.append("\"");
					}
					ret.append(">");
					if (isStringList.at(i)) {
						ret.append("<![CDATA[");
						ret.append(stringUtil.getAnyString(val));
						ret.append("]]>");
					}
					else {
						ret.append(stringUtil.getAnyString(val));
					}
					ret.append("</data>");
				}
				ret.append("</tuple>");
			}
		}
	}
}
