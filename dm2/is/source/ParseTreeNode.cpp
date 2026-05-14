/*
 * クラス ParseTreeNode
 *
 * 構文解析結果の木構造を表現するクラスの実装．
 * ただし，ほとんど抽象メソッドのため，実装内容は少ない．
 */

/* クラス定義ヘッダファイル */
#include "Parser.h"

/* 依存ライブラリ */
#include <string>

using namespace IS;

/**
* デストラクタ.
*
* @author	Yousuke Watanabe
* @date	2018/03/22
*/
ParseTreeNode::~ParseTreeNode()
{
}

/**
 * シンボルと文字列の変換をするクラスメソッド.
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/09
 * @param	s	変換対象になるシンボル
 * @return	シンボルを文字列変換したもの
 */
string ParseTreeNode::convertSymbolToString(Symbol s)
{
    switch(s) {
        case Symbol::QUERY:
            return "QUERY";
        case Symbol::CREATE_STREAM:
            return "CREATE_STREAM";
        case Symbol::CREATE_TABLE:
            return "CREATE_TABLE";
        case Symbol::TABLE_ATTR_LIST:
            return "TABLE_ATTR_LIST";
        case Symbol::TABLE_ATTR:
            return "TABLE_ATTR";
        case Symbol::DATE_PATTERN:
            return "DATE_PATTERN";
        case Symbol::DROP_STREAM:
            return "DROP_STREAM";
        case Symbol::DROP_TABLE:
            return "DROP_TABLE";
        case Symbol::MASTER:
            return "MASTER";
        case Symbol::MASTER_LIST:
            return "MASTER_LIST";
        case Symbol::IDENTIFIER:
            return "IDENTIFIER";
        case Symbol::WITH_RECURSIVE:
            return "WITH_RECURSIVE";
        case Symbol::SUBQUERY:
            return "SUBQUERY";
        case Symbol::UNION:
            return "UNION";
        case Symbol::SELECT:
            return "SELECT";
        case Symbol::ASTERISK:
            return "ASTERISK";
        case Symbol::SELECT_LIST:
            return "SELECT_LIST";
        case Symbol::EXPRESSION_LIST:
            return "EXPRESSION_LIST";
        case Symbol::EXPRESSION:
            return "EXPRESSION";
        case Symbol::EXPRESSION_RENAME:
            return "EXPRESSION_RENAME";
        case Symbol::FUNCTION:
            return "FUNCTION";
        case Symbol::FROM:
            return "FROM";
        case Symbol::FROM_LIST:
            return "FROM_LIST";
        case Symbol::TABLE:
            return "TABLE";
        case Symbol::TABLE_RENAME:
            return "TABLE_RENAME";
        case Symbol::AS:
            return "AS";
        case Symbol::WINDOW:
            return "WINDOW";
        case Symbol::PARTITION:
            return "PARTITION";
        case Symbol::HIERARCHY:
            return "HIERARCHY";
        case Symbol::WHERE:
            return "WHERE";
        case Symbol::PREDICATE_LIST:
            return "PREDICATE_LIST";
        case Symbol::AND:
            return "AND";
        case Symbol::PREDICATE:
            return "PREDICATE";
        case Symbol::COMPARATOR:
            return "COMPARATOR";
        case Symbol::NUMBER:
            return "NUMBER";
        case Symbol::SIGN:
            return "SIGN";
        case Symbol::STRING:
            return "STRING";
        case Symbol::SEMICOLON:
            return "SEMICOLON";
        case Symbol::NULL_VALUE:
            return "NULL";
        case Symbol::BOOL_VALUE:
            return "BOOL";
        default:
            return "UNKNOWN";
    }
}


/**
 * ノードの子ノードすべてをSQL文字列として統合する.
 *
 * @author	Yousuke Watanabe
 * @date	2018/03/22
 * @param	separator	ノード同士の文字列の区切りに入れる文字列
 * @return	統合結果となるSQL形式の文字列
 */
string ParseTreeNode::concatChildNodes(string separator)
{
	string rval = "";
	for(int i=0; i < getSize(); i++){
		rval += getChildNode(i)->toSQLString();
		if(i+1 < getSize())
			rval += separator;
	}
	return rval;
}

/**
 * ノードの子ノードすべてをSQL文字列として統合する.
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/07
 * @param	separator1	ノード同士の文字列の区切りに入れる文字列(奇数要素の後)
 * @param	separator2	ノード同士の文字列の区切りに入れる文字列(偶数要素の後)
 * @return	統合結果となるSQL形式の文字列
 */
string ParseTreeNode::concatChildNodes(string separator1, string separator2)
{
	string rval = "";
	for(int i=0; i < getSize(); i++){
		rval += getChildNode(i)->toSQLString();
		if(i+1 < getSize()){
			if(i % 2 == 0)
				rval += separator1;
			else
				rval += separator2;
		}
	}
	return rval;
}


/**
 * ノードの内容をSQL文字列に変換をする.
 *
 * @author	Yousuke Watanabe
 * @date	2019/04/09
 * @return	ノードの変換結果となるSQL形式の文字列
 */
string ParseTreeNode::toSQLString()
{
	Symbol s = getSymbol();
    LoggerPtr logger = Logger::getLogger("ParseTreeNode");
    string sqlString="";

    
    switch(s) {
        case Symbol::QUERY:
            return concatChildNodes(" ");
        case Symbol::CREATE_STREAM:
			if(getSize() == 2)
				return "CREATE STREAM " + getChildNode(0)->getValue() + " ( " + getChildNode(1)->toSQLString() + " ) ";
			else if(getSize() == 4)
				return "CREATE STREAM " + getChildNode(0)->getValue() + " ( " + getChildNode(1)->toSQLString() + " ) " + getChildNode(2)->getValue() + " [" + getChildNode(3)->toSQLString() + "]";
			else
				return "CREATE STREAM " + getChildNode(0)->getValue() + " ( " + getChildNode(1)->toSQLString() + " ) " + getChildNode(2)->getValue() + " [" + getChildNode(3)->toSQLString() + "]" + getChildNode(4)->getValue() + " [" + getChildNode(5)->toSQLString() + "]";
       case Symbol::CREATE_TABLE:
            return "CREATE TABLE " +  getChildNode(0)->getValue() + " ( " + getChildNode(1)->toSQLString() + " ) ";
        case Symbol::TABLE_ATTR_LIST:
            logger->debug("TABLE_ATTR_LIST reached");
            return concatChildNodes(", ");
        case Symbol::TABLE_ATTR:
            logger->debug("TABLE_ATTR reached");
            return  concatChildNodes(" ");
        case Symbol::DATE_PATTERN:
            return concatChildNodes(" ");
        case Symbol::DROP_STREAM:
			return "DROP STREAM " + getChildNode(0)->getValue();
        case Symbol::DROP_TABLE:
			return "DROP TABLE " + getChildNode(0)->getValue();
        case Symbol::UPDATE:
            if(getSize() == 2)
                return "UPDATE " + getChildNode(0)->getValue() + " SET " + getChildNode(1)->toSQLString();
            else
                return "UPDATE " + getChildNode(0)->getValue() + " SET " + getChildNode(1)->toSQLString() + getChildNode(2)->toSQLString();
        case Symbol::DELETE:
            if(getSize() == 1)
                return "DELETE FROM " + getChildNode(0)->getValue() ;
            else
                return "DELETE FROM " + getChildNode(0)->getValue() + " " +  getChildNode(1)->toSQLString();
        case Symbol::MASTER:
            return "MASTER " + getChildNode(0)->toSQLString();
        case Symbol::MASTER_LIST:
            return concatChildNodes(" , ");
        case Symbol::COLUMN_LIST:
           return concatChildNodes(" , ");
        case Symbol::SET_ATTR_LIST:
            return concatChildNodes(" , ");
        case Symbol::SET_ATTR:
            return  getChildNode(0)->getValue()  + " = " + getChildNode(1)->toSQLString();
        case Symbol::IDENTIFIER:
            return getValue();
        case Symbol::WITH_RECURSIVE:
            return "WITH RECURSIVE " + getChildNode(0)->toSQLString() + " AS ( " + getChildNode(1)->toSQLString() + " )";
        case Symbol::SUBQUERY:
            return concatChildNodes(" ");
        case Symbol::UNION:
            return concatChildNodes(" UNION ");
        case Symbol::SELECT:
            return "SELECT " + getChildNode(0)->toSQLString();
        case Symbol::ASTERISK:
            return "*";
        case Symbol::SELECT_LIST:
            return concatChildNodes(" , ");
        case Symbol::EXPRESSION_LIST:
            return concatChildNodes(" , ");
        case Symbol::EXPRESSION:
            return getChildNode(0)->toSQLString();
        case Symbol::EXPRESSION_RENAME:
            return getChildNode(1)->toSQLString() + " AS " + getChildNode(0)->getValue();
        case Symbol::FUNCTION:
            return getChildNode(0)->toSQLString() + "(" + getChildNode(1)->toSQLString() + ")";
        case Symbol::FROM:
            return "FROM " + getChildNode(0)->toSQLString();
        case Symbol::FROM_LIST:
            return concatChildNodes(" , ");
        case Symbol::INSERT_INTO:
            sqlString.append("INSERT INTO " + getChildNode(0)->getValue());
            for(int index=1; index < getSize();index++){
                switch(getChildNode(index)->getSymbol()){
                    case Symbol::COLUMN_LIST:
                       logger->debug("COLUMN_LIST reached");
                        sqlString.append(" ( " + getChildNode(index)->toSQLString() + " ) ");
                        break;
                    case Symbol::VALUE_ATTR_LIST:
                        sqlString.append(" VALUES ( " + getChildNode(index)->toSQLString() + " ) ");
                        break;
                    default:    /* subquery|union */
                        sqlString.append(" " + getChildNode(index)->toSQLString());
                        break;
                }
            }
            logger->debug(sqlString);
            return sqlString;
        case Symbol::VALUE_ATTR_LIST:
            return concatChildNodes(" , ");
        case Symbol::TABLE:
			if(getChildNode(0)->getSymbol() == Symbol::IDENTIFIER){
				if(getSize() == 1)
					return getChildNode(0)->toSQLString();
				else if(getSize() == 2)
					return getChildNode(0)->toSQLString() + "[" + getChildNode(1)->toSQLString() +"]";
				else 
					return getChildNode(0)->toSQLString() + "[" + getChildNode(1)->toSQLString() + ", " + getChildNode(2)->toSQLString() +"]";
			}
			else if(getChildNode(0)->getSymbol() == Symbol::SUBQUERY)
				return "( " + getChildNode(0)->toSQLString() + " )";
        case Symbol::TABLE_RENAME:
            return getChildNode(1)->toSQLString() + " AS " + getChildNode(0)->toSQLString();
        case Symbol::DEFAULT:
            logger->debug("DEFAULT reached");
            return "DEFAULT " + getChildNode(0)->toSQLString();
        case Symbol::AS:
            return "AS";
        case Symbol::WINDOW:
            return concatChildNodes(" ");
        case Symbol::PARTITION:
            return concatChildNodes("PARTITION BY");
        case Symbol::HIERARCHY:
            return getChildNode(0)->getValue();
        case Symbol::WHERE:
            return " WHERE " + getChildNode(0)->toSQLString();
        case Symbol::PREDICATE_LIST:
            return concatChildNodes(" AND ");
        case Symbol::AND:
            return "AND";
        case Symbol::PREDICATE:
            return concatChildNodes(" ");
        case Symbol::COMPARATOR:
            return getValue();
        case Symbol::NUMBER:
            return getValue();
        case Symbol::SIGN:
            return getValue();
        case Symbol::STRING:
            return getValue();
        case Symbol::BOOL_VALUE:
            return getValue();
        case Symbol::SEMICOLON:
            return ";";
        case Symbol::NULL_VALUE:
            return "NULL";
        default:
            return "";
    }
}


