/*
 * クラス IntermediateNode
 *
 * 構文解析結果の木構造のうち，非終端ノードを表すクラスの実装．
 */

/* クラス定義ヘッダファイル */
#include "Parser.h"

/* 依存ライブラリ */
#include <stdexcept>
#include <list>

using namespace IS;

/**
 * コンストラクタ.
 *
 * 
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	symbol	終端記号/非終端記号の種類を表すシンボル
 */
IntermediateNode::IntermediateNode(Symbol symbol)
{
    this->symbol = symbol;
}

/**
 * コンストラクタ(子要素つき).
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	symbol	終端記号/非終端記号の種類を表すシンボル
 * @param	values	子要素の文字列のリスト
 */
IntermediateNode::IntermediateNode(Symbol symbol, initializer_list<string> values)
{
    this->symbol = symbol;
    for(auto x : values)
        this->values.push_back(new LeafNode(x));
}

/**
 * コンストラクタ(部分木つき).
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	symbol	終端記号/非終端記号の種類を表すシンボル
 * @param	values	子要素へのポインタのリスト
 */
IntermediateNode::IntermediateNode(Symbol symbol, initializer_list<ParseTreeNode*> values)
{
    this->symbol = symbol;
    for(auto x : values)
        this->values.push_back(x);
}

/**
 * デストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 */
IntermediateNode::~IntermediateNode()
{
    for(auto p = values.begin(); p != values.end(); p++){
        delete *p;
    }
    values.clear();
}

/**
 * 子要素の文字列(内部でLeafNodeに変換される)を末尾に追加する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	s	子要素の文字列表現
 */
void IntermediateNode::add(string s)
{
    this->values.push_back(new LeafNode(s));
}

/**
 * 子要素を末尾に追加する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	n	子要素へのポインタ
 */
void IntermediateNode::add(ParseTreeNode* n)
{
    this->values.push_back(n);
}

/**
 * ノードのシンボルを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	ノードのシンボル
 */
Symbol IntermediateNode::getSymbol()
{
    return this->symbol;
}

/**
 * ノードの子要素の数を返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	ノードのもつ子要素の数
 */
int IntermediateNode::getSize()
{
    return this->values.size();
}

/**
 * ノードが終端ノードかどうかを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	ノードが終端ノードの場合はtrueを返すが，IntermediateNodeの場合は常にfalseを返す.
 */
bool IntermediateNode::isLeafNode()
{
    return false;
}

/**
 * ノードが非終端ノードかどうかを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	ノードが非終端ノードの場合はtrueを返す.IntermediateNodeの場合は常にtrueを返す.
 */
bool IntermediateNode::isIntermediateNode()
{
    return true;
}

/**
 * ノードが終端ノードの場合は値を返し，非終端の場合はtoStringと同じ結果を返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	IntermediateNodeの場合は常にtoStringと同じ結果を返す.
 */
string IntermediateNode::getValue()
{
    return toString();
}

/**
 * ノードが非終端ノードの場合，指定した位置の子要素を返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	index	子要素のインデックス(0始まり)
 * @return	指定した位置の子要素へのポインタを返す.子要素がない場合はnullptrを返す.
 */
ParseTreeNode* IntermediateNode::getChildNode(int index)
{
    int i = 0;
    for(auto p = values.begin(); p != values.end(); p++){
        if(i == index)
            return *p;
        i++;
    }
    return nullptr;
}

/**
 * 自分自身と子孫の中から引数のシンボルと同じもののリストを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	symbol	検索条件になるシンボル
 * @return	自分自身と子孫の中から引数のシンボルと同じものへのポインタを集めたリスト
 */
list<ParseTreeNode*>* IntermediateNode::getDescendantNodes(Symbol symbol)
{
    list<ParseTreeNode*> *rval = new list<ParseTreeNode*>();
    if(symbol == this->symbol){
        rval->push_back(this);
        return rval;
    }
    for(auto x : values){
        list<ParseTreeNode*>* sub = x->getDescendantNodes(symbol);
        for(auto y : *sub){
            rval->push_back(y);
        }
    }
    return rval;
}

/**
 * ノードの複製を作り，それへのポインタを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	複製されたノードへのポインタ
 */
ParseTreeNode* IntermediateNode::copy()
{
    IntermediateNode* rval = new IntermediateNode(this->symbol);
    for(auto x : this->values){
        rval->add(x->copy());
    }
    return rval;
}

/**
 * ノードの内容をLispのS式的な文字列へ変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	ノードの文字列表現
 */
string IntermediateNode::toString()
{
	string rval = "( " + convertSymbolToString(this->symbol) + " ";

    for(auto x : this->values)
        rval += x->toString() + " ";

	rval += ")";
	return rval;
}
