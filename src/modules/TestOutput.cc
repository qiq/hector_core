/**
 * Test module.
 */
#include <config.h>

#include "TestOutput.h"
#include "TestResource.h"


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

Resource *TestOutput::ProcessSimple(Resource *resource) {
	TestResource *tr = dynamic_cast<TestResource*>(resource);
	if (tr) {
		LOG_INFO(logger, "Resource arrived (" << tr->getStr() << ")");
		items++;
		delete tr;
	}
	return NULL;
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return (Module*)new TestOutput(objects, id, threadIndex);
}

extern "C" void destroy(Module* p) {
	delete p;
}

