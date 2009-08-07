/**
 * Dummy module, does nothing.
 */

#ifndef _MODULE_SAVE_RESOURCE_H_
#define _MODULE_SAVE_RESOURCE_H_

#include <config.h>

#include "Module.h"

class ModuleSaveResource : public Module {
public:
	ModuleSaveResource() {};
	~ModuleSaveResource() {};
	bool Init(Server *server, Config *config, const char *id);
	module_t getType();
	void Process(Resource *resource);
	void createCheckpoint();
};

inline module_t ModuleLoadResource::getType() {
	return MODULE_OUTPUT;
}

#endif
