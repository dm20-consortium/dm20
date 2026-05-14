#ifndef __SIDMANAGEMENT_H__
#define __SIDMANAGEMENT_H__

#include <string>
#include "UnorderedMap.h"

using namespace std;



namespace CS{

    	/**
	 * @class	ISidManagement
	 *
	 * @brief	受信したメッセージを管理し、送信するIPアドレスを割り出す
	 *
	 * @author	Nagoya University
	 * @date	2023/03/21
	 */
    class SidManagement {
		private:
			// UnorderedMap<int, string> sid_ip_map;
			UnorderedMap<unsigned long long, string> *sid_ip_map; 
			string filename;
		
        public:
			SidManagement();
			~SidManagement();
			void init(string _filename);
			void show();
			string sid2ip(unsigned long long sid);
			//vector<int> getKeys();
			vector<unsigned long long> getKeys();
            int updateSidIp(unsigned long long sid, const string& ip, const bool delete_flg); 
    };



}

#endif