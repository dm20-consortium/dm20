#ifndef IUDPCLIENT_H
#define IUDPCLIENT_H
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
namespace CS{
	class IUdpClient {
	public:
		virtual ~IUdpClient();

		virtual void Init(const std::string& fd_name, const std::string& port, const std::string& ip) = 0;
			
		virtual int SendClientData(struct clientdata &buf_) = 0;
		virtual int SendPacket(struct send_message &buf_) = 0;

		virtual void CloseSocketFd() = 0;
	};
}
#endif