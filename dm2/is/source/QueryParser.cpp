/*
 * クラス QueryParser
 *
 * クエリ文字列から構文解析結果の木構造を生成するクラスの実装．
 */

/* クラス定義ヘッダファイル */
#include "Parser.h"

/* 依存ライブラリ */
#include <cctype>
#include <iostream>

using namespace IS;

/**
 * コンストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	str	クエリ文字列
 */
QueryParser::QueryParser(string str)
{
    this->tokens = new Tokenizer(str);

    this->query = parseQuery();

	this->gen = new OperatorTreeGenerator(query);
}

/**
 * デストラクタ.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 */
QueryParser::~QueryParser()
{
	delete this->tokens;
	delete this->gen;
	this->query = nullptr;
}

/**
 * 構文解析が終わった木のルートノードへのポインタを返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	構文解析結果の木のルートノードへのポインタ
 */
ParseTreeNode* QueryParser::getQuery()
{
    return query;
}

/**
 * 構文解析と変換が終わったオペレータグラフへの参照を返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	オペレータグラフへのポインタ
 */
Graph* QueryParser::getGraph()
{
	return gen->getGraph();
}

/**
* 構文解析と変換が終わったオペレータグラフリストのXML文字列を返す.
*
* @author	Nagoya University
* @date	2019/06/19
* @return	オペレータグラフリストのXML文字列
*/
string QueryParser::getGraphListXML()
{
	return gen->getGraphListXML();
}

/**
 * 大文字小文字を区別しない文字列の同値比較.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	left	比較対象の文字列
 * @param	right	比較対象の文字列
 * @return	leftとrightが文字列として等しい(大文字小文字を区別しない)場合はtrue
 */
bool QueryParser::iequals(string left, string right)
{
	if(left.size() != right.size())
		return false;
	for(unsigned int i=0; i < left.size() && right.size(); i++){
		if(toupper(left[i]) != toupper(right[i]))
			return false;
	}
	return true;
}

/**
 * 大文字を小文字に変換した文字列の複製を返す.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	value	変換対象の文字列
 * @return	valueに含まれる大文字を全て小文字に変換した文字列
 */
string QueryParser::to_lower_copy(string value)
{
	char* buf = new char[value.size()+1];
	for(unsigned int i=0; i < value.size(); i++){
		buf[i] = tolower(value[i]);
	}
	buf[value.size()] = '\0';
		
	return string(buf, value.size());
}

/**
 * 非終端記号QUERYの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <QUERY> := <MASTER>? <WITH_RECURSIVE>? ( <SUBQUERY> | <UNION> ) <SEMICOLON>?
 *          | <CREATE_STREAM>
 *          | <DROP_STREAM>
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/07
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseQuery()
{
    IntermediateNode* node = new IntermediateNode(Symbol::QUERY);

	/* | <CREATE_STREAM> <CREATE_TABLE> */
	if(tokens->hasMoreTokens() && iequals(tokens->lookAheadToken(), "CREATE") ){

		tokens->getToken();		/* pop "CREATE" token */
	
		/* "CREATE_STREAM" */
		if( tokens->hasMoreTokens() && iequals(tokens->lookAheadToken(), "STREAM")){
			ParseTreeNode* create_stream = parseCreateStream();
			node->add(create_stream);
		}
		/* "CREATE_TABLE" */
		else if( tokens->hasMoreTokens() && iequals(tokens->lookAheadToken(), "TABLE")){
			ParseTreeNode* create_table = parseCreateTable();
			node->add(create_table);
		}

		if(tokens->hasMoreTokens() == true && tokens->compareSymbol(Symbol::SEMICOLON))
			throw ParseException("There are remaining tokens");

		return node;
	}

	/* | <DROP_STREAM> */
	if(tokens->hasMoreTokens() && iequals(tokens->lookAheadToken(), "DROP") ){
	
		tokens->getToken();		/* pop "DROP" token */


		/* "DROP_STREAM" */
		if( tokens->hasMoreTokens() && iequals(tokens->lookAheadToken(), "STREAM")){
			ParseTreeNode* drop_stream = parseDropStream();
			node->add(drop_stream);
		}
		
		/* "DROP_TABLE" */
		else if( tokens->hasMoreTokens() && iequals(tokens->lookAheadToken(), "TABLE")){
			ParseTreeNode* drop_table = parseDropTable();
			node->add(drop_table);
		}
		
		if(tokens->hasMoreTokens() == true && tokens->compareSymbol(Symbol::SEMICOLON))
			throw ParseException("There are remaining tokens");

		return node;
	}

	/* <UPDATE>? */
	if(tokens->hasMoreTokens() && iequals(tokens->lookAheadToken(), "UPDATE") ){
		ParseTreeNode* update = parseUpdateTable();
		if(update != nullptr)
			node->add(update);

		return node;
	}

	/* <DELETE>? */
	if(tokens->hasMoreTokens() && iequals(tokens->lookAheadToken(), "DELETE") ){
		ParseTreeNode* deleteTable = parseDeleteTable();
		if(deleteTable != nullptr)
			node->add(deleteTable);

		return node;
	}

	/* <INSERT>? */
	if(tokens->hasMoreTokens() && iequals(tokens->lookAheadToken(), "INSERT") ){
		ParseTreeNode* insertInto = parseInsertInto();
		if(insertInto != nullptr)
			node->add(insertInto);
			
		return node;
	}


	/* <MASTER>? */
	if(tokens->hasMoreTokens() && iequals(tokens->lookAheadToken(), "MASTER") ){
		ParseTreeNode* master = parseMaster();
		if(master != nullptr)
			node->add(master);
	}

	/* <WITH_RECURSIVE>? */
	if(tokens->hasMoreTokens() && iequals(tokens->lookAheadToken(), "WITH") ){
		ParseTreeNode* with = parseWithRecursive();
		if(with != nullptr)
			node->add(with);
	}

	/* <SUBQUERY> */
    ParseTreeNode* subquery = parseSubquery();
    if(subquery == nullptr)
		throw ParseException("SELECT and FROM clauses are necessary");

	if(! tokens->hasMoreTokens() || ! iequals(tokens->lookAheadToken(), "UNION") ){
		node->add(subquery);
	}
	else {
		/* <UNION> := <SUBQUERY> ("UNION" <SUBQUERY>)+  */
		IntermediateNode *uni = new IntermediateNode(Symbol::UNION);
		uni->add(subquery);
		while(tokens->compareSymbol(Symbol::UNION) == true){
			subquery = parseSubquery();
			if(subquery == nullptr)
				throw ParseException("SELECT and FROM clauses are necessary");
			uni->add(subquery);
		}
		node->add(uni);
	}

	/* <SEMICOLON>? */
	if(tokens->hasMoreTokens() == true && tokens->compareSymbol(Symbol::SEMICOLON))
		throw ParseException("There are remaining tokens");

    return node;
}

/**
 * 非終端記号CREATE_STREAMの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <CREATE_STREAM> := "CREATE" "STREAM" <IDENTIFIER> "(" <TABLE_ATTR_LIST> ")"
 *                  | "CREATE" "STREAM" <IDENTIFIER> "(" <TABLE_ATTR_LIST> ")" "TIMER_FIX"  "[" <NUMBER> "]"
 *                  | "CREATE" "STREAM" <IDENTIFIER> "(" <TABLE_ATTR_LIST> ")" "TIMER_FIX"  "[" <NUMBER> "]　"START_FIX" "[" <NUMBER> "]"
 *                  | "CREATE" "STREAM" <IDENTIFIER> "(" <TABLE_ATTR_LIST> ")" "TIMER_CRON" "[" <DATE_PATTERN> "]"
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/06
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseCreateStream()
{
    IntermediateNode* node = new IntermediateNode(Symbol::CREATE_STREAM);

	/* "CREATE" */
	/* already poped
	if(! tokens->hasMoreTokens() || ! iequals(tokens->getToken(), "CREATE"))
		throw ParseException("\"CREATE\" is expected, but not found");
    */

	/* "STREAM" */
	if(! tokens->hasMoreTokens() || ! iequals(tokens->getToken(), "STREAM"))
		throw ParseException("\"STREAM\" is expected, but not found");

	/* <IDENTIFIER> */
	if(! tokens->hasMoreTokens() )
		throw ParseException("next token does not exist");
	ParseTreeNode *identifier = parseIdentifier();
	if(identifier == nullptr)
		throw ParseException("cannot parse identifier");
	node->add(identifier);

	/* "(" */
	if(tokens->compareToken("(") != true)
		throw ParseException("'(' is expected");

	/* <TABLE_ATTR_LIST> */
	ParseTreeNode *table_attr_list = parseTableAttrList();
	if(table_attr_list == nullptr)
		throw ParseException("cannnot parse Table_Attr_List");
	node->add(table_attr_list);

	/* ")" */
	if(tokens->compareToken(")") != true)
		throw ParseException("')' is expected");
	
	/* "TIME_FIX" */
	if(tokens->hasMoreTokens() && iequals(tokens->lookAheadToken(), "TIMER_FIX")){
		/* "TIMER_FIX" */
		ParseTreeNode *time = parseIdentifier();
		if(time == nullptr)
			throw ParseException("cannot parse identifier");
		node->add(time);

		/* "[" */
		if(tokens->compareToken("[") != true)
			throw ParseException("'[' is expected");

		/* <NUMBER> */
		LeafNode *leaf = new LeafNode(tokens->getToken());
		if(leaf->isNumber() == false)
			throw ParseException("the token should be number");
		node->add(leaf);

		/* "]" */
		if(tokens->compareToken("]") != true)
			throw ParseException("']' is expected");
	}
	/* "TIME_CRON" */
	else if(tokens->hasMoreTokens() && iequals(tokens->lookAheadToken(), "TIMER_CRON")){
		/* "TIMER_CRON" */
		ParseTreeNode *time = parseIdentifier();
		if(time == nullptr)
			throw ParseException("cannot parse identifier");
		node->add(time);

		/* "[" */
		if(tokens->compareToken("[") != true)
			throw ParseException("'[' is expected");

		/* <DATE_PATTERN> */
		ParseTreeNode *date = parseDatePattern();
		if(date == nullptr)
			throw ParseException("cannot parse date pattern");
		node->add(date);

		/* "]" */
		if(tokens->compareToken("]") != true)
			throw ParseException("']' is expected");
	}
	/* "START_FIX" */
	if(tokens->hasMoreTokens() && iequals(tokens->lookAheadToken(), "START_FIX")){
		/* "START_FIX" */
		ParseTreeNode *start = parseIdentifier();
		if(start == nullptr)
			throw ParseException("cannot parse identifier");
		node->add(start);

		/* "[" */
		if(tokens->compareToken("[") != true)
			throw ParseException("'[' is expected");

		/* <NUMBER> */
		LeafNode *leaf = new LeafNode(tokens->getToken());
		if(leaf->isNumber() == false)
			throw ParseException("the token should be number");
		node->add(leaf);

		/* "]" */
		if(tokens->compareToken("]") != true)
			throw ParseException("']' is expected");
	}

	return node;
}

/**
 * 非終端記号TABLE_ATTR_LISTの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <TABLE_ATTR_LIST> := <IDENTIFIER> <IDENTIFIER> ("," <IDENTIFIER> <IDENTIFIER> )*
 * </pre>
 *
 * @author	Nagoya University
 * @date	2022/02/15
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseTableAttrList()
{
	IntermediateNode *node = new IntermediateNode(Symbol::TABLE_ATTR_LIST);

	do {
		/* <IDENTIFIER> */
		ParseTreeNode *tableAttr = parseTableAttr();
		if(tableAttr == nullptr)
			throw ParseException("TableAttr is expected, but not found");
		node->add(tableAttr);

	} while(tokens->compareToken(",") == true);

	return node;
}
/**
 * 非終端記号TABLE_ATTRの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <TABLE_ATTR> := <IDENTIFIER> <IDENTIFIER> (<DEFAULT>value_attr)?
 * </pre>
 *
 * @author	Nagoya University
 * @date	2022/02/015
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseTableAttr()
{
	IntermediateNode *node = new IntermediateNode(Symbol::TABLE_ATTR);

	/* <IDENTIFIER> */
	ParseTreeNode *identifier = parseIdentifier();
	if(identifier == nullptr)
		throw ParseException("identifier is expected, but not found");
	node->add(identifier);

	/* <IDENTIFIER> */
	ParseTreeNode *type= parseIdentifier();
	if(type == nullptr)
		throw ParseException("type name is expected, but not found");
	node->add(type);

	/* <DEFAULT> */
	if(tokens->hasMoreTokens() && iequals(tokens->lookAheadToken(), "DEFAULT")){
		logger->debug("CREATE TABLE:: DEFAULT reach!!");
		ParseTreeNode *default_node = parseDefault();
		node->add(default_node);
	}

	return node;
}

/**
 * 非終端記号DATE_PATTERNの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <DATE_PATTERN> := (<NUMBER>|"*")(<NUMBER>|"*")(<NUMBER>|"*")(<NUMBER>|"*")(<NUMBER>|"*")
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/07
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseDatePattern()
{
	IntermediateNode *node = new IntermediateNode(Symbol::DATE_PATTERN);

	for(int i=0; i < 5; i++){
		if(tokens->hasMoreTokens() == false)
			throw ParseException("no token exists");

		/* <NUMBER>|"*" */
		LeafNode *leaf = new LeafNode(tokens->getToken());
		if(leaf->isNumber() == false && leaf->isAsterisk() == false)
			throw ParseException("the token should be number or *");
		node->add(leaf);

	}
	if(node->getSize() != 5)
		throw ParseException("the size of date pattern should be 5");

	return node;
}

/**
 * 非終端記号DROP_STREAMの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <DROP_STREAM> := "DROP" "STREAM" <IDENTIFIER>
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/06
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseDropStream()
{
    IntermediateNode* node = new IntermediateNode(Symbol::DROP_STREAM);

	/* "DROP" */
	/* Already poped 
	if(! tokens->hasMoreTokens() || ! iequals(tokens->getToken(), "DROP"))
		throw ParseException("\"DROP\" is expected, but not found");
	*/

	/* "STREAM" */
	if(! tokens->hasMoreTokens() || ! iequals(tokens->getToken(), "STREAM"))
		throw ParseException("\"STREAM\" is expected, but not found");

	/* <IDENTIFIER> */
	if(! tokens->hasMoreTokens() )
		throw ParseException("next token does not exist");
	ParseTreeNode *identifier = parseIdentifier();
	if(identifier == nullptr)
		throw ParseException("cannot parse identifier");
	node->add(identifier);

	return node;
}


/**
 * 非終端記号DROP_STREAMの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <DROP_TABLE> := "DROP" "TABLE" <IDENTIFIER>
 * </pre>
 *
 * @author	Nagoya University
 * @date	2022/02/14
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseDropTable()
{
    IntermediateNode* node = new IntermediateNode(Symbol::DROP_TABLE);

	/* "TABLE" */
	if(! tokens->hasMoreTokens() || ! iequals(tokens->getToken(), "TABLE"))
		throw ParseException("\"TABLE\" is expected, but not found");

	/* <IDENTIFIER> */
	if(! tokens->hasMoreTokens() )
		throw ParseException("next token does not exist");
	ParseTreeNode *identifier = parseIdentifier();
	if(identifier == nullptr)
		throw ParseException("cannot parse identifier");
	node->add(identifier);

	return node;
}


/**
 * 非終端記号MASTERの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <MASTER> := "MASTER" <MASTER_LIST>
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseMaster()
{
    IntermediateNode* node = new IntermediateNode(Symbol::MASTER);

	if(tokens->compareSymbol(Symbol::MASTER) == false)
		throw ParseException("MASTER-clause must start with \"MASTER\"");

	ParseTreeNode* list = parseMasterList();
	if(list == nullptr || list->getSize() == 0)
		throw ParseException("Master List must not be null or empty.");
	node->add(list);

	return node;
}

/**
 * 非終端記号MASTER_LISTの解析.
 *
 * <pre>
 * 下記の構文ルールに対応
 * <MASTER_LIST> := <IDENTIFIER> ( "," <IDENTIFIER> )*
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseMasterList()
{
	IntermediateNode *node = new IntermediateNode(Symbol::MASTER_LIST);

	do {
		ParseTreeNode *identifier = parseIdentifier();
		if(identifier == nullptr)
			throw ParseException("identifier is expected, but not found");
		node->add(identifier);

	} while(tokens->compareToken(",") == true);

	return node;
}

/**
 * 非終端記号COLUMN_LISTの解析.
 *
 * <pre>
 * 下記の構文ルールに対応
 * <COLUMN_LIST> := <IDENTIFIER> ( "," <IDENTIFIER> )*
 * </pre>
 *
 * @author	Nagoya University
 * @date	2022/02/24
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseColumnList()
{
	IntermediateNode *node = new IntermediateNode(Symbol::COLUMN_LIST);

	do {
		ParseTreeNode *identifier = parseIdentifier();
		if(identifier == nullptr)
			throw ParseException("identifier is expected, but not found");
		node->add(identifier);

	} while(tokens->compareToken(",") == true);

	return node;
}

/**
 * 終端記号IDENTIFIERの解析.
 *
 * <pre>
 * 下記の構文ルールに対応
 * <IDENTIFIER> := [a-zA-Z][a-zA-Z0-9_]*
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseIdentifier()
{
	if(tokens->hasMoreTokens() != true)
		throw ParseException("identfier is expected, but no next token exists");

	LeafNode* node = new LeafNode(tokens->getToken());

	if(node->isIdentifier() != true)
		throw ParseException("'" + node->getValue() + "' is not an identifier");
	return node;
}

/**
 * 非終端記号WITH_RECURSIVEの解析.
 *
 * <pre>
 * 下記の構文ルールに対応
 * <WITH_RECURSIVE> := "WITH" "RECURSIVE" <IDENTIFIER> "AS" "(" <UNION> ")"
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseWithRecursive()
{
    IntermediateNode* node = new IntermediateNode(Symbol::WITH_RECURSIVE);

	/* "WITH" */
	if(! tokens->hasMoreTokens() || ! iequals(tokens->getToken(), "WITH"))
		throw ParseException("\"WITH\" is expected, but not found");

	/* "RECURSIVE" */
	if(! tokens->hasMoreTokens() || ! iequals(tokens->getToken(), "RECURSIVE"))
		throw ParseException("\"RECURSIVE\" is expected, but not found");

	/* <IDENTIFIER> */
	if(! tokens->hasMoreTokens() )
		throw ParseException("next token does not exist");
	ParseTreeNode *identifier = parseIdentifier();
	if(identifier == nullptr)
		throw ParseException("cannot parse identifier");
	node->add(identifier);

	/* "AS" */
	if(! tokens->hasMoreTokens() || ! iequals(tokens->getToken(), "AS"))
		throw ParseException("\"AS\" is expected, but not found");

	/* "(" */
	if(! tokens->compareToken("("))
		throw ParseException("\"(\" is expected, but not found");

	/* <UNION> := <SUBQUERY> ("UNION" <SUBQUERY>)+  */
	IntermediateNode *uni = new IntermediateNode(Symbol::UNION);
	do {
		ParseTreeNode *subquery = parseSubquery();
		if(subquery == nullptr)
			throw ParseException("cannot parse subquery");
		uni->add(subquery);
	} while(tokens->compareSymbol(Symbol::UNION));
	node->add(uni);

	/* ")" */
	if(! tokens->compareToken(")"))
		throw ParseException("\")\" is expected, but not found");

	return node;
}

/**
 * 非終端記号SUBQURYの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <SUBQUERY> := <SELECT> <FROM> <WHERE>?
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseSubquery()
{
    IntermediateNode* node = new IntermediateNode(Symbol::SUBQUERY);

	ParseTreeNode* select = parseSelect();
	if(select == nullptr)
		throw ParseException("SELECT is necessary");
	node->add(select);

	ParseTreeNode* from = parseFrom();
	if(from == nullptr)
		throw ParseException("FROM is necessary");
	node->add(from);

	if(tokens->hasMoreTokens() && iequals(tokens->lookAheadToken(), "WHERE") ){
		ParseTreeNode* where = parseWhere();
		if(where != nullptr)
			node->add(where);
	}

    return node;
}


/**
 * 非終端記号SELECTの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <SELECT> := "SELECT"  ( <ASTERISK> | <SELECT_LIST> )
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	構文解析後のツリーへのポインタ
*/
ParseTreeNode* QueryParser::parseSelect()
{
	IntermediateNode* node = new IntermediateNode(Symbol::SELECT);

	if(tokens->compareSymbol(Symbol::SELECT) == false)
		throw ParseException("SELECT-clause must start with \"SELECT\"");

	ParseTreeNode* list;
	if(tokens->lookAheadToken() == "*"){
		list = parseAsterisk();
	}
	else {
		list = parseSelectList();
	}

	if(list == nullptr || list->getSize()==0)
		throw ParseException("SELECT-clause must not be null or empty.");
	node->add(list);

	return node;
}

/**
 * 終端記号ASTERISKの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <ASTERISK> := "*"
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseAsterisk()
{
	if(tokens->hasMoreTokens() != true)
		throw ParseException("* is expected, but no next token exists");

	LeafNode* node = new LeafNode(tokens->getToken());
	if(node->getValue() != "*" )
		throw ParseException("* is expected");
	return node;
}

/**
 * 非終端記号SELECT_LISTの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <SELECT_LIST> := ( <EXPRESSION> | <EXPRESSION_RENAME> ) ( "," ( <EXPRESSION> | <EXPRESSION_RENAME ) )*
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseSelectList()
{
	IntermediateNode *node = new IntermediateNode(Symbol::SELECT_LIST);

	do{
		ParseTreeNode *expression = parseExpression();
		if(expression == nullptr)
			throw ParseException("expression is expected, but not found");

		/* <EXPRESSION> */
		if(tokens->compareSymbol(Symbol::AS) == false){
			node->add(expression);
		}

		/* <EXPRESSION_RENAME> := <EXPRESSION> "AS" <IDENTIFIER> */
		else {
			IntermediateNode *rename = new IntermediateNode(Symbol::EXPRESSION_RENAME);
			ParseTreeNode *newname = parseIdentifier();

			rename->add(newname);
			rename->add(expression);
			node->add(rename);
		}
	} while(tokens->compareToken(",") == true);

	return node;
}


/**
 * 非終端記号EXPRESSION_LISTの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <EXPRESSION_LIST> := <EXPRESSION> ( "," <EXPRESSION> )*
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseExpressionList()
{
	IntermediateNode *node = new IntermediateNode(Symbol::EXPRESSION_LIST);

	do{
		ParseTreeNode *expression = parseExpression();
		if(expression == nullptr)
			throw ParseException("expression is expected, but not found");
		node->add(expression);

	} while(tokens->compareToken(",") == true);

	return node;
}

/**
 * 非終端記号EXPRESSIONの解析.
 * <EXPRESSION> := "(" <EXPRESSION> ")" | <NUMBER> | <STRING>| <IDENTIFIER> | <FUNCTION> 
 *
 * <pre>
 * 下記の構文ルールに相当
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseExpression()
{
	IntermediateNode *node = new IntermediateNode(Symbol::EXPRESSION);

	/* "(" <EXPRESSION> ")" */
	if(tokens->compareToken("(")==true){
		ParseTreeNode *child = parseExpression();
		if(child == nullptr || child->getSize()== 0)
            throw ParseException("expression is expected, but not found");

		if(tokens->compareToken(")")==false)
            throw ParseException("')' is expected, but not found");

		node->add(child);
		return node;
	}

	if(tokens->hasMoreTokens() == false)
		throw ParseException("no token exists");

	string token = tokens->getToken();
	//std::cout << "token:" << token << std::endl;
	if (token == "[") {
		while (true) {
			if (tokens->hasMoreTokens() == false)
				throw ParseException("']' is expected, but not found");
			string token_part = tokens->getToken();
			token += token_part;
			if (token_part == "]") break;
		}
	}
	LeafNode *leaf = new LeafNode(token);
	//LeafNode *leaf = new LeafNode(tokens->getToken());

	if(leaf == nullptr)
		throw ParseException("leaf node is null");

	/* <NUMBER> | <STRING> | <NUMBER_ARRAY> | <STRING_ARRAY> */
	if(leaf->isNumber() || leaf->isString() || leaf->isNumberArray() || leaf->isStringArray()){
		node->add(leaf);
		return node;
	}
	/* <SIGN> */
	if(leaf->isSign()){
		LeafNode *next = new LeafNode(leaf->getValue() + tokens->getToken());
		if(next->isNumber() == false)
			throw ParseException("A number is expected here");
		node->add(next);
		return node;
	}

	if(! leaf->isIdentifier())
		throw ParseException("This symbol '" + leaf->getValue() + "' is not allowed here");

	/* <IDENTIFIER> */
	if(tokens->compareToken("(")==false){
		/* <FORMULA> */
		vector<string> operator_list = {"+", "-", "*", "/", "%"};
		for (string ope : operator_list) {
			while(tokens->compareToken(ope)==true){
				if (tokens->hasMoreTokens() == false)
					throw ParseException(ope + " is expected, but not next found");
				string token_part = tokens->getToken();
				token = token + ope + token_part;
				leaf = new LeafNode(token);
				if(! leaf->isFormula())
					throw ParseException("This symbol '" + leaf->getValue() + "' is not allowed By Formula");
			}
		}
		node->add(leaf);
		return node;
	}
	/* <FUNCTION> */
	else {
		tokens->pushToken("(");
		tokens->pushToken(leaf->getValue());
	
		ParseTreeNode *child = parseFunction();
		if(child == nullptr)
			throw ParseException("function is expected, but not found");
		node->add(child);
		return node;
	}
}

/**
 * 非終端記号FUNCTIONの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <FUNCTION> := <IDENTIFIER> "(" <EXPRESSION_LIST>? ")"
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/16
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseFunction()
{
	IntermediateNode *node = new IntermediateNode(Symbol::FUNCTION);

	ParseTreeNode *functionName = parseIdentifier();
	if(functionName == nullptr)
		throw ParseException("function name (identifier) is expected, but not found");
	node->add(functionName);

	if(tokens->compareToken("(") == false)
		throw ParseException("'(' is expected, but not found");

	if(tokens->compareToken(")") == false){
		ParseTreeNode *argumentList = parseExpressionList();
		node->add(argumentList);

		if(tokens->compareToken(")") == false)
			throw ParseException("')' is expected, but not found");
	}
	else {
		ParseTreeNode *argumentList = new IntermediateNode(Symbol::EXPRESSION_LIST);
		node->add(argumentList);
	}

	return node;
}

/**
 * 非終端記号FROMの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <FROM> := "FROM" <FROM_LIST>
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseFrom()
{
	IntermediateNode* node = new IntermediateNode(Symbol::FROM);

	if(tokens->compareSymbol(Symbol::FROM) == false)
		throw ParseException("FROM-clause must start with \"FROM\"");

	ParseTreeNode* list = parseFromList();
	if(list == nullptr || list->getSize()==0)
		throw ParseException("FromList must not be null or empty.");
	node->add(list);

	return node;
}

/**
 * 非終端記号FROM_LISTの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <FROM_LIST> := ( &lt;TABLE> | <TABLE_RENAME> ) ( "," ( &lt;TABLE> | <TABLE_RENAME>) )*
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseFromList()
{
	IntermediateNode *node = new IntermediateNode(Symbol::FROM_LIST);

	do {
		ParseTreeNode *table = parseTable();
		if(table == nullptr)
			throw ParseException("cannot parse table");

		/* <TABLE> */
		if(tokens->compareSymbol(Symbol::AS) == false){
			node->add(table);
		}

		/* <TABLE_RENAME> := <TABLE> "AS" <IDENTIFIER> */
		else {
			IntermediateNode *rename = new IntermediateNode(Symbol::TABLE_RENAME);
			ParseTreeNode *newname = parseIdentifier();

			rename->add(newname);
			rename->add(table);
			node->add(rename);
		}
	} while(tokens->compareToken(","));

	return node;
}

/**
 * 非終端記号TABLEの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * &lt;TABLE> := ( <IDENTIFIER> ("[" <WINDOW> ("," <HIERARCHY> )? "]")? ) | ( "(" <SUBQUERY> ")" )
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/07
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseTable()
{
	IntermediateNode* node = new IntermediateNode(Symbol::TABLE);

	if(tokens->compareToken("(") == false){
		if(tokens->hasMoreTokens() == false)
			throw ParseException("there is not next token");

		/* <IDENTIFIER> */
		ParseTreeNode *tablename = parseIdentifier();
		if(tablename == nullptr)
			throw ParseException("cannot parse table name");
		node->add(tablename);

		/* ("[" <WINDOW> ( "," <HIERARCHY> )? "]")? */
		if(tokens->compareToken("[") == true){
			if(tokens->hasMoreTokens() == false)
				throw ParseException("There is no next token");

			ParseTreeNode *partition = parsePartition();
			if(partition != nullptr) {
				node->add(partition);
			}
			ParseTreeNode *window = parseWindow();
			if(window == nullptr)
				throw ParseException("cannot parse window");
			node->add(window);

			/* ( "," <HIERARCHY> )? */
			if(tokens->compareToken(",") == true){
				ParseTreeNode* hierarchy = parseHierarchy();
				if(hierarchy == nullptr)
					throw ParseException("cannot parse hierarchy");
				node->add(hierarchy);
			}

			if(tokens->compareToken("]") != true){
				throw ParseException("']' is expected");
			}
		}

	}
	else {
		/* "(" <SUBQUERY> ")" */
		ParseTreeNode *child = parseSubquery();
		node->add(child);
		if(tokens->compareToken(")") == false){
			throw ParseException("')' is expected, but not found");
		}
	}

	return node;
}

/**
 * 非終端記号PARTITIONの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <PARTITION> := <PARTITION><BY>column_list
 * </pre>
 *
 * @author	Shinichi Kusayama
 * @date	2025/08/12
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parsePartition()
{
	if (!tokens->compareToken("PARTITION") && !tokens->compareToken("partition")) {
		return nullptr;
	}
	IntermediateNode* node = new IntermediateNode(Symbol::PARTITION);
	LeafNode *partition = new LeafNode(tokens->getToken());
	node->add(partition);

	ParseTreeNode *column_list = parseSelectList();
	if(column_list == nullptr)
			throw ParseException("cannnot parse Column_List");
	node->add(column_list);
	return node;
}

/**
 * 非終端記号WINDOWの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <WINDOW> := (  ( <PARTITION> ) "ROWS" <NUMBER> )
 *           | ( "RANGE"  <NUMBER> ( "now" | "msec" | "sec" | "min" | "hour" | "day" ) )
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseWindow()
{
	IntermediateNode* node = new IntermediateNode(Symbol::WINDOW);

	LeafNode *type = new LeafNode(tokens->getToken());
	if(type->isIdentifier() == false)
		throw ParseException("the first token in window must be ROWS or RANGE");
	node->add(type);

	if(tokens->hasMoreTokens() == false)
		throw ParseException("There is no next token");

	if(iequals(type->getValue(), "ROWS") ){
		LeafNode *number = new LeafNode(tokens->getToken());
		if(number->isNumber() == false)
			throw ParseException("the second token in window must be a number");
		node->add(number);
		if(tokens->hasMoreTokens() == true) {
			bool isSort = false;
			// sort を取り出す。取り出せなかったトークン（例：[ROWS 1]の"]"）は、元に戻す。
			if (tokens->compareToken("sort")) {
				isSort = true;
			} else if (tokens->compareToken("SORT")) {
				isSort = true;
			}
			if (isSort) {
				string sort_str = "sort";
				// by
				if(tokens->hasMoreTokens() == false)
					throw ParseException("the 'by' token in window is missing after 'sort'");
				sort_str += tokens->getToken();
				// DISTANCE_COST
				if(tokens->hasMoreTokens() == false)
					throw ParseException("the 'DISTANCE_COST' token in window is missing");
				sort_str += tokens->getToken();
				// LIMIT
				if(tokens->hasMoreTokens() == false)
					throw ParseException("the 'LIMIT' token in window is missing");
				sort_str += tokens->getToken();
				LeafNode *sort = new LeafNode(sort_str);
				string lower = to_lower_copy(sort->getValue());
				if(lower != "sortbydistance_costlimit" )
					throw ParseException("word after 'rows num',  must be 'sort by distance_cost limit'");
				node->add(sort);
				// number
				if(tokens->hasMoreTokens() == false)
					throw ParseException("the LIMIT's next token is missing");
				LeafNode *number = new LeafNode(tokens->getToken());
				if(number->isNumber() == false)
					throw ParseException("the 'LIMIT' token in window must be a number");
				node->add(number);
				// SEED
				if(tokens->hasMoreTokens() == false)
					throw ParseException("the 'SEED' token in window is missing");
				string seed_str = tokens->getToken();
				LeafNode *seed = new LeafNode(seed_str);
				string lower_seed = to_lower_copy(seed->getValue());
				if(lower_seed != "seed" )
					throw ParseException("the 'SEED' token is missing");
				node->add(seed);
				// number
				if(tokens->hasMoreTokens() == false)
					throw ParseException("the SEED's next token is missing");
				LeafNode *seed_number = new LeafNode(tokens->getToken());
				if(number->isNumber() == false)
					throw ParseException("the 'SEED' token in window must be a number");
				node->add(seed_number);
			} 
		}
	}
	else if(iequals(type->getValue(), "RANGE")){
		LeafNode *number = new LeafNode(tokens->getToken());
		if(number->isNumber() == false)
			throw ParseException("the first token in window must be a number");
		node->add(number);

		if(tokens->hasMoreTokens() == false)
			throw ParseException("There is no next token");

		LeafNode *unit = new LeafNode(tokens->getToken());
		string lower = to_lower_copy(unit->getValue());
		if(lower != "now" && lower != "msec" && lower != "sec" && lower != "min" && lower != "hour" && lower != "day" )
			throw ParseException("unit size of window must be now|msec|sec|min|hour|day");
		node->add(unit);
	}
	else {
		throw ParseException("Unknown window type: " + type->getValue());
	}

	return node;
}

/**
 * 非終端記号HIERARCHYの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <HIERARCHY> := "OWN" | "NEARBY" <NUMBER>? | "ALL" | <NUMBER>
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/07
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseHierarchy()
{
	IntermediateNode* node = new IntermediateNode(Symbol::HIERARCHY);

	if(tokens->hasMoreTokens() == false)
		throw ParseException("There is no next token");

	LeafNode *hierarchy = new LeafNode(tokens->getToken());
	if(iequals(hierarchy->getValue(), "OWN") == false
			&& iequals(hierarchy->getValue(), "NEARBY") == false
			&& iequals(hierarchy->getValue(), "ALL") == false
			&& hierarchy->isNumber() == false)
		throw ParseException("Hierarchy should be OWN or NEARBY or ALL or number");

	node->add(hierarchy);

	if (iequals(hierarchy->getValue(), "NEARBY") == true) {
		LeafNode *leaf = new LeafNode(tokens->getToken());
		if (leaf->isNumber() == false)
			tokens->pushToken(leaf->getValue());
		else
			node->add(leaf);
	}

	return node;
}

/**
 * 非終端記号WHEREの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <WHERE> := "WHERE" <PREDICATE_LIST>
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseWhere()
{
	IntermediateNode* node = new IntermediateNode(Symbol::WHERE);

	if(tokens->compareSymbol(Symbol::WHERE) == false)
		throw ParseException("WHERE-clause must start with \"WHERE\"");

	ParseTreeNode* list = parsePredicateList();
	if(list == nullptr || list->getSize()==0)
		throw ParseException("PredicateList must not be null or empty.");
	node->add(list);

	return node;
}

/**
 * 非終端記号PREDICATE_LISTの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <PREDICATE_LIST> := <PREDICATE> ( "AND" <PREDICATE> )*
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parsePredicateList()
{
	IntermediateNode* node = new IntermediateNode(Symbol::PREDICATE_LIST);

	do{
		ParseTreeNode *child = parsePredicate();
		if(child == nullptr)
			throw ParseException("cannot parse predicate");

		node->add(child);
	} while(tokens->compareSymbol(Symbol::AND) == true);

	return node;
}

/**
 * 非終端記号PREDICATEの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <PREDICATE> := <EXPRESSION> <COMPARATOR> <EXPRESSION>
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parsePredicate()
{
	IntermediateNode *node = new IntermediateNode(Symbol::PREDICATE);

	ParseTreeNode *left = parseExpression();
	if(left == nullptr)
		throw ParseException("cannot parse expression");
	node->add(left);

	ParseTreeNode *comparator = parseComparator();
	if(comparator == nullptr)
		throw ParseException("cannot parse comparator");
	node->add(comparator);

	ParseTreeNode *right = parseExpression();
	if(right == nullptr)
		throw ParseException("cannot parse expression");
	node->add(right);

	return node;
}

/**
 * 非終端記号COMPARATORの解析.
 * <pre>
 * 下記の構文ルールに相当
 * <COMPARATOR> := "=" | "<" | ">" | "<=" | ">=" | "<>" | "!="
 * </pre>
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseComparator()
{
	if(tokens->hasMoreTokens() != true)
		throw ParseException("comparator is expected, but no next token exists");

	LeafNode *node = new LeafNode(tokens->getToken());

	if(node->isComparator() != true)
		throw ParseException("'" + node->getValue() + "' is not a comparator");
	return node;
}

/**
 * 非終端記号CREATE_TABLEの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 *<CREATE_TABLE> := "CREATE" "TABLE" <IDENTIFIER> "(" <TABLE_ATTR_LIST> ")"
 *  </pre>
 *
 * @author	Nagoya University
 * @date	2022/02/10
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseCreateTable()
{
    IntermediateNode* node = new IntermediateNode(Symbol::CREATE_TABLE);

	/* "CREATE" */
	/* alread poped
	if(! tokens->hasMoreTokens() || ! iequals(tokens->getToken(), "CREATE"))
		throw ParseException("\"CREATE\" is expected, but not found");
	*/

	/* "TABLE" */
	if(! tokens->hasMoreTokens() || ! iequals(tokens->getToken(), "TABLE"))
		throw ParseException("\"TABLE\" is expected, but not found");

	/* <IDENTIFIER> */
	if(! tokens->hasMoreTokens() )
		throw ParseException("next token does not exist");
	ParseTreeNode *identifier = parseIdentifier();
	if(identifier == nullptr)
		throw ParseException("cannot parse identifier");
	node->add(identifier);

	/* "(" */
	if(tokens->compareToken("(") != true)
		throw ParseException("'(' is expected");

	/* <TABLE_ATTR_LIST> */
	ParseTreeNode *table_attr_list = parseTableAttrList();
	if(table_attr_list == nullptr)
		throw ParseException("cannnot parse Table_Attr_List");
	node->add(table_attr_list);

	/* ")" */
	if(tokens->compareToken(")") != true)
		throw ParseException("')' is expected");
	
	return node;
}

/**
 * 非終端記号DEFAULTの解析
 *
 * <pre>
 * 下記の構文ルールに相当
 * <DEFAULT> := "DEFAULT" <VALUE_ATTR>
 * </pre>
 *
 * @author	Nagoya University
 * @date	2022/02/15
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseDefault()
{
	IntermediateNode* node = new IntermediateNode(Symbol::DEFAULT);

	if(! tokens->hasMoreTokens() || ! iequals(tokens->getToken(), "DEFAULT"))
		throw ParseException("DEFAULT-clause must start with \"DEFAULT\"");

	ParseTreeNode* list = parseValueAttr();
	if(list == nullptr || list->getSize()==0)
		throw ParseException("parseValueAttr must not be null or empty.");
	node->add(list);

	return node;
}


/**
 * 非終端記号VALUE_ATTRの解析
 *
 * <pre>
 * 下記の構文ルールに相当
 * <VALUE_ATTR> := <NUMBER>|<WORDS>|<BOOL_VALUE>|<NULL>
 * </pre>
 *
 * @author	Nagoya University
 * @date	2022/02/15
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseValueAttr()
{
	if(tokens->hasMoreTokens() != true)
		throw ParseException("valueAttr is expected, but no next token exists");

	LeafNode *node = new LeafNode(tokens->getToken());

	if(node->isValues() != true) 
		throw ParseException("'" + node->getValue() + "' is not a value");
	return node;

}

/**
 * 非終端記号UPDATE TABLEの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 *<UPDATE_TABLE> := "UPDATE"  <IDENTIFIER> "SET" <SET_ATTR_LIST> (WHERE)?
 *  </pre>
 *
 * @author	Nagoya University
 * @date	2022/02/16
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseUpdateTable()
{
    IntermediateNode* node = new IntermediateNode(Symbol::UPDATE);
	
	logger->debug("UPDATE reach!!");

	/* "UPDATE" */
	if(! tokens->hasMoreTokens() || ! iequals(tokens->getToken(), "UPDATE"))
		throw ParseException("\"UPDATE\" is expected, but not found");

	/* <IDENTIFIER> */
	if(! tokens->hasMoreTokens() )
		throw ParseException("next token does not exist");
	ParseTreeNode *identifier = parseIdentifier();
	if(identifier == nullptr)
		throw ParseException("cannot parse identifier");
	node->add(identifier);

	/* "SET" */
	if(! tokens->hasMoreTokens() || ! iequals(tokens->getToken(), "SET"))
		throw ParseException("\"SET\" is expected, but not found");

	/* <SET_ATTR_LIST> */
	ParseTreeNode *set_attr_list = parseSetAttrList();
	if(set_attr_list == nullptr)
		throw ParseException("cannnot parse Set_Attr_List");
	node->add(set_attr_list);

	/* <WHERE> */
	if(tokens->hasMoreTokens() && iequals(tokens->lookAheadToken(), "WHERE")){
		ParseTreeNode *where_node = parseWhere();
		node->add(where_node);
	}
	return node;
}


/**
 * 非終端記号SET_ATTR_LISTの解析
 *
 * <pre>
 * 下記の構文ルールに相当
 * <SET_ATTR_LIST> := <SET_ATTR>(<COMMMA>(SET_ATTR>)*
 * </pre>
 *
 * @author	Nagoya University
 * @date	2022/02/16
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseSetAttrList()
{
	logger->debug("parseSetAttrList reach!!");

	IntermediateNode *node = new IntermediateNode(Symbol::SET_ATTR_LIST);

	do {
		/* <SET_ATTR> */
		ParseTreeNode *setAttr = parseSetAttr();
		if(setAttr == nullptr)
			throw ParseException("SetAttr is expected, but not found");
		node->add(setAttr);

	} while(tokens->compareToken(",") == true);

	return node;
}
/**
 * 非終端記号SET_ATTRの解析
 *
 * <pre>
 * 下記の構文ルールに相当
 * <SET_ATTR> := <IDENTIFIER><EQ><EXPRESSION>
 * </pre>
 *
 * @author	Nagoya University
 * @date	2022/02/16
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseSetAttr()
{
	logger->debug("parseSetAttr reach!!");
	
	IntermediateNode *node = new IntermediateNode(Symbol::SET_ATTR);

	/* <IDENTIFIER> */
	if(! tokens->hasMoreTokens() )
		throw ParseException("next token does not exist");
	ParseTreeNode *identifier = parseIdentifier();
	if(identifier == nullptr)
		throw ParseException("cannot parse identifier");
	node->add(identifier);

    /*<EQ> */
	if(tokens->compareToken("=") != true)
		throw ParseException("'=' is expected");

	/* <EXPRESSION> */
	if(! tokens->hasMoreTokens() )
		throw ParseException("next token does not exist");
	ParseTreeNode *expression = parseExpression();
	if(expression == nullptr)
		throw ParseException("expression is expected, but not found");
	node->add(expression);

	logger->debug("parseSetAttr DONE");

	return node;
}

/**
 * 非終端記号DELETE_TABLEの解析
 *
 * <pre>
 * 下記の構文ルールに相当
 * <DELETE_TABLE> := "DELETE" "FROM" <IDENTIFIER><WHERE>
 * </pre>
 *
 * @author	Nagoya University
 * @date	2022/02/16
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseDeleteTable()
{
	logger->debug("parseDeleteTable reach!!");
	IntermediateNode *node = new IntermediateNode(Symbol::DELETE);

	/* DELETE */
	if(! tokens->hasMoreTokens() || ! iequals(tokens->getToken(), "DELETE"))
		throw ParseException("\"DELETE\" is expected, but not found");

	/* "FROM" */
	if(! tokens->hasMoreTokens() || ! iequals(tokens->getToken(), "FROM"))
		throw ParseException("\"FROM\" is expected, but not found");

	/* <IDENTIFIER> */
	if(! tokens->hasMoreTokens() )
		throw ParseException("next token does not exist");
	ParseTreeNode *identifier = parseIdentifier();
	if(identifier == nullptr)
		throw ParseException("cannot parse identifier");
	node->add(identifier);

	/* <WHERE> */
	if(tokens->hasMoreTokens() && iequals(tokens->lookAheadToken(), "WHERE")){
		ParseTreeNode *where_node = parseWhere();
		node->add(where_node);
	}

	return node;
}

/**
 * 非終端記号INSERT_INTOの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <INSERT_INTO> := "INSERT" "INFO" <IDENTIFIER> ( "(" <TABLE_ATTR_LIST> ")" )? "VALUES" "(" <VALUE_ATTR_LIST> ")"
 *               |  "INSERT" "INFO" <IDENTIFIER> ( "(" <TABLE_ATTR_LIST> ")" )? ( subquery | union )
 *  </pre>
 *
 * @author  Nagoya University
 * @date	2022/02/16
 * @return	構文解析後のツリーへのポインタ
 */
ParseTreeNode* QueryParser::parseInsertInto()
{
	
    IntermediateNode* node = new IntermediateNode(Symbol::INSERT_INTO);

        /* "INSERT" */
        if(! tokens->hasMoreTokens() || ! iequals(tokens->getToken(), "INSERT"))
                throw ParseException("\"INSERT\" is expected, but not found");

        /* "INTO" */
        if(! tokens->hasMoreTokens() || ! iequals(tokens->getToken(), "INTO"))
                throw ParseException("\"INTO\" is expected, but not found");

        /* <IDENTIFIER> */
        if(! tokens->hasMoreTokens() )
                throw ParseException("next token does not exist");
        ParseTreeNode *identifier = parseIdentifier();
        if(identifier == nullptr)
                throw ParseException("cannot parse identifier");
        node->add(identifier);

        /* "(" */
    	if (tokens->hasMoreTokens() && iequals(tokens->lookAheadToken(), "(")){

			tokens->getToken();

			/* <COLUMN_LIST> */
			ParseTreeNode *column_list = parseColumnList();
			if(column_list == nullptr)
					throw ParseException("cannnot parse Column_List");
			node->add(column_list);

			/* ")" */
			if(tokens->compareToken(")") != true)
				throw ParseException("')' is expected");
		}	
        
		/* "VALUES" */
        if(tokens->hasMoreTokens() && iequals(tokens->lookAheadToken(), "VALUES")){
        
			tokens->getToken();

			/* "(" */
    	    if(tokens->compareToken("(") != true)
                throw ParseException("'(' is expected");
        
			/* "VALUES" */
            ParseTreeNode *valueAttrList = parseValueAttrList();
            if(valueAttrList == nullptr)
                    throw ParseException("cannot parse identifier");
            node->add(valueAttrList);

            /* ")" */
            if(tokens->compareToken(")") != true)
                throw ParseException("')' is expected");
        }

        /* "subquery | union" */
        else if(tokens->hasMoreTokens() ){

			/* <SUBQUERY> */
			ParseTreeNode* subquery = parseSubquery();
			if(subquery == nullptr)
				throw ParseException("SELECT and FROM clauses are necessary");

			if(! tokens->hasMoreTokens() || ! iequals(tokens->lookAheadToken(), "UNION") ){
				node->add(subquery);
			}
			else {
				/* <UNION> := <SUBQUERY> ("UNION" <SUBQUERY>)+  */
				IntermediateNode *uni = new IntermediateNode(Symbol::UNION);
				uni->add(subquery);
				while(tokens->compareSymbol(Symbol::UNION) == true){
					subquery = parseSubquery();
					if(subquery == nullptr)
						throw ParseException("SELECT and FROM clauses are necessary");
					uni->add(subquery);
				}
				node->add(uni);
			}

        }

        return node;
}


/**
 * 非終端記号VALUE_ATTR_LISTの解析.
 *
 * <pre>
 * 下記の構文ルールに相当
 * <VALUE_ATTR_LIST> := <VALUE_ATTR> ( "," <VALUE_ATTR>)*
 * 
 * </pre>
 *
 * @author  Nagoya University
 * @date	2022/02/16
 * @return	構文解析後のツリーへのポインタ
 */

ParseTreeNode* QueryParser::parseValueAttrList()
{
	IntermediateNode *node = new IntermediateNode(Symbol::VALUE_ATTR_LIST);
	logger->debug("VALUE_ATTR_LIST reach!");
	do {
		/* <VALUE_ATTR> */
		ParseTreeNode*valueAttr = parseValueAttr();
		if(valueAttr == nullptr)
			throw ParseException("ValueAttr is expected, but not found");
		node->add(valueAttr);

	} while(tokens->compareToken(",") == true);

	return node;
}