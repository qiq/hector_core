/**
 * Class representing queue of resources (mainly HTML pages) while processing.
 * Robot uses several queues.
 */

#ifndef _RESOURCEQUEUE_H_
#define _RESOURCEQUEUE_H_

#include <unistd.h>
#include <deque>
#include "Resource.h"
#include "../lib/common.h"
#include "../lib/Lock.h"

using namespace std;

class ResourceQueue {
private:
	// queue object, guarded by queue_lock
	CondLock queue_lock;
	int queue_size;
	deque<Resource*> *queue;
public:
	ResourceQueue(int size);
	~ResourceQueue();

	bool isSpace();
	bool putResource(Resource *r, bool sleep);
	int putResources(Resource **r, int size, bool sleep);
	bool isReady();
	Resource *getResource(bool sleep);
	int getResources(Resource **r, int size, bool sleep);
};

#endif
