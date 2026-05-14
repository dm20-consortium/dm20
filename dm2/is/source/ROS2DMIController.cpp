#include "ROS2DMIController.h"



namespace IS {
    ROS2DMIDownloaderWrapper &downloader = ROS2DMIDownloaderWrapper::get_instance();
    /**
     * コンストラクタ
     *
     * @author   Naoto Komatsu
     * @date     2023/12/20
     */
    ROS2DMIController::ROS2DMIController() {
    }

    /**
     * デストラクタ
     *
     * @author   Naoto Komatsu
     * @date     2023/12/20
     */
    ROS2DMIController::~ROS2DMIController() {
    }

    /**
     * 初期化処理
     *
     * @author   Naoto Komatsu
     * @date     2023/12/20
     */
    void ROS2DMIController::init() {
    }

    /**
     * 開始処理
     *
     * @author   Shinichi Kusayama
     * @date     2024/05/14
     */
    void ROS2DMIController::start() {
		uploaderStart();
		downloaderStart();
    }

    /**
     * 終了処理
     *
     * @author   Shinichi Kusayama
     * @date     2024/05/14
     */
    void ROS2DMIController::stop() {
        uploaderStop();
        sleep(1);	//
        downloaderStop();
    }

    /**
     * ROS2DMIからのデータ受信スレッド起動
     *
     * @author   Naoto Komatsu
     * @date     2023/12/20
     */
    void ROS2DMIController::uploaderStart() {
        uploader.init();
        uploader.start();
    }

    /*
     * publish処理
     *
     * @author   Naoto Komatsu
     * @date     2023/12/28
     */
    void ROS2DMIController::downloaderStart() {
        downloader.init();
    }


    /**
     * Receiver終了処理
     *
     * @author   Naoto Komatsu
     * @date     2023/12/20
     */
    void ROS2DMIController::uploaderStop() {
        uploader.stop();
    }

    /**
     * Sender終了処理
     *
     * @author   Naoto Komatsu
     * @date     2023/12/20
     */
    void ROS2DMIController::downloaderStop() {
        downloader.stop();
    }
    /*
     * publish処理
     *
     * @author   Naoto Komatsu
     * @date     2023/12/28
     */
    void ROS2DMIController::wrapPublishTo(IS::TupleSet &tupleset, string &tableName) {
        downloader.publishTo(tupleset, tableName);
    }

    /*
     * ROS2DMIの異常監視スレッドの立ち上げ
     *
     * @author   Naoto Komatsu
     * @date     2024/3/14
     */
    void ROS2DMIController::errorHandling() {
        thread ros2dmiErrHndl(&ROS2DMIController::watchDogProcess, this);
        ros2dmiErrHndl.detach();
        logger_ros->warn("# ROS2-DMI_WatchDog has started.");
    }

    /*
     * ROS2DMIの異常監視を行う
     *
     * @author   Naoto Komatsu
     * @date     2024/3/14
     */
    void ROS2DMIController::watchDogProcess() {
        while(true) {
            if(uploaderErrorFlag) {
                ROS2DMIController::uploaderStart();
                logger_ros->warn("# ROS2-DMI_Uploader is restarting.");
            }
            if(downloaderErrorFlag) {
                ROS2DMIController::downloaderStart();
                logger_ros->warn("# ROS2-DMI_Downloader is restarting.");
            }
        }
    }
}
