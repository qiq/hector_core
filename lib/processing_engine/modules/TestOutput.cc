/**
 * Test module.
 */
#include <config.h>

#include "TestOutput.h"
#include "TestResource.h"

using namespace std;

TestOutput::TestOutput(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	items = 0;

	props = new ObjectProperties<TestOutput>(this);
	props->Add("items", &TestOutput::GetItems, NULL);
}

TestOutput::~TestOutput() {
	delete props;
}

char *TestOutput::GetItems(const char *name) {
	return int2str(items);
}

bool TestOutput::Init(vector<pair<string, string> > *params) {
	// second stage?
	if (!params)
		return true;

	props->InitProperties(params);
	return true;
}

Resource *TestOutput::ProcessOutputSync(Resource *resource) {
	if (TestResource::IsInstance(resource)) {
		TestResource *tr = static_cast<TestResource*>(resource);
		LOG_INFO_R(this, tr, "Resource arrived (" << tr->GetStr() << ")");
	} else {
		LOG_INFO_R(this, resource, "Resource arrived");
	}
	items++;
	return resource;
}

// the class factories

extern "C" Module* hector_module_create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return (Module*)new TestOutput(objects, id, threadIndex);
}
