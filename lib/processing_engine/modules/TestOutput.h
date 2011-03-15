/**
TestOutput.la, output, native
Just print out a message that resource arrived.

Dependencies: none

Parameters:
items		r/o	Total items processed
*/

#ifndef _LIB_PROCESSING_ENGINE_MODULES_TEST_OUTPUT_H_
#define _LIB_PROCESSING_ENGINE_MODULES_TEST_OUTPUT_H_

#include <config.h>

#include "Module.h"
#include "ObjectValues.h"

class TestOutput : public Module {
public:
	TestOutput(ObjectRegistry *objects, const char *id, int threadIndex);
	~TestOutput();
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	Module::Type GetType();
	Resource *ProcessOutputSync(Resource *resource);

private:
	int items;		// ObjectLock

	char *GetItems(const char *name);

	ObjectValues<TestOutput> *values;
	char *GetValueSync(const char *name);
	bool SetValueSync(const char *name, const char *value);
	std::vector<std::string> *ListNamesSync();
};

inline Module::Type TestOutput::GetType() {
	return OUTPUT;
}

inline char *TestOutput::GetValueSync(const char *name) {
	return values->GetValue(name);
}

inline bool TestOutput::SetValueSync(const char *name, const char *value) {
	return values->SetValue(name, value);
}

inline std::vector<std::string> *TestOutput::ListNamesSync() {
	return values->ListNames();
}

#endif
