/**
 * Test module.
 */
#include <config.h>

#include "TestOutput.h"
#include "TestResource.h"

using namespace std;

TestOutput::TestOutput(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
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
	// second stage?
	if (!params)
		return true;

	values->InitValues(params);
	return true;
}

void TestOutput::ProcessOutput(Resource *resource) {
	if (resource->getTypeId() == TestResource::typeId) {
		TestResource *tr = static_cast<TestResource*>(resource);
		LOG_INFO_R(tr, "Resource arrived (" << tr->getStr() << ")");
		delete tr;
	} else {
		LOG_INFO_R(resource, "Resource arrived");
		delete resource;
	}
	ObjectLockWrite();
	items++;
	ObjectUnlock();
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return (Module*)new TestOutput(objects, id, threadIndex);
}
