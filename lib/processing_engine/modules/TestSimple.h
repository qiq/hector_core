/**
TestSimple.la, simple, native
Process resources using ProcessSimple()

Dependencies: none
Parameters:
items		r/o	Total items processed
foo		r/w	Test string
flipStatus	r/w	Whether to change status from 1 to 0 (or vice-versa)
setStatus	r/w	Whether to set status to an arbitrary value (setStatus >= 0)
*/

#ifndef _LIB_PROCESSING_ENGINE_MODULES_TEST_SIMPLE_H_
#define _LIB_PROCESSING_ENGINE_MODULES_TEST_SIMPLE_H_

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
	Resource *ProcessSimple(Resource *resource);

private:
	int items;		// ObjectLock
	char *foo;		// ObjectLock
	bool flipStatus;	// ObjectLock
	int setStatus;		// ObjectLock

	char *getItems(const char *name);
	char *getFoo(const char *name);
	void setFoo(const char *name, const char *value);
	char *getFlipStatus(const char *name);
	void setFlipStatus(const char *name, const char *value);
	char *getSetStatus(const char *name);
	void setSetStatus(const char *name, const char *value);

	ObjectValues<TestSimple> *values;
	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	bool isInitOnly(const char *name);
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

inline bool TestSimple::isInitOnly(const char *name) {
	return values->isInitOnly(name);
}

inline std::vector<std::string> *TestSimple::listNamesSync() {
	return values->listNamesSync();
}

#endif
