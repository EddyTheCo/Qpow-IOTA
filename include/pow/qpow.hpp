#pragma once
/**
 * https://github.com/iotaledger/tips/blob/main/tips/TIP-0012/tip-0012.md
 *
 **/

#include<QByteArray>
#include<QDebug>
#include <QThread>
namespace qiota{

	namespace qpow {

		class WorkerThread : public QObject
		{
			Q_OBJECT
			public:
				WorkerThread(const quint64& start_nonce_,
						const quint64& step_,int id_):id(id_),
				start_nonce(start_nonce_),step(step_),stop(false){};
				public slots:
					void send_stop(void){stop=true;};
				void doWork(const quint64 target_zeros_, const QByteArray curl_in_, const QByteArray treZeros_);
signals:
				void found_nonce(const quint64 &s);
			private:
				int id;
				bool stop;
				const quint64 start_nonce,step;
		};

		class nonceFinder : public QObject
		{
			Q_OBJECT
			public:
				nonceFinder();

				void calculate(const QByteArray &Message, const quint32 Min_PoW_Score);
				void handleResults(const quint64& nonce);

signals:
				void nonce_found(const quint64 &s);
				void operate(const quint64& target_zeros_, const QByteArray& curl_in_, const QByteArray& treZeros_);
			private:
				quint64 thenonce;
				std::vector<QThread> Threads;
				std::vector<WorkerThread*> Workers;

		};

		quint64 get_nonce(const QByteArray &Message/*sin nonce*/, const quint32& Min_PoW_Score);

	}
}
