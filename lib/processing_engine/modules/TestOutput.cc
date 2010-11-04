/**
 * Test module.
 */
#include <config.h>

#include "TestOutput.h"
#include "TestResource.h"

using namespace std;

TestOutput::TestOutput(ObjectRegistry *objects, ProcessingEngine *engine, const char *id, int threadIndex): Module(objects, engine, id, threadIndex) {
	items = 0;

	values = new ObjectValues<TestOutput>(this);
	values->addGetter("items", &TestOutput::getItems);
}

TestOutput::~TestOutput() {
	delete values;
}

char *TestOutput::getItems(const char *name) {
	return int2str(items);
}

bool TestOutput::Init(vector<pair<string, string> > *params) {
	values->InitValues(params);
	return true;
}

void TestOutput::ProcessOutput(Resource *resource) {
	if (resource->getTypeId() == TestResource::typeId) {
		TestResource *tr = static_cast<TestResource*>(resource);
		LOG_INFO("Resource arrived (" << tr->getStr() << ")");
		delete tr;
	} else {
		LOG_INFO("Resource arrived (" << resource->getId() << ")");
		delete resource;
	}
	ObjectLockWrite();
	items++;
	ObjectUnlock();
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, ProcessingEngine *engine, const char *id, int threadIndex) {
	return (Module*)new TestOutput(objects, engine, id, threadIndex);
}
