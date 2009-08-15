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
//TODO
// - lock everything + count minItems + minSize, co se vejde do vsech front (zapamatuj si i index fronty s nejmensim)
// - zjisti, kolika prvnim n resources to odpovida
// - uspi se na nejvetsi fronte, pokud mas spat (jinak vrat 0) a nevejde se tam ani jeden resource
// - ^^ v cyklu
// - projdi zase vsechny fronty a pridej prvnich n resources
//
//	if (simpleOutputQueue) {
//		for (int i = 0; i < size; i++) {
//			int status = r->getStatus();
//			if (simpleFilter > 0 && status != simpleFilter)
//				continue;
//			return simpleOutputQueue->putItem(r, sleep);
//		return true;
//	}
//	if (simpleOutputQueue)
//		return simpleOutputQueue->putItems(r, size, sleep);
}
