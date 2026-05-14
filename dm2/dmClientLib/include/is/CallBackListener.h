#ifndef CALLBACKLISTENER_H
#define CALLBACKLISTENER_H

#include <chrono>
#include "is/ResultSet.h"

using namespace std;

/**
* CallBackListener
*
* @author	Nagoya University
* @date	2019/09/20
*/

class CallBackListener {
public:
	virtual ~CallBackListener() {}
	virtual void callBack(ResultSet rs) = 0;
};

#endif //CALLBACKLISTENER_H