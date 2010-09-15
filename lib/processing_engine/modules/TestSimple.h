/**
 * Test module, does nothing.
 */

#ifndef _TEST_SIMPLE_H_
#define _TEST_SIMPLE_H_

#include <config.h>

#include <string>
#include <tr1/unordered_map>
#include "common.h"
#include "Module.h"
#include "ObjectValues.h"

class TestSimple : public Module {
public:
	TestSimple(ObjectRegistry *objects, const char *id, int threadIndex);
	~TestSimple();
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	Module::Type getType();
	Resource *Process(Resource *resource);

private:
	int items;		// guarded by ObjectLock
	char *foo;		// guarded by ObjectLock
	bool flipStatus;	// guarded by ObjectLock
	int setStatus;		// guarded by ObjectLock

	ObjectValues<TestSimple> *values;

	char *getItems(const char *name);
	char *getFoo(const char *name);
	void setFoo(const char *name, const char *value);
	char *getFlipStatus(const char *name);
	void setFlipStatus(const char *name, const char *value);
	char *getSetStatus(const char *name);
	void setSetStatus(const char *name, const char *value);

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	std::vector<std::string> *listNamesSync();
};

inline 	Module::Type TestSimple::getType() {
	return SIMPLE;
}

inline char *TestSimple::getValueSync(const char *name) {
	return values->getValueSync(name);
}

inline bool TestSimple::setValueSync(const char *name, const char *value) {
	return values->setValueSync(name, value);
}

inline std::vector<std::string> *TestSimple::listNamesSync() {
	return values->listNamesSync();
}

#endif
