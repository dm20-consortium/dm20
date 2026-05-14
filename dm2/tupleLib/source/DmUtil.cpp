#include "is/DmUtil.h"

/**
* コンストラクタ
*
* @author	Nagoya University
* @date	2018/03/13
*/

DmUtil::DmUtil()
{
}

/**
* デストラクタ
*
* @author	Nagoya University
* @date	2018/03/13
*/

DmUtil::~DmUtil()
{
}

/**
* 現在時刻をmsec単位で取得
*
* @author	Nagoya University
* @date	2018/03/13
*
* @return	現在時刻(msec)
*/

long DmUtil::getTimeMillisec()
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

/**
* 現在時刻をmicrosec単位で取得
*
* @author	Nagoya University
* @date	2018/03/13
*
* @return	現在時刻(microsec)
*/

long DmUtil::getTimeMicrosec()
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

/**
* 現在時刻をmsec単位で文字列として取得
*
* @author	Nagoya University
* @date	2018/03/13
*
* @return	現在時刻文字列(msec)
*/

string DmUtil::getTimeMillisecStr()
{
	long now = getTimeMillisec();
	return std::to_string(now);
}
/**
*プロセスIDを取得する
*
* @author	Nagoya University
* @date	2019/07/23
*
* @return	プロセスID
*
*/

string DmUtil::getPID()
{
	pid_t pid = getpid();
	return std::to_string(pid);
}



/**
* 符号なし整数型をバイト配列の値にして文字列型に変換
*
* @author	Nagoya University
* @date	2022/02/04
*
* @param [in]	input	64bit符合なし整数

* @return	変換後16進数文字列
*/
string DmUtil::getStringFromUnsignedLong(unsigned long input)
{
	char byte_char[16];				//16進数(4bit)で1文字とするバイト配列
	unsigned char bit_mask = 0xF;	//64bitの最上位4bitから変換
	int check_bit = 64 - 4;
	for(int i = 0; i < 16; i++, check_bit -= 4) {
		unsigned char byte_val = static_cast<unsigned char>(input >> check_bit) & bit_mask;
		if(byte_val < 0xA) {
			byte_char[i] = '0' + byte_val;
		} else {
			byte_char[i] = 'A' + byte_val - 0xA;
		}
	}
	std::string str(byte_char, sizeof(byte_char));
	return str;
}

/**
* バイト配列の値で入った文字列型を符号なし整数型にして変換
*
* @author	Nagoya University
* @date	2022/02/04
*
* @param [in]	input	16進数文字列
*
* @return	変換後符号なし整数
*/
unsigned long DmUtil::getUnsignedLongFromString(const string &arg_input)
{
	string input = arg_input;
	// 空白を削除
	const size_t pos = input.find_first_of(" 　");
	if (pos != string::npos) {
		input.erase(pos);
	}
	unsigned long ret = 0;
	int size = static_cast<int>(input.size());
	unsigned long value;
	
	// 64bit(16進数16文字)を超える場合はエラー
	if(input.size() > 16) {
		//throw CastException();
		return ret;
	}
	// 1文字ずつ下位ビットから整数に変換
	for(int i = 0; i < size; i++) {
		if('0' <= input[size - i - 1] && input[size - i - 1] <= '9') {
			value = (input[size - i - 1] - '0');
		}
		else if('A' <= input[size - i - 1] && input[size - i - 1] <= 'F') {
			value = (input[size - i - 1] - 'A') + 0xA;
		}
		else if('a' <= input[size - i - 1] && input[size - i - 1] <= 'f') {
			value = (input[size - i - 1] - 'a') + 0xA;
		}
		else {
			// 0～9, A～F, a～f 以外の文字はエラー
			//throw CastException();
			return ret;
		}
		ret |= value << (i * 4);
	}
	return ret;
}
/**
*  ハーバーサイン距離計算
*
* @author	Shinichi Kusayama
* @date	2025/02/23
*
* @param [in]	lat1	latitude 1
* @param [in]	lon1	longitude 1
* @param [in]	lat2	latitude 1
* @param [in]	lon2	longitude 1
*
* @return	距離
*/
// ハーバーサイン距離計算
double DmUtil::haversine(double lat1, double lon1, double lat2, double lon2) {
	

    constexpr double DEG_TO_RAD = M_PI / 180.0;
    double dlat = (lat2 - lat1) * DEG_TO_RAD;
    double dlon = (lon2 - lon1) * DEG_TO_RAD;

    double a = std::pow(std::sin(dlat / 2), 2) +
               std::cos(lat1 * DEG_TO_RAD) * std::cos(lat2 * DEG_TO_RAD) * std::pow(std::sin(dlon / 2), 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    return EARTH_RADIUS * c;
}

/**
*  3D距離計算
*
* @author	Shinichi Kusayama
* @date	2025/02/23
*
* @param [in]	lat1	latitude 1
* @param [in]	lon1	longitude 1
* @param [in]	lat2	latitude 1
* @param [in]	lon2	longitude 1
*
* @return	距離
*/
// スコア計算（基準行との比較）
double DmUtil::get3DDistance(double lat1, double lon1, double alt1, double lat2, double lon2, double alt2) {
    double distance = haversine(lat1, lon1, lat2, lon2);
    double altitude_diff = std::abs(alt1 - alt2);
    double total_distance = std::sqrt(distance * distance + altitude_diff * altitude_diff);
    
    return total_distance;
}
