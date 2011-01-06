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
	src->setStatus(1);
	Resource *r = Resource::AcquireResource(typeId);
	if (!r) {
		LOG4CXX_ERROR_R(logger, r, "Cannot generate resource of type: " << typeId);
		return NULL;
	}
	r->setId(src->getId()+10000);
	if (r->getTypeId() == TestResource::typeId) {
		TestResource *tr = static_cast<TestResource*>(r);
		char s[1024];
		snprintf(s, sizeof(s), "pe-%d", src->getId()+10000);
		tr->setStr(s);
	}
	r->setAttachedResource(src);
	return r;
}

Resource *Call::FinishResource(Resource *tmp) {
	Resource *r = tmp->getAttachedResource();

	r->setStatus(0);

	Resource::ReleaseResource(tmp);
	return r;
}

TestCall::TestCall(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	items = 0;
	maxRequests = 1000;
	timeTick = DEFAULT_TIME_TICK;
	resourceType = NULL;
	targetEngine = NULL;

	values = new ObjectValues<TestCall>(this);
	values->addGetter("items", &TestCall::getItems);
	values->addGetter("maxRequests", &TestCall::getMaxRequests);
	values->addSetter("maxRequests", &TestCall::setMaxRequests, true);
	values->addGetter("timeTick", &TestCall::getTimeTick);
	values->addSetter("timeTick", &TestCall::setTimeTick);
	values->addGetter("resourceType", &TestCall::getResourceType);
	values->addSetter("resourceType", &TestCall::setResourceType, true);
	values->addGetter("targetEngine", &TestCall::getTargetEngine);
	values->addSetter("targetEngine", &TestCall::setTargetEngine);
}

TestCall::~TestCall() {
	delete resourceType;
	delete values;
	delete call;
}

char *TestCall::getItems(const char *name) {
	return int2str(items);
}

char *TestCall::getMaxRequests(const char *name) {
	return int2str(maxRequests);
}

void TestCall::setMaxRequests(const char *name, const char *value) {
	maxRequests = str2int(value);
}

char *TestCall::getTimeTick(const char *name) {
	return int2str(timeTick);
}

void TestCall::setTimeTick(const char *name, const char *value) {
	timeTick = str2long(value);
}

char *TestCall::getResourceType(const char *name) {
	return resourceType ? strdup(resourceType) : NULL;
}

void TestCall::setResourceType(const char *name, const char *value) {
	free(resourceType);
	resourceType = strdup(value);
}

char *TestCall::getTargetEngine(const char *name) {
	return targetEngine ? strdup(targetEngine) : NULL;
}

void TestCall::setTargetEngine(const char *name, const char *value) {
	free(targetEngine);
	targetEngine = strdup(value);
}

bool TestCall::Init(vector<pair<string, string> > *params) {
	// second stage?
	if (!params) {
		ProcessingEngine *engine = dynamic_cast<ProcessingEngine*>(objects->getObject(targetEngine));
		if (!engine) {
			LOG_ERROR(this, "Invalid targetEngine parameter" << targetEngine);
			return false;
		}
		call->setProcessingEngine(engine);
		return true;
	}

	if (!values->InitValues(params))
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
	int typeId = Resource::NameToId(resourceType);
	if (typeId < 0) {
		LOG_ERROR(this, "Cannot load " << resourceType << " library");
		return false;
	}

	call = new Call(maxRequests, typeId);

	return true;
}

int TestCall::ProcessMulti(queue<Resource*> *inputResources, queue<Resource*> *outputResources, int *expectingResources) {
	ObjectLockRead();
	int tick = timeTick;
	ObjectUnlock();
	return call->Process(inputResources, outputResources, expectingResources, tick);
}

// factory functions

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return new TestCall(objects, id, threadIndex);
}
