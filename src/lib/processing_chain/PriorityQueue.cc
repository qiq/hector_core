/**
 *
 */

#include <config.h>

#include "PriorityQueue.h"

log4cxx::LoggerPtr PriorityQueue::logger(log4cxx::Logger::getLogger("lib.processing_chain.PriorityQueue"));

PriorityQueue::PriorityQueue(ObjectRegistry *objects) {
	this->objects = objects;

	simpleInputQueue = NULL;
	priorityInputQueue = NULL;
	queueCount = 0;
}

PriorityQueue::~PriorityQueue() {
	delete priorityInputQueue;
}

bool PriorityQueue::init(Config *config, const char *id) {
	char buffer[1024];
	vector<string> *v;

	// input queue(s)
	snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/inputQueue/@ref", id);
	v = config->getValues(buffer);
	int i = 1;
	if (v) {
		if (v->size() > 1) {
			// one queue, no priority
			const char *qid = (*v)[0].c_str();
			snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/inputQueue[%d]/@priority", qid, i);
			char *s = config->getFirstValue(buffer);
			if (s) {
				LOG4CXX_INFO(logger, "InputQueue priority ignored: " << qid);
				return false;
			}
			free(s);

			simpleInputQueue = dynamic_cast<Queue*>(objects->getObject(qid));
			if (!simpleInputQueue) {
				LOG4CXX_ERROR(logger, "Queue not found: " << qid);
				return false;
			}
			queueCount = 1;
		} else {
			// more sub-queues, should have distinct priority
			priorityInputQueue = new PrioritySyncQueue<Resource>();
			int n = v->size();
			int i = 1;
			for (vector<string>::iterator iter = v->begin(); iter != v->end(); ++iter) {
				const char *qid = iter->c_str();
				snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/inputQueue[%d]/@priority", qid, i);
				char *s = config->getFirstValue(buffer);
				int priority = 0;
				if (s) {
					if (sscanf(s, "%d", &priority) != 1) {
						LOG4CXX_ERROR(logger, "Invalid inputQueue priority: " << s);
						return false;
					}
				}
				free(s);
				Queue *queue = dynamic_cast<Queue*>(objects->getObject(qid));
				if (!queue) {
					LOG4CXX_ERROR(logger, "Queue not found: " << qid);
					return false;
				}

				priorityInputQueue->addQueue(queue->getSyncQueue(), priority, i == n);
				++i;
			}
			queueCount = i-1;
		}
		delete v;
	}

	return true;
}

void PriorityQueue::start() {
	if (priorityInputQueue)
		priorityInputQueue->clearCancel();
}

void PriorityQueue::stop() {
	if (priorityInputQueue)
		priorityInputQueue->cancelAll();
}

int PriorityQueue::getQueueCount() {
	return queueCount;
}

Resource *PriorityQueue::getResource(bool sleep) {
	return simpleInputQueue ? simpleInputQueue->getResource(sleep) : priorityInputQueue->getItem(sleep);
}

int PriorityQueue::getResources(Resource **r, int size, bool sleep) {
	return simpleInputQueue ? simpleInputQueue->getResources(r, size, sleep) : priorityInputQueue->getItems(r, size, sleep);
}
