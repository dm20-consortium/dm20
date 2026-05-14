#pragma once
#include <thread>
#include <log4cxx/logger.h>
#include <functional>

#include "Operator.h"
#include "is/DmUtil.h"

using namespace log4cxx;

namespace IS {

	/**
	 * 一定間隔でデータ取得要求を実施するクラス
	 * 主に時間Window指定による定期実行に用いる
	 *
	 * @author	Nagoya University
	 * @date	2018/03/14
	 */

	class TimerWorker {
	private:
		LoggerPtr logger = Logger::getLogger("TimerWorker");
	public:
		long intervalMsec = 0;
		std::function<void()> callbackFunction;

		/**
		 * コンストラクタ：（時間指定、駆動オペレータ）
		 *
		 * @author	Nagoya University
		 * @date	2018/03/14
		 *
		 * @param 		  	msec 	駆動間隔時間
		 * @param [in,out]	callbackFunction	コールバック関数
		 */

		TimerWorker(long msec, std::function<void()> callbackFunction) : abort_(false), th_(&TimerWorker::run, this) {
			//REL_COMMENT logger->trace("+TimerWorker::TimerWorker operator");
			this->intervalMsec = msec;
			this->callbackFunction = callbackFunction;
			//REL_COMMENT logger->trace("-TimerWorker::TimerWorker operator");
		}

		/**
		 * デストラクタ：確実にスレッドを完了させる
		 *
		 * @author	Nagoya University
		 * @date	2018/03/14
		 */

		virtual ~TimerWorker() {
			//REL_COMMENT logger->trace("+TimerWorker::~TimerWorker");
			abort_thread();
			if(th_.joinable()) th_.join();
			//REL_COMMENT logger->trace("-TimerWorker::~TimerWorker");
		}

		/**
		 * スレッド処理を中断させる
		 *
		 * @author	Nagoya University
		 * @date	2018/03/14
		 */

		void abort_thread() {
			std::lock_guard<std::mutex> lk(mtx_);
			if (!abort_) {
				logger->trace("TimerWorker::notify abort event");
				abort_ = true;
				cv_.notify_all();
			}
		}

	private:
		bool abort_;
		std::mutex mtx_;
		std::condition_variable cv_;

		// 宣言の順番に注意。threadオブジェクトは最後に初期化させる
		std::thread th_;

		/**
		 * スレッド実行
		 *
		 * @author	Nagoya University
		 * @date	2018/03/14
		 */

		void run() {
			usleep(10 * 1000); // コンストラクタの値代入より早くrunが実行される場合があるためsleep
			if (intervalMsec == 0) {
				logger->error("ERROR +TimerWorker::run Not defined param. So abort.");
				return;
			}
			logger->trace("+TimerWorker::run");

			// スレッド処理の待機時間の例（1秒）
			const auto sleep_time = std::chrono::milliseconds(intervalMsec);

			// スレッド処理のメインループ
			while (true) {
				std::unique_lock<std::mutex> lk(mtx_);

				if (abort_) {
					logger->trace("TimerWorker aborted");
					break;
				}
				callbackFunction();

				// 指定時間待機する。
				// 中断メソッドが呼ばれたらタイムアウトを抜ける
				cv_.wait_for(lk, sleep_time, [this] { return abort_; });

			}
			//REL_COMMENT logger->trace("-TimerWorker::run");
		}
	};
}
