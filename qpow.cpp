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

        void WorkerThread::doWork(quint64 target_zeros,  QByteArray curl_in,  QByteArray treZeros, quint64 start_block) {
            const quint64  start_nonce=start_block*step;

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

                if(stop)
                {
                    return;
                }
				if(curl_hash.get_TrailingZeros()>=target_zeros)
				{
                    mutex.lock();
                    stop=true;
					emit found_nonce(nonce);
                    mutex.unlock();
					return;
				}



			}

		}
        nonceFinder::nonceFinder():thenonce(0),NThreads(20),Min_PoW_Score_(1500)
		{
            const quint64 shift=std::numeric_limits < qint64 >::max()/NThreads;
            worker = new WorkerThread(shift);
            connect(worker, &WorkerThread::found_nonce, this, &nonceFinder::nonce_found);
            QObject::connect(this, &nonceFinder::nonce_found, worker, &QObject::deleteLater);

		};

		void nonceFinder::calculate(const QByteArray& Message)
		{

			const quint8 target_zeros=ceil(log(1.0*(Message.size()+8)*Min_PoW_Score_)/LOG3);
			const auto pow_digest=QCryptographicHash::hash(Message, QCryptographicHash::Blake2b_256);
			const auto curl_in=get_Trits_from_Bytes(pow_digest);
			const auto treZeros=QByteArray(3,0);

            const auto  Nthreads=20;
            for(quint64 i=0;i<Nthreads;i++)
            {
                Threads.push_back(std::thread(&WorkerThread::doWork, worker, target_zeros,curl_in,treZeros,i));

            }

		}



	}
}
