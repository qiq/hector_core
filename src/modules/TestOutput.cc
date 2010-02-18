/**
 * Test module.
 */
#include <config.h>

#include "TestOutput.h"
#include "TestResource.h"

bool TestOutput::Init(vector<pair<string, string> > *params) {
	return true;
}

Resource *TestOutput::Process(Resource *resource) {
	TestResource *tr = dynamic_cast<TestResource*>(resource);
	if (tr) {
		LOG_INFO(logger, "Resource arrived (" << tr->getStr() << ")");
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

