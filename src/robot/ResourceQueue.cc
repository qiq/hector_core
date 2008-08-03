/**
 * Class representing queue of resources (mainly HTML pages) while processing.
 * Robot uses several queues.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "ResourceQueue.h"

ResourceQueue::ResourceQueue(int size = 1000) {

	this->queue_size = size;
	queue = new deque<Resource*>(size);
}

ResourceQueue::~ResourceQueue() {
	queue_lock.lock();
	assert(queue->empty());
	delete queue;
	queue = NULL;
	queue_lock.unlock();
}

bool ResourceQueue::isSpace() {
	bool result = false;
	queue_lock.lock();
	if ((int)queue->size() < queue_size)
		result = true;
	queue_lock.unlock();
	return result;
}

bool ResourceQueue::putResource(Resource *r, bool sleep) {
	queue_lock.lock();
	while ((int)queue->size() == queue_size) {
		if (!sleep) {
			queue_lock.unlock();
			return false;
		}
		queue_lock.waitSend();
	}
	queue->push_back(r);
	queue_lock.signalRecv();
	queue_lock.unlock();
	return true;
}

int ResourceQueue::putResources(Resource **r, int size, bool sleep) {
	queue_lock.lock();
	while ((int)queue->size() == queue_size) {
		if (!sleep) {
			queue_lock.unlock();
			return 0;
		}
		queue_lock.waitSend();
	}
	int i;
	for (i = 0; i < size && i < (queue_size - (int)queue->size()); i++) {
		queue->push_back(r[i]);
	}
	queue_lock.signalRecv();
	queue_lock.unlock();
	return i;
}

bool ResourceQueue::isReady() {
	bool result = false;
	queue_lock.lock();
	while (queue->size() > 0)
		result = true;
	queue_lock.unlock();
	return result;
}

Resource *ResourceQueue::getResource(bool sleep) {
	queue_lock.lock();
	while (queue->size() == 0) {
		if (!sleep) {
			queue_lock.unlock();
			return NULL;
		}
		queue_lock.waitRecv();
	}
	Resource *r = (*queue)[0];
	queue->pop_front();
	queue_lock.signalSend();
	queue_lock.unlock();

	return r;
}

int ResourceQueue::getResources(Resource **r, int size, bool sleep) {
	queue_lock.lock();
	while (queue->size() == 0) {
		if (!sleep) {
			queue_lock.unlock();
			return 0;
		}
		queue_lock.waitRecv();
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
