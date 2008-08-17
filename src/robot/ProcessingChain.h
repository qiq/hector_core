/**
 * Processing chain: call several modules
 */

#ifndef _PROCESSING_CHAIN_H_
#define _PROCESSING_CHAIN_H_

#include <log4cxx/logger.h>
#include <vector>
#include "common.h"
#include "Processor.h"
#include "Resource.h"
#include "SyncQueue.h"

class ProcessingChain {
	vector<Processor*> processors;
	vector<SyncQueue<Resource>*> queues;
	int queueSize;

	static log4cxx::LoggerPtr logger;
public:
	ProcessingChain();
	~ProcessingChain();
	bool Init(Config *config, const char *name);
	void Start();
	void Stop();
};

#endif
