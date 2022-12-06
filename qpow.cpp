#include"pow/qpow.hpp"
#include"hashing/qcurlp81.hpp"
#include"encoding/qb1t6.hpp"

#include<QIODevice>
#include <QCryptographicHash>
#define LOG3 1.098612288668109691395245236922525704647490557822749451734694333
namespace qiota{

	namespace qpow {
using namespace qencoding::qb1t6;
using namespace qhashing;

		void WorkerThread::doWork(const quint64 target_zeros, const QByteArray curl_in, const QByteArray treZeros) {


			for(quint64 nonce=start_nonce;nonce<start_nonce+step;nonce++)
			{
				QByteArray curl_input(curl_in);
				QByteArray nonceA;
				QDataStream stream(&nonceA, QIODevice::WriteOnly);
				stream.setByteOrder(QDataStream::LittleEndian);
				stream<<nonce;
				curl_input.append(get_Trits_from_Bytes(nonceA));

				curl_input.append(treZeros);

				auto curl_hash=curlP(curl_input);

				if(curl_hash.get_TrailingZeros()>=target_zeros)
				{

					emit found_nonce(nonce);
					return;
				}

				if(stop)
				{
					return;
				}

			}
		}
		nonceFinder::nonceFinder():thenonce(0), Threads(std::vector<QThread>(QThread::idealThreadCount())),Min_PoW_Score_(1500)
		{

			const auto  Nthreads=Threads.size();

			const quint64 shift=std::numeric_limits < qint64 >::max()/Nthreads;

			for(quint64 i=0;i<Nthreads;i++)
			{

				auto worker = new WorkerThread(i*shift,shift,i);
				worker->moveToThread(&Threads[i]);
				connect(&Threads[i], &QThread::finished, worker, &QObject::deleteLater);
				connect(this, &nonceFinder::operate, worker, &WorkerThread::doWork);
				connect(worker, &WorkerThread::found_nonce, this, &nonceFinder::handleResults);
				Threads[i].start();
				Workers.push_back(worker);

			}
			QObject::connect(this, &nonceFinder::nonce_found, this, &QObject::deleteLater);


		};

		void nonceFinder::calculate(const QByteArray& Message)
		{

			const quint8 target_zeros=ceil(log(1.0*(Message.size()+8)*Min_PoW_Score_)/LOG3);
			const auto pow_digest=QCryptographicHash::hash(Message, QCryptographicHash::Blake2b_256);
			const auto curl_in=get_Trits_from_Bytes(pow_digest);
			const auto treZeros=QByteArray(3,0);
			emit operate(target_zeros,curl_in,treZeros);

		}

		void nonceFinder::handleResults(const quint64& nonce)
		{

			if(!thenonce)
			{
				for(auto i=0;i<Threads.size();i++)
				{
					Workers[i]->send_stop();
					Threads[i].quit();
					Threads[i].wait();
				}
				emit nonce_found(nonce);
			}
			thenonce=nonce;
		}

	}
}
