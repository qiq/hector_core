/**
 * Class representing a queue of items.
 *
 * Queue is synchronized, when full, writers would block, if empty, readers
 * would block. The global queue may be broken into several different
 * sub-queues according to priority (int) values, default priority is 0.
 * Sub-queues are sorted according to priorities, the lock is shared among all
 * sub-queues.
 *
 * timeout arguments are specified in useconds, < 0 = do not sleep, 0 = sleep forever
 */

#ifndef _LIB_SYNCQUEUE_H_
#define _LIB_SYNCQUEUE_H_

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
	
	int GetPriority();

	bool IsSpaceFor(int itemSize);
	void PutItem(T *r);
	bool IsReady();
	T *GetItem();

	int GetCurrentSize();
	int GetCurrentItems();
	int GetMaxSize();
	int GetMaxItems();
	static bool comp(SimpleQueue<T> *a, SimpleQueue<T> *b);

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
int SimpleQueue<T>::GetPriority() {
	return priority;
}

template <class T>
bool SimpleQueue<T>::IsSpaceFor(int itemSize) {
	return ((maxItems == 0 || (int)queue->size() < maxItems) && (maxSize == 0 || queueSize+itemSize <= maxSize)) ? true : false;
}

template <class T>
void SimpleQueue<T>::PutItem(T *r) {
	queue->push_back(r);
	queueSize += r->GetSize();
}

template <class T>
bool SimpleQueue<T>::IsReady() {
	return (queue->size() > 0) ? true : false;
}

template <class T>
T *SimpleQueue<T>::GetItem() {
	if (queue->size() == 0)
		return NULL;
	T *r = (*queue)[0];
	queue->pop_front();
	queueSize -= r->GetSize();
	return r;
}

template <class T>
int SimpleQueue<T>::GetCurrentSize() {
	return queueSize;
}

template <class T>
int SimpleQueue<T>::GetCurrentItems() {
	return queue->size();
}

template <class T>
int SimpleQueue<T>::GetMaxSize() {
	return maxSize;
}

template <class T>
int SimpleQueue<T>::GetMaxItems() {
	return maxItems;
}

template <class T>
bool SimpleQueue<T>::comp(SimpleQueue<T> *a, SimpleQueue<T> *b) {
	// reversed, so that we have descending numbers (higher priority first)
	return a->GetPriority() > b->GetPriority();
}

// By default, no sub-queue is created, they may be added using addQueue()

template<class T>
class SyncQueue {
public:
	SyncQueue();
	~SyncQueue();
	void addQueue(int priority, int maxItems, int maxSize);
	bool hasQueue(int priority);
	int GetQueuesCount();
	void Stop();
	void Start();
	void Pause();
	void Resume();

	bool IsSpace(T *r, int priority = 0);
	bool PutItem(T *r, struct timeval *timeout, int priority = 0);
	int PutItems(T **r, int size, struct timeval *timeout, int priority = 0);
	bool IsReady();
	T *GetItem(struct timeval *timeout);
	int GetItems(T **r, int size, struct timeval *timeout);
	int queueItems(int priority = 0);

	int firstNonEmptyQueueIndex();

private:
	// queue object, following attributes (+queues) are guarded by queueLock
	CondLock queueLock;
	int waitingReaders;
	int waitingWriters;
	bool cancel;

	// all data-driven operations must hold this lock for a moment: (purpose: pause/resume)
	PlainLock pauseLock;

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
	std::sort(queues.begin(), queues.end(), SimpleQueue<T>::comp);
}

template <class T>
bool SyncQueue<T>::hasQueue(int priority) {
	typename std::tr1::unordered_map<int, SimpleQueue<T>*>::iterator iter = priority2queue.find(priority);
	return iter != priority2queue.end();
}

template <class T>
int SyncQueue<T>::GetQueuesCount() {
	return queues.size();
}

template <class T>
void SyncQueue<T>::Stop() {
	queueLock.Lock();
	cancel = true;
	while (waitingReaders > 0) {
		queueLock.SignalRecv();
		queueLock.Unlock();
		sched_yield();
		queueLock.Lock();
	}
	while (waitingWriters > 0) {
		queueLock.SignalSend();
		queueLock.Unlock();
		sched_yield();
		queueLock.Lock();
	}

	for (typename vector<SimpleQueue<T>*>::iterator iter = queues.begin(); iter != queues.end(); ++iter) {
		(*iter)->deleteAllItems();
	}
	queueLock.Unlock();
}

template <class T>
void SyncQueue<T>::Start() {
	queueLock.Lock();
	cancel = false;
	queueLock.Unlock();
}

template <class T>
void SyncQueue<T>::Pause() {
	pauseLock.Lock();
}

template <class T>
void SyncQueue<T>::Resume() {
	pauseLock.Unlock();
}

template <class T>
bool SyncQueue<T>::IsSpace(T *r, int priority) {
	bool result = false;
	queueLock.Lock();
	typename std::tr1::unordered_map<int, SimpleQueue<T>*>::iterator iter = priority2queue.find(priority);
	assert(iter != priority2queue.end());
	SimpleQueue<T> *q = *iter;
	result = q->IsSpaceFor(r->GetSize());
	queueLock.Unlock();
	return result;
}

// returns false if canceled
template <class T>
bool SyncQueue<T>::PutItem(T *r, struct timeval *timeout, int priority) {
	pauseLock.Lock();
	pauseLock.Unlock();
	queueLock.Lock();
	if (cancel) {
		queueLock.Unlock();
		return false;
	}
	typename std::tr1::unordered_map<int, SimpleQueue<T>*>::iterator iter = priority2queue.find(priority);
	assert(iter != priority2queue.end());
	SimpleQueue<T> *q = iter->second;
	int itemSize = r->GetSize();
	while (!q->IsSpaceFor(itemSize)) {
		if (!timeout) {
			queueLock.Unlock();
			return false;
		}
		waitingWriters++;
		bool timedOut = !queueLock.WaitSend(timeout);
		waitingWriters--;
		if (cancel || timedOut) {
			queueLock.Unlock();
			return false;
		}
	}

	q->PutItem(r);

	queueLock.SignalRecv();
	queueLock.Unlock();
	return true;
}

template <class T>
int SyncQueue<T>::PutItems(T **r, int size, struct timeval *timeout , int priority) {
	pauseLock.Lock();
	pauseLock.Unlock();
	queueLock.Lock();
	if (cancel) {
		queueLock.Unlock();
		return 0;
	}
	typename std::tr1::unordered_map<int, SimpleQueue<T>*>::iterator iter = priority2queue.find(priority);
	assert(iter != priority2queue.end());
	SimpleQueue<T> *q = *iter;
	int itemSize = r[0]->GetSize();
	while ((q->GetMaxItems() > 0 && q->GetCurrentItems() == q->GetMaxItems()) || (q->GetMaxSize() > 0 && q->GetCurrentSize()+itemSize > q->GetMaxSize()))  {
		if (!timeout) {
			queueLock.Unlock();
			return 0;
		}
		waitingWriters++;
		bool timedOut = !queueLock.WaitSend(timeout);
		waitingWriters--;
		if (cancel || timedOut) {
			queueLock.Unlock();
			return 0;
		}
	}

	int max = q->GetMaxItems() > 0 ? (q->GetMaxItems() - q->GetCurrentItems()) : 0;
	if (max == 0 || max > size)
		max = size;

	int i;	
	for (i = 0; i < max && q->GetCurrentSize() + r[i]->GetSize() <= q->GetMaxSize(); i++) {
		q->PutItem(r[i]);
	}

	queueLock.SignalRecv();
	queueLock.Unlock();
	return i;
}

template <class T>
bool SyncQueue<T>::IsReady() {
	bool result = false;
	queueLock.Lock();
	if (firstNonEmptyQueueIndex() >= 0)
		result = true;
	queueLock.Unlock();
	return result;
}

template <class T>
T *SyncQueue<T>::GetItem(struct timeval *timeout) {
	pauseLock.Lock();
	pauseLock.Unlock();
	queueLock.Lock();
	if (cancel) {
		queueLock.Unlock();
		return NULL;
	}

	while (firstNonEmptyQueueIndex() < 0) {
		if (!timeout) {
			queueLock.Unlock();
			return NULL;
		}
		waitingReaders++;
		bool timedOut = !queueLock.WaitRecv(timeout);
		waitingReaders--;
		if (cancel || timedOut) {
			queueLock.Unlock();
			return NULL;
		}
	}

	T *r = queues[firstNonEmptyQueueIndex()]->GetItem();

	queueLock.SignalSend();
	queueLock.Unlock();

	return r;
}

template <class T>
int SyncQueue<T>::GetItems(T **r, int size, struct timeval *timeout) {
	pauseLock.Lock();
	pauseLock.Unlock();
	queueLock.Lock();
	if (cancel) {
		queueLock.Unlock();
		return 0;
	}
	while (firstNonEmptyQueueIndex() < 0) {
		if (!timeout) {
			queueLock.Unlock();
			return 0;
		}
		waitingReaders++;
		bool timedOut = !queueLock.WaitRecv(timeout);
		waitingReaders--;
		if (cancel || timedOut) {
			queueLock.Unlock();
			return 0;
		}
	}
	int i = 0;
	for (typename vector<SimpleQueue<T>*>::iterator iter = queues.begin(); iter != queues.end() && i < size; ++iter) {
		while ((*iter)->GetCurrentItems() > 0 && i < size) {
			r[i] = (*iter)->GetItem();
			i++;
		}
	}

	queueLock.SignalSend();
	queueLock.Unlock();

	return i;
}

template <class T>
int SyncQueue<T>::queueItems(int priority) {
	typename std::tr1::unordered_map<int, SimpleQueue<T>*>::iterator iter = priority2queue.find(priority);
	assert(iter != priority2queue.end());
	return iter->second->GetCurrentItems();
}

template <class T>
int SyncQueue<T>::firstNonEmptyQueueIndex() {
	int i = 0;
	for (typename vector<SimpleQueue<T>*>::iterator iter = queues.begin(); iter != queues.end(); ++iter) {
		if ((*iter)->GetCurrentItems() > 0)
			return i;
		i++;
	}
	return -1;
}

#endif
