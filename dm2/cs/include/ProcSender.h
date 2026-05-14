#ifndef __PROCSENDER_H__
#define __PROCSENDER_H__

#include "Cs.h"
#include "NwRcv.h"

using namespace std;

namespace CS
{
    /**
     * @class	ProcSender
     *
     * @brief	プロセス間通信(UDP)送信クラス
     *
     * @author	Nagoya University
     * @date	2023/03/21
     */
    class ProcSender
    {
    public:
        ProcSender(NwRcvSettings &tmp_settings);
        ~ProcSender();

        std::thread* Run(CS::Queue<CS::clientdata> *queue);

    private:
        Util dm2util;                       // utility
        log4cxx::LoggerPtr logger;          // log
        NwRcvSettings settings;

        std::thread* pthread;                    // Proc送信用スレッド
        Queue<clientdata> *p_queue;

        UnorderedMap<string, time_t> sid_sq_id_received_time_map;
        std::thread* pthread_duplicate_manage;

        const UdpProcClient udpprocclient;

        // int my_sid;   // 自身のStation ID
        unsigned long long my_sid;   // 自身のStation ID
        int trace_on; // トレースログのON/OFFフラグ
        int max_sendto_times;

        static void sender(const ProcSender *param, UnorderedMap<string, time_t> &duplicate_check_map, const string &confDirPath);
        // 重複受信チェック
        static int duplicate_receive_check(const ProcSender* param, clientdata &cdata, UnorderedMap<string, time_t> &duplicate_check_map);
        static void clean_duplicate_map(const ProcSender* param, UnorderedMap<string, time_t>& sid_sq_id_received_time_map);
    };
}
#endif
