/**
 * Dummy module, does nothing.
 */

#ifndef _MODULE_SAVE_RESOURCE_H_
#define _MODULE_SAVE_RESOURCE_H_

#include <config.h>

#include "Module.h"

class ModuleSaveResource : public Module {
public:
	ModuleSaveResource(ObjectRegistry *objects, const char *id): Module(objects, id) {};
	~ModuleSaveResource() {};
	bool init(Config *config);
	module_t getType();
	void process(Resource *resource);
	void createCheckpoint();

	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
};

inline module_t ModuleSaveResource::getType() {
	return MODULE_OUTPUT;
}

#endif
