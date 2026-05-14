#ifndef INFORMATIONSOURCEPARSER_H
#define INFORMATIONSOURCEPARSER_H
#include <is/Tuple.h>
#include <is/TupleSet.h>
#include <is/StringUtil.h>
#include <is/Schema.h>
#include "Settings.h"
#include "ErrorCode.h"
#include <is/DmUtil.h>

#include <string>
#include <vector>
#include <log4cxx/logger.h>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/dom/DOMLSOutput.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/dom/DOMLSParser.hpp>
#include <xercesc/dom/DOMLSInput.hpp>
#include <xercesc/dom/DOMLSParserFilter.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

// For SAX API
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

using namespace std;
using namespace xercesc;
using namespace log4cxx;

namespace IS {


	/**
	* SAXAPIにて情報源をパースするクラス（単一エレメント取得用）
	*
	* @author	Nagoya University
	* @date	2018/04/09
	*/
	class SaxSingleResultHandler : public xercesc::DefaultHandler {
	private:
		string searchTagName;
		string charsStr = "";

	public:
		string ret;
		SaxSingleResultHandler(string tagName);
		void startElement(const XMLCh* const uri, const XMLCh* const localname,
			const XMLCh* const qname, const xercesc::Attributes &attrs);
		void endElement(const XMLCh* const uri, const XMLCh* const localname,
			const XMLCh* const qname);
		void characters(const XMLCh* const chars, const XMLSize_t length);
	};

	/**
	* SAXAPIにて情報源をパースするクラス（タプルとストリーム名を同時に取得）
	*
	* @author	Nagoya University
	* @date	2018/04/25
	*/
	class SaxTuplesAllHandler : public xercesc::DefaultHandler {
	private:
		StringUtil stringUtil;
		Schema schemaInfo;
		string data;
		string timestamp;
		Tuple* tuple = NULL;
		int idx = 0;
		string charsStr = "";
		bool isErr = false;
#if MEASURE_MODE2 == 1
		long createTime = 0;
#endif

		const char* tupleTag = "tuple";
		const char* dataTag = "data";
		const char* timeTag = "timestamp";
		const char* tableNameTag = "tablename";
		const char* timeAttr = "time";

	public:
		vector<Tuple> ret;
		string streamName;
		void startElement(const XMLCh* const uri, const XMLCh* const localname,
			const XMLCh* const qname, const xercesc::Attributes &attrs);
		void endElement(const XMLCh* const uri, const XMLCh* const localname,
			const XMLCh* const qname);
		void characters(const XMLCh* const chars, const XMLSize_t length);
	};

	/**
	 * 情報源をパースするクラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 */

	class InformationSourceParser
	{
	private:
		InformationSourceParser() = default;
		~InformationSourceParser() = default;

		StringUtil stringUtil;
		const string replaceWord = "REPSIZE";
		const string replaceWordFlg = "F_NO";
		const string replaceWordFlgMax = "F_MAX";

		LoggerPtr logger = Logger::getLogger("InformationSourceParser");
		IS::Settings &settings = IS::Settings::get_instance();
		void writeXML(xercesc::DOMDocument* pmyDOMDocument, const char *filePath);
		void write(DOMNode* node);
		void writeElement(DOMElement* element);
		void writeText(DOMText* text);
		DOMElement* createElement(DOMDocument* doc, const string &tag);
		void appendDomText(DOMDocument* doc, DOMElement* ele, const string &value);
		void getTagName(DOMElement* element, string &result);
		void getSingleTextBySAX(const string &xmlResult, const string &tagName, string &result);
		int convertToXMLList(const string &xmlResult, vector<string> &ret, const int sepSize);
		void replaceSizeInfo(string &result);
		void appendSchemaAndTupleStr(TupleSet &ts, string &ret);

	public:
		InformationSourceParser(const InformationSourceParser&) = delete;
		InformationSourceParser& operator=(const InformationSourceParser&) = delete;
		InformationSourceParser(InformationSourceParser&&) = delete;
		InformationSourceParser& operator=(InformationSourceParser&&) = delete;

		static InformationSourceParser& get_instance() {
			static InformationSourceParser inst;
			return inst;
		}

		const int BUF_MAX = 4096;

		void init();
		void finalize();
		DOMDocument* parseXmlFile(const string &filePath);
		void parseXmlFileToString(const string &filePath, string &result);
		DOMDocument* parseSchema(const Schema &schema);
		void setSchemaInfo(Schema* schema, const string &tablename, const string &schema_path);
		void setSchemaInfo(DOMDocument* doc, Schema* schema); // 未使用
		void outputXML(const xercesc::DOMDocument* pmyDOMDocument, string &result);

		// Create XML
		void createQueryXML(const string &query, const int receptionPort, const string &key, const string &dmi, string &result);
		void createQueryResult(const unsigned int mngId, TupleSet& ts, string &result);
		int  createQueryResultList(const unsigned int mngId, TupleSet& ts, const int sepSize, vector<string> &ret);
		void createQueryResultStr(const unsigned int mngId, TupleSet& ts, string &result, const int tcpPort);
		int  createQueryResultStrList(const unsigned int mngId, TupleSet& ts, const int sepSize, vector<string> &ret);
		int  createQueryResult(const string &senderId, const string &destinationId, const unsigned int mngId, const unsigned int requestedMngId, const string &requestedDestId, TupleSet& ts, string &ret);
		int  createQueryResultList(const string &senderId, const string &destinationId, const unsigned int mngId, const unsigned int requestedMngId, TupleSet& ts, const int sepSize, vector<string> &ret);
		void createErrorResult(const unsigned int mngId, const ErrorCode code, const string &msg, string &result);
		int  createErrorResult(const string &senderId, const string &destinationId, const unsigned int mngId, const unsigned int requestedMngId, const string &requestedDestId, const ErrorCode code, string &ret);
		void createStreamXML(const string &senderId, const string &destinationId, TupleSet &tupleset, string &result);
		int  createStreamXMLList(const string &senderId, const string &destinationId, TupleSet &tupleset, const int sepSize, vector<string> &ret);
		void createStreamXMLStr(const string &senderId, const string &destinationId, TupleSet &tupleset, string &ret);
		//int  createCancelXML(unsigned int mngId, const int receptionSID, const int senderSID, string &ret);
		int  createCancelXML(unsigned int mngId, const unsigned long long receptionSID, const unsigned long long senderSID, string &ret);
		//int  createOperatorTreeXML(const string &senderId, const string &destinationId, string operatorTreeXML, const unsigned int mngId, const int requestVehicleId, const int range, const unsigned int receptionMngId, const int receptionEdgeID, const string &edgeSIDListStr, string &ret);
		int  createOperatorTreeXML(const string &senderId, const string &destinationId, string operatorTreeXML, const unsigned int mngId, const unsigned long long requestVehicleId, const int range, const unsigned int receptionMngId, const unsigned long long receptionEdgeID, const string &edgeSIDListStr, string &ret);
		int  createOperatorTreeXMLList(const string &senderId, const string &destinationId, string operatorTreeXML, const unsigned int mngId, const int sepSize, vector<string> &ret);
		void createGeneralSingleResponseXML(const string &rootTag, const string &value, string &ret);
		void createGeneralDataListResponseXML(const vector<vector<string>> &valList, string &ret);

		// Get Information
		void getQueryString(const string &queryXml, string &result);
		void getOperatorTreeXML(const string &queryXml, string &operatorTreeXML);
		int getReceptionPort(const string &queryXml);
		string getStringByTagName(const string &queryXml, const string &tag);
		/*
		int getSenderSID(const string &queryXml);
		int getDestinationSID(const string &queryXml);
		bool getRequestInfo(const string &xmlResult, int &senderId, int &destId, int &requestVehicleId, int &range, unsigned int &receptionMngId, int &receptionEdgeId, string &edgeSIDListStr);
		*/
		unsigned long long getSenderSID(const string &queryXml);
		unsigned long long getDestinationSID(const string &queryXml);
		bool getRequestInfo(const string &xmlResult, unsigned long long &senderId, unsigned long long &destId, unsigned long long &requestVehicleId, int &range, unsigned int &receptionMngId, unsigned long long &receptionEdgeId, string &edgeSIDListStr);
		int getErrorCode(const string &xmlResult);
		unsigned int getAttrValueFromRootTag(const string &attrName, const string &queryXml);
		unsigned long long getAttrValueULongLongFromRootTag(const string &attrName, const string &queryXml);
		string getAttrValueStrFromRootTag(const string &attrName, const string &queryXml);
		bool getRootTagName(const string &queryXml, string &result);
		bool getRootTagNameAndKey(const string &queryXml, string &rootTag, string &key);
		void getUserAuthInfo(const string &queryXml, string &user, string &hash, string &pid);
		int getRetryParam(const string &queryXml);

		void getTuplesBySAX(const string &streamXml, string &streamName, vector<Tuple> &tuples);
		void getTagValue(const DOMElement* element, const string &id, string &ret);
		string getChildText(const DOMNode* ele); // 戻り値として返却
		void getChildText(const DOMNode* ele, string &result);
		DOMNodeList* getElementByCharTagName(DOMElement* elem, char *tagName);

	};
}
#endif  // INFORMATIONSOURCEPARSER_H
