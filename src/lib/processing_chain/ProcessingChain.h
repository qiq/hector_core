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
#include "Processor.h"
#include "Queue.h"

class Server;

class ProcessingChain : public Object {
	vector<Processor*> processors;
	vector<Queue*> queues;

	Server *server;

	static log4cxx::LoggerPtr logger;
public:
	ProcessingChain();
	~ProcessingChain();
	bool Init(Server *server, Config *config, const char *id);
	void Start();
	void Stop();
	void createCheckpoint();

	const char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
};

#endif
