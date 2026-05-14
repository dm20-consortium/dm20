#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <string>

using namespace std;

#include <is/DmException.h>
/**
* 行の入力値を収集して昇順で連番を返す
* @author    Nagoya University
* @date      2023/02/21
* @param     引数1=double
* @return    int
*/
extern "C" vector< vector<string> >UF_ROW_NUMBER_ASC(vector< vector<string> > argList)
{
	vector< vector<string> > retList;
	vector<double> valueList, sortedValueList;
	map<double, int> keyIndex;
	// 入力された各行に対して操作を行う
	for(int i=0; i<argList.size(); i++)
	{
		double a1;
		try {
			a1 = std::stod(argList.at(i).at(0)); // 引数1
		} catch (invalid_argument) {
			throw CastException("Data type is not match!");
		} catch (out_of_range) {
			throw CastException("Insufficient number of arguments!");
		}
		valueList.push_back(a1);
	}
	copy(valueList.begin(), valueList.end(), back_inserter(sortedValueList) );
	sort(sortedValueList.begin(), sortedValueList.end());
	int rank = 0;
	// ソート済みの値リストに対して、ランクを登録する。重複する箇所は、最初だけにする。
	for (int i = 0; i < sortedValueList.size(); i++) {
		double val = sortedValueList[i];
		rank++;
		if (keyIndex.find(val) == keyIndex.end()) {
			keyIndex[val] = rank;
		}
	}
	// 元の並び順に対して、値をキーに先程のランクを登録していく。重複している場合は、ランクをインクリメントする。
	// ３個以上重複している可能性もあるので、map自体もインクリメントする。
	for (int i = 0; i < valueList.size(); i++) {
		double val = valueList[i];
		int rank = keyIndex[val];
		keyIndex[val] = keyIndex[val] + 1;
		vector<string> ret;
		ret.push_back(std::to_string(rank));
		retList.push_back(ret);
	}
	return retList;
}
