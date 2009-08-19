/**
 * Dummy module, does nothing.
 */

#ifndef _MODULE_DUMMY_INPUT_H_
#define _MODULE_DUMMY_INPUT_H_

#include <config.h>

#include "Module.h"

class ModuleDummyInput : public Module {
public:
	ModuleDummyInput(ObjectRegistry *objects, const char *id): Module(objects, id) {};
	~ModuleDummyInput() {};
	bool init(Config *config);
	module_t getType();
	Resource *process();
	void createCheckpoint();

	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
};

inline module_t ModuleDummyInput::getType() {
	return MODULE_INPUT;
}

#endif
