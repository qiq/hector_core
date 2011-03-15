/**
 * POSIX lock wrapping class
 */

#ifndef _LIB_PLAIN_LOCK_H_
#define _LIB_PLAIN_LOCK_H_

#include <config.h>

#include <pthread.h>

class PlainLock {
public:
	PlainLock(bool recursive = false);
	~PlainLock();
	void Lock();
	void Unlock();

protected:
	pthread_mutex_t *mutex;
};

inline PlainLock::PlainLock(bool recursive) {
	mutex = new pthread_mutex_t;
	if (recursive) {
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(mutex, &attr);
		pthread_mutexattr_destroy(&attr);
	} else {
		pthread_mutex_init(mutex, NULL);
	}
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
