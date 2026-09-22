#ifndef __UDPSENDINTERFACE_H__
#define __UDPSENDINTERFACE_H__

#include "Cs.h"
#include <string>
#include <cerrno>
#include <cstring>
#include <iostream>

namespace CS{
	/**
	 * @class	UdpSendInterface
	 *
	 * @brief	UDP送信インタフェース
	 * 			IS、APLに公開するCSインタフェース
	 *
	 * @author	Nagoya University
	 * @date	2018/03/15
	 */
	class UdpSendInterface : public UdpClient
	{
	private:
		send_message buf_;
		send_message_vector buf_vector_;

	public:
		UdpSendInterface();
		void AplUdpSendto(const char* send_fd_name_, const char* recv_fd_name_, unsigned long long src_stid_, unsigned long long dst_stid_, short retry_level_, int retry_lifetime_, std::vector<char> &&payload_);
		void IsStreamSendtoCs(unsigned long long lane_id_, unsigned long long src_stid_, unsigned long long dst_stid_, short retry_level_, int retry_lifetime_, std::vector<char> &&payload_);
		
		void SecSendtoCs(sockaddr_un server_addr_, unsigned long long src_stid_, unsigned long long dst_stid_, short retry_level_, int retry_lifetime_, char *payload_, int length_, const std::string &confDirPath);
		void MngConnectionInfoSendtoCs(const char* send_fd_name_, unsigned long long src_stid_, unsigned long long dst_stid_, send_message_mng &sm);

		~UdpSendInterface(){}
	};
}
#endif
