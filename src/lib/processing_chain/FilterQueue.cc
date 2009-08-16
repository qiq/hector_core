/**
 *
 */

#include <config.h>

#include "FilterQueue.h"

log4cxx::LoggerPtr FilterQueue::logger(log4cxx::Logger::getLogger("lib.processing_chain.FilterQueue"));

FilterQueue::FilterQueue(ObjectRegistry *objects) {
	this->objects = objects;

	simpleOutputQueue = NULL;
	filterOutputQueue = NULL;
}

FilterQueue::~FilterQueue() {
	delete filterOutputQueue;
}

bool FilterQueue::Init(Config *config, const char *id) {
	char buffer[1024];
	vector<string> *v;

	// output queue(s)
	snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/outputQueue/@ref", id);
	v = config->getValues(buffer);
	int i = 1;
	if (v) {
		if (v->size() == 1) {
			// simple case
			const char *qid = (*v)[0].c_str();
			snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/outputQueue[%d]/@filter", qid, i);
			char *s = config->getFirstValue(buffer);
			simpleFilter = 0;
			if (s) {
				if (sscanf(s, "%d", &simpleFilter) != 1) {
					LOG4CXX_ERROR(logger, "Invalid outputQueue filter: " << s);
					return false;
				}
			}
			free(s);
			Queue *queue = dynamic_cast<Queue*>(objects->getObject(qid));
			if (!simpleOutputQueue) {
				LOG4CXX_ERROR(logger, "Queue not found: " << qid);
				return false;
			}
			simpleOutputQueue = queue->getSyncQueue();
		} else {
			// more queues
			for (vector<string>::iterator iter = v->begin(); iter != v->end(); ++iter) {
				const char *qid = iter->c_str();
				snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/outputQueue[%d]/@filter", qid, i);
				char *s = config->getFirstValue(buffer);
				int filter = 0;
				if (s) {
					if (sscanf(s, "%d", &filter) != 1) {
						LOG4CXX_ERROR(logger, "Invalid outputQueue filter: " << s);
						return false;
					}
				}
				free(s);
				Queue *queue = dynamic_cast<Queue*>(objects->getObject(qid));
				if (!queue) {
					LOG4CXX_ERROR(logger, "Queue not found: " << qid);
					return false;
				}

				OutputQueue *oq = new OutputQueue(queue, filter);
				filterOutputQueue->push_back(oq);
				i++;
			}
			// sort queues according to id
			std::sort(filterOutputQueue->begin(), filterOutputQueue->end());
		}
		delete v;
	}

	return true;
}

void FilterQueue::Start() {
}

void FilterQueue::Stop() {
}

int FilterQueue::getQueueCount() {
	if (simpleOutputQueue)
		return 1;
	return filterOutputQueue->size();
}

bool FilterQueue::putResource(Resource *r, bool sleep) {
	int status = r->getStatus();

	// simple case
	if (simpleOutputQueue) {
		if (simpleFilter == 0 || status == simpleFilter)
			return simpleOutputQueue->putItem(r, sleep);
		return true;
	}

	// multiple queues case

	// we lock all queues, there is no need to worry about deadlock,
	// because we always lock in ascending order (according to queueId)
	for (vector<OutputQueue*>::iterator iter = filterOutputQueue->begin(); iter != filterOutputQueue->end(); ++iter) {
		OutputQueue *q = *iter;
		if (q->getFilter() > 0 && status != q->getFilter())
			continue;

		if (sleep) {
			q->getQueue()->putResource(r, true);
		} else {
			q->getLock()->lock();
			if (!(q->getSyncQueue()->isSpaceRaw(r))) {
				// unlock everything
				for (vector<OutputQueue*>::iterator iter2 = filterOutputQueue->begin(); iter2 != iter; ++iter2)
					(*iter2)->getLock()->unlock();
				q->getLock()->unlock();
				return false;
			}
		}
	}

	if (!sleep) {
		// every queue is locked, add resource and unlock it
		for (vector<OutputQueue*>::iterator iter = filterOutputQueue->begin(); iter != filterOutputQueue->end(); ++iter) {
			OutputQueue *q = *iter;
			if (q->getFilter() > 0 && status != q->getFilter())
				continue;

			q->getSyncQueue()->putItemRaw(r, true);
			q->getLock()->unlock();
		}
	}

	return true;
}

int FilterQueue::putResources(Resource **r, int size, bool sleep) {
	assert(size > 0);

	// simpler case
	if (simpleOutputQueue) {
		// we must lock queue and insert as many items as possible
		simpleOutputQueue->getLock()->lock();

		int item;
		for (item = 0; item < size; ++item) {
			int rstatus = r[item]->getStatus();
			if (simpleFilter > 0 && rstatus != simpleFilter)
				continue;

			while (!(simpleOutputQueue->isSpaceRaw(r[item]))) {
				if (!sleep) {
					simpleOutputQueue->getLock()->unlock();
					return item;
				} else {
					if (!(simpleOutputQueue->waitForSpaceRaw(r[item]))) {
						simpleOutputQueue->getLock()->unlock();
						return item;
					}
				}
			}
			simpleOutputQueue->putItemRaw(r[item], true);
		}
		simpleOutputQueue->getLock()->unlock();
		return item;
	}

	// much more complicated: several queues
	bool loop;
	int minIndex;		// index of queue with least free space
	int minItems;		// number of items that may be inserted in the queue
	do {
		loop = false;
		minIndex = std::numeric_limits<int>::max();
		minItems = std::numeric_limits<int>::max();
		// lock queues in sequence + check how many resources we are able to put into every queue
		int index = 0;
		int n = filterOutputQueue->size();
		for (index = 0; index < n; ++index) {
			OutputQueue *q = (*filterOutputQueue)[index];
			SyncQueue<Resource> *syncQueue = q->getSyncQueue();

			// lock all queues
			syncQueue->getLock()->lock();

			int availItems = syncQueue->getMaxItemsRaw() - syncQueue->getCurrentItemsRaw();
			int availSize = syncQueue->getMaxSizeRaw() - syncQueue->getCurrentSizeRaw();
			int item;
			for (item = 0; item < size; ++item) {
				// optimization
				if (item >= minItems)
					break;
	
				int rstatus = r[item]->getStatus();
				if (q->getFilter() > 0 && rstatus != q->getFilter())
					continue;
	
				availItems--;
				availSize -= r[item]->getSize();
				if (availItems < 0 || availSize < 0)
					break;
			}
			// record which queue is the most full
			if (item < minItems) {
				minItems = item;
				minIndex = index;
			}
			// no need to check more queues, this one is full
			if (minItems == 0)
				break;
		}

		if (minItems == 0) {
			// unlock everything (except the queue we are waiting for)
			// N.B. test: we may keep all previous locks locked, to prevent unfair outruns from foreign queues
			if (!sleep)
				minIndex = -1;
			index = 0;
			for (vector<OutputQueue*>::iterator iter = filterOutputQueue->begin(); iter != filterOutputQueue->end(); ++iter) {
				OutputQueue *q = *iter;

				if (index != minIndex)
					q->getLock()->unlock();
				index++;
			} 
			if (!sleep)
				return 0;	// all queues are unlocked

			// wait for minIndex queue to become ready (it is still locked)
			if (!(*filterOutputQueue)[minIndex]->getSyncQueue()->waitForSpaceRaw(r[minItems])) {
				(*filterOutputQueue)[minIndex]->getLock()->unlock();
				return 0;
			}

			// we must unlock minIndex queue anyway (to prevent deadlock)
			// some other thread may overtake us
			(*filterOutputQueue)[minIndex]->getLock()->unlock();

			// check again
			loop = true;
		}
	} while (loop);

	// all queues are locked and we can insert minItems elements into each of them
	for (vector<OutputQueue*>::iterator iter = filterOutputQueue->begin(); iter != filterOutputQueue->end(); ++iter) {
		OutputQueue *q = *iter;

		for (int i = 0; i < minItems; ++i) {
			int rstatus = r[i]->getStatus();
			if (q->getFilter() > 0 && rstatus != q->getFilter())
				continue;

			q->getSyncQueue()->putItemRaw(r[i], true);
		}
		q->getLock()->unlock();

	}

	return minItems;
}
