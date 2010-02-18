/**
 * Dummy module.
 */
#include <config.h>

#include <string.h>
#include "DummySimple.h"
#include "WebResource.h"

DummySimple::DummySimple(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	dummy = NULL;
	foo = NULL;
	values = new ObjectValues<DummySimple>(this);

	values->addGetter("dummy", &DummySimple::getDummy);
	values->addSetter("dummy", &DummySimple::setDummy);
	values->addGetter("foo", &DummySimple::getFoo);
	values->addSetter("foo", &DummySimple::setFoo);
	values->addGetter("alias", &DummySimple::getDummy);
	values->addSetter("alias", &DummySimple::setDummy);
}

DummySimple::~DummySimple() {
	delete values;

	free(dummy);
	free(foo);
}

char *DummySimple::getDummy(const char *name) {
	return dummy ? strdup(dummy) : NULL;
}

void DummySimple::setDummy(const char *name, const char *value) {
	free(dummy);
	dummy = strdup(value);
}

char *DummySimple::getFoo(const char *name) {
	return foo ? strdup(foo) : NULL;
}

void DummySimple::setFoo(const char *name, const char *value) {
	free(foo);
	foo = strdup(value);
}

bool DummySimple::Init(vector<pair<string, string> > *params) {
	values->InitValues(params);
	return true;
}

Resource *DummySimple::Process(Resource *resource) {
	WebResource *wr = dynamic_cast<WebResource*>(resource);
	if (wr) {
		LOG_INFO(logger, "Dummy: processing resource " << wr->getURL());
	}
	return resource;
}

// factory functions

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return new DummySimple(objects, id, threadIndex);
}

extern "C" void destroy(Module* p) {
	delete p;
}
