/**
 * Dummy module.
 */
#include <config.h>

#include <string.h>
#include "ModuleDummy.h"
#include "Server.h"
#include "WebResource.h"

char *ModuleDummy::getDummy() {
	return dummy ? strdup(dummy) : NULL;
}

void ModuleDummy::setDummy(const char *value) {
	free(dummy);
	dummy = strdup(value);
}

char *ModuleDummy::getFoo() {
	return foo ? strdup(foo) : NULL;
}

void ModuleDummy::setFoo(const char *value) {
	free(foo);
	foo = strdup(value);
}

ModuleDummy::ModuleDummy(ObjectRegistry *objects, const char *id): Module(objects, id) {
	dummy = NULL;
	foo = NULL;

	getters["dummy"] = &ModuleDummy::getDummy;
	setters["dummy"] = &ModuleDummy::setDummy;
	getters["foo"] = &ModuleDummy::getFoo;
	setters["foo"] = &ModuleDummy::setFoo;
	getters["alias"] = &ModuleDummy::getDummy;
	setters["alias"] = &ModuleDummy::setDummy;
}

ModuleDummy::~ModuleDummy() {
	free(dummy);
	free(foo);
}

bool ModuleDummy::Init(Config *config) {
	return true;
}

void ModuleDummy::Process(Resource *resource) {
	WebResource *wr = dynamic_cast<WebResource*>(resource);
	if (wr) {
		LOG4CXX_INFO(logger, "Dummy: processing resource " << wr->getURL());
	}
	return;
}

void ModuleDummy::createCheckpoint() {
	// TODO
}

char *ModuleDummy::getValue(const char *name) {
	char *result = NULL;
	stdext::hash_map<string, char*(ModuleDummy::*)(), string_hash>::iterator iter = getters.find(name);
	if (iter != getters.end()) {
		lock.lock();
		result = (this->*(iter->second))();
		lock.unlock();
	}
	return result;
}

bool ModuleDummy::setValue(const char *name, const char *value) {
	stdext::hash_map<string, void(ModuleDummy::*)(const char*), string_hash>::iterator iter = setters.find(name);
	if (iter != setters.end()) {
		lock.lock();
		(this->*(iter->second))(value);
		lock.unlock();
		return true;
	}
	return false;
}

vector<string> *ModuleDummy::listNames() {
	vector<string> *result = new vector<string>();
	for (stdext::hash_map<string, char*(ModuleDummy::*)(), string_hash>::iterator iter = getters.begin(); iter != getters.end(); iter++) {
		result->push_back(iter->first);
	}
	return result;
}

// factory functions

extern "C" Module* create(ObjectRegistry *objects, const char *id) {
	return new ModuleDummy(objects, id);
}

extern "C" void destroy(Module* p) {
	delete p;
}
