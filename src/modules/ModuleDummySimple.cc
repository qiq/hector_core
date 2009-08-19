/**
 * Dummy module.
 */
#include <config.h>

#include <string.h>
#include "ModuleDummySimple.h"
#include "Server.h"
#include "WebResource.h"

ModuleDummySimple::ModuleDummySimple(ObjectRegistry *objects, const char *id): Module(objects, id) {
	dummy = NULL;
	foo = NULL;

	getters["dummy"] = &ModuleDummySimple::getDummy;
	setters["dummy"] = &ModuleDummySimple::setDummy;
	getters["foo"] = &ModuleDummySimple::getFoo;
	setters["foo"] = &ModuleDummySimple::setFoo;
	getters["alias"] = &ModuleDummySimple::getDummy;
	setters["alias"] = &ModuleDummySimple::setDummy;
}

ModuleDummySimple::~ModuleDummySimple() {
	free(dummy);
	free(foo);
}

char *ModuleDummySimple::getDummy() {
	return dummy ? strdup(dummy) : NULL;
}

void ModuleDummySimple::setDummy(const char *value) {
	free(dummy);
	dummy = strdup(value);
}

char *ModuleDummySimple::getFoo() {
	return foo ? strdup(foo) : NULL;
}

void ModuleDummySimple::setFoo(const char *value) {
	free(foo);
	foo = strdup(value);
}


bool ModuleDummySimple::init(Config *config) {
	return true;
}

void ModuleDummySimple::process(Resource *resource) {
	WebResource *wr = dynamic_cast<WebResource*>(resource);
	if (wr) {
		LOG4CXX_INFO(logger, "Dummy: processing resource " << wr->getURL());
	}
	return;
}

void ModuleDummySimple::createCheckpoint() {
	// TODO
}

char *ModuleDummySimple::getValue(const char *name) {
	char *result = NULL;
	stdext::hash_map<string, char*(ModuleDummySimple::*)(), string_hash>::iterator iter = getters.find(name);
	if (iter != getters.end()) {
		propertyLock.lock();
		result = (this->*(iter->second))();
		propertyLock.unlock();
	}
	return result;
}

bool ModuleDummySimple::setValue(const char *name, const char *value) {
	stdext::hash_map<string, void(ModuleDummySimple::*)(const char*), string_hash>::iterator iter = setters.find(name);
	if (iter != setters.end()) {
		propertyLock.lock();
		(this->*(iter->second))(value);
		propertyLock.unlock();
		return true;
	}
	return false;
}

vector<string> *ModuleDummySimple::listNames() {
	vector<string> *result = new vector<string>();
	for (stdext::hash_map<string, char*(ModuleDummySimple::*)(), string_hash>::iterator iter = getters.begin(); iter != getters.end(); ++iter) {
		result->push_back(iter->first);
	}
	return result;
}

// factory functions

extern "C" Module* create(ObjectRegistry *objects, const char *id) {
	return new ModuleDummySimple(objects, id);
}

extern "C" void destroy(Module* p) {
	delete p;
}
