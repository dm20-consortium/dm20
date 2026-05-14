#ifndef REQUESTOPERATOR_H
#define REQUESTOPERATOR_H

#include <log4cxx/logger.h>

#include "Operator.h"
#include "is/StringUtil.h"
#include "InformationSourceParser.h"
#include "LocationManager.h"
#include "UdpSendInterface.h"

using namespace log4cxx;
using namespace CS;

namespace IS {

	/**
	 * クエリの転送依頼処理を扱うオペレータ
	 *
	 * @author	Nagoya University
	 * @date	2018/08/27
	 */

	class RequestOperator : public Operator, public CS::UdpSendInterface
	{
	private:
		const string MyName = "Request";
		LoggerPtr logger = Logger::getLogger("RequestOperator");
		StringUtil stringUtil;
		IS::Settings &settings = IS::Settings::get_instance();
		//IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();
		IS::LocationManager &LM = IS::LocationManager::get_instance();
		
		string fdDirPath = "";
		/*
		unsigned int requestVehicleSID = 0;
		unsigned int dstSID = 0;
		*/
		unsigned long long requestVehicleSID = 0;
		unsigned long long dstSID = 0;
		string streamName = "";
		string operatorTreeXML = "";
		int range = 0;
		unsigned int mngId = 0;
		unsigned int requestedMngId = 0;
		//unsigned int receptionEdgeId = 0;
		unsigned long long receptionEdgeId = 0;
		unsigned int receptionMngId = 0;
		string edgeSIDListStr = "";

		bool exit_flag = false;

	public:
		/*
		RequestOperator(unsigned int mngId, unsigned int dstSID, string operatorTreeXML);
		RequestOperator(unsigned int mngId, unsigned int dstSID, string operatorTreeXML, unsigned int requestVehicleSID, int range, unsigned int receptionMngId, unsigned int receptionEdgeId, const string &edgeSIDListStr);
		*/
		RequestOperator(unsigned int mngId, unsigned long long dstSID, string operatorTreeXML);
		RequestOperator(unsigned int mngId, unsigned long long dstSID, string operatorTreeXML, unsigned long long requestVehicleSID, int range, unsigned int receptionMngId, unsigned long long receptionEdgeId, const string &edgeSIDListStr);
		virtual ~RequestOperator();

		/*
		bool setRequestedMngId(unsigned int destId, unsigned int mngId, unsigned int senderId);
		void setCancellationRange(const unsigned int senderId);
		unsigned int getDestSID();
		*/
		bool setRequestedMngId(unsigned long long destId, unsigned int mngId, unsigned long long senderId);
		void setCancellationRange(const unsigned long long senderId);
		unsigned long long getDestSID();

		// 処理
		virtual bool process(vector<IS::TupleSet>& ts);
		virtual void exit();
	};
}
#endif  // REQUESTOPERATOR_H
