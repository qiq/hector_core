/**
 * Class representing queue of resources (mainly HTML pages) while processing.
 * Robot uses several queues.
 */

#ifndef _RESOURCEQUEUE_H_
#define _RESOURCEQUEUE_H_

#include <unistd.h>
#include <pthread.h>
#include <deque>
#include <string>
#include "Resource.h"
#include "../lib/common.h"
#include "../lib/Lock.h"

using namespace std;
//namespace stdext = ::__gnu_cxx;

class ResourceQueue {
private:
	// queue object, guarded by queue_lock
	CondLock queue_lock;
	int queue_size;
	deque<Resource*> *queue;
public:
	ResourceQueue(int size);
	~ResourceQueue();

	void putResource(Resource *r);
	Resource *getResource();
};

#endif
