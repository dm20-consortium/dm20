#ifndef FILESORCE_H
#define FILESORCE_H

#include "InformationSource.h"
#include "is/DmUtil.h"
#include "NetworkSource.h"

#include <thread>
#include <log4cxx/logger.h>

// for UDP
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace log4cxx;

namespace IS {

	/**
	*
	* @author	Nagoya University
	* @date	2018/04/26
	*/

	class FileSource : public InformationSource
	{
	private:
		LoggerPtr logger = Logger::getLogger("FileSource");
		string sourcename;
		virtual void receiveProcess(RecvData &data) = 0;

	protected:
		bool exit_flag = false;

	public:
		FileSource(string name);
		virtual ~FileSource();
		
		void fileReadInput(const string &fileName, int num);

	};
}
#endif  // FILESORCE_H
