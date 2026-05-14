#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include <is/DmException.h>
/**
* 引数1の配列を','区切りの文字列へと変換する
* @author    Nagoya University
* @date      2020/03/17
* @param     引数1=vector(double)
* @return    string
*/
extern "C" vector< vector<string> > UF_ARRAY_TO_STRING(vector< vector<string> > argList)
{
  vector< vector<string> > retList;

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    vector<vector<string>> dimenStrList; // 次元毎で分割した文字列
    vector<vector<int>> dimenList; // 次元毎の配列サイズ
    bool check = true; // 最終的に文字列操作を行うかのフラグ

    std::string a1;
    try {
      a1 = argList.at(i).at(0); // 引数1
    } catch (invalid_argument) {
      throw CastException("Data type is not match!");
    } catch (out_of_range) {
      throw CastException("Insufficient number of arguments!");
    }

    // 文字列から配列定義の文字列を削除する
    size_t point;
    while((point = a1.find("ARRAY", 0, 5)) != string::npos){
      a1.erase(point,5);
    }
    while((point = a1.find("array", 0, 5)) != string::npos){
      a1.erase(point,5);
    }

    // 文字列内で、最初の'['が何度続くかで何次元配列かを算出する
    int length = 0;
    point = a1.find("[");
    while(length < a1.length())
    {
      if (a1.at(length+point) != '[' )
        break;

      length++;
    }

    // 配列定義を削除した文字列をベクターに格納
    vector<string> param;
    param.push_back(a1);
    dimenStrList.push_back(param);

    // 次元毎に配列数を算出して、Vectorに格納する
    for (int j=0; j < length; j++)
    {
       vector<string> searchList;
       vector<int> tmpDimenList;
       vector<string> tmpList;
       std::string searchStr;
       
       // 検索対象の文字列を作成
       std::string findStr;
       int count = 0;
       // 次元の数 - 1だけ']'をつける
       while (count < length-(j+1))
       {
         findStr += "]";
         count++;
       }
       // 末尾に','を追加
       findStr += ",";

       searchList = dimenStrList.at(j);
       for (int k=0; k < searchList.size(); k++) {
         searchStr = searchList.at(k);
         int dimen = 1;
         size_t position = 0;
         // 文字列が末尾となるまでfindで検索を行う
         while(position != std::string::npos)
         {
           position = searchStr.find(findStr);
           if (position != std::string::npos) {
             // 検索対象文字列が見つかった場合はカウントアップを行い
             // 検索文字列をポイント以前の文字列を抜き出して
             // ベクターに設定し、検索文字列をポイント以降のみにする
             dimen++;
             string tmpStr = searchStr.substr(0, position);
             tmpList.push_back(tmpStr);
             searchStr = searchStr.substr(position + findStr.length());
           }
         }
         // 次元の区切りで分割した文字列をvectorに格納
         tmpList.push_back(searchStr);

         // 検索した回数=j次元の配列数をvectorに格納
         tmpDimenList.push_back(dimen);
       }
       dimenStrList.push_back(tmpList);
       dimenList.push_back(tmpDimenList);
    }

    // 多次元配列の場合に、2次元以降のサイズチェックを行う
    for (int dimens=1; dimens < dimenList.size(); dimens++) {
      // 次元毎にサイズを取り出して比較を行う
      vector<int> dimenRow = dimenList.at(dimens);
      for (int rowSize=1; rowSize < dimenRow.size(); rowSize++) {
        int base, compare;
        base = dimenRow.at(0);
        compare = dimenRow.at(rowSize);
        // 次元の先頭要素数と同じかをチェック
        if (base != compare) {
          check = false;
          // 次元の数が異なる配列が含まれているためエラーとする
          throw CastException("multidimensional arrays must have array expression with matching dimensions!");
          break;
        } 
      }
      if (!check) {
          break;
      }
    }

    // 次元チェックを行い正常に終了した場合は
    // 文字列操作にて配列を文字列とする
    if (check) {

      // 文字列から'['を削除する
      while((point = a1.find_first_of("[")) != string::npos){
        a1.erase(point,1);
      }

      // 文字列から']'を削除する
      while((point = a1.find_first_of("]")) != string::npos){
        a1.erase(point,1);
      }

      // 文字列から','が連続している場合に1つ削除する
      while((point = a1.find(",,",0,2)) != string::npos){
        a1.erase(point,1);
      }

      ret.push_back(a1);
    }
    retList.push_back(ret);
  }
  return retList;
}
