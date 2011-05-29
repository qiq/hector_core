/**
 * TestCall module.
 */
#include <config.h>

#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include "TestCall.h"
#include "TestResource.h"

using namespace std;

// sleep TIME_TICK useconds waiting for socket changes
#define DEFAULT_TIME_TICK 100*1000

Call::Call(int maxResources, int typeId) : CallProcessingEngine(maxResources, true) {
	this->typeId = typeId;
}

Resource *Call::PrepareResource(Resource *src) {
	src->SetStatus(1);
	Resource *r = Resource::GetRegistry()->AcquireResource(typeId);
	if (!r) {
		LOG4CXX_ERROR_R(logger, r, "Cannot generate resource of type: " << typeId);
		return NULL;
	}
	r->SetId(src->GetId()+10000);
	if (TestResource::IsInstance(r)) {
		TestResource *tr = static_cast<TestResource*>(r);
		char s[1024];
		snprintf(s, sizeof(s), "pe-%d", src->GetId()+10000);
		tr->SetStr(s);
	}
	r->SetAttachedResource(src);
	return r;
}

Resource *Call::FinishResource(Resource *tmp) {
	Resource *r = tmp->GetAttachedResource();

	r->SetStatus(0);

	Resource::GetRegistry()->ReleaseResource(tmp);
	return r;
}

TestCall::TestCall(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	items = 0;
	maxRequests = 1000;
	timeTick = DEFAULT_TIME_TICK;
	resourceType = NULL;
	targetEngine = NULL;

	props = new ObjectProperties<TestCall>(this);
	props->Add("items", &TestCall::GetItems);
	props->Add("maxRequests", &TestCall::GetMaxRequests, &TestCall::SetMaxRequests, true);
	props->Add("timeTick", &TestCall::GetTimeTick, &TestCall::SetTimeTick);
	props->Add("resourceType", &TestCall::GetResourceType, &TestCall::SetResourceType, true);
	props->Add("targetEngine", &TestCall::GetTargetEngine, &TestCall::SetTargetEngine);
}

TestCall::~TestCall() {
	free(targetEngine);
	free(resourceType);
	delete props;
	delete call;
}

char *TestCall::GetItems(const char *name) {
	return int2str(items);
}

char *TestCall::GetMaxRequests(const char *name) {
	return int2str(maxRequests);
}

void TestCall::SetMaxRequests(const char *name, const char *value) {
	maxRequests = str2int(value);
}

char *TestCall::GetTimeTick(const char *name) {
	return int2str(timeTick);
}

void TestCall::SetTimeTick(const char *name, const char *value) {
	timeTick = str2long(value);
}

char *TestCall::GetResourceType(const char *name) {
	return resourceType ? strdup(resourceType) : NULL;
}

void TestCall::SetResourceType(const char *name, const char *value) {
	free(resourceType);
	resourceType = strdup(value);
}

char *TestCall::GetTargetEngine(const char *name) {
	return targetEngine ? strdup(targetEngine) : NULL;
}

void TestCall::SetTargetEngine(const char *name, const char *value) {
	free(targetEngine);
	targetEngine = strdup(value);
}

bool TestCall::Init(vector<pair<string, string> > *params) {
	// second stage?
	if (!params) {
		ProcessingEngine *engine = dynamic_cast<ProcessingEngine*>(objects->GetObject(targetEngine));
		if (!engine) {
			LOG_ERROR(this, "Invalid targetEngine parameter" << targetEngine);
			return false;
		}
		call->SetProcessingEngine(engine);
		return true;
	}

	if (!props->InitProperties(params))
		return false;

	if (maxRequests <= 0) {
		LOG_ERROR(this, "Invalid maxRequests value: " << maxRequests);
		return false;
	}

	if (!targetEngine || strlen(targetEngine) == 0) {
		LOG_ERROR(this, "targetEngine parameter missing");
		return false;
	}

	if (!resourceType) {
		LOG_ERROR(this, "resourceType is not defined");
		return false;
	}
	int typeId = Resource::GetRegistry()->NameToId(resourceType);
	if (typeId < 0) {
		LOG_ERROR(this, "Cannot load " << resourceType << " library");
		return false;
	}

	call = new Call(maxRequests, typeId);

	return true;
}

bool TestCall::ProcessMultiSync(queue<Resource*> *inputResources, queue<Resource*> *outputResources, int *expectingResources, int *processingResources) {
	int processing = call->Process(inputResources, outputResources, expectingResources, timeTick);
	if (processingResources)
		*processingResources = processing;
	return processing > 0;
}

// factory functions

extern "C" Module* hector_module_create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return new TestCall(objects, id, threadIndex);
}
