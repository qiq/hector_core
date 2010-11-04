/**
 * CallProcessingEngine module.
 */
#include <config.h>

#include <string.h>
#include "CallProcessingEngineModule.h"
#include "ProcessingEngine.h"

using namespace std;

// sleep TIME_TICK useconds waiting for socket changes
#define DEFAULT_TIME_TICK 100*1000

CallProcessingEngineModule::CallProcessingEngineModule(ObjectRegistry *objects, ProcessingEngine *engine, const char *id, int threadIndex): Module(objects, engine, id, threadIndex) {
	items = 0;
	maxRequests = 1000;
	timeTick = DEFAULT_TIME_TICK;

	tmpInputResource = NULL;

	values = new ObjectValues<CallProcessingEngineModule>(this);
	values->addGetter("items", &CallProcessingEngineModule::getItems);
	values->addGetter("maxRequests", &CallProcessingEngineModule::getMaxRequests);
	values->addSetter("maxRequests", &CallProcessingEngineModule::setMaxRequests, true);
	values->addGetter("timeTick", &CallProcessingEngineModule::getTimeTick);
	values->addSetter("timeTick", &CallProcessingEngineModule::setTimeTick);
}

CallProcessingEngineModule::~CallProcessingEngineModule() {
	delete values;
}

char *CallProcessingEngineModule::getItems(const char *name) {
	return int2str(items);
}

char *CallProcessingEngineModule::getMaxRequests(const char *name) {
	return int2str(maxRequests);
}

void CallProcessingEngineModule::setMaxRequests(const char *name, const char *value) {
	maxRequests = str2int(value);
}

char *CallProcessingEngineModule::getTimeTick(const char *name) {
	return int2str(timeTick);
}

void CallProcessingEngineModule::setTimeTick(const char *name, const char *value) {
	timeTick = str2long(value);
}

bool CallProcessingEngineModule::Init(vector<pair<string, string> > *params) {
	if (!values->InitValues(params, true))
		return false;

	if (maxRequests <= 0) {
		LOG_ERROR("Invalid maxRequests value: " << maxRequests);
		return false;
	}

	string name;
	for (vector<pair<string, string> >::iterator iter = params->begin(); iter != params->end(); ++iter) {
		if (iter->first == "targetEngine") {
			name = iter->second;
			break;
		}
	}
	if (name.length() == 0) {
		LOG_ERROR("targetEngine parameter missing");
		return false;
	}

	targetEngine = dynamic_cast<ProcessingEngine*>(objects->getObject(name.c_str()));
	if (!targetEngine) {
		LOG_ERROR("Invalid targetEngine parameter" << name);
		return false;
	}

	for (int i = 0; i < maxRequests; i++) {
		CallResourceInfo *ri = new CallResourceInfo();
		unused.push_back(ri);
	}

	return true;
}

bool CallProcessingEngineModule::ReadWrite(queue<Resource*> *inputResources, queue<Resource*> *outputResources, struct timeval *tv) {
	// status of some resource changes (written or read at least one resource)
	bool changed = false;

	// stored input resource
	bool processInput = true;
	if (tmpInputResource && running.size() <= maxRequests) {
		if (targetEngine->ProcessResource(tmpInputResource->qr, running.size() == 0 ? tv : NULL)) {
			running[tmpInputResource->qr->getId()] = tmpInputResource;
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
		Resource *qr = PrepareResource(src);
		if (qr) {
			tmpInputResource = unused.back();
			unused.pop_back();
			tmpInputResource->src = src;
			tmpInputResource->qr = qr;
			if (targetEngine->ProcessResource(qr, running.size() == 0 ? tv : NULL)) {
				running[qr->getId()] = tmpInputResource;
				tmpInputResource = NULL;
				changed = true;
			} else {
				// cancel or PE input queue is full: keep resource in tmpInputResource and stop trying
				processInput = false;
			}
		} else {
			// cannot process this resource
			outputResources->push(src);
			ObjectLockWrite();
			++items;
			ObjectUnlock();
		}
		inputResources->pop();
	}

	if (running.size() == 0)
		return changed;

	// read resources in non-blocking mode
	bool processOutput = true;
	while (processOutput && running.size() > 0) {
		for (std::tr1::unordered_map<int, CallResourceInfo*>::iterator iter = running.begin(); iter != running.end(); ++iter) {
			int id = iter->second->qr->getId();
			Resource *r;
			if (targetEngine->GetProcessedResource(id, &r, tv)) {
				// finish processed resource
				std::tr1::unordered_map<int, CallResourceInfo*>::iterator iter = running.find(id);
				assert(iter != running.end());
				CallResourceInfo *ri = iter->second;
				FinishResource(ri->src, r);
				outputResources->push(ri->src);
				delete r;
				unused.push_back(iter->second);
				running.erase(id);

				ObjectLockWrite();
				++items;
				ObjectUnlock();

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

int CallProcessingEngineModule::ProcessMulti(queue<Resource*> *inputResources, queue<Resource*> *outputResources) {
	ObjectLockRead();
	int tick = timeTick;
	ObjectUnlock();
	struct timeval tv = ConstructTimeout(tick);

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

int CallProcessingEngineModule::ProcessingResources() {
	return running.size();
}
