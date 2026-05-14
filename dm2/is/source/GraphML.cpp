/* 
 * GraphML.cpp
 * 
 * グラフデータのXML形式の一つGraphMLを扱うためのライブラリの実装．
 * XMLの解析にはApache Xercesを使用．
 */

/* クラス定義ヘッダファイル */
#include "GraphML.h"

/* 依存ライブラリ */
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <iostream>

using namespace IS;
using namespace xercesc;

/*
 * クラス AttributeContainer
 */

/**
 * コンストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @param	id	属性ID(必須)
 */
AttributeContainer::AttributeContainer(string id) : map::map<string,string>()
{
	this->operator[]("id") = id;
}

/**
 * デストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 */
AttributeContainer::~AttributeContainer() 
{
	this->clear();
}

/**
 * id属性のための専用のgetter.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @return	ID
 */
string AttributeContainer::getID()
{
	return this->operator[]("id");
}

/**
 * 属性名と値のペアの集合を文字列化して返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @return	属性と値をペアにした文字列
 */
string AttributeContainer::toString()
{
	string rval = "id=\"" + getID() + "\" ";
	for(pair<string, string> p : *this){
		if(p.first != "id")
			rval += p.first + "=\"" + p.second + "\" ";
	}
	return rval;
}


/*
 * クラス DataStore
 */

/**
 * コンストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 */
DataStore::DataStore() : map::map<string,string>()
{
	;
}

/**
 * デストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 */
DataStore::~DataStore()
{
	this->clear();
}

/**
 * dataタグの並びの文字列を返す.
 *
 * 以下の形式の繰返しで文字列に変換する．
 * <pre>
 * <data key="name">value</data>
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 */
string DataStore::toString()
{
	string rval = "";
	for(pair<string,string> p : *this){
		rval += "<data key=\"" + p.first + "\">" + escape(p.second) + "</data>\n";
	}
	return rval;
}

/**
 * XML出力用に特殊文字を文字参照にエスケープする.
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/07
 */
string DataStore::escape(string str)
{
	string rval = "";
	for(unsigned int i=0; i < str.size(); i++){
		if(str[i] == '<')
			rval += "&lt;";
		else if(str[i] == '>')
			rval += "&gt;";
		else if(str[i] == '&')
			rval += "&amp;";
		else if(str[i] == '"')
			rval += "&quot;";
		else
			rval += str[i];
	}
	return rval;
}


/*
 * クラス Key
 */

/**
 * コンストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @param	id	属性ID(必須)
 */
Key::Key(string id) : AttributeContainer::AttributeContainer(id)
{
	;
}

/**
 * デストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 */
Key::~Key()
{
	this->clear();
}

/**
 * GraphMLのkeyタグを文字列として返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @return	keyタグの文字列
 */
string Key::toString()
{
	return "<key " + AttributeContainer::toString() + "/>";
}

/*
 * クラス Node
 */

/**
 * コンストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @param	id	属性ID(必須)
 */
Node::Node(string id): AttributeContainer::AttributeContainer(id)
{
	;
}

/**
 * デストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 */
Node::~Node()
{
	this->clear();
}

/**
 * nodeタグの子要素のdataタグに対応するDataStoreを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @return	子要素のdataタグに対応するDataStoreクラスのインスタンス
 */
DataStore* Node::getDataStore()
{
	return &store;
}

/**
 * GraphMLのnodeタグの文字列として返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @return	GraphMLのnodeタグの文字列
 */
string Node::toString()
{
	string rval = "<node " + AttributeContainer::toString() + ">";
	rval += store.toString();
	rval += "</node>\n";

	return rval;
}


/*
 * クラス Edge
 */


/**
 * コンストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @param	id	属性ID(必須)
 * @param	source	始点ノード(必須)
 * @param	target	終点ノード(必須)
 */
Edge::Edge(string id, Node* source, Node* target): AttributeContainer::AttributeContainer(id)
{
	this->source = source;
	if(source != nullptr)
		this->operator[]("source") = source->getID();

	this->target = target;
	if(target != nullptr)
		this->operator[]("target") = target->getID();
}

/**
 * デストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 */
Edge::~Edge()
{
	this->store.~DataStore();
	this->clear();
	this->source = nullptr;
	this->target = nullptr;
}

/**
 * edgeタグの子要素のdataタグに対応するDataStoreを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @return	子要素のdataタグに対応するDataStoreクラスのインスタンス
 */
DataStore* Edge::getDataStore()
{
	return &store;
}

/**
 * edgeタグの始点ノードのidを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @return	始点ノード
 */
string Edge::getSourceNodeID()
{
	return this->operator[]("source");
}

/**
 * edgeタグの終点ノードのidを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @return	終点ノード
 */
string Edge::getTargetNodeID()
{
	return this->operator[]("target");
}

/**
 * edgeタグの始点ノードのidをセットする.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @param	source	始点ノード
 */
void Edge::setSourceNode(Node* source)
{
	this->source = source;
}

/**
 * edgeタグの終点ノードのidをセットする.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @param	target	終点ノード
 */
void Edge::setTargetNode(Node* target)
{
	this->target = target;
}

/**
 * 始点ノードへのポインタを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @return	始点ノードへのポインタ
 */
Node* Edge::getSourceNode()
{
	return source;
}

/**
 * 終点ノードへのポインタを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @return	終点ノードへのポインタ
 */
Node* Edge::getTargetNode()
{
	return target;
}

/**
 * GraphMLのedgeタグの文字列表現を返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @return	edgeタグの文字列
 */
string Edge::toString()
{
	string rval = "<edge " + AttributeContainer::toString() + ">";
	rval += store.toString();
	rval += "</edge>\n";

	return rval;
}


/*
 * クラス Graph
 */

/**
 * コンストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @param	id	属性ID(必須)
 */
Graph::Graph(string id) : AttributeContainer::AttributeContainer(id)
{
	this->operator[]("edgedefault") = "undirected";
}

/**
 * デストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 */
Graph::~Graph()
{
	this->clear();
	keys.clear();
	nodes.clear();
	edges.clear();
}

/**
 * グラフにKeyを1つ追加する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @param	k	Keyクラスのインスタンスへのポインタ
 */
void Graph::addKey(Key* k)
{
	keys[k->getID()] = k;
}

/**
 * グラフにNodeを1つ追加する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @param	n	Nodeクラスのインスタンスへのポインタ
 */
void Graph::addNode(Node* n)
{
	DataStore* kvs = n->getDataStore();
	(*kvs)["nid"] = n->getID();
	nodes[n->getID()] = n;
}

/**
 * グラフにEdgeを1つ追加する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @param	e	Edgeクラスのインスタンスへのポインタ
 */
void Graph::addEdge(Edge* e)
{
	edges[e->getID()] = e;
}

/**
 * 引数のidに対応したKeyを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @param	kid	keyタグについたIDの文字列
 * @return	kidに対応したKeyクラスのインスタンスへのポインタ．対応するKeyがなければnullptrを返す.
 */
Key* Graph::getKey(string kid)
{
	if(keys.count(kid) > 0)
		return keys[kid];
	else
		return nullptr;
}

/**
 * 引数のidに対応したNodeを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @param	nid	nodeタグについたIDの文字列
 * @return	nidに対応したNodeクラスのインスタンスへのポインタ．対応するNodeがなければnullptrを返す.
 */
Node* Graph::getNode(string nid)
{
	if(nodes.count(nid) > 0)
		return nodes[nid];
	else
		return nullptr;
}

/**
 * 引数のidに対応したEdgeを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @param	eid	edgeタグについたIDの文字列
 * @return	eidに対応したEdgeクラスのインスタンスへのポインタ．対応するEdgeがなければnullptrを返す.
 */
Edge* Graph::getEdge(string eid)
{
	if(edges.count(eid) > 0)
		return edges[eid];
	else
		return nullptr;
}

/**
 * 引数のidに対応したKeyが存在すればtrue，なければfalseを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @param	kid	keyタグについたIDの文字列
 * @return	kidに対応するKeyがあればtrue,なければfalse
 */
bool Graph::containsKey(string kid)
{
	return keys.count(kid) > 0;
}

/**
 * 引数のidに対応したNodeが存在すればtrue，なければfalseを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @param	nid	nodeタグについたIDの文字列
 * @return	nidに対応するNodeがあればtrue,なければfalse
 */
bool Graph::containsNode(string nid)
{
	return nodes.count(nid) > 0;
}

/**
 * 引数のidに対応したEdgeが存在すればtrue，なければfalseを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @param	eid	edgeタグについたIDの文字列
 * @return	eidに対応するEdgeがあればtrue,なければfalse
 */
bool Graph::containsEdge(string eid)
{
	return edges.count(eid) > 0;
}

/**
 * グラフ内の全Keyへのポインタのリストを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @return	Keyへのポインタを要素とするリストへのポインタ
 */
list<Key*>* Graph::getKeys()
{
	list<Key*>* rval = new list<Key*>();
	for(pair<string, Key*> p : keys)
		rval->push_back(p.second);
	return rval;
}

/**
 * グラフ内の全Nodeへのポインタのリストを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @return	Nodeへのポインタを要素とするリストへのポインタ
 */
list<Node*>* Graph::getNodes()
{
	list<Node*>* rval = new list<Node*>();
	for(pair<string, Node*> p : nodes)
		rval->push_back(p.second);
	return rval;
}

/**
 * グラフ内の全Edgeへのポインタのリストを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @return	Edgeへのポインタを要素とするリストへのポインタ
 */
list<Edge*>* Graph::getEdges()
{
	list<Edge*>* rval = new list<Edge*>();
	for(pair<string, Edge*> p : edges)
		rval->push_back(p.second);
	return rval;
}

/**
 * Edgeの中で，引数のNodeを終点にしているものへのポインタのリストを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @param	n	Edgeを探すときの条件となるNodeへのポインタ
 * @return	Edgeへのポインタを要素とするリストへのポインタ
 */
list<Edge*>* Graph::getInputEdges(Node* n)
{
	list<Edge*>* rval = new list<Edge*>();
	for(pair<string, Edge*> p : edges){
		if(p.second->getTargetNodeID() == n->getID())
			rval->push_back(p.second);
	}
	return rval;
}

/**
 * Edgeの中で，引数のNodeを始点にしているものへのポインタのリストを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @param	n	Edgeを探すときの条件となるNodeへのポインタ
 * @return	Edgeへのポインタを要素とするリストへのポインタ
 */
list<Edge*>* Graph::getOutputEdges(Node* n)
{
	list<Edge*>* rval = new list<Edge*>();
	for(pair<string, Edge*> p : edges){
		if(p.second->getSourceNodeID() == n->getID())
			rval->push_back(p.second);
	}
	return rval;
}


/**
 * Graphをgraphmlのタグ形式でXMLの文字列を出力する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @return	GraphMLの文字列
 */
string Graph::toString()
{
	string rval;

	// Keyは現状localに展開するオペレータツリーにのみ構成される想定
	for (pair<string, Key*> p : keys)
		rval += p.second->toString() + "\n";

	rval += "<graph " + AttributeContainer::toString() + ">\n";

	for(pair<string, Node*> p : nodes)
		rval += p.second->toString();

	for(pair<string, Edge*> p : edges)
		rval += p.second->toString();

	rval += "</graph>\n";

	return rval;
}


/**
 * stringデータから GraphMLデータを読み込み，Graphを生成する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @param	input	GraphMLの情報を含む文字列
 * @return	文字列を解析結果を格納したGraphへのポインタ
 */
Graph* Graph::parse(string input)
{
	try {
		XMLPlatformUtils::Initialize();
	}
	catch(const XMLException& xe){
		char* message = XMLString::transcode(xe.getMessage());
		cout << message << endl;
		XMLString::release(&message);
		return nullptr;
	}

	XercesDOMParser* parser = new XercesDOMParser();
	ErrorHandler* handler = (ErrorHandler*)new HandlerBase();
	parser->setErrorHandler(handler);

	MemBufInputSource source((const XMLByte*)input.c_str(), input.length(), "memory buffer");

	Graph* rval = nullptr;

	try {
		parser->parse(source);

		DOMDocument* doc = parser->getDocument();

		// Graph Tag
		rval = new Graph("g0"); // g0 is a dummy id. it will be overwritten

		XMLCh *graph = XMLString::transcode("graph");
		DOMNodeList* graphList = doc->getElementsByTagName(graph);
		if(graphList->getLength() < 1 || graphList->getLength() > 1)
			throw new GraphMLException("The document must have only one <graph> element");
		DOMNamedNodeMap* gattr = graphList->item(0)->getAttributes();
		for(unsigned int i=0; i < gattr->getLength(); i++){
			char *c_name = XMLString::transcode(gattr->item(i)->getNodeName());
			char *c_value = XMLString::transcode(gattr->item(i)->getNodeValue());
			string name = string(c_name);
			string value = string(c_value);
			(*rval)[name] = value;
			XMLString::release(&c_name);
			XMLString::release(&c_value);
		}
		XMLString::release(&graph);

		// Key Tags
		XMLCh *key = XMLString::transcode("key");
		DOMNodeList* keyList = doc->getElementsByTagName(key);
		for(unsigned int i=0; i < keyList->getLength(); i++){
			Key* k = new Key("k0");  // k0 is a dummy id. it will be overwritten

			DOMNamedNodeMap* kattr = keyList->item(i)->getAttributes();
			for(unsigned int j=0; j < kattr->getLength(); j++){
				char *c_name = XMLString::transcode(kattr->item(i)->getNodeName());
				char *c_value = XMLString::transcode(kattr->item(i)->getNodeValue());
				string name = string(c_name);
				string value = string(c_value);
				(*k)[name] = value;
				XMLString::release(&c_name);
				XMLString::release(&c_value);
			}
			rval->addKey(k);
		}
		XMLString::release(&key);


		// Node Tags
		XMLCh *node = XMLString::transcode("node");
		DOMNodeList* nodeList = doc->getElementsByTagName(node);
		for(unsigned int i=0; i < nodeList->getLength(); i++){
			Node* n = new Node("n0");  // n0 is a dummy id. it will be overwritten

			DOMNamedNodeMap* nattr = nodeList->item(i)->getAttributes();
			for(unsigned int j=0; j < nattr->getLength(); j++){
				char *c_name = XMLString::transcode(nattr->item(i)->getNodeName());
				char *c_value = XMLString::transcode(nattr->item(i)->getNodeValue());
				string name = string(c_name);
				string value = string(c_value);
				(*n)[name] = value;
				XMLString::release(&c_name);
				XMLString::release(&c_value);
			}

			DOMNodeList* nchild = nodeList->item(i)->getChildNodes();
			for(unsigned int j=0; j < nchild->getLength(); j++){
				char *c_name = XMLString::transcode(nchild->item(j)->getNodeName());
				string name = string(c_name);
				if(name == "data"){
					XMLCh *c_key1 = XMLString::transcode("key");
					char *c_key2 = XMLString::transcode(nchild->item(j)->getAttributes()->getNamedItem(c_key1)->getNodeValue());
					char *c_value = XMLString::transcode(nchild->item(j)->getNodeValue());
					string key = string(c_key2);
					string value = string(c_value);
					n->getDataStore()->operator[](key) = value;
					XMLString::release(&c_key1);
					XMLString::release(&c_key2);
					XMLString::release(&c_value);
				}
				XMLString::release(&c_name);
			}

			rval->addNode(n);
		}
		XMLString::release(&node);

		// Edge Tags
		XMLCh *edge = XMLString::transcode("edge");
		DOMNodeList* edgeList = doc->getElementsByTagName(edge);
		for(unsigned int i=0; i < edgeList->getLength(); i++){
			Edge* e = new Edge("e0", nullptr, nullptr);  // e0 is a dummy id. it will be overwritten

			DOMNamedNodeMap* eattr = edgeList->item(i)->getAttributes();
			for(unsigned int j=0; j < eattr->getLength(); j++){
				char *c_name = XMLString::transcode(eattr->item(i)->getNodeName());
				char *c_value = XMLString::transcode(eattr->item(i)->getNodeValue());
				string name = string(c_name);
				string value = string(c_value);
				(*e)[name] = value;
				XMLString::release(&c_name);
				XMLString::release(&c_value);
			}
			e->setSourceNode(rval->getNode(e->getSourceNodeID()));
			e->setTargetNode(rval->getNode(e->getTargetNodeID()));

			DOMNodeList* echild = edgeList->item(i)->getChildNodes();
			for(unsigned int j=0; j < echild->getLength(); j++){
				char *c_name = XMLString::transcode(echild->item(j)->getNodeName());
				string name = string(c_name);
				if(name == "data"){
					XMLCh *c_key1 = XMLString::transcode("key");
					char *c_key2 = XMLString::transcode(echild->item(j)->getAttributes()->getNamedItem(c_key1)->getNodeValue());
					char *c_value = XMLString::transcode(echild->item(j)->getNodeValue());
					string key = string(c_key2);
					string value = string(c_value);
					e->getDataStore()->operator[](key) = value;
					XMLString::release(&c_key1);
					XMLString::release(&c_key2);
					XMLString::release(&c_value);
				}
				XMLString::release(&c_name);
			}
			rval->addEdge(e);
		}
		XMLString::release(&edge);
	}
	catch(const XMLException& xe){
		char* message = XMLString::transcode(xe.getMessage());
		cout << message << endl;
		XMLString::release(&message);
		rval = nullptr;
	}
	catch(const DOMException& de){
		char* message = XMLString::transcode(de.getMessage());
		cout << message << endl;
		XMLString::release(&message);
		rval = nullptr;
	}
	catch(...){
		cout << "Unexpected exception"<<endl;
		rval = nullptr;
	}
	try {
		XMLPlatformUtils::Terminate();
	}
	catch(const XMLException& xe){
		char* message = XMLString::transcode(xe.getMessage());
		cout << message << endl;
		XMLString::release(&message);
		return nullptr;
	}

	delete handler;
	delete parser;
	return rval;
}


/*
 * クラス GraphMLException
 */

/**
 * コンストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @param	msg	エラーメッセージ
 */
GraphMLException::GraphMLException(string msg)
{
	this->message = msg;
}

/**
 * エラーメッセージの文字列を返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/21
 * @return	エラーメッセージの文字列
 */
string GraphMLException::getMessage()
{
	return message;
}


