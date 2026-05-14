#include "ArrivalTupleNotifier.h"

#include <algorithm>

using namespace std;

namespace IS {

    /**
     * コンストラクタ
     *
     * @author	Nagoya University
     * @date	2018/03/13
     */

    ArrivalTupleNotifier::ArrivalTupleNotifier()
    {
		pthread_mutex_init(&listener_mtx, NULL);
		pthread_mutex_lock(&listener_mtx);
        listeners.clear();
		pthread_mutex_unlock(&listener_mtx);
    }

    /**
     * デストラクタ
     *
     * @author	Nagoya University
     * @date	2018/03/13
     */

    ArrivalTupleNotifier::~ArrivalTupleNotifier()
    {
		pthread_mutex_lock(&listener_mtx);
        listeners.clear();
		pthread_mutex_unlock(&listener_mtx);
		pthread_mutex_destroy(&listener_mtx);
    }

    /**
     * データを渡すリスナーを追加する
     *
     * @author	Nagoya University
     * @date	2018/03/13
     *
     * @param [in,out]	listener
     */

    void ArrivalTupleNotifier::addListener(ArrivalTupleListener* listener)
    {
		pthread_mutex_lock(&listener_mtx);
        listeners.push_back(listener);
		pthread_mutex_unlock(&listener_mtx);
    }

    /**
     * データを渡すリスナーを削除する
     *
     * @author	Nagoya University
     * @date	2018/03/13
     *
     * @param [in,out]	listener
     */

    void ArrivalTupleNotifier::removeListener(ArrivalTupleListener* listener)
    {
		pthread_mutex_lock(&listener_mtx);
        vector<ArrivalTupleListener*>::iterator ite = find(listeners.begin(), listeners.end(), listener);
        if(ite != listeners.end()) 
        {
            listeners.erase(ite);
        }
		pthread_mutex_unlock(&listener_mtx);
    }

	/**
	 * データの渡し元のリスナーを追加する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in,out]	listener
	 */

	void ArrivalTupleNotifier::addParent(ArrivalTupleListener* listener)
	{
		parents.push_back(listener);
	}

	/**
	 * 再帰オペレータにて繰り返し処理へ繋げるためのリスナーを設定する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param [in,out]	listener
	 */

	void ArrivalTupleNotifier::addReturn(ArrivalTupleListener* listener)
	{
		returns.push_back(listener);
	}

    /**
     * リスナーを通じて次のオペレータにデータを渡す
     *
     * @author	Nagoya University
     * @date	2018/03/13
     *
     * @param	tupleset	タプルセット
     * @param	callObj 	呼び元のオブジェクト
     */

    bool ArrivalTupleNotifier::fireEvent(TupleSet &tupleset, const ArrivalTupleListener* callObj)
    {
		tupleset.fromOperator = callObj;
		pthread_mutex_lock(&listener_mtx);
        unsigned int n = listeners.size();
		if (n == 0) {
			//logger->debug("[fireEvent] listeners.size():0 So stop chain.");
			pthread_mutex_unlock(&listener_mtx);
			return false;
		}
        for(unsigned int index = 0; index < n; index++)
        {
			//REL_COMMENT logger->trace(" ArrivalTupleNotifier::fireEvent::receiveTupleSet before");
            listeners[index]->receiveTupleSet(tupleset);
        }
		pthread_mutex_unlock(&listener_mtx);
		return true;
    }

	/**
	 * 再帰オペレータにて繰り返し処理を実施する際に次のオペレータにデータを渡す
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	tupleset	タプルセット
	 * @param	callObj 	呼び元のオブジェクト
	 */

	void ArrivalTupleNotifier::returnEvent(TupleSet& tupleset, const ArrivalTupleListener* callObj)
	{
		tupleset.fromOperator = callObj;

		for (unsigned int index = 0; index < returns.size(); index++)
		{
				returns[index]->receiveTupleSet(tupleset);
		}
	}

	/**
	 * 自身の親オペレータに対してINPUTデータのリクエストを要求する
	 * 引数には呼び元の親オペレータを指定する事で、別の親オペレータに対しての要求を実施する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	callObj	呼び元のオブジェクト
	 * @param	referenceTime	参照時間
	 *
	 * @return	これ以上親オペレータが存在しない場合はtrue,親オペレータが存在する場合はfalseを返す
	 */

	bool ArrivalTupleNotifier::requestParentProc(const ArrivalTupleListener* callObj, long referenceTime)
	{
		unsigned int n = parents.size();
		if (n > 1) {
			for (unsigned int index = 0; index < n; index++)
			{
				if (parents[index] != callObj) {
					//logger->debug("[requestParentProc] Parent Operator requestData call. ParentOPNum:" + std::to_string(n) + " callOPidx:" + std::to_string(index));
					parents[index]->requestData(referenceTime);
					return false;
				}
			}
		}
		else if (n == 1) {
			//logger->debug("[requestParentProc] Parent Operator requestData call.");
			parents[0]->requestData(referenceTime);
			return false;
		}
		else {
			// これ以上親オペレータがいない場合はtrueを返却
			//logger->debug("[requestParentProc] Not Found Parent Operator. First Operator notify_one call.");
			return true;
		}
		return false;

	}
}