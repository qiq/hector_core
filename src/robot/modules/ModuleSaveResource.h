/**
 * Dummy module, does nothing.
 */

#ifndef _MODULE_SAVE_RESOURCE_H_
#define _MODULE_SAVE_RESOURCE_H_

#include "ModuleOutput.h"

class ModuleSaveResource : public ModuleOutput {
public:
	ModuleSaveResource() {};
	virtual ~ModuleSaveResource() {};
	bool Init(Config *config, const char *name);
	void Process(Resource *resource);
	virtual void createCheckpoint();
	virtual string *getStatistics();
};

#endif
