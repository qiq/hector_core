/**
 * POSIX lock wrapping class
 */

#ifndef _LIB_COND_LOCK_H_
#define _LIB_COND_LOCK_H_

#include <config.h>

#include <pthread.h>
#include <sys/time.h>
#include "PlainLock.h"

class CondLock: public PlainLock {
	pthread_cond_t *condSend;
	pthread_cond_t *condRecv;
public:
	CondLock();
	~CondLock();

	bool WaitSend(struct timeval *timeout);
	void SignalSend();
	bool WaitRecv(struct timeval *timeout);
	void SignalRecv();
};

inline CondLock::CondLock() {
	condSend = new pthread_cond_t;
	condRecv = new pthread_cond_t;
	pthread_cond_init(condSend, NULL);
	pthread_cond_init(condRecv, NULL);
}

inline CondLock::~CondLock() {
	pthread_cond_destroy(condSend);
	pthread_cond_destroy(condRecv);
	delete condSend;
	delete condRecv;
}

// returns false if error/timeout occured
inline bool CondLock::WaitSend(struct timeval *timeout) {
	if (!timeout || timeout->tv_sec == 0) {
		return (pthread_cond_wait(condSend, mutex) == 0);
	} else {
		struct timespec ts;
		ts.tv_sec = timeout->tv_sec;
		ts.tv_nsec = timeout->tv_usec * 1000;
		return (pthread_cond_timedwait(condSend, mutex, &ts) == 0);
	}
}

inline void CondLock::SignalSend() {
	pthread_cond_broadcast(condSend);
}

// returns false if error/timeout occured
inline bool CondLock::WaitRecv(struct timeval *timeout) {
	if (!timeout || timeout->tv_sec == 0) {
		return (pthread_cond_wait(condRecv, mutex) == 0);
	} else {
		struct timespec ts;
		ts.tv_sec = timeout->tv_sec;
		ts.tv_nsec = timeout->tv_usec * 1000;
		return (pthread_cond_timedwait(condRecv, mutex, &ts) == 0);
	}
}

inline void CondLock::SignalRecv() {
	pthread_cond_broadcast(condRecv);
}
#endif
