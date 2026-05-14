#ifndef RDBACCESSOPERATOR_H
#define RDBACCESSOPERATOR_H

#include <log4cxx/logger.h>
#include <unordered_map>
#include <pqxx/pqxx>

#include "Operator.h"
#include "is/StringUtil.h"
#include "InformationSourceManager.h"
#include "QueryManager.h"
#include "QueueManager.h"
#include "Parser.h"

using namespace log4cxx;
using namespace pqxx;

namespace IS {

	/**
	 * RDBから静的情報取得を扱うオペレータ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	class RDBAccessOperator : public Operator
	{
	private:
		const string MyName = "RDBAccess";
		LoggerPtr logger = Logger::getLogger("RDBAccessOperator");
		StringUtil stringUtil;
		IS::Settings &settings = IS::Settings::get_instance();
		IS::InformationSourceManager &ism = IS::InformationSourceManager::get_instance();

		IS::TupleSet retTupleSet;
		bool existGeoData = false;
		bool isUpdate = true;
		bool isCache = false;
		bool isAsterisk = false;
		long cachedTime = 0;
		unsigned int mngId = 0;
		string masterName = "";
		string editQuery = "";
		bool isEditSuccess = false;
		unordered_map<string, string> renameMap;	// key:originName, value:reName
		IS::Schema outputSchema;
		vector<string> tokenList;
		vector<string> relationTableList;
		bool isExitListen = false;
		bool isCreateAffectedRow = false;
        bool isCreateResult = false;
		
		string getTypeString(int num);
		void createListenThread();
		void getST_AsEWKT_forEachLine(work &T, const result &R, IS::TupleSet &ts);
		void setResponseTupleSet(const string columnName , const string returnValue);

	public:
		RDBAccessOperator(unsigned int mngId, string param, string masterName);
		virtual ~RDBAccessOperator();

		void notifyFromRDB(string table);

		// 処理
		virtual bool process(vector<IS::TupleSet>& ts);
		virtual void requestData(long referenceTime);
	};
}
#endif  // RDBACCESSOPERATOR_H