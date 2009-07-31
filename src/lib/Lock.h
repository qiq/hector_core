/**
 * POSIX lock wrapping class
 */

#ifndef _LOCK_H_
#define _LOCK_H_

#include <config.h>

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

#endif
