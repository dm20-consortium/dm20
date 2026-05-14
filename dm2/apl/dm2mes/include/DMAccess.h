/*
 * DMAccess.h
 *
 *  Created on: 2019/08/27
 *      Author: Nagoya University
 */

#ifndef _DMACCESS_H_
#define _DMACCESS_H_

#include <string>
#include <fstream>
#include <is/DmManager.h>

// デフォルトのユーザ名・パスワード・接続先を変えたい場合は下記を編集して下さい。
#define DEFAULT_DM_USER "dm2sampleuser"
#define DEFAULT_DM_PASS "dm2samplepassword"
#define DEFAULT_DM_IP "127.0.0.1"
// For Secure Mode
#define DEFAULT_CA_CERT "/etc/ssl/dm2/cacert.pem"
#define DEFAULT_CA_KEY "/etc/ssl/dm2/private/cakey.pem"
#define DEFAULT_CA_PASS "xxx"

class DMAccess {
private:
public:
	string _ip = DEFAULT_DM_IP;
	string _userid = DEFAULT_DM_USER;
	string _password = DEFAULT_DM_PASS;
	bool _isTransportMode;
	bool _isSecureMode;
	Connection *_DMcon;

	DMAccess();
	virtual ~DMAccess();

	bool DMConnect(void);
	bool DMConnect(string ip, string userid, string password, bool isTransportMode, bool isSecureMode);
	void DMDisconnect(void);
	void DMDisconnect(Connection *_pcon);

	void DMReconnect(void);
	bool cancelQuery(int mngId);
	string getFile(string fname);
	bool oneShotQueryExec(string queryStr, bool putHeader);
	unsigned int continuousQueryExec(string *queryStr, std::function<void (ResultSet)> fc, string *outMsg);
	unsigned int destQueryExec(string *queryStr, unsigned long long destSID, unsigned long long execSID, string *outMsg);
	bool cancelQuery(unsigned int mngId);

	int getColumnNumber(ResultSetMetaData *p_rsmd, string argCname, string argCtype);
};

#endif /* _DMACCESS_H_ */
