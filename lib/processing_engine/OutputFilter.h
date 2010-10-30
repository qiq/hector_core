// This class represents one nextProcessor element

#ifndef _LIB_PROCESSING_ENGINE_OUTPUTFILTER_H_
#define _LIB_PROCESSING_ENGINE_OUTPUTFILTER_H_

#include "Resource.h"
#include "SyncQueue.h"

class OutputFilter {
	int filter;
	bool emptyFilter;
	int priority;
	char *processor;
	bool copy;
	SyncQueue<Resource> *queue;
public:
	OutputFilter();
	~OutputFilter();

	int getFilter();
	bool isEmptyFilter();
	void setFilter(int filter);
	int getPriority();
	void setPriority(int priority);
	const char *getProcessor();
	void setProcessor(const char *ref);
	bool getCopy();
	void setCopy(bool copy);
	SyncQueue<Resource> *getQueue();
	void setQueue(SyncQueue<Resource> *queue);
	bool processResource(Resource *r, struct timeval *timeout);
};

inline OutputFilter::OutputFilter() {
	filter = 0;
	emptyFilter = true;
	priority = 0;
	processor = NULL;
	copy = false;
	queue = NULL;
}

inline OutputFilter::~OutputFilter() {
	free(processor);
}

inline int OutputFilter::getFilter() {
	return filter;
}

inline bool OutputFilter::isEmptyFilter() {
	return emptyFilter;
}

inline void OutputFilter::setFilter(int filter) {
	this->filter = filter;
	emptyFilter = false;
}

inline int OutputFilter::getPriority() {
	return priority;
}

inline void OutputFilter::setPriority(int priority) {
	this->priority = priority;
}

inline const char *OutputFilter::getProcessor() {
	return processor;
}

inline void OutputFilter::setProcessor(const char *processor) {
	this->processor = processor ? strdup(processor) : NULL;
}

inline bool OutputFilter::getCopy() {
	return copy;
}

inline void OutputFilter::setCopy(bool copy) {
	this->copy = copy;
}

inline SyncQueue<Resource> *OutputFilter::getQueue() {
	return queue;
}

inline void OutputFilter::setQueue(SyncQueue<Resource> *queue) {
	this->queue = queue;
}

inline bool OutputFilter::processResource(Resource *r, struct timeval *timeout) {
	return queue->putItem(r, timeout, priority);
}

#endif
