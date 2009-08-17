/**
 * Class representing a queue of items.
 *
 * Queue is synchronized, when full, writers would block, if empty, readers
 * would block.
 */

#ifndef _SYNCQUEUE_H_
#define _SYNCQUEUE_H_

#include <config.h>

#include <assert.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <deque>
#include "common.h"
#include "CondLock.h"

using namespace std;

template<class T>
class SyncQueue {
	// queue object, all attributes are guarded by queueLock
	CondLock queueLock;
	int maxItems;
	int maxSize;
	int queueSize;
	deque<T*> *queue;
	int waitingReaders;
	int waitingWriters;
	bool cancel;

	bool processed;	// for PriorityQueue
public:
	SyncQueue(int maxItems, int maxSize);
	~SyncQueue();
	void cancelAll();
	void clearCancel();
	void pause();
	void resume();

	bool isSpace(T *r);
	bool putItem(T *r, bool sleep);
	int putItems(T **r, int size, bool sleep);
	bool isReady();
	T *getItem(bool sleep);
	int getItems(T **r, int size, bool sleep);

	int getCurrentSize();
	int getCurrentItems();
	int getMaxSize();
	int getMaxItems();

	// dirty methods needed by PriorityQueue and FilterQueue, they are
	// called with lock held
	CondLock *getLock();
	bool isReadyRaw();
	bool isSpaceRaw(T *r);
	T *getItemRaw(bool signal);
	bool putItemRaw(T* r, bool signal);
	bool waitForSpaceRaw(T *r);
	int getCurrentSizeRaw();
	int getCurrentItemsRaw();
	int getMaxSizeRaw();
	int getMaxItemsRaw();
	void setProcessed();
	bool getProcessed();
};

template <class T>
SyncQueue<T>::SyncQueue(int maxItems, int maxSize) {
	this->maxItems = maxItems;
	this->maxSize = maxSize;
	queueSize = 0;
	queue = new deque<T*>();
	waitingReaders = 0;
	waitingWriters = 0;
	cancel = false;

	processed = false;
}

template <class T>
SyncQueue<T>::~SyncQueue() {
	cancelAll();
	for (typename deque<T*>::iterator iter = queue->begin(); iter != queue->end(); ++iter) {
		delete (*iter);
	}
	delete queue;
}

template <class T>
void SyncQueue<T>::cancelAll() {
	queueLock.lock();
	cancel = true;
	while (waitingReaders > 0) {
		queueLock.signalRecv();
		queueLock.unlock();
		sched_yield();
		queueLock.lock();
	}
	while (waitingWriters > 0) {
		queueLock.signalSend();
		queueLock.unlock();
		sched_yield();
		queueLock.lock();
	}
	for (unsigned i = 0; i < queue->size(); i++)
		delete (*queue)[i];
	queue->clear();
	queueLock.unlock();
}

template <class T>
void SyncQueue<T>::clearCancel() {
	queueLock.lock();
	cancel = false;
	queueLock.unlock();
}

template <class T>
void SyncQueue<T>::pause() {
	queueLock.lock();
}

template <class T>
void SyncQueue<T>::resume() {
	queueLock.lock();
}

template <class T>
bool SyncQueue<T>::isSpace(T *r) {
	bool result = false;
	queueLock.lock();
	if ((int)queue->size() < maxItems && queueSize+r->getSize() <= maxSize)
		result = true;
	queueLock.unlock();
	return result;
}

template <class T>
bool SyncQueue<T>::putItem(T *r, bool sleep) {
	queueLock.lock();
	if (cancel) {
		queueLock.unlock();
		return false;
	}
	int itemSize = r->getSize();
	while ((maxItems > 0 && (int)queue->size() == maxItems) || (maxSize > 0 && queueSize+itemSize > maxSize)) {
		if (!sleep) {
			queueLock.unlock();
			return false;
		}
		waitingWriters++;
		queueLock.waitSend();
		waitingWriters--;
		if (cancel) {
			queueLock.unlock();
			return false;
		}
	}
	queue->push_back(r);
	queueSize += r->getSize();
	queueLock.signalRecv();
	queueLock.unlock();
	return true;
}

template <class T>
int SyncQueue<T>::putItems(T **r, int size, bool sleep) {
	queueLock.lock();
	if (cancel) {
		queueLock.unlock();
		return 0;
	}
	int itemSize = r[0]->getSize();
	while ((maxItems > 0 && (int)queue->size() == maxItems) || (maxSize > 0 && queueSize+itemSize > maxSize))  {
		if (!sleep) {
			queueLock.unlock();
			return 0;
		}
		waitingWriters++;
		queueLock.waitSend();
		waitingWriters--;
		if (cancel) {
			queueLock.unlock();
			return 0;
		}
	}
	int max = maxItems > 0 ? (maxItems - (int)queue->size()) : 0;
	if (max == 0 || max > size)
		max = size;

	int i;	
	for (i = 0; i < max && queueSize + itemSize <= maxSize; i++) {
		queue->push_back(r[i]);
		queueSize += itemSize;
		itemSize = r[i]->getSize();
	}
	queueLock.signalRecv();
	queueLock.unlock();
	return i;
}

template <class T>
bool SyncQueue<T>::isReady() {
	bool result = false;
	queueLock.lock();
	if (queue->size() > 0)
		result = true;
	queueLock.unlock();
	return result;
}

template <class T>
T *SyncQueue<T>::getItem(bool sleep) {
	queueLock.lock();
	if (cancel) {
		queueLock.unlock();
		return NULL;
	}
	while (queue->size() == 0) {
		if (!sleep) {
			queueLock.unlock();
			return NULL;
		}
		waitingReaders++;
		queueLock.waitRecv();
		waitingReaders--;
		if (cancel) {
			queueLock.unlock();
			return NULL;
		}
	}
	T *r = (*queue)[0];
	queue->pop_front();
	queueSize -= r->getSize();
	queueLock.signalSend();
	queueLock.unlock();

	return r;
}

template <class T>
int SyncQueue<T>::getItems(T **r, int size, bool sleep) {
	queueLock.lock();
	if (cancel) {
		queueLock.unlock();
		return 0;
	}
	while (queue->size() == 0) {
		if (!sleep) {
			queueLock.unlock();
			return 0;
		}
		waitingReaders++;
		queueLock.waitRecv();
		waitingReaders--;
		if (cancel) {
			queueLock.unlock();
			return 0;
		}
	}
	int i = 0;
	for (i = 0; i < size && (int)queue->size() > 0; i++) {
		r[i] = (*queue)[0];
		queue->pop_front();
		queueSize -= r[i]->getSize();
	}
	queueLock.signalSend();
	queueLock.unlock();

	return i;
}

template <class T>
int SyncQueue<T>::getCurrentSize() {
	int result;
	queueLock.lock();
	result = queueSize;
	queueLock.unlock();
	return result;
}

template <class T>
int SyncQueue<T>::getCurrentItems() {
	int result;
	queueLock.lock();
	result = queue->size();
	queueLock.unlock();
	return result;
}

template <class T>
int SyncQueue<T>::getMaxSize() {
	int result;
	queueLock.lock();
	result = maxSize;
	queueLock.unlock();
	return result;
}

template <class T>
int SyncQueue<T>::getMaxItems() {
	int result;
	queueLock.lock();
	result = maxItems;
	queueLock.unlock();
	return result;
}

template <class T>
CondLock *SyncQueue<T>::getLock() {
	return &queueLock;
}


template <class T>
bool SyncQueue<T>::isReadyRaw() {
	return queue->size() > 0 ? true : false;
}

template <class T>
bool SyncQueue<T>::isSpaceRaw(T *r) {
	return (int)queue->size() < maxItems && queueSize+r->getSize() <= maxSize ? true : false;
}

template <class T>
T *SyncQueue<T>::getItemRaw(bool signal) {
	assert(queue->size() > 0);
	T *r = (*queue)[0];
	queue->pop_front();
	queueSize -= r->getSize();
	if (signal)
		queueLock.signalSend();

	return r;
}

template <class T>
bool SyncQueue<T>::putItemRaw(T *r, bool signal) {
	int itemSize = r->getSize();
	assert((maxItems == 0 || (int)queue->size() < maxItems) && (maxSize == 0 || queueSize+itemSize <= maxSize));
	queue->push_back(r);
	queueSize += r->getSize();
	if (signal)
		queueLock.signalRecv();
	return true;
}

template <class T>
bool SyncQueue<T>::waitForSpaceRaw(T *r) {
	int itemSize = r->getSize();
	while ((maxItems > 0 && (int)queue->size() == maxItems) || (maxSize > 0 && queueSize+itemSize > maxSize)) {
		waitingWriters++;
		queueLock.waitSend();
		waitingWriters--;
		if (cancel)
			return false;
	}
	return true;
}

template <class T>
int SyncQueue<T>::getCurrentSizeRaw() {
	return queueSize;
}

template <class T>
int SyncQueue<T>::getCurrentItemsRaw() {
	return queue->size();
}

template <class T>
int SyncQueue<T>::getMaxSizeRaw() {
	return maxSize;
}

template <class T>
int SyncQueue<T>::getMaxItemsRaw() {
	return maxSize;
}

template <class T>
void SyncQueue<T>::setProcessed() {
	processed = true;
}

template <class T>
bool SyncQueue<T>::getProcessed() {
	return processed;
}

#endif
