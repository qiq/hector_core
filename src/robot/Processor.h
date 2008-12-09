/**
 * Processing: call modules
 */

#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_

#include <log4cxx/logger.h>
#include <pthread.h>
#include "Config.h"
#include "Lock.h"

class Processor {
protected:
	int nThreads;
	pthread_t *threads;

	Lock running_lock;
	bool running;

	static log4cxx::LoggerPtr logger;
public:
	Processor();
	virtual ~Processor();
	virtual bool Running();
	virtual void runThread() = 0;
	virtual bool Init(Config *config, const char *name) = 0;
	virtual void Start();
	virtual void Stop();
	virtual void createCheckpoint() = 0;

	void *loadLibrary(const char *lib, const char *sym);
};

#endif
