/**
 * Class for efficient allocation of many elements (but only complete free possible).
 */

#ifndef _LIB_MEMORY_POOL_H_
#define _LIB_MEMORY_POOL_H_

#include <config.h>

#include <vector>
#include "common.h"
#include "PlainLock.h"

template<class T>
class MemoryPool {
public:
	MemoryPool(int chunkSize);
	~MemoryPool();
	T *alloc();
	void clear();
private:
	PlainLock lock;
	int chunkSize;
	std::vector<T*> chunks;
	int used;
};

template <class T>
MemoryPool<T>::MemoryPool(int chunkSize) {
	this->chunkSize = chunkSize;
	this->used = 0;
}

template <class T>
MemoryPool<T>::~MemoryPool() {
	lock.Lock();
	for (typename std::vector<T*>::iterator iter = chunks.begin(); iter != chunks.end(); ++iter)
		delete (*iter);
	lock.Unlock();
}

template <class T>
T *MemoryPool<T>::alloc() {
	lock.Lock();
	if (used >= chunkSize) {
		T *chunk = new T[chunkSize];
		chunks.push_back(chunk);
		used = 0;
	}
	int index = used++;
	lock.Unlock();
	return &chunks.back()[index];
}

template <class T>
void MemoryPool<T>::clear() {
	lock.Lock();
	for (typename std::vector<T*>::iterator iter = chunks.begin(); iter != chunks.end(); ++iter)
		delete (*iter);
	chunks.clear();
	lock.Unlock();
}

#endif
