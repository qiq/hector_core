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
#include "ObjectProperties.h"

class TestSimple : public Module {
public:
	TestSimple(ObjectRegistry *objects, const char *id, int threadIndex);
	~TestSimple();
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	Module::Type GetType();
	Resource *ProcessSimpleSync(Resource *resource);

private:
	int items;		// ObjectLock
	char *foo;		// ObjectLock
	bool flipStatus;	// ObjectLock
	int setStatus;		// ObjectLock

	char *GetItems(const char *name);
	char *GetFoo(const char *name);
	void SetFoo(const char *name, const char *value);
	char *GetFlipStatus(const char *name);
	void SetFlipStatus(const char *name, const char *value);
	char *GetSetStatus(const char *name);
	void SetSetStatus(const char *name, const char *value);

	ObjectProperties<TestSimple> *props;
	char *GetPropertySync(const char *name);
	bool SetPropertySync(const char *name, const char *value);
	std::vector<std::string> *ListPropertiesSync();
};

inline 	Module::Type TestSimple::GetType() {
	return SIMPLE;
}

inline char *TestSimple::GetPropertySync(const char *name) {
	return props->GetProperty(name);
}

inline bool TestSimple::SetPropertySync(const char *name, const char *value) {
	return props->SetProperty(name, value);
}

inline std::vector<std::string> *TestSimple::ListPropertiesSync() {
	return props->ListProperties();
}

#endif
