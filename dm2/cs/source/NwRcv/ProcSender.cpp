#include "ProcSender.h"

using namespace std;
using namespace CS;
using namespace log4cxx;

/**
 * @fn		ProcSender::ProcSender()
 * @brief	ProcSenderコンストラクタ
 * @author	Nagoya University
 * @date	2023/03/21
 * @return
 */
ProcSender::ProcSender(NwRcvSettings &tmp_settings)
{
    memcpy(&settings, &tmp_settings, sizeof(tmp_settings));

    // log4cxxの初期化
    logger = Logger::getLogger("ProcSender");
}

/**
 * @fn		ProcSender::ProcSender()
 * @brief	ProcSenderデストラクタ
 * @author	Nagoya University
 * @date	2023/03/21
 * @return
 */
ProcSender::~ProcSender()
{
    delete pthread;
    delete pthread_duplicate_manage;
}

std::thread* ProcSender::Run(CS::Queue<CS::clientdata> *queue)
{
    LOG4CXX_INFO(logger, "start");
	p_queue = queue;

    // プロセス送信用スレッド生成
    pthread = new thread(sender, this, ref(sid_sq_id_received_time_map), settings.dm2util.getConfigDirectory());

    // 重複受信マップ管理用スレッド生成
    pthread_duplicate_manage = new thread(clean_duplicate_map, this, ref(sid_sq_id_received_time_map));

	LOG4CXX_INFO(logger, "exit");

	return pthread;
}

/**
 * @fn		ProcSender::sender()
 * @brief	プロセスへの送信処理
 * @author	Nagoya University
 * @date	2023/03/21
 * @return
 */
void ProcSender::sender(const ProcSender *param, UnorderedMap<string, time_t> &sid_sq_id_received_time_map, const string &confDirPath)
{
    const ProcSender *me = param;       // 自身のメンバへのアクセスはmeを通じて行うこと
    LOG4CXX_INFO(me->logger, "start");

	//自車両内のIS向け送信用socketFD作成
	UdpProcClient ownisudpprocclient;
	sockaddr_un own_is_addr = ownisudpprocclient.Init(confDirPath + FD_CStoIS);

    // 自車のCS(VehicleProcRcv)向け送信用socketFD作成
    UdpProcClient owncsudpprocclient;
    sockaddr_un own_cs_addr = owncsudpprocclient.Init(confDirPath + FD_CStoCS);

    // Security向け送信用socketFD作成
    UdpProcClient ownsecudpprocclient;
    sockaddr_un own_sec_addr = ownsecudpprocclient.Init(confDirPath + FD_CStoSEC);

    struct clientdata cdata;    // 受信データ　＋　送信元IPアドレスの構造体
    bool send_result = false;
    int max_sendto_times = me->settings.max_sendto_times;
    int cs_duplicate_check = me->settings.cs_duplicate_check;

    long duplicate_cnt = 0;
    string log_str = "";
    while (1)
    {
        //LOG4CXX_DEBUG(me->logger, "pop before");
        cdata = me->p_queue->Pop();
        //LOG4CXX_DEBUG(me->logger, "pop after");
        log_str = Util(me->dm2util).PrintSend_message(cdata.msg);
        LOG4CXX_INFO(me->logger, log_str);

#if TRACELOG == 1
        // tracelogの取得と格納
        get_tracelog(trace_on, ref(cdata.msg), my_sid, VNWWT, rcv_q.Size());
#endif

        // 重複受信チェック
        if (cs_duplicate_check == 1)
        {
            if( me->duplicate_receive_check(me, cdata, sid_sq_id_received_time_map))
            {
                duplicate_cnt++;
                string msg = "重複受信チェックに該当 : No." + to_string(duplicate_cnt) + ",src_sid:" + to_string(cdata.msg.src_station_id) + ",check_id:" + to_string(cdata.msg.duplication_check_id);
                // 重複あり
                LOG4CXX_INFO(me->logger, msg);
                if (duplicate_cnt % 100000 == 1) {
                    LOG4CXX_WARN(me->logger, msg);
                    if (duplicate_cnt == std::numeric_limits<long>::max()) duplicate_cnt = 0;
                }
                // 送信しない
                continue;
            }
        }

         // 送信成否をクリア(成功時にtrueとなる)
        send_result = false;

         // ■IS部にデータを送信
        if((cdata.msg).msg_type == DM2TYPE_IS)
        {
            LOG4CXX_DEBUG(me->logger, "IS部に送信 開始");
            for(int i = 0; i< max_sendto_times; i++)
            {
			    // UdpReceiveInterfaceには client_data型で渡す
                if(ownisudpprocclient.Sendto(cdata, own_is_addr) < 0) {
                    send_result = false;
                    LOG4CXX_DEBUG(me->logger, "IS部に送信 失敗");
                }
                else {
                    send_result = true;
                    LOG4CXX_DEBUG(me->logger, "IS部に送信 成功");
                    break;
                }
            }
        }
        // ■APL部にデータを送信
        else if((cdata.msg).msg_type == DM2TYPE_APL)
        {
            string fd_path = confDirPath + string((cdata.msg).fd_name);
            LOG4CXX_DEBUG(me->logger, "APL部に送信 開始");
            UdpProcClient udpclientapl;
            sockaddr_un server_apl_addr = udpclientapl.Init(confDirPath);
            for(int i = 0; i < max_sendto_times; i++)
            {
			    // UdpReceiveInterfaceには client_data型で渡す
                if(udpclientapl.Sendto(cdata, server_apl_addr) < 0) {   
                    send_result = false;
                    LOG4CXX_DEBUG(me->logger, "APL部に送信 失敗");
                    // APLへの送信に失敗した場合、socketを再生成してデータ送信を試みる
                    udpclientapl.CloseSocketFd();
                    server_apl_addr = udpclientapl.Init(confDirPath);
                }
                else {
                    send_result = true;
                    LOG4CXX_DEBUG(me->logger, "APL部に送信 成功");
                    break;
                }
            }
            //udpclientaplのクローズ処理
            udpclientapl.CloseSocketFd();
        }
        //■CS(ProcRcv)にデータを転送
        else if((cdata.msg).msg_type == DM2TYPE_CS)
        {
            LOG4CXX_DEBUG(me->logger, "CSに送信 開始");
            if((cdata.msg).cs_message_detail == DM2CS_MSG_ACK)
            {
                (cdata.msg).transmission_flag++; // CSへの転送フラグをプラス１
                for(int i = 0; i < max_sendto_times; i++)
                {
                    if(owncsudpprocclient.Sendto(cdata.msg, own_cs_addr) < 0) {
                        send_result = false;
                        LOG4CXX_DEBUG(me->logger, "CSに送信 失敗");
                    } 
                    else {
                        send_result = true;
                        LOG4CXX_DEBUG(me->logger, "CSに送信 成功");
                        break;
                    }
                }
            }
        }
        //■Security部にデータを送信
        else if((cdata.msg).msg_type == DM2TYPE_SEC)
        {
            LOG4CXX_DEBUG(me->logger, "Security部に送信 開始");
            for(int i = 0; i< max_sendto_times; i++)
            {
			    // UdpReceiveInterfaceには client_data型で渡す
                if(ownisudpprocclient.Sendto(cdata, own_sec_addr) < 0) {
                    send_result = false;
                    LOG4CXX_DEBUG(me->logger, "Security部に送信 失敗");
                }
                else {
                    send_result = true;
                    LOG4CXX_DEBUG(me->logger, "Security部に送信 成功");
                    break;
                }
            }
        }
        if (send_result == false)
        {   // APL/IS/CS_ProcServerへの送信が規定回数連続で失敗した
            LOG4CXX_WARN(me->logger, "Sendto Upper over max_sendto_times");
        }
    }
}

/**
 * @fn int ProcSender::duplicate_receive_check(clientdata& cdata)
 * @brief 同時通信重複チェック
 * @param cdata 受信データ構造体
 * @return int 0:重複データなし, 1:重複データあり
 * @attention CsModule.h dual_connect_check()を参考に作成。
 *            ただし、重複受信数が状況によって変わるため重複確認マップからのキー削除は定期的な削除のみとする
 *            マップは既存のUnorderedMapではなく、std::mapを使用
 */
int ProcSender::duplicate_receive_check(const ProcSender* param, clientdata &cdata, UnorderedMap<string, time_t> &duplicate_check_map)
{
    const ProcSender* me = param;

	// strcpy(cdata.from_ip, src_ip); //送信元IPアドレスの格納
	int ret = 0;
	// 既に受信済になっていないか、重複確認マップを検索
    string key = to_string((cdata.msg).src_station_id) + to_string((cdata.msg).duplication_check_id);

    LOG4CXX_DEBUG(me->logger, "重複チェック開始：" + key);
    if (duplicate_check_map.UnorderedMapKeyExist(key) )
    {
        LOG4CXX_DEBUG(me->logger, "重複あり");
        //重複データあり
		ret = 1;
	}
    else
    {
        LOG4CXX_DEBUG(me->logger, "重複なし");
        //重複データなし
        ret = 0;

		//重複データがなければ重複確認マップに格納
        duplicate_check_map.UnorderedMapInsert(key, time(NULL));
	}

	return ret;
}

/**
 * @fn void ProcSender::worker_clean_map(UnorderedMap<string, time_t>& vehicle_sid_sq_id_received_time_map_th) 
 * @brief スレッド。引数のマップを指定間隔で削除
 */
void ProcSender::clean_duplicate_map(const ProcSender* param, UnorderedMap<string, time_t>& sid_sq_id_received_time_map)
{
    const ProcSender* me = param;

	while(1)
    {
		LOG4CXX_INFO(me->logger, "sid_sq_id_received_time_map削除処理実行");
		sid_sq_id_received_time_map.UnorderedMapEraseOldTime(me->settings.dual_map_clear_time);

		sleep(me->settings.dual_map_clear_time);
	}
}
