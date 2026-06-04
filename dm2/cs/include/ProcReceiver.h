#ifndef __PROCRECEIVER_H__
#define __PROCRECEIVER_H__

#include <functional>
#include "Cs.h"
#include "ProcRcv.h"
using namespace std;

namespace CS{
	/**
	 * @class	ProcReceiver
	 *
	 * @brief	プロセス間通信(UDP)受信クラス
	 *
	 * @author	Nagoya University
	 * @date	2023/03/21
	 */
	class ProcReceiver
	{
	private:
		Util dm2util;							// utility
		log4cxx::LoggerPtr logger;				// log
		ProcRcvSettings& settings;

		thread* pthread;						// Proc受信用スレッド

		const UdpProcServer udpprocserver;

		// int my_sid; 							// 自身のStation ID
		unsigned long long my_sid; 				// 自身のStation ID
		int trace_on;							// トレースログのON/OFFフラグ

		static void receiver(const ProcReceiver* param, const string confDirPath);
		static void receiveDataToQueue(const ProcReceiver* me, struct send_message &buf, struct clientdata &cdata);
		vector<Queue<clientdata>*>* p_queues;
		void sid_update_check(const ProcReceiver* me, const char dm2_payload[]) const;
    	std::function<void(ProcRcvSettings&, uint)> onNewSender;
    	std::function<void(uint)> onStopSender;
	public:
		ProcReceiver(ProcRcvSettings &settings);
		~ProcReceiver();
		void SetOnNewSender(std::function<void(ProcRcvSettings&, uint)> cb) {
			onNewSender = cb;
		}
		void SetOnStopSender(std::function<void(uint)> cb) {
			onStopSender = cb;
		}

		std::thread* Run(vector<Queue<clientdata>*>* pvec_queues);
	};
}
#endif
