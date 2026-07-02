#ifndef TUPLE_H
#define TUPLE_H
#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>
#include <string>
#include <typeinfo>
#include <cxxabi.h>         // for Debug(Demangle)
//#include <log4cxx/logger.h>

#include "is/Schema.h"

#if __cplusplus < 201703L || defined(USE_EXP_ANY)
	#include <experimental/any>
    using std::experimental::any;      // any
#else
	#include <any>
#endif

using std::string;                 // string
//using namespace log4cxx;

namespace IS {

    /**
     * 行情報を扱うクラス
     *
     * @author	Nagoya University
     * @date	2018/03/14
     */

    class Tuple{
    private:

        std::vector<any> values;
        std::vector<long> timestamps;
		std::vector<bool> isNull;
        long recvTime = 0;
        std::string tableName = "";
        double cost;

    public:
        Tuple();
        Tuple(int size);
		Tuple(const Schema &schema);
        Tuple(Tuple t1, Tuple t2);  // join?
		~Tuple();

        int size() const;

        // value
		bool setValue(const int index, const any &val, const long timestamp);
		bool setValue(const int index, const any &val, const long timestamp, const bool isnull);
		void setRecvTime(const long time);
		void setCost(const double _cost);

		bool updateValueByIdx(const unsigned int idx, const any &val);
		void updateTimestampByIdx(int idx, long timestamp);
        void updateValueIf(int idx, const any &caseVal, const any &updateVal);
		
        bool getValue(const int index, any& val, long& timestamp) const;
		bool getValue(const int index, any& val, long& timestamp, bool& isnull);
		bool getValueByIdx(const unsigned int idx, any& val) const;
		int getIntValueByIdx(const unsigned int idx) const;
		string getStringValueByIdx(const unsigned int idx) const;
		bool getTimestampByIdx(const unsigned int idx, long& time) const;
		long getTimestamp() const;
		void clearValue();
		long getRecvTime();
		double getCost() const;

		void resize(int size);
		void resize(const Schema &schema);
		void swap(const unsigned int fromIdx, const unsigned int toIdx);
		bool checkTimestamp(const long time, const Schema &schema);
		long getNewestTimestamp() const;
		string getLogTimestamp() const;
		bool deleteValueByIdx(const int idx);
        
		void setTableName(const string &_tableName);
		string getTableName();
        // source, time
        // void setTimestamp(string source, long time);
        // void setTimestamps(std::vector<string> sources, std::vector<long> times);
        // long getTimestamp(string source);
        // 

        Tuple getSubset(const std::vector<int> &selectColmns);

        void dump();        // for Debug
		void dumpAny(any val);        // for Debug
		string getDumpAny(any val);        // for Debug

    private:
        bool checkIndex(int index) const;
    };
}
#endif // TUPLE_H