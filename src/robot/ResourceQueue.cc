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

void ResourceQueue::putResource(Resource *r) {
	queue_lock.lock();
	while ((int)queue->size() == queue_size)
		queue_lock.waitSend();
	queue->push_back(r);
	queue_lock.signalRecv();
	queue_lock.unlock();
}

Resource *ResourceQueue::getResource() {
	queue_lock.lock();
	while (queue->size() == 0)
		queue_lock.waitRecv();
	Resource *r = (*queue)[queue->size()-1];
	queue->pop_front();
	queue_lock.signalSend();
	queue_lock.unlock();

	return r;
}
