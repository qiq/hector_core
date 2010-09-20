/**
 * Class representing queue of resources (mainly HTML pages) while processing.
 * It uses Google Protocol Buffers to de/serialize.
 */

#ifndef _TEST_PROTOBUF_RESOURCE_H_
#define _TEST_PROTOBUF_RESOURCE_H_

#include <config.h>

#include <string>
#include <log4cxx/logger.h>
#include "ProtobufResource.h"
#include "TestProtobufResource.pb.h"

class TestProtobufResource : public ProtobufResource {
public:
	TestProtobufResource();
	~TestProtobufResource() {};
	// create copy of a resource
	ProtobufResource *Clone();
	// type id of a resource (to be used by Resources::CreateResource(typeid))
	int getTypeId();
	// type string of a resource
	const char *getTypeStr();
	// module prefix (e.g. Hector for Hector::TestResource)
	const char *getModuleStr();
	// id should be unique across all resources
	int getId();
	void setId(int id);
	// status may be tested in Processor to select target queue
	int getStatus();
	void setStatus(int status);
	// save and restore resource
	std::string *Serialize();
	bool Deserialize(std::string *s);
	int getSerializedSize();
	bool Serialize(google::protobuf::io::ZeroCopyOutputStream *output);
	bool Deserialize(google::protobuf::io::ZeroCopyInputStream *input, int size);
	// used by queues in case there is limit on queue size
	int getSize();
	// return string representation of the resource (e.g. for debugging purposes)
	char *toString();

	void setStr(const char *s);
	const char *getStr();

	static const int typeId = 2;

protected:
	hector::resources::TestProtobufResource r;

	static log4cxx::LoggerPtr logger;
};

inline int TestProtobufResource::getTypeId() {
	return typeId;
}

inline const char *TestProtobufResource::getTypeStr() {
	return "TestProtobufResource";
}

inline const char *TestProtobufResource::getModuleStr() {
	return "Hector";
}

inline int TestProtobufResource::getId() {
	return r.id();
}

inline void TestProtobufResource::setId(int id) {
	r.set_id(id);
}

inline int TestProtobufResource::getStatus() {
	return r.status();
}

inline void TestProtobufResource::setStatus(int status) {
	r.set_status(status);
}

inline std::string *TestProtobufResource::Serialize() {
	return MessageSerialize(&r);
}

inline bool TestProtobufResource::Deserialize(std::string *s) {
	return MessageDeserialize(&r, s);
}

inline int TestProtobufResource::getSerializedSize() {
	return MessageGetSerializedSize(&r);
}

inline bool TestProtobufResource::Serialize(google::protobuf::io::ZeroCopyOutputStream *output) {
	return MessageGetSerializedSize(&r);
}

inline bool TestProtobufResource::Deserialize(google::protobuf::io::ZeroCopyInputStream *input, int size) {
	return MessageDeserialize(&r, input, size);
}

inline void TestProtobufResource::setStr(const char *str) {
	r.set_str(str);
}

inline const char *TestProtobufResource::getStr() {
	return r.str().c_str();
}

#endif
