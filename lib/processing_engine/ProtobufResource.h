/**
 * Class representing a resource, basic item of processing.
 */

#ifndef _LIB_PROCESSING_ENGINE_PROTOBUF_RESOURCE_H_
#define _LIB_PROCESSING_ENGINE_PROTOBUF_RESOURCE_H_

#include <config.h>

#include <string>
#include <log4cxx/logger.h>
#include <google/protobuf/message.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include "Resource.h"
#include "Object.h"	// for LOG_*

class ProtobufResource : public Resource {
public:
	ProtobufResource() {};
	virtual ~ProtobufResource() {};
	// create copy of a resource
	virtual ProtobufResource *Clone() = 0;
	// type id of a resource (to be used by Resources::CreateResource(typeid))
	virtual int getTypeId() = 0;
	// type string of a resource
	virtual const char *getTypeStr() = 0;
	// module prefix (e.g. Hector for Hector::TestResource)
	virtual const char *getModuleStr() = 0;

	// save and restore resource
	virtual std::string *Serialize() = 0;
	virtual bool Deserialize(const char *data, int size) = 0;
	virtual int getSerializedSize() = 0;
	virtual bool Serialize(google::protobuf::io::ZeroCopyOutputStream *output) = 0;
	virtual bool SerializeWithCachedSizes(google::protobuf::io::ZeroCopyOutputStream *output) = 0;
	virtual bool Deserialize(google::protobuf::io::ZeroCopyInputStream *input, int size) = 0;
	// used by queues in case there is limit on queue size, this size may
	// be somewhat arbitrary
	virtual int getSize() = 0;
	// return string representation of the resource (e.g. for debugging purposes)
	virtual std::string *toString(Object::LogLevel logLevel) = 0;
protected:
	static log4cxx::LoggerPtr logger;

	std::string *MessageSerialize(google::protobuf::Message *msg);
	bool MessageDeserialize(google::protobuf::Message *msg, const char *data, int size);
	bool MessageSerialize(google::protobuf::Message *msg, google::protobuf::io::ZeroCopyOutputStream *output);
	int MessageGetSerializedSize(google::protobuf::Message *msg);
	bool MessageSerializeWithCachedSizes(google::protobuf::Message *msg, google::protobuf::io::ZeroCopyOutputStream *output);
	bool MessageDeserialize(google::protobuf::Message *msg, google::protobuf::io::ZeroCopyInputStream *input, int size);
};

inline std::string *ProtobufResource::MessageSerialize(google::protobuf::Message *msg) {
	std::string *result = new std::string();
	if (!msg->SerializeToString(result)) {
		LOG4CXX_ERROR(logger, "Cannot serialize Resource");
		delete result;
		return NULL;
	}
	return result;
}

inline bool ProtobufResource::MessageDeserialize(google::protobuf::Message *msg, const char *data, int size) {
	return msg->ParseFromArray((void*)data, size);
}

inline bool ProtobufResource::MessageSerialize(google::protobuf::Message *msg, google::protobuf::io::ZeroCopyOutputStream *output) {
	return msg->SerializeToZeroCopyStream(output);
}

inline bool ProtobufResource::MessageSerializeWithCachedSizes(google::protobuf::Message *msg, google::protobuf::io::ZeroCopyOutputStream *output) {
	google::protobuf::io::CodedOutputStream encoder(output);
	msg->SerializeWithCachedSizes(&encoder);
	return true;
}

inline int ProtobufResource::MessageGetSerializedSize(google::protobuf::Message *msg) {
	return msg->ByteSize();
}

inline bool ProtobufResource::MessageDeserialize(google::protobuf::Message *msg, google::protobuf::io::ZeroCopyInputStream *input, int size) {
	return msg->ParseFromBoundedZeroCopyStream(input, size);
}

#endif
