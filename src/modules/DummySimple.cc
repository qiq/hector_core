/**
 * Dummy module.
 */
#include <config.h>

#include <string.h>
#include "DummySimple.h"
#include "Server.h"
#include "WebResource.h"

DummySimple::DummySimple(ObjectRegistry *objects, const char *id): Module(objects, id) {
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

void DummySimple::SaveCheckpoint(const char *path, const char *id) {
	// TODO
}

void DummySimple::RestoreCheckpoint(const char *path, const char *id) {
	// TODO
}

char *DummySimple::getValue(const char *name) {
	char *result = NULL;
	std::tr1::unordered_map<string, char*(DummySimple::*)()>::iterator iter = getters.find(name);
	if (iter != getters.end()) {
		propertyLock.lock();
		result = (this->*(iter->second))();
		propertyLock.unlock();
	}
	return result;
}

bool DummySimple::setValue(const char *name, const char *value) {
	std::tr1::unordered_map<string, void(DummySimple::*)(const char*)>::iterator iter = setters.find(name);
	if (iter != setters.end()) {
		propertyLock.lock();
		(this->*(iter->second))(value);
		propertyLock.unlock();
		return true;
	}
	return false;
}

vector<string> *DummySimple::listNames() {
	vector<string> *result = new vector<string>();
	for (std::tr1::unordered_map<string, char*(DummySimple::*)()>::iterator iter = getters.begin(); iter != getters.end(); ++iter) {
		result->push_back(iter->first);
	}
	return result;
}

// factory functions

extern "C" Module* create(ObjectRegistry *objects, const char *id) {
	return new DummySimple(objects, id);
}

extern "C" void destroy(Module* p) {
	delete p;
}
