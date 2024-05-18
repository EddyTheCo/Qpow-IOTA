#include "pow/qpow.hpp"
#include "encoding/qb1t6.hpp"
#include "hashing/qcurlp81.hpp"
#include <QCryptographicHash>
#include <QIODevice>
#include <QTimer>
#define LOG3 1.098612288668109691395245236922525704647490557822749451734694333
namespace qiota
{

namespace qpow
{
using namespace qencoding::qb1t6;
using namespace qhashing;
class WorkerThread : public QObject
{
    Q_OBJECT
  public:
    WorkerThread(const QByteArray &curl_in_m, const quint8 &target_zeros_m, const quint64 &step_,
                 QObject *parent = nullptr)
        : QObject(parent), step(step_), stop(false), curl_in_(curl_in_m.data(), curl_in_m.size()),
          target_zeros_(target_zeros_m){};

    void doWork(const quint64 &start_block);
    void send_stop(void);
  signals:
    void found_nonce(quint64 s);

  private:
#ifdef USE_THREADS
    std::mutex mutex;
#endif

    bool stop;
    const quint64 step;
    const quint8 target_zeros_;
    const QByteArray curl_in_;
};
void WorkerThread::send_stop(void)
{
#ifdef USE_THREADS
    mutex.lock();
    stop = true;
    mutex.unlock();
#else
    stop = true;
#endif
}
void WorkerThread::doWork(const quint64 &start_block)
{
    const quint64 start_nonce = start_block * step;
    const auto treZeros = QByteArray(3, 0);

    for (quint64 nonce = start_nonce; nonce < start_nonce + step; nonce++)
    {
        QByteArray curl_input(curl_in_.data(), curl_in_.size());
        QByteArray nonceA;
        QDataStream stream(&nonceA, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << nonce;
        curl_input.append(get_Trits_from_Bytes(nonceA));

        curl_input.append(treZeros);

        auto curl_hash = curlP(curl_input);

        if (stop)
        {
            return;
        }
        if (curl_hash.get_TrailingZeros() >= target_zeros_)
        {

            if (!stop)
            {
#ifdef USE_THREADS
                mutex.lock();
#endif
                stop = true;
                emit found_nonce(nonce);
#ifdef USE_THREADS
                mutex.unlock();
#endif
            }

            return;
        }
    }
}
/*!
  \class nonceFinder
  \brief The nonceFinder class calculates the nonce in order to fulfill PoW following the
  [TIP](https://github.com/iotaledger/tips/blob/main/tips/TIP-0012/tip-0012.md)

  A nonceFinder calculates a quint64 value (nonce) needed to get certain pow score from a message.
  A more detailed explanation can be found on the
  [TIP](https://github.com/iotaledger/tips/blob/main/tips/TIP-0012/tip-0012.md)

  The nonce finder should use  multithread to calculate the nonce.
  The use of multithread in wasm is experimental(it works in some browsers).
  Even when using multithread the calculation of the nonce is slow.
  This could be related to the curlp81 hash function, a batched version of that function has to be implemented.



*/
/*!
  \fn nonceFinder::nonceFinder()

  Construct a nonceFinder object. One needs to set the Min_PoW_Score_ that by default is equal to 1500.
  */
nonceFinder::nonceFinder()
    : thenonce(0), NThreads(
#ifdef USE_THREADS
                       (std::thread::hardware_concurrency()) ? std::thread::hardware_concurrency() : 16
#else
                       1
#endif
                       ),
      Min_PoW_Score_(1500){

      };

/*!
  \fn void nonceFinder::calculate(const QByteArray& Message)
  Start searching for the nonce needed that together with the Message will give
  a PoW score larger than the Min_PoW_Score_.
  If the nonce is found the nonceFinder will emit nonce_found(quint64 s) signal.
  After 30 secs of calculation if the nonce have not been found the  nonce_not_found() signal will be emitted.
  \param Message is the serialized message, block.
  */
void nonceFinder::calculate(const QByteArray &Message)
{
    const quint64 shift = std::numeric_limits<quint64>::max() / NThreads;
    const quint8 target_zeros = ceil(log(1.0 * (Message.size() + 8) * Min_PoW_Score_) / LOG3);
    const auto pow_digest = QCryptographicHash::hash(Message, QCryptographicHash::Blake2b_256);
    const auto curl_in = get_Trits_from_Bytes(pow_digest);
    auto worker = new WorkerThread(curl_in, target_zeros, shift, this);
    connect(worker, &WorkerThread::found_nonce, this, [=](const quint64 &s) {
        emit nonce_found(s);
        worker->deleteLater();
    });
    QTimer::singleShot(30000, this, [=]() {
        worker->send_stop();
        worker->deleteLater();
        emit nonce_not_found();
    });
#ifdef USE_THREADS
    for (quint64 i = 0; i < NThreads; i++)
    {
        auto var = std::thread(&WorkerThread::doWork, worker, i);
        var.detach();
    }
#else
    worker->doWork(0);
#endif
}

} // namespace qpow
} // namespace qiota
#include "qpow.moc"
