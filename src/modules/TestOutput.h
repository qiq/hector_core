/**
 * Test module, does nothing.
 */

#ifndef _TEST_OUTPUT_H_
#define _TEST_OUTPUT_H_

#include <config.h>

#include "Module.h"

class TestOutput : public Module {
public:
	TestOutput(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {};
	~TestOutput() {};
	bool Init(vector<pair<string, string> > *params);
	module_t getType();
	Resource *Process(Resource *resource);
};

inline module_t TestOutput::getType() {
	return MODULE_OUTPUT;
}

#endif
