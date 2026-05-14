#ifndef INFORMATIONSOURCE_H
#define INFORMATIONSOURCE_H

// InformationSourceクラス
// streamspinnerのInformationSource + Wrapperに相当する

#include "ArrivalTupleNotifier.h"


using namespace std;

namespace IS {

    /**
     * 情報源を扱うクラス
     *
     * @author	Nagoya University
     * @date	2018/03/13
     */

    class InformationSource : public ArrivalTupleNotifier
    {
    private:
        string sourcename;

    public:
        InformationSource(string name);
        virtual ~InformationSource(){}; 

        virtual void init() =0;       // abst
        virtual void start() =0;      // abst
        virtual void stop() =0;       // abst

        // getter
        string getName();

    };
}
#endif  // INFORMATIONSOURCE_H