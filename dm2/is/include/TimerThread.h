#ifndef TIMER_THREAD_H
#define TIMER_THREAD_H

#include <pthread.h>
#include <functional>

/**
 * 一定間隔でデータ取得要求を実施するクラス
 * 主に時間Window指定による定期実行に用いる
 * TimerWorkerクラスの改善版
 *
 * @author	Shinichi Kusayama
 * @date	2024/06/03
 */
class TimerThread {
public:
    TimerThread(std::function<void()> func, int start_fix_ms_, int interval_ms_);
    ~TimerThread();

    void start();
    void run();

private:
    pthread_mutex_t timer_create_mtx;
    pthread_cond_t timer_create_cond;
    timer_t timer_id;
    pthread_t thread;
    int start_fix_ms;  // タイマーを開始する際の調整時間（ミリ秒の下2桁）
    int interval_ms;   // タイマーのインターバル時間（ミリ秒）

    std::function<void()> callback_func;

    static void timer_notify(union sigval arg);
    static void* timer_notifed_thread(void* arg);
    static void* thread_wrapper(void* arg);

    void setup_timer();
    void create_thread();
};

#endif // TIMER_THREAD_H
