/**
 * CallProcessingEngine.
 */
#include <config.h>

#include <assert.h>
#include <string.h>
#include "CallProcessingEngine.h"

using namespace std;

log4cxx::LoggerPtr CallProcessingEngine::logger(log4cxx::Logger::getLogger("CallProcessingEngine"));

CallProcessingEngine::CallProcessingEngine(int maxRequests) {
	this->maxRequests = maxRequests;
	tmpInputResource = NULL;
}

CallProcessingEngine::~CallProcessingEngine() {
}

bool CallProcessingEngine::ReadWrite(queue<Resource*> *inputResources, queue<Resource*> *outputResources, struct timeval *tv) {
	// status of some resource changes (written or read at least one resource)
	bool changed = false;

	// stored input resource
	bool processInput = true;
	if (tmpInputResource && running.size() <= maxRequests) {
		if (engine->ProcessResource(tmpInputResource, running.size() == 0 ? tv : NULL)) {
			running.insert(tmpInputResource->getId());
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
		tmpInputResource = PrepareResource(src);
		if (tmpInputResource) {
			if (engine->ProcessResource(tmpInputResource, running.size() == 0 ? tv : NULL)) {
				running.insert(tmpInputResource->getId());
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
	bool processOutput = outputResources ? true : false;
	while (processOutput && running.size() > 0) {
		vector<int> erase;
		for (tr1::unordered_set<int>::iterator iter = running.begin(); iter != running.end(); ++iter) {
			Resource *r = engine->GetProcessedResource(*iter, tv);
			if (r) {
				// finish processed resource
				outputResources->push(FinishResource(r));
				erase.push_back(*iter);

				changed = true;
			} else {
				// cancelled or timeout
				processOutput = false;
				break;
			}
		}
		for (vector<int>::iterator iter = erase.begin(); iter != erase.end(); ++iter) {
			running.erase(*iter);
		}
	}
	return changed;
}

int CallProcessingEngine::Process(queue<Resource*> *inputResources, queue<Resource*> *outputResources, int *expectingResources, int timeTick) {
	assert(engine->getOutputQueue() != NULL);
	struct timeval tv = ConstructTimeout(timeTick);

	// do not block
	while (ReadWrite(inputResources, outputResources, NULL))
		;
	// nothing to process
	if (running.size() == 0) {
		if (expectingResources)
			*expectingResources = maxRequests-(tmpInputResource ? 1 : 0);
		return 0;
	}
	// block in read (once)
	ReadWrite(inputResources, outputResources, &tv);
	// do not block
	while (ReadWrite(inputResources, outputResources, NULL))
		;

	// finished resources are already appended to the outputResources queue
	if (expectingResources)
		*expectingResources = maxRequests-running.size()-(tmpInputResource ? 1 : 0);
	return running.size();
}

void CallProcessingEngine::Pass(queue<Resource*> *inputResources, int timeTick) {
	assert(engine->getOutputQueue() == NULL);
	struct timeval tv = ConstructTimeout(timeTick);

	ReadWrite(inputResources, NULL, &tv);
}
