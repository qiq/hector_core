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

	int GetFilter();
	bool IsEmptyFilter();
	void SetFilter(int filter);
	int GetPriority();
	void SetPriority(int priority);
	const char *GetProcessor();
	void SetProcessor(const char *ref);
	bool GetCopy();
	void SetCopy(bool copy);
	SyncQueue<Resource> *GetQueue();
	void SetQueue(SyncQueue<Resource> *queue);
	bool ProcessResource(Resource *r, struct timeval *timeout);
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

inline int OutputFilter::GetFilter() {
	return filter;
}

inline bool OutputFilter::IsEmptyFilter() {
	return emptyFilter;
}

inline void OutputFilter::SetFilter(int filter) {
	this->filter = filter;
	emptyFilter = false;
}

inline int OutputFilter::GetPriority() {
	return priority;
}

inline void OutputFilter::SetPriority(int priority) {
	this->priority = priority;
}

inline const char *OutputFilter::GetProcessor() {
	return processor;
}

inline void OutputFilter::SetProcessor(const char *processor) {
	this->processor = processor ? strdup(processor) : NULL;
}

inline bool OutputFilter::GetCopy() {
	return copy;
}

inline void OutputFilter::SetCopy(bool copy) {
	this->copy = copy;
}

inline SyncQueue<Resource> *OutputFilter::GetQueue() {
	return queue;
}

inline void OutputFilter::SetQueue(SyncQueue<Resource> *queue) {
	this->queue = queue;
}

inline bool OutputFilter::ProcessResource(Resource *r, struct timeval *timeout) {
	return queue->PutItem(r, timeout, priority);
}

#endif
