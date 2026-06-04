#include "StreamReceiver.h"

using namespace std;

namespace IS {
	BufferQueue stream_rcv_q(100000);

	/**
	 * コンストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 *
	 * @param	name	ネットワーク名称
	 * @param	port	ポート番号
	 */
	StreamReceiver::StreamReceiver(string name, int port)
		: NetworkSource(name, port), FileSource(name)
	{

	}

	/**
	 * デストラクタ
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	StreamReceiver::~StreamReceiver()
	{
	}

	/**
	* 初期化処理
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*/

	void StreamReceiver::init()
	{
	}

	/**
	* ファイルによるストリームデータの読み込みスレッドを起動
	*
	* @author	Nagoya University
	* @date	2018/04/26
	*
	* @param	file	ファイル	
	* @param	num	読み込みたい車両台数を指定
	*/
	void StreamReceiver::fileReadStart(string file, int num)
	{
		thread fileReadThread(&StreamReceiver::fileRead, this, file, num);
		fileReadThread.detach();
	}

	/**
	* ファイルによるストリームデータの読み込み
	*
	* @author	Nagoya University
	* @date	2018/04/26
	*
	* @param	file	ファイル
	* @param	num	読み込む車両台数
	*/
	void StreamReceiver::fileRead(string file, int num)
	{
		fileReadInput(file, num);
	}

	/**
	* 受信スレッドの開始
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*/

	void StreamReceiver::start()
	{
		start(false);
	}

	/**
	* 受信スレッドの開始(暗号化可否指定)
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param	isSSL	暗号化実施可否
	*/

	void StreamReceiver::start(bool isSSL)
	{
		IS::Settings &settings = IS::Settings::get_instance();
		int max_worker_thread = std::stoi(settings.getParameter("STREAM_RECEIVE_WORKER_NUM"));

		if (!isSSL) {
			thread recvThread(&StreamReceiver::receiveUDPdata, this);
			/* TODO : スレッドの高優先度設定を試みたがあまり変わらず
			sched_param sch_params;
			sch_params.sched_priority = 99;
			if (pthread_setschedparam(recvThread.native_handle(), SCHED_RR, &sch_params)) {
				std::cerr << "Failed to set Thread scheduling : " << std::strerror(errno) << std::endl;
				//logger->error("Failed to set Thread scheduling : " + std::strerror(errno));
			}
			*/
			recvThread.detach();
		}
		else {
			thread recvThread(&StreamReceiver::receiveDTLSdata, this, setupDTLS());
			recvThread.detach();
		}

		for (int i = 0; i < max_worker_thread; i++) {
			thread workerThread(&StreamReceiver::workerProc, this, ref(stream_rcv_q));
			workerThread.detach();
		}
		logger->debug("StreamReceiveWorker thread created :" + std::to_string(max_worker_thread));
	}

	/**
	* 受信処理の終了
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*/

	void StreamReceiver::stop()
	{
		logger->debug("[StreamReceiver] exit_flag ON");
		NetworkSource::exit_flag = true;
	}

	/**
	* 受信処理
	*
	* @author	Nagoya University
	* @date	2018/03/13
	*
	* @param	data	受信データ.
	*/

	void StreamReceiver::receiveProcess(RecvData &data)
	{
		stream_rcv_q.Push(data);
	}

	void StreamReceiver::workerProc(BufferQueue& q)
	{
		IS::QueueManager &QM = IS::QueueManager::get_instance();
		IS::ProtobufParser &pp = IS::ProtobufParser::get_instance();
		IS::InformationSourceParser &isp = IS::InformationSourceParser::get_instance();

		IS::SessionManager &SM = IS::SessionManager::get_instance();
		RecvData data;
		string rootTagName, key, user;
		bool doSkipLog = true;
		// 性能測定用コード Start
		//long preTime = DmUtil::getTimeMicrosec();
		//int row = 0;
		// 性能測定用コード End
		while (1) {
			q.Pop(data);
			//【暫定対策用】ある端末(a)で自分が他の端末(b)に送るストリーム(A)と、
			// 他の端末(b)から(a)へ送られてくるストリーム(B)を分けたい場合に(A)を強制的にリネームする機能
			// ここで、リネームさせた場合、(a)、(b)両方の端末にリネームしたスキーマ情報(ex.is/conf/schema/sendobj.xml)が必要になる。
			// auto pos = data.payload.find("object_info");
			// if (pos != string::npos) {
			// data.payload.replace(pos, 11, "sendobj_info");
			// }

			pp.init();
			
			struct ProtobufHeaderInfo headerInfo;
			pp.getProtobufHeaderInfo(data.payload, headerInfo);
			if (headerInfo.headerSize > 0) {
				key = headerInfo.header.key;
			} else {
				//【暫定対策用】getRootTagNameAndKey内のtrancodeでSIGABRTするため
				if (data.payload.length() > 4100000) {
					if (doSkipLog) logger->warn("[workerProc] Skip Data<Length Over>:" + data.payload.substr(0,100) + ", ip:" + string(inet_ntoa(data.client.sin_addr)));
					doSkipLog = false;
					continue;
				}
				isp.init();
				if (!isp.getRootTagNameAndKey(data.payload, rootTagName, key)) {
					if (doSkipLog) logger->warn("[workerProc] Skip Data1:" + data.payload + ", ip:" + string(inet_ntoa(data.client.sin_addr)));
					isp.finalize();
					doSkipLog = false;
					continue;
				}
				isp.finalize();
				if (!SM.checkSession(key, user)) {
					if (doSkipLog) logger->warn("[workerProc] Skip Data1:" + data.payload + ", ip:" + string(inet_ntoa(data.client.sin_addr)));
					pp.finalize();
					doSkipLog = false;
					continue;
				}
			}
			// セッション確認（＆ユーザ情報取得）
			if (!SM.checkSession(key, user)) {
				if (doSkipLog) logger->warn("[workerProc] Skip Data2:" + data.payload + ", ip:" + string(inet_ntoa(data.client.sin_addr)));
				doSkipLog = false;
				continue;
			}
			QM.addQueueProc(data, user);
			// 性能測定用コード Start
			//row++;
			//if (row % 100 == 0) {
			//cout << row << "," << DmUtil::getTimeMicrosec()- preTime << endl;
			//preTime = DmUtil::getTimeMicrosec();
			//}
			// 性能測定用コード End
		}
	}
}
