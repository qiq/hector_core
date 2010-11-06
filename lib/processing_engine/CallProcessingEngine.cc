/**
 * CallProcessingEngine.
 */
#include <config.h>

#include <string.h>
#include "CallProcessingEngine.h"

using namespace std;

log4cxx::LoggerPtr CallProcessingEngine::logger(log4cxx::Logger::getLogger("CallProcessingEngine"));

CallProcessingEngine::CallProcessingEngine(int maxRequests, int timeTick, ProcessingEngine *engine) {
	this->maxRequests = maxRequests;
	this->timeTick = timeTick;
	this->engine = engine;
	tmpInputResource = NULL;

	for (int i = 0; i < maxRequests; i++) {
		CallResourceInfo *ri = new CallResourceInfo();
		unused.push_back(ri);
	}
}

CallProcessingEngine::~CallProcessingEngine() {
	for (vector<CallResourceInfo*>::iterator iter = unused.begin(); iter != unused.end(); ++iter)
		delete *iter;
}

bool CallProcessingEngine::ReadWrite(queue<Resource*> *inputResources, queue<Resource*> *outputResources, struct timeval *tv) {
	// status of some resource changes (written or read at least one resource)
	bool changed = false;

	// stored input resource
	bool processInput = true;
	if (tmpInputResource && running.size() <= maxRequests) {
		if (engine->ProcessResource(tmpInputResource->tmp, running.size() == 0 ? tv : NULL)) {
			running[tmpInputResource->tmp->getId()] = tmpInputResource;
			tmpInputResource = NULL;
			changed = true;
		} else {
			// cannot put stored input resource, no way to put any other resource
			processInput = false;
		}
	}

	// get input resources and start resolution for them
	while (processInput && inputResources->size() > 0 && running.size() < maxRequests) {
		Resource *src = inputResources->front();
		Resource *tmp = PrepareResource(src);
		if (tmp) {
			tmpInputResource = unused.back();
			unused.pop_back();
			tmpInputResource->src = src;
			tmpInputResource->tmp = tmp;
			if (engine->ProcessResource(tmp, running.size() == 0 ? tv : NULL)) {
				running[tmp->getId()] = tmpInputResource;
				tmpInputResource = NULL;
				changed = true;
			} else {
				// cancel or PE input queue is full: keep resource in tmpInputResource and stop trying
				processInput = false;
			}
		} else {
			// cannot process this resource
			outputResources->push(src);
		}
		inputResources->pop();
	}

	if (running.size() == 0)
		return changed;

	// read resources in non-blocking mode
	bool processOutput = true;
	while (processOutput && running.size() > 0) {
		for (std::tr1::unordered_map<int, CallResourceInfo*>::iterator iter = running.begin(); iter != running.end(); ++iter) {
			int id = iter->second->tmp->getId();
			Resource *r = engine->GetProcessedResource(id, tv);
			if (r) {
				// finish processed resource
				std::tr1::unordered_map<int, CallResourceInfo*>::iterator iter = running.find(id);
				assert(iter != running.end());
				CallResourceInfo *ri = iter->second;
				FinishResource(ri->src, r);
				outputResources->push(ri->src);
				delete r;
				unused.push_back(iter->second);
				running.erase(id);

				changed = true;
			} else {
				// cancelled or timeout
				processOutput = false;
				break;
			}
		}
	}
	return changed;
}

int CallProcessingEngine::ProcessMulti(queue<Resource*> *inputResources, queue<Resource*> *outputResources) {
	struct timeval tv = ConstructTimeout(timeTick);

	// do not block
	while (ReadWrite(inputResources, outputResources, NULL))
		;
	// nothing to process
	if (running.size() == 0)
		return maxRequests;
	// block in read (once)
	ReadWrite(inputResources, outputResources, &tv);
	// do not block
	while (ReadWrite(inputResources, outputResources, NULL))
		;

	// finished resources are already appended to the outputResources queue
	return maxRequests-running.size()-(tmpInputResource ? 1 : 0);
}

int CallProcessingEngine::ProcessingResources() {
	return running.size();
}
