#pragma once
/**
 * https://github.com/iotaledger/tips/blob/main/tips/TIP-0012/tip-0012.md
 *
 **/

#include<QByteArray>
#include<QObject>
#ifdef USE_THREADS
#include<thread>
#include<mutex>
#endif
#include <QtCore/QtGlobal>

#if defined(SHAREDLIB_LIBRARY)
#  define SHAREDLIB_EXPORT Q_DECL_EXPORT
#else
#  define SHAREDLIB_EXPORT Q_DECL_IMPORT
#endif

namespace qiota{

	namespace qpow {


		class QEXPORT_IMPORT nonceFinder : public QObject
		{
			Q_OBJECT
			public:
				nonceFinder();

				void calculate(const QByteArray &Message);
				void set_Min_Pow_Score(quint32 Min_PoW_Score_m){Min_PoW_Score_=Min_PoW_Score_m;};


signals:
				void nonce_found(quint64 s);
				void nonce_not_found(void);

			private:
				quint64 thenonce,NThreads;
				quint32 Min_PoW_Score_;

		};


	}
}
