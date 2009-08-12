/**
 * Dummy module, does nothing.
 */

#ifndef _MODULE_DUMMY_H_
#define _MODULE_DUMMY_H_

#include <config.h>

#include <string>
#include <ext/hash_map>
#include "common.h"
#include "Lock.h"
#include "Module.h"

class ModuleDummy : public Module {
	Lock lock;
	char *dummy;
	char *foo;

	stdext::hash_map<string, char*(ModuleDummy::*)(), string_hash> getters;
	stdext::hash_map<string, void(ModuleDummy::*)(const char*), string_hash> setters;

	char *getDummy();
	void setDummy(const char *value);
	char *getFoo();
	void setFoo(const char *value);
public:
	ModuleDummy(ObjectRegistry *objects, const char *id);
	~ModuleDummy();
	bool Init(Config *config);
	module_t getType();
	void Process(Resource *resource);
	void createCheckpoint();

	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
};

inline module_t ModuleDummy::getType() {
	return MODULE_SIMPLE;
}

#endif
