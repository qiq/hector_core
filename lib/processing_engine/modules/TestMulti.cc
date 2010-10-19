/**
 * Test module.
 */
#include <config.h>

#include <string.h>
#include <sys/select.h>
#include "TestMulti.h"
#include "TestResource.h"

using namespace std;

TestMulti::TestMulti(ObjectRegistry *objects, ProcessingEngine *engine, const char *id, int threadIndex): Module(objects, engine, id, threadIndex) {
	items = 0;
	foo = NULL;
	resources = new queue<TestResource*>();

	values = new ObjectValues<TestMulti>(this);
	values->addGetter("items", &TestMulti::getItems);
	values->addGetter("foo", &TestMulti::getFoo);
	values->addSetter("foo", &TestMulti::setFoo);
	values->addGetter("alias", &TestMulti::getFoo);
	values->addSetter("alias", &TestMulti::setFoo);
}

TestMulti::~TestMulti() {
	delete values;
	free(foo);
	delete resources;
}

char *TestMulti::getItems(const char *name) {
	return int2str(items);
}

char *TestMulti::getFoo(const char *name) {
	return foo ? strdup(foo) : NULL;
}

void TestMulti::setFoo(const char *name, const char *value) {
	free(foo);
	foo = strdup(value);
}

bool TestMulti::Init(vector<pair<string, string> > *params) {
	values->InitValues(params);
	return true;
}

#define MAX_RESOURCES 100
int TestMulti::ProcessMulti(queue<Resource*> *inputResources, queue<Resource*> *outputResources) {
	while (inputResources->size() > 0 && resources->size() <= MAX_RESOURCES) {
		TestResource *tr = dynamic_cast<TestResource*>(inputResources->front());
		if (tr)
			resources->push(tr);
		inputResources->pop();
	}

	if (resources->size() == 0)
		return -1;

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 100;

	if (select(1, NULL, NULL, NULL, &tv) < 0) {
		LOG_INFO("Error in select() = " << errno);
		return 0;
	}
	TestResource *tr = resources->front();
	resources->pop();
	outputResources->push(tr);
	LOG_INFO("Processed TestResource (" << tr->getStr() << ")");
	ObjectLockWrite();
	++items;
	ObjectUnlock();
	
	return MAX_RESOURCES-resources->size();
}

int TestMulti::ProcessingResources() {
	return resources->size();
}

// factory functions

extern "C" Module* create(ObjectRegistry *objects, ProcessingEngine *engine, const char *id, int threadIndex) {
	return new TestMulti(objects, engine, id, threadIndex);
}
