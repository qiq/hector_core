/**
 * Dummy module, does nothing.
 */

#ifndef _MODULE_DUMMY_OUTPUT_H_
#define _MODULE_DUMMY_OUTPUT_H_

#include <config.h>

#include "Module.h"

class ModuleDummyOutput : public Module {
public:
	ModuleDummyOutput(ObjectRegistry *objects, const char *id): Module(objects, id) {};
	~ModuleDummyOutput() {};
	bool init(Config *config);
	module_t getType();
	Resource *process(Resource *resource);
	void createCheckpoint();

	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
};

inline module_t ModuleDummyOutput::getType() {
	return MODULE_OUTPUT;
}

#endif
