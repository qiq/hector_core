/**
 * CallProcessingEngine: call another processing engine module template. Module
 * needs to implement PrepareResource and FinishResource methods and call ReadWrite() as appropriate
 */

#ifndef _LIB_PROCESSING_ENGINE_CALL_PROCESSING_ENGINE_H_
#define _LIB_PROCESSING_ENGINE_CALL_PROCESSING_ENGINE_H_

#include <config.h>

#include <tr1/unordered_set>
#include <log4cxx/logger.h>
#include "common.h"
#include "Module.h"
#include "ObjectValues.h"
#include "ProcessingEngine.h"

class CallProcessingEngine {
public:
	// maxRequests: number of concurrent requests
	CallProcessingEngine(ProcessingEngine *engine, int maxRequests);
	virtual ~CallProcessingEngine();
	int Process(std::queue<Resource*> *inputResources, std::queue<Resource*> *outputResources, int timeTick);
	void Pass(std::queue<Resource*> *inputResources, int timeTick);
	int ProcessingResources();

	// may be called instead of ProcessMulti()
	bool ReadWrite(std::queue<Resource*> *inputResources, std::queue<Resource*> *outputResources, struct timeval *tv);

protected:
	// returns NULL in case src resource cannot be processed (e.g. is incompatible)
	virtual Resource *PrepareResource(Resource *src) = 0;
	virtual Resource *FinishResource(Resource *tmp) = 0;

	int maxRequests;
	ProcessingEngine *engine;
	Resource *tmpInputResource;
	std::tr1::unordered_set<int> running;

	static log4cxx::LoggerPtr logger;
};

#endif
