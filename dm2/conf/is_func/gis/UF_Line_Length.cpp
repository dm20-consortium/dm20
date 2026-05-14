#include <iostream>
#include <vector>
#include <string>
#include <is/StringUtil.h>
#include <geos_c.h>
#include <algorithm>
#include <is/DmException.h>

using namespace std;

void notice_function(const char *message, ...){ printf("%s", message); } // 依存ライブラリの出力用関数
void initialize(){ initGEOS(notice_function, notice_function); }         // 依存ライブラリの初期化処理
void finish(){ finishGEOS(); }                                           // 依存ライブラリの終了処理

/**
* LINESTRING/MULTILINESTRINGの長さを返却する。
* @author    Nagoya University
* @date      2020/03/12
* @param     引数1=geometry(LINESTRING/MULTILINESTRING)
* @return    double
*/
extern "C" vector< vector<string> > UF_LINE_LENGTH(vector< vector<string> > argList)
{
  //printf("UF_Line_Length called!\n");
  vector< vector<string> > retList; // ISに返却する結果2次元配列(retList[row][col])

  // 依存ライブラリの初期化処理
  

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    IS::StringUtil stringUtil;
    vector<string> ret;
    double length = 0.0;
    string a1;
    try {
      a1 = argList.at(i).at(0); // 引数1
    } catch (invalid_argument) {
       throw CastException("Data type is not match!");
    } catch (out_of_range) {
       throw CastException("Insufficient number of arguments!");
    }

    // WKTからSRIDを分離する
    int srId1 = stringUtil.getSRID(a1);
    if (srId1 != 0) a1 = stringUtil.split(a1,";").at(1);

    // Geometry文字列を大文字に変換
    string geomStUpp1 = a1;
    transform(geomStUpp1.begin(),geomStUpp1.end(),geomStUpp1.begin(),::toupper);

    // Geometryの型チェック
    string geomName = geomStUpp1;
    geomName = stringUtil.split(geomName,"(").at(0);
    string::size_type left = geomName.find_first_not_of(" ");
    geomName = stringUtil.split(geomName.substr(left)," ").at(0);
    if ((geomName != "LINESTRING") && (geomName != "MULTILINESTRING")) {
      if ((geomName != "POINT") && (geomName != "POLYGON") &&
          (geomName != "MULTIPOINT") && (geomName != "MULTIPOLYGON")) {
         throw CastException("parse error - invalid geometry.");
      } else {
        //(MULTI)POINT、(MULTI)POLYGONは0を返却
        length = 0.0;
        string retString = to_string(length);
        ret.push_back(retString);
        retList.push_back(ret);
        continue;
      }
    }

    // GeometryのEMPTY判定
    // EMPTYの場合は0を返却
    if (geomStUpp1.find("EMPTY") != std::string::npos) {
      ret.push_back("0");
      retList.push_back(ret);
      continue;
    }

    // WKT -> Geometry
    GEOSGeometry* g1;
    g1 = GEOSGeomFromWKT(a1.c_str());
    if (g1 == NULL) {
      
      throw CastException("GEOS ERROR. WKT to Geom failed.");
    }

    // GeometryにSRIDを設定
    GEOSSetSRID(g1,srId1);

    // 長さを取得
    GEOSLength(g1,&length);

    // Geometryの破棄
    GEOSGeom_destroy(g1);

    // 表示形式に整形
    string retString = to_string(length);

    ret.push_back(retString);
    retList.push_back(ret);
  }

  // 依存ライブラリの終了処理
  
  //printf("UF_Line_Length end!\n");
  return retList;
}
