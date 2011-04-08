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
#include "ObjectProperties.h"

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

	ObjectProperties<TestOutput> *props;
	char *GetPropertySync(const char *name);
	bool SetPropertySync(const char *name, const char *value);
	std::vector<std::string> *ListPropertiesSync();
};

inline Module::Type TestOutput::GetType() {
	return OUTPUT;
}

inline char *TestOutput::GetPropertySync(const char *name) {
	return props->GetProperty(name);
}

inline bool TestOutput::SetPropertySync(const char *name, const char *value) {
	return props->SetProperty(name, value);
}

inline std::vector<std::string> *TestOutput::ListPropertiesSync() {
	return props->ListProperties();
}

#endif
