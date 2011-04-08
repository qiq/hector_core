/**
 * Test module.
 */
#include <config.h>

#include <string.h>
#include "TestSimple.h"
#include "TestResource.h"

using namespace std;

TestSimple::TestSimple(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	items = 0;
	foo = NULL;
	flipStatus = false;
	setStatus = -1;

	props = new ObjectProperties<TestSimple>(this);
	props->Add("items", &TestSimple::GetItems);
	props->Add("foo", &TestSimple::GetFoo, &TestSimple::SetFoo);
	props->Add("alias", &TestSimple::GetFoo, &TestSimple::SetFoo);
	props->Add("flipStatus", &TestSimple::GetFlipStatus, &TestSimple::SetFlipStatus);
	props->Add("setStatus", &TestSimple::GetSetStatus, &TestSimple::SetSetStatus);
}

TestSimple::~TestSimple() {
	delete props;
	free(foo);
}

char *TestSimple::GetItems(const char *name) {
	return int2str(items);
}

char *TestSimple::GetFoo(const char *name) {
	return foo ? strdup(foo) : NULL;
}

void TestSimple::SetFoo(const char *name, const char *value) {
	free(foo);
	foo = strdup(value);
}

char *TestSimple::GetFlipStatus(const char *name) {
	return bool2str(flipStatus);
}

void TestSimple::SetFlipStatus(const char *name, const char *value) {
	switch (str2bool(value)) {
	case 0:
		flipStatus = false;
		break;
	case 1:
		flipStatus = true;
		break;
	default:
		LOG_ERROR(this, "Invalid boolean value: " << value);
	}
}

char *TestSimple::GetSetStatus(const char *name) {
	return int2str(setStatus);
}

void TestSimple::SetSetStatus(const char *name, const char *value) {
	setStatus = str2int(value);
}

bool TestSimple::Init(vector<pair<string, string> > *params) {
	// second stage?
	if (!params)
		return true;

	props->InitProperties(params);
	return true;
}

Resource *TestSimple::ProcessSimpleSync(Resource *resource) {
	if (!TestResource::IsInstance(resource))
		return resource;
	TestResource *tr = static_cast<TestResource*>(resource);
	LOG_DEBUG_R(this, tr, "Processing (" << tr->GetStr() << ")");
	if (flipStatus)
		tr->SetStatus(tr->GetStatus() == 0 ? 1 : 0);
	if (setStatus >= 0)
		tr->SetStatus(setStatus);
	++items;
	return resource;
}

// factory functions

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return new TestSimple(objects, id, threadIndex);
}
