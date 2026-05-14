/**
* ヘッダファイル GeoAccessor.h
* @author    Nagoya University
* @date      2020/03/11
*/

#ifndef GEOACCESSOR_H
#define GEOACCESSOR_H

#include <iostream>
#include <vector>
#include <string>
#include <geos_c.h>

using namespace std;

// WKTから指定次元の座標をリストで取得する。
vector<double> GeoAccessor(string wktText, int targetDimension);

#endif
