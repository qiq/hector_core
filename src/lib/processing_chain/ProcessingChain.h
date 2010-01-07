/**
 * Processing chain: call several modules
 */

#ifndef _PROCESSING_CHAIN_H_
#define _PROCESSING_CHAIN_H_

#include <config.h>

#include <string>
#include <vector>
#include <log4cxx/logger.h>
#include <tr1/unordered_map>
#include "common.h"
#include "Object.h"
#include "ObjectRegistry.h"
#include "Processor.h"

class ProcessingChain : public Object {
	vector<Processor*> processors;

	Lock propertyLock;
	bool propRun;
	bool propPause;

	std::tr1::unordered_map<string, char*(ProcessingChain::*)()> getters;
	std::tr1::unordered_map<string, void(ProcessingChain::*)(const char*)> setters;

	char *getRun();
	void setRun(const char *value);
	char *getPause();
	void setPause(const char *value);

	void doPause();
	void doResume();

	static log4cxx::LoggerPtr logger;
public:
	ProcessingChain(ObjectRegistry *objects, const char *id);
	~ProcessingChain();
	bool init(Config *config);
	void start();
	void stop();
	void pause();
	void resume();
	void createCheckpoint();

	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
};

#endif
