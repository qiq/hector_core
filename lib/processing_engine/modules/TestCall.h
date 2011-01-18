/**
TestCall.la, multi, native
Call another processing engine.

Dependencies: none

Parameters:
items		r/o	Total items processed
maxRequests	init	Number of concurrent requests
timeTick	r/w	Max time to spend in ProcessMulti()
resourceType;	init	Type of resource to generate
targetEngine;	init	engine to be called
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
	int ProcessMulti(std::queue<Resource*> *inputResources, std::queue<Resource*> *outputResources, int *expectingResources);

private:
	int items;		// ObjectLock, items processed
	int maxRequests;	// initOnly, number of concurrent requests
	int timeTick;		// ObjectLock, max time to spend in ProcessMulti()
	char *resourceType;	// init only, type of resource to generate
	char *targetEngine;	// init-only, read-only

	char *getItems(const char *name);
	char *getMaxRequests(const char *name);
	void setMaxRequests(const char *name, const char *value);
	char *getTimeTick(const char *name);
	void setTimeTick(const char *name, const char *value);
	char *getResourceType(const char *name);
	void setResourceType(const char *name, const char *value);
	char *getTargetEngine(const char *name);
	void setTargetEngine(const char *name, const char *value);

	ObjectValues<TestCall> *values;
	char *GetValue(const char *name);
	bool SetValue(const char *name, const char *value);
	bool IsInitOnly(const char *name);
	std::vector<std::string> *ListNames();

	Call *call;
};

inline Module::Type TestCall::getType() {
	return MULTI;
}

inline char *TestCall::GetValue(const char *name) {
	return values->GetValue(name);
}

inline bool TestCall::SetValue(const char *name, const char *value) {
	return values->SetValue(name, value);
}

inline bool TestCall::IsInitOnly(const char *name) {
	return values->IsInitOnly(name);
}

inline std::vector<std::string> *TestCall::ListNames() {
	return values->ListNames();
}

#endif
