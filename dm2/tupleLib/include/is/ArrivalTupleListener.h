#ifndef ARRIVALTUPLELISTENER_H
#define ARRIVALTUPLELISTENER_H

#include <chrono>
#include "is/TupleSet.h"

using namespace std;

namespace IS {

    /**
     * 到着タプルをListenするためのインターフェースクラス
     *
     * @author	Nagoya University
     * @date	2018/03/13
     */

    class ArrivalTupleListener {
    public:
        virtual ~ArrivalTupleListener() {}
        virtual void receiveTupleSet(TupleSet &ts) = 0;
		virtual void requestData(long referenceTime) = 0;
    };
}
#endif //ARRIVALTUPLELISTENER