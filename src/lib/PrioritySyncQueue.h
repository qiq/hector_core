/**
 * Class representing several queues, with different prioritites.
 *
 * Overview
 * - we use several different SyncQueues, every one has a (distinct) priority
 * - writers: directly using SyncQueue, no changes
 * - readers: using PrioritySyncQueue: read from all queues at once. There is one
 *   (common) Condition Variable. Reader blocks unless there is a T in _any_
 *   queue. So we check all queues keeping a single lock.
 * - One lock must be shared across all SyncQueues, as well as Recv CV.
 */

#ifndef _PRIORITYSYNCQUEUE_H_
#define _PRIORITYSYNCQUEUE_H_

#include <config.h>

#include <assert.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <deque>
#include <vector>
#include "common.h"
#include "CondLock.h"
#include "SyncQueue.h"

using namespace std;

// used for sorting of SyncQueues according to priority
template<class T>
class Priority {
	SyncQueue<T> *queue;
	int priority;
public:
	Priority(SyncQueue<T> *queue, int priority): queue(queue), priority(priority) {};
	~Priority() {};

	SyncQueue<T> *getQueue();
	int getPriority();

	bool operator<(Priority<T> other);
};

template <class T>
SyncQueue<T> *Priority<T>::getQueue() {
	return queue;
}

template <class T>
int Priority<T>::getPriority() {
	return priority;
}

template <class T>
bool Priority<T>::operator<(Priority<T> other) {
	// reversed, so that we have descending numbers (higher priority first)
	return priority > other.getPriority();
}

template<class T>
class PrioritySyncQueue {
	// queue object, all attributes are guarded by queueLock
	CondLock queueLock;
	vector<Priority<T>*> priorities;
	vector<SyncQueue<T>*> queues;
	int waitingReaders;
	bool cancel;

	int getReadyQueueIndex();
public:
	PrioritySyncQueue();
	~PrioritySyncQueue();

	void addQueue(SyncQueue<T> *queue, int priority, bool last);

	void cancelAll();
	bool isReady();
	T *getItem(bool sleep);
	int getItems(T **r, int size, bool sleep);

	int getCurrentSize();
	int getCurrentItems();
};

template <class T>
int PrioritySyncQueue<T>::getReadyQueueIndex() {
	int i = 0;
	for (typename vector<SyncQueue<T>*>::iterator iter = queues.begin(); iter != queues.end(); ++iter) {
		if ((*iter)->isReadyRaw())
			return i;
		++i;
	}
	return -1;
}

template <class T>
PrioritySyncQueue<T>::PrioritySyncQueue() {
	waitingReaders = 0;
	cancel = false;
}

template <class T>
PrioritySyncQueue<T>::~PrioritySyncQueue() {
	cancelAll();
	for (typename vector<Priority<T>*>::iterator iter = priorities.begin(); iter != priorities.end(); ++iter) {
		delete *iter;
	}

	// replace deleted mutexes/Recv CVs with new ones
	for (typename vector<SyncQueue<T>*>::iterator iter = queues.begin(); iter != queues.end(); ++iter) {
		CondLock *lock = (*iter)->getLock();
		pthread_mutex_t *mutex = new pthread_mutex_t;
		pthread_mutex_init(mutex, NULL);

		pthread_cond_t *cond = new pthread_cond_t;
		pthread_cond_init(cond, NULL);
		lock->setMutex(mutex, false);
		lock->setCondRecv(cond, false);
	}
}

template <class T>
void PrioritySyncQueue<T>::addQueue(SyncQueue<T> *queue, int priority, bool last) {
	if (queue && !queue->getProcessed()) {
		queue->setProcessed();
		priorities.push_back(new Priority<T>(queue, priority));
	}
	if (last) {
		// sort queues according to the priority
		std::sort(priorities.begin(), priorities.end());
		queues.clear();
		for (typename vector<Priority<T>*>::iterator iter = priorities.begin(); iter != priorities.end(); ++iter) {
			queues.push_back((*iter)->getQueue());
		}

		// replace mutex and Recv condition variable
		pthread_mutex_t *mutex = queueLock.getMutex();
		pthread_cond_t *condRecv = queueLock.getCondRecv();

		for (typename vector<SyncQueue<T>*>::iterator iter = queues.begin(); iter != queues.end(); ++iter) {
			(*iter)->getLock()->setMutex(mutex, true);
			(*iter)->getLock()->setCondRecv(condRecv, true);
		}
	}
}

template <class T>
void PrioritySyncQueue<T>::cancelAll() {
	queueLock.lock();
	cancel = true;
	while (waitingReaders > 0) {
		queueLock.signalRecv();
		queueLock.unlock();
		sched_yield();
		queueLock.lock();
	}
	queueLock.unlock();
}

template <class T>
bool PrioritySyncQueue<T>::isReady() {
	bool result = false;
	queueLock.lock();
	for (typename vector<SyncQueue<T>*>::iterator iter = queues.begin(); iter != queues.end(); ++iter) {
		if (iter->isReady()) {
			result = true;
			break;
		}
	}
	queueLock.unlock();
	return result;
}

template <class T>
T *PrioritySyncQueue<T>::getItem(bool sleep) {
	queueLock.lock();
	if (cancel) {
		queueLock.unlock();
		return NULL;
	}
	int readyQueueIndex = getReadyQueueIndex();
	while (readyQueueIndex < 0) {
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
		readyQueueIndex = getReadyQueueIndex();
	}
	
	T *r = queues[readyQueueIndex]->getItemRaw(true);
	queueLock.unlock();

	return r;
}

template <class T>
int PrioritySyncQueue<T>::getItems(T **r, int size, bool sleep) {
	queueLock.lock();
	if (cancel) {
		queueLock.unlock();
		return 0;
	}
	int readyQueueIndex = getReadyQueueIndex();
	while (readyQueueIndex < 0) {
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
	int items = 0;
	int qn = queues.size();
	int qi = readyQueueIndex;
	while (qi < qn) {
		if (queues[qi]->isReadyRaw()) {
			qi++;
		} else {
			r[items] = queues[qi]->getItemRaw(false);
			if (++items == size)
				break;
		}
	}
	queueLock.signalSend();
	queueLock.unlock();

	return items;
}

template <class T>
int PrioritySyncQueue<T>::getCurrentSize() {
	int result;
	queueLock.lock();
	for (typename vector<SyncQueue<T>*>::iterator iter = queues.begin(); iter != queues.end(); ++iter) {
		result += iter->getCurrentSizeRaw();
	}
	queueLock.unlock();
	return result;
}

template <class T>
int PrioritySyncQueue<T>::getCurrentItems() {
	int result;
	queueLock.lock();
	for (typename vector<SyncQueue<T>*>::iterator iter = queues.begin(); iter != queues.end(); ++iter) {
		result += iter->getCurrentItemsRaw();
	}
	queueLock.unlock();
	return result;
}

#endif
