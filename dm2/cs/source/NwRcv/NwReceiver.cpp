#include "NwReceiver.h"

using namespace std;
using namespace CS;
using namespace log4cxx;

/**
 * @fn		NwReceiver::NwReceiver()
 * @brief	NwReceiverコンストラクタ
 * @author	Nagoya University
 * @date	2023/03/21
 * @return
 */
NwReceiver::NwReceiver(
		NwRcvSettings &tmp_settings,
		uint count
		)
{
	memcpy(&settings, &tmp_settings, sizeof(tmp_settings));
	memcpy(&setting_num, &count, sizeof(count));
	// log4cxxの初期化
	logger = Logger::getLogger("NwReceiver");

	// 通信方法の設定
	switch(settings.socket_types[setting_num]) {
		case 0:
		case 10:
		case 11:
			server = new UdpNwServer();
			break;
		case 1:
			server = new UdpMcNwServer();
			break;
		case 2:
			server = new DtlsNwServer();
			break;
	}
}
/**
 * @fn		NwReceiver::~NwReceiver()
 * @brief	NwReceiverデストラクタ
 * @author	Nagoya University
 * @date	2023/03/21
 * @return
 */
NwReceiver::~NwReceiver(void)
{
	delete pthread;
}

/**
 * @fn		ProcReceiver::Run()
 * @brief	起動処理
 * @author	Nagoya University
 * @date	2023/03/21
 * @return
 */
std::thread* NwReceiver::Run(CS::Queue<CS::clientdata>* queue)
{
	LOG4CXX_INFO(logger, settings.interface_names[setting_num] + " " + "start");
	p_queue = queue;

	// ネットワーク受信用スレッド生成
	pthread = new thread(receiver, this, settings.dm2util.getConfigDirectory());
	LOG4CXX_INFO(logger, settings.interface_names[setting_num] + " " + "exit");

	return pthread;
}

/**
 * @fn		ProcReceiver::receiver()
 * @brief	受信処理
 * @author	Nagoya University
 * @date	2023/03/21
 * @return
 */
void NwReceiver::receiver(const NwReceiver* param, const string &confDirPath)
{
	const NwReceiver* me = param;
	string interface_name = me->settings.interface_names[me->setting_num];
	string udp_port_number = me->settings.udp_port_numbers[me->setting_num];
	int ip_ver = me->settings.interface_ip_vers[me->setting_num];
	int socket_type = me->settings.socket_types[me->setting_num];
	int cs_packet_size = me->settings.cs_packet_sizes[me->setting_num];
	string aes_key = me->settings.udp_etm_aes_keys[me->setting_num];

	LOG4CXX_INFO(me->logger, interface_name + " " + "receiver start");
	string log_str = "";
	string ivFilePath = confDirPath + UDP_IV_FILE;
	string fd_cs_to_cs = confDirPath + FD_CStoCS;
	// epollで使用する変数
	int epfd;
	int nfds;
	int i;
	struct epoll_event ev, ev_ret[NEVENTS];
	int sockfd_common; // それぞれのインタフェースの共通socketディスクリプタ
	string ip_address;
	switch(socket_type) {
		case 0:
		case 10:
		case 11:
			sockfd_common = ((UdpNwServer*)(me->server))->Init(interface_name, udp_port_number, "", ip_address, ip_ver, confDirPath);
			break;
		case 1:
			sockfd_common = ((UdpMcNwServer*)(me->server))->Init(udp_port_number, interface_name, ip_ver);
			break;
		case 2:
 			sockfd_common = ((DtlsNwServer*)(me->server))->Init(interface_name, ip_ver, udp_port_number, "", confDirPath);
			break;
	}

	if (sockfd_common < 0)
	{
		LOG4CXX_WARN(me->logger, interface_name + " " + "server Init error");
		exit(EXIT_FAILURE);			// [TODO]失敗した際にexitでよいか？
	}
	LOG4CXX_INFO(me->logger, interface_name + " " + "sockfd_common = " + to_string(sockfd_common));

	// epollで利用するためのインスタンスを生成
	epfd = epoll_create(NEVENTS);
	if (epfd < 0)
	{
		LOG4CXX_WARN(me->logger, interface_name + " " + "epoll_create fail");
		exit(EXIT_FAILURE);			// [TODO]失敗した際にexitでよいか？
	}

	// 共通ソケットの追加
	memset(&ev, 0, sizeof(ev));
	ev.events = EPOLLIN;
	ev.data.fd = sockfd_common;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd_common, &ev) != 0)
	{
		LOG4CXX_WARN(me->logger, interface_name + " " + "epoll_ctl_common fail");
		exit(EXIT_FAILURE);			// [TODO]失敗した際にexitでよいか？
	}
	send_message buf;	// 受信データ構造体のオブジェクト
	clientdata m_cdata; // スレッドに渡す受信データ構造体のオブジェクト
	struct sockaddr_storage ss; // IPv4/IPv6の両方に対応するための構造体

	unsigned long long now_time;
	struct timespec ts;
	// 分割ペイロードサイズ = 分割全体サイズ - ヘッダのサイズ (send_message構造体からペイロードサイズを引いたもの)
	int division_size = cs_packet_size - (sizeof(buf) - MSGSIZE);

	int combination_map_clear_time_ = 100;
	UnorderedMap<std::string, time_t> flagment_data_receive_time_map;
	UnorderedMap<std::string, std::vector<std::string>> flagment_data_combination_map;
	std::thread th0(ClearUnorderedMap, 
			std::ref(combination_map_clear_time_),
			std::ref(flagment_data_combination_map),
			std::ref(flagment_data_receive_time_map));

	bool doFirst = false;
	int flagment = 0, flagment_max = 0;
	std::string key = "";
	std::string combined_payload = "";

	sockaddr_un own_cs_addr;
	UdpProcClient owncsudpprocclient;
	//自クラウドのCS(ProcRcv)向け送信用socketFD作成
	own_cs_addr = owncsudpprocclient.Init(fd_cs_to_cs);
	while (1)
	{
		nfds = epoll_wait(epfd, ev_ret, NEVENTS, EPOLL_TIMEOUT);
		if (nfds < 0)
		{
			LOG4CXX_WARN(me->logger, interface_name + " " + "epoll_wait fail");
			exit(EXIT_FAILURE);
		}
		for (i = 0; i < nfds; i++)
		{
			if (ev_ret[i].data.fd == sockfd_common)
			{
				// ここから受信処理
				log_str = "sockfd_commonで受信 ev_ret[" + to_string(i) + "].data.fd == " + to_string(ev_ret[i].data.fd);
				LOG4CXX_INFO(me->logger, interface_name + " " + log_str);

				// 通信タイプ別で受信
				int res = 0;
				memset(&buf, 0, sizeof(buf));
				switch(socket_type) {
					case 0:
						LOG4CXX_DEBUG(me->logger, "UDP(ユニキャスト, 暗号化なし) 受信開始");
						res = ((UdpNwServer*)(me->server))->Recvfrom(sockfd_common, buf, ss);
						break;
					case 10:
						LOG4CXX_DEBUG(me->logger, "UDP(ユニキャスト, 暗号化あり) 受信開始");
						//res = ((UdpNwServer*)(me->server))->RecvfromDecrypt(sockfd_common, buf, ss, sizeof(buf), iv);
						res = ((UdpNwServer*)(me->server))->RecvfromEtM(sockfd_common, buf, ss, sizeof(buf), aes_key);
						break;
					case 11:
						LOG4CXX_DEBUG(me->logger, "UDP(ユニキャスト, 暗号化・署名あり) 受信開始");
						//res = ((UdpNwServer*)(me->server))->RecvfromDecrypt(sockfd_common, buf, ss, sizeof(buf), iv);
						res = ((UdpNwServer*)(me->server))->RecvfromEtMonPki(sockfd_common, buf, ss, sizeof(buf), aes_key);
						break;
					case 1:
						LOG4CXX_DEBUG(me->logger, "UDP(マルチキャスト) 受信開始");
						res = ((UdpNwServer*)(me->server))->Recvfrom(sockfd_common, buf, ss);
						break;
					case 2:
						LOG4CXX_DEBUG(me->logger, "DTLS 受信開始");
						res = ((DtlsNwServer*)(me->server))->Recvfrom(sockfd_common, ss, me->p_queue, cs_packet_size, me->settings.my_sid, fd_cs_to_cs);
						break;
				}

				if (res < 0)
				{
					LOG4CXX_INFO(me->logger, interface_name + " " + "Recvfrom fail");
					continue;
				}
				bool doSend = false;
				//std::cout << res << std::endl;
				if (res >= 0) {
					if(buf.flagment_sum == 1){
						doSend = true;
					}else if(buf.flagment_sum > 1){
						//分割されたデータの結合処理
						flagment = buf.flagment_offset;
						flagment_max = buf.flagment_sum;
						key = std::to_string(buf.src_station_id).append(std::to_string(buf.flagment_duplication_check_id));

						//flagment_data_receive_time_mapを更新
						try{
							flagment_data_receive_time_map.UnorderedMapUpdate(key, time(NULL));
						}catch(std::out_of_range& e){
							if(flagment_data_receive_time_map.UnorderedMapInsert(key, time(NULL)) < 0){
								#if DEBUG >= 2
								std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "keyが存在するのでInsert中止" << std::endl;
								#endif
							}
						}

						// flagment_data_combination_mapに受信途中がないかチェック
						if(flagment_data_combination_map.UnorderedMapKeyExistVector(key) == true){
							// 受信途中が存在する場合
							try{
								flagment_data_combination_map.UnorderedMapUpdateVectorValueThenDecrease(key, flagment, std::string(buf.dm2_payload, division_size));
							}catch(std::out_of_range& oor){
								std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "out_of_range" << std::endl;
							}
							//keyのvectorの要素数がflagment_maxと同値なら全てデータが揃ったと判断し、結合及び削除してからnotifyする。
							combined_payload = flagment_data_combination_map.UnorderedMapVectorCombineAndDeletePlusSize(key, flagment_max);

							if(combined_payload.length() > 0){
								combined_payload.copy(buf.dm2_payload, MSGSIZE);
								buf.flagment_offset = 0;
								buf.flagment_sum = 1;
								doSend = true;
								flagment_data_receive_time_map.UnorderedMapErase(key);
							}
						}else{
							// 受信途中が存在しない場合
							try{
								flagment_data_combination_map.UnorderedMapInsertVectorPlusSize(key, flagment_max, flagment, std::string(buf.dm2_payload, division_size));
							}catch(std::out_of_range& oor){
								std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "out_of_range" << std::endl;
							}
						}

					}else{
						std::cout << "FILE:" << __FILE__ <<  ", LINE:" << __LINE__ << " " << "受信データのflagment_sum値が想定外:" << buf.flagment_sum << std::endl;
					}
				}
				if (!doSend) continue;
				//std::cout << buf.dm2_payload << std::endl;
				LOG4CXX_INFO(me->logger, interface_name + " " + "Recvfrom Success!");
				m_cdata.msg = buf;
				strcpy(m_cdata.from_ip, ip_address.c_str());
				log_str = Util(me->dm2util).PrintSend_message(m_cdata.msg);
				LOG4CXX_INFO(me->logger, interface_name + " " + log_str);
				LOG4CXX_DEBUG(me->logger, m_cdata.msg.dm2_payload);

				// 宛先SIDが自分(車両)宛てである場合
				if((m_cdata.msg).dst_station_id == me->settings.my_sid || (m_cdata.msg).dst_station_id == 90000000)
				{
					LOG4CXX_DEBUG(me->logger, "interface_name + " " + 宛先SIDが自分(車両)宛てである場合");

					clock_gettime(CLOCK_REALTIME, &ts);
					now_time = ts.tv_sec * 1000000000 + ts.tv_nsec;
					log_str =  "lid=," + to_string((m_cdata.msg).lane_id) + "," + to_string(now_time).substr(6);
					LOG4CXX_INFO(me->logger, log_str);

					// APL/IS/CSProcへ送るため、キューに入れて受け渡す(同時通信重複チェックはキューから取得後行う)
					me->p_queue->Push(m_cdata);
				} 
				//宛先SIDが0の場合(想定外)
				else if((m_cdata.msg).dst_station_id == 0){
					LOG4CXX_WARN(me->logger, interface_name + " " + "宛先SIDが0の場合(想定外)");
				}
				//宛先SIDが自分(車両)宛て以外の場合
				else{
					if (!doFirst) {
						LOG4CXX_WARN(me->logger, interface_name + " " + "宛先SIDが自分(車両)宛て以外の場合");
						doFirst = true;
					}
					if ((m_cdata.msg).transmission_flag <= 10) { 
						(m_cdata.msg).transmission_flag++; //転送フラグをプラス１
						owncsudpprocclient.Sendto(m_cdata.msg, own_cs_addr);
					}
					// 自分宛以外の場合は、そのままメッセージを送るべき宛先に送付する
					//me->p_queue->Push(m_cdata);
				}
			}
			else {
			 	log_str = "上記以外 ev_ret[" + to_string(i) + "].data.fd == " + to_string(ev_ret[i].data.fd);
			 	LOG4CXX_INFO(me->logger, interface_name + " " + log_str);
			}
		}
	}
	owncsudpprocclient.CloseSocketFd();
}

void NwReceiver::ClearUnorderedMap(
			int& combination_map_clear_time_th,
			UnorderedMap<std::string, std::vector<std::string>>& flagment_data_combination_map_th,
			UnorderedMap<std::string, time_t>& flagment_data_receive_time_map_th
			)
{
	while(1){
		std::string key_ = "0";
		while(1){
			//◆結合MAP管理用MAPにて現在時刻より指定時間以上古いデータの車両SID＋フラグメントシーケンス番号を取得
			key_ = flagment_data_receive_time_map_th.UnorderedMapOldKeyString(combination_map_clear_time_th);
			if(key_ != "0"){
				//◆取得した車両SIDをkey値として、結合MAPからエントリを削除。
				flagment_data_combination_map_th.UnorderedMapErase(key_);
				//◆取得した車両SIDをkey値として、結合MAP管理用MAPからエントリを削除。
				flagment_data_receive_time_map_th.UnorderedMapErase(key_);
			}else{
				break;
			}
		}
		sleep(combination_map_clear_time_th);
	}
}