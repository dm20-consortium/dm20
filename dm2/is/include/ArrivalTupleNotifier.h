#ifndef ARRIVALTUPLENOTIFIER_H
#define ARRIVALTUPLENOTIFIER_H

#include <vector>
#include <log4cxx/logger.h>
#include <is/ArrivalTupleListener.h>
#include <is/TupleSet.h>

using namespace std;
using namespace log4cxx;

namespace IS {

    /**
     * タプル到着時の通知処理クラス
     *
     * @author	Nagoya University
     * @date	2018/03/13
     */

    class ArrivalTupleNotifier {
    
    protected:
        vector<ArrivalTupleListener*> listeners;
		vector<ArrivalTupleListener*> parents;
		vector<ArrivalTupleListener*> returns;

		bool fireEvent(TupleSet& tupleset, const ArrivalTupleListener* callObj);
		void returnEvent(TupleSet& tupleset, const ArrivalTupleListener* callObj);
		bool requestParentProc(const ArrivalTupleListener* callObj, long referenceTime);
	private:
		LoggerPtr logger = Logger::getLogger("ArrivalTupleNotifier");
		pthread_mutex_t listener_mtx;
    public:
        ArrivalTupleNotifier();
        virtual ~ArrivalTupleNotifier();

        void addListener(ArrivalTupleListener* listener);
        void removeListener(ArrivalTupleListener* listener);

		void addParent(ArrivalTupleListener* listener);
		void addReturn(ArrivalTupleListener* listener);

    };
}
#endif  // ARRIVALTUPLENOTIFIER_H