#ifndef __NWSENDER_H__
#define __NWSENDER_H__

#include "Cs.h"
#include "ProcRcv.h"
#include "SidManagement.h"
#include "DtlsNwClient.h"

#include <atomic>
#include <mutex>

using namespace std;

namespace CS{

	/**
	 * @class	INwSender
	 *
	 * @brief	プロセス間通信(UDP)受信 インターフェースクラス
	 *
	 * @author	Nagoya University
	 * @date	2023/03/21
	 */
	class INwSender
	{
	public:
		virtual ~INwSender() {}
		virtual std::thread* Run(CS::Queue<CS::clientdata>* queue) = 0;
		virtual Socket* getSocket() = 0;
		virtual void setRunExitThreadFlg() = 0;
	};


	/**
	 * @class	NwSender
	 *
	 * @brief	プロセス間通信(UDP)受信クラス
	 *
	 * @author	Nagoya University
	 * @date	2023/03/21
	 */
	class NwSender : public INwSender
	{
	private:
	    SidManagement sidManagement;
	    std::mutex mtx;
	    std::condition_variable cv;
	    bool ipChanged = false;
	    bool dataAvailable = false;
		Util dm2util;
		log4cxx::LoggerPtr logger;
		ProcRcvSettings& settings;					// dm2conf設定
		uint setting_num;							// ネットワーク設定番号
		thread* pthread;							// Network送信用スレッド
		Socket* client;		    				    // クライアント	
		std::atomic<bool> exit_run_thread{false};
		std::mutex ipChangeMutex; // 同期用ミューテックス	

		static void sender(const NwSender* param, const string confDirPath);
		static int send_to_edge(const NwSender* param, clientdata& se_cdata, addrinfo& res_addr_first_, std::string udp_port_number);
		
		Queue<clientdata>* p_queue;					// ProcReceiverから送信データを受け取るためのキュー
	public:
		NwSender(ProcRcvSettings &settings, uint count);
		~NwSender();

		std::thread* Run(CS::Queue<CS::clientdata>* queue);
		Socket* getSocket();
		void setRunExitThreadFlg();
	};
}
#endif // __NWSENDER_H__
