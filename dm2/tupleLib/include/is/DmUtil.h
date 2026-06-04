#ifndef DM_UTIL_H
#define DM_UTIL_H

#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <time.h>
#include <unistd.h>
using namespace std;

/**
 * ユーティリティークラス
 *
 * @author	Nagoya University
 * @date	2018/03/15
 */

class DmUtil
{
private:
	static constexpr double EARTH_RADIUS = 6371000.0;
public:
	DmUtil();
	~DmUtil();
	static long getTimeMillisec();
	static long getTimeMicrosec();
	static string getTimeMillisecStr();
	static string getPID();
	static string getStringFromUnsignedLong(unsigned long input);
	static unsigned long getUnsignedLongFromString(const string &input);
	static double haversine(double lat1, double lon1, double lat2, double lon2);
	static double get3DDistance(double lat1, double lon1, double alt1, double lat2, double lon2, double alt2);
};
#endif  // DM_UTIL_H
