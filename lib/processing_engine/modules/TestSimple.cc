/**
 * Test module.
 */
#include <config.h>

#include <string.h>
#include "TestSimple.h"
#include "TestResource.h"

TestSimple::TestSimple(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	items = 0;
	foo = NULL;
	flipStatus = false;
	setStatus = -1;
	values = new ObjectValues<TestSimple>(this);

	values->addGetter("items", &TestSimple::getItems);
	values->addGetter("foo", &TestSimple::getFoo);
	values->addSetter("foo", &TestSimple::setFoo);
	values->addGetter("alias", &TestSimple::getFoo);
	values->addSetter("alias", &TestSimple::setFoo);
	values->addGetter("flipStatus", &TestSimple::getFlipStatus);
	values->addSetter("flipStatus", &TestSimple::setFlipStatus);
	values->addGetter("setStatus", &TestSimple::getSetStatus);
	values->addSetter("setStatus", &TestSimple::setSetStatus);
}

TestSimple::~TestSimple() {
	delete values;
	free(foo);
}

char *TestSimple::getItems(const char *name) {
	return int2str(items);
}

char *TestSimple::getFoo(const char *name) {
	return foo ? strdup(foo) : NULL;
}

void TestSimple::setFoo(const char *name, const char *value) {
	free(foo);
	foo = strdup(value);
}

char *TestSimple::getFlipStatus(const char *name) {
	return bool2str(flipStatus);
}

void TestSimple::setFlipStatus(const char *name, const char *value) {
	switch (str2bool(value)) {
	case 0:
		flipStatus = false;
		break;
	case 1:
		flipStatus = true;
		break;
	default:
		LOG_ERROR(logger, "Invalid boolean value: " << value);
	}
}

char *TestSimple::getSetStatus(const char *name) {
	return int2str(setStatus);
}

void TestSimple::setSetStatus(const char *name, const char *value) {
	setStatus = str2int(value);
}

bool TestSimple::Init(vector<pair<string, string> > *params) {
	values->InitValues(params);
	return true;
}

Resource *TestSimple::Process(Resource *resource) {
	TestResource *tr = dynamic_cast<TestResource*>(resource);
	if (tr) {
		LOG_INFO(logger, "Processing TestResource (" << tr->getStr() << ")");
		if (flipStatus)
			tr->setStatus(tr->getStatus() == 0 ? 1 : 0);
		if (setStatus >= 0)
			tr->setStatus(setStatus);
		++items;
	}
	return resource;
}

// factory functions

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return new TestSimple(objects, id, threadIndex);
}
