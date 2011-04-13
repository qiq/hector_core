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

class ResourceAttrInfo;
class ResourceInputStream;
class ResourceOutputStream;

class TestResourceInfo : public ResourceInfo {
public:
	TestResourceInfo();
};

class TestResource : public Resource {
public:
	TestResource() {};
	~TestResource() {};
	// create copy of a resource
	Resource *Clone();
	// Clear current resource (as would delete + new do)
	void Clear();
	// save and restore resource
	bool Serialize(ResourceOutputStream &output);
	bool Deserialize(ResourceInputStream &input);
	// used by queues in case there is a limit on queue size
	int GetSize();
	// get info about this resource
	ResourceInfo *GetResourceInfo();
	// return string representation of the resource (e.g. for debugging purposes)
	std::string ToString(Object::LogLevel logLevel);

	void SetStr(const std::string &s);
	const std::string GetStr();

	// We cannot rely on the dynamic_cast as we use shared libraries and dlopen.
	static bool IsInstance(Resource *resource);

protected:
	std::string str;

	static TestResourceInfo resourceInfo;
	static log4cxx::LoggerPtr logger;
};

inline int TestResource::GetSize() {
	return str.length();
}

inline ResourceInfo *TestResource::GetResourceInfo() {
	return &TestResource::resourceInfo;
}

inline const std::string TestResource::GetStr() {
	return str;
}

inline void TestResource::SetStr(const std::string &str) {
	this->str = str;
}

inline bool TestResource::IsInstance(Resource *resource) {
	return resource->GetTypeId() == resourceInfo.GetTypeId();
}

#endif
