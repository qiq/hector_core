/**
 * Class representing queue of resources (mainly HTML pages) while processing.
 * It uses Google Protocol Buffers to de/serialize.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCES_TEST_PROTOBUF_RESOURCE_H_
#define _LIB_PROCESSING_ENGINE_RESOURCES_TEST_PROTOBUF_RESOURCE_H_

#include <config.h>

#include <string>
#include <log4cxx/logger.h>
#include "common.h"
#include "Resource.h"
#include "TestProtobufResource.pb.h"

class ResourceAttrInfo;
class ResourceInputStream;
class ResourceOutputStream;

class TestProtobufResourceInfo : public ResourceInfo {
public:
	TestProtobufResourceInfo();
};

class TestProtobufResource : public Resource {
public:
	TestProtobufResource() {};
	~TestProtobufResource() {};
	// create copy of a resource
	Resource *Clone();
	// Clear current resource (as would delete + new do)
	void Clear();
	// save and restore resource
	bool Serialize(ResourceOutputStream &output);
	bool Deserialize(ResourceInputStream &input);
	// used by queues in case there is limit on queue size
	int GetSize();
	// get info about this resource
	ResourceInfo *GetResourceInfo();
	// return string representation of the resource (e.g. for debugging purposes)
	std::string ToString(Object::LogLevel logLevel);

	void SetStr(const std::string &s);
	const std::string GetStr();

	static bool IsInstance(Resource *resource);

protected:
	hector::resources::TestProtobufResource r;

	static TestProtobufResourceInfo resourceInfo;
	static log4cxx::LoggerPtr logger;
};

inline int TestProtobufResource::GetSize() {
	return r.str().size();
}

inline ResourceInfo *TestProtobufResource::GetResourceInfo() {
	return &TestProtobufResource::resourceInfo;
}

inline const std::string TestProtobufResource::GetStr() {
	return r.str();
}

inline void TestProtobufResource::SetStr(const std::string &str) {
	r.set_str(str);
}

inline bool TestProtobufResource::IsInstance(Resource *resource) {
	return resource->GetTypeId() == resourceInfo.GetTypeId();
}

#endif
