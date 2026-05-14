#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <bitset>
#include <cmath>

#include "BitString.h"

using namespace std;

#include <is/DmException.h>

/**
* ビット配列の値で入った Y を,Y のサイズ分,X の中間ビットと比較し,等しければ trueを返す.
* X の中間ビットとは,X の上位ビットから Z だけ下位にずらしたビットのことである.
* X のビット数が Z 以下,X の中間ビットのサイズが Y のビットのサイズより小さい,
* X の中間ビットと不等号のいずれかであれば,false を返す.
*
* @author    Nagoya University
* @date      2022/02/04
* @param     引数1=string バイト配列(16進数文字列)
* @param     引数2=string ビット配列(2進数文字列)
* @param     引数3=unsigned int 比較開始ビット位置
* @return    bool
*/
extern "C" vector< vector<string> > UF_MATCH_MID_BIT(vector< vector<string> > argList)
{
	vector< vector<string> > retList;

	// 入力された各行に対して操作を行う
	for(int i=0; i<(int)argList.size(); i++)
	{
		if(argList.at(i).size() != 3) {
			throw CastException("Insufficient number of arguments!");
		}

		vector<string> ret;
		bool is_matched = false;
		
		try {
			// バイト配列(16進数)をビット配列(2進数)に変換
			string bit_str_x = UF_BitString::byteStrToBitStr(argList.at(i).at(0));
			// ビット配列が正しいフォーマットかチェック
			UF_BitString::checkBitStr(argList.at(i).at(1));
			string &bit_str_y = argList.at(i).at(1);
			// ビット判定開始位置を文字列から数値に変換
			int start_bit_x = UF_BitString::strToInt(argList.at(i).at(2));

			// サイズが妥当であれば各ビットが一致するか中間ビットを判定
			int num_of_bit_x = bit_str_x.size();
			int num_of_bit_y = bit_str_y.size();
			if(num_of_bit_x - start_bit_x >= num_of_bit_y) {
				if(bit_str_x.substr(start_bit_x, num_of_bit_y) == bit_str_y){
					is_matched = true;
				}
			}
		} catch (invalid_argument const&) {
		  throw CastException("Data type is not match!");
		} catch (out_of_range const&) {
		  throw CastException("input is out of range!");
		}

		ret.push_back(std::to_string(is_matched));
		retList.push_back(ret);
	}
	return retList;
}
