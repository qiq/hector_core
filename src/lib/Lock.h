/**
 * POSIX lock wrapping class
 */

#ifndef _LOCK_H_
#define _LOCK_H_

#include <config.h>

#include <pthread.h>

class Lock {
protected:
	pthread_mutex_t *mutex;

public:
	Lock();
	~Lock();
	void lock();
	void unlock();

	pthread_mutex_t *getMutex();
	void setMutex(pthread_mutex_t *mutex, bool free);
};

inline Lock::Lock() {
	mutex = new pthread_mutex_t;
	pthread_mutex_init(mutex, NULL);
}

inline Lock::~Lock() {
	pthread_mutex_destroy(mutex);
	delete mutex;
}

inline void Lock::lock() {
	pthread_mutex_lock(mutex);
}

inline void Lock::unlock() {
	pthread_mutex_unlock(mutex);
}

inline pthread_mutex_t *Lock::getMutex() {
	return mutex;
}

inline void Lock::setMutex(pthread_mutex_t *mutex, bool free) {
	if (this->mutex != mutex) {
		if (free) {
			pthread_mutex_destroy(this->mutex);
			delete this->mutex;
		}
		this->mutex = mutex;
	}
}

#endif
