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
#include "ObjectProperties.h"
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
	Module::Type GetType();
	bool ProcessMultiSync(std::queue<Resource*> *inputResources, std::queue<Resource*> *outputResources, int *expectingResources, int *processingResources);

private:
	int items;		// ObjectLock, items processed
	int maxRequests;	// initOnly, number of concurrent requests
	int timeTick;		// ObjectLock, max time to spend in ProcessMulti()
	char *resourceType;	// init only, type of resource to generate
	char *targetEngine;	// init-only, read-only

	char *GetItems(const char *name);
	char *GetMaxRequests(const char *name);
	void SetMaxRequests(const char *name, const char *value);
	char *GetTimeTick(const char *name);
	void SetTimeTick(const char *name, const char *value);
	char *GetResourceType(const char *name);
	void SetResourceType(const char *name, const char *value);
	char *GetTargetEngine(const char *name);
	void SetTargetEngine(const char *name, const char *value);

	ObjectProperties<TestCall> *props;
	char *GetPropertySync(const char *name);
	bool SetPropertySync(const char *name, const char *value);
	std::vector<std::string> *ListPropertiesSync();

	Call *call;
};

inline Module::Type TestCall::GetType() {
	return MULTI;
}

inline char *TestCall::GetPropertySync(const char *name) {
	return props->GetProperty(name);
}

inline bool TestCall::SetPropertySync(const char *name, const char *value) {
	return props->SetProperty(name, value);
}

inline std::vector<std::string> *TestCall::ListPropertiesSync() {
	return props->ListProperties();
}

#endif
