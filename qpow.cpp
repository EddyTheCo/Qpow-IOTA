#include"pow/qpow.hpp"
#include"hashing/qcurlp81.hpp"
#include"encoding/qb1t6.hpp"
#include<QDebug>
#include<QIODevice>
#include <QCryptographicHash>
#include<QTimer>
#define LOG3 1.098612288668109691395245236922525704647490557822749451734694333
namespace qiota{

namespace qpow {
using namespace qencoding::qb1t6;
using namespace qhashing;
void WorkerThread::send_stop(void)
{
#ifdef USE_THREADS
    mutex.lock();
    stop=true;
    mutex.unlock(); std::vector<std::thread> Threads;
#else
    stop=true;
#endif

}
void WorkerThread::doWork(quint64 start_block) {
    const quint64  start_nonce=start_block*step;
    const auto treZeros=QByteArray(3,0);

    for(quint64 nonce=start_nonce;nonce<start_nonce+step;nonce++)
    {
        QByteArray curl_input(curl_in_.data(),curl_in_.size());
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
        if(curl_hash.get_TrailingZeros()>=target_zeros_)
        {

            if(!stop)
            {
#ifdef USE_THREADS
    mutex.lock();
#endif

                stop=true;
                emit found_nonce(nonce);
            }
#ifdef USE_THREADS
    mutex.unlock();
#endif

            return;
        }



    }

}
nonceFinder::nonceFinder():thenonce(0),NThreads(
#ifdef USE_THREADS
8
#else
1
#endif
),
Min_PoW_Score_(1500)
{



};

void nonceFinder::calculate(const QByteArray& Message)
{
    const quint64 shift=std::numeric_limits < quint64 >::max()/NThreads;
    const quint8 target_zeros=ceil(log(1.0*(Message.size()+8)*Min_PoW_Score_)/LOG3);
    qDebug()<<"target_zeros:"<<target_zeros;
    const auto pow_digest=QCryptographicHash::hash(Message, QCryptographicHash::Blake2b_256);
    const auto curl_in=get_Trits_from_Bytes(pow_digest);

    worker = new WorkerThread(curl_in,target_zeros,shift);
    connect(worker, &WorkerThread::found_nonce, this, [=](const quint64 &s){emit nonce_found(s);worker->deleteLater();});
    QTimer::singleShot(30000, this, [this](){
        worker->send_stop();
        worker->deleteLater();
        emit nonce_not_found();
    });
#ifdef USE_THREADS
    for(quint64 i=0;i<NThreads;i++)
    {
        Threads.push_back(std::thread(&WorkerThread::doWork, worker, i));
        Threads.back().detach();
    }
#else
    worker->doWork(0);
#endif

}

}
}
