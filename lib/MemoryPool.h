/**
 * Class for efficient allocation of many elements (but only complete free possible).
 */

#ifndef _LIB_MEMORY_POOL_H_
#define _LIB_MEMORY_POOL_H_

#include <config.h>

#include <vector>
#include "common.h"
#include "PlainLock.h"

template<class T, bool useLock>
class MemoryPool {
public:
	MemoryPool(int chunkSize);
	~MemoryPool();
	T *Alloc();
	void Clear();
private:
	PlainLock lock;
	int chunkSize;
	std::vector<T*> chunks;
	int used;
};

template <class T, bool useLock>
MemoryPool<T, useLock>::MemoryPool(int chunkSize) {
	this->chunkSize = chunkSize;
	this->used = chunkSize;
}

template <class T, bool useLock>
MemoryPool<T, useLock>::~MemoryPool() {
	if (useLock)
		lock.Lock();
	for (typename std::vector<T*>::iterator iter = chunks.begin(); iter != chunks.end(); ++iter)
		delete[] (*iter);
	if (useLock)
		lock.Unlock();
}

template <class T, bool useLock>
T *MemoryPool<T, useLock>::Alloc() {
	if (useLock)
		lock.Lock();
	if (used >= chunkSize) {
		T *chunk = new T[chunkSize]();
		chunks.push_back(chunk);
		used = 0;
	}
	int index = used++;
	if (useLock)
		lock.Unlock();
	return &chunks.back()[index];
}

template <class T, bool useLock>
void MemoryPool<T, useLock>::Clear() {
	if (useLock)
		lock.Lock();
	for (typename std::vector<T*>::iterator iter = chunks.begin(); iter != chunks.end(); ++iter)
		delete (*iter);
	chunks.clear();
	if (useLock)
		lock.Unlock();
}

#endif
