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

	getters["dummy"] = &DummySimple::getDummy;
	setters["dummy"] = &DummySimple::setDummy;
	getters["foo"] = &DummySimple::getFoo;
	setters["foo"] = &DummySimple::setFoo;
	getters["alias"] = &DummySimple::getDummy;
	setters["alias"] = &DummySimple::setDummy;
}

DummySimple::~DummySimple() {
	free(dummy);
	free(foo);
}

char *DummySimple::getDummy() {
	return dummy ? strdup(dummy) : NULL;
}

void DummySimple::setDummy(const char *value) {
	free(dummy);
	dummy = strdup(value);
}

char *DummySimple::getFoo() {
	return foo ? strdup(foo) : NULL;
}

void DummySimple::setFoo(const char *value) {
	free(foo);
	foo = strdup(value);
}

bool DummySimple::Init(vector<pair<string, string> > *params) {
	return true;
}

Resource *DummySimple::Process(Resource *resource) {
	WebResource *wr = dynamic_cast<WebResource*>(resource);
	if (wr) {
		LOG4CXX_INFO(logger, "Dummy: processing resource " << wr->getURL());
	}
	return resource;
}

char *DummySimple::getValueSync(const char *name) {
	std::tr1::unordered_map<string, char*(DummySimple::*)()>::iterator iter = getters.find(name);
	if (iter != getters.end())
		return (this->*(iter->second))();
	return NULL;
}

bool DummySimple::setValueSync(const char *name, const char *value) {
	std::tr1::unordered_map<string, void(DummySimple::*)(const char*)>::iterator iter = setters.find(name);
	if (iter != setters.end()) {
		(this->*(iter->second))(value);
		return true;
	}
	return false;
}

vector<string> *DummySimple::listNamesSync() {
	vector<string> *result = new vector<string>();
	for (std::tr1::unordered_map<string, char*(DummySimple::*)()>::iterator iter = getters.begin(); iter != getters.end(); ++iter) {
		result->push_back(iter->first);
	}
	return result;
}

// factory functions

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return new DummySimple(objects, id, threadIndex);
}

extern "C" void destroy(Module* p) {
	delete p;
}
