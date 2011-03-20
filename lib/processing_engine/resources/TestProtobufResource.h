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
#include "ProtobufResource.h"
#include "ResourceAttrInfoT.h"
#include "TestProtobufResource.pb.h"

class TestProtobufResource : public ProtobufResource {
public:
	TestProtobufResource() {};
	~TestProtobufResource() {};
	// create copy of a resource
	ProtobufResource *Clone();
	// Clear current resource (as would delete + new do)
	void Clear();
	// save and restore resource
	std::string *Serialize();
	int GetSerializedSize();
	bool SerializeWithCachedSize(google::protobuf::io::CodedOutputStream *output);
	bool Deserialize(const char *data, int size);
	bool Deserialize(google::protobuf::io::CodedInputStream *input);
	// return ResourceAttrInfo describing all attributes
	std::vector<ResourceAttrInfo*> *GetAttrInfoList();
	// type id of a resource (to be used by ResourceRegistry::AcquireResource(typeid))
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
	static const int typeId = 2;

	hector::resources::TestProtobufResource r;

	static log4cxx::LoggerPtr logger;
};

inline std::string *TestProtobufResource::Serialize() {
	r.set_id(GetId());
	r.set_status(GetStatus());

	std::string *result = new std::string();
	r.SerializeToString(result);
	return result;
}

inline int TestProtobufResource::GetSerializedSize() {
	r.set_id(GetId());
	r.set_status(GetStatus());

	return r.ByteSize();
}

inline bool TestProtobufResource::SerializeWithCachedSize(google::protobuf::io::CodedOutputStream *output) {
	// r.id and r.status were set in GetSerializedSize() already
	r.SerializeWithCachedSizes(output);
	return true;
}

inline bool TestProtobufResource::Deserialize(const char *data, int size) {
	bool result = r.ParseFromArray((void*)data, size);

	// we keep id
	SetStatus(r.status());
	return result;
}

bool TestProtobufResource::Deserialize(google::protobuf::io::CodedInputStream *input) {
	return r.ParseFromCodedStream(input);
}

//inline ResourceFieldInfo *TestProtobufResource::GetFieldInfo(const char *name) {
//	return new ResourceFieldInfoT<TestProtobufResource>(name);
//}

inline int TestProtobufResource::GetTypeId() {
	return typeId;
}

inline const char *TestProtobufResource::GetTypeString(bool terse) {
	return terse ? "TPR" : "TestProtobufResource";
}

inline const char *TestProtobufResource::GetObjectName() {
	return "TestProtobufResource";
}

inline int TestProtobufResource::GetSize() {
	return 1;
}

inline const std::string TestProtobufResource::GetStr() {
	return r.str();
}

inline void TestProtobufResource::SetStr(const std::string &str) {
	r.set_str(str);
}

inline bool TestProtobufResource::IsInstance(Resource *resource) {
	return resource->GetTypeId() == typeId;
}

#endif
