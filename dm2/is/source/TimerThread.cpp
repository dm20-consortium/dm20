#include "TimerThread.h"
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/timerfd.h>
#include <time.h>

volatile sig_atomic_t stop_flag = 0;
/**
 * コンストラクタ
 *
 * @author	Shinichi Kusayama
 * @date	2024/06/03
 *
 * @param	func	    定周期で実行させたい関数
 * @param	interval_ns	実行間隔（ナノ秒）
 */
TimerThread::TimerThread(std::function<void()> func, int start_fix_ms_, int interval_ms_) 
    : thread(0) {
    this->callback_func = func;
    this->start_fix_ms = start_fix_ms_;
    this->interval_ms = interval_ms_;
    pthread_mutex_init(&timer_create_mtx, nullptr);
    pthread_cond_init(&timer_create_cond, nullptr);
}

/**
 * デストラクタ
 *
 * @author	Shinichi Kusayama
 * @date	2024/06/03
 *
 */
TimerThread::~TimerThread() {
    pthread_mutex_destroy(&timer_create_mtx);
    pthread_cond_destroy(&timer_create_cond);
}

/**
 * 初期化処理
 *
 * @author	Shinichi Kusayama
 * @date	2024/06/03
 *
 */
void TimerThread::start() {
    setup_timer();
    create_thread();
}

/**
 * 実行処理
 *
 * @author	Shinichi Kusayama
 * @date	2024/06/03
 *
 */
void TimerThread::run() {
    start();
}

/**
 * 指定されたタイマーの間隔で通知する
 *
 * @author	Shinichi Kusayama
 * @date	2024/06/03
 *
 * @param	arg	    sigevent構造体 (非同期ルーチンからの通知用の構造体)
 */
void TimerThread::timer_notify(union sigval arg) {
    TimerThread* self = static_cast<TimerThread*>(arg.sival_ptr);
    //std::cout << "Tid = " << pthread_self() << std::endl;
    // pthread_cond_waitで待っているスレッドに対して、通知
    pthread_cond_signal(&self->timer_create_cond);
}

/**
 * 通知を受けて、コールバック関数を起動する
 *
 * @author	Shinichi Kusayama
 * @date	2024/06/03
 *
 * @param	arg	    sigevent構造体 (非同期ルーチンからの通知用の構造体)
 */
void* TimerThread::timer_notifed_thread(void* arg) {
    TimerThread* self = static_cast<TimerThread*>(arg);
    int ret;
    //struct timespec curTime, lastTime;
    //clock_gettime(CLOCK_REALTIME, &lastTime);

    pthread_mutex_lock(&self->timer_create_mtx);
    while (true) {
        // 通知を待つ
        ret = pthread_cond_wait(&self->timer_create_cond, &self->timer_create_mtx);
        if (ret == 0) {
            /* 以下、単体テスト用
            clock_gettime(CLOCK_REALTIME, &curTime);
            if (curTime.tv_nsec < lastTime.tv_nsec) {
                std::cout << "Interval = " << curTime.tv_sec - lastTime.tv_sec - 1 << "." << curTime.tv_nsec + 1000000000 - lastTime.tv_nsec << std::endl;
            } else {
                std::cout << "Interval = " << curTime.tv_sec - lastTime.tv_sec << "." << curTime.tv_nsec - lastTime.tv_nsec << std::endl;
            }
            lastTime = curTime;
            */
            // コールバック関数を起動
            self->callback_func();
        }
        if (stop_flag) {
            break;
        }
    }
    pthread_mutex_unlock(&self->timer_create_mtx);
    return nullptr;
}

/**
 * void型のコールバック関数をpthread_createの引数に合わせて、void*型に変換するラッパー関数
 *
 * @author	Shinichi Kusayama
 * @date	2024/06/03
 *
 * @param	arg	    TimerThreadインスタンス
 */
void* TimerThread::thread_wrapper(void* arg) {
    TimerThread* self = static_cast<TimerThread*>(arg);
    return self->timer_notifed_thread(self);
}

/**
 * タイマー設定処理
 *
 * @author	Shinichi Kusayama
 * @date	2024/06/03
 *
 */
void TimerThread::setup_timer() {
    struct itimerspec ts;
    struct sigevent se;

    // sigevent構造体に、通知用メソッドtimer_notifyを登録
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = this;
    se.sigev_notify_function = timer_notify;
    se.sigev_notify_attributes = nullptr;

    // 初回起動とその後の実行間隔を登録。秒とナノ秒で単位を分ける
    int interval_sec = int(interval_ms / 1000);
    int interval_nanosec = (interval_ms % 1000) * 1000 * 1000;
    ts.it_value.tv_sec = interval_sec;
    ts.it_value.tv_nsec = interval_nanosec;
    ts.it_interval.tv_sec = interval_sec;
    ts.it_interval.tv_nsec = interval_nanosec;

    // START_FIX指定あり？
    if (start_fix_ms > 0) {
        struct timespec curTime;
        clock_gettime(CLOCK_REALTIME, &curTime);
        // ナノ秒の下8桁を抽出
        long nanoseconds_8digit = curTime.tv_nsec % (100 * 1000 * 1000);
        int start_fix_ns = start_fix_ms * 1000 * 1000;
        int start_ns = 0;
        if (nanoseconds_8digit <= start_fix_ns) {
            // 例）FIX時刻が、80ミリ秒で、現在時刻のミリ秒の下2桁が79ミリ秒であれば、1ミリ秒待った後に開始する。下記はそれをナノ秒単位で求めている。
            start_ns = start_fix_ns - nanoseconds_8digit;
        } else {
            // 例）FIX時刻が、80ミリ秒で、現在時刻のミリ秒の下2桁が81ミリ秒であれば、81 + 100 - 80 = 101ミリ秒待った後に開始する。下記はそれをナノ秒単位で求めている。
            start_ns = start_fix_ns + 100 * 1000 * 1000 - nanoseconds_8digit;
        }
        ts.it_value.tv_sec = 0;
        ts.it_value.tv_nsec = start_ns;
    }

    int status = timer_create(CLOCK_REALTIME, &se, &timer_id);
    if (status == -1) {
        perror("Fail to create timer");
        exit(1);
    }

    status = timer_settime(timer_id, 0, &ts, 0);
    if (status == -1) {
        std::cerr << "Fail to set timer:" << ts.it_value.tv_sec << "[sec]," << ts.it_value.tv_nsec << "[nsec]" << std::endl;
        exit(1);
    }
}

/**
 * スレッド起動処理
 *
 * @author	Shinichi Kusayama
 * @date	2024/06/03
 *
 */
void TimerThread::create_thread() {
    int ret = pthread_create(&thread, nullptr, thread_wrapper, this);
    if (ret != 0) {
        perror("Cannot create thread");
        exit(1);
    }
    // 呼び出し先のクラスに処理を返すため、デタッチ
    pthread_detach(thread);
}

