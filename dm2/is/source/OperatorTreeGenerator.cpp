/*
 * クラス OperatorTreeGenerator
 *
 * 構文解析木からオペレータグラフへ変換するクラスの実装．
 */

/* クラス定義ヘッダファイル */
#include "Parser.h"

/* 依存ライブラリ */
#include <iostream>
#include <list>

using namespace IS;
using namespace std;

/**
 * コンストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	root	構文解析結果の木のルートへのポインタ
 */
OperatorTreeGenerator::OperatorTreeGenerator(ParseTreeNode* root)
{
	this->gcount = 0;
	this->kcount = 0;
	this->ncount = 0;
	this->ecount = 0;

	this->root = root;
	this->graph = new Graph("g" + to_string(gcount++));
	
	// パラメータセットの設定(graphml可視化用)
	Key* k1 = new Key("operator_type");
	(*k1)["for"] = "node";
	(*k1)["attr.name"] = "operator_type";
	(*k1)["attr.type"] = "string";
	this->graph->addKey(k1);
	Key* k2 = new Key("parameter");
	(*k2)["for"] = "node";
	(*k2)["attr.name"] = "parameter";
	(*k2)["attr.type"] = "string";
	this->graph->addKey(k2);
	Key* k3 = new Key("nid");
	(*k3)["for"] = "node";
	(*k3)["attr.name"] = "nid";
	(*k3)["attr.type"] = "string";
	this->graph->addKey(k3);


	this->graphList.push_back(this->graph);
	this->master = "";
		
	analyzeMaster(root);

	analyzeQuery(root);

}

/**
 * デストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 */
OperatorTreeGenerator::~OperatorTreeGenerator()
{
	root = nullptr;
	graph = nullptr;
	for (auto itr = graphList.begin(); itr != graphList.end(); ++itr) {
		delete *itr;
		*itr = nullptr;
	}
}

/**
 * 変換後のオペレータグラフへのポインタを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	変換されたオペレータグラフへのポインタ
 */
Graph* OperatorTreeGenerator::getGraph()
{
	return graph;
}

/**
* 変換後のオペレータグラフリストのXML文字列を返す.
*
* @author	Nagoya University
* @date	2019/06/19
* @return	変換されたオペレータグラフリストのXML文字列
*/
string OperatorTreeGenerator::getGraphListXML()
{
	string rval = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	rval += "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\"\n";
	rval += "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
	rval += "  xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns \n";
	rval += "    http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">\n";

	for (auto itr = graphList.begin(); itr != graphList.end(); ++itr) {
		rval += (*itr)->toString();
	}

	rval += "</graphml>\n";

	return rval;
}

/**
 * MASTERノードを変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	subtree	変換対象の部分木へのポインタ
 */
void OperatorTreeGenerator::analyzeMaster(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s == Symbol::MASTER_LIST){
		for(int i=0; i < subtree->getSize(); i++){
			ParseTreeNode* child = subtree->getChildNode(i);
			if(child->getSymbol() == Symbol::IDENTIFIER){
				master += child->getValue();
				if(i+1 < subtree->getSize())
					master += ",";
			}
		}
	}
	else if(subtree->isIntermediateNode()){
		for(int i=0; i < subtree->getSize(); i++){
			analyzeMaster(subtree->getChildNode(i));
		}
	}
	return;
}

/**
 * QUERYノードを変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/08
 * @param	subtree	変換対象の部分木へのポインタ
 * @return	変換後のオペレータグラフへのポインタ
 */
Node* OperatorTreeGenerator::analyzeQuery(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::QUERY)
		throw new ParseException("input node is not QUERY");

	Node* out = new Node("n" + to_string(ncount++));
	DataStore* kvs = out->getDataStore();
	(*kvs)["operator_type"] = "out";
	(*kvs)["parameter"] = "";
	(*kvs)["master"] = master;
	graph->addNode(out);

	if(isRDB(subtree)){
		Node* rdb = new Node("n" + to_string(ncount++));
		DataStore* kvs = rdb->getDataStore();
		(*kvs)["operator_type"] = "RDB";
		(*kvs)["parameter"] = subtree->toSQLString();
		(*kvs)["master"] = master;
		graph->addNode(rdb);
		createEdge(rdb, out);
		return out;
	}

	for(int i=0; i < subtree->getSize(); i++){
		ParseTreeNode* child = subtree->getChildNode(i);
		secondNodeSymbol = child->getSymbol();

		if(secondNodeSymbol == Symbol::SUBQUERY){
			Node* sub = analyzeSubQuery(child);
			createEdge(sub, out);
		}
		else if(secondNodeSymbol == Symbol::UNION){
			Node* uni = analyzeUnion(child);
			createEdge(uni, out);
		}
		else if(secondNodeSymbol == Symbol::CREATE_STREAM){
			Node* create = analyzeCreateStream(child);
			createEdge(create, out);
		}
		else if(secondNodeSymbol == Symbol::DROP_STREAM){
			Node* drop = analyzeDropStream(child);
			createEdge(drop, out);
		}
		else if(secondNodeSymbol == Symbol::WITH_RECURSIVE){
			analyzeRecursive(child);
		}
	}
	return out;
}

/**
 * CREATE_STREAMノードを変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/06
 * @param	subtree	変換対象の部分木へのポインタ
 * @return	変換後のオペレータグラフへのポインタ
*/
Node* OperatorTreeGenerator::analyzeCreateStream(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::CREATE_STREAM)
		throw new ParseException("input node is not CREATE_STREAM");
	if(subtree->getSize() < 2)
		throw new ParseException("the size of CREATE_STREAM's children is less than 2");

	string stream = subtree->getChildNode(0)->getValue();
    string attrs = subtree->getChildNode(1)->toSQLString(); /* Default句対応 */
	
	Node* create = new Node("n" + to_string(ncount++));
	DataStore* kvs = create->getDataStore();
	(*kvs)["operator_type"] = "create";
	(*kvs)["stream_name"] = stream;
	(*kvs)["parameter"] = attrs;
	if(subtree->getSize() >= 4){
		string sclass = subtree->getChildNode(2)->getValue();
		(*kvs)["stream_class"] = sclass;

		string val = subtree->getChildNode(3)->toSQLString();
		(*kvs)["val"] = val;
	}
	if(subtree->getSize() == 6){
		string sclass = subtree->getChildNode(4)->getValue();
		(*kvs)["stream_fix"] = sclass;

		string val = subtree->getChildNode(5)->toSQLString();
		(*kvs)["val_fix"] = val;
	}
	(*kvs)["master"] = master;
	graph->addNode(create);

	return create;
}

/**
 * TABLE_ATTR_LISTノードを変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/06
 * @param	subtree	変換対象の部分木へのポインタ
 * @return	変換後の文字列
*/
string OperatorTreeGenerator::analyzeTableAttrList(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::TABLE_ATTR_LIST)
		throw new ParseException("input node is not TABLE_ATTR_LIST");

	string rval = "";

	for(int i=0; i < subtree->getSize() && i+1 < subtree->getSize(); i+=2){
		rval += subtree->getChildNode(i)->getValue() + " " + subtree->getChildNode(i+1)->getValue();
		if(i+2 < subtree->getSize())
			rval += ", ";
	}
	return rval;
}

/**
 * DROP_STREAMノードを変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/06
 * @param	subtree	変換対象の部分木へのポインタ
 * @return	変換後のオペレータグラフへのポインタ
*/
Node* OperatorTreeGenerator::analyzeDropStream(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::DROP_STREAM)
		throw new ParseException("input node is not DROP_STREAM");
	if(subtree->getSize() != 1)
		throw new ParseException("the size of DROP_STREAM's children is not 1");

	ParseTreeNode* identifier = subtree->getChildNode(0);

	Node* drop = new Node("n" + to_string(ncount++));
	DataStore* kvs = drop->getDataStore();
	(*kvs)["operator_type"] = "drop";
	(*kvs)["stream_name"] = identifier->getValue();
	(*kvs)["master"] = master;
	graph->addNode(drop);

	return drop;
}

/**
 * WITH_RECURSIVEノードを変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/17
 * @param	subtree	変換対象の部分木へのポインタ
 * @return	変換後のオペレータグラフへのポインタ
*/
Node* OperatorTreeGenerator::analyzeRecursive(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::WITH_RECURSIVE)
		throw new ParseException("input node is not WITH_RECURSIVE");

	if(subtree->getSize() != 2)
		throw new ParseException("The size of WITH_RECURSIVE must be 2");

	string tableName = subtree->getChildNode(0)->getValue();
	Node* recursive = new Node("n" + to_string(ncount++));
	DataStore* kvs = recursive->getDataStore();
	(*kvs)["operator_type"] = "recursive";
	(*kvs)["parameter"] = "" + tableName;
	(*kvs)["master"] = master;
	graph->addNode(recursive);

	ParseTreeNode* subquery = subtree->getChildNode(1);
	Node* input = nullptr;
	if(subquery->getSymbol() == Symbol::UNION)
		input = analyzeUnion(subquery);
	else if(subquery->getSymbol() == Symbol::SUBQUERY)
		input = analyzeSubQuery(subquery);
	else
		throw new ParseException("The child of WITH_RECURSIVE is inappropriate");

	createEdge(input, recursive);
	return recursive;
}

/**
 * SUBQUERYノードを変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	subtree	変換対象の部分木へのポインタ
 * @return	変換後のオペレータグラフへのポインタ
*/
Node* OperatorTreeGenerator::analyzeSubQuery(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::SUBQUERY)
		throw new ParseException("input node is not SUBQUERY");

	return analyzeSelect(subtree);
}

/**
 * UNIONノードを変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	subtree	変換対象の部分木へのポインタ
 * @return	変換後のオペレータグラフへのポインタ
 */
Node* OperatorTreeGenerator::analyzeUnion(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::UNION)
		throw new ParseException("input node is not UNION");
	if(subtree->getSize() < 2)
		throw new ParseException("the size of UNION's children is less than 2");

	ParseTreeNode* leftChild = subtree->getChildNode(0);
	Symbol l = leftChild->getSymbol();
	if(l != Symbol::SUBQUERY)
		throw new ParseException("child of UNION must be SUBQUERY");
	Node* leftNode = analyzeSubQuery(leftChild);

	Node* uni = new Node("n" + to_string(ncount++));
	DataStore* kvs = uni->getDataStore();
	(*kvs)["operator_type"] = "union";
	(*kvs)["parameter"] = "";
	(*kvs)["master"] = master;
	graph->addNode(uni);

	for(int i=1; i < subtree->getSize(); i++){
		ParseTreeNode* rightChild = subtree->getChildNode(i);
		Symbol r = rightChild->getSymbol();
		if(r != Symbol::SUBQUERY)
			throw new ParseException("child of UNION must be SUBQUERY");
		Node* rightNode = analyzeSubQuery(rightChild);

		createEdge(leftNode, uni);
		createEdge(rightNode, uni);

		if(i + 1 < subtree->getSize()){
			leftNode = uni;

			uni = new Node("n" + to_string(ncount++));
			kvs = uni->getDataStore();
			(*kvs)["operator_type"] = "union";
			(*kvs)["parameter"] = "";
			(*kvs)["master"] = master;
			graph->addNode(uni);
		}

	}

	return uni;
}


/**
 * SELECTノードを変換する
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/09
 * @param	subtree	変換対象の部分木へのポインタ
 * @return	変換後のオペレータグラフへのポインタ
 */
Node* OperatorTreeGenerator::analyzeSelect(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::SUBQUERY)
		throw new ParseException("input node is not SUBQUERY");

	Node* in = analyzeWhere(subtree);

	// case "SELECT *"
	ParseTreeNode* select = subtree->getChildNode(0);
	if(select->getSize()  == 1 && select->getChildNode(0)->getSymbol() == Symbol::ASTERISK){
		return in;
	}

	// otherwise
	for(int i=0; i < select->getChildNode(0)->getSize(); i++){
		ParseTreeNode* child= select->getChildNode(0)->getChildNode(i);
		Symbol cs = child->getSymbol();
		if(cs == Symbol::EXPRESSION_RENAME){
			in = analyzeSelectListExpression(child->getChildNode(1), in);
		}
		else if(cs == Symbol::EXPRESSION){
			in = analyzeSelectListExpression(child, in);
		}
	}

	Node* out = new Node("n" + to_string(ncount++));
	DataStore* kvs = out->getDataStore();
	(*kvs)["operator_type"] = "projection";
	(*kvs)["parameter"] = findColumns(select->getChildNode(0));
	(*kvs)["master"] = master;

	graph->addNode(out);
	createEdge(in, out);

	for(int i=0; i < select->getChildNode(0)->getSize(); i++){
		ParseTreeNode* child= select->getChildNode(0)->getChildNode(i);
		Symbol s = child->getSymbol();
		if(s == Symbol::EXPRESSION_RENAME){
			Node* rename = new Node("n" + to_string(ncount++));
			DataStore* r = rename->getDataStore();
			(*r)["operator_type"] = "renameColumn";
			string oldname = analyzeExpression(child->getChildNode(1));
			string newname = child->getChildNode(0)->getValue();
			(*r)["parameter"] = "" + oldname + " as " + newname;
			(*r)["master"] = master;

			graph->addNode(rename);
			createEdge(out, rename);
			out = rename;
		}
	}

	return out;
}

/**
 * SELECT_LISTの下のEXPRESSIONノードを変換する
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/16
 * @param	subtree	変換対象の部分木へのポインタ
 * @param	in 入力ノードへのポインタ
 * @return	変換後のオペレータグラフへのポインタ
 */
Node* OperatorTreeGenerator::analyzeSelectListExpression(ParseTreeNode* subtree, Node* in)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::EXPRESSION)
		throw new ParseException("input node is not EXPRESSION");
	if(subtree->getSize() != 1)
		throw new ParseException("input node is not EXPRESSION");

	ParseTreeNode* child = subtree->getChildNode(0);
	Symbol c = child->getSymbol();

	if(c == Symbol::IDENTIFIER || c == Symbol::NUMBER || c == Symbol::STRING){
		return in;
	}
	else if(c == Symbol::FUNCTION){
		if(child->getSize() != 2)
			throw new ParseException("The size of children of FUNCTION  must be 2");

		string functionName = child->getChildNode(0)->getValue();
		ParseTreeNode* arguments = child->getChildNode(1);

		for(int i=0; i < arguments->getSize(); i++){
			in = analyzeSelectListExpression(arguments->getChildNode(i), in);
		}

		Node* eval = new Node("n" + to_string(ncount++));
		DataStore* e = eval->getDataStore();
		(*e)["operator_type"] = "eval";
		(*e)["parameter"] = functionName + "(" + findColumns(arguments) + ")";
		(*e)["master"] = master; 
		graph->addNode(eval);

		createEdge(in, eval);
		return eval;
	}

	return nullptr;
}



/**
 * SELECT_LISTノードを文字列に変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	subtree	変換対象の部分木へのポインタ
 * @return	変換後の文字列
 */
string OperatorTreeGenerator::findColumns(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::SELECT_LIST && s != Symbol::EXPRESSION_LIST)
		throw new ParseException("input node is not SELECT_LIST, EXPRESSION_LIST");

	string rval = "";

	for(int i=0; i < subtree->getSize(); i++){
		ParseTreeNode* child = subtree->getChildNode(i);
		Symbol s = child->getSymbol();

		if(s == Symbol::EXPRESSION)
			rval += analyzeExpression(child);
		else if(s == Symbol::EXPRESSION_RENAME)
			rval += analyzeExpression(child->getChildNode(1));

		if(i+1 < subtree->getSize())
			rval += ",";
	}
	return rval;
}


/**
 * WHEREノードを変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	subtree	変換対象の部分木へのポインタ
 * @return	変換後のオペレータグラフへのポインタ
 */
Node* OperatorTreeGenerator::analyzeWhere(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::SUBQUERY)
		throw new ParseException("input node is not SUBQUERY");

	Node* from = analyzeFrom(subtree->getChildNode(1));
	
	if(subtree->getSize() == 3){
		ParseTreeNode* where = subtree->getChildNode(2);
		string param = analyzePredicateList(where->getChildNode(0), from);

		Node* selection = new Node("n"+ to_string(ncount++));
		DataStore* kvs = selection->getDataStore();
		(*kvs)["operator_type"] = "selection";
		(*kvs)["parameter"] = param;
		(*kvs)["master"] = master;
		graph->addNode(selection);

		createEdge(from, selection);
		return selection;
	}
	else
		return from;
}

/**
 * PREDICATE_LISTノードを変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	subtree	変換対象の部分木へのポインタ
 * @param	topNode[in,out]	先頭ノードへの参照
 * @return	変換後の文字列
 */
string OperatorTreeGenerator::analyzePredicateList(ParseTreeNode* subtree, Node* &topNode)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::PREDICATE_LIST)
		throw new ParseException("input node is not PREDICATE_LIST");

	string rval = "";

	for(int i=0; i < subtree->getSize(); i++){
		rval += analyzePredicate(subtree->getChildNode(i), topNode);
		if(i+1 < subtree->getSize())
			rval += " AND ";
	}
	return rval;
}

/**
 * PREDICATEノードを変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	subtree	変換対象の部分木へのポインタ
 * @param	topNode[in,out]	先頭ノードへの参照
 * @return	変換後の文字列
 */
string OperatorTreeGenerator::analyzePredicate(ParseTreeNode* subtree, Node* &topNode)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::PREDICATE)
		throw new ParseException("input node is not PREDICATE");

	string rval = "";
	rval += analyzeExpression(subtree->getChildNode(0), topNode);
	rval += analyzeComparator(subtree->getChildNode(1));
	rval += analyzeExpression(subtree->getChildNode(2), topNode);

	return rval;
}


/**
 * EXPRESSIONノードを変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	subtree	変換対象の部分木へのポインタ
 * @return	変換後の文字列
 */
string OperatorTreeGenerator::analyzeExpression(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::EXPRESSION)
		throw new ParseException("input node is not EXPRESSION");

	ParseTreeNode* child = subtree->getChildNode(0);
	Symbol c = child->getSymbol();
	if(c == Symbol::EXPRESSION)
		return analyzeExpression(child);
	else if(c == Symbol::FUNCTION)
		return analyzeFunction(child);
	else if(child->isLeafNode())
		return child->getValue();
	else
		return "";
}

/**
 * EXPRESSIONノードを変換する.(Node生成あり)
 *
 * @author	Nagoya University
 * @date	2019/06/25
 * @param	subtree	変換対象の部分木へのポインタ
 * @param	topNode[in,out]	先頭ノードへの参照
 * @return	変換後の文字列
 */
string OperatorTreeGenerator::analyzeExpression(ParseTreeNode* subtree, Node* &topNode)
{
	Symbol s = subtree->getSymbol();
	if (s != Symbol::EXPRESSION)
		throw new ParseException("input node is not EXPRESSION");

	ParseTreeNode* child = subtree->getChildNode(0);
	Symbol c = child->getSymbol();
	if (c == Symbol::EXPRESSION)
		return analyzeExpression(child, topNode);
	else if (c == Symbol::FUNCTION)
		return analyzeFunction(child, topNode);
	else if (child->isLeafNode())
		return child->getValue();
	else
		return "";
}

/**
 * COMPARATORノードを変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	subtree	変換対象の部分木へのポインタ
 * @return	変換後の文字列
 */
string OperatorTreeGenerator::analyzeComparator(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::COMPARATOR)
		throw new ParseException("input node is not COMPARATOR");
	return subtree->getValue();
}
	
/**
 * FUNCTIONノードを変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	subtree	変換対象の部分木へのポインタ
 * @return	変換後の文字列
 */
string OperatorTreeGenerator::analyzeFunction(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::FUNCTION)
		throw new ParseException("input node is not FUNCTION");

	string rval = subtree->getChildNode(0)->getValue();
	rval += "(";
	ParseTreeNode* child = subtree->getChildNode(1);
	rval += analyzeExpressionList(child);
	rval += ")";
	return rval;
}

/**
 * FUNCTIONノードを変換する.(Node生成あり)
 *
 * @author	Nagoya University
 * @date	2019/06/25
 * @param	subtree	変換対象の部分木へのポインタ
 * @param	topNode[in,out]	先頭ノードへの参照
 * @return	変換後の文字列
 */
string OperatorTreeGenerator::analyzeFunction(ParseTreeNode* subtree, Node* &topNode)
{
	Symbol s = subtree->getSymbol();
	if (s != Symbol::FUNCTION)
		throw new ParseException("input node is not FUNCTION");

	string rval = subtree->getChildNode(0)->getValue();
	rval += "(";
	ParseTreeNode* child = subtree->getChildNode(1);
	rval += analyzeExpressionList(child, topNode);
	rval += ")";

	Node* eval = new Node("n" + to_string(ncount++));
	DataStore* e = eval->getDataStore();
	(*e)["operator_type"] = "eval";
	(*e)["parameter"] = rval;
	(*e)["master"] = master;
	graph->addNode(eval);

	createEdge(topNode, eval);
	topNode = eval;

	return rval;
}

/**
 * EXPRESSION_LISTノードを変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	subtree	変換対象の部分木へのポインタ
 * @return	変換後の文字列
 */
string OperatorTreeGenerator::analyzeExpressionList(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::EXPRESSION_LIST)
		throw new ParseException("input node is not EXPRESSION_LIST");

	string rval = "";
	for(int i=0; i < subtree->getSize(); i++){
		ParseTreeNode* child = subtree->getChildNode(i);
		rval += analyzeExpression(child);
		if(i+1 < subtree->getSize())
			rval += ",";
	}
	return rval;
}

/**
 * EXPRESSION_LISTノードを変換する.(Node生成あり) 
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	subtree	変換対象の部分木へのポインタ
 * @param	topNode[in,out]	先頭ノードへの参照
 * @return	変換後の文字列
 */
string OperatorTreeGenerator::analyzeExpressionList(ParseTreeNode* subtree, Node* &topNode)
{
	Symbol s = subtree->getSymbol();
	if (s != Symbol::EXPRESSION_LIST)
		throw new ParseException("input node is not EXPRESSION_LIST");

	string rval = "";
	for (int i = 0; i < subtree->getSize(); i++) {
		ParseTreeNode* child = subtree->getChildNode(i);
		rval += analyzeExpression(child, topNode);
		if (i + 1 < subtree->getSize())
			rval += ",";
	}
	return rval;
}

/**
 * FROMノードを変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	subtree	変換対象の部分木へのポインタ
 * @return	変換後のオペレータグラフへのポインタ
 */
Node* OperatorTreeGenerator::analyzeFrom(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::FROM)
		throw new ParseException("input node is not FROM");

	return analyzeFromList(subtree->getChildNode(0));
}


/**
 * FROM_LISTノードを変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	subtree	変換対象の部分木へのポインタ
 * @return	変換後のオペレータグラフへのポインタ
 */
Node* OperatorTreeGenerator::analyzeFromList(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::FROM_LIST)
		throw new ParseException("input node is not FROM_LIST");
	if(subtree->getSize() < 1)
		throw new ParseException("the size of FROM_LIST's children is less than 1");

	ParseTreeNode* lchild = subtree->getChildNode(0);
	Node* lnode;
	if(lchild->getSymbol() == Symbol::TABLE){
		lnode = analyzeTable(lchild);
	}
	else{
		lnode = analyzeTableRename(lchild);
	}

	for(int i=1; i < subtree->getSize(); i++){
		ParseTreeNode* rchild = subtree->getChildNode(i);
		Node* rnode;
		if(rchild->getSymbol() == Symbol::TABLE_RENAME)
			rnode = analyzeTableRename(rchild);
		else
			rnode = analyzeTable(rchild);

		Node* join = new Node("n" + to_string(ncount++));
		DataStore* kvs = join->getDataStore();
		(*kvs)["operator_type"] = "join";
		(*kvs)["parameter"] = "";
		(*kvs)["master"] = master;

		graph->addNode(join);
		
		createEdge(lnode, join);
		createEdge(rnode, join);
		lnode = join;
	}

	return lnode;
}

/**
 * TABLE_RENAMEノードを変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	subtree	変換対象の部分木へのポインタ
 * @return	変換後のオペレータグラフへのポインタ
 */
Node* OperatorTreeGenerator::analyzeTableRename(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::TABLE_RENAME)
		throw new ParseException("input node is not TABLE_RENAME");
	if(subtree->getSize() < 2)
		throw new ParseException("the size of TABLE_RENAME's children is less than 2");

	string newname = subtree->getChildNode(0)->getValue();
	Node* in = analyzeTable(subtree->getChildNode(1));

	list<string> oldnames = findTableNames(subtree->getChildNode(1));
	Node* rename = nullptr;
	DataStore* kvs = nullptr;

	for(auto oldname : oldnames){
		rename = new Node("n" + to_string(ncount++));
		kvs = rename->getDataStore();
		(*kvs)["operator_type"] = "renameTable";
		(*kvs)["parameter"] = "" + oldname + " as " + newname;
		(*kvs)["master"] = master;

		graph->addNode(rename);
		createEdge(in, rename);
		in = rename;
	}

	return rename;
}

/**
 * 部分木の入力テーブル名のリストを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	subtree	変換対象の部分木へのポインタ
 * @return	テーブル名の文字列のリスト
 */
list<string> OperatorTreeGenerator::findTableNames(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	list<string> rval;

	if(subtree->isLeafNode())
		;
	else if(s == Symbol::TABLE && subtree->getSize() > 0 && subtree->getChildNode(0)->isLeafNode())
		rval.push_back(subtree->getChildNode(0)->getValue());
	else if(s == Symbol::TABLE_RENAME && subtree->getSize() > 0 && subtree->getChildNode(0)->isLeafNode())
		rval.push_back(subtree->getChildNode(0)->getValue());
	else {
		for(int i=0; i < subtree->getSize(); i++)
			rval.merge(findTableNames(subtree->getChildNode(i)));
	}
	return rval;
}



/**
 * TABLEノードを変換する.
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/07
 * @param	subtree	変換対象の部分木へのポインタ
 * @return	変換後のオペレータグラフへのポインタ
 */
Node* OperatorTreeGenerator::analyzeTable(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s != Symbol::TABLE)
		throw new ParseException("input node is not TABLE");

	if(subtree->getSize() < 1)
		throw new ParseException("the size of TABLE's children is less than 1");

	// case: subquery
	if(subtree->getChildNode(0)->getSymbol() == Symbol::SUBQUERY)
		return analyzeSubQuery(subtree->getChildNode(0));

	// case: normal table (RDB or stream)
	Node* rval = new Node("n" + to_string(ncount++));
	DataStore* kvs = rval->getDataStore();
	(*kvs)["operator_type"] = "in";
	(*kvs)["master"] = master;

	// テーブル名
	string name = subtree->getChildNode(0)->getValue();
	(*kvs)["parameter"] = name;
	
	Symbol symbol = subtree->getChildNode(1)->getSymbol();
	if(subtree->getSize() == 1){
		Node* tableNode = nullptr;
		list<Node*>* nlist = graph->getNodes();

		// Recursiveの外側用に同じ名前を持つrecursiveオペレータがないかをチェックする
		for(Node* n : *nlist){
			DataStore* d = n->getDataStore();
			if((*d)["operator_type"] == "recursive" && (*d)["parameter"].size() >= name.size() && (*d)["parameter"].substr(0, name.size()) == name){
				tableNode = n;
				break;
			}
		}
		if(tableNode != nullptr){ // 既存のものがあった場合は再利用
			return tableNode;
		}
		// Recursiveの内側用に同じ名前を持つinオペレータがないかをチェックする
		for(Node* n : *nlist){
			DataStore* d = n->getDataStore();
			if((*d)["operator_type"] == "in" && (*d)["parameter"] == name){
				tableNode = n;
				break;
			}
		}
		if(tableNode != nullptr){ // 既存のものがあった場合は再利用
			return tableNode;
		}
		else {
			(*kvs)["operator_type"] = "RDB";
			(*kvs)["parameter"] = "SELECT * FROM " + name;
			graph->addNode(rval);
			return rval;
		}
	}
	else if(subtree->getSize() == 2 || symbol == Symbol::PARTITION){
		ParseTreeNode* window;
		ParseTreeNode* wtype;
		ParseTreeNode* partition = nullptr;
		if (symbol == Symbol::PARTITION) {
			window = subtree->getChildNode(2);
			wtype = window->getChildNode(0);
			partition = subtree->getChildNode(1);
		} else {
			window = subtree->getChildNode(1);
			wtype = window->getChildNode(0);
		}

		if(QueryParser::iequals(wtype->getValue(), "ROWS")){
			ParseTreeNode* number = window->getChildNode(1);
			(*kvs)["range_window"] = number->getValue();
			if (window->getSize() == 6) {
				// rows 3 sort by distnace_cost limit 2 seed 1
				ParseTreeNode* limit_number = window->getChildNode(3);
				ParseTreeNode* seed_number = window->getChildNode(5);
				(*kvs)["limit"] = limit_number->getValue();
				(*kvs)["seed"] = seed_number->getValue();
			}
		}
		else if(QueryParser::iequals(wtype->getValue(), "RANGE")){
			ParseTreeNode* number = window->getChildNode(1);
			ParseTreeNode* unit = window->getChildNode(2);
			(*kvs)["time_window"] = number->getValue() + " " + unit->getValue();
		}
		if (partition != nullptr) {
			(*kvs)["partition"] = findColumns(partition->getChildNode(1));
		}
		graph->addNode(rval);
		return rval;
	}
	else if(subtree->getSize() == 3){
		ParseTreeNode* window = subtree->getChildNode(1);
		ParseTreeNode* wtype = window->getChildNode(0);
		ParseTreeNode* hierarchy = subtree->getChildNode(2);

		if(QueryParser::iequals(wtype->getValue(), "ROWS")){
			ParseTreeNode* number = window->getChildNode(1);
			(*kvs)["range_window"] = number->getValue();
		}
		else if(QueryParser::iequals(wtype->getValue(), "RANGE")){
			ParseTreeNode* number = window->getChildNode(1);
			ParseTreeNode* unit = window->getChildNode(2);
			(*kvs)["time_window"] = number->getValue() + " " + unit->getValue();
		}

		if(QueryParser::iequals(hierarchy->getChildNode(0)->getValue(), "OWN")){
			graph->addNode(rval);
			return rval;
		}
		else {
			// OWN以外のhierarchyが指定されている場合
			Graph* graphX = new Graph("g" + to_string(gcount++));

			// 依頼するオペレータツリーを生成
			Node* transfer = new Node("n" + to_string(ncount++));
			string tempStream = "temp_MNGID_" + graphX->getID() + "_" + transfer->getID();
			DataStore* tds = transfer->getDataStore();
			(*tds)["operator_type"] = "transfer";
			(*tds)["parameter"] = "own," + tempStream;
			(*tds)["master"] = master;

			Node* in = new Node("n" + to_string(ncount++));;
			DataStore* ids = in->getDataStore();
			(*ids)["operator_type"] = "in";
			(*ids)["parameter"] = (*kvs)["parameter"];
			(*ids)["range_window"] = "1";
			if(master == "")
				master = tempStream;
			(*ids)["master"] = (*kvs)["parameter"];

			(*kvs)["parameter"] = tempStream + "," + (*kvs)["parameter"];

			graphX->addNode(transfer);
			graphX->addNode(in);

			Edge* edge = new Edge("e" + to_string(ecount++), in, transfer);
			graphX->addEdge(edge);

			// 依頼オペレータの生成
			Node* request = new Node("n" + to_string(ncount++));
			DataStore* rds = request->getDataStore();
			(*rds)["operator_type"] = "request";
			string parameter = hierarchy->getChildNode(0)->getValue() + "," + graphX->getID();

			// NEARBYの範囲指定オプション
			if (QueryParser::iequals(hierarchy->getChildNode(0)->getValue(), "NEARBY") && hierarchy->getSize() > 1)
				parameter.append("," + hierarchy->getChildNode(1)->getValue());
			
			(*rds)["parameter"] = parameter;

			graph->addNode(rval);
			graph->addNode(request);
			
			graphList.push_back(graphX);

			return rval;
		}
	}
	return rval;
}


/**
 * 渡された部分木がRDBだけで構成されているかをチェックする.
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/08
 * @param	subtree	対象の部分木へのポインタ
 * @return	部分木がRDBへの処理だけで構成されている場合は true
 */
bool OperatorTreeGenerator::isRDB(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s == Symbol::TABLE && subtree->getChildNode(0)->getSymbol()==Symbol::IDENTIFIER)
		return subtree->getSize() == 1;
	else if(s == Symbol::DROP_STREAM || s==Symbol::CREATE_STREAM)
		return false;
	else {
		bool flag = true;
		for(int i=0; i < subtree->getSize(); i++){
			if(subtree->getChildNode(i)->isIntermediateNode())
				flag &= isRDB(subtree->getChildNode(i));
		}
		return flag;
	}
}

/**
 * 渡された部分木がStreamだけで構成されているかをチェックする.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	subtree	対象の部分木へのポインタ
 * @return	部分木がStreamへの処理だけで構成されている場合は true
 */
bool OperatorTreeGenerator::isStream(ParseTreeNode* subtree)
{
	Symbol s = subtree->getSymbol();
	if(s == Symbol::TABLE && subtree->getChildNode(0)->getSymbol()==Symbol::IDENTIFIER)
		return subtree->getSize() > 1;
	else {
		bool flag = true;
		for(int i=0; i < subtree->getSize(); i++)
			flag &= isStream(subtree->getChildNode(i));
		return flag;
	}
}

/**
 * FROM_LISTの部分木からRDBだけを抽出する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	fromList	FROM_LISTの部分木へのポインタ
 * @return	RDB処理だけで構成された部分木へのポインタをリスト化したもの
 */
list<ParseTreeNode*> OperatorTreeGenerator::extractRDBs(ParseTreeNode* fromList)
{
	list<ParseTreeNode*> rval;
	Symbol s = fromList->getSymbol();

	if(s == Symbol::FROM_LIST){
		for(int i=0; i < fromList->getSize(); i++){
			if(isRDB(fromList->getChildNode(i)))
				rval.push_back(fromList->getChildNode(i));
		}
	}
	return rval;
}

/**
 * FROM_LISTの部分木からStreamだけを抽出する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	fromList	FROM_LISTの部分木へのポインタ
 * @return	Stream処理だけで構成された部分木へのポインタをリスト化したもの
 */
list<ParseTreeNode*> OperatorTreeGenerator::extractStreams(ParseTreeNode* fromList)
{
	list<ParseTreeNode*> rval;
	Symbol s = fromList->getSymbol();

	if(s == Symbol::FROM_LIST){
		for(int i=0; i < fromList->getSize(); i++){
			if(isStream(fromList->getChildNode(i)))
				rval.push_back(fromList->getChildNode(i));
		}
	}
	return rval;
}

/**
* エッジを生成する.
*
* @author	Nagoya University
* @date	2019/06/27
* @param	from	接続元ノード
* @param	to		接続先ノード
*/
void OperatorTreeGenerator::createEdge(Node* from, Node* to)
{
	DataStore* d = from->getDataStore();
	if (secondNodeSymbol == Symbol::WITH_RECURSIVE && (*d)["operator_type"] == "recursive") {
		(*d)["parameter"] = (*d)["parameter"] + "," + to->getID();
	}
	Edge* edge = new Edge("e" + to_string(ecount++), from, to);
	graph->addEdge(edge);

}


