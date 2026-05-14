#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <dlfcn.h>
#include <geos_c.h>

using namespace std;
using multiFunc = vector<vector<string>>(*)(vector<vector<string>>);
multiFunc func = NULL;
vector<vector<string>> argList;
bool multiFuncExec(bool);
std::vector<std::string> split2(std::string str, char del);
std::vector<std::string> split(std::string str, char del);
void notice_function(const char *message, ...){ printf("%s", message); } // 依存ライブラリの出力用関数
void initialize(){ initGEOS(notice_function, notice_function); }         // 依存ライブラリの初期化処理
void finish(){ finishGEOS(); }                                           // 依存ライブラリの終了処理
int main(int argc, const char* argv[])
{
	if (argc <= 1) {
		cerr << "[Usage] ./driver FunctionName " << endl;
		cerr << "Ex.) echo 1,2 | ./driver ../UF_PLUS" << endl;
		cerr << "     ===> 3" << endl;
		cerr << "" << endl;
		cerr << "[Option] -d: Debug Input File " << endl;
		cerr << "         -m: Multi Row Mode (use Pipe)" << endl;
		cerr << "" << endl;
		cerr << "[Memo] Case Array) [1,2] " << endl;
		cerr << "       Case String) \"1,2\" " << endl;
		cerr << "" << endl;
		cerr << "[Notice] About characters" << endl;
		cerr << "   source filename   : Allow lower and upper case" << endl;
		cerr << "   funcname in source: upper case only" << endl;
		return false;
	}
	std::string functionName = argv[1];
	std::string libname = functionName + ".so";
	if (functionName.find('/') != string::npos) {
		auto pos = functionName.rfind('/');
		functionName = functionName.substr(pos+1);
	}
	transform(functionName.begin(),functionName.end(),functionName.begin(),::toupper);
	initialize();
	bool isDebug = false;
	bool multiRow = false;
	for (int i = 2; i < argc; i++) {
		string s = argv[i];
		if (s == "-d") {
			isDebug  = true;
		}
		else if (s == "-m") {
			multiRow = true;
		}
	}
	if (isDebug) {
		cout << "Lib Path:" << libname << endl;
		cout << "extern C Function Name:" << functionName << endl;
	}
	void *handle = NULL;
	handle = dlopen(libname.c_str(), RTLD_LAZY);
	if (handle == NULL) {
		cerr << "dlopen err. '" + libname + "' is not exist" << endl;
		return false;
	}
	func = (multiFunc)dlsym(handle, functionName.c_str());
	if (handle == NULL) {
		cerr << "dlsym err" << endl;
		return false;
	}
	std::string line;
	while (true)
	{
		if (std::getline(std::cin, line)) {
			vector<string> colList = split2(line, ',');
			argList.push_back(colList);
			if (!multiRow) {
				if (!multiFuncExec(isDebug)) {
					break;
				}
				argList.clear();
			}
		} else {
			break;
		}
	}
	if (multiRow) {
		multiFuncExec(isDebug);
	}
	dlclose(handle);
	finish();
}
bool multiFuncExec(bool debug) {
	vector<vector<string>> retList;
	if (debug) {
		cout << "Input Data:" << endl;
		for (int idx = 0; idx < argList.size(); idx++) {
			for (unsigned int i = 0; i < argList.at(idx).size(); i++) {
				if (i != 0) cout << ",";
				cout << argList.at(idx).at(i);
			}
			cout << "" << endl;
		}
	}
	try {
		retList = func(argList);
	} 
	catch (const exception &ex) {
		cerr << "exception err" << endl;
		return false;
	}
	if (debug) {
		cout << "Output Data:" << endl;
	}
	if (retList.size() > 0) {
		for (int idx = 0; idx < retList.size(); idx++) {
			for (unsigned int i = 0; i < retList.at(0).size(); i++) {
				if (i != 0) cout << ",";
				cout << retList.at(idx).at(i);
			}
			cout << "" << endl;
		}
	}
	return true;
}
std::vector<std::string> split2(std::string str, char del) {

	vector<string> strList = split(str, del);
	vector<string> retList;
	vector<string> vs;
	string prefix1 = "[";
	string suffix1 = "]";
	string prefix2 = "\"";
	string suffix2 = "\"";
	for (auto itr = strList.begin(); itr != strList.end() ; ++itr) {
		if (vs.size() > 0 || itr->find(prefix1) == 0 || itr->find(prefix2) == 0) {
			if (itr->find(prefix2) == 0) {
				itr->replace(0, 1, "");
			}
			if (*itr != "") {
				if (itr->find(suffix1) == itr->size() - 1 || itr->find(suffix2) == itr->size() - 1) {
					if (itr->find(suffix2) == itr->size() - 1) {
						itr->replace(itr->size() - 1, 1, "");
					}
					vs.push_back(*itr);
					string s = "";
					for (auto itr2 = vs.begin(); itr2 != vs.end(); ++itr2) {
						if (s != "") s += ",";
						s += *itr2;
					}
					retList.push_back(s);
					vs.clear();
				} else {
					vs.push_back(*itr);
				}
			} else {
				vs.push_back(*itr);
			}
		} else {
			// 可変長配列以外のケース
			retList.push_back(*itr);
		}
	}
	return retList;
}
std::vector<std::string> split(std::string str, char del) {
    int first = 0;
    int last;
    auto check = str.find_first_of(del);
    std::vector<std::string> result;
    if (check == string::npos) {
    	result.push_back(str);
        return result;
    } else {
    	last = check;
    }

    while (first < (int)str.size()) {
        std::string subStr(str, first, last - first);
        result.push_back(subStr);
        first = last + 1;
        last = str.find_first_of(del, first);
        if (str.find_first_of(del, first) == std::string::npos) {
            last = str.size();
        }
    }
    return result;
}
