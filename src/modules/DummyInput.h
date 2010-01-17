/**
 * Dummy module, does nothing.
 */

#ifndef _DUMMY_INPUT_H_
#define _DUMMY_INPUT_H_

#include <config.h>

#include "Module.h"

class DummyInput : public Module {
public:
	DummyInput(ObjectRegistry *objects, const char *id): Module(objects, id) {};
	~DummyInput() {};
	bool Init(vector<pair<string, string> > *params);
	module_t getType();
	Resource *Process(Resource *resource);
	void SaveCheckpoint(const char *path, const char *id);
	void RestoreCheckpoint(const char *path, const char *id);

	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
};

inline module_t DummyInput::getType() {
	return MODULE_INPUT;
}

#endif
