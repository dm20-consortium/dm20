#include "InformationSourceManager.h"
#include "QueryReceiver.h"
#include "StreamReceiver.h"
#include "Settings.h"
#include "SessionManager.h"
#include "LocationManager.h"
#include "CAMReceiver.h"
#include "CsReceiver.h"
#include "UdpSendInterface.h"
#include "ProtobufParser.h"
#include "ROS2DMIController.h"
#include "UDPDMIController.h"
#include "ProtobufDmiController.h"

#include <iostream>
#include <string>
#include <csignal>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <fcntl.h>
#include <sys/file.h>
using namespace std;
using namespace log4cxx;

using std::cout;		// cout
using std::endl;		// endl
using std::string;		// string

LoggerPtr logger = Logger::getLogger("Main");

volatile std::sig_atomic_t gSignalStatus;

void signal_handler(int signal)
{
	gSignalStatus = signal;
	logger->info("Signal received. sigNum:" + std::to_string(signal));
}

void notice_function(const char *message, ...){ printf("%s", message); } // 依存ライブラリの出力用関数
void usage(const char *cmd);
bool isExpired(const char* lockFile, int timeoutSeconds);
void repeatUpdateTimestamp(const char* lockFile, int updateInterval);
bool fctlLockFile(const char* lockFile, int updateInterval);
int isProc(int argc, char* argv[]);
const char* processLockFile = "/tmp/dm2is.lock";
/**
 * メイン処理
 *
 * @author	Shinichi Kusayama
 * @date	2024/12/29
 *
 * @return	isProc結果
 */

int main(int argc, char* argv[])
{	
	int ret = isProc(argc, argv);

    return ret;
}
/**
 * Information Support メイン処理
 *
 * @author	Nagoya University
 * @date	2018/03/13
 *
 * @return	0:正常 1:異常
 */
int isProc(int argc, char* argv[])
{	
	std::signal(SIGINT, signal_handler);
	//std::signal(SIGTERM, signal_handler);
	//std::signal(SIGABRT, signal_handler);
	std::signal(SIGKILL, signal_handler);

	initGEOS( notice_function, notice_function );
	IS::Settings &settings					= IS::Settings::get_instance();
	//IS::InformationSourceParser &isp		= IS::InformationSourceParser::get_instance();
	IS::InformationSourceManager &ism		= IS::InformationSourceManager::get_instance();
	IS::CAMReceiver *recvCam				= NULL;
	IS::QueryReceiver *recvQuery			= NULL;
	IS::StreamReceiver *recvStream			= NULL;
	IS::QueryReceiver *recvQuerySSL			= NULL;
	IS::StreamReceiver *recvStreamSSL		= NULL;
	IS::QueryReceiver *recvQueryAdmin		= NULL;
	IS::StreamReceiver *recvStreamAdmin		= NULL;
	IS::CsReceiver csRecv;
	IS::QueryManager &QeryM	= IS::QueryManager::get_instance();
	IS::QueueManager &QueuM = IS::QueueManager::get_instance();
	IS::SessionManager &SM	= IS::SessionManager::get_instance();
	IS::LocationManager &LM = IS::LocationManager::get_instance();

#if USE_UDP_DMI == 1
	IS::UDPDMIController udpDmiCtrl;
#endif
#if USE_ROS2DMI == 1
	IS::ROS2DMIController ros2DmiCtrl;
#endif

  IS::ProtobufDmiController::SharedPtr pbDmiCtrl;
#if USE_PROTOBUF_DMI == 1
  pbDmiCtrl.reset(new IS::ProtobufDmiController());
#endif

	string fileName = "";
	string confDirPath = "";
	string confDirPathFromDB = "";
	string my_sid = "";
	string schemaForPresetQuery = "";
	string windowForPresetQuery = "";
	int vehicleNum = 0;
	string opt;
	CS::Util util;
	// Protocol Buffersのバージョンに互換性の問題が無いかチェック
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	int ch;
	while ((ch = getopt(argc, argv, "d:f:n:s:S:w:h")) != -1) {
		switch (ch) {
		case 'd':
			opt = optarg;
			if (opt.substr(0,1) != "-") confDirPath = opt;
			break;
		case 'f':
			opt = optarg;
			if (opt.substr(0,1) != "-") fileName = opt;
			break;
		case 'n':
			opt = optarg;
			if (opt.substr(0,1) != "-") vehicleNum = stoi(opt);
			break;
		case 's':
			opt = optarg;
			if (opt.substr(0,1) != "-") {
				try {
					my_sid = opt;
					stoull(opt);
				} catch (...) {
					fprintf(stderr, "failed to parse station-id-value: %s\n", opt.c_str());
					usage(argv[0]);
					exit(1);
				}
			}
			break;
		case 'w':
			opt = optarg;
			if (opt.substr(0,1) != "-") {
				try {
					windowForPresetQuery = opt;
					stoi(opt);
				} catch (...) {
					fprintf(stderr, "failed to parse window-size-value: %s\n", opt.c_str());
					usage(argv[0]);
					exit(1);
				}
			}
			break;
		case 'S':
			opt = optarg;
			if (opt.substr(0,1) != "-") schemaForPresetQuery = opt;
			break;
		default:
			usage(argv[0]);
		    exit(1);
		}
	}
	string env_path = "";
	if (confDirPath == "") {
		// 環境変数を取得
		env_path = util.getAndCheckConfDirPath(false);
		if (env_path != "") confDirPath = env_path;
	}
	if (confDirPath != "") {
		if (util.chkDirPath(confDirPath)) {
			if (env_path == "") {
				cout << "[INFO] Config Directory set from arg [-d]" << endl;
			} else {
				cout << "[INFO] Config Directory set from env [DM2_CONF_DIR_PATH] " << endl;
			}
			settings.setConfigDirectory(confDirPath);
		} else {
			cout << "[WARN] config Directory does not exist. " << endl;
			confDirPath = "";
		}
	}
	if (confDirPath == "") {
		cerr << "[ERROR] Set Config Directory in 2-Way. \n  1) arg [-d] \n  2) env [DM2_CONF_DIR_PATH] \n " << endl;
		return 1;
	}
	try {
		// log4cxxの初期化
		PropertyConfigurator::configure(settings.getConfigDirectory() + settings.CONF_FILE_LOG);
		// 初期設定のロード (DBから設定済みの場合はコールしない)
		if (confDirPathFromDB == "") settings.initSettings();
		if (my_sid != "") settings.setMyStationID(my_sid);
		// Xercesの初期化
		//isp.init();

		// リレーショナルスキーマの取得
		bool first_ism_error = true;
		while (!ism.init()) {
			if (first_ism_error) {
				cout << "[INFO] Waiting to connect to the RDB." << endl;
				first_ism_error = false;
			}
			sleep(2);
		}

		// QueueManagerの初期化
		QueuM.init();

		// LocationManagerの初期化
		LM.init();

		// QueryManagerの初期化
		QeryM.init();

		// SessionManagerの初期化
		SM.init(confDirPath);

		// Queueの初期化
		QueuM.initQueue();

		// CAM受信スレッドの起動
		recvCam = new IS::CAMReceiver("CAM", 9000);
		recvCam->start();

		// クエリ受信スレッドの起動
		recvQuery = new IS::QueryReceiver("QUERY", 9001);
		recvQuery->start();

		// ストリーム受信スレッドの起動
		recvStream = new IS::StreamReceiver("STREAM", 9002);
		recvStream->start();
		if (fileName.length() != 0) {
			recvStream->fileReadStart(fileName, vehicleNum);
		}

		// クエリ受信(admin)スレッドの起動
		recvQueryAdmin = new IS::QueryReceiver("QUERY_ADMIN", 10001);
		recvQueryAdmin->start();

		// ストリーム受信(admin)スレッドの起動
		recvStreamAdmin = new IS::StreamReceiver("STREAM_ADMIN", 10002);
		recvStreamAdmin->start();

		if (settings.getParameter("IS_ENABLE_SSL") != "0") {
			// SSLライブラリの初期化処理
			NetworkSource::SSL_init();

			// クエリ受信スレッドの起動
			recvQuerySSL = new IS::QueryReceiver("QUERY_SSL", 11001);
			recvQuerySSL->start(true);

			// ストリーム受信スレッドの起動
			recvStreamSSL = new IS::StreamReceiver("STREAM_DTLS", 11002);
			recvStreamSSL->start(true);
		}

		// CS受信スレッドの起動
		csRecv.start();

		// プリセットクエリの登録
		QeryM.registerPresetQueryUsingPath(settings.getConfigDirectory() + settings.CONF_DIR_PRESET_TIMER);
		QeryM.registerPresetQueryUsingPath(settings.getConfigDirectory() + settings.CONF_DIR_PRESET_QUERY);
		if (schemaForPresetQuery != "") QeryM.registerPresetQueryUsingSchema(schemaForPresetQuery, windowForPresetQuery);

    	// 各種インターフェースの起動
#if USE_UDP_DMI == 1
		udpDmiCtrl.start();
#endif
#if USE_ROS2DMI == 1
		ros2DmiCtrl.start();
#endif
    if (pbDmiCtrl != nullptr) pbDmiCtrl->start();

		logger->info("===== DM2.0PF DB SYSTEM START =====");
		logger->debug("Please Ctl+c or kill command to finish DB system.");
		while (true)
		{
			if (gSignalStatus != 0) break;
			sleep(2);
		}
	}
	catch (std::bad_alloc& e) {
		string what(e.what());
		logger->error("bad_alloc caught: " + what);
	}
	catch (std::exception& e) {
		string what(e.what());
		logger->error("exception caught: " + what);
	}
	LM.stop();

	recvCam->stop();
	recvQuery->stop();
	recvStream->stop();
	recvQueryAdmin->stop();
	recvStreamAdmin->stop();

#if USE_UDP_DMI == 1
	udpDmiCtrl.stop();
#endif
#if USE_ROS2DMI == 1
	ros2DmiCtrl.stop();
#endif
  if (pbDmiCtrl != nullptr) pbDmiCtrl->stop();

	delete recvCam;
	delete recvQuery;
	delete recvStream;
	delete recvQueryAdmin;
	delete recvStreamAdmin;


	if (settings.getParameter("IS_ENABLE_SSL") != "0") {
		recvQuerySSL->stop();
		recvStreamSSL->stop();
		delete recvQuerySSL;
		delete recvStreamSSL;

		NetworkSource::SSL_finalize();
	}

	//isp.finalize();
	//終了せず固まってしまうため、暫定的にコメントアウト
	//QeryM.stop();
	//QueuM.stop();
	finishGEOS();
	// ロック解放
	unlink(processLockFile);
	if (gSignalStatus == SIGINT || gSignalStatus == SIGTERM) {
		logger->info("===== DM2.0PF DB SYSTEM END =====");
		//終了せず固まってしまうため、暫定的にquick_exit関数で強制終了させる
		quick_exit(0);
	}
	logger->info("===== DM2.0PF DB SYSTEM END =====");
	return 0;
}

/**
 * ロックファイルの期限切れチェック
 *
 * @author	Shinichi Kusayama
 * @date	2024/12/29
 *
 * @param 	lockFile		ロックファイル
 * @param 	timeoutSeconds	タイムアウト秒
 *
 * @return	期限切れか否か
 */
bool isExpired(const char* lockFile, int timeoutSeconds) {
    struct stat fileStat;
    if (stat(lockFile, &fileStat) == -1) return true; // ファイルが存在しない場合、期限切れとみなす

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	if ((now - fileStat.st_mtime) > timeoutSeconds) {
		std::cout << "[isExpired] Now: " << now << ", Last Modified: " << fileStat.st_mtime
          << ", Elapsed: " << now - fileStat.st_mtime << " seconds" << std::endl;

	}
    return (now - fileStat.st_mtime) > timeoutSeconds;
}
/**
 * ロックファイル更新ループ
 *
 * @author	Shinichi Kusayama
 * @date	2024/12/29
 *
 * @param 	lockFile		ロックファイル
 * @param 	updateInterval	更新間隔
 *
 * @return	なし
 */
void repeatUpdateTimestamp(const char* lockFile, int updateInterval) {
    while (true) {
		int fd = open(lockFile, O_RDWR | O_CREAT, 0666);
		if (fd != -1) {
			if (write(fd, "1", 1) == -1) logger->info("updateTimestamp write error");
			close(fd);
		}
        std::this_thread::sleep_for(std::chrono::seconds(updateInterval));
    }
}
bool fctlLockFile(const char* lockFile, int updateInterval) {
	int fd = open(lockFile, O_RDWR | O_CREAT, 0666);
	if (fd != -1) {
		if (flock(fd, LOCK_EX | LOCK_NB) == 0) {
			if (write(fd, "1", 1) == -1) cerr << "fctlLockFile write error" << endl;;
			flock(fd, LOCK_UN);		// ロック解除
			close(fd);
			return true;
		} else {
			// locked by Another process
		}
	} else {
		cerr << "fctlLockFile open error" << endl;;
	}
	return false;
}
/**
 * 引数説明
 *
 * @author	Shinichi Kusayama
 * @date	2024/4/22
 *
 * @return	なし
*/
void usage(const char *cmd)
{
	printf("Usage: %s [options] \n"
		"\n"
		"Options: [] => Required arg. {} => Any arg. \n"
		"  -d <directory path>       Set Config Directory Path. \n"
		"                            [Another Way] export DM2_CONF_DIR_PATH=/home/dm2/dm2/conf\n"
		"  -f <input file>           Set Dummy Input File. \n"
		"  -n <vehicle num/100msec>  Set vehicle input data [num/100msec] from dummy input file. \n"
		"  -s <my station id>        Set My Station ID. \n"
		"  -S <schema>               [Test Mode] Set Preset-Query ==> MASTER schema SELECT * FROM schema [rows 1] Destination:90000000<Group Cast SID>  \n"
		"  -w <row size>             [Test Mode] Set Preset-Query-Window-Size  Default:rows 1 => rows <row size> \n"
		"  -h                        Show help. \n"
		"\n",
		cmd);
}
