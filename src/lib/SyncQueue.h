/**
 * Class representing a queue of items.
 *
 * Queue is synchronized, when full, writers would block, if empty, readers
 * would block.
 */

#ifndef _SYNCQUEUE_H_
#define _SYNCQUEUE_H_

#include <assert.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <deque>
#include "common.h"
#include "Lock.h"

using namespace std;

template<class T>
class SyncQueue {
private:
	// queue object, guarded by queue_lock
	CondLock queue_lock;
	int queue_size;
	deque<T*> *queue;
	int waitingReaders;
	int waitingWriters;
	bool cancel;
public:
	SyncQueue(int size = 1000);
	~SyncQueue();
	void cancelAll();

	bool isSpace();
	bool putItem(T *r, bool sleep);
	int putItems(T **r, int size, bool sleep);
	bool isReady();
	T *getItem(bool sleep);
	int getItems(T **r, int size, bool sleep);
};

template <class T>
SyncQueue<T>::SyncQueue(int size) {

	this->queue_size = size;
	queue = new deque<T*>();
	waitingReaders = 0;
	waitingWriters = 0;
	cancel = false;
}

template <class T>
SyncQueue<T>::~SyncQueue() {
	cancelAll();
	delete queue;
	queue = NULL;
}

template <class T>
void SyncQueue<T>::cancelAll() {
	queue_lock.lock();
	cancel = true;
	while (waitingReaders > 0) {
		queue_lock.signalRecv();
		queue_lock.unlock();
		sched_yield();
		queue_lock.lock();
	}
	while (waitingWriters > 0) {
		queue_lock.signalSend();
		queue_lock.unlock();
		sched_yield();
		queue_lock.lock();
	}
	for (unsigned i = 0; i < queue->size(); i++)
		delete (*queue)[i];
	queue->clear();
	queue_lock.unlock();
}

template <class T>
bool SyncQueue<T>::isSpace() {
	bool result = false;
	queue_lock.lock();
	if ((int)queue->size() < queue_size)
		result = true;
	queue_lock.unlock();
	return result;
}

template <class T>
bool SyncQueue<T>::putItem(T *r, bool sleep) {
	queue_lock.lock();
	if (cancel) {
		queue_lock.unlock();
		delete r;
		pthread_exit(NULL);
	}
	while ((int)queue->size() == queue_size) {
		if (!sleep) {
			queue_lock.unlock();
			return false;
		}
		waitingWriters++;
		queue_lock.waitSend();
		waitingWriters--;
		if (cancel) {
			queue_lock.unlock();
			delete r;
			pthread_exit(NULL);
		}
	}
	queue->push_back(r);
	queue_lock.signalRecv();
	queue_lock.unlock();
	return true;
}

template <class T>
int SyncQueue<T>::putItems(T **r, int size, bool sleep) {
	queue_lock.lock();
	if (cancel) {
		queue_lock.unlock();
		delete r;
		pthread_exit(NULL);
	}
	while ((int)queue->size() == queue_size) {
		if (!sleep) {
			queue_lock.unlock();
			return 0;
		}
		waitingWriters++;
		queue_lock.waitSend();
		waitingWriters--;
		if (cancel) {
			queue_lock.unlock();
			delete r;
			pthread_exit(NULL);
		}
	}
	int i;
	for (i = 0; i < size && i < (queue_size - (int)queue->size()); i++) {
		queue->push_back(r[i]);
	}
	queue_lock.signalRecv();
	queue_lock.unlock();
	return i;
}

template <class T>
bool SyncQueue<T>::isReady() {
	bool result = false;
	queue_lock.lock();
	while (queue->size() > 0)
		result = true;
	queue_lock.unlock();
	return result;
}

template <class T>
T *SyncQueue<T>::getItem(bool sleep) {
	queue_lock.lock();
	if (cancel) {
		queue_lock.unlock();
		pthread_exit(NULL);
	}
	while (queue->size() == 0) {
		if (!sleep) {
			queue_lock.unlock();
			return NULL;
		}
		waitingReaders++;
		queue_lock.waitRecv();
		waitingReaders--;
		if (cancel) {
			queue_lock.unlock();
			pthread_exit(NULL);
		}
	}
	T *r = (*queue)[0];
	queue->pop_front();
	queue_lock.signalSend();
	queue_lock.unlock();

	return r;
}

template <class T>
int SyncQueue<T>::getItems(T **r, int size, bool sleep) {
	queue_lock.lock();
	if (cancel) {
		queue_lock.unlock();
		pthread_exit(NULL);
	}
	while (queue->size() == 0) {
		if (!sleep) {
			queue_lock.unlock();
			return 0;
		}
		waitingReaders++;
		queue_lock.waitRecv();
		waitingReaders--;
		if (cancel) {
			queue_lock.unlock();
			pthread_exit(NULL);
		}
	}
	int i = 0;
	for (i = 0; i < size && (int)queue->size() > 0; i++) {
		r[i] = (*queue)[0];
		queue->pop_front();
	}
	queue_lock.signalSend();
	queue_lock.unlock();

	return i;
}

#endif
