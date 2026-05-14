#ifndef DMIOPERATOR_H
#define DMIOPERATOR_H

#include <log4cxx/logger.h>

#include "Operator.h"
#include "is/StringUtil.h"
#include "QueueManager.h"
#include "QueryManager.h"
#include <is/DmUtil.h>
#include "ErrorCode.h"

#include "ROS2DMIController.h"
#include "UDPDMIController.h"
#include "ProtobufDmiController.h"

using namespace log4cxx;

namespace IS {

	/**
	 * ストリームを削除するオペレータ
	 *
	 * @author	Naoto Komatsu
	 * @date	2024/2/1
	 */

	class DmiOperator : public Operator
	{
	private:
		const string MyName = "Dmi";
		LoggerPtr logger = Logger::getLogger("DmiOperator");
		StringUtil stringUtil;

		string user;
		string streamName = "";
		string dmiName;
		IS::ROS2DMIController ros2dmiCtrl;
		IS::UDPDMIController udpdmiCtrl;
		IS::ProtobufDmiController pbdmiCtrl;

	public:
		DmiOperator(const string &user, unsigned int mngId, const RecvData &data, const string& dmi);
		~DmiOperator();

		// 処理
		virtual bool process(vector<IS::TupleSet>& ts);
		//virtual bool process_close();
	};
}
#endif  // DMIOPERATOR_H
