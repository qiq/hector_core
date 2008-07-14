/**
 * POSIX lock wrapping class
 */

#ifndef _LOCK_H_
#define _LOCK_H_

#include <pthread.h>

class Lock {
protected:
	pthread_mutex_t mutex;

public:
	Lock() { pthread_mutex_init(&mutex, NULL); };
	~Lock() { pthread_mutex_destroy(&mutex); };
	void lock() { pthread_mutex_lock(&mutex); };
	void unlock() { pthread_mutex_unlock(&mutex); };
};

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
