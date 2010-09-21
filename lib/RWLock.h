/**
 * POSIX lock wrapping class
 */

#ifndef _LIB_RWLOCK_H_
#define _LIB_RWLOCK_H_

#include <config.h>

#include <pthread.h>

class RWLock {
protected:
	pthread_rwlock_t *rwlock;

public:
	RWLock();
	~RWLock();
	void LockRead();
	void LockWrite();
	void Unlock();
};

inline RWLock::RWLock() {
	rwlock = new pthread_rwlock_t;
	pthread_rwlock_init(rwlock, NULL);
}

inline RWLock::~RWLock() {
	pthread_rwlock_destroy(rwlock);
	delete rwlock;
}

inline void RWLock::LockRead() {
	pthread_rwlock_rdlock(rwlock);
}

inline void RWLock::LockWrite() {
	pthread_rwlock_wrlock(rwlock);
}

inline void RWLock::Unlock() {
	pthread_rwlock_unlock(rwlock);
}

#endif
