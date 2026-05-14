/*
 * CPPファイル GridMap.cpp
 *
 * written by: watanabe
 * date: 2018.2.28
 */
#include "GridMap.h"


/* 依存ライブラリ */
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <stdio.h>
#include <math.h>

using namespace std;
using namespace xercesc;


/* GEOSの警告出力用関数 */
void notice_function(const char *message, ...)
{
	printf("%s", message);
}

/* GEOSのエラー出力用関数 */
void error_function(const char *message, ...)
{
	printf("%s", message);
}


/*
 * KMLファイルの<coordinates>タグ解析用の関数．
 *
 * 解析に失敗した場合は nullptr を返す．
 */
vector<vector<double>*>* analyzeCoordinates(DOMNode *coordinates)
{
	string tag = XMLString::transcode(coordinates->getNodeName());
	if(tag != "coordinates")
		return nullptr;

	string text = XMLString::transcode(coordinates->getTextContent());

	vector<vector<double>*> *dlist = new vector<vector<double>*>();
	vector<double> *point = new vector<double>();
	int count = 0;

	int current = 0;
	int pos = text.find(",", current);

	while(pos != text.npos){
		string substring = text.substr(current, pos - current);

		double value = 0.0;
		
		try {
			value = stod(substring);
			if(isnan(value) || isinf(value))
				return nullptr;
		} catch(invalid_argument ia){
			break;
		}

		point->push_back(value);

		count++;
		if(count % 3 == 0){
			dlist->push_back(point);
			point = new vector<double>();
		}

		current = pos + 1;
		pos = text.find(",", current);

		if(pos == text.npos || pos > text.find("\n", current)){
			pos = text.find("\n", current);
			if(pos == text.npos && current < text.size())
				pos = text.size();
		}
	}
	return dlist;
}


/*
 * KMLファイルの<LinearRing>タグ解析用の関数．
 */
GEOSGeometry* analyzeLinearRing(DOMNode *linearring)
{
	string tag = XMLString::transcode(linearring->getNodeName());
	if(tag != "LinearRing")
		return nullptr;

	DOMNodeList *children = linearring->getChildNodes();
	for(int n=0; n < children->getLength(); n++){
		DOMNode *child = children->item(n);

		string childTag = XMLString::transcode(child->getNodeName());
		if(childTag == "coordinates"){
			vector<vector<double>*> *dlist = analyzeCoordinates(child);
			if(dlist == nullptr || dlist->size() == 0)
				return nullptr;

			dlist->push_back(dlist->front());
			GEOSCoordSeq seq = GEOSCoordSeq_create(dlist->size(), dlist->front()->size());

			for(int i=0; i < dlist->size(); i++){
				vector<double> *point = dlist->at(i);
				for(int j=0; j < point->size(); j++){
					if(isinf(point->at(j)) || isnan(point->at(j)))
						return nullptr;

					if(j == 0)
						GEOSCoordSeq_setX(seq, i, point->at(j));
					else if(j == 1)
						GEOSCoordSeq_setY(seq, i, point->at(j));
					else if(j == 2)
						GEOSCoordSeq_setZ(seq, i, point->at(j));
				}
			}
			GEOSGeometry *shell = GEOSGeom_createLinearRing(seq);
			delete dlist;
			return shell;
		}
	}
	return nullptr;
}


/*
 * KMLファイルの<outerBoundaryIs>タグ解析用の関数．
 */
GEOSGeometry* analyzeOuterBoundaryIs(DOMNode *outerboundary)
{
	string tag = XMLString::transcode(outerboundary->getNodeName());
	if(tag != "outerBoundaryIs")
		return nullptr;

	DOMNodeList *children = outerboundary->getChildNodes();
	for(int i=0; i < children->getLength(); i++){
		DOMNode *child = children->item(i);

		string childTag = XMLString::transcode(child->getNodeName());
		if(childTag == "LinearRing"){
			return analyzeLinearRing(child);
		}
	}
	return nullptr;
}


/*
 * KMLファイルの<Polygon>タグ解析用の関数．
 */
GEOSGeometry* analyzePolygon(DOMNode *polygon)
{
	string tag = XMLString::transcode(polygon->getNodeName());
	if(tag != "Polygon")
		return nullptr;

	DOMNodeList *children = polygon->getChildNodes();
	for(int i=0; i < children->getLength(); i++){
		DOMNode *child = children->item(i);
		if(child == nullptr)
			continue;
		string childTag = XMLString::transcode(child->getNodeName());

		if(childTag == "outerBoundaryIs"){
			GEOSGeometry *shell = analyzeOuterBoundaryIs(child);
			if(shell == nullptr)
				return nullptr;

			GEOSGeometry *hole[0];
			GEOSGeometry *g = GEOSGeom_createPolygon(shell, hole, 0);

			return g;
		}
	}
	return nullptr;
}


/*
 * KMLファイルの<Point>タグ解析用の関数．
 */
GEOSGeometry* analyzePoint(DOMNode *point)
{
	string tag = XMLString::transcode(point->getNodeName());
	if(tag != "Point")
		return nullptr;

	DOMNodeList *children = point->getChildNodes();
	for(int n=0; n < children->getLength(); n++){
		DOMNode *child = children->item(n);
		if(child == nullptr)
			continue;
		string childTag = XMLString::transcode(child->getNodeName());

		if(childTag == "coordinates"){
			vector<vector<double>*> *dlist = analyzeCoordinates(child);
			if(dlist == nullptr || dlist->size() == 0)
				return nullptr;

			GEOSCoordSeq seq = GEOSCoordSeq_create(dlist->size(), dlist->front()->size());

			for(int i=0; i < dlist->size(); i++){
				vector<double> *point = dlist->at(i);
				for(int j=0; j < point->size(); j++){
					if(isinf(point->at(j)) || isnan(point->at(j)))
						return nullptr;

					if(j == 0)
						GEOSCoordSeq_setX(seq, i, point->at(j));
					else if(j == 1)
						GEOSCoordSeq_setY(seq, i, point->at(j));
					else if(j == 2)
						GEOSCoordSeq_setZ(seq, i, point->at(j));
				}
			}
			GEOSGeometry *point = GEOSGeom_createPoint(seq);
			delete dlist;
			return point;
		}
	}
	return nullptr;
}


/*
 * KMLファイルの<LineStyle>タグ解析用の関数．
 */
State analyzeLineStyle(DOMNode *linestyle)
{
	string tag = XMLString::transcode(linestyle->getNodeName());
	if(tag != "LineStyle")
		return UNKNOWN;

	DOMNodeList *children = linestyle->getChildNodes();
	State rval = UNKNOWN;

	for(int i=0; i < children->getLength(); i++){
		DOMNode *child = children->item(i);
		if(child == nullptr)
			continue;
		string childTag = XMLString::transcode(child->getNodeName());

		if(childTag == "color"){
			string color = string(XMLString::transcode(child->getTextContent()));
			if(color == "b20000ff")
				rval = OBJECT;
			else if(color == "b200ffff")
				rval = EMPTY;
			else if(color == "b20099ff")	//20180607
				rval = TEMP_OBJECT;	//20180607
			else if(color == "ff000000")
				rval = CAMERA;

		}
	}
	return rval;
}


/*
 * KMLファイルの<PolyStyle>タグ解析用の関数．
 */
State analyzePolyStyle(DOMNode *polystyle)
{
	string tag = XMLString::transcode(polystyle->getNodeName());
	if(tag != "PolyStyle")
		return UNKNOWN;

	DOMNodeList *children = polystyle->getChildNodes();
	State rval = UNKNOWN;

	for(int i=0; i < children->getLength(); i++){
		DOMNode *child = children->item(i);
		if(child == nullptr)
			continue;
		string childTag = XMLString::transcode(child->getNodeName());

		if(childTag == "color"){
			string color = string(XMLString::transcode(child->getTextContent()));
			if(color == "b20000ff")
				rval = OBJECT;
			else if(color == "b200ffff")
				rval = EMPTY;
			else if(color == "b20099ff")	//20180607
				rval = TEMP_OBJECT;	//20180607
		}
	}
	return rval;
}


/*
 * KMLファイルの<Style>タグ解析用の関数．
 */
State analyzeStyle(DOMNode *style)
{
	string tag = XMLString::transcode(style->getNodeName());
	if(tag != "Style")
		return UNKNOWN;

	DOMNodeList *children = style->getChildNodes();
	State rval = UNKNOWN;

	for(int i=0; i < children->getLength(); i++){
		DOMNode *child = children->item(i);
		if(child == nullptr)
			continue;
		string childTag = XMLString::transcode(child->getNodeName());

		if(childTag == "LineStyle"){
			rval = analyzeLineStyle(child);
		}
		else if(childTag == "PolyStyle"){
			rval = analyzePolyStyle(child);
		}
	}
	return rval;
}



/*
 * KMLファイルの<Placemark>タグ解析用の関数．
 */
vector<ObjectInfo*>* analyzePlacemarks(DOMNodeList *plist)
{
	vector<ObjectInfo*>* olist = new vector<ObjectInfo*>();

	for(int i=0; i < plist->getLength(); i++){
		DOMNode *placemark = plist->item(i);
		if(placemark == nullptr)
			continue;

		ObjectInfo *o = (ObjectInfo*)malloc(sizeof(ObjectInfo));
		o->name = NULL;
		o->type = UNKNOWN;
		o->geom = nullptr;


		DOMNodeList *children = placemark->getChildNodes();
		if(children == nullptr)
			continue;
		for(int j=0; j < children->getLength(); j++){
			DOMNode *child = children->item(j);
			if(child == nullptr)
				continue;
			string childTag = XMLString::transcode(child->getNodeName());

			// <name> tag
			if(childTag == "name"){
				o->name = XMLString::transcode(child->getTextContent());
			}

			// <Style> tag
			else if(childTag == "Style"){
				o->type = analyzeStyle(child);
			}

			// <Polygon> tag
			else if(childTag == "Polygon"){
				o->geom = analyzePolygon(child);
			}

			// <Point> tag
			/*
			else if(childTag == "Point"){
				o->geom = analyzePoint(child);
				o->type = CAMERA;
			}
			*/
		}

		if(o->geom != nullptr)
			olist->push_back(o);
		else
			free(o);
	}
	return olist;
}


/*
 * KMLファイルを解析して，ObjectInfoのリストを生成する関数．
 */
vector<ObjectInfo*>* parseKML(string filename) 
{
    XercesDOMParser *parser = new XercesDOMParser();
	vector<ObjectInfo*> *olist = nullptr;

    try {
        parser->parse(filename.c_str());

		DOMDocument *d = parser->getDocument();
		DOMNodeList *plist = d->getElementsByTagName(XMLString::transcode("Placemark"));

		olist = analyzePlacemarks(plist);
		
    } catch(const XMLException& x){
        char *message = XMLString::transcode(x.getMessage());
        cout << message <<endl;
        XMLString::release(&message);
    } catch(const DOMException& d){
        char *message = XMLString::transcode(d.getMessage());
        cout << message <<endl;
        XMLString::release(&message);
    } catch(...) {
        cout << "Unexpected exception" <<endl;
    }
    delete parser;
    return olist;
}


/*
 * ObjectInfoの廃棄処理のための関数．
 */
void clearObjectInfoList(vector<ObjectInfo*> *olist)
{
	for(ObjectInfo *o : *olist){
		GEOSGeom_destroy(o->geom);
		XMLString::release(&(o->name));

		free(o);
	}
	olist->clear();
	delete olist;
}



/*
 * KMLファイルからのオブジェクトの占有領域と，グリッドマップの各セルの重複関係を調べる関数．
 */
void computeOverlap(GridMap *g, vector<ObjectInfo*> *olist)
{
	for(int i=0; i < g->numRows; i++){
		for(int j=0; j < g->numCols; j++){
			for(int k=0; k < olist->size(); k++){
				ObjectInfo *o = olist->at(k);

				if(o->type == OBJECT){
					if(GEOSIntersects(o->geom, g->cells[i][j].area) )
						g->cells[i][j].state = OBJECT;
				}
				else if(o->type == EMPTY){
					if(g->cells[i][j].state == UNKNOWN && GEOSCovers(o->geom, g->cells[i][j].area) )
						g->cells[i][j].state = EMPTY;
				}
				else if(o->type == TEMP_OBJECT){								//20180607
					if(g->cells[i][j].state == UNKNOWN && GEOSCovers(o->geom, g->cells[i][j].area) )	//20180607
						g->cells[i][j].state = TEMP_OBJECT;						//20180607
				}												//20180607
				else if(o->type == CAMERA){
					if(g->cells[i][j].state == UNKNOWN && GEOSCovers(o->geom, g->cells[i][j].area) )
						g->cells[i][j].state = CAMERA;
				}

			}
		}
	}
}


/*
 * 関数 initialize()
 *
 * 依存ライブラリの初期化処理を実行する必要があるため，
 * ライブラリを使用するときに最初の1回だけ呼び出す必要がある．
 */
void initialize()
{
    try {
        XMLPlatformUtils::Initialize();
    } catch(const XMLException& e){
        char *message = XMLString::transcode(e.getMessage());
        cout << message << endl;
        XMLString::release(&message);
        return;
    }

	//initGEOS(notice_function, error_function);
}


/*
 * 関数 finish()
 *
 * 依存ライブラリの終了処理を実行する必要があるため，
 * プログラムを終了する前に最後に1回だけ呼び出す必要がある．
 */
void finish()
{
    XMLPlatformUtils::Terminate();

	//finishGEOS();
}


/*
 * 関数 createGridMap()
 *
 * グリッドマップの初期化処理のための関数．指定した範囲を指定した個数に分割したグリッドを生成する．
 *
 * points: グリッドマップのエリア全体を表す4角形の各点の3次元座標
 * numRows: 縦方向のセル総数
 * numCols: 横方向のセル総数
 */
GridMap* createGridMap(double points[4][3], int numRows, int numCols)
{
	GridMap* g = new GridMap();
	g->numRows = numRows;
	g->numCols = numCols;
	g->cells = new Cell*[numRows];

	double minX = min(min(min(points[0][0], points[1][0]), points[2][0]), points[3][0]);
	double maxX = max(max(max(points[0][0], points[1][0]), points[2][0]), points[3][0]);
	double minY = min(min(min(points[0][1], points[1][1]), points[2][1]), points[3][1]);
	double maxY = max(max(max(points[0][1], points[1][1]), points[2][1]), points[3][1]);
	double minZ = min(min(min(points[0][2], points[1][2]), points[2][2]), points[3][2]);
	double maxZ = max(max(max(points[0][2], points[1][2]), points[2][2]), points[3][2]);

	double deltaX = (maxX - minX) / (double)numCols;
	double deltaY = (maxY - minY) / (double)numRows;

	for(int i=0; i < numRows; i++){
		g->cells[i] = new Cell[numCols];
		for(int j=0; j < numCols; j++){
			g->cells[i][j].state = UNKNOWN;

			GEOSCoordSequence *seq = GEOSCoordSeq_create(5, 3);

			GEOSCoordSeq_setX(seq, 0, minX + deltaX  * j);
			GEOSCoordSeq_setY(seq, 0, maxY - deltaY * i);
			GEOSCoordSeq_setZ(seq, 0, minZ);

			GEOSCoordSeq_setX(seq, 1, minX + deltaX  * (j+1));
			GEOSCoordSeq_setY(seq, 1, maxY - deltaY * i);
			GEOSCoordSeq_setZ(seq, 1, minZ);

			GEOSCoordSeq_setX(seq, 2, minX + deltaX  * (j+1));
			GEOSCoordSeq_setY(seq, 2, maxY - deltaY * (i+1));
			GEOSCoordSeq_setZ(seq, 2, minZ);

			GEOSCoordSeq_setX(seq, 3, minX + deltaX  * j);
			GEOSCoordSeq_setY(seq, 3, maxY - deltaY * (i+1));
			GEOSCoordSeq_setZ(seq, 3, minZ);

			GEOSCoordSeq_setX(seq, 4, minX + deltaX  * j);
			GEOSCoordSeq_setY(seq, 4, maxY - deltaY * i);
			GEOSCoordSeq_setZ(seq, 4, minZ);

			GEOSGeometry *shell = GEOSGeom_createLinearRing(seq);
			GEOSGeometry *hole[0];
			g->cells[i][j].area = GEOSGeom_createPolygon(shell, hole, 0);
		}
	}
	return g;
}



/*
 * 関数 getGridMap()
 *
 * KMLファイルからオブジェクトの占有領域を読みだして，グリッドマップ形式に変換する関数．
 * KMLファイルが複数ある場合は，その数分だけのグリッドマップへ変換する．
 * この関数ではグリッドマップ同士の情報のフュージョンは行わない．
 * 
 * points: グリッドマップのエリア全体を表す4角形の各点の3次元座標
 * files: KML形式のファイル名(複数)
 * numFiles: KML形式のファイルの数
 */
GridMap* getGridMap(double points[4][3], char *files[], int numFiles)
{
	vector<GridMap*> *glist = new vector<GridMap*>();

	int numRows = DEFAULT_NUM_ROWS;
	int numCols = DEFAULT_NUM_COLUMNS;

	for(int i=0; i < numFiles; i++){
		GridMap *g = createGridMap(points, numRows, numCols);

		string filename(files[i]);

		vector<ObjectInfo*> *olist = parseKML(filename);

		computeOverlap(g, olist);

		clearObjectInfoList(olist);

		glist->push_back(g);
	}

	GridMap *garray = new GridMap[glist->size()];
	for(int i=0; i < glist->size(); i++)
		garray[i] = *(glist->at(i));

	return garray;
}


/*
 * 関数 printGridMap()
 *
 * 標準出力へGridMapをテキスト表示する(デバッグ確認用)．使用するキャラクタは以下の通り:
 * Unknown Object -> 'U'，
 * Empty Object   -> 'E'，
 * 実在 Object    -> 'O'，
 * カメラ設置位置 -> 'C'，
 */
void printGridMap(GridMap g)
{
	for(int i=0; i < g.numRows; i++){
		for(int j=0; j < g.numCols; j++){
			if(g.cells[i][j].state == EMPTY)
				printf("E");
			else if(g.cells[i][j].state == OBJECT)
				printf("O");
			else if(g.cells[i][j].state == TEMP_OBJECT)	//20180607
				printf("T");				//20180607
			else if(g.cells[i][j].state == CAMERA)
				printf("C");
			else
				printf("U");
		}
		printf("\n");
	}
}


/*
 * 関数 writeToKMLFile()
 *
 * グリッドマップを指定されたファイルへKML形式で出力する．
 * skipUnknownの値でUnknownオブジェクトの出力を抑制するかを選べる(trueなら出力しない)
 */
void writeToKMLFile(char *filename, GridMap g, bool skipUnknown)
{
	FILE* fp;

	fp = fopen(filename, "w");
	if( fp == NULL){
		printf("Cannot open file: %s\n", filename);
		exit(0);
	}

	fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(fp, "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n");
	fprintf(fp, "<Document>\n");

	for(int i=0; i < g.numRows; i++){
		for(int j=0; j < g.numCols; j++){
			if(g.cells[i][j].state == UNKNOWN && skipUnknown == true)
				continue;

			fprintf(fp, "<Placemark id=\"%d\">\n", i*g.numRows + j);

			fprintf(fp, "\t<name>%d</name>\n", i*g.numRows + j);

			fprintf(fp, "\t<Style>\n");

			fprintf(fp, "\t\t<LineStyle>\n");
			if(g.cells[i][j].state == OBJECT)
				fprintf(fp, "\t\t\t<color>b20000ff</color>\n");
			else if(g.cells[i][j].state == EMPTY)
				fprintf(fp, "\t\t\t<color>b200ffff</color>\n");
			else if(g.cells[i][j].state == TEMP_OBJECT)			//20180607
				fprintf(fp, "\t\t\t<color>b20099ff</color>\n");		//20180607
			else if(g.cells[i][j].state == CAMERA)
				fprintf(fp, "\t\t\t<color>ff000000</color>\n");
			else 
				fprintf(fp, "\t\t\t<color>a0000000</color>\n");
			fprintf(fp, "\t\t</LineStyle>\n");

			fprintf(fp, "\t\t<PolyStyle>\n");
			if(g.cells[i][j].state == OBJECT)
				fprintf(fp, "\t\t\t<color>b20000ff</color>\n");
			else if(g.cells[i][j].state == EMPTY)
				fprintf(fp, "\t\t\t<color>b200ffff</color>\n");
			else if(g.cells[i][j].state == TEMP_OBJECT)			//20180607
				fprintf(fp, "\t\t\t<color>b20099ff</color>\n");		//20180607
			else if(g.cells[i][j].state == CAMERA)
				fprintf(fp, "\t\t\t<color>ff000000</color>\n");
			else 
				fprintf(fp, "\t\t\t<color>a0000000</color>\n");
			fprintf(fp, "\t\t</PolyStyle>\n");

			fprintf(fp, "\t</Style>\n");

			fprintf(fp, "\t<Polygon>\n");

			if(g.cells[i][j].state == CAMERA)
				fprintf(fp, "\t\t<extrude>1</extrude>\n");
			else
				fprintf(fp, "\t\t<extrude>0</extrude>\n");

			fprintf(fp, "\t\t<tessellate>1</tessellate>\n");
			fprintf(fp, "\t\t<altitudeMode>relativeToGround</altitudeMode>\n");
			fprintf(fp, "\t\t<outerBoundaryIs>\n");
			fprintf(fp, "\t\t\t<LinearRing>\n");
			fprintf(fp, "\t\t\t\t<coordinates>\n");

			GEOSGeometry *ring = GEOSBoundary(g.cells[i][j].area);
			const GEOSCoordSequence *seq = GEOSGeom_getCoordSeq(ring);
			unsigned int seqSize, seqDim;
			GEOSCoordSeq_getSize(seq, &seqSize);
			GEOSCoordSeq_getDimensions(seq, &seqDim);

			for(int k=0; k < seqSize; k++){
				fprintf(fp, "\t\t\t\t\t");
				for(int l=0; l < seqDim; l++){
					double value = 0.0;
					if(l == 0)
						GEOSCoordSeq_getX(seq, k, &value);
					else if(l == 1)
						GEOSCoordSeq_getY(seq, k, &value);
					else if(l == 2)
						GEOSCoordSeq_getZ(seq, k, &value);

					fprintf(fp, "%.9lf", value);
					if(l+1 < seqDim)
						fprintf(fp, ",");
					else
						fprintf(fp, " \n");
				}
			}

			fprintf(fp, "\t\t\t\t</coordinates>\n");
			fprintf(fp, "\t\t\t</LinearRing>\n");
			fprintf(fp, "\t\t</outerBoundaryIs>\n");

			fprintf(fp, "\t</Polygon>\n");

			fprintf(fp, "</Placemark>\n");
		}
	}

	fprintf(fp, "</Document>\n");
	fprintf(fp, "</kml>\n");
	fclose(fp);
}


/*
 * 関数 clearGridMap()
 *
 * グリッドマップの廃棄処理のための関数．
 */
void clearGridMap(GridMap *g)
{
	for(int i=0; i < g->numRows; i++){
		for(int j=0; j < g->numCols; j++){
			GEOSGeom_destroy(g->cells[i][j].area);
			g->cells[i][j].area = NULL;
		}
		delete g->cells[i];
		g->cells[i] = NULL;
	}
	delete g->cells;
	g->cells = NULL;
}

//************* ADD START *************//
/*
* input文字列からLinearRingを取得する関数．
*/
GEOSGeometry* analyzeLinearRing(string input)
{
	//cout << "[analyzeLinearRing] IN param = " << input << endl;
	vector<string> pointList, valList;
	IS::StringUtil stringUtil;
	stringUtil.split(input, ",", pointList);
	GEOSCoordSeq seq = GEOSCoordSeq_create(pointList.size(), 3);

	for (int i = 0; i < pointList.size(); i++) {
		//cout << "[analyzeLinearRing] pointList[" << std::to_string(i) << "] = " << pointList.at(i) << endl;
		stringUtil.replaceAll(pointList.at(i), "  ", " ");
		stringUtil.split(pointList.at(i), " ", valList);
		//cout << "[analyzeLinearRing] setX = " << valList.at(0) << endl;
		GEOSCoordSeq_setX(seq, i, std::stod(valList.at(0)));
		//cout << "[analyzeLinearRing] setY = " << valList.at(1) << endl;
		GEOSCoordSeq_setY(seq, i, std::stod(valList.at(1)));
		//cout << "[analyzeLinearRing] setZ = " << valList.at(2) << endl;
		GEOSCoordSeq_setZ(seq, i, std::stod(valList.at(2)));
	}
	GEOSGeometry *shell = GEOSGeom_createLinearRing(seq);
	//cout << "[analyzeLinearRing] OUT" << endl;
	return shell;
}

/*
* input文字列からPolygonを取得する関数．
*/
GEOSGeometry* analyzePolygon(string input)
{
	//cout << "[analyzePolygon] IN param = " << input << endl;
	vector<string> value;
	IS::StringUtil stringUtil;
	stringUtil.getStringInBracket(input, value);
	GEOSGeometry *shell = analyzeLinearRing(value.at(0));
	if (shell == nullptr)
		return nullptr;

	GEOSGeometry *hole[0];
	GEOSGeometry *g = GEOSGeom_createPolygon(shell, hole, 0);

	return g;
}

/*
* input文字列から領域のState情報を取得する関数．
*/
State analyzePolyState(string input)
{
	//cout << "[analyzePolyState] IN param =" << input << endl;
	IS::StringUtil stringUtil;
	State rval = UNKNOWN;

	vector<string> delStr = { "[","]" };
	vector<double> ret;
	input = stringUtil.getDeleteStrings(input, delStr);
	try {
		for (string val : stringUtil.split(input, ",")) {
			//cout << "[analyzePolyState] val = " << val << endl;
			ret.push_back(std::stod(val));
		}
	}
	catch (std::invalid_argument) {
		throw;
	}

	return (ret[0]>ret[1]? (ret[0]>ret[2]? OBJECT:UNKNOWN) : (ret[1]>ret[2]? EMPTY:UNKNOWN));
}

/*
* EvalOperatorのユーザ定義関数にて渡された2次元配列解析用の関数．
*/
vector<ObjectInfo*>* analyzeVectorInfo(vector<vector<string>> rows)
{
	vector<ObjectInfo*>* olist = new vector<ObjectInfo*>();

	for (vector<string> row : rows) {

		ObjectInfo *o = (ObjectInfo*)malloc(sizeof(ObjectInfo));
		// 名称の設定
		char* cstr = new char[row.at(1).size() + 1];
		std::char_traits<char>::copy(cstr, row.at(1).c_str(), row.at(1).size() + 1);
		o->name = cstr;

		// 座標情報の設定
		o->geom = nullptr;
		o->geom = analyzePolygon(row.at(2));

		// 領域情報の設定
		o->type = UNKNOWN;
		o->type = analyzePolyState(row.at(3));
		//printf("o->type = %d\n", o->type);

		if (o->geom != nullptr)
			olist->push_back(o);
		else
			free(o);
	}
	return olist;
}

/*
* 関数 createAreaPoints()
*
* グリッドマップの初期化処理のための関数．指定した基準点から指定した範囲で領域を生成する．
* 縦横のGrid数と四方領域の4点座標配列を返す
*
* point: エリアの中心点を表す3次元座標
* x_m: 横方向の長さ(メートル) 
* y_m: 縦方向の長さ(メートル) 
* gridNumX: [in,out] X方向のGrid数
* gridNumY: [in,out] Y方向のGrid数
* points: [in,out] 基準点を基準に指定された長さで生成された四方領域の4点座標配列
*/
void createAreaPoints(double point[1][3], double x_m, double y_m, int &gridNumX, int &gridNumY, double points[4][3])
{
	// 既存実装の数値から算出(センサの精度の関係上1グリッド当たり30cm四方を想定)
	double longitude30cm = 0.0000030517578125;	// 経度
	double latitude30cm  = 0.000002001953125;	// 緯度

	gridNumX = x_m / 0.3;
	gridNumY = y_m / 0.3;

	double maxX = point[0][0] + (double)((double)gridNumX * longitude30cm) / 2.0;
	double minX = point[0][0] - (double)((double)gridNumX * longitude30cm) / 2.0;
	double maxY = point[0][1] + (double)((double)gridNumY * latitude30cm) / 2.0;
	double minY = point[0][1] - (double)((double)gridNumY * latitude30cm) / 2.0;

	points[0][0] = minX;
	points[0][1] = maxY;
	points[0][2] = point[0][2];
	points[1][0] = minX;
	points[1][1] = minY;
	points[1][2] = point[0][2];
	points[2][0] = maxX;
	points[2][1] = maxY;
	points[2][2] = point[0][2];
	points[3][0] = maxX;
	points[3][1] = minY;
	points[3][2] = point[0][2];
	
	//cout << "POLYGON((";
	//cout << minX << " " << maxY << " " << point[0][2] << ",";
	//cout << minX << " " << minY << " " << point[0][2] << ",";
	//cout << maxX << " " << minY << " " << point[0][2] << ",";
	//cout << maxX << " " << maxY << " " << point[0][2] << ",";
	//cout << minX << " " << maxY << " " << point[0][2] << "))" << endl;

	return;
}

/*
* 関数 getGridMap()
*
* 入力した2次元配列からオブジェクトの占有領域を読みだして，グリッドマップ形式に変換する関数．
* 観測者が複数いる場合は，その数分だけのグリッドマップへ変換する．
* この関数ではグリッドマップ同士の情報のフュージョンは行わない．
*
* points: グリッドマップのエリア全体を表す4角形の各点の3次元座標
* paramList: ISから渡された2次元配列(paramList[行数][カラム]
*            カラム 0:観測者ID
*                   1:観測されたオブジェクトのローカルID
*                   2:座標情報 (Ex. POLYGON(...))
*                   3:領域ステータス (Ex. [0.8, 0.2, 0]) 左からobject, empty, unknownの存在確立を表す
* num: [in,out] 観測者数
* rowsNum: X方向のGrid数
* colsNum: Y方向のGrid数
*/
GridMap* getGridMap(double points[4][3], vector<vector<string>> paramList, int &num, int rowsNum, int colsNum)
{
	//cout << "[getGridMap] IN" << endl;
	vector<GridMap*> *glist = new vector<GridMap*>();

	int numRows = rowsNum;
	int numCols = colsNum;

	// 観測者毎にデータを分ける
	map<string, vector<vector<string>>> observedMap;
	for (vector<string> row : paramList) {
		auto itr = observedMap.find(row.at(0));
		if (itr != observedMap.end()) {
			//既に設定済み
			observedMap[row.at(0)].push_back(row);
		}
		else {
			//未設定
			vector<vector<string>> firstData;
			firstData.push_back(row);
			observedMap[row.at(0)] = firstData;
		}
	}
	num = observedMap.size();

	// 観測者毎にGridMapを生成する
	for(auto itr = observedMap.begin(); itr != observedMap.end(); ++itr) {
		//cout << "[getGridMap] Create GridMap key:" << itr->first << endl;
		GridMap *g = createGridMap(points, numRows, numCols);

		vector<ObjectInfo*> *olist = analyzeVectorInfo(itr->second);

		computeOverlap(g, olist);

		clearObjectInfoList(olist);

		glist->push_back(g);
	}

	GridMap *garray = new GridMap[glist->size()];
	for (int i = 0; i < glist->size(); i++)
		garray[i] = *(glist->at(i));

	//cout << "[getGridMap] OUT" << endl;
	return garray;
}


/*
* 関数 convertGridMapToVector()
*
* グリッドマップの情報を元に隣り合った領域を統合し、2次元配列に変換する
*/
void convertGridMapToVector(GridMap g, vector<vector<string>> &vecInfo)
{
	//cout << "[convertGridMapToVector] IN" << endl;
	vector<Cell*> cellList;
	int id = 0;
	for (int i = 0; i < g.numRows; i++) {
		for (int j = 0; j < g.numCols; j++) {
			if (g.cells[i][j].state == EMPTY || g.cells[i][j].state == OBJECT || g.cells[i][j].state == TEMP_OBJECT) {
				bool isIntegrate = false;
				for (Cell *cell : cellList) {
					// 双方の領域が接しており、かつ以下のいずれかの場合は領域を結合する
					// ・双方が同じ領域ステートの場合
					// ・双方がOBJECTとTEMP_OBJECTの組み合わせの場合
					if (GEOSIntersects(g.cells[i][j].area, cell->area) && 
						( g.cells[i][j].state == cell->state || (g.cells[i][j].state == OBJECT && cell->state == TEMP_OBJECT) || (g.cells[i][j].state == TEMP_OBJECT && cell->state == OBJECT) ) ) {
						//cout << "intersects!!! cells[" << i << "][" << j << "]" << endl;
						//cout << "WKT[" << g.cells[i][j].state << "] : " << std::string(GEOSGeomToWKT(g.cells[i][j].area)) << endl;
						cell->area = GEOSUnion(g.cells[i][j].area, cell->area);
						//cout << "UNION WKT : " << std::string(GEOSGeomToWKT(cell->area)) << endl;
						isIntegrate = true;
					}
				}
				if (!isIntegrate) {
					Cell *cell = new Cell;
					cell->area = g.cells[i][j].area;
					cell->state = g.cells[i][j].state;
					cellList.push_back(cell);
					//cout << "geoList.push_back!!! cells[" << i << "][" << j << "]" << endl;
					//cout << "WKT[" << cell->state << "] : " << std::string(GEOSGeomToWKT(cell->area)) << endl;
				}
			}
		}
	}
	// 検出した領域同士で重複があれば合成する
	for (int i = 0; i < cellList.size(); i++) {
		for (int j = cellList.size() - 1; 0 <= j; j--) {
			if (i == j) continue;
			if (GEOSIntersects(cellList.at(i)->area, cellList.at(j)->area) && cellList.at(i)->state == cellList.at(j)->state) {
				cellList.at(i)->area = GEOSUnion(cellList.at(i)->area, cellList.at(j)->area);
				delete cellList.at(j);
				cellList.erase(cellList.begin() + j);
				//cout << "delete idx :" << j << endl;
			}
		}
	}
	for (Cell *cell : cellList) {
		vector<string> row;
		// ID
		row.push_back(std::to_string(++id));
		//cout << "UNION : " << id << ",";
		// 座標情報
		string wkt = std::string(GEOSGeomToWKT(cell->area));
		row.push_back(wkt);
		//cout << wkt << ",";
		// 領域情報(TODO : 存在確立の計算)
		string state;
		if (cell->state == OBJECT) {
			state = "[1.0,0,0]";
		}
		// TODO : 内部情報として管理していた仮オブジェクトも出力時はOBJECTと見做す
		else if(cell->state == TEMP_OBJECT){
			state = "[1.0,0,0]";
		}
		else if(cell->state == EMPTY){
			state = "[0,1.0,0]";
		}
		row.push_back(state);
		//cout << state << endl;
		vecInfo.push_back(row);
	}
	//cout << "[convertGridMapToVector] END" << endl;
}
//************* ADD  END  *************//
