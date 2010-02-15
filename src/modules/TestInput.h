/**
 * Test module, does nothing.
 */

#ifndef _TEST_INPUT_H_
#define _TEST_INPUT_H_

#include <config.h>

#include "Module.h"

class TestInput : public Module {
public:
	TestInput(ObjectRegistry *objects, const char *id, int threadIndex);
	~TestInput();
	bool Init(vector<pair<string, string> > *params);
	module_t getType();
	Resource *Process(Resource *resource);

private:
	int items;
	int maxItems;
	char *idPrefix;
};

inline module_t TestInput::getType() {
	return MODULE_INPUT;
}

#endif
