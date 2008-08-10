/**
 * Processing: call modules
 */

#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_

#include <log4cxx/logger.h>
#include <pthread.h>
#include "Config.h"
#include "Resource.h"
#include "ResourceQueue.h"

class Processor {
protected:
	int nThreads;
	pthread_t *threads;
	bool running;

	static log4cxx::LoggerPtr logger;
public:
	Processor();
	virtual ~Processor();
	virtual void runThread() = 0;
	virtual bool Init(Config *config, const char *name) = 0;
	virtual void Start();
	virtual void Stop();

	void *loadLibrary(const char *lib, const char *sym);
	//TODO: init external module
};

#endif
