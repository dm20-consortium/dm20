#include "FileSource.h"

#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

namespace IS {

	/**
	* コンストラクタ
	*
	* @author	Nagoya University
	* @date	2018/04/26
	*
	* @param	name	ネットワーク名称
	*/
	FileSource::FileSource(string name)
		:InformationSource(name) {
		this->sourcename = name;
	}

	/**
	* デストラクタ
	*
	* @author	Nagoya University
	* @date	2018/04/26
	*/
	FileSource::~FileSource()
	{
	}

	/**
	* ファイルを読み込み受信処理にデータを流す
	*
	* @author	Nagoya University
	* @date	2018/04/26
	*
	* @param	fileName		ファイル名
	* @param	num	読み込む車両台数
	*/
	void FileSource::fileReadInput(const string &fileName, int num) {
		logger->info("Start ReadInputFile(" + fileName + ")");
		std::ifstream ifs(fileName);
		std::string str;
		bool start = true;
		long startTime = 0;
		int row = 0;
		if (ifs.fail())
		{
			std::cerr << "失敗" << std::endl;
			return;
		}
		RecvData data;
		if (num == 0) {
			while (getline(ifs, str))
			{
				data.payload = str;
				receiveProcess(data);
				row++;
			}
		}
		else {
			
			while (getline(ifs, str))
			{
				if (start) {
					startTime = DmUtil::getTimeMillisec();
					start = false;
				}
				data.payload = str;
				receiveProcess(data);
				row++;
				if (row % num == 0) {
					long waitTime = 100 - (DmUtil::getTimeMillisec() - startTime);
					if (waitTime > 0) {
						logger->info("Push data:" + to_string(num) + "[Total:" + to_string(row) + "] waitTime:" + to_string(waitTime) + "[msec]");
						usleep(waitTime * 1000);
					}
					else {
						logger->info("Push data:" + to_string(num) + "[Total:" + to_string(row) + "] overTime:" + to_string(waitTime) + "[msec]");
					}
					start = true;
				}
			}
		}
		logger->info("Finish ReadInputFile(" + fileName + ") InputRow:" + std::to_string(row));
		return;
	}
}