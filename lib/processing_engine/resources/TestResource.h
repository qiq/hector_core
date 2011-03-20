/**
 * Class representing queue of resources (mainly HTML pages) while processing.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCES_TEST_RESOURCE_H_
#define _LIB_PROCESSING_ENGINE_RESOURCES_TEST_RESOURCE_H_

#include <config.h>

#include <string>
#include <vector>
#include <log4cxx/logger.h>
#include "Resource.h"
#include "ResourceAttrInfoT.h"

class TestResource : public Resource {
public:
	TestResource() {};
	~TestResource() {};
	// create copy of a resource
	Resource *Clone();
	// Clear current resource (as would delete + new do)
	void Clear();
	// save and restore resource
	std::string *Serialize();
	bool Deserialize(const char *data, int size);
	// return ResourceAttrInfo describing one field
	std::vector<ResourceAttrInfo*> *GetAttrInfoList();
	// type id of a resource (to be used by Resources::AcquireResource(typeid))
	int GetTypeId();
	// type string of a resource
	const char *GetTypeString(bool terse = false);
	// object name (for construction of an object or a reference)
	const char *GetObjectName();
	// used by queues in case there is limit on queue size
	int GetSize();
	// return string representation of the resource (e.g. for debugging purposes)
	std::string ToString(Object::LogLevel logLevel);

	void SetStr(const std::string &s);
	const std::string GetStr();

	static bool IsInstance(Resource *resource);

protected:
	static const int typeId = 1;

	std::string str;

	static log4cxx::LoggerPtr logger;
};

inline int TestResource::GetTypeId() {
	return typeId; 
}

inline const char *TestResource::GetTypeString(bool terse) {
	return terse ? "TR" : "TestResource";
}

inline const char *TestResource::GetObjectName() {
	return "TestResource";
}

inline int TestResource::GetSize() {
	return 1;
}

inline const std::string TestResource::GetStr() {
	return str;
}

inline void TestResource::SetStr(const std::string &str) {
	this->str = str;
}

inline bool TestResource::IsInstance(Resource *resource) {
	return resource->GetTypeId() == typeId;
}

#endif
