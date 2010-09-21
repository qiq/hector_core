/**
 * POSIX lock wrapping class
 */

#ifndef _LIB_PLAIN_LOCK_H_
#define _LIB_PLAIN_LOCK_H_

#include <config.h>

#include <pthread.h>

class PlainLock {
protected:
	pthread_mutex_t *mutex;

public:
	PlainLock();
	~PlainLock();
	void Lock();
	void Unlock();
};

inline PlainLock::PlainLock() {
	mutex = new pthread_mutex_t;
	pthread_mutex_init(mutex, NULL);
}

inline PlainLock::~PlainLock() {
	pthread_mutex_destroy(mutex);
	delete mutex;
}

inline void PlainLock::Lock() {
	pthread_mutex_lock(mutex);
}

inline void PlainLock::Unlock() {
	pthread_mutex_unlock(mutex);
}

#endif
