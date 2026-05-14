/*
 * Main.h
 *
 *  Created on: 2018/06/05
 *      Author: Nagoya University
 */

#ifndef _MAIN_H_
#define _MAIN_H_

#include "DMSender.h"
#include "UdpSendInterface.h"

bool DMRecv(string ip, string userid, string password, string master_schema, string schema_name, bool isTransportMode,
		string query, string window, long timeout, int addTimestamp, int delay, bool isSecureMode, 
		string columns, string plus_schema_name, bool allowDuplication, string paritition_keys, string where);
bool DMSetDestQuery(string ip, string userid, string password, string master_schema, string schema_name,
		bool isTransportMode, string query, string window, int addTimestamp, bool isSecureMode, 
		unsigned long long destSID, unsigned long long execSID, string columns, string paritition_keys, string where);
bool DMOneshot(string ip, string userid, string password, string schema_name, bool isTransportMode, string query, bool isSecureMode, int create_timer);
bool DMOneshotByFile(string ip, string userid, string password, string schema_name, bool isTransportMode, string query_file, bool isSecureMode);
bool DMCancel(string ip, string userid, string password, bool isTransportMode, bool isSecureMode, unsigned int mngId);
void handler(int signum);
static void usage(const char *cmd);
void deleteNl(std::string &targetStr);
string getQuery(string fname);
void RegisterSendList(string send_list, string input_file);
#endif /* _MAIN_H_ */
