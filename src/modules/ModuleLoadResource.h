/**
 * Dummy module, does nothing.
 */

#ifndef _MODULE_LOAD_RESOURCE_H_
#define _MODULE_LOAD_RESOURCE_H_

#include <config.h>

#include "Module.h"

class ModuleLoadResource : public Module {
public:
	ModuleLoadResource(ObjectRegistry *objects, const char *id): Module(objects, id) {};
	~ModuleLoadResource() {};
	bool init(Config *config);
	module_t getType();
	Resource *process();
	void createCheckpoint();

	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
};

inline module_t ModuleLoadResource::getType() {
	return MODULE_INPUT;
}

#endif
