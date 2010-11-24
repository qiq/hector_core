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
	// save and restore resource
	std::string *Serialize();
	bool Deserialize(const char *data, int size);
	int getSerializedSize();
	bool Serialize(google::protobuf::io::ZeroCopyOutputStream *output);
	bool SerializeWithCachedSizes(google::protobuf::io::ZeroCopyOutputStream *output);
	bool Deserialize(google::protobuf::io::ZeroCopyInputStream *input, int size);

	// return ResourceInfo describing one field
	ResourceFieldInfo *getFieldInfo(const char *name);
	// type id of a resource (to be used by Resources::CreateResource(typeid))
	int getTypeId();
	// type string of a resource
	const char *getTypeStr();
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
	return MessageSerialize(&r);
}

inline bool TestProtobufResource::Deserialize(const char *data, int size) {
	bool result = MessageDeserialize(&r, data, size);
	// we keep id
	setStatus(r.status());
	return result;
}

inline int TestProtobufResource::getSerializedSize() {
	r.set_id(getId());
	r.set_status(getStatus());
	return MessageGetSerializedSize(&r);
}

inline bool TestProtobufResource::Serialize(google::protobuf::io::ZeroCopyOutputStream *output) {
	r.set_id(getId());
	r.set_status(getStatus());
	return MessageSerialize(&r, output);
}

inline bool TestProtobufResource::SerializeWithCachedSizes(google::protobuf::io::ZeroCopyOutputStream *output) {
	// r.id and r.status were set in getSerializedSize() already
	return MessageSerializeWithCachedSizes(&r, output);
}

inline bool TestProtobufResource::Deserialize(google::protobuf::io::ZeroCopyInputStream *input, int size) {
	bool result = MessageDeserialize(&r, input, size);
	// we keep id
	setStatus(r.status());
	return result;
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

inline const char *TestProtobufResource::getModuleStr() {
	return "Hector";
}

inline void TestProtobufResource::setStr(const std::string &str) {
	r.set_str(str);
}

inline const std::string &TestProtobufResource::getStr() {
	return r.str();
}

#endif
