#ifndef INFORMATIONSOURCEPARSER_H
#define INFORMATIONSOURCEPARSER_H

#include "is/StringUtil.h"
#include "is/ResultSet.h"
#include "is/ResultSetMetaData.h"
#include "is/Tuple.h"

#include <string>
#include <vector>
#include <unordered_map>

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

namespace IS {

	/**
	* SAXAPIにて情報源をパースするクラス
	*
	* @author	Nagoya University
	* @date	2018/04/09
	*/
    class SaxEventHandler : public xercesc::DefaultHandler {
      private:
        const char* schemaTag	= "schema";
        const char* tupleTag	= "tuple";
        const char* nameTag		= "name";
        const char* typeTag		= "type";
        const char* dataTag		= "data";
        const char* timestampTag= "timestamp";
        const char* isNullTag	= "isNull";
        const char* resultTag	= "result";
		const char* idAttr		= "id";
		const char* timeAttr = "time";
		const char* isNullAttr = "isNull";

		vector<std::unordered_map<string, string>> valList;
		vector<std::unordered_map<string, string>> timeList;
		vector<std::unordered_map<string, string>> nullList;
		vector<string> nameList;
		vector<string> typeList;
		std::unordered_map<string, string> val;
		std::unordered_map<string, string> time;
		std::unordered_map<string, string> nullVal;

		unsigned int mngId = 0;
		unsigned int size = 0;
		string charsStr = "";
		string timeStr = "";
		string isNullStr = "";
		int idx = 0;

      public:
        ResultSet ret;
        void startElement(const XMLCh* const uri, const XMLCh* const localname,
                          const XMLCh* const qname, const xercesc::Attributes &attrs);
        void endElement  (const XMLCh* const uri, const XMLCh* const localname,
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
		const string replaceWord = "REPSIZE";
		const string replaceWordFlg = "F_NO";
		const string replaceWordFlgMax = "F_MAX";
		StringUtil stringUtil;
		
		void writeXML(xercesc::DOMDocument* pmyDOMDocument, const char *filePath);
		void write(DOMNode* node);
		void writeElement(DOMElement* element);
		void writeText(DOMText* text);
		
		void outputXML(xercesc::DOMDocument* pmyDOMDocument, string &result);

		DOMElement* createElement(DOMDocument* doc, const string &tag);
		void appendDomText(DOMDocument* doc, DOMElement* ele, const string &value);
		inline void getChildText(DOMNode* ele, string &result);
		inline DOMNodeList* getElementByCharTagName(DOMElement* elem, const char *tagName);
		void getTagValue(DOMElement* element, const string &id, string &result);
		int convertToXMLList(const string &xmlResult, vector<string> &ret, const int sepSize);
		void replaceSizeInfo(string &result);

	public:
		InformationSourceParser();
		~InformationSourceParser();

		void createQueryXML(const string &query, const int receptionPort, const string &key, string &result, const bool &getTcpSession);
		void createQueryXML(const string &query, const string &key, string &result, const unsigned long long destSID, const unsigned long long executerSID);
		void createStreamXML(const string &senderId, const string &destinationId, const string &streamName, const vector<Tuple> &tuples, const string &key, string &result);
		int createStreamXMLList(const string &senderId, const string &destinationId, const string &streamName, const vector<Tuple> &tuples, const string &key, const int sepSize, vector<string> &result);
		void createCancelXML(unsigned int mngId, const string &key, string &result);
		void createSessionXML(const string &username, const string &md5, const string &pid, string &result);
		void parseXmlFile(const string &filePath);
		unsigned int getMngId(const string &xmlResult, int &tcpPort);
		int getErrorCode(const string &xmlResult);
		string getErrorMessage(const string &xmlResult);
		string getSessionKey(const string &xmlResult);
		void getResultSetBySAX(const string &xmlResult, ResultSet &ret); // SAXAPI
	};
}
#endif  // INFORMATIONSOURCEPARSER_H
