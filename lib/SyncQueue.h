/**
 * Class representing a queue of items.
 *
 * Queue is synchronized, when full, writers would block, if empty, readers
 * would block. The global queue may be broken into several different
 * sub-queues according to priority (int) values, default priority is 0.
 * Sub-queues are sorted according to priorities, the lock is shared among all
 * sub-queues.
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
#include <algorithm>
#include <deque>
#include <vector>
#include <tr1/unordered_map>
#include "common.h"
#include "CondLock.h"

using namespace std;

// one queue (sub-queue of SyncQueue with defined priority), not synchronized
// (must be done in SyncQueue)
template<class T>
class SimpleQueue {
public:
	SimpleQueue(int priority, int maxItems, int maxSize);
	~SimpleQueue();
	void deleteAllItems();
	
	int getPriority();
	bool operator<(SimpleQueue<T> other);

	bool isSpace(T *r);
	void putItem(T *r);
	bool isReady();
	T *getItem();

	int getCurrentSize();
	int getCurrentItems();
	int getMaxSize();
	int getMaxItems();

private:
	int priority;
	int maxItems;
	int maxSize;
	int queueSize;
	deque<T*> *queue;
};

template <class T>
SimpleQueue<T>::SimpleQueue(int priority, int maxItems, int maxSize) {
	this->priority = priority;
	this->maxItems = maxItems;
	this->maxSize = maxSize;
	queueSize = 0;
	queue = new deque<T*>();
}

template <class T>
SimpleQueue<T>::~SimpleQueue() {
	// to be sure, should be deleted in SyncQueue::Stop()
	for (typename deque<T*>::iterator iter = queue->begin(); iter != queue->end(); ++iter) {
		delete (*iter);
	}
	delete queue;
}

template <class T>
void SimpleQueue<T>::deleteAllItems() {
	for (typename deque<T*>::iterator iter = queue->begin(); iter != queue->end(); ++iter) {
		delete (*iter);
	}
	queue->clear();
}

template <class T>
int SimpleQueue<T>::getPriority() {
	return priority;
}

template <class T>
bool SimpleQueue<T>::operator<(SimpleQueue<T> other) {
	// reversed, so that we have descending numbers (higher priority first)
	return priority > other.getPriority();
}

template <class T>
bool SimpleQueue<T>::isSpace(T *r) {
	return ((int)queue->size() < maxItems && queueSize+r->getSize() <= maxSize) ? true : false;
}

template <class T>
void SimpleQueue<T>::putItem(T *r) {
	queue->push_back(r);
	queueSize += r->getSize();
}

template <class T>
bool SimpleQueue<T>::isReady() {
	return (queue->size() > 0) ? true : false;
}

template <class T>
T *SimpleQueue<T>::getItem() {
	if (queue->size() == 0)
		return NULL;
	T *r = (*queue)[0];
	queue->pop_front();
	queueSize -= r->getSize();
	return r;
}

template <class T>
int SimpleQueue<T>::getCurrentSize() {
	return queueSize;
}

template <class T>
int SimpleQueue<T>::getCurrentItems() {
	return queue->size();
}

template <class T>
int SimpleQueue<T>::getMaxSize() {
	return maxSize;
}

template <class T>
int SimpleQueue<T>::getMaxItems() {
	return maxItems;
}

// By default, no sub-queuei is created, they may be added using addQueue()

template<class T>
class SyncQueue {
public:
	SyncQueue();
	~SyncQueue();
	void addQueue(int priority, int maxItems, int maxSize);
	bool hasQueue(int priority);
	int getQueuesCount();
	void Stop();
	void Start();
	void Pause();
	void Resume();

	bool isSpace(T *r, int priority = 0);
	bool putItem(T *r, bool sleep, int priority = 0);
	int putItems(T **r, int size, bool sleep, int priority = 0);
	bool isReady();
	T *getItem(bool sleep);
	int getItems(T **r, int size, bool sleep);
	int queueItems(int priority = 0);

	int firstNonEmptyQueueIndex();

private:
	// queue object, following attributes (+queues) are guarded by queueLock
	CondLock queueLock;
	int waitingReaders;
	int waitingWriters;
	bool cancel;

	// all data-driven operations must hold this lock for a moment: (purpose: pause/resume)
	Lock pauseLock;

	vector<SimpleQueue<T>*> queues;
	std::tr1::unordered_map<int, SimpleQueue<T>*> priority2queue;
};

template <class T>
SyncQueue<T>::SyncQueue() {
	waitingReaders = 0;
	waitingWriters = 0;
	cancel = false;
}

template <class T>
SyncQueue<T>::~SyncQueue() {
	Stop();
	// to be sure, should be deleted in Stop()
	for (typename vector<SimpleQueue<T>*>::iterator iter = queues.begin(); iter != queues.end(); ++iter) {
		delete *iter;
	}
}

template <class T>
void SyncQueue<T>::addQueue(int priority, int maxItems, int maxSize) {
	SimpleQueue<T> *q = new SimpleQueue<T>(priority, maxItems, maxSize);

	typename std::tr1::unordered_map<int, SimpleQueue<T>*>::iterator iter = priority2queue.find(priority);
	assert(iter == priority2queue.end());
	priority2queue[priority] = q;
	queues.push_back(q);
	std::sort(queues.begin(), queues.end());
}

template <class T>
bool SyncQueue<T>::hasQueue(int priority) {
	typename std::tr1::unordered_map<int, SimpleQueue<T>*>::iterator iter = priority2queue.find(priority);
	return iter != priority2queue.end();
}

template <class T>
int SyncQueue<T>::getQueuesCount() {
	return queues.size();
}

template <class T>
void SyncQueue<T>::Stop() {
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

	for (typename vector<SimpleQueue<T>*>::iterator iter = queues.begin(); iter != queues.end(); ++iter) {
		(*iter)->deleteAllItems();
	}
	queueLock.unlock();
}

template <class T>
void SyncQueue<T>::Start() {
	queueLock.lock();
	cancel = false;
	queueLock.unlock();
}

template <class T>
void SyncQueue<T>::Pause() {
	pauseLock.lock();
}

template <class T>
void SyncQueue<T>::Resume() {
	pauseLock.unlock();
}

template <class T>
bool SyncQueue<T>::isSpace(T *r, int priority) {
	bool result = false;
	queueLock.lock();
	typename std::tr1::unordered_map<int, SimpleQueue<T>*>::iterator iter = priority2queue.find(priority);
	assert(iter != priority2queue.end());
	SimpleQueue<T> *q = *iter;
	if (q->getCurrentItems() < q->getMaxItems() && q->getCurrentSize()+r->getSize() <= q->getMaxSize())
		result = true;
	queueLock.unlock();
	return result;
}

// returns false if canceled
template <class T>
bool SyncQueue<T>::putItem(T *r, bool sleep, int priority) {
	pauseLock.lock();
	pauseLock.unlock();
	queueLock.lock();
	if (cancel) {
		queueLock.unlock();
		return false;
	}
	typename std::tr1::unordered_map<int, SimpleQueue<T>*>::iterator iter = priority2queue.find(priority);
	assert(iter != priority2queue.end());
	SimpleQueue<T> *q = iter->second;
	int itemSize = r->getSize();
	while ((q->getMaxItems() > 0 && q->getCurrentItems() == q->getMaxItems()) || (q->getMaxSize() > 0 && q->getCurrentSize()+itemSize > q->getMaxSize())) {
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

	q->putItem(r);

	queueLock.signalRecv();
	queueLock.unlock();
	return true;
}

template <class T>
int SyncQueue<T>::putItems(T **r, int size, bool sleep, int priority) {
	pauseLock.lock();
	pauseLock.unlock();
	queueLock.lock();
	if (cancel) {
		queueLock.unlock();
		return 0;
	}
	typename std::tr1::unordered_map<int, SimpleQueue<T>*>::iterator iter = priority2queue.find(priority);
	assert(iter != priority2queue.end());
	SimpleQueue<T> *q = *iter;
	int itemSize = r[0]->getSize();
	while ((q->getMaxItems() > 0 && q->getCurrentItems() == q->getMaxItems()) || (q->getMaxSize() > 0 && q->getCurrentSize()+itemSize > q->getMaxSize()))  {
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

	int max = q->getMaxItems() > 0 ? (q->getMaxItems() - q->getCurrentItems()) : 0;
	if (max == 0 || max > size)
		max = size;

	int i;	
	for (i = 0; i < max && q->getCurrentSize() + r[i]->getSize() <= q->getMaxSize(); i++) {
		q->putItem(r[i]);
	}

	queueLock.signalRecv();
	queueLock.unlock();
	return i;
}

template <class T>
bool SyncQueue<T>::isReady() {
	bool result = false;
	queueLock.lock();
	if (firstNonEmptyQueueIndex() >= 0)
		result = true;
	queueLock.unlock();
	return result;
}

template <class T>
T *SyncQueue<T>::getItem(bool sleep) {
	pauseLock.lock();
	pauseLock.unlock();
	queueLock.lock();
	if (cancel) {
		queueLock.unlock();
		return NULL;
	}

	while (firstNonEmptyQueueIndex() < 0) {
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

	T *r = queues[firstNonEmptyQueueIndex()]->getItem();

	queueLock.signalSend();
	queueLock.unlock();

	return r;
}

template <class T>
int SyncQueue<T>::getItems(T **r, int size, bool sleep) {
	pauseLock.lock();
	pauseLock.unlock();
	queueLock.lock();
	if (cancel) {
		queueLock.unlock();
		return 0;
	}
	while (firstNonEmptyQueueIndex() < 0) {
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
	for (typename vector<SimpleQueue<T>*>::iterator iter = queues.begin(); iter != queues.end() && i < size; ++iter) {
		while ((*iter)->getCurrentItems() > 0 && i < size) {
			r[i] = (*iter)->getItem();
			i++;
		}
	}

	queueLock.signalSend();
	queueLock.unlock();

	return i;
}

template <class T>
int SyncQueue<T>::queueItems(int priority) {
	typename std::tr1::unordered_map<int, SimpleQueue<T>*>::iterator iter = priority2queue.find(priority);
	assert(iter != priority2queue.end());
	return iter->second->getCurrentItems();
}

template <class T>
int SyncQueue<T>::firstNonEmptyQueueIndex() {
	int i = 0;
	for (typename vector<SimpleQueue<T>*>::iterator iter = queues.begin(); iter != queues.end(); ++iter) {
		if ((*iter)->getCurrentItems() > 0)
			return i;
		i++;
	}
	return -1;
}

#endif
