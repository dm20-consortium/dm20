#include "SidManagement.h"
#include <string>
using namespace std;
using namespace CS;


/**
 * @fn		SidManagement::SidManagement()
 * @brief	コンストラクタ
 * @author	Nagoya University
 * @date	2023/03/21
 * @return
 */
SidManagement::SidManagement()
{
    //sid_ip_map = new UnorderedMap<int, string>;
    sid_ip_map = new UnorderedMap<unsigned long long, string>;
}
/**
 * @fn		SidManagement::~SidManagement()
 * @brief	デストラクタ
 * @author	Nagoya University
 * @date	2023/03/21
 * @return
 */
SidManagement::~SidManagement()
{
    delete sid_ip_map;
}
/**
 * @fn		SidManagement::init(string _filename)
 * @brief	起動処理
 * @param	filename ファイルパス
 * @author	Nagoya University
 * @date	2023/03/21
 * @return
 */
void SidManagement::init(string _filename)
{
    filename = _filename;
    delete sid_ip_map;
    sid_ip_map = new UnorderedMap<unsigned long long, string>; // 過去のデータが残るので作り直し
    sid_ip_map->UnorderedMapMake(filename.c_str());
}
/**
 * @fn		SidManagement::show()
 * @brief	リスト表示処理
 * @author	Nagoya University
 * @date	2023/03/21
 * @return
 */
void SidManagement::show()
{
    std::cout << "*** " << filename << " START ***" << std::endl;
    sid_ip_map->UnorderedMapShow();
    std::cout << "*** " << filename << " END ***" << std::endl;
}

/**
 * @fn		SidManagement::sid2ip(unsigned long long sid)
 * @brief	SID→IPアドレス変換処理
 * @param	sid SID
 * @author	Nagoya University
 * @date	2023/03/21
 * @return
 */
string SidManagement::sid2ip(unsigned long long sid)
{
    return sid_ip_map->UnorderedMapGetValue(sid);
}

/**
 * @fn		SidManagement::getKeys()
 * @brief	キー取得処理
 * @author	Nagoya University
 * @date	2023/03/21
 * @return
 */
vector<unsigned long long> SidManagement::getKeys()
{
    vector<unsigned long long> keys = sid_ip_map->UnorderedMapKeyExtraction(); 
    return keys;
}

/**
 * @fn		SidManagement::updateSidIp(unsigned long long sid, const std::string& ip)
 * @brief	更新処理
 * @param	sid SID
 * @param	ip ip
 * @author	Nagoya University
 * @date	2025/02/28
 * @return	エラー -1, 更新なし 0, 追加 1, 更新 2
 */
int SidManagement::updateSidIp(unsigned long long sid, const std::string& ip, const bool delete_flg)
{
	int changed = 0;

	// 既存のipを取得し、変更が必要か確認
	std::string current_ip;
	try
	{
		current_ip = sid2ip(sid);
		if (current_ip == ip)
		{
			if (delete_flg) {
				changed = 3;
			} else {
				// IPアドレスが同じなので、変更は不要
				return changed;
			}
		} else 	{
			// IPアドレスが異なるので、変更が必要
			changed = 2;
		}
	}
	catch (const std::out_of_range& e)
	{
		// sidが存在しない場合、新規として追加
		changed = 1;
	}

	if (changed == 0)
	{
		// 変更がないため、処理を終了
		return changed;
	}
	
	// ファイルを開いて更新処理を行う
	std::ifstream infile(filename);
	std::ofstream outfile("temp.csv");
	std::string line;
	bool updated = false;

	// ファイルが正しく開けたか確認
	if (!infile.is_open() || !outfile.is_open())
	{
		// エラーメッセージの表示や例外のスローなどを検討
		return -1;
	}

	while (std::getline(infile, line))
	{
		size_t pos = line.find(',');
		if (pos != std::string::npos)
		{
			unsigned long long existing_sid;
			try
			{
				existing_sid = std::stoull(line.substr(0, pos));
			}
			catch (const std::exception& e)
			{
				// 数値への変換に失敗した場合、その行をそのまま書き込む
				outfile << line << std::endl;
				continue;
			}

			if (existing_sid == sid)
			{
				if (!delete_flg) {
					outfile << sid << "," << ip << std::endl;
				}
				updated = true;
			}
			else
			{
				outfile << line << std::endl;
			}
		}
		else
		{
			// カンマが見つからない場合、その行をそのまま書き込む
			outfile << line << std::endl;
		}
	}

	if (!updated)
	{
		// sid が見つからなかった場合、新しいエントリを追加
		outfile << sid << "," << ip << std::endl;
	}

	infile.close();
	outfile.close();

	// ファイル操作のエラーチェック
	if (remove(filename.c_str()) != 0)
	{
		// エラー処理（必要に応じて例外をスローするなど）
		return -1;
	}

	if (rename("temp.csv", filename.c_str()) != 0)
	{
		// エラー処理
		return -1;
	}

	init(filename); // 読み込みなおし

	// 変更があったので true を返す
	return changed;
}
