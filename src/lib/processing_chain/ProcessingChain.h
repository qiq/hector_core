/**
 * Processing chain: call several modules
 */

#ifndef _PROCESSING_CHAIN_H_
#define _PROCESSING_CHAIN_H_

#include <config.h>

#include <string>
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

	Lock propertyLock;
	bool running;

	stdext::hash_map<string, char*(ProcessingChain::*)(), string_hash> getters;
	stdext::hash_map<string, void(ProcessingChain::*)(const char*), string_hash> setters;

	char *getRunning();
	void setRunning(const char *value);

	static log4cxx::LoggerPtr logger;
public:
	ProcessingChain(ObjectRegistry *objects, const char *id);
	~ProcessingChain();
	bool init(Config *config);
	void start();
	void stop();
	void createCheckpoint();

	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
};

#endif
