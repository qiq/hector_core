/**
 * TestCall: translate DNS name to IP address
 */

#ifndef _LIB_PROCESSING_ENGINE_MODULES_TEST_CALL_H_
#define _LIB_PROCESSING_ENGINE_MODULES_TEST_CALL_H_

#include <config.h>

#include <tr1/unordered_map>
#include "CallProcessingEngine.h"
#include "common.h"
#include "Module.h"
#include "ObjectValues.h"
#include "ProcessingEngine.h"

class Call : public CallProcessingEngine {
public:
	Call(int maxResources, int typeId);
	~Call() {};

protected:	
	Resource *PrepareResource(Resource *src);
	Resource *FinishResource(Resource *tmp);

private:
	int typeId;
};

class TestCall : public Module {
public:
	TestCall(ObjectRegistry *objects, const char *id, int threadIndex);
	~TestCall();
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	Module::Type getType();
	int ProcessMulti(std::queue<Resource*> *inputResources, std::queue<Resource*> *outputResources);
	int ProcessingResources();

private:
	int items;		// ObjectLock, items processed
	int maxRequests;	// initOnly, number of concurrent requests
	int timeTick;		// ObjectLock, max time to spend in ProcessMulti()
	char *resourceType;	// init only, type of resource to generate
	char *targetEngine;	// init-only, read-only

	ObjectValues<TestCall> *values;

	Call *call;

	char *getItems(const char *name);
	char *getMaxRequests(const char *name);
	void setMaxRequests(const char *name, const char *value);
	char *getTimeTick(const char *name);
	void setTimeTick(const char *name, const char *value);
	char *getResourceType(const char *name);
	void setResourceType(const char *name, const char *value);
	char *getTargetEngine(const char *name);
	void setTargetEngine(const char *name, const char *value);

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	bool isInitOnly(const char *name);
	std::vector<std::string> *listNamesSync();
};

inline Module::Type TestCall::getType() {
	return MULTI;
}

inline char *TestCall::getValueSync(const char *name) {
	return values->getValueSync(name);
}

inline bool TestCall::setValueSync(const char *name, const char *value) {
	return values->setValueSync(name, value);
}

inline bool TestCall::isInitOnly(const char *name) {
	return values->isInitOnly(name);
}

inline std::vector<std::string> *TestCall::listNamesSync() {
	return values->listNamesSync();
}

#endif
