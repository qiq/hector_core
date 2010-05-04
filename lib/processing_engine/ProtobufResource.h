/**
 * Class representing a resource, basic item of processing.
 */

#ifndef _PROTOBUF_RESOURCE_H_
#define _PROTOBUF_RESOURCE_H_

#include <config.h>

#include <string>
#include <log4cxx/logger.h>
#include <google/protobuf/message.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include "Resource.h"
#include "Object.h"	// for LOG_*

using namespace std;

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
	// id should be unique across all resources
	virtual int getId() = 0;
	virtual void setId(int id) = 0;
	// status may be tested in Processor to select target queue
	virtual int getStatus() = 0;
	virtual void setStatus(int status) = 0;
	// save and restore resource
	virtual string *Serialize() = 0;
	virtual bool Deserialize(string *s) = 0;
	virtual int getSerializedSize() = 0;
	virtual bool Serialize(google::protobuf::io::ZeroCopyOutputStream *output) = 0;
	virtual bool Deserialize(google::protobuf::io::ZeroCopyInputStream *input, int size) = 0;
	// used by queues in case there is limit on queue size, this size may
	// be somewhat arbitrary
	virtual int getSize() = 0;
protected:
	static log4cxx::LoggerPtr logger;

	string *MessageSerialize(google::protobuf::Message *msg);
	bool MessageDeserialize(google::protobuf::Message *msg, string *s);
	bool MessageSerialize(google::protobuf::Message *msg, google::protobuf::io::ZeroCopyOutputStream *output);
	int MessageGetSerializedSize(google::protobuf::Message *msg);
	bool MessageDeserialize(google::protobuf::Message *msg, google::protobuf::io::ZeroCopyInputStream *input, int size);
};

inline string *ProtobufResource::MessageSerialize(google::protobuf::Message *msg) {
	string *result = new string();
	if (!msg->SerializeToString(result)) {
		LOG_ERROR(logger, "Cannot serialize Resource");
		delete result;
		return NULL;
	}
	return result;
}

inline bool ProtobufResource::MessageDeserialize(google::protobuf::Message *msg, string *s) {
	return msg->ParseFromString(*s);
}

inline bool ProtobufResource::MessageSerialize(google::protobuf::Message *msg, google::protobuf::io::ZeroCopyOutputStream *output) {
	return msg->SerializeToZeroCopyStream(output);
}

inline int ProtobufResource::MessageGetSerializedSize(google::protobuf::Message *msg) {
	return msg->ByteSize();
}

inline bool ProtobufResource::MessageDeserialize(google::protobuf::Message *msg, google::protobuf::io::ZeroCopyInputStream *input, int size) {
	return msg->ParseFromBoundedZeroCopyStream(input, size);
}

#endif
