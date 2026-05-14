/*
 * クラス LeafNode
 *
 * 構文解析結果の木構造のうち，終端ノードを表すクラスの実装．
 */

/* クラス定義ヘッダファイル */
#include "Parser.h"

/* 依存ライブラリ */
#include <stdexcept>
#include <list>
#include <cctype>
#include <cstring>

using namespace IS;



/**
 * コンストラクタ.
 *
 * 
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	value	引数は終端記号の文字列
 */
LeafNode::LeafNode(string value)
{
    this->value = value;
    if(isNumber())
        this->symbol = Symbol::NUMBER;
    else if (isComparator())
        this->symbol = Symbol::COMPARATOR;
    else if (isSign())
        this->symbol = Symbol::SIGN;
    else if (isString())
        this->symbol = Symbol::STRING;
    else if (isAsterisk())
        this->symbol = Symbol::ASTERISK;
    else if (isIdentifier())
        this->symbol = Symbol::IDENTIFIER;
    else if (isSemicolon())
        this->symbol = Symbol::SEMICOLON;
    else if (isValues())
        this->symbol = Symbol::VALUES;
    else if (isNumberArray())
        this->symbol = Symbol::NUMBER_ARRAY;
    else if (isStringArray())
        this->symbol = Symbol::STRING_ARRAY;
	else
		this->symbol = Symbol::UNKNOWN;
}

/**
 * デストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 */
LeafNode::~LeafNode()
{
    ;
}

/**
 * コンストラクタ引数valueの種類がVALUE_ATTR
 *
 * @author	Nagoya University
 * @date	2022/02/15
 * @return	文字列<VALUE_ATTR>であればtrue, それ以外はfalse
 */
bool LeafNode::isValues()
{
    return isNumber() | isString() | isBoolen() | isNull();
}
/**
 * コンストラクタ引数valueの種類が数値かどうかを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	文字列が数値であればtrue, それ以外はfalse
 */
bool LeafNode::isNumber()
{
    try {
        std::stod(value);
        return true;
    } catch (std::invalid_argument& e){
        return false;
    }
}

/**
 * コンストラクタ引数valueの種類がNULLかどうかを返す.
 *
 * @author	Nagoya University
 * @date	2022/02/21
 * @return	文字列が"NULL"であればtrue, それ以外はfalse
 */
bool LeafNode::isNull()
{
    return (!strcasecmp(value.c_str() ,  "null"));
}

/**
 * コンストラクタ引数valueの種類がBooleanかどうかを返す.
 *
 * @author	Nagoya University
 * @date	2022/02/21
 * @return	文字列が"TRUE","FALSE"であればtrue, それ以外はfalse
 */
bool LeafNode::isBoolen()
{
    return (!strcasecmp(value.c_str() ,  "true") | !strcasecmp(value.c_str() ,  "false"));
}

/**
 * コンストラクタ引数valueの種類がシングルクォートで囲まれた文字列かどうかを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	文字列がシングルクォートで囲まれていればtrue, それ以外はfalse
 */
bool LeafNode::isString()
{
    return (value.front() == '\'' && value.back() == '\'') || (value.front() == '"' && value.back() == '"');
}

/**
 * コンストラクタ引数valueの種類が比較演算子かどうかを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	文字列が比較演算子であればtrue, それ以外はfalse
 */
bool LeafNode::isComparator()
{
    return value == "<" || value == ">" || value == ">=" || value == "<=" || value == "=" || value == "<>";
}

/**
 * コンストラクタ引数valueの種類が符号かどうかを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/09
 * @return	文字列が符号であればtrue, それ以外はfalse
 */
bool LeafNode::isSign()
{
    return value == "+" || value == "-";
}


/**
 * コンストラクタ引数valueの種類が * (アスタリスク) かどうかを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	文字列が*であればtrue, それ以外はfalse
 */
bool LeafNode::isAsterisk()
{
	return value == "*";
}

/**
 * コンストラクタ引数valueの種類が識別子かどうかを返す.
 *
 * <pre>
 * 識別子は以下の2つの条件を同時に満たす．
 * ・1文字目がアルファベットまたはアンダースコア'_'
 * ・2文字目以降はアルファベットまたは数字またはアンダースコア'_'またはドット'.'
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	文字列が識別子であればtrue, それ以外はfalse
 */
bool LeafNode::isIdentifier()
{
	if(value.size() <= 0)
		return false;
	if(! (isalpha(value[0]) || value[0] == '_'))
		return false;
	for(unsigned int i=1; i < value.size(); i++)
		if(! (isalnum(value[i]) || value[i] == '_' || value[i] == '.'))
			return false;
	return true;
}
/**
 * コンストラクタ引数valueの種類が識別子かどうかを返す.
 *
 * <pre>
 * 識別子は以下の2つの条件を同時に満たす．
 * ・1文字目がアルファベットまたはアンダースコア'_'
 * ・2文字目以降はアルファベットまたは数字またはアンダースコア'_'またはドット'.'
 * </pre>
 *
 * @author	Shinichi Kusayama
 * @date	2025/02/22
 * @return	文字列が識別子であればtrue, それ以外はfalse
 */
bool LeafNode::isFormula()
{
	if(value.size() <= 0)
		return false;
	if(! (isalpha(value[0]) || value[0] == '_'))
		return false;
	for(unsigned int i=1; i < value.size(); i++)
		if(! (isalnum(value[i]) || value[i] == '_' || value[i] == '.' || value[i] == '+' || value[i] == '-' || value[i] == '*' || value[i] == '/' || value[i] == '%'))
			return false;
	return true;
}

/**
 * コンストラクタ引数valueの種類が識別子かどうかを返す.
 *
 * <pre>
 * 識別子は以下の2つの条件を同時に満たす．
 * ・文字列が'[]'で囲まれている
 * ・囲まれている文字がすべて数値あるいはカンマ
 * </pre>
 *
 * @author	Shinichi Kusayama
 * @date	2025/02/22
 * @return	文字列が識別子であればtrue, それ以外はfalse
 */
 bool LeafNode::isNumberArray()
 {
     if(value.size() <= 1)
         return false;
     if(! (value[0] == '['))
         return false;
     if (! (value[value.size() - 1] == ']')) 
         return false;
     for(unsigned int i=1; i < value.size() - 1; i++)
         if(! (isalnum(value[i]) || value[i] == ','))
             return false;
     return true;
 }
 
/**
 * コンストラクタ引数valueの種類が識別子かどうかを返す.
 *
 * <pre>
 * 識別子は以下の2つの条件を同時に満たす．
 * ・文字列が'[]'で囲まれている
 * ・囲まれている文字がすべて文字列あるいはカンマ※現状、省略
 * </pre>
 *
 * @author	Shinichi Kusayama
 * @date	2025/02/22
 * @return	文字列が識別子であればtrue, それ以外はfalse
 */
 bool LeafNode::isStringArray()
 {
     if(value.size() <= 1)
         return false;
     if(! (value[0] == '['))
         return false;
     if (! (value[value.size() - 1] == ']')) 
         return false;
     return true;
 }

/**
 * コンストラクタ引数valueの種類が ; (セミコロン)かどうかを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	文字列がセミコロンであればtrue, それ以外はfalse
 */
bool LeafNode::isSemicolon()
{
	return value == ";";
}


/**
 * ノードのシンボルを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	ノードのシンボル
 */
Symbol LeafNode::getSymbol()
{
    return symbol;
}

/**
 * ノードの子要素の数を返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	ノードのもつ子要素の数
 */
int LeafNode::getSize()
{
    return 1;
}

/**
 * ノードが終端ノードかどうかを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	ノードが終端ノードの場合はtrueを返すが，LeafNodeの場合は常にtrueを返す.
 */
bool LeafNode::isLeafNode()
{
    return true;
}

/**
 * ノードが非終端ノードかどうかを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	ノードが非終端ノードの場合はtrueを返す.LeafNodeの場合は常にfalseを返す.
 */
bool LeafNode::isIntermediateNode()
{
    return false;
}

/**
 * ノードが終端ノードの場合は値を返し，非終端の場合はtoStringと同じ結果を返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	LeafNodeの場合は常にvalueと同じ値を返す.
 */
string LeafNode::getValue()
{
    return value;
}

/**
 * ノードが非終端ノードの場合，指定した位置の子要素を返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	index	子要素のインデックス(0始まり)
 * @return	LeafNodeには子要素がないので，常にnullptrを返す.
 */
ParseTreeNode* LeafNode::getChildNode(int index)
{
    return nullptr;
}

/**
 * 自分自身と子孫の中から引数のシンボルと同じもののリストを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	symbol	検索条件になるシンボル
 * @return	自分自身と子孫の中から引数のシンボルと同じものへのポインタを集めたリスト.
 */
list<ParseTreeNode*>* LeafNode::getDescendantNodes(Symbol symbol)
{
    list<ParseTreeNode*> *rval = new list<ParseTreeNode*>();
    if(symbol == this->symbol)
        rval->push_back(this);
    return rval;
}

/**
 * ノードの複製を作り，それへのポインタを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	複製されたノードへのポインタ
 */
ParseTreeNode* LeafNode::copy()
{
    return new LeafNode(this->value);
}

/**
 * ノードの内容をLispのS式的な文字列へ変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	ノードの文字列表現
 */
string LeafNode::toString()
{
	return "( " + convertSymbolToString(this->symbol) + " " + this->value + " )";
}

