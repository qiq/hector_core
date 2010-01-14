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
	bool Init(Config *config);
	module_t getType();
	Resource *Process(Resource *resource);
	void SaveCheckpoint(const char *path, const char *id);
	void RestoreCheckpoint(const char *path, const char *id);

	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
};

inline module_t ModuleDummyInput::getType() {
	return MODULE_INPUT;
}

#endif
