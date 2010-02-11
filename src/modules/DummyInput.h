/**
 * Dummy module, does nothing.
 */

#ifndef _DUMMY_INPUT_H_
#define _DUMMY_INPUT_H_

#include <config.h>

#include "Module.h"

class DummyInput : public Module {
public:
	DummyInput(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {};
	~DummyInput() {};
	bool Init(vector<pair<string, string> > *params);
	module_t getType();
	Resource *Process(Resource *resource);
};

inline module_t DummyInput::getType() {
	return MODULE_INPUT;
}

#endif
