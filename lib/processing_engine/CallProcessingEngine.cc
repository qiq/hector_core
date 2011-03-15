/**
 * CallProcessingEngine.
 */
#include <config.h>

#include <assert.h>
#include <string.h>
#include "CallProcessingEngine.h"

using namespace std;

log4cxx::LoggerPtr CallProcessingEngine::logger(log4cxx::Logger::getLogger("CallProcessingEngine"));

CallProcessingEngine::CallProcessingEngine(int maxRequests, bool ordered) {
	this->maxRequests = maxRequests;
	tmpInputResource = NULL;
	this->ordered = ordered;
	running = 0;
}

CallProcessingEngine::~CallProcessingEngine() {
}

bool CallProcessingEngine::ReadWrite(queue<Resource*> *inputResources, queue<Resource*> *outputResources, struct timeval *tv) {
	// status of some resource changes (written or read at least one resource)
	bool changed = false;

	// stored input resource
	bool processInput = true;
	if (tmpInputResource && running <= maxRequests) {
		if (engine->ProcessResource(tmpInputResource, running == 0 ? tv : NULL)) {
			if (ordered)
				runningSet.insert(tmpInputResource->GetId());
			else
				runningVector.push_back(tmpInputResource->GetId());
			running++;
			tmpInputResource = NULL;
			changed = true;
		} else {
			// cannot put stored input resource, no way to put any other resource
			processInput = false;
		}
	}

	// get input resources and start resolution for them
	while (processInput && inputResources->size() > 0 && running < maxRequests) {
		Resource *src = inputResources->front();
		tmpInputResource = PrepareResource(src);
		if (tmpInputResource) {
			if (engine->ProcessResource(tmpInputResource, running == 0 ? tv : NULL)) {
				if (ordered)
					runningSet.insert(tmpInputResource->GetId());
				else
					runningVector.push_back(tmpInputResource->GetId());
				running++;
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

	if (running == 0)
		return changed;

	// read resources in non-blocking mode
	if (!outputResources)
		return changed;
	vector<Resource*> finished;
	if (ordered) {
		engine->GetProcessedResources(&runningSet, &finished, tv);
		running = runningSet.size();
	} else {
		engine->GetProcessedResourcesOrdered(&runningVector, &finished, tv);
		running = runningVector.size();
	}
	for (int i = 0; i < (int)finished.size(); i++) {
		// finish processed resource
		outputResources->push(FinishResource(finished[i]));
		changed = true;
	}
	return changed;
}

int CallProcessingEngine::Process(queue<Resource*> *inputResources, queue<Resource*> *outputResources, int *expectingResources, int timeTick) {
	assert(engine->GetOutputQueue() != NULL);
	struct timeval tv = ConstructTimeout(timeTick);

	// do not block
	while (ReadWrite(inputResources, outputResources, NULL))
		;
	// nothing to process
	if (running == 0) {
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
		*expectingResources = maxRequests-running-(tmpInputResource ? 1 : 0);
	return running;
}

void CallProcessingEngine::Pass(queue<Resource*> *inputResources, int timeTick) {
	assert(engine->GetOutputQueue() == NULL);
	struct timeval tv = ConstructTimeout(timeTick);

	ReadWrite(inputResources, NULL, &tv);
}
