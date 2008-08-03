/**
 * Processing chain: call several modules
 */

#ifndef _PROCESSING_CHAIN_H_
#define _PROCESSING_CHAIN_H_

#include <vector>
#include "Processor.h"
#include "Resource.h"
#include "ResourceQueue.h"

class ProcessingChain {
	vector<Processor*> processors;
	vector<ResourceQueue*> queues;
	int queueSize;

	bool running;
public:
	ProcessingChain();
	~ProcessingChain();
	bool Init(Config *config, const char *name);
	void Start();
	void Stop();
};

#endif
