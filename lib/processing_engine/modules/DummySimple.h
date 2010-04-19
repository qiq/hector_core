/**
 * Dummy module, does nothing.
 */

#ifndef _DUMMY_SIMPLE_H_
#define _DUMMY_SIMPLE_H_

#include <config.h>

#include <string>
#include <tr1/unordered_map>
#include "common.h"
#include "Lock.h"
#include "Module.h"
#include "ObjectValues.h"

class DummySimple : public Module {
public:
	DummySimple(ObjectRegistry *objects, const char *id, int threadIndex);
	~DummySimple();
	bool Init(vector<pair<string, string> > *params);
	module_t getType();
	Resource *Process(Resource *resource);

protected:
	char *dummy;
	char *foo;

	ObjectValues<DummySimple> *values;

	char *getDummy(const char *name);
	void setDummy(const char *name, const char *value);
	char *getFoo(const char *name);
	void setFoo(const char *name, const char *value);

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	vector<string> *listNamesSync();

//	void SaveCheckpointSync(const char *path, const char *id);
//	void RestoreCheckpointSync(const char *path, const char *id);
};

inline module_t DummySimple::getType() {
	return MODULE_SIMPLE;
}

inline char *DummySimple::getValueSync(const char *name) {
	return values->getValueSync(name);
}

inline bool DummySimple::setValueSync(const char *name, const char *value) {
	return values->setValueSync(name, value);
}

inline vector<string> *DummySimple::listNamesSync() {
	return values->listNamesSync();
}

#endif
