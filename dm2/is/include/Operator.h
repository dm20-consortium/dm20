#ifndef OPERATOR_H
#define OPERATOR_H

#include <queue>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <log4cxx/logger.h>
#include <pqxx/pqxx>
#include <iomanip>

#include <is/TupleSet.h>
#include <is/ArrivalTupleListener.h>
#include "ArrivalTupleNotifier.h"
#include <is/DmUtil.h>
#include "Settings.h"

using namespace log4cxx;

namespace IS {

	/**
	* 時間単位の処理タプル数を計測するための構造体
	*
	* @author	Nagoya University
	* @date	2019/10/30
	*/
	struct performInfo {
		long time;
		int num;
	};

    /**
     * データ操作を扱う基底クラス
     *
     * @author	Nagoya University
     * @date	2018/03/13
     */

    class Operator : public ArrivalTupleListener, public ArrivalTupleNotifier
    {
	protected:
		string type = "OPERATOR";
		string nodeId = "";
		unsigned int mngId;
		string argument = "";
		std::deque<TupleSet> buffer;
		pthread_mutex_t bufMtx;
		bool exitReady = false;

		// 統計情報
		long processNum = 0;
		long processNumSum = 0;
		double getTimeAVG = 0;
		double getTimeMax = 0;
		double getTimeMin = 0;
		double processTimeAVG = 0;
		double processTimeMax = 0;
		double processTimeMin = 0;
		double fireTimeAVG = 0;
		double fireTimeMax = 0;
		double fireTimeMin = 0;
		double allTimeAVG = 0;
		double allTimeMax = 0;
		double allTimeMin = 0;
		double tupleNumAVG = 0;
		long tupleNumMax = 0;
		long tupleNumMin = 0;
		int inputSum = 0;
		int outputSum = 0;

    private:
		LoggerPtr logger = Logger::getLogger("Operator");
		bool start_flag = false;
        bool exit_flag = false;

        std::condition_variable cond;
        std::mutex mtx;
        bool data_ready = false;
		bool operationReady = false;
		long startTime = 0;
		pthread_mutex_t inputMtx;
		pthread_mutex_t outputMtx;
		deque<performInfo> inputList;
		deque<performInfo> outputList;
		std::vector<TupleSet> ts;

		IS::Settings &settings = IS::Settings::get_instance();
		StringUtil stringUtil;
		string pid;
		
		void setPerformInfo(const long time, const vector<TupleSet> &ts, deque<performInfo> &list, int &sum);
		void calcTime(const double &startTime, const double &endTime, double &time, double &timeAVG, double &timeMin, double &timeMax);
		void calcTupleNum(const long &tupleNum, double &tupleNumAVG, long &tupleNumMin, long &tupleNumMax);
		int getMinute(string hhmm);
		
		// for DEBUG START
		void appendValue(const Tuple &tuple, string &result);
		void appendAttribute(TupleSet& ts, string &result);
		void appendAttributeType(TupleSet& ts, string &result);
		const int DisplayChar = 12;					// 表示文字数
		const int printBufSize = 200;
		string printFormat = "%" + std::to_string(DisplayChar) + "s";		// フォーマット
		// for DEBUG END
    public:
        Operator();
        virtual ~Operator();

		// オペレータを実行するために必要なタプルセット数を定義
		// 各オペレータに応じてコンストラクタにて数を更新すること
		unsigned int expect_data_size = 1;

		// 自身を起こした呼び出し元オブジェクトを保持
		// 呼び出し元以外のデータが必要となった時に
		// 呼び出し元以外のINPUTすべきオペレータにデータ要求を投げる際に使用
		const ArrivalTupleListener* callerObj = NULL;

        // Inherit ArrivalTupleListener
        // tupleを登録する
        virtual void receiveTupleSet(TupleSet &ts);
		virtual void requestData(long referenceTime);

        // スレッド処理
        virtual void start();
        virtual void exit();
        virtual void execute();
		virtual void notify_one();

		/*
			Operatorが処理待機待ち状態になった際にtrueを返却する
		*/
		bool isReady();
		void setNodeId(string nodeId);

		/*
			処理データが到着し、処理中の場合にtrueを返却する
		*/
		bool isDataReady();

		string getType() { return type; }
		int getBufferSize() { return buffer.size(); }
		double getProcTimeAVG() { return processTimeAVG; }		
		int getPerformInfoNum(const string type);

        // 処理
        virtual bool process(std::vector<TupleSet> &ts);
        virtual void process_close();
        virtual long getTupleSet(std::vector<TupleSet> &ts);
		
		// for debug
		void printInputInfo(TupleSet& ts, const string &parameter);
		void printInputInfos(vector<IS::TupleSet>& tsList, const string &parameter);
		void printOutputInfo(TupleSet& ts);
		void cOutputInfo(TupleSet& ts);
    };
}

#endif // OPERATOR_H
