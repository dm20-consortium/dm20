#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include <is/DmException.h>

void createDimenList(string &argStr, vector< vector<int> >* dimenList, vector< vector<string> >* dimenStrList )
{
  // 文字列内で、最初の'['が何度続くかで何次元配列かを算出する
  int length = 0;
  size_t point = argStr.find("[");
  while(length < argStr.length())
  {
    if (argStr.at(length+point) != '[' )
      break;

    length++;
  }

  // 初回用の検索文字列は引数のままのため
  // そのままVectorに格納する
  vector<string> param;
  param.push_back(argStr);
  dimenStrList->push_back(param);

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

    searchList = dimenStrList->at(j);
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
    dimenStrList->push_back(tmpList);
    dimenList->push_back(tmpDimenList);
  }
  dimenStrList->clear();
}

bool checkLocalDimen(vector< vector<int> >* dimenList)
{
  bool ret = true;

  // 多次元配列の場合に、2次元以降のサイズチェックを行う
  for (int dimens=1; dimens < dimenList->size(); dimens++) {
    // 次元毎にサイズを取り出して比較を行う
    vector<int> dimenRow = dimenList->at(dimens);
    for (int rowSize=1; rowSize < dimenRow.size(); rowSize++) {
      int base, compare;
      base = dimenRow.at(0);
      compare = dimenRow.at(rowSize);
      // 次元の先頭要素数と同じかをチェック
      if (base != compare) {
        ret = false;
        // 次元の数が異なる配列が含まれているためエラーとする
        throw CastException("multidimensional arrays must have array expression with matching dimensions!");
        break;
      }
    }
    if (!ret) {
        break;
    }
  }
  return ret;
}

/**
* 引数1の配列と引数2の配列を結合する
* @author    Nagoya University
* @date      2020/03/17
* @param     引数1=vector(double) 引数2=vector(double)
* @return    vector(double)
*/
extern "C" vector< vector<string> > UF_ARRAY_CAT(vector< vector<string> > argList)
{
  vector< vector<string> > retList;

  // 入力された各行に対して操作を行う
  for(int i=0; i<argList.size(); i++)
  {
    vector<string> ret;
    vector< vector<string> > dimenStrList; // 次元毎で分割した文字列
    vector< vector<int> > dimenList1; // 次元毎の配列サイズ格納用vector(引数1)
    vector< vector<int> > dimenList2; // 次元毎の配列サイズ格納用vector(引数2)
    bool check1, check2;

    std::string a1, a2;
    try {
      a1 = argList.at(i).at(0);  // 引数1
      a2 = argList.at(i).at(1);  // 引数2
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
    while((point = a2.find("ARRAY", 0, 5)) != string::npos){
      a2.erase(point,5);
    }
    while((point = a2.find("array", 0, 5)) != string::npos){
      a2.erase(point,5);
    }

    createDimenList(a1, &dimenList1, &dimenStrList);
    createDimenList(a2, &dimenList2, &dimenStrList);

    check1 = checkLocalDimen(&dimenList1);
    check2 = checkLocalDimen(&dimenList2);

    // 引数内部の次元チェックが正常だったので
    // 結合する配列間の次元チェックを行う
    if (check1 && check2) {
      // 次元の数が異なっているためエラーとする
      bool catCheck = true;

      int a1Size = (int)dimenList1.size();
      int a2Size = (int)dimenList2.size();
      int diffSize = a1Size - a2Size;
      // 次元数の差が1以内でなければエラーとする
      if (diffSize != 1 && diffSize != 0 && diffSize != -1) {
        catCheck = false;
      } else {
        if (a1Size > a2Size) {
          // 配列の各次元でのサイズが一致しているか確認する
          for (int i=0; i < dimenList2.size(); i++) {
            vector<int> tmpDimen1, tmpDimen2;
            tmpDimen1 = dimenList1.at(i+1);
            tmpDimen2 = dimenList2.at(i);
            if (tmpDimen1.at(0) != tmpDimen2.at(0)) {
              catCheck = false;
              break;
            }
          }
        } else if (a2Size > a1Size) {
          // 配列の各次元でのサイズが一致しているか確認する
          for (int i=0; i < dimenList1.size(); i++) {
            vector<int> tmpDimen1, tmpDimen2;
            tmpDimen1 = dimenList1.at(i);
            tmpDimen2 = dimenList2.at(i+1);
            if (tmpDimen1.at(0) != tmpDimen2.at(0)) {
              catCheck = false;
              break;
            }
          }
        } else {
          // 配列の各次元でのサイズが一致しているか確認する
          for (int i=1; i < dimenList2.size(); i++) {
            vector<int> tmpDimen1, tmpDimen2;
            tmpDimen1 = dimenList1.at(i);
            tmpDimen2 = dimenList2.at(i);
            if (tmpDimen1.at(0) != tmpDimen2.at(0)) {
              catCheck = false;
              break;
            }
          }
        }
      }

      if (!catCheck) {
        // 次元の数が異なっているためエラーとする
        throw CastException("multidimensional arrays must have array expression with matching dimensions!");
      } else {
        size_t point1, point2;
        std::string retStr;

        // '['の始まりを探して文字列を切り出す
        point1 = a1.find('[');
        point2 = a2.find('[');
        a1 = a1.substr(point1);
        a2 = a2.substr(point2);
        if (a1Size > a2Size) {
          // 結合するため引数1の末尾の']'を削除
          a1 = a1.substr(0, a1.length() - 1);
          // 結合するため引数2の末尾に']'を追加
          a2 += "]";
          // 間に','を入れて2つを結合
          retStr = a1 + "," + a2;
        } else if (a2Size > a1Size) {
          // 結合するため引数1の先頭に'['を追加
          a1 = "[" + a1;
          // 結合するため引数2の先頭の'['を削除
          a2 = a2.substr(1);
          // 間に','を入れて2つを結合
          retStr = a1 + "," + a2;
        } else {
          // 結合するため引数1の末尾の']'を削除
          a1 = a1.substr(0, a1.length() - 1);
          // 結合するため引数2の先頭の'['を削除
          a2 = a2.substr(1);
          // 間に','を入れて2つを結合
          retStr = a1 + "," + a2;
        }
        size_t point;
        // 文字列から'['を'{'に置換する
        while((point = retStr.find_first_of("[")) != string::npos){
          retStr.replace(point,1,"{");
        }

        // 文字列から']'を'}'に置換する
        while((point = retStr.find_first_of("]")) != string::npos){
          retStr.replace(point,1,"}");
        }
        ret.push_back(retStr);
      }
    }
    retList.push_back(ret);
  }
  return retList;
}
