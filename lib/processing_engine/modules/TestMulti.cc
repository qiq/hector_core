/**
 * Test module.
 */
#include <config.h>

#include <string.h>
#include <sys/select.h>
#include "TestMulti.h"
#include "TestResource.h"

using namespace std;

#define MAX_RESOURCES 100
#define DEFAULT_TIMETICK 100*1000

TestMulti::TestMulti(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	items = 0;
	foo = NULL;
	timeTick = DEFAULT_TIMETICK;

	values = new ObjectValues<TestMulti>(this);
	values->Add("items", &TestMulti::GetItems);
	values->Add("foo", &TestMulti::GetFoo, &TestMulti::SetFoo);
	values->Add("alias", &TestMulti::GetFoo, &TestMulti::SetFoo);
	values->Add("timeTick", &TestMulti::GetTimeTick, &TestMulti::SetTimeTick);
}

TestMulti::~TestMulti() {
	delete values;
	free(foo);
}

char *TestMulti::GetItems(const char *name) {
	return int2str(items);
}

char *TestMulti::GetFoo(const char *name) {
	return foo ? strdup(foo) : NULL;
}

void TestMulti::SetFoo(const char *name, const char *value) {
	free(foo);
	foo = strdup(value);
}

char *TestMulti::GetTimeTick(const char *name) {
	return int2str(timeTick);
}

void TestMulti::SetTimeTick(const char *name, const char *value) {
	timeTick = str2int(value);
}

bool TestMulti::Init(vector<pair<string, string> > *params) {
	// second stage?
	if (!params)
		return true;

	values->InitValues(params);
	return true;
}

int TestMulti::ProcessMultiSync(queue<Resource*> *inputResources, queue<Resource*> *outputResources, int *expectResources) {
	while (inputResources->size() > 0 && resources.size() <= MAX_RESOURCES) {
		Resource *r = inputResources->front();
		if (TestResource::IsInstance(r))
			resources.push(static_cast<TestResource*>(r));
		inputResources->pop();
	}

	if (resources.size() == 0) {
		if (expectResources)	
			*expectResources = MAX_RESOURCES;
		return 0;
	}

	struct timeval tv;
	tv.tv_sec = timeTick / 1000000;
	tv.tv_usec = timeTick % 1000000;

	if (select(1, NULL, NULL, NULL, &tv) < 0) {
		LOG_INFO(this, "Error in select() = " << errno);
		if (expectResources)	
			*expectResources = 0;
		return resources.size();
	}
	TestResource *tr = resources.front();
	resources.pop();
	outputResources->push(tr);
	LOG_INFO_R(this, tr, "Processed TestResource (" << tr->GetStr() << ")");
	++items;

	if (expectResources)
		*expectResources = MAX_RESOURCES-resources.size();
	return resources.size();
}

// factory functions

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return new TestMulti(objects, id, threadIndex);
}
