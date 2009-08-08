/**
 * Dummy module, does nothing.
 */

#ifndef _MODULE_DUMMY_H_
#define _MODULE_DUMMY_H_

#include <config.h>

#include "Module.h"

class ModuleDummy : public Module {
public:
	ModuleDummy(ObjectRegistry *objects, const char *id): Module(objects, id) {}
	~ModuleDummy() {}
	bool Init(Config *config);
	module_t getType();
	void Process(Resource *resource);
	void createCheckpoint();

	const char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
};

inline module_t ModuleDummy::getType() {
	return MODULE_SIMPLE;
}

#endif
