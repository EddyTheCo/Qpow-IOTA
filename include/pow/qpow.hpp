#pragma once
/**
 * https://github.com/iotaledger/tips/blob/main/tips/TIP-0012/tip-0012.md
 *
 **/

#include<QByteArray>
#include<QDebug>
#include<thread>
#include<mutex>
namespace qiota{

	namespace qpow {

		class WorkerThread :public QObject
		{
			Q_OBJECT
			public:
                WorkerThread(const QByteArray&curl_in_m, const quint8& target_zeros_m,const quint64& step_):
                    step(step_),stop(false),curl_in_(curl_in_m.data(),curl_in_m.size()),target_zeros_(target_zeros_m){};

                void doWork(quint64 start_block);
                void send_stop(void);
signals:
				void found_nonce(const quint64 &s);
			private:
				std::mutex mutex;
				bool stop;
                const quint64 step;
                const quint8 target_zeros_;
                const QByteArray curl_in_;
		};

		class nonceFinder : public QObject
		{
			Q_OBJECT
			public:
                nonceFinder();

                void calculate(const QByteArray &Message);
				void set_Min_Pow_Score(quint32 Min_PoW_Score_m){Min_PoW_Score_=Min_PoW_Score_m;};


signals:
				void nonce_found(const quint64 &s);
                void nonce_not_found(void);

			private:
				quint64 thenonce,NThreads;
				std::vector<std::thread> Threads;
				WorkerThread* worker;
				quint32 Min_PoW_Score_;

		};


	}
}
