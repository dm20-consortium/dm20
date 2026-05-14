#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <string>
#include <vector>
#include <zlib.h>
#include <zstd.h>
#include <cstdint>
#include <limits>
#include <cstring>
#include <map>
#include <cxxabi.h>

#if __cplusplus < 201703L || defined(USE_EXP_ANY)
	#include <experimental/any>
	using std::experimental::fundamentals_v1::any;   // any_cast
	using std::experimental::any_cast;   // any_cast
#else
	#include <any>
#endif

using namespace std;

#define TCP_SEND_SIZE 1300

#define SEND_BUF_SIZE 1280
// COMPRESSED_BUF_SIZE - HEADER_SIZE (圧縮フラグ<char>:1、長さ<int>:8、キー情報<long>]4、シーケンス番号<int>:4)
#define COMPRESSED_BUF_SIZE 1263

#define UNCOMPRESSED_BUF_SIZE 65000

struct IsHeader {
	char compressFlg;
	int length;
	long key;
	int seq;
	string schema_name;
};
struct IsHeaderInfo {
	IsHeader header;
	char *payload_p;
	int headerSize;
};
struct IsDigit {
	int operator()(int c) { return isdigit(c); }
};

namespace IS {

	/**
	 * 文字列操作を扱うユーティリティクラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/15
	 */

	class StringUtil
	{
	private:
		vector<any> getVectorAny(const string &str, const string &type, const bool &isNull);
		vector<vector<any>> get2VectorAny(const string &str, const string &type, const bool &isNull);

		enum dataType {
			LONG = 0,
			DOUBLE = 1,
			INT = 2,
			STRING = 3,
			BOOL = 4,
			ULONG = 5,
			UINT = 6
		};
		map<string, dataType> typeMap =
		{
			{ "long", LONG },
			{ "ulong", ULONG },
			{ "int8", LONG }, 
			{ "double", DOUBLE },
			{ "float4", DOUBLE },
			{ "float8", DOUBLE },
			{ "numeric", DOUBLE },
			{ "int", INT },
			{ "uint", UINT },
			{ "integer", INT },
			{ "int2", INT },
			{ "int4", INT },
			{ "string", STRING },
			{ "character varying", STRING },
			{ "char", STRING },
			{ "text", STRING },
			{ "bpchar", STRING },
			{ "varchar", STRING },
			{ "geometry", STRING },
			{ "geography", STRING },
			{ "bool", BOOL}
		};

	public:
		string deleteSpace(string buf);
		string trim(const string& target);
		void deleteStrings(string &target, const vector<string> &deleteStrList);
		string getDeleteStrings(const string target, const vector<string> &deleteStrList);
		void deleteNl(std::string &targetStr);
		vector<string> split(const string &target, const string &regex);
		void split(const string &target, const string &regex, vector<string> &result);
		void splitExcludeBrackets(const string &target, const string &regex, vector<string> &result);
		void splitBySize(const string &target, const unsigned int size, vector<string> &result);
		void replaceOnce(string &target, const string &before, const string &after);
		void replaceAll(string &target, const string &before, const string &after);
		void replace(string &target, const string &before, const string &after);
		void toUpper(string &target);
		void toLower(string &target);
		int getXMLSize(const string &target);
		void getHeaderStr(const string &target, string &result);
		void getHeaderInfo(string &target, string &key, int &flagment, int &flagmentMax);
		bool contain(const string& s, const string& v) const;
		bool starts_with(const std::string& s, const std::string& prefix);
		bool ends_with(const string& s, const string& suffix);

		string getAnyString(const any &val);
		vector<string> getVectorString(string &args);
		void getStringInBracket(string input, vector<string> &ret);
		vector<string> getStringInBracketFirst(string input, string left, string right);
		any getAnyValFromString(const string &str, const string &type);
		any getAnyValFromString(const string &str, const string &type, const bool &isNull);
		void getAnyValFromString(const string &str, const string &type, any &val);
		void getAnyValFromString(const string &str, const string &type, any &val, const bool &isNull);
		bool convertBool(const string &str, bool &ret);
		bool checkSupportDataType(const string &type);
		bool checkDataTypeMatches(const string &type1, const string &type2, bool isOutputErr);
		int getSRID(const string &input);
		bool isNumber(const string &input);
		bool isString(const string &input);
		string getClassName(const type_info& id);
		bool compress(string inStr, Bytef *outBuf);
		int decompress(char *inBuf, char *outBuf);
		bool compressUsingZstd(string inStr, int inSize, void *outBuf, int *outSize);
		bool decompressUsingZstd(char *inBuf, char *outBuf, int inSize, int *outSize);
		void getIsHeader(char *buf, IsHeaderInfo &info);
		int setCompressedBufWithHeader(string inStr, char *outBuf, char compressFlg, long key);
		string getValueByXMLTag(string target, string key);


	};

    /**
     * any型の表示用クラス
     *
     * @author	Nagoya University
     * @date	2018/03/14
     */

    class Demangle
    {
    private:
        
    public:
		char * realname;
        /**
         * コンストラクタ
         *
         * @author	Nagoya University
         * @date	2018/03/14
         *
         * @param	ti	タイプ情報
         */

        Demangle( std::type_info const & ti )
        {
            int status = 0 ;
            realname = abi::__cxa_demangle( ti.name(), 0, 0, &status ) ;
        }
        Demangle( Demangle const & ) = delete ;
        Demangle & operator = ( Demangle const & ) = delete ;

        /**
         * デストラクタ
         *
         * @author	Nagoya University
         * @date	2018/03/14
         */

        ~Demangle()
        {
            std::free( realname ) ;
        }

        operator char const * () const
        {
            return realname ;
        }
    };
}
#endif  // STRINGUTIL_H

