/**
 * POSIX lock wrapping class
 */

#ifndef _LIB_COND_LOCK_H_
#define _LIB_COND_LOCK_H_

#include <config.h>

#include <pthread.h>
#include "PlainLock.h"

class CondLock: public PlainLock {
	pthread_cond_t *condSend;
	pthread_cond_t *condRecv;
public:
	CondLock();
	~CondLock();

	void WaitSend() { pthread_cond_wait(condSend, mutex); }
	void SignalSend() { pthread_cond_broadcast(condSend); }
	void WaitRecv() { pthread_cond_wait(condRecv, mutex); }
	void SignalRecv() { pthread_cond_broadcast(condRecv); }
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

#endif
