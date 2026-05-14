#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <is/ResultSet.h>
#include <is/StringUtil.h>
#include <is/DmException.h>

#include <geos/geom/Geometry.h>
#include <geos/geom/Coordinate.h>
#include <geos/algorithm/Angle.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Point.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Polygon.h>

using namespace std;
using namespace geos;
using namespace geos::geom;

IS::StringUtil stringUtil;

Point* createPoint(const string &input)
{
	PrecisionModel *pm = new PrecisionModel(2.0, 0, 0);
#if GEOS_VER == 36
        GeometryFactory::unique_ptr global_factory = GeometryFactory::create(pm, -1);
#else
        GeometryFactory::Ptr global_factory = GeometryFactory::create(pm, -1);
#endif
	delete pm;
	if (!stringUtil.contain(input, "POINT"))
	{
		throw CastException();
	}
	Point *p = NULL;
	vector<string> value, valList;
	stringUtil.getStringInBracket(input, value);
	value.at(0) = stringUtil.trim(value.at(0));
	stringUtil.split(value.at(0), " ", valList);
	if (valList.size() == 2) {
		Coordinate c(std::stod(valList[0]), std::stod(valList[1]));
#if GEOS_VER == 312
  	p = global_factory->createPoint(c).release();
#else
  	p = global_factory->createPoint(c);
#endif
	}
	else if (valList.size() == 3) {
		Coordinate c(std::stod(valList[0]), std::stod(valList[1]), std::stod(valList[2]));
#if GEOS_VER == 312
  	p = global_factory->createPoint(c).release();
#else
  	p = global_factory->createPoint(c);
#endif
	}
	//delete global_factory;
	int srId = stringUtil.getSRID(input);
	if (srId != 0) p->setSRID(srId);

	return p;
}
/**
* 水平面での方位をラジアン単位で返却する。
* @author    Nagoya University
* @date      2020/03/04
* @param     引数1=geometry, 引数2=geometry
* @return    double
*/
extern "C" vector< vector<string> > UF_AZIMUTH(vector< vector<string> > argList)
{
  //printf("UF_Azimuth called!\n");
  vector< vector<string> > retList; // ISに返却する結果2次元配列(retList[row][col])

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    IS::StringUtil stringUtil;
    vector<string> ret;
    string a1,a2;
    try {
      a1 = argList.at(i).at(0); // 引数1
      a2 = argList.at(i).at(1); // 引数2
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }

    // WKTからSRIDを分離する
    int srId1 = stringUtil.getSRID(a1);
    if (srId1 != 0) a1 = stringUtil.split(a1,";").at(1);
    int srId2 = stringUtil.getSRID(a2);
    if (srId2 != 0) a2 = stringUtil.split(a2,";").at(1);

    // Geometry文字列を大文字に変換
    string geomStUpp1 = a1;
    transform(geomStUpp1.begin(),geomStUpp1.end(),geomStUpp1.begin(),::toupper);
    string geomStUpp2 = a2;
    transform(geomStUpp2.begin(),geomStUpp2.end(),geomStUpp2.begin(),::toupper);

    // Geometryの型チェック
    vector<string> geomNameList;
    geomNameList.push_back(stringUtil.split(geomStUpp1,"(").at(0));
    geomNameList.push_back(stringUtil.split(geomStUpp2,"(").at(0));
    for (int c = 0; c < geomNameList.size(); c++) {
      string geomName = geomNameList.at(c);
      geomName = stringUtil.split(geomName,"(").at(0);
      string::size_type left = geomName.find_first_not_of(" ");
      geomName = stringUtil.split(geomName.substr(left)," ").at(0);
      if (geomName != "POINT") {
        throw CastException("argment must be POINT geometries.");
      }
    }

    // SRIDが同一か確認
    if (srId1 != srId2) {
      throw CastException("Operation on mixed SRID geometries.");
    }

    // GeometryのEMPTY判定
    if ((geomStUpp1.find("EMPTY") != std::string::npos) || (geomStUpp2.find("EMPTY") != std::string::npos)) {
      throw CastException("point offset out of range");
    }

    // EWKT -> Geometry(POINT)
    Geometry* gm1;
    Geometry* gm2;
    try {
      ResultSet rs;
      gm1 = createPoint(a1);
      gm2 = createPoint(a2);
    } catch (...) {
      throw CastException("parse error - invalid geometry.");
    }
    if ((gm1 == NULL) || (gm2 == NULL)) {
      throw CastException("parse error - invalid geometry.");
    }

    // Coordinateを取得
#if GEOS_VER == 312
    const CoordinateXY* cd1 = gm1->getCoordinate();
    const CoordinateXY* cd2 = gm2->getCoordinate();
#else
    const Coordinate* cd1 = gm1->getCoordinate();
    const Coordinate* cd2 = gm2->getCoordinate();
#endif

    // 2つのPOINTがXY同一座標の場合は空文字を返却
    if (cd1->equals2D(*cd2)) {
      ret.push_back("");
      retList.push_back(ret);
      continue;
    }

    // 方位角を計算
    double rad = 0.0;
    rad = geos::algorithm::Angle::angle(*cd1,*cd2);
    if (rad < 0) {
      rad = M_PI + (M_PI - fabs(rad));
    }
    rad = fabs(rad - M_PI * 2);
    rad = fabs(rad + M_PI / 2);
    if (rad >= M_PI * 2) {
      rad -= M_PI * 2;
    }

    // 表示形式に整形
    string retString = to_string(rad);

    ret.push_back(retString);
    retList.push_back(ret);
  }
  //printf("UF_Azimuth end!\n");
  return retList;
}
