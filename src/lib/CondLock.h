/**
 * POSIX lock wrapping class
 */

#ifndef _COND_LOCK_H_
#define _COND_LOCK_H_

#include <config.h>

#include <pthread.h>
#include "Lock.h"

class CondLock: public Lock {
	pthread_cond_t condSend;
	pthread_cond_t condRecv;
public:
	CondLock() {
		pthread_cond_init(&condSend, NULL);
		pthread_cond_init(&condRecv, NULL);
	}
	~CondLock() {
		pthread_cond_destroy(&condSend);
		pthread_cond_destroy(&condRecv);
	}

	void waitSend() { pthread_cond_wait(&condSend, &mutex); }
	void signalSend() { pthread_cond_signal(&condSend); }
	void waitRecv() { pthread_cond_wait(&condRecv, &mutex); }
	void signalRecv() { pthread_cond_signal(&condRecv); }
};

#endif
