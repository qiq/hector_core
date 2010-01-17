/**
 * Dummy module, does nothing.
 */

#ifndef _DUMMY_OUTPUT_H_
#define _DUMMY_OUTPUT_H_

#include <config.h>

#include "Module.h"

class DummyOutput : public Module {
public:
	DummyOutput(ObjectRegistry *objects, const char *id): Module(objects, id) {};
	~DummyOutput() {};
	bool Init(vector<pair<string, string> > *params);
	module_t getType();
	Resource *Process(Resource *resource);
	void SaveCheckpoint(const char *path, const char *id);
	void RestoreCheckpoint(const char *path, const char *id);

	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
};

inline module_t DummyOutput::getType() {
	return MODULE_OUTPUT;
}

#endif
