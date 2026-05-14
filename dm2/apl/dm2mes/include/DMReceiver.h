/*
 * DMReceiver.h
 *
 *  Created on: 2019/08/27
 *      Author: Nagoya University
 */

#ifndef _DMRECEIVER_H_
#define _DMRECEIVER_H_

#include <string>
#include <vector>
#include <map>
#include "DMAccess.h"
#include "DMSender.h"

using namespace std;
class DMReceiver : public DMAccess {
private:
	unsigned int mngId;
	int addTimestamp = 0;
	string _plus_schema_name = "";
	string getValue(string input, int colSize);
	void callbackForStream(ResultSet rs);
	void callbackForStreamPlusSendIs(ResultSet rs);
	string makeQuery(string master_schema, string schema_name, string window, string columns, string paritition_keys, string where);
 	void SendIs(string ord_s);

public:
	DMReceiver();
	virtual ~DMReceiver();

	bool continuousQuery(string master_schema, string schema_name, string query, string window, int addTimestamp, 
		string columns, string plus_schema_name, bool duplication, string paritition_keys, string where);
	bool destQuery(string master_schema, string schema_name, string query, string window, unsigned long long destSID, 
		unsigned long long execSID, string columns, string paritition_keys, string where);
	bool cancelQueryForStream(void);

};

#endif /* _DMRECEIVER_H_ */
