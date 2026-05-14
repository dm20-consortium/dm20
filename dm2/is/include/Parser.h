/* 
 * Parser.h
 * 
 * クエリ解析器とその関連クラスのライブラリヘッダ．
 */
#ifndef PARSER_H

#define PARSER_H

#include <string>
#include <list>
#include "GraphML.h"
#include <log4cxx/logger.h>

using namespace std;
using namespace IS;
using namespace log4cxx;

namespace IS {

	/**
	 * 列挙型 Symbolは，文法上の非終端記号，終端記号を表す.
	 *
	 * <p>構文規則は以下の通り:</p>
	 * <pre>
	 * <QUERY> := <MASTER>? <WITH_RECURSIVE>? ( <SUBQUERY> | <UNION> ) <SEMICOLON>?
	 *          | <CREATE_STREAM> <SEMICOLON>?
	 *          | <DROP_STREAM> <SEMICOLON>?
	 *          | <CREATE_TABLE> <SEMICOLON>?
	 *          | <DROP_TABLE> <SEMICOLON>?
	 *          | <INSERT_TABLE> <SEMICOLON>?
	 *          | <UPDATE_TABLE> <SEMICOLON>?
	 *          | <DELETE_TABLE> <SEMICOLON>?
	 * <CREATE_STREAM> := "CREATE" "STREAM" <IDENTIFIER> "(" <TABLE_ATTR_LIST> ")"
	 *                  | "CREATE" "STREAM" <IDENTIFIER> "(" <TABLE_ATTR_LIST> ")" "TIMER_FIX"  "[" <NUMBER> "]"
	 *                  | "CREATE" "STREAM" <IDENTIFIER> "(" <TABLE_ATTR_LIST> ")" "TIMER_FIX"  "[" <NUMBER> "]　"START_FIX" "[" <NUMBER> "]"
	 *                  | "CREATE" "STREAM" <IDENTIFIER> "(" <TABLE_ATTR_LIST> ")" "TIMER_CRON" "[" <DATE_PATTERN> "]"
	 * <CREATE_TABLE> := "CREATE" "TABLE" <IDENTIFIER>  "(" <TABLE_ATTR_LIST> ")"
	 * <UPDATE_TABLE> := "UPDATE" "TABLE" <IDENTIFIER> "SET" <SET_ATTR_LIST> ( <WHERE> )?
	 * <DELETE_TABLE> := "DELETE" "FROM" <IDENTIFER>  ( <WHERE> )?
	 * <TABLE_ATTR_LIST> := <TABLE_ATTR> ( "," <TABLE_ATTR> )*
	 * <TABLE_ATTR> := <IDENTIFIER> <IDENTIFIER> ( "DEFAULT" <VALUE_ATTR> )?
	 * <VALUE_ATTR_LIST> := <VALUE_ATTR> ( "," <VALUE_ATTR> )*
	 * <VALUE_ATTR> := <NUMBER>|<WORDS>|<BOOL_VALUE>|<NULL>
	 * <SET_ATTR_LIST> :=  <SET_ATTR> ( "," <SET_ATTR> )*
	 * <SET_ATTR> := <IDENTIFIER> "=" <EXPRESSION>
	 * <DATE_PATTERN> := (<NUMBER>|"*")(<NUMBER>|"*")(<NUMBER>|"*")(<NUMBER>|"*")(<NUMBER>|"*")
	 * <DROP_STREAM> := "DROP" "STREAM" <IDENTIFIER>
	 * <DROP_TABLE> := "DROP" "TABLE"  <IDENTIFIER>
	 * <INSERT_TABLE> := "INSERT" "INTO" <IDENTIFIER> ( "(" <TABLE_ATTR_LIST> ")" )? "VALUES" "(" <VALUE_ATTR_LIST> ")"
	 *                 | "INSERT" "INTO" <IDENTIFIER> ( "(" <TABLE_ATTR_LIST> ")" )? ( <SUBQUERY> | <UNION> )
	 * <MASTER> := "MASTER" <MASTER_LIST>
	 * <MASTER_LIST> := <IDENTIFIER> ( "," <IDENTIFIER> )*
	 * <IDENTIFIER> := [a-zA-Z][a-zA-Z0-9_]*  (終端記号)
	 * <WITH_RECURSIVE> := "WITH" "RECURSIVE" <IDENTIFIER> <AS> "(" <UNION> ")"
	 * <SUBQUERY> := <SELECT> <FROM> <WHERE>?
	 * <UNION> := <SUBQUERY> ("UNION" <SUBQUERY>)+
	 * <SELECT> := "SELECT"  ( <ASTERISK> | <SELECT_LIST> )
	 * <ASTERISK> := "*" (終端記号)
	 * <SELECT_LIST> := ( <EXPRESSION> | <EXPRESSION_RENAME> ) ( "," ( <EXPRESSION> | <EXPRESSION_RENAME ) )*
	 * <EXPRESSION_LIST> := <EXPRESSION> ( "," <EXPRESSION> )*
	 * <EXPRESSION> := "(" <EXPRESSION> ")" | <NUMBER> | <STRING>| <IDENTIFIER> | <FUNCTION> 
	 * <EXPRESSION_RENAME> := <EXPRESSION> <AS> <IDENTIFIER> 
	 * <FUNCTION> := <IDENTIFIER> "(" <EXPRESSION_LIST>? ")"
	 * <FROM> := "FROM" <FROM_LIST>
	 * <FROM_LIST> := ( &lt;TABLE> | <TABLE_RENAME> ) ( "," ( &lt;TABLE> | <TABLE_RENAME>) )*
	 * &lt;TABLE> := ( <IDENTIFIER> ("[" <WINDOW> ("," <HIERARCHY> )? "]")? ) | ( "(" <SUBQUERY> ")" )
	 * &lt;TABLE_RENAME> := &lt;TABLE> <AS> <IDENTIFIER>
	 * <AS> := "AS" (終端記号)
	 * <WINDOW> := ( ( <PARTITION> ) "ROWS" <NUMBER> ) 
	 *           | ( "RANGE"  <NUMBER> ( "now" | "msec" | "sec" | "min" | "hour" | "day" ) )
	 * <PARTITION> := "PARTITION" "BY" <TABLE_ATTR_LIST>
	 * <HIERARCHY> := "OWN" | "NEARBY" <NUMBER>? | "ALL" | <NUMBER>
	 * <WHERE> := "WHERE" <PREDICATE_LIST>
	 * <PREDICATE_LIST> := <PREDICATE> ( <AND> <PREDICATE> )*
	 * <AND> := "AND" (終端記号)
	 * <PREDICATE> := <EXPRESSION> <COMPARATOR> <EXPRESSION>
	 * <COMPARATOR> := "=" | "<" | ">" | "<=" | ">=" | "<>" | "!=" (終端記号)
	 * <SIGN> := "+" | "-" (終端記号)
	 * </pre>
	 *
	 * @author	Yousuke Watanabe
	 * @date	2019/04/09
	 */
    enum class Symbol {
        QUERY,            /*!< 非終端記号 <QUERY>  */
        CREATE_STREAM,    /*!< 非終端記号 <CREATE_STREAM>  */
        TABLE_ATTR_LIST,  /*!< 非終端記号 <TABLE_ATTR_LIST>  */
		TABLE_ATTR,	      /*!< 非終端記号 <TABLE_ATTR>  */	
		SET_ATTR_LIST,    /*!< 非終端記号 <SET_ATTR_LIST>  */	
		SET_ATTR,	      /*!< 非終端記号 <SET_ATTR>  */	
		VALUE_ATTR_LIST,  /*!< 非終端記号 <VALUE_ATTR_LIST>  */
        DATE_PATTERN,     /*!< 非終端記号 <DATE_PATTERN>  */
        DROP_STREAM,      /*!< 非終端記号 <DROP_STREAM>  */
		DROP_TABLE,       /*!< 非終端記号 <DROP_TABLE>  */
        MASTER,           /*!< 非終端記号 <MASTER> */
        MASTER_LIST,      /*!< 非終端記号 <MASTER_LIST> */
        IDENTIFIER,       /*!< 非終端記号 <IDENTIFIER> */
        FORMULA,          /*!< 非終端記号 <FORMULA> */
		WITH_RECURSIVE,   /*!< 非終端記号 <WITH_RECURSIVE> */
        SUBQUERY,         /*!< 非終端記号 <SUBQUERY> */
		UNION,            /*!< 非終端記号 <UNION> */
        SELECT,           /*!< 非終端記号 <SELECT> */
		ASTERISK,         /*!< 非終端記号 <ASTERISK> */
		SELECT_LIST,      /*!< 非終端記号 <SELECT_LIST> */
		EXPRESSION_LIST,  /*!< 非終端記号 <EXPRESSION_LIST> */
        EXPRESSION,       /*!< 非終端記号 <EXPRESSION> */
		EXPRESSION_RENAME,/*!< 非終端記号 <EXPRESSION_RENAME> */
        FUNCTION,         /*!< 非終端記号 <FUNCTION> */
        FROM,             /*!< 非終端記号 <FROM> */
        FROM_LIST,        /*!< 非終端記号 <FROM_LIST> */
        TABLE,            /*!< 非終端記号 &lt;TABLE> */
        TABLE_RENAME,     /*!< 非終端記号 <TABLE_RENAME> */
        AS,               /*!< 非終端記号 <AS> */
        WINDOW,           /*!< 非終端記号 <WINDOW> */
        PARTITION,        /*!< 非終端記号 <PARTITION> */
        HIERARCHY,        /*!< 非終端記号 <HIERARCHY> */
        WHERE,            /*!< 非終端記号 <WHERE> */
        PREDICATE_LIST,   /*!< 非終端記号 <PREDICATE_LIST> */
        AND,              /*!< 非終端記号 <AND> */
        PREDICATE,        /*!< 非終端記号 <PREDICATE> */
        COMPARATOR,       /*!< 非終端記号 <COMPARATOR> */
		CREATE_TABLE,     /*!< 非終端記号 <CREATE_TABLE>  */
		INSERT_INTO,	  /*!< 非終端記号 <INSERT_INTO> */
   		VALUES,			  /*!< 非終端記号 <VALUES> */
   		UPDATE,			  /*!< 非終端記号 <UPDATE> */
   		SET,			  /*!< 非終端記号 <SET> */
   		DELETE,			  /*!< 非終端記号 <DELETE> */
		DEFAULT,		  /*!< 非終端記号 <DEFAULT> */
		COLUMN_LIST,      /*!< 非終端記号 <COLUMN_LIST> */
   		NULL_VALUE,		  /*!< 終端記号 <NULL> */
   		BOOL_VALUE,		  /*!< 終端記号 <BOOL> */
   		TRUE,			  /*!< 非終端記号 <TRUE> */
   		FALSE,			  /*!< 非終端記号 <FALSE> */
        NUMBER,           /*!< 数字(終端記号) */
        SIGN,             /*!< 符号(終端記号) */
        STRING,           /*!< '文字列'(終端記号) */
        SEMICOLON,        /*!< セミコロン(終端記号) */
        NUMBER_ARRAY,     /*!< [数値,数値](終端記号) */
        STRING_ARRAY,     /*!< ['文字列','文字列'](終端記号) */
        UNKNOWN           /*!< 不明なシンボル */
    };

	/**
	 * Tokenizerは，字句解析を行うクラス.
	 *
	 * <p>
	 * 文字列から文法上の構成単位であるトークンを切りだす．
	 * トークンの列を格納したキューのイメージで操作する．
	 * </p>
	 *
	 * @author	Yousuke Watanabe
	 * @date	2018/03/21
	 */
    class Tokenizer {
        public:

			/* コンストラクタ．targetは解析対象の文字列 */
            Tokenizer(string target);

			/* デストラクタ */
            ~Tokenizer();

			/* 次のトークンがまだある場合はtrue，文字列の最後に達した場合はfalse */
            bool hasMoreTokens();

			/* キューの先頭にトークンを置く */
            void pushToken(string token);

			/* キューの先頭のトークンを読む(先頭を除去しない) */
            string lookAheadToken();

			/* キューの先頭のトークンを取得する(先頭のトークンはキューから除去される) */
            string getToken();

			/* キューの先頭のトークンと，引数文字列を比較した結果を返す．一致した場合だけ，先頭が除去される． */
            bool compareToken(string expected);

			/* キューの先頭のトークンと，引数シンボルを比較した結果を返す．一致した場合だけ，先頭が除去される． */
            bool compareSymbol(Symbol expected);

        private:

			/* 解析後のトークンの列を保持するキュー */
            list<string> tokens;

			/* 文字列をトークンに分割する */
            void tokenize(string str);

			/* 引数の文字が空白文字かどうか */
            bool isWhitespace(char ch);

			/* 引数の文字が括弧やカンマなどの構造を表す文字かどうか */
            bool isStructureSymbol(char ch);

			/* 引数の文字が<や>などの比較演算の記号かどうか */
            bool isComparatorSymbol(char ch);

			/* 引数の文字が，文字列の開始や終了を意味する ' であるか */
            bool isStringSymbol(char ch);

			/* 引数の文字が算術演算の記号かどうか */
            bool isArithmeticSymbol(char ch);

			/* 引数の文字がセミコロンかどうか */
			bool isSemicolon(char ch);
    };

	/**
	 * ParseTreeNodeは，構文解析結果の木構造を表現するクラスの親クラス.
	 *
	 * <p>
	 * 非終端ノード(IntermediateNode)と終端ノード(LeafNode)の2種類のサブクラスを持つ．
	 * </p>
	 *
	 * @author	Yousuke Watanabe
	 * @date	2018/03/21
	 */
    class ParseTreeNode {
        public: 
			/* デストラクタ */
			virtual ~ParseTreeNode();

			/* シンボルと文字列の変換をするクラスメソッド */
            static string convertSymbolToString(Symbol s);

			/* ノードのシンボルを返す (抽象メソッド) */
            virtual Symbol getSymbol() = 0; 

			/* ノードの子要素の数を返す (抽象メソッド) */
            virtual int getSize() = 0;

			/* ノードが終端ノードかどうかを返す (抽象メソッド) */
            virtual bool isLeafNode() = 0;

			/* ノードが非終端ノードかどうかを返す (抽象メソッド) */
            virtual bool isIntermediateNode() = 0;

			/* ノードが終端ノードの場合，値を返す.非終端の場合はtoStringと同じ結果 (抽象メソッド) */
            virtual string getValue() = 0;

			/* ノードが非終端ノードの場合，指定した位置の子要素を返す (抽象メソッド) */
            virtual ParseTreeNode* getChildNode(int index) = 0;

			/* ノードが非終端ノードの場合，子孫の中から引数のシンボルと同じもののリストを返す (抽象メソッド) */
            virtual list<ParseTreeNode*>* getDescendantNodes(Symbol symbol) = 0;

			/* ノードの複製を作り，それへの参照を返す (抽象メソッド) */
            virtual ParseTreeNode* copy() = 0;

			/* ノードの内容をLispのS式的な文字列へ変換する (抽象メソッド) */
			virtual string toString() = 0;

			/* ノードの子ノードすべてをSQL文字列として統合するメソッド */
			string concatChildNodes(string separator);

			/* ノードの子ノードすべてをSQL文字列として統合するメソッド(偶数セパレータ，奇数セパレータ) */
			string concatChildNodes(string separator1, string separator2);

   			/* ノードの内容をSQL文字列に変換をするメソッド */
            string toSQLString();

 };


	/**
	 * LeafNodeは，構文解析結果の木構造を表現するクラスのうち，終端ノードを表すクラス.
	 *
	 * <p>
	 * ParseTreeNodeを継承している．
	 * </p>
	 *
	 * @author	Yousuke Watanabe
	 * @date	2018/03/21
	 */
    class LeafNode : public ParseTreeNode {
        private:
			/* 終端記号の種類を表す */
            Symbol symbol;

			/* 終端記号の文字列を保持する */
            string value;

        public:
			/* コンストラクタ．引数は終端記号の文字列 */
            LeafNode(string value);

			/* デストラクタ */
            ~LeafNode();

			/* valueの種類が数値かどうか */
            bool isNumber();

			/* Valueの種類がValueかどうか */
			bool isValues(); 

			/* valueの種類がBoolenかどうか */
			bool isBoolen();
	
			/* valueの種類がNULLかどうか */
			bool isNull();

			/* valueの種類が文字列かどうか */
            bool isString();

			/* valueの種類が比較演算子かどうか */
            bool isComparator();

			/* valueの種類が符号かどうか */
            bool isSign();

			/* valueの種類が * かどうか */
            bool isAsterisk();

			/* valueの種類が Identifier かどうか */
            bool isIdentifier();

			/* valueの種類が Formula かどうか */
            bool isFormula();

			/* valueの種類が ; かどうか */
			bool isSemicolon();

			/* valueの種類が数値配列かどうか */
            bool isNumberArray();

			/* valueの種類が文字列配列かどうか */
            bool isStringArray();

			/* ノードのシンボルを返す */
            Symbol getSymbol() override; 

			/* ノードの子要素の数を返す */
            int getSize() override;

			/* ノードが終端ノードかどうかを返す */
            bool isLeafNode() override;

			/* ノードが非終端ノードかどうかを返す */
            bool isIntermediateNode() override;

			/* ノードが終端ノードの場合，値を返す */
            string getValue() override;

			/* ノードが非終端ノードの場合，指定した位置の子要素を返す */
            ParseTreeNode* getChildNode(int index) override;

			/* ノードが非終端ノードの場合，自分自身と子孫の中から引数のシンボルと同じもののリストを返す */
            list<ParseTreeNode*>* getDescendantNodes(Symbol symbol) override;

			/* ノードの複製を作り，それへの参照を返す */
            ParseTreeNode* copy() override;

			/* ノードの内容をLispのS式的な文字列へ変換する */
			string toString() override;
    };

	/**
	 * IntermediateNodeは，構文解析結果の木構造を表現するクラスのうち，非終端ノードを表すクラス.
	 *
	 * <p>
	 * ParseTreeNodeを継承している．
	 * </p>
	 *
	 * @author	Yousuke Watanabe
	 * @date	2018/03/21
	 */
    class IntermediateNode : public ParseTreeNode {
        private:
			/* 終端記号の種類を表す */
            Symbol symbol;

			/* 終端記号の文字列を保持する */
            list<ParseTreeNode*> values;

        public:
			/* コンストラクタ．引数は終端記号の種類を表すシンボル */
            IntermediateNode(Symbol symbol);

			/* コンストラクタ．引数は終端記号の種類を表すシンボル，子要素の文字列のリスト */
            IntermediateNode(Symbol symbol, initializer_list<string> values);

			/* コンストラクタ．引数は終端記号の種類を表すシンボル，子要素のリスト */
            IntermediateNode(Symbol symbol, initializer_list<ParseTreeNode*> values);

			/* デストラクタ */
            ~IntermediateNode();

			/* 子要素の文字列(LeafNodeに変換される)を末尾に追加する */
            void add(string s);

			/* 子要素を末尾に追加する */
            void add(ParseTreeNode* n);

			/* ノードのシンボルを返す */
            Symbol getSymbol() override; 

			/* ノードの子要素の数を返す */
            int getSize() override;

			/* ノードが終端ノードかどうかを返す */
            bool isLeafNode() override;

			/* ノードが非終端ノードかどうかを返す */
            bool isIntermediateNode() override;

			/* ノードが終端ノードの場合，値を返す.非終端の場合はtoStringと同じ結果 */
            string getValue() override;

			/* ノードが非終端ノードの場合，指定した位置の子要素を返す */
            ParseTreeNode* getChildNode(int index) override;

			/* ノードが非終端ノードの場合，自分自身と子孫の中から引数のシンボルと同じもののリストを返す */
            list<ParseTreeNode*>* getDescendantNodes(Symbol symbol) override;

			/* ノードの複製を作り，それへの参照を返す */
            ParseTreeNode* copy() override;

			/* ノードの内容をLispのS式的な文字列へ変換する */
			string toString() override;
    };


	/**
	 * OperatorTreeGeneratorは，構文解析木からオペレータグラフへ変換するクラス.
	 *
	 * @author	Yousuke Watanabe
	 * @date	2019/04/06
	 */
	class OperatorTreeGenerator {
		public:
			/* コンストラクタ．引数は構文解析器のルートへの参照 */
			OperatorTreeGenerator(ParseTreeNode* root);

			/* デストラクタ */
			~OperatorTreeGenerator();

			/* 変換後のオペレータグラフへの参照を返す */
			Graph* getGraph();

			/* 変換後のオペレータグラフリストのXML文字列を返す */
			string getGraphListXML();

		private:
			/* 構文解析器のルートへの参照 */
			ParseTreeNode* root;

			/* 変換後のオペレータグラフへの参照 */
			Graph* graph;

			/* 変換後のオペレータグラフへの参照リスト */
			list<Graph*> graphList;

			/* クエリのマスタ節のストリーム名の文字列 */
			string master;

			/* Graphへ付与するidのためのカウンタ */
			int gcount;

			/* Keyへ付与するidのためのカウンタ */
			int kcount;

			/* Nodeへ付与するidのためのカウンタ */
			int ncount;

			/* Edgeへ付与するidのためのカウンタ */
			int ecount;

			/* 第二層ノードのシンボル */
			Symbol secondNodeSymbol = Symbol::UNKNOWN;

			/* MASTERノードを変換する */
			void analyzeMaster(ParseTreeNode* subtree);

			/* QUERYノードを変換する */
			Node* analyzeQuery(ParseTreeNode* subtree);

			/* CREATE_STREAMノードを変換する */
			Node* analyzeCreateStream(ParseTreeNode* subtree);

			/* CREATE_TABLEノードを変換する */
			Node* analyzeCreateTable(ParseTreeNode* subtree);

			/* TABLE_ATTR_LISTノードを変換する */
			string analyzeTableAttrList(ParseTreeNode* subtree);

			/* DROP_STREAMノードを変換する */
			Node* analyzeDropStream(ParseTreeNode* subtree);

			/* DROP_TABLEノードを変換する */
			Node* analyzeDropTable(ParseTreeNode* subtree);

			/* WITH_RECURSIVEノードを変換する */
			Node* analyzeRecursive(ParseTreeNode* subtree);

			/* SUBQUERYノードを変換する */
			Node* analyzeSubQuery(ParseTreeNode* subtree);

			/* UNIONノードを変換する */
			Node* analyzeUnion(ParseTreeNode* subtree);

			/* SELECTノードを変換する */
			Node* analyzeSelect(ParseTreeNode* subtree);

			/* SELECTノードの下のEXPRESSIONノードを変換する */
			Node* analyzeSelectListExpression(ParseTreeNode* subtree, Node* in);

			/* SELECT_LISTノードを文字列に変換する */
			string findColumns(ParseTreeNode* subtree);

			/* WHEREノードを変換する */
			Node* analyzeWhere(ParseTreeNode* subtree);

			/* PREDICATE_LISTノードを変換する */
			string analyzePredicateList(ParseTreeNode* subtree, Node* &topNode);

			/* PREDICATEノードを変換する */
			string analyzePredicate(ParseTreeNode* subtree, Node* &topNode);

			/* EXPRESSIONノードを変換する */
			string analyzeExpression(ParseTreeNode* subtree);

			/* EXPRESSIONノードを変換する(Node生成あり) */
			string analyzeExpression(ParseTreeNode* subtree, Node* &topNode);

			/* COMPARATORノードを変換する */
			string analyzeComparator(ParseTreeNode* subtree);

			/* FUNCTIONノードを変換する */
			string analyzeFunction(ParseTreeNode* subtree);

			/* FUNCTIONノードを変換する(Node生成あり)  */
			string analyzeFunction(ParseTreeNode* subtree, Node* &topNode);

			/* EXPRESSION_LISTノードを変換する */
			string analyzeExpressionList(ParseTreeNode* subtree);

			/* EXPRESSION_LISTノードを変換する(Node生成あり)  */
			string analyzeExpressionList(ParseTreeNode* subtree, Node* &topNode);

			/* FROMノードを変換する */
			Node* analyzeFrom(ParseTreeNode* subtree);

			/* FROM_LISTノードを変換する */
			Node* analyzeFromList(ParseTreeNode* subtree);

			/* TABLE_RENAMEノードを変換する */
			Node* analyzeTableRename(ParseTreeNode* subtree);

			/* 部分木の入力テーブル名のリストを返す */
			list<string> findTableNames(ParseTreeNode* subtree);

			/* TABLEノードを変換する */
			Node* analyzeTable(ParseTreeNode* subtree);

			/* 渡された部分木がRDBだけで構成されているかをチェックする */
			bool isRDB(ParseTreeNode* subtree);

			/* 渡された部分木がStreamだけで構成されているかをチェックする */
			bool isStream(ParseTreeNode* subtree);

			/* FROM_LISTの部分木からRDBだけを抽出する */
			list<ParseTreeNode*> extractRDBs(ParseTreeNode* fromList);

			/* FROM_LISTの部分木からStreamだけを抽出する */
			list<ParseTreeNode*> extractStreams(ParseTreeNode* fromList);

			/* Edgeを生成する */
			void createEdge(Node* from, Node* to);
	};


	/**
	 * QueryParserは，クエリ文字列から構文解析結果の木構造を生成するクラス.
	 *
	 * @author	Yousuke Watanabe
	 * @date	2019/04/06
	 */
    class QueryParser {
        public:
			/* コンストラクタ．引数はクエリ文字列 */
            QueryParser(string str);

			/* デストラクタ */
            ~QueryParser();

			/* (テスト用)構文解析が終わった木のルートノードへの参照を返す */
            ParseTreeNode* getQuery();

			/* 構文解析と変換が終わったオペレータグラフへの参照を返す */
            Graph* getGraph();

			/* 構文解析と変換が終わったオペレータグラフリストのXML文字列を返す */
			string getGraphListXML();

			/* 大文字小文字を区別しない文字列の同値比較 */
			static bool iequals(string left, string right);

   			/* 大文字を小文字に変換した文字列の複製を返す */
			static string to_lower_copy(string value);

     private:
	 		LoggerPtr logger = Logger::getLogger("QueryParser");
		
			/* 字句解析されたトークンの集合 */
            Tokenizer* tokens;

			/* 構文解析が終わった木のルートノードへの参照 */
            ParseTreeNode* query;

			/* オペレータグラフへの変換器 */
			OperatorTreeGenerator* gen;

			/* 非終端記号QUERYの解析 */
            ParseTreeNode* parseQuery();

			/* 非終端記号CREATE_STREAMの解析 */
            ParseTreeNode* parseCreateStream();

			/* 非終端記号TABLE_ATTR_LISTの解析 */
            ParseTreeNode* parseTableAttrList();

			/* 非終端記号TABLE_ATTRの解析 */
            ParseTreeNode* parseTableAttr();

			/* 非終端記号DATE_PATTERNの解析 */
            ParseTreeNode* parseDatePattern();

			/* 非終端記号DROP_STREAMの解析 */
            ParseTreeNode* parseDropStream();

			/* 非終端記号DROP_TABLEの解析 */
            ParseTreeNode* parseDropTable();

			/* 非終端記号MASTERの解析 */
            ParseTreeNode* parseMaster();

			/* 非終端記号MASTER_LISTの解析 */
            ParseTreeNode* parseMasterList();

			/* 非終端記号COLUMN_LISTの解析 */
            ParseTreeNode* parseColumnList();

			/* 終端記号IDENTIFIERの解析 */
            ParseTreeNode* parseIdentifier();

			/* 非終端記号WITH_RECURSIVEの解析 */
            ParseTreeNode* parseWithRecursive();

			/* 非終端記号SUBQURYの解析 */
            ParseTreeNode* parseSubquery();

			/* 非終端記号SELECTの解析 */
            ParseTreeNode* parseSelect();

			/* 終端記号ASTERISKの解析 */
            ParseTreeNode* parseAsterisk();

			/* 非終端記号SELECT_LISTの解析 */
            ParseTreeNode* parseSelectList();

			/* 非終端記号EXPRESSION_LISTの解析 */
            ParseTreeNode* parseExpressionList();

			/* 非終端記号EXPRESSIONの解析 */
            ParseTreeNode* parseExpression();

			/* 非終端記号FUNCTIONの解析 */
            ParseTreeNode* parseFunction();

			/* 非終端記号FROMの解析 */
            ParseTreeNode* parseFrom();

			/* 非終端記号FROM_LISTの解析 */
            ParseTreeNode* parseFromList();

			/* 非終端記号TABLEの解析 */
            ParseTreeNode* parseTable();

			/* 非終端記号PARTITIONの解析 */
			
			ParseTreeNode* parsePartition();
			/* 非終端記号WINDOWの解析 */
            ParseTreeNode* parseWindow();

			/* 非終端記号HIERARCHYの解析 */
            ParseTreeNode* parseHierarchy();

			/* 非終端記号WHEREの解析 */
            ParseTreeNode* parseWhere();

			/* 非終端記号PREDICATE_LISTの解析 */
            ParseTreeNode* parsePredicateList();

			/* 非終端記号PREDICATEの解析 */
            ParseTreeNode* parsePredicate();

			/* 非終端記号COMPARATORの解析 */
            ParseTreeNode* parseComparator();

			/* 非終端記号CREATE_TABLEの解析 */
            ParseTreeNode* parseCreateTable();

			/* 非終端記号DEFAULTの解析 */
            ParseTreeNode* parseDefault();
			
			/* 非終端記号VALUE_ATTRの解析 */
            ParseTreeNode* parseValueAttr();
		
			/* 非終端記号UPDATE_TABLEの解析 */
            ParseTreeNode* parseUpdateTable();
				
			/* 非終端記号SET_ATTR_LISTの解析 */
            ParseTreeNode* parseSetAttrList();
				
			/* 非終端記号SET_ATTRの解析 */
            ParseTreeNode* parseSetAttr();
					
			/* 非終端記号DELET_TABLEの解析 */
            ParseTreeNode* parseDeleteTable();

			/* 非終端記号VALUE_ATTR_LISTの解析 */
            ParseTreeNode* parseInsertInto();

    		/* 非終端記号VALUE_ATTR_LISTの解析 */
            ParseTreeNode* parseValueAttrList();

};

	/**
	 * ParseExceptionは，クエリ解析に関する例外を通知するためのクラス.
	 *
	 * @author	Yousuke Watanabe
	 * @date	2018/03/21
	 */
	class ParseException : public exception {
		public:
			/* コンストラクタ．引数はエラーメッセージ */
			ParseException(string msg);

			/* エラーメッセージの文字列 */
			string getMessage() const;

		private:
			/* エラーメッセージの文字列を保持 */
			string message;
	};
}


#endif
