/* 
 * Tokenizer.cpp
 *
 * 字句解析を行うクラスの実装．
 */

/* クラス定義ヘッダファイル */
#include "Parser.h"

/* 依存ライブラリ */
#include <vector>
#include <cctype>

using namespace IS;

/**
 * コンストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	target	解析対象の文字列
 */
Tokenizer::Tokenizer(string target)
{
    tokenize(target);
}

/**
 * デストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 */
Tokenizer::~Tokenizer()
{
	tokens.clear();
}

/**
 * 次のトークンを取得する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	次のトークンがまだある場合はtrue，文字列の最後に達した場合はfalse
 */
bool Tokenizer::hasMoreTokens()
{
    return ! tokens.empty();
}


/**
 * キューの先頭にトークンを置く.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	token	キューの先頭に置くトークンの文字列
 */
void Tokenizer::pushToken(string token)
{
    tokens.push_front(token);
}

/**
 * キューの先頭のトークンを読む(先頭を除去しない).
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	キューの先頭にあるトークンの文字列(キューからは除去されない)
 */
string Tokenizer::lookAheadToken()
{
    return tokens.front();
}

/**
 * キューの先頭のトークンを取得する(先頭のトークンはキューから除去される).
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	キューの先頭にあるトークンの文字列(キューから除去される)
 */
string Tokenizer::getToken()
{
    string rval = tokens.front();
    tokens.pop_front();
    return rval;
}

/**
 * キューの先頭のトークンと，引数文字列を比較した結果を返す．一致した場合だけ，先頭が除去される.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	expected	比較対象となる文字列
 * @return	比較対象の文字列とキューの先頭にあるトークンの文字列が一致する場合はtrue (先頭のトークンは除去される).それ以外はfalse(先頭のトークンは維持される)
 */
bool Tokenizer::compareToken(string expected)
{
    if(! hasMoreTokens())
        return false;

    string s = getToken();
    if( s != expected ){
        pushToken(s);
        return false;
    }
    return true;
}

/**
 * キューの先頭のトークンと，引数シンボルを比較した結果を返す．一致した場合だけ，先頭が除去される.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	expected	比較対象となるシンボル
 * @return	比較対象のシンボルとキューの先頭にあるトークンが一致するシンボルの場合はtrue (先頭のトークンは除去される).それ以外はfalse(先頭のトークンは維持される)
 */
bool Tokenizer::compareSymbol(Symbol expected)
{
	if(! hasMoreTokens())
		return false;

	string token = getToken();
	string symbol = ParseTreeNode::convertSymbolToString(expected);
	if( ! QueryParser::iequals(token, symbol)){
		pushToken(token);
		return false;
	}
	return true;
}


/**
 * 文字列をトークンに分割する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	str	分割対象となる文字列
 */
void Tokenizer::tokenize(string str)
{
    string buffer;

    char ch;

    for(unsigned int i=0; i < str.size(); i++){
        ch = str.at(i);
        if(! isWhitespace(ch) && (! isStructureSymbol(ch) && ! isComparatorSymbol(ch) && ! isStringSymbol(ch) && ! isArithmeticSymbol(ch) && ! isSemicolon(ch)) )
            buffer += ch;

		else {
			if(buffer.size() > 0){
				tokens.push_back(buffer);
				buffer.clear();
			}
			if(isStructureSymbol(ch)){
                buffer += ch;
				tokens.push_back(buffer);
                buffer.clear();
            }
			else if(isComparatorSymbol(ch)){
                buffer += ch;
				char nextch = str.at(i+1);
				if(isComparatorSymbol(nextch)){
                    buffer += nextch;
					i++;
				}
				tokens.push_back(buffer);
                buffer.clear();
			}
			else if(isStringSymbol(ch)){
                buffer += ch;
				i++;
				for(; i < str.size() ; i++){
					ch = str.at(i);
                    buffer += ch;
					if(isStringSymbol(ch))
						break;
				}
				tokens.push_back(buffer);
                buffer.clear();
			}
			else if(isArithmeticSymbol(ch)){
				buffer += ch;
				tokens.push_back(buffer);
				buffer.clear();
			}
			else if(isSemicolon(ch)){
				buffer += ch;
				tokens.push_back(buffer);
				buffer.clear();
			}
		}
	}
	if(buffer.size() > 0){
		tokens.push_back(buffer);
		buffer.clear();
	}
}

/**
 * 引数の文字が空白文字かどうかを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	ch	対象文字列
 * @return	対象文字列が空白文字ならtrue，それ以外はfalse
 */
bool Tokenizer::isWhitespace(char ch)
{
    return isspace(ch);
}

/**
 * 引数の文字が括弧やカンマなどの構造を表す文字かどうかを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	ch	対象文字列
 * @return	対象文字列が括弧やカンマならtrue，それ以外はfalse
 */
bool Tokenizer::isStructureSymbol(char ch)
{
    return ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == ',';
}

/**
 * 引数の文字が<や>などの比較演算の記号かどうかを返す
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	ch	対象文字列
 * @return	対象文字列が比較演算記号ならtrue，それ以外はfalse
 */
bool Tokenizer::isComparatorSymbol(char ch)
{
    return ch == '<' || ch == '>' || ch == '!' || ch == '=';
}

/**
 * 引数の文字が，文字列の開始や終了を意味する ' であるかを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	ch	対象文字列
 * @return	対象文字列がシングルクォート記号ならtrue，それ以外はfalse
 */
bool Tokenizer::isStringSymbol(char ch)
{
    return ch == '\'' || ch == '\"';
}

/**
 * 引数の文字が算術演算の記号かどうかを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	ch	対象文字列
 * @return	対象文字列が算術演算記号ならtrue，それ以外はfalse
 */
bool Tokenizer::isArithmeticSymbol(char ch)
{
    return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%';
}

/**
 * 引数の文字がセミコロンかどうかを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	ch	対象文字列
 * @return	対象文字列がセミコロン記号ならtrue，それ以外はfalse
 */
bool Tokenizer::isSemicolon(char ch)
{
    return ch == ';';
}



