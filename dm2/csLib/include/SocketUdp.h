#ifndef SOCKETUDP_H
#define SOCKETUDP_H

#include "Cs.h"

namespace CS{

	/**
	 * @class	SocketUdp
	 *
	 * @brief	DM2.0UDPソケットクラス
	 *
	 * @author	Nagoya University
	 * @date	2018/03/12
	 */
	class SocketUdp:public Socket{
		
	public:

		/**
		 * @fn	SocketUdp::SocketUdp()
		 *
		 * @brief	コンストラクタ
		 *
		 * @author	Nagoya University
		 * @date	2018/03/12
		 */
		SocketUdp():Socket(){}

		/**
		 * @fn	virtual SocketUdp::~SocketUdp()
		 *
		 * @brief	デストラクタ
		 *
		 * @author	Nagoya University
		 * @date	2018/03/12
		 */
		virtual ~SocketUdp(){}

	};
}

#endif