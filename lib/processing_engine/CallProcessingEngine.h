/**
 * CallProcessingEngine: call another processing engine module template. Module
 * needs to implement PrepareResource and FinishResource methods and call ReadWrite() as appropriate
 */

#ifndef _LIB_PROCESSING_ENGINE_CALL_PROCESSING_ENGINE_H_
#define _LIB_PROCESSING_ENGINE_CALL_PROCESSING_ENGINE_H_

#include <config.h>

#include <tr1/unordered_map>
#include <log4cxx/logger.h>
#include "common.h"
#include "Module.h"
#include "ObjectValues.h"
#include "ProcessingEngine.h"

class CallProcessingEngine {
public:
	typedef struct CallResourceInfo_ {
		Resource *src;	// currently processed resource
		Resource *tmp;	// calling processing engine with this resource
	} CallResourceInfo;

	// maxRequests: number of concurrent requests
	CallProcessingEngine(ProcessingEngine *engine, int maxRequests);
	~CallProcessingEngine();
	int Process(std::queue<Resource*> *inputResources, std::queue<Resource*> *outputResources, int timeTick);
	void Pass(std::queue<Resource*> *inputResources, int timeTick);
	int ProcessingResources();

	// may be called instead of ProcessMulti()
	bool ReadWrite(std::queue<Resource*> *inputResources, std::queue<Resource*> *outputResources, struct timeval *tv);

protected:
	// returns NULL in case src resource cannot be processed (e.g. is incompatible)
	virtual Resource *PrepareResource(Resource *src) = 0;
	virtual void FinishResource(Resource *src, Resource *tmp) = 0;

	int maxRequests;
	ProcessingEngine *engine;
	CallResourceInfo *tmpInputResource;
	std::vector<CallResourceInfo*> unused;
	std::tr1::unordered_map<int, CallResourceInfo*> running;

	static log4cxx::LoggerPtr logger;
};

#endif
