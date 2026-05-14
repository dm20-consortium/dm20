/* 
 * GraphML.h
 * 
 * グラフデータのXML形式の一つGraphMLを扱うためのライブラリヘッダ．
 */
#ifndef GRAPHML_H

#define GRAPHML_H

#include <string>
#include <list>
#include <map>

using namespace std;

namespace IS {

	/**
	 * AttributeContainerは， Key , Node , Edge などの属性を持つクラスのための親クラス.
	 *
	 * <p>
	 * 属性の管理はmapを継承して実現.
	 * </p>
	 *
	 * @author	Yousuke Watanabe
	 * @date	2018/03/21
	 */
	class AttributeContainer : public map<string,string>{
		public:
			/* コンストラクタ．id属性は必須 */
			AttributeContainer(string id);

			/* デストラクタ */
			virtual ~AttributeContainer();

			/* id属性だけは専用のgetterを用意 */
			string getID();

			/* 属性名と値のペアの集合を文字列化して返す */
			virtual string toString();
	};

	/**
	 * DataStoreは，GraphMLのdataタグを表現するためのクラスで，mapを継承している.
	 *
	 * <p>
	 * nodeタグとedgeタグは子要素としてdataタグを持つことができる．
	 * <p>
	 * 
	 * @author	Yousuke Watanabe
	 * @date	2019/04/07
	 */
	class DataStore : public map<string,string> {
		public:
			/* コンストラクタ */
			DataStore();
			
			/* デストラクタ */
			~DataStore();

			/* <data key="name">value</data>タグの並びの文字列を返す */
			string toString();

		private:
			/* XML出力用に特殊文字を文字参照にエスケープする */
			string escape(string str);
	};

	/**
	 * Keyは，GraphMLのkeyタグに対応するクラス.
	 *
	 * <p>AttributeContainerを継承し，mapのサブクラスでもある.</p>
	 * <pre>
	 * 主な属性は以下の3つが使用可能．
	 * ・"for" : 属性がedge用のものかnode用かを指定．
	 * ・"attr.name" : 属性名．
	 * ・"attr.type" : 属性のデータ型．
	 * </pre>
	 *
	 * @author	Yousuke Watanabe
	 * @date	2018/03/21
	 */
	class Key : public AttributeContainer {
		public:
			/* コンストラクタ */
			Key(string id);

			/* デストラクタ */
			~Key();

			/* <key>タグを文字列として返す */
			string toString() override;
	};

	/**
	 * Nodeは，GraphMLのnodeタグに対応し，グラフデータのノードを表すクラス.
	 *
	 * <p>
	 * AttributeContainerを継承し，mapのサブクラスでもある．
	 * </p>
	 *
	 * @author	Yousuke Watanabe
	 * @date	2018/03/21
	 */
	class Node : public AttributeContainer {
		public:
			/* コンストラクタ */
			Node(string id);

			/* デストラクタ */
			~Node();

			/* <node>タグの子要素の<data>タグに対応するDataStoreを返す */
			DataStore* getDataStore();

			/* <node>タグを文字列として返す */
			string toString() override;

		private:
			/* <node>タグの子要素の<data>タグの情報を保持 */
			DataStore store;

	};

	/**
	 * Edgeは，GraphMLのedgeタグに対応し，グラフデータのエッジを表すクラス.
	 *
	 * <p>
	 * AttributeContainerを継承し，mapのサブクラスでもある．
	 * </p>
	 *
	 * @author	Yousuke Watanabe
	 * @date	2018/03/21
	 */
	class Edge : public AttributeContainer {
		public:
			/* コンストラクタ．id属性，sourceは始点ノード，targetは終点ノード */
			Edge(string id, Node* source, Node* target);

			/* デストラクタ */
			~Edge();

			/* <edge>タグの子要素の<data>タグに対応するDataStoreを返す */
			DataStore* getDataStore();

			/* <edge>タグの始点ノードのidを返す */
			string getSourceNodeID();

			/* <edge>タグの終点ノードのidを返す */
			string getTargetNodeID();

			/* <edge>タグの始点ノードのidをセットする */
			void setSourceNode(Node* source);

			/* <edge>タグの終点ノードのidをセットする */
			void setTargetNode(Node* target);

			/* 始点ノードへの参照を返す．*/
			Node* getSourceNode();

			/* 終点ノードへの参照を返す．*/
			Node* getTargetNode();

			/* <edge>タグの文字列表現を返す */
			string toString() override;

		private:
			/* <edge>タグの子要素の<data>タグの情報を保持 */
			DataStore store;

			/* 始点ノードへの参照 */
			Node* source;

			/* 終点ノードへの参照 */
			Node* target;

	};

	/**
	 * Graphは，GraphMLのgraphタグに対応し，グラフデータ全体を表現するクラス.
	 *
	 * <p>
	 * あるグラフは，キー定義の集合，ノードの集合，エッジの集合からなる．
	 * </p>
	 *
	 * @author	Yousuke Watanabe
	 * @date	2018/03/21
	 */
	class Graph : public AttributeContainer {
		public:
			/* コンストラクタ．idは一意でなければならない */
			Graph(string id);

			/* デストラクタ */
			~Graph();

			/* グラフにKeyを1つ追加する */
			void addKey(Key* k);

			/* グラフにNodeを1つ追加する */
			void addNode(Node* n);

			/* グラフにEdgeを1つ追加する */
			void addEdge(Edge* e);

			/* 引数のidに対応したKeyを返す．なければnullptr */
			Key* getKey(string kid);

			/* 引数のidに対応したNodeを返す．なければnullptr */
			Node* getNode(string nid);

			/* 引数のidに対応したEdgeを返す．なければnullptr */
			Edge* getEdge(string eid);

			/* 引数のidに対応したKeyが存在すればtrue，なければfalse */
			bool containsKey(string kid);

			/* 引数のidに対応したNodeが存在すればtrue，なければfalse */
			bool containsNode(string nid);

			/* 引数のidに対応したEdgeが存在すればtrue，なければfalse */
			bool containsEdge(string eid);

			/* グラフ内の全Keyへの参照のリストを返す */
			list<Key*>* getKeys();

			/* グラフ内の全Nodeへの参照のリストを返す */
			list<Node*>* getNodes();

			/* グラフ内の全Edgeへの参照のリストを返す */
			list<Edge*>* getEdges();

			/* Edgeの中で，引数のNodeを終点にしているものへの参照のリストを返す */
			list<Edge*>* getInputEdges(Node* n);

			/* Edgeの中で，引数のNodeを始点にしているものへの参照のリストを返す */
			list<Edge*>* getOutputEdges(Node* n);

			/* <graphml><graph>タグの形式で文字列を出力する */
			string toString() override;

			/* stringデータから GraphMLデータを読み込み，Graphを生成する．*/
			static Graph* parse(string input);

		private:
			/* idとKeyへの参照の対応を管理するmap */
			map<string, Key*> keys;

			/* idとNodeへの参照の対応を管理するmap */
			map<string, Node*> nodes;

			/* idとEdgeへの参照の対応を管理するmap */
			map<string, Edge*> edges;
	};

	/**
	 * GraphMLExceptionは，GraphMLに関する例外を通知するためのクラス.
	 *
	 * @author	Yousuke Watanabe
	 * @date	2018/03/21
	 */
	class GraphMLException : public exception {
		public:
			/* コンストラクタ．引数はエラーメッセージ */
			GraphMLException(string msg);

			/* エラーメッセージの文字列 */
			string getMessage();

		private:
			/* エラーメッセージの文字列を保持 */
			string message;
	};
}

#endif
