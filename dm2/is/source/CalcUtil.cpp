#include "CalcUtil.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"

#include <iostream>

using std::cout;		// cout
using std::endl;		// endl
using namespace std;

namespace IS {

	/**
	 * 引数に指定された文字列から計算を実施する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/13
	 *
	 * @param	state	計算式
	 *
	 * @return	計算結果
	 */

	double CalcUtil::CalcString(const string &state)
	{
		char varstack[100];  //演算子用のスタック
		char porstr[100][20]; //逆ポーランド記法配列（数値は文字列、他は1文字）
		char numstr[20];     //数値

		int i;
		int k = 0;//stacksize
		int varstacksize = 0;
		int numstrsize = 0;
		char str[100];
		strncpy(str, state.c_str(), 100);

		int strLen = strlen(str);
		if (strLen == 0) {
			logger->error("parameter is empty err");
			return 0.0;
		}
		if (strLen >= 98) {
			logger->error("parameter too long err");
			return 0.0;
		}

		//前後に '(', ')' を追加
		for (i = strLen; i > 0; i--)
			str[i] = str[i - 1];
		str[0] = '(';
		strLen += 2;
		str[strLen - 1] = ')';
		str[strLen] = '\0';

		for (i = 0; i < strLen; i++) {
			switch (str[i]) {//入力文字列の先頭文字
			case '0':case '1':case '2':case '3':case '4':  //数値の場合
			case '5':case '6':case '7':case '8':case '9':case '.':
				numstr[numstrsize] = str[i];//数値文字列に追加
				numstrsize++;
				break;
			case '+': //+,-の場合は演算子スタックを掃き出す
			case '-':
				if (numstrsize > 0) {
					numstr[numstrsize] = '\0';
					strcpy(porstr[k], numstr);
					k++;
					numstrsize = 0;
				}
				while (varstacksize > 0) {//演算子スタックが空でない場合
					if (varstack[varstacksize - 1] == '(') {//stack の top
						break;//トップが開き括弧のときはブレイク
					}
					//演算子スタックからポーランド記法へ
					porstr[k][0] = varstack[varstacksize - 1];
					porstr[k][1] = '\0';
					k++;
					varstacksize--;//演算子スタックから1文字削除
				}
				varstack[varstacksize] = str[i];
				varstacksize++;
				break;
			case '*':
			case '/': //*,/の場合は演算子スタックに追加
				if (numstrsize > 0) {
					//数値文字列があればポーランド記法に加える
					numstr[numstrsize] = '\0';
					strcpy(porstr[k], numstr);
					k++;
				}
				numstrsize = 0;//数値文字列の初期化（空にする）
				if (varstacksize > 0 && (varstack[varstacksize - 1] == '*' || varstack[varstacksize - 1] == '/')) {
					//演算子スタックからポーランド記法へ
					porstr[k][0] = varstack[varstacksize - 1];
					porstr[k][1] = '\0';
					k++;
					varstacksize--;//演算子スタックから1文字削除
				}
				varstack[varstacksize] = str[i];
				varstacksize++;
				break;
			case '(':
				varstack[varstacksize] = str[i];//演算子スタックへ
				varstacksize++;
				break;
			case ')':
				if (numstrsize > 0) {
					//数値文字列があればポーランド記法に加える
					numstr[numstrsize] = '\0';
					strcpy(porstr[k], numstr);
					k++;
				}
				numstrsize = 0;//数値文字列の初期化（空にする）
				while (varstacksize > 0) {//演算子スタックが空でない間
					if (varstack[varstacksize - 1] == '(') {
						varstacksize--;//開き括弧の時はスタックから消してブレイク
						break;
					}
					porstr[k][0] = varstack[varstacksize - 1];//ポーランド記法文字列へ
					porstr[k][1] = '\0';
					k++;
					varstacksize--;//演算子スタックから1文字削除
				}
				break;
			}
		}

		//  逆ポーランド記法の計算
		double numstack[100];
		int numstacksize = 0;

		for (i = 0; i < k; i++) {
			if (porstr[i][0] >= '0'&&porstr[i][0] <= '9') {
				numstack[numstacksize] = atof(porstr[i]);
				numstacksize++;
			}
			else {
				if (numstacksize < 2) {
					logger->error("numstacksize < 2 err. state:" + state);
					return 0.0;
				}
				double ope1, ope2;
				ope1 = numstack[numstacksize - 2];
				ope2 = numstack[numstacksize - 1];
				switch (porstr[i][0]) {
				case '-':numstack[numstacksize - 2] = (ope1 - ope2); break;
				case '+':numstack[numstacksize - 2] = (ope1 + ope2); break;
				case '/':numstack[numstacksize - 2] = (ope1 / ope2); break;
				case '*':numstack[numstacksize - 2] = (ope1*ope2); break;
				}
				numstacksize--;
			}
		}
		if (numstacksize != 1) {
			logger->error("numstacksize != 1 err. state:" + state);
			return 0.0;
		}

		return numstack[0];
	}
}