#include <iostream>

#include "is/Schema.h"
#include "is/StringUtil.h"
#include <strings.h>

using std::cout;                     // cout
using std::cerr;                     // cerr
using std::endl;                     // endl
using std::string;                   // string

namespace IS {
	StringUtil stringUtil;

	// 初期化
	const string Schema::RDB = "RDB";
	const string Schema::STREAM = "STREAM";
	const string Schema::UNKNOWN = "UNKNOWN";

	const string Schema::COL_USERID = "dm2_creator_id";
	const string Schema::COL_IPADDR = "dm2_ip_addr";
	const string Schema::COL_HASH = "dm2_hash";
	const string Schema::COL_TS_GRP = "dm2_ts_group";
	vector<string> adminColumnNameList{Schema::COL_USERID, Schema::COL_IPADDR, Schema::COL_HASH, Schema::COL_TS_GRP};
	vector<string> adminColumnTypeList{"string", "string", "string", "string"};

	Attribute::Attribute()
	{
		name.clear();
		type.clear();
	}

	Attribute::~Attribute()
	{
		name.clear();
		type.clear();
	}

    /**
     * コンストラクタ：（引数なし）
     *
     * @author	Nagoya University
     * @date	2018/03/14
     */

    Schema::Schema()
    {
        attributes.clear();
        //tabletype = UNKNOWN;
        //tablename = UNKNOWN;
		userAccessMap.clear();
		colNameIdxMap.clear();
		evalMinIdxMap.clear();
		evalMaxIdxMap.clear();
    }

	/**
	 * コンストラクタ：（テーブル名、テーブルタイプ）
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	tableName	テーブル名
	 * @param	type	 	テーブルタイプ(RDB, STREAM, UNKNOWN)
	 */

	Schema::Schema(const string &tableName, const string &type)
	{
		attributes.clear();
		tabletype = type;
		tablename = tableName;
		reltype = NONE;
		userAccessMap.clear();
		colNameIdxMap.clear();
		evalMinIdxMap.clear();
		evalMaxIdxMap.clear();
	}

	/**
	* コンストラクタ：（テーブル名、テーブルタイプ、リレーションタイプ）
	*
	* @author	Nagoya University
	* @date	2020/01/28
	*
	* @param	tableName	テーブル名
	* @param	type	 	テーブルタイプ(RDB, STREAM, UNKNOWN)
	* @param	relType	 	リレーションタイプ
	*/

	Schema::Schema(const string &tableName, const string &type, relationType relType)
	{
		attributes.clear();
		tabletype = type;
		tablename = tableName;
		reltype = relType;
		userAccessMap.clear();
		colNameIdxMap.clear();
		evalMinIdxMap.clear();
		evalMaxIdxMap.clear();
	}

	/**
	* コンストラクタ：（テーブル名、列情報リスト、テーブルタイプ）
	*
	* @author	Nagoya University
	* @date	2019/01/30
	*
	* @param	name	テーブル名
	* @param	attrs	列情報リスト
	* @param	type 	テーブルタイプ(RDB, STREAM, HISTORY, UNKNOWN)
	*/

	Schema::Schema(const string &name, const std::vector<Attribute> &attrs, const string &type)
	{
		if (attrs.empty() ||
			((type != RDB) && (type != STREAM) && (type != UNKNOWN)))
		{
			cerr << "Auguments are invalid. type:" << type;
		}

		attributes.clear();
		colNameIdxMap.clear();
		evalMinIdxMap.clear();
		evalMaxIdxMap.clear();
		for (int index = 0; index < (int)(attrs.size()); index++)
		{
			Attribute attribute;;
			attribute.name = attrs[index].name;
			attribute.type = attrs[index].type;
			attribute.orgTableName = name;
			attribute.defaultValue =attrs[index].defaultValue; /* default句対応 */
			colNameIdxMap[attrs[index].name] = attributes.size();
			attributes.push_back(attribute);
		}
		tabletype = type;
		tablename = name;
		userAccessMap.clear();
	}

    /**
     * デストラクタ
     *
     * @author	Nagoya University
     * @date	2018/03/14
     */

    Schema::~Schema()
    {
		if (attributes.size() > 0) {
			clear();
		}
    }

	/**
	* クリア
	*
	* @author	Nagoya University
	* @date	2018/03/14
	*/

	void Schema::clear()
	{
		attributes.clear();
		attributes.shrink_to_fit();
		relationTableList.clear();
		relationTableList.shrink_to_fit();
		userAccessMap.clear();
		colNameIdxMap.clear();
		evalMinIdxMap.clear();
		evalMaxIdxMap.clear();
	}

    /**
     * 列情報の追加
     *
     * @author	Nagoya University
     * @date	2018/03/14
     *
     * @param	attr	列情報
     */

    void Schema::append(const Attribute &attr)
    {
		Attribute attribute;
        attribute.name = attr.name;
		attribute.type = attr.type;
		attribute.defaultValue = attr.defaultValue;  /* default句対応 */
		
		stringUtil.toLower(attribute.type);
		if (!stringUtil.checkSupportDataType(attribute.type)) {
			cerr << "[append] Not Support DataType. tableName:" << tablename << " attr.name:" << attr.name << " attr.type:" << attribute.type << endl;
		}
		if (attr.orgTableName.empty()) {
			attribute.orgTableName = tablename;
		}
		else {
			attribute.orgTableName = attr.orgTableName;
		}
		attribute.hidden = attr.hidden;
		colNameIdxMap[attr.name] = attributes.size();
        attributes.push_back(attribute);
    }

	/**
	* ユーザ定義関数の列情報の追加
	*
	* @author	Shinichi Kusayama
	* @date	2023/10/18
	*
	*/
	void Schema::appendEvalColumnRange(const string &evalName, int minIdx, int maxIdx)
	{
		evalMinIdxMap[evalName] = minIdx;
		evalMaxIdxMap[evalName] = maxIdx;
	}

	/**
	* 管理者列情報の追加
	*
	* @author	Nagoya University
	* @date	2019/07/25
	*
	*/

	void Schema::appendAdminColumn()
	{
		for (int i = 0; i < adminColumnNameList.size(); i++) {
			Attribute attr;
			attr.name = adminColumnNameList[i];
			attr.type = adminColumnTypeList[i];
			attr.hidden = true;
			attr.orgTableName = tablename;
			colNameIdxMap[attr.name] = attributes.size();
			attributes.push_back(attr);
		}
	}

	/**
	* 列情報のコピー
	*
	* @author	Nagoya University
	* @date	2019/07/25
	*
	* @param	schema	スキーマ情報
	*/

	void Schema::copyAttributes(const Schema &schema)
	{
		attributes.clear();
		colNameIdxMap.clear();
		attributes = schema.getAttributes();
		
		for (unsigned int i = 0; i < attributes.size(); i++) {
			colNameIdxMap[attributes.at(i).name] = i;
		}
	}

	/**
	* 列情報の追加
	*
	* @author	Nagoya University
	* @date	2019/07/25
	*
	* @param	schema	スキーマ情報	
	*/

	void Schema::addAttributes(const Schema &schema)
	{
		if (this->tablename != schema.getTableName()) {
			vector<string> colNameSep;
			
			// 違うテーブル名でaddされた場合
			for (unsigned int i = 0; i < attributes.size(); i++) {
				stringUtil.splitExcludeBrackets(attributes[i].name, ".", colNameSep);
				if (colNameSep.size() == 1) {
					// カラム名のみ保持していた場合はテーブル名を付与する
					attributes[i].name = this->tablename + "." + attributes[i].name;
				}
			}
			colNameIdxMap.clear();
			for (unsigned int i = 0; i < attributes.size(); i++) {
				colNameIdxMap[attributes.at(i).name] = i;
			}

			for (int i = 0; i < schema.getAttributeSize(); i++) {
				Attribute attr = schema.getAttribute(i);
				stringUtil.splitExcludeBrackets(attr.name, ".", colNameSep);
				if (colNameSep.size() == 1) {
					// カラム名のみ保持していた場合はテーブル名を付与する
					attr.name = schema.getTableName() + "." + attr.name;
				}
				colNameIdxMap[attr.name] = attributes.size();
				attributes.push_back(attr);
			}
		}
		else {
			// 同じテーブル名でaddされた場合
			for (int i = 0; i < schema.getAttributeSize(); i++) {
				Attribute attr = schema.getAttribute(i);
				colNameIdxMap[attr.name] = attributes.size();
				attributes.push_back(attr);
			}
		}
		
	}

	/**
	* アクセス権限情報の追加
	*
	* @author	Nagoya University
	* @date	2019/09/04
	*
	* @param	accessTxt	アクセス権限文字列
	*/

	void Schema::setAccessPrivileges(const string &accessTxt)
	{
		vector<string> userList = stringUtil.split(accessTxt, ",");
		for (string s : userList) {
			vector<string> sList = stringUtil.split(s, "=");
			if (sList.size() == 2) {
				userAccessMap[sList.at(0)] = sList.at(1);
				//logger->debug("[setAccessPrivileges] TableName:" + tablename + " User:" + sList.at(0) + " accessPrivileges:" + sList.at(1));
			}
			else {
				cerr << "[setAccessPrivileges] invalied parameter Expect:'userName=ar' Please confirm " << tablename << ".xml" << endl;
			}
		}
	}

	/**
	* 関連テーブルリストの設定
	*
	* @author	Nagoya University
	* @date	2020/02/04
	*
	* @param	list	関連テーブルリスト
	*/

	void Schema::setRelationTableList(const vector<string> &list)
	{
		this->relationTableList = list;
	}

    /**
     * テーブル名を更新する
     *
     * @author	Nagoya University
     * @date	2018/03/14
     *
     * @param	from	変更前テーブル名
     * @param	to  	変更後テーブル名
     *
     * @return	True	テーブル名変更の成功
     * 			false	テーブル名変更の失敗
     */

    bool Schema::renameTableName(const string &from, const string &to)
    {
		if (from == to) {
			//logger->debug("[renameTableName] Already renameTable. from:" + from + " to:" + to);
			return true;
		}
        if(from.empty() || to.empty())
        {
			cerr << "[renameTableName] Arguments are invalid. from:" << " to:" << to << endl;
            return false;
        }

        bool found = false;
        if(strcasecmp(tablename.c_str(), from.c_str()) == 0) {
            tablename = to;
            found = true;
		}
		else {
			//logger->debug("[renameTableName] Arguments are invalid. from:" + from + " original:" + tablename);
			tablename = to;
			return true;
		}
        
        return found;
    }

    /**
     * カラム名を変更する
     *
     * @author	Nagoya University
     * @date	2018/03/14
     *
     * @param	from	変更前カラム名
     * @param	to  	変更後カラム名
     *
     * @return	True	カラム名変更の成功
     * 			false	カラム名変更の失敗
     */

    bool Schema::renameColumnName(const string &from, const string &to)
    {
		if (from == to) {
			//logger->debug("[renameColumnName] Already rename column. from:" + from + " to:" + to);
			return true;
		}
        if(from.empty() || to.empty())
        {
			cerr << "[renameColumnName] Arguments are invalid. from:" << from << " to:" << to << endl;
            return false;
        }

        bool found = false;

		int index = getAttributeIdx(from);
		if (index >= 0) {
			attributes.at(index).name = to;
			if (stringUtil.contain(from, ".") && stringUtil.starts_with(from, tablename + ".")) {
				colNameIdxMap.erase(stringUtil.split(from, ".")[1]);
				colNameIdxMap.erase(from);
			}
			else {
				colNameIdxMap.erase(from);
			}
			colNameIdxMap[to] = index;
			return true;
		}
		else {
			string attrStr;
			for (Attribute attr : attributes) {
				if (attrStr.length() != 0) attrStr.append(",");
				attrStr.append(attr.name);
			}
			cerr << "[renameColumnName] Attribute not found. fromAttrName:" << from << " myTableName:" << tablename << " attrNameList:" << attrStr << endl;
		}

        return found;

    }

    /**
     * 列情報リストを取得する
     *
     * @author	Nagoya University
     * @date	2018/03/14
     *
     * @return	列情報リスト
     */

    std::vector<Attribute> Schema::getAttributes() const
    {
        return attributes;
    }

    /**
     * index指定で列情報を取得する
     *
     * @author	Nagoya University
     * @date	2018/03/14
     *
     * @param	index	列番号
     *
     * @return	列情報
     */

    Attribute Schema::getAttribute(int index) const
    {
        if(index < 0 || index >= (int)(attributes.size()))
        {
			cerr << "Index Out of Bounds Exception. :" << std::to_string(index) << endl;
			Attribute attr;
			return attr;
        }

        return attributes[index];
    }


	/**
	* index指定で列情報(defaultValue)を取得する
	*
	* @author	Nagoya University
	* @date	2022/02/18
	*
	* @param	index	列番号
	*
	* @return	type情報
	*/

	string Schema::getAttributeDefaultValue(int index) const
	{
		if (index < 0 || index >= (int)(attributes.size()))
		{
			cerr << "Index Out of Bounds Exception. :" << std::to_string(index) << endl;
		}

		return attributes[index].defaultValue;
	}

	/**
	* index指定で列情報(Type)を取得する
	*
	* @author	Nagoya University
	* @date	2019/02/12
	*
	* @param	index	列番号
	*
	* @return	type情報
	*/

	string Schema::getAttributeType(int index) const
	{
		if (index < 0 || index >= (int)(attributes.size()))
		{
			cerr << "Index Out of Bounds Exception. :" << std::to_string(index) << endl;
		}

		return attributes[index].type;
	}

	/**
	* index指定で列情報(Name)を取得する
	*
	* @author	Nagoya University
	* @date	2019/02/12
	*
	* @param	index	列番号
	*
	* @return	Name情報
	*/

	string Schema::getAttributeName(int index) const
	{
		if (index < 0 || index >= (int)(attributes.size()))
		{
			cerr << "Index Out of Bounds Exception. :" << std::to_string(index) << endl;
		}

		return attributes[index].name;
	}

	/**
	* index指定で列情報(隠し属性)を取得する
	*
	* @author	Nagoya University
	* @date	2019/02/12
	*
	* @param	index	列番号
	*
	* @return	隠し属性情報
	*/

	bool Schema::getAttributeHidden(int index) const
	{
		if (index < 0 || index >= (int)(attributes.size()))
		{
			cerr << "Index Out of Bounds Exception. :" << std::to_string(index) << endl;
		}

		return attributes[index].hidden;
	}

	/**
	* index指定で列情報(orgTableName)を取得する
	*
	* @author	Nagoya University
	* @date	2019/02/12
	*
	* @param	index	列番号
	*
	* @return	オリジナル時のテーブル名情報
	*/

	string Schema::getAttributeOrgTblName(int index) const
	{
		if (index < 0 || index >= (int)(attributes.size()))
		{
			cerr << "Index Out of Bounds Exception. :" << std::to_string(index) << endl;
		}

		return attributes[index].orgTableName;
	}

	/**
	* リレーションタイプを取得する
	*
	* @author	Nagoya University
	* @date	2020/01/28
	*
	*
	* @return	リレーションタイプ
	*/

	Schema::relationType Schema::getRelationType() const
	{
		return reltype;
	}

    /**
     * 列名指定で列情報を取得する
     *
     * @author	Nagoya University
     * @date	2018/03/14
     *
     * @param	name	列名
     *
     * @return	列情報
     */

    Attribute Schema::getAttribute(string name) const
    {
		Attribute rval;

		int index = getAttributeIdx(name);
		if (index >= 0) {
			return attributes.at(index);
		}
		else {
			cerr << "Attribute not found:" << name << endl;
		}
        return rval;
    }

	/**
	* 列名指定でインデックスを取得する
	*
	* @author	Nagoya University
	* @date	2020/02/18
	*
	* @param	name	列名
	* @param	isLog	ログ出力可否
	*
	* @return	インデックス
	*/

	int Schema::getAttributeIdx(const string &name, bool isLog) const
	{
		int retIdx = -1;
		int match = 0;
		string attrStr;

		if (colNameIdxMap.count(name) != 0) {
			// 指定した名前が一致した場合
			return colNameIdxMap.at(name);
		}

		if (stringUtil.starts_with(name, tablename + ".")) {
			// テーブル名.カラム名を指定した場合
			string colName = stringUtil.split(name, ".")[1];
			if (colNameIdxMap.count(colName) != 0) {
				return colNameIdxMap.at(colName);
			}
		}
		else {
			// カラム名指定の場合
			if (stringUtil.starts_with(attributes.at(0).name, tablename + ".")) {
				// attributeにテーブル名が付与されている場合
				vector<string> ret;
				for (unsigned int i = 0; i < attributes.size(); i++) {
					ret.clear();
					stringUtil.splitExcludeBrackets(attributes.at(i).name, ".", ret);
					if (ret.size() == 2) {
						if (name == ret[1]) {
							retIdx = i;
							match++;
						}
					}					
				}
				if (match == 1) {
					return retIdx;
				}
				else if (match > 1) {
					retIdx = -1;
					for (Attribute attr : attributes) {
						if (attrStr.length() != 0) attrStr.append(", ");
						attrStr.append(attr.name);
					}
					cerr << "[getAttributeIdx] Attribute is duplicated(count:" << to_string(match) << "). Please add tablename. AttrName:" << name << " myTableName:" << tablename << " myAttrNameList:" << attrStr << endl;
				}
			}
		}
		if (attrStr.length() == 0) {
			for (Attribute attr : attributes) {
				if (attrStr.length() != 0) attrStr.append(", ");
				attrStr.append(attr.name);
			}
		}
		if (isLog && match == 0) {
			cerr << "[getAttributeIdx] Attribute not found. AttrName:" << name << " myTableName:" << tablename << " myAttrNameList:" << attrStr << endl;
		}
		return retIdx;
	}

	/**
	* 列名指定でインデックスを取得する
	*
	* @author	Nagoya University
	* @date	2019/06/27
	*
	* @param	name	列名
	*
	* @return	インデックス
	*/

	int Schema::getAttributeIdx(const string &name) const
	{
		return getAttributeIdx(name, true);
	}

	/**
	 * 指定した列名の存在可否を判定する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	name	列名
	 *
	 * @return	True	存在する
	 * 			false	存在しない
	 */

	bool Schema::isExistAttribute(const string &name) const
	{
		//logger->debug("[isExistAttribute] name:" + name + " tablename:" + tablename);
		if (getAttributeIdx(name, false) >= 0) {
			return true;
		}
		else
		{
			if (evalMinIdxMap.find(name) != evalMinIdxMap.end()) {
				return true;
			}
			return false;
		}
	}

	/**
	 * 指定した列名の存在可否を判定する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	name	列名
	 *
	 * @return	True	存在する
	 * 			false	存在しない
	 */

	bool Schema::isExistAttribute(const string &name, int &minIdx, int &maxIdx) 
	{
		//logger->debug("[isExistAttribute] name:" + name + " tablename:" + tablename);
		if (getAttributeIdx(name, false) >= 0) {
			return true;
		}
		else
		{
			if (evalMinIdxMap.find(name) != evalMinIdxMap.end()) {
				minIdx = evalMinIdxMap[name];
				maxIdx = evalMaxIdxMap[name];
				return true;
			}
			return false;
		}
	}
    /**
     * テーブルタイプを取得する
     *
     * @author	Nagoya University
     * @date	2018/03/14
     *
     * @return	テーブルタイプ
     */

    string Schema::getTableType() const
    {
        return tabletype;   
    }

	/**
	 * テーブル名を取得する
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @return	テーブル名
	 */

	string Schema::getTableName() const
	{
		return tablename;
	}

	/**
	* 関連テーブルリストを取得する
	*
	* @author	Nagoya University
	* @date	2020/02/04
	*
	* @return	関連テーブルリスト
	*/

	vector<string> Schema::getRelationTableList() const
	{
		return this->relationTableList;
	}

	/**
	* 指定したユーザの読み取り可否を取得する
	*
	* @author	Nagoya University
	* @date	2019/09/04
	*
	* @param	userName	ユーザ名
	* @return	読み取り可否
	*/
	bool Schema::isSelect(const string &userName)
	{
		return checkAccessMap(userName, "r");
	}

	/**
	* 指定したユーザの追加可否を取得する
	*
	* @author	Nagoya University
	* @date	2019/09/04
	*
	* @param	userName	ユーザ名
	* @return	追加可否
	*/
	bool Schema::isAppend(const string &userName)
	{
		return checkAccessMap(userName, "a");
	}

	/**
	* 指定したユーザと操作文字列から許可可否を取得する
	*
	* @author	Nagoya University
	* @date	2019/09/04
	*
	* @param	userName	ユーザ名
	* @param	op			操作文字
	* @return	許可可否
	*/
	bool Schema::checkAccessMap(const string &userName, const string &op)
	{
		string operation;
		if (op == "r") operation = "reading";
		else if (op == "a") operation = "appending";
		// "R"の管理者情報読出し権限に関しては付与されていない場合が大半なのでログ出力しない

		if (userAccessMap.count(userName) > 0) {
			if (userAccessMap[userName].find(op) != std::string::npos) return true;
			else if(operation.length() != 0) cerr << "[checkAccessMap] User '" << userName << "' has no permission for " << operation << " stream '" << tablename << "'." << endl;
		}
		else if (userAccessMap.count("public") > 0) {
			if (userAccessMap["public"].find(op) != std::string::npos) return true;
			else if (operation.length() != 0) cerr << "[checkAccessMap] Users '" << userName << "' and 'public' have no permission for " << operation << " stream '" << tablename << "'." << endl;
		}
		else if (operation.length() != 0)
			cerr << "[checkAccessMap] No permissions for Users '" << userName << "' and 'public' for " << operation << " stream '" << tablename << "'." << endl;
		return false;
	}

	/**
	* 管理者カラムの参照権限可否を取得する
	*
	* @author	Nagoya University
	* @date	2019/09/04
	*
	* @param	userName	ユーザ名
	* @return	参照権限可否
	*/
	bool Schema::isRefAdminCol(const string &userName)
	{
		return checkAccessMap(userName, "R");
	}

    /**
     * 列数を取得する
     *
     * @author	Nagoya University
     * @date	2018/03/14
     *
     * @return	列数
     */

    int Schema::getAttributeSize() const
    {
        return attributes.size();
    }

    /**
     * 管理者項目の列数を取得する
     *
     * @author	Shinichi Kusayama
     * @date	2024/07/10
     *
     * @return	列数
     */

    int Schema::getAdminAttributeSize() const
    {
        return adminColumnNameList.size();
    }

	/**
	* attributeを削除する
	*
	* @author	Nagoya University
	* @date	2019/09/09
	*
	* @param	index	カラム番号
	* @return	削除成功可否
	*/

	bool Schema::deleteAttribute(int index)
	{
		attributes.erase(attributes.begin() + index);
		colNameIdxMap.clear();
		for (unsigned int i = 0; i < attributes.size(); i++) {
			colNameIdxMap[attributes.at(i).name] = i;
		}
		return true;
	}

    /**
     * targetの属性を自身の属性の後に追加したSchemaを返す（未使用）
     *
     * @author	Nagoya University
     * @date	2018/03/14
     *
     * @param	target	指定スキーマ
     *
     * @return	スキーマ情報
     */

    Schema Schema::concat(Schema target)
    {
        Schema rval;
        // 自身の属性を追加
        for(Attribute attr : attributes)
        {
            rval.attributes.push_back(attr);
        }
        // ターゲットの属性を追加
        for(Attribute attr : target.attributes)
        {
            rval.attributes.push_back(attr);
        }
        rval.tabletype = UNKNOWN;

        return rval;
    }

    /**
     * 属性名群で構成されるschemaを返す（未使用）
     *
     * @author	Nagoya University
     * @date	2018/03/14
     *
     * @param	attrNameList	列名リスト
     *
     * @return	スキーマ情報
     */

    Schema Schema::subset(std::vector<string> attrNameList)
    {
        // "*"指定の場合は自身のschemaを返す
        if((attrNameList.size() == 1) && attrNameList[0] == "*")
        {
            return *this;
        }

        Schema rval;
        // 属性名群の並び順でshcemaを構成する
        for(string &attrName:attrNameList)
        {
            // 属性名でattrbutesを探索、見つかった場合は格納する
            bool found = false;
            for(Attribute attr:attributes)
            {
                if(attr.name == attrName)
                {
                    rval.attributes.push_back(attr);
                    found = true;
                }
            }
            if(found == false)
            {
				cerr << "Attribute not found:" << attrName << endl;
            }
        }
        rval.tabletype = UNKNOWN;

        return rval;
    }

}
