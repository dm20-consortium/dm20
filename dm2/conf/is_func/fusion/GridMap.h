/*
 * ヘッダファイル GridMap.h
 *
 * written by: watanabe
 * date: 2018.2.28
 */

/* 二重include対策 */
#ifndef GRIDMAP_H
#define GRIDMAP_H


/* 依存ライブラリ */
#include <geos_c.h>
//************* ADD START *************//
#include <vector>
#include <map>
#include <is/StringUtil.h>
//************* ADD  END  *************//

/* デフォルトのグリッドマップの縦方向のセル総数 */
#define DEFAULT_NUM_ROWS 512

/* デフォルトのグリッドマップの横方向のセル総数 */
#define DEFAULT_NUM_COLUMNS 512


/*
 * 列挙型 State
 *
 * オブジェクトの種別や，セルの状態を表す定数を定義する
 */
typedef enum {
		UNKNOWN =  0,   // Unknownオブジェクト
		EMPTY   =  1,   // Emptyオブジェクト
		OBJECT  =  2,   // 実在するオブジェクト
		TEMP_OBJECT = 3,// 仮オブジェクト  //20180607
		CAMERA  = 99,   // カメラ設置位置
} State;


/*
 * 構造体 ObjectInfo
 *
 * 各センサから報告された物体情報を格納する．
 * KMLの<Placemark>タグに対応する．
 */
typedef struct {
	char* name;         // オブジェクト名
	State type;         // オブジェクト種別
	GEOSGeometry* geom; // 占有領域(Polygon)
} ObjectInfo;


/*
 * 構造体 Cell
 *
 * 占有グリッドの各セルを表す．
 */
typedef struct {
	GEOSGeometry* area;  // セルが表現する領域(Polygon)
	State state;         // セルの状態
} Cell;


/*
 * 構造体 GridMap
 *
 * 占有グリッド全体を表す．
 */
typedef struct {
	Cell **cells;        // グリッドのセルの2次元配列 cells[numRows][numCols]
	int numRows;         // 縦方向のセル総数
	int numCols;         // 横方向のセル総数
} GridMap;


/*
 * 関数 initialize()
 *
 * 依存ライブラリの初期化処理を実行する必要があるため，
 * ライブラリを使用するときに最初の1回だけ呼び出す必要がある．
 */
void initialize();


/*
 * 関数 finish()
 *
 * 依存ライブラリの終了処理を実行する必要があるため，
 * プログラムを終了する前に最後に1回だけ呼び出す必要がある．
 */
void finish();


/*
 * 関数 createGridMap()
 *
 * グリッドマップの初期化処理のための関数．指定した範囲を指定した個数に分割したグリッドを生成する．
 *
 * points: グリッドマップのエリア全体を表す4角形の各点の3次元座標
 * numRows: 縦方向のセル総数
 * numCols: 横方向のセル総数
 */
GridMap* createGridMap(double points[4][3], int numRows, int numCols);

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
GridMap* getGridMap(double points[4][3], char *files[], int numFiles);


/*
 * 関数 printGridMap()
 *
 * 標準出力へGridMapをテキスト表示する(デバッグ確認用)．使用するキャラクタは以下の通り:
 * Unknown Object -> 'U'，
 * Empty Object   -> 'E'，
 * 実在 Object    -> 'O'，
 * カメラ設置位置 -> 'C'，
 */
void printGridMap(GridMap g);


/*
 * 関数 writeToKMLFile()
 *
 * グリッドマップを指定されたファイルへKML形式で出力する．
 * skipUnknownの値でUnknownオブジェクトの出力を抑制するかを選べる(trueなら出力しない)
 */
void writeToKMLFile(char *filename, GridMap g, bool skipUnknown);


/*
 * 関数 clearGridMap()
 *
 * グリッドマップの廃棄処理のための関数．
 */
void clearGridMap(GridMap *g);

//************* ADD START *************//
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
void createAreaPoints(double point[1][3], double x_m, double y_m, int &gridNumX, int &gridNumY, double points[4][3]);

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
GridMap* getGridMap(double points[4][3], vector<vector<string>> paramList, int &num, int rowsNum, int colsNum);

/*
* 関数 convertGridMapToVector()
*
* グリッドマップの情報を元に隣り合った領域を統合し、2次元配列に変換する
*/
void convertGridMapToVector(GridMap g, vector<vector<string>> &vecInfo);
//************* ADD  END  *************//

/* 二重include対策 */
#endif
