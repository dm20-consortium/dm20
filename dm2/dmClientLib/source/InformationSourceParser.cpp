#include "is/InformationSourceParser.h"
#include "is/DmUtil.h"

#include <iostream>
#include <iomanip>
#include <mutex>

using namespace std;

namespace IS {
	const int BUF_MAX = 4096;
	std::mutex mtx;           // mutex for critical section

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

    inline void SaxEventHandler::startElement(const XMLCh* const uri, const XMLCh* const localname,
                                 const XMLCh* const qname, const xercesc::Attributes &attrs) {

      char* qnames = XMLString::transcode(qname);
      //cout << "start  : " << qnames << endl;

	  if (strcmp(tupleTag, qnames) == 0) {
		  idx = 0;
		  val.clear();
		  time.clear();
		  nullVal.clear();
	  }
	  else if (strcmp(schemaTag, qnames) == 0) {
		  nameList.clear();
		  typeList.clear();
	  }

	  if (attrs.getLength() != 0) {
		  for (int i = 0; i < attrs.getLength(); i++) {
			  //cout << "getLocalName:" << XMLString::transcode(attrs.getLocalName(i)) << endl;
			  //cout << "getValue:" << XMLString::transcode(attrs.getValue(i)) << endl;
			  char *attr = XMLString::transcode(attrs.getLocalName(i));
			  char *value = XMLString::transcode(attrs.getValue(i));
			  if (strcmp(timeAttr, attr) == 0) {
				  timeStr = string(value);
			  }
			  else if (strcmp(isNullAttr, attr) == 0) {
				  isNullStr = string(value);
			  }
			  else if (strcmp(idAttr, attr) == 0) {
				  string str = string(value);
				  mngId = std::stoul(str);
			  }
			  XMLString::release(&attr);
			  XMLString::release(&value);
		  }
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

	inline void SaxEventHandler::endElement  (const XMLCh* const uri, const XMLCh* const localname,
                    const XMLCh* const qname) {

      char* qnames = XMLString::transcode(qname);
      //cout << "end    : " << qnames << endl;

      if (strcmp(dataTag,qnames) == 0) {
        val.insert(std::pair<string, string>(nameList.at(idx), charsStr));
		time.insert(std::pair<string, string>(nameList.at(idx), timeStr.length() == 0 ? "0":timeStr));
		nullVal.insert(std::pair<string, string>(nameList.at(idx), isNullStr.length() == 0 ? "0" : "1"));
		timeStr = "";
		isNullStr = "";
		idx++;
      }

      else if (strcmp(tupleTag,qnames) == 0) {
        valList.push_back(val);
        timeList.push_back(time);
        nullList.push_back(nullVal);
      }
      else if (strcmp(nameTag,qnames) == 0) nameList.push_back(charsStr);
      else if (strcmp(typeTag,qnames) == 0) typeList.push_back(charsStr);
      else if (strcmp(resultTag,qnames) == 0) {
        ResultSetMetaData meta(mngId, nameList, typeList);
        ResultSet result(valList, timeList, nullList, meta);
        ret = result;
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

	inline void SaxEventHandler::characters(const XMLCh* const chars, const XMLSize_t length) {

		char* charss = XMLString::transcode(chars);
		if (chars != NULL && strstr(charss, "\n") == NULL) {
			//cout << "Char   : " << charss << "[" << length << "]" << endl;
			charsStr.append(charss);
		}
		XMLString::release(&charss);
	}

	/**
	 * コンストラクタ
	 * Xercesの初期化処理を実施する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/15
	 */

	InformationSourceParser::InformationSourceParser()
	{
		mtx.lock();
		try {
			XMLPlatformUtils::Initialize();
		}
		catch (const XMLException& toCatch) {
			char* message = XMLString::transcode(toCatch.getMessage());
			cout << "Error during initialization! :\n" << message << "\n";
			XMLString::release(&message);
			mtx.unlock();
			return;
		}
		mtx.unlock();
	}

	/**
	 * デストラクタ
	 * Xercesの終了処理を実施する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/15
	 */

	InformationSourceParser::~InformationSourceParser()
	{
		mtx.lock();
		try {
			XMLPlatformUtils::Terminate();
		}
		catch (const XMLException& toCatch) {
			char* message = XMLString::transcode(toCatch.getMessage());
			cout << "Error during Terminate! :\n" << message << "\n";
			XMLString::release(&message);
			mtx.unlock();
			return;
		}
		mtx.unlock();
	}

	void InformationSourceParser::createChangeidXML(const string &query, const int receptionPort, const string &key, string &result, const bool &getTcpSession)
	{
		XMLCh tempStr[BUF_MAX];
		XMLCh tempStr2[BUF_MAX];
		XMLString::transcode("Range", tempStr, BUF_MAX - 1);
		DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

		XMLString::transcode("change_id", tempStr, BUF_MAX - 1);
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

		if (receptionPort != 0) {
			// port情報
			DOMElement*   port = createElement(doc, "port");
			appendDomText(doc, port, std::to_string(receptionPort));
			rootQuery->appendChild(port);
		}
		if (getTcpSession) {
			// TCP Sessionをサーバへ要求
			DOMElement*   tcp = createElement(doc, "tcp_session");
			appendDomText(doc, tcp, std::to_string(true));
			rootQuery->appendChild(tcp);
		}
		outputXML(doc, result);
		replaceSizeInfo(result);

		// done with the document, must call release() to release the entire document resources
		doc->release();
		return;
	}

	/**
	 * Query発行時のXML文字列を生成する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/15
	 *
	 * @param	query		 	クエリ文字列
	 * @param	receptionPort	受付ポート番号(One-shot: 0, Continuous Query: 0以外)
	 * @param	key		セッションキー
	 * @param [in,out]	result XML文字列
	 * @param   getTcpSession   TCP Sessionをサーバへ要求
	 * 
	 */

	void InformationSourceParser::createQueryXML(const string &query, const int receptionPort, const string &key, string &result, const bool &getTcpSession)
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

		if (receptionPort != 0) {
			// port情報
			DOMElement*   port = createElement(doc, "port");
			appendDomText(doc, port, std::to_string(receptionPort));
			rootQuery->appendChild(port);
		}
		if (getTcpSession) {
			// TCP Sessionをサーバへ要求
			DOMElement*   tcp = createElement(doc, "tcp_session");
			appendDomText(doc, tcp, std::to_string(true));
			rootQuery->appendChild(tcp);
		}
		outputXML(doc, result);
		replaceSizeInfo(result);

		// done with the document, must call release() to release the entire document resources
		doc->release();
		return;
	}
	/**
	 * Query発行時のXML文字列を生成する
	 *
	 * @author	Shinichi Kusayama
	 * @date	2023/08/14
	 *
	 * @param	query		 	クエリ文字列
	 * @param	key		セッションキー
	 * @param [in,out]	result XML文字列
 	 * @param	destSID	宛先SID
	 * 
	 */

	void InformationSourceParser::createQueryXML(const string &query, const string &key, string &result, const unsigned long long destSID, const unsigned long long executerSID)
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

		// sender情報
		DOMElement*   sender = createElement(doc, "sender");
		if (executerSID == 0) {
			appendDomText(doc, sender, "");
		} else {
			string executerSIDstr = to_string(executerSID);
			appendDomText(doc, sender, executerSIDstr);
		}
		rootQuery->appendChild(sender);

		// destination情報
		DOMElement*   destination = createElement(doc, "destination");
		string destSIDstr = to_string(destSID);
		appendDomText(doc, destination, destSIDstr);
		rootQuery->appendChild(destination);

		// body情報
		DOMElement*   body = createElement(doc, "body");
		appendDomText(doc, body, query);
		rootQuery->appendChild(body);

		outputXML(doc, result);
		replaceSizeInfo(result);

		// done with the document, must call release() to release the entire document resources
		doc->release();
		return;
	}

	/**
	 * ストリームデータ送信時のXML文字列を生成する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/15
	 *
	 * @param	senderId	 	送信者ID
	 * @param	destinationId	送信先ID（ストリームデータは不特定なエッジが送信先のため"-"指定となる
	 * @param	streamName   	ストリーム名
	 * @param	tuples		 	タプルリスト
	 * @param	key		セッションキー
	 * @param [in,out]	result XML文字列
	 * 
	 */

	void InformationSourceParser::createStreamXML(const string &senderId, const string &destinationId, const string &streamName, const vector<Tuple> &tuples, const string &key, string &result)
	{
		XMLCh tempStr[BUF_MAX];
		XMLCh tempStr2[BUF_MAX];
		XMLString::transcode("Range", tempStr, BUF_MAX - 1);
		DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

		XMLString::transcode("stream", tempStr, BUF_MAX - 1);
		DOMDocument*   doc = impl->createDocument(0, tempStr, 0);
		DOMElement*   rootStream = doc->getDocumentElement();

		// サイズ情報
		XMLString::transcode("size", tempStr, BUF_MAX - 1);
		XMLString::transcode(replaceWord.c_str(), tempStr2, BUF_MAX - 1);
		rootStream->setAttribute(tempStr, tempStr2);

		// キー情報
		XMLString::transcode("key", tempStr, BUF_MAX - 1);
		XMLString::transcode(key.c_str(), tempStr2, BUF_MAX - 1);
		rootStream->setAttribute(tempStr, tempStr2);

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
		appendDomText(doc, tablename, streamName);
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
		for (Tuple tuple : tuples)
		{
			DOMElement*   tupleEle = doc->createElement(chTuple);
			for (int i = 0; i < tuple.size(); i++)
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
	* ストリームデータ送信時のXML文字列を生成する
	*
	* @author	Nagoya University
	* @date	2018/03/15
	*
	* @param	senderId	 	送信者ID
	* @param	destinationId	送信先ID（ストリームデータは不特定なエッジが送信先のため"-"指定となる
	* @param	streamName   	ストリーム名
	* @param	tuples		 	タプルリスト
	* @param	key		セッションキー
	* @param [in,out]	result XML文字列
	*
	* @return	電文長
	*
	*/

	int InformationSourceParser::createStreamXMLList(const string &senderId, const string &destinationId, const string &streamName, const vector<Tuple> &tuples, const string &key, const int sepSize, vector<string> &result)
	{
		XMLCh tempStr[BUF_MAX];
		XMLCh tempStr2[BUF_MAX];
		XMLString::transcode("Range", tempStr, BUF_MAX - 1);
		DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

		XMLString::transcode("stream", tempStr, BUF_MAX - 1);
		DOMDocument*   doc = impl->createDocument(0, tempStr, 0);
		DOMElement*   rootStream = doc->getDocumentElement();

		// サイズ情報
		XMLString::transcode("size", tempStr, BUF_MAX - 1);
		XMLString::transcode(replaceWord.c_str(), tempStr2, BUF_MAX - 1);
		rootStream->setAttribute(tempStr, tempStr2);

		// キー情報
		XMLString::transcode("key", tempStr, BUF_MAX - 1);
		XMLString::transcode(key.c_str(), tempStr2, BUF_MAX - 1);
		rootStream->setAttribute(tempStr, tempStr2);

		// 生成時間
		XMLString::transcode("ct", tempStr, BUF_MAX - 1);
		XMLString::transcode(std::to_string(DmUtil::getTimeMillisec()).c_str(), tempStr2, BUF_MAX - 1);
		rootStream->setAttribute(tempStr, tempStr2);

		// フラグメント情報
		XMLString::transcode("flagment", tempStr, BUF_MAX - 1);
		XMLString::transcode(replaceWordFlg.c_str(), tempStr2, BUF_MAX - 1);
		rootStream->setAttribute(tempStr, tempStr2);

		// フラグメント情報(Max)
		XMLString::transcode("flagmentMax", tempStr, BUF_MAX - 1);
		XMLString::transcode(replaceWordFlgMax.c_str(), tempStr2, BUF_MAX - 1);
		rootStream->setAttribute(tempStr, tempStr2);

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
		appendDomText(doc, tablename, streamName);
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
		for (Tuple tuple : tuples)
		{
			DOMElement*   tupleEle = doc->createElement(chTuple);
			for (int i = 0; i < tuple.size(); i++)
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
		string xmlResult = "";

		outputXML(doc, xmlResult);
		int sizeLen = convertToXMLList(xmlResult, result, sepSize);
		
		// done with the document, must call release() to release the entire document resources
		doc->release();
		return sizeLen;
	}

	/**
	 * 継続クエリのキャンセル時のXML文字列を生成する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/15
	 *
	 * @param	mngId	クエリ管理番号
	 * @param	key		セッションキー
	 * @param [in,out]	result XML文字列
	 * 
	 */

	void InformationSourceParser::createCancelXML(unsigned int mngId, const string &key, string &result)
	{
		XMLCh tempStr1[BUF_MAX];
		XMLCh tempStr2[BUF_MAX];
		XMLString::transcode("Range", tempStr1, BUF_MAX - 1);
		DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr1);

		XMLString::transcode("cancel", tempStr1, BUF_MAX - 1);
		DOMDocument*   doc = impl->createDocument(0, tempStr1, 0);
		DOMElement*   resultEl = doc->getDocumentElement();

		// サイズ情報
		XMLString::transcode("size", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWord.c_str(), tempStr2, BUF_MAX - 1);
		resultEl->setAttribute(tempStr1, tempStr2);

		// 管理番号
		XMLString::transcode("id", tempStr1, BUF_MAX - 1);
		XMLString::transcode(std::to_string(mngId).c_str(), tempStr2, BUF_MAX - 1);
		resultEl->setAttribute(tempStr1, tempStr2);

		// キー情報
		XMLString::transcode("key", tempStr1, BUF_MAX - 1);
		XMLString::transcode(key.c_str(), tempStr2, BUF_MAX - 1);
		resultEl->setAttribute(tempStr1, tempStr2);

		outputXML(doc, result);
		replaceSizeInfo(result);
		
		// done with the document, must call release() to release the entire document resources
		doc->release();
		return;
	}

	/**
	* セッション取得要求時のXML文字列を生成する
	*
	* @author	Nagoya University
	* @date	2019/07/14
	*
	* @param	username	ユーザ名
	* @param	md5			ハッシュ値
	* @param	pid			プロセスID
	* @param [in,out]	result XML文字列
	*
	*/

	void InformationSourceParser::createSessionXML(const string &username, const string &md5, const string &pid, string &result)
	{
		XMLCh tempStr1[BUF_MAX];
		XMLCh tempStr2[BUF_MAX];
		XMLString::transcode("Range", tempStr1, BUF_MAX - 1);
		DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr1);

		XMLString::transcode("create_session", tempStr1, BUF_MAX - 1);
		DOMDocument*   doc = impl->createDocument(0, tempStr1, 0);
		DOMElement*   resultEl = doc->getDocumentElement();

		// サイズ情報
		XMLString::transcode("size", tempStr1, BUF_MAX - 1);
		XMLString::transcode(replaceWord.c_str(), tempStr2, BUF_MAX - 1);
		resultEl->setAttribute(tempStr1, tempStr2);

		// ユーザID
		XMLString::transcode("user", tempStr1, BUF_MAX - 1);
		XMLString::transcode(username.c_str(), tempStr2, BUF_MAX - 1);
		resultEl->setAttribute(tempStr1, tempStr2);

		// ハッシュ値
		XMLString::transcode("hash", tempStr1, BUF_MAX - 1);
		XMLString::transcode(md5.c_str(), tempStr2, BUF_MAX - 1);
		resultEl->setAttribute(tempStr1, tempStr2);

		// pid
		XMLString::transcode("pid", tempStr1, BUF_MAX - 1);
		XMLString::transcode(pid.c_str(), tempStr2, BUF_MAX - 1);
		resultEl->setAttribute(tempStr1, tempStr2);

		outputXML(doc, result);
		replaceSizeInfo(result);

		// done with the document, must call release() to release the entire document resources
		doc->release();
		return;
	}
	/**
	* 指定したタグ名のエレメントを生成する
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param [in,out]	doc	DOMDocumentオブジェクト
	* @param 		  	tag	タグ名
	*
	* @return	エレメント
	*/

	DOMElement* InformationSourceParser::createElement(DOMDocument* doc, const string &tag)
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

	void InformationSourceParser::appendDomText(DOMDocument* doc, DOMElement* ele, const string &value)
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
	* @param [in,out]	result 取得文字列
	* 
	*/

	inline void InformationSourceParser::getChildText(DOMNode* ele, string &result)
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
			cout << "Not existed child element. what:" << e.what() << endl;
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

	inline DOMNodeList* InformationSourceParser::getElementByCharTagName(DOMElement* elem, const char *tagName)
	{
		XMLCh* attributeName = XMLString::transcode(tagName);
		DOMNodeList* nlist = elem->getElementsByTagName(attributeName);
		XMLString::release(&attributeName);

		return nlist;
	}

	/**
	* タグ内のattributeIDを指定し、値を取得する
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param [in,out]	element	指定エレメント
	* @param 		  	id	   	AttributeID
	* @param [in,out]	result 値
	* 
	*/

	void InformationSourceParser::getTagValue(DOMElement* element, const string &id, string &result) {

		string atrName;
		DOMNamedNodeMap* map = element->getAttributes();
		for (XMLSize_t i = 0; i < map->getLength(); i++) {
			DOMAttr* attr = static_cast<DOMAttr*>(map->item(i));
			char* attr_name = XMLString::transcode(attr->getName());
			atrName = attr_name;
			if (id == atrName) {
				char* attr_value = XMLString::transcode(attr->getValue());
				result = attr_value;
				XMLString::release(&attr_name);
				XMLString::release(&attr_value);
				break;
			}
			XMLString::release(&attr_name);
		}
		return;
	}

	/**
	* 指定ファイルパスのXMLファイルをパースし、XML文字列で取得する(未使用)
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param	filePath	ファイルパス
	*
	* @return	XML文字列
	*/

	void InformationSourceParser::parseXmlFile(const string &filePath)
	{
		XercesDOMParser* parser = new XercesDOMParser();
		parser->setValidationScheme(XercesDOMParser::Val_Always);
		parser->setDoNamespaces(true); // optional

		ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
		parser->setErrorHandler(errHandler);

		const char* xmlFile = filePath.c_str();

		try {
			parser->parse(xmlFile);
			DOMDocument* dom = parser->getDocument();
			write(dom);
		}
		catch (const XMLException& toCatch) {
			char* message = XMLString::transcode(toCatch.getMessage());
			cout << "Exception message is: \n" << message << "\n";
			XMLString::release(&message);
			delete parser;
			delete errHandler;
			return;
		}
		catch (const DOMException& toCatch) {
			char* message = XMLString::transcode(toCatch.msg);
			cout << "Exception message is: \n" << message << "\n";
			XMLString::release(&message);
			delete parser;
			delete errHandler;
			return;
		}
		catch (...) {
			cout << "Unexpected Exception \n";
			return;
		}

		delete parser;
		delete errHandler;
		return;
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
	* @param [in,out]	pmyDOMDocument	DOMDocumentオブジェクト
	* @param [in,out]	result XML文字列
	* 
	*/

	void InformationSourceParser::outputXML(xercesc::DOMDocument* pmyDOMDocument, string &result)
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
	 * クエリ管理番号を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/15
	 *
	 * @param	xmlResult	XML文字列
	 *
	 * @return	クエリ管理番号
	 */

	unsigned int InformationSourceParser::getMngId(const string &xmlResult, int &port)
	{
		unsigned int mngId = 0;
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
				fprintf(stderr, "checking testLSExceptions failed at line %i\n", __LINE__);
			}

			DOMElement* root = doc->getDocumentElement();

			// resultからidを取得
			string idStr, portStr;
			getTagValue(root, "id", idStr);
			mngId = std::stoul(idStr);
			getTagValue(root, "port", portStr);
			if (portStr != "") port = std::stoi(portStr);
			//doc->release();

		}
		catch (DOMException&)
		{
			fprintf(stderr, "checking testLSExceptions failed at line %i\n", __LINE__);

		}
		domBuilder->release();
		input->release();
		return mngId;
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
			fprintf(stderr, "checking testLSExceptions failed at line %i\n", __LINE__);

		}
		domBuilder->release();
		input->release();
		return std::stoi(ret);
	}


	/**
	* エラーメッセージを取得する
	*
	* @author	Nagoya University
	* @date	2019/09/05
	*
	* @param	xmlResult	XML文字列
	*
	* @return	エラーコードを返却、エラーコードがない場合は空文字を返却
	*/

	string InformationSourceParser::getErrorMessage(const string &xmlResult)
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
				return ret;
			}

			DOMElement* root = doc->getDocumentElement();
			XMLString::transcode("msg", tempStr, BUF_MAX - 1);
			DOMNodeList* nodeList = root->getElementsByTagName(tempStr);
			if (nodeList->getLength() == 0)
			{
				return ret;
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
			fprintf(stderr, "checking testLSExceptions failed at line %i\n", __LINE__);

		}
		domBuilder->release();
		input->release();
		return ret;
	}

	/**
	* セッションキーを取得する
	*
	* @author	Nagoya University
	* @date	2019/07/11
	*
	* @param	xmlResult	XML文字列
	*
	* @return	セッションキーを返却、認証エラー時は空文字を返却
	*/

	string InformationSourceParser::getSessionKey(const string &xmlResult)
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
				fprintf(stderr, "checking testLSExceptions failed at line %i, xml:%s\n", __LINE__, xmlResult.c_str());
				return ret;
			}

			DOMElement* root = doc->getDocumentElement();

			// resultからsessionKeyを取得
			getTagValue(root, "value", ret);
			//doc->release();

		}
		catch (DOMException&)
		{
			fprintf(stderr, "checking testLSExceptions failed at line %i\n", __LINE__);

		}
		domBuilder->release();
		input->release();
		return ret;
	}

	/**
	* クエリ結果の列名と値をSAXAPIで取得する
	*
	* @author	Nagoya University
	* @date	2018/04/09
	*
	* @param	xmlResult	XML文字列
	* @param [in,out]	ret リザルトセット
	* 
	*/

	void InformationSourceParser::getResultSetBySAX(const string &xmlResult, ResultSet &ret)
	{
#if MEASURE_MODE == 1
		long startTime = DmUtil::getTimeMillisec();
		long procTime = startTime;
#endif

		SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
		parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
		parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional
		xercesc::MemBufInputSource xmlBuf(reinterpret_cast<const XMLByte*>(xmlResult.c_str()), xmlResult.size(), "xmlResult", false);
		SaxEventHandler* defaultHandler = new SaxEventHandler();
		parser->setContentHandler(defaultHandler);
		parser->setErrorHandler(defaultHandler);
#if MEASURE_MODE == 1
		long now = DmUtil::getTimeMillisec();
		double msec = (now - procTime);
		cout << "[STAT] ISP preProc processTime(msec):" << msec << endl;

		procTime = DmUtil::getTimeMillisec();
#endif
		try
		{
			parser->parse(xmlBuf);
#if MEASURE_MODE == 1
			now = DmUtil::getTimeMillisec();
			msec = (now - procTime);
			cout << "[STAT] ISP parseProc processTime(msec):" << msec << endl;
#endif
		}
		catch (const XMLException& toCatch) {
			char* message = XMLString::transcode(toCatch.getMessage());
			cout << "Exception message is: \n"
				<< message << ",[" << xmlResult << "]\n";
			XMLString::release(&message);
			return;
		}
		catch (const SAXParseException& toCatch) {
			char* message = XMLString::transcode(toCatch.getMessage());
			cout << "Exception message is: \n"
					<< message << ",[" << xmlResult << "]\n";
			XMLString::release(&message);
			return;
		}
		catch (...) {
			cout << "Unexpected Exception \n";
			return;
		}
#if MEASURE_MODE == 1
		procTime = DmUtil::getTimeMillisec();
#endif
		ret = defaultHandler->ret;
#if MEASURE_MODE == 1
		now = DmUtil::getTimeMillisec();
		msec = (now - procTime);
		cout << "[STAT] ISP ResultSetProc processTime(msec):" << msec << endl;
		procTime = DmUtil::getTimeMillisec();
#endif
		delete parser;
		delete defaultHandler;
#if MEASURE_MODE == 1
		now = DmUtil::getTimeMillisec();
		msec = (now - procTime);
		cout << "[STAT] ISP releaseProc processTime(msec):" << msec << endl;
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

		// UDP上限に達しないように電文を分割する(200はヘッダをappendする分のマージン)
		stringUtil.splitBySize(xmlResult, sepSize - 200, ret);

		// フラグメント最大値を付与
		stringUtil.replaceAll(header, replaceWordFlgMax, std::to_string(ret.size()));

		int sumLength = 0;
		int noHeaderLen = 0;
		// フラグメント情報を付与しながら各電文にヘッダを付与する
		for (unsigned int i = 0; i < ret.size(); i++) {
			if (i != 0) {
				noHeaderLen = noHeaderLen + ret.at(i).length();
				ret.at(i) = header + "\n" + ret.at(i);
				stringUtil.replaceAll(ret.at(i), replaceWordFlg, std::to_string(i));
			}
			else {
				stringUtil.replaceAll(ret.at(i), replaceWordFlgMax, std::to_string(ret.size()));
				stringUtil.replaceAll(ret.at(i), replaceWordFlg, std::to_string(i));
				noHeaderLen = noHeaderLen + ret.at(i).length();
			}
			sumLength = sumLength + ret.at(i).length();
		}

		int sizeLen = std::to_string(sumLength - replaceWord.length() * ret.size()).length();
		for (unsigned int i = 0; i < ret.size(); i++) {
			stringUtil.replaceAll(ret.at(i), replaceWord, std::to_string(sumLength - (replaceWord.length() - sizeLen) * ret.size()));
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
#if DEBUG == 1
		cout << "[replaceSizeInfo] result.size:" << result.size() << " replaceWD:" << replaceWord.length() << endl;
#endif
		if (to_string(addSize).length() != to_string(emptySize).length()) {
			stringUtil.replaceAll(result, replaceWord, std::to_string(addSize + (to_string(addSize).length() - to_string(emptySize).length())));
		}
		else {
			stringUtil.replaceAll(result, replaceWord, std::to_string(addSize));
		}
	}
}
