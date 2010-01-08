/**
 * Dummy module, does nothing.
 */

#ifndef _MODULE_DUMMY_SIMPLE_H_
#define _MODULE_DUMMY_SIMPLE_H_

#include <config.h>

#include <string>
#include <tr1/unordered_map>
#include "common.h"
#include "Lock.h"
#include "Module.h"

class ModuleDummySimple : public Module {
	Lock propertyLock;
	char *dummy;
	char *foo;

	std::tr1::unordered_map<string, char*(ModuleDummySimple::*)()> getters;
	std::tr1::unordered_map<string, void(ModuleDummySimple::*)(const char*)> setters;

	char *getDummy();
	void setDummy(const char *value);
	char *getFoo();
	void setFoo(const char *value);
public:
	ModuleDummySimple(ObjectRegistry *objects, const char *id);
	~ModuleDummySimple();
	bool init(Config *config);
	module_t getType();
	Resource *process(Resource *resource);
	void createCheckpoint();

	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
};

inline module_t ModuleDummySimple::getType() {
	return MODULE_SIMPLE;
}

#endif
