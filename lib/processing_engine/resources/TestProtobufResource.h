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
#include "ResourceFieldInfo.h"
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

	// return ResourceInfo describing one field
	ResourceFieldInfo *getFieldInfo(const char *name);
	// type id of a resource (to be used by Resources::AcquireResource(typeid))
	int getTypeId();
	// type string of a resource
	const char *getTypeStr();
	const char *getTypeStrShort();
	// module prefix (e.g. Hector for Hector::TestResource)
	const char *getModuleStr();
	// used by queues in case there is limit on queue size
	int getSize();
	// return string representation of the resource (e.g. for debugging purposes)
	std::string toString(Object::LogLevel logLevel);

	void setStr(const std::string &s);
	const std::string &getStr();

	static const int typeId = 2;

protected:
	hector::resources::TestProtobufResource r;

	static log4cxx::LoggerPtr logger;
};

inline std::string *TestProtobufResource::Serialize() {
	r.set_id(getId());
	r.set_status(getStatus());

	std::string *result = new std::string();
	r.SerializeToString(result);
	return result;
}

inline int TestProtobufResource::GetSerializedSize() {
	r.set_id(getId());
	r.set_status(getStatus());

	return r.ByteSize();
}

inline bool TestProtobufResource::SerializeWithCachedSize(google::protobuf::io::CodedOutputStream *output) {
	// r.id and r.status were set in getSerializedSize() already
	r.SerializeWithCachedSizes(output);
	return true;
}

inline bool TestProtobufResource::Deserialize(const char *data, int size) {
	bool result = r.ParseFromArray((void*)data, size);

	// we keep id
	setStatus(r.status());
	return result;
}

bool TestProtobufResource::Deserialize(google::protobuf::io::CodedInputStream *input) {
	return r.ParseFromCodedStream(input);
}

inline ResourceFieldInfo *TestProtobufResource::getFieldInfo(const char *name) {
	return new ResourceFieldInfoT<TestProtobufResource>(name);
}

inline int TestProtobufResource::getTypeId() {
	return typeId;
}

inline const char *TestProtobufResource::getTypeStr() {
	return "TestProtobufResource";
}

inline const char *TestProtobufResource::getTypeStrShort() {
	return "TPR";
}

inline const char *TestProtobufResource::getModuleStr() {
	return "Hector";
}

inline int TestProtobufResource::getSize() {
	return 1;
}

inline void TestProtobufResource::setStr(const std::string &str) {
	r.set_str(str);
}

inline const std::string &TestProtobufResource::getStr() {
	return r.str();
}

#endif
