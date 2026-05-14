#ifndef SCHEMA_H
#define SCHEMA_H

#include <vector>
#include <string>
#include <map>
//#include <log4cxx/logger.h>

using std::string;                             // string
//using namespace log4cxx;
using namespace std;

namespace IS {
	class StringUtil;

    /**
     * 列情報を扱うクラス
     *
     * @author	Nagoya University
     * @date	2018/03/14
     */

    class Attribute {
	private:
        public:
			Attribute();
			~Attribute();
            string name;
            string type;
			string orgTableName;
			string defaultValue;			/* default句対応 */
			bool hidden = false;
    };

    /**
     * スキーマ情報を扱うクラス
     *
     * @author	Nagoya University
     * @date	2018/03/14
     */

    class Schema {
	public:
		enum relationType {
			NONE = 0,
			RELATION = 1,
			VIEW = 2,
			M_VIEW = 3
		};
    private:

        std::vector<Attribute> attributes;
        string tabletype = "UNKNOWN";
        string tablename = "UNKNOWN";
		std::map<string, string> userAccessMap;
		relationType reltype = NONE;
		vector<string> relationTableList;
		map<string, int> colNameIdxMap;
		map<string, int> evalMinIdxMap;
		map<string, int> evalMaxIdxMap;
		bool checkAccessMap(const string &user, const string &op);

    public:
		// table_type
		static const string RDB;
		static const string STREAM;
		static const string UNKNOWN;

		static const string COL_USERID;
		static const string COL_IPADDR;
		static const string COL_HASH;
		static const string COL_TS_GRP;

        Schema();
		Schema(const string &tableName, const string &type);
		Schema(const string &tableName, const string &type, relationType relType);
		Schema(const string &tablename, const std::vector<Attribute> &attrs, const string &type);
        ~Schema();

		void clear();
        void appendEvalColumnRange(const string &evalName, int minIdx, int maxIdx);
        void append(const Attribute &attr);
		void appendAdminColumn();
		void copyAttributes(const Schema &schema);
		void addAttributes(const Schema &schema);
		void setAccessPrivileges(const string &accessTxt);
		void setRelationTableList(const vector<string> &list);
        bool renameTableName(const string &from, const string &to);
        bool renameColumnName(const string &from, const string &to);

        std::vector<Attribute> getAttributes() const;
        Attribute getAttribute(int index) const;
        Attribute getAttribute(string name) const;
		string getAttributeType(int index) const;
		string getAttributeDefaultValue(int index) const;
		string getAttributeName(int index) const;
		bool getAttributeHidden(int index) const;
		string getAttributeOrgTblName(int index) const;
		relationType getRelationType() const;
		int getAttributeIdx(const string &name) const;
		int getAttributeIdx(const string &name, bool isLog) const;
		bool isExistAttribute(const string &name) const;
		bool isExistAttribute(const string &name, int &minIdx, int &maxIdx) ;
        string getTableType() const;
		string getTableName() const;
		vector<string> getRelationTableList() const;
		bool isSelect(const string &userName);
		bool isAppend(const string &userName);
		bool isRefAdminCol(const string &userName);
        int getAttributeSize() const;
        int getAdminAttributeSize() const;
		bool deleteAttribute(int index);
        Schema concat(Schema target); // 未使用
        Schema subset(std::vector<string> attrNameList); //未使用

    };
}
#endif //SCHEMA_H
