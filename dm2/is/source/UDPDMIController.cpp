#include "UDPDMIController.h"

using namespace log4cxx;
LoggerPtr logger_udp = Logger::getLogger("UDPDMIController");

namespace IS {
    /**
     * コンストラクタ
     *
     * @author   Naoto Komatsu
     * @date     2024/02/14
     */
    UDPDMIController::UDPDMIController() {
    }

    /**
     * デストラクタ
     *
     * @author   Naoto Komatsu
     * @date     2024/02/14
     */
    UDPDMIController::~UDPDMIController() {
    }

    /**
     * 初期化処理
     *
     * @author   Naoto Komatsu
     * @date     2024/02/14
     */
    void UDPDMIController::init() {
    }

    /**
     * 開始処理
     *
     * @author   Shinichi Kusayama
     * @date     2024/05/14
     */
    void UDPDMIController::start() {
		uploaderStart();
		downloaderStart();
    }

    /**
     * Receiver終了処理
     *
     * @author   Naoto Komatsu
     * @date     2024/02/14
     */
    void UDPDMIController::stop() {
        downloader.stop();
    }

    /**
     * UDPDMIからのデータ受信スレッド起動
     *
     * @author   Naoto Komatsu
     * @date     2024/02/14
     */
    void UDPDMIController::uploaderStart() {
        uploader.init();
        uploader.start();
    }

    /**
     * UDPDMIからのデータ受信スレッド起動
     *
     * @author   Naoto Komatsu
     * @date     2024/02/14
     */
    void UDPDMIController::downloaderStart() {
        downloader.init();
        downloader.start();
    }

    /**
     * UDPDMIからのデータ受信スレッド起動
     *
     * @author   Naoto Komatsu
     * @date     2024/02/14
     */
    void UDPDMIController::wrapSendTo(IS::TupleSet &tupleset, string &tableName) {
        downloader.sendTo(tupleset, tableName);
    }

}
