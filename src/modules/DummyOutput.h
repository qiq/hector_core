/**
 * Dummy module, does nothing.
 */

#ifndef _DUMMY_OUTPUT_H_
#define _DUMMY_OUTPUT_H_

#include <config.h>

#include "Module.h"

class DummyOutput : public Module {
public:
	DummyOutput(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {};
	~DummyOutput() {};
	bool Init(vector<pair<string, string> > *params);
	module_t getType();
	Resource *ProcessSimple(Resource *resource);
};

inline module_t DummyOutput::getType() {
	return MODULE_OUTPUT;
}

#endif
