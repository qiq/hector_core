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
	values->InitValues(params);
	return true;
}

Resource *TestOutput::Process(Resource *resource) {
	TestResource *tr = dynamic_cast<TestResource*>(resource);
	if (tr) {
		LOG_INFO(logger, "Resource arrived (" << tr->getStr() << ")");
		ObjectLockWrite();
		items++;
		ObjectUnlock();
		delete tr;
	} else {
		LOG_INFO(logger, "Resource arrived (" << tr->getId() << ")");
		ObjectLockWrite();
		items++;
		ObjectUnlock();
		delete resource;
	}
	return NULL;
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return (Module*)new TestOutput(objects, id, threadIndex);
}
