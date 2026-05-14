#include "CoordStrFormat/CoordStrFormat.h"
#include <float.h>

using namespace std;

/**
* 0.0のdouble型の数値文字列からマイナス符号を削除する
* @author    Nagoya University
* @date      2020/03/16
* @param     str=double型の数値文字列
* @return    整形後文字列
*/
std::string deleteZeroMinus(std::string str) {

  std::string retString = str;

  // double型を取得
  double num = stod(str);

  // 0.0にマイナス符号がある場合は符号を削除
  if (num == 0.0) {
    if (retString[0] == '-') retString.erase(0,1);
  }

  return retString;
}

/**
* double型の数値文字列から末尾の不要な0を削除する
* @author    Nagoya University
* @date      2020/03/16
* @param     str=double型の数値文字列
* @return    整形後文字列
*/
std::string deleteExtraZero(std::string str) {

  //std::cout << __func__ << ": " << "str: " << str << std::endl;
  std::string retString = "";
  ostringstream oss;
  int precision = 0;

  // 小数点位置を探す
  bool digitExists = true;
  int pos = str.find_first_of('.');
  if ((pos == std::string::npos) || (pos == 0)) {
    digitExists = false;
  }

  // double型を取得
  double num = stod(str);

  // 整数部の桁数を取得
  std::string intStr = str.substr(0,pos);
  int intStrSize = intStr.size();
  if (intStr[0] == '-') intStrSize-=1;

  // 小数部の精度を決定
  if (digitExists == true) {
    // 小数部を取得
    std::string decStr = str.substr(pos+1);

    int signDigit = -1;
    if (decStr.find_last_not_of("0") == std::string::npos) signDigit =  1;                             // 全て0の場合は1桁残す
    if (decStr.find_last_not_of("0") != std::string::npos) signDigit = decStr.find_last_not_of("0")+1; // 0でない数字の後ろまで

    // 保証可能な最大精度を超えている場合は補正
    if (signDigit > DBL_DIG) signDigit = DBL_DIG;

    // 小数部を1桁残す場合
    if (signDigit == 1) {
      if ((intStrSize >= DBL_DIG) || ((intStrSize + signDigit) >= DBL_DIG)) {
        // 整数部が最大精度を超えている場合、
        // fixedを指定すると10進数表記になってしまうため指定しない
        // 整数部と小数部の合計が最大精度を超えている場合、Fixidは不要なため指定しない
      } else {
        // fixedを指定して0を補足する
        oss << setiosflags(ios::fixed);
      }
    }

    // 精度を設定
    if ((signDigit == DBL_DIG) || ((intStrSize + signDigit) >= DBL_DIG)) {
      // 小数部が最大精度を超えている、または、
      // 整数部と合わせると超える場合は最大精度
      precision = DBL_DIG;
    } else {
      // 整数の桁数を精度に加算
      precision = signDigit;
    }
  }

  // 指定精度で変換
  oss << setprecision(precision) << num;

  // 変換結果を出力
  retString = oss.str();

  //std::cout << __func__ << ": " << "retString: " << retString << std::endl;

  return retString;
}

/**
* GEOSから返却されたGeometry型のWKT文字列から不要な文字を削除する。
*   - ジオメトリ名の不要文字
*   - 座標末尾の0
*   - 0.0のマイナス符号
* @author    Nagoya University
* @date      2020/03/16
* @param     str=GEOSから返却されたGeometry型の文字列
* @return    整形後文字列
*/
std::string stringFormatGeosToDm2(std::string wktStr) {

  std::string retString = "";

  // ジオメトリ名と座標文字列の区切り位置を探す
  int pos = wktStr.find_first_of('(');
  if (pos == std::string::npos) {
    return wktStr;
  }

  // ジオメトリ名と座標文字列に分ける
  std::string strGeomName = "";   // ジオメトリ名
  std::string strCoordinate = ""; // 座標文字列
  if (pos != 0) {
    strGeomName = wktStr.substr(0,pos);
  }
  strCoordinate = wktStr.substr(pos);

  // ジオメトリ名の編集
  for (int j = 0; j < strGeomName.size(); ++j) {
    // 半角スペースを探索
    if (strGeomName[j] == ' ') {
      // 半角スペースがあれば以降を削除
      strGeomName.erase(j);
    }
  }

  // 座標文字列の編集
  // double型の文字列を抽出してdeleteExtraZero()の編集結果と置き換える。
  std::string formattedStr = ""; // 編集結果文字列
  bool getNumStart = false; // double文字列内にいるか(文字列内フラグ)
  int numStartPos = 0;      // double文字列の先頭位置
  int numEndPos = 0;        // double文字列の終了位置
  for (int i = 0; i < strCoordinate.size(); ++i) { // 座標文字列を一文字づつ処理
    char ch = strCoordinate[i];
    if ((isdigit(ch)) || (ch == '-') || (ch == '.')) { // 数値文字か"-"か"."(double型文字)
      if (!getNumStart) {
        // double文字列外から内になった場合、開始位置を設定し、文字列内フラグを設定
        numStartPos = i;
        getNumStart = true;
      }
      numEndPos = i; // 終了位置を更新
    } else { // double型文字列以外
      if (getNumStart) {
        // double型文字列の外に出た場合、開始/終了位置から切り出して編集
        std::string numStr = strCoordinate.substr(numStartPos,numEndPos - numStartPos+1);
        // 末尾に不要な0がある場合は削除
        std::string deletedNumStr = deleteExtraZero(numStr);
        // 0.0にマイナス符号がある場合は符号を削除
        deletedNumStr = deleteZeroMinus(deletedNumStr);
        // 編集結果を結果文字列に追加
        formattedStr += deletedNumStr;
        getNumStart = false;
      }
      // double型文字以外はそのまま結果文字列に追加
      if ((ch == ' ') && (i!=0)) {
        // カンマの後ろの空白は追加しない
        if (strCoordinate[i-1] != ',') formattedStr += ch;
      } else {
        formattedStr += ch;
      }
    }
  }

  // 返却文字列を作成
  retString = strGeomName + formattedStr;

  return retString;
}
