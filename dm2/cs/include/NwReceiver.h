#ifndef __NWRECEIVER_H__
#define __NWRECEIVER_H__

#include "Cs.h"
#include "DtlsNwClient.h"
#include "NwRcv.h"

using namespace std;

namespace CS
{
	/**
	 * @class	INwReceiver
	 *
	 * @brief	ネットワーク通信受信 インターフェースクラス
	 *
	 * @author	Nagoya University
	 * @date	2023/03/21
	 */
	class INwReceiver
	{
	public:
		virtual ~INwReceiver() {}
		virtual std::thread* Run(CS::Queue<CS::clientdata>* queue) = 0;
	};


    /**
     * @class	NwReceiver
     *
     * @brief	ネットワーク通信受信クラス
     *
     * @author	Nagoya University
     * @date	2023/03/21
     */
    class NwReceiver : public INwReceiver
    {
    public:
        NwReceiver(
            NwRcvSettings &tmp_settings,
            uint count
        );
        ~NwReceiver();

        std::thread* Run(CS::Queue<CS::clientdata> *queue);

    private:
        Util dm2util;
        log4cxx::LoggerPtr logger;
        NwRcvSettings settings;
        uint setting_num;
        Socket* server;                     // 受信サーバー

        thread* pthread;                    // Network受信用スレッド

        string udp_port_number;           // 受信UDPポート番号
        string log_str;
        
        Queue<clientdata>* p_queue;			// ProcSenderから受信データを渡すためのキュー

        static void receiver(const NwReceiver* param, const string &confDirPath);
        
		static void ClearUnorderedMap(int& combination_map_clear_time_, UnorderedMap<std::string, std::vector<std::string>>& flagment_data_combination_map_, 
		              UnorderedMap<std::string, time_t>& flagment_data_receive_time_map_);
    };
}
#endif
