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
	char *s;

	// output queue(s)
	snprintf(buffer, sizeof(buffer), "/Config/Processor[@id='%s']/outputQueue/@ref", getId());
	v = config->getValues(buffer);
	int i = 1;
	if (v) {
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
			outputQueues.push_back(oq);
			i++;
		}
		delete v;
	}

	// sort queues
	// TODO

	return true;
}

void FilterQueue::Start() {
}

void FilterQueue::Stop() {
	queue->cancelAll();
}

Resource *FilterQueue::getResource(bool wait) {
	return simpleInputQueue ? simpleInputQueue->getResource(wait) : priorityInputQueue->getItem(wait);
}

int FilterQueue::getResources(Resource **r, int size, bool wait) {
	return simpleInputQueue ? simpleInputQueue->getResources(r, size, wait) : priorityInputQueue->getItems(r, size, wait);
}
