#ifndef IUDPSERVER_H
#define IUDPSERVER_H
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <linux/wireless.h>
#include "Struct.h"
namespace CS{
	class IUdpServer {
	public:
		virtual ~IUdpServer();

		virtual int Init(const std::string& fd_name, std::string interface_name, std::string port_no) = 0;
		virtual int RecvClientData(clientdata &buf_, int socket_res_) = 0;
		virtual int RecvPacket(send_message &buf_, int socket_res_) = 0;
	};
}
#endif