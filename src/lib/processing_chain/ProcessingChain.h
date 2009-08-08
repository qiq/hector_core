/**
 * Processing chain: call several modules
 */

#ifndef _PROCESSING_CHAIN_H_
#define _PROCESSING_CHAIN_H_

#include <config.h>

#include <vector>
#include <log4cxx/logger.h>
#include "common.h"
#include "Object.h"
#include "ObjectRegistry.h"
#include "Processor.h"
#include "Queue.h"

class ProcessingChain : public Object {
	vector<Processor*> processors;
	vector<Queue*> queues;

	static log4cxx::LoggerPtr logger;
public:
	ProcessingChain(ObjectRegistry *objects, const char *id);
	~ProcessingChain();
	bool Init(Config *config);
	void Start();
	void Stop();
	void createCheckpoint();

	const char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
};

#endif
