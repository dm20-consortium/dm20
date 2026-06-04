#ifndef __UDPRECEIVEINTERFACE_H__
#define __UDPRECEIVEINTERFACE_H__

#include "Cs.h"
#include <functional>

namespace CS{
	/**
	* @class	UdpReceiveInterface
	*
	* @brief	UDP受信インタフェース
	* 			IS、APLに公開するCSインタフェース
	*
	* @author	Nagoya University
	* @date	2018/03/15
	*/
	class UdpReceiveInterface
	{
	private:
		send_message_upper upper_buf_;
		//send_message buf_;
		struct clientdata buf_;

	public:

		UdpReceiveInterface();
		
		void run(const char* fd_name, std::function<void(send_message_upper)>notify);

		void run_is(const std::string &fd_name, const std::string &nic, const std::string &port, std::function<void(send_message_upper)>notify);

		void run_sec(const std::string &fd_name, std::function<void(send_message_upper)>notify);

		void run_mng_ctl(const std::string &fd_name, std::function<void(send_message_upper)>notify);
		
		void notify(send_message_upper upper_buf_);

		~UdpReceiveInterface(){}
	};

}
#endif
